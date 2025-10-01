#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"

#include "gamemode/mod_zb1.h"

#include "knife_twin_axes.h"

LINK_ENTITY_TO_CLASS(twinaxes_summon, CTwinAxesSummon)

#define WONDERCANNON_MINE_EXP_RADIUS	39.37 * 5

CTwinAxesSummon* CTwinAxesSummon::Create(int iType, const Vector& vecOrigin, const Vector& vecAngles, edict_t* pentOwner)
{
	edict_t* pent = CREATE_NAMED_ENTITY(MAKE_STRING("twinaxes_summon"));

	if (FNullEnt(pent))
	{
		ALERT(at_console, "NULL Ent in Create!\n");
		return NULL;
	}

	CTwinAxesSummon* pMine = (CTwinAxesSummon*)Instance(pent);

	if (pMine)
	{
		pMine->m_iType = iType;
		pMine->pev->owner = pentOwner;
		pMine->pev->origin = vecOrigin;
		pMine->pev->angles = vecAngles;
		pMine->Spawn();
	}

	return pMine;
}

void CTwinAxesSummon::Spawn(void)
{
	Precache();

	SET_MODEL(ENT(pev), "models/dgaxe_summon.mdl");
	UTIL_SetOrigin(pev, pev->origin);

	pev->angles = g_vecZero;
	pev->gravity = 2.5;
	pev->solid = SOLID_BBOX;
	pev->movetype = MOVETYPE_TOSS;
	pev->frame = 0;
	pev->rendermode = kRenderTransTexture;
	pev->renderamt = 255;
	pev->renderfx = kRenderFxFadeSlow;

	m_iRenderAmt = 255;
	pev->classname = MAKE_STRING("twinaxes_summon");
	pev->nextthink = gpGlobals->time + 0.01f;
	SetThink(&CTwinAxesSummon::MineThink);
}

void CTwinAxesSummon::Init(CBasePlayer* pOwner, Vector vecVelocity)
{
	m_pOwner = pOwner;
	m_iTeam = m_pOwner->m_iTeam;
	pev->velocity = std::move(vecVelocity);
}

void CTwinAxesSummon::Precache(void)
{
	PRECACHE_MODEL("models/dgaxe_summon.mdl");
	PRECACHE_MODEL("models/ef_wondercannon_area.mdl");

	PRECACHE_MODEL("sprites/ef_wondercannon_bomb_set.spr");
	PRECACHE_MODEL("sprites/ef_wondercannon_hit3.spr");

	PRECACHE_SOUND("weapons/dgaxe_skill2_exp.wav");
	PRECACHE_SOUND("weapons/wondercannon_comd_exp.wav");
}

void CTwinAxesSummon::Remove()
{
	MESSAGE_BEGIN(MSG_ALL, SVC_TEMPENTITY, pev->origin);
	WRITE_BYTE(TE_KILLENTITYATTACHMENTS);
	WRITE_SHORT(entindex());
	MESSAGE_END();

	SetThink(nullptr);
	pev->effects |= EF_NODRAW; // 0x80u
	return UTIL_Remove(this);
}

void CTwinAxesSummon::GetSprite()
{
	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY, pev->origin);
	WRITE_BYTE(TE_EXPLOSION);
	WRITE_COORD(pev->origin.x);
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z);
	WRITE_SHORT(MODEL_INDEX("sprites/ef_wondercannon_bomb_set.spr"));
	WRITE_BYTE(2);
	WRITE_BYTE(20);
	WRITE_BYTE(TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NOSOUND);
	MESSAGE_END();

	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY, pev->origin);
	WRITE_BYTE(TE_EXPLOSION);
	WRITE_COORD(pev->origin.x);
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z);
	WRITE_SHORT(MODEL_INDEX("sprites/ef_wondercannon_bomb_set.spr"));
	WRITE_BYTE(2);
	WRITE_BYTE(20);
	WRITE_BYTE(TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NOSOUND);
	MESSAGE_END();
}

void CTwinAxesSummon::DoBombExp(bool IsManual)
{
	
}

float CTwinAxesSummon::GetDamage() const
{
	float flDamage = 1.0f;

	flDamage = 110.0f;
	if (g_pModRunning->DamageTrack() == DT_ZB)
		flDamage = 1900.0f;
	else if (g_pModRunning->DamageTrack() == DT_ZBS)
		flDamage = 39400.0f;
	
	return flDamage;
}

void EXPORT CTwinAxesSummon::MineThink(void)
{
	pev->nextthink = gpGlobals->time + 0.01f;

	if (!m_pOwner)
	{
		Remove();
		return;
	}

	if (m_pOwner->m_bIsZombie || !m_pOwner->IsAlive())
	{
		Remove();
		return;
	}

	CBasePlayerWeapon* pWeapon = (CBasePlayerWeapon*)m_pOwner->m_rgpPlayerItems[KNIFE_SLOT];

	if (!pWeapon)
	{
		Remove();
		return;
	}
	
	CBaseEntity* pEntity = NULL;

	if (pev->flags & FL_ONGROUND)
	{
		if (m_iState == 2)
		{
			if (gpGlobals->time - tWorldTime8 < 1.0f)
			{
				tDelta8 += gpGlobals->time - tWorldTime8;
			}

			if (tNextAttack8 > 0.3f || (gpGlobals->time - tWorldTime8 > 0.3f) || tDelta8 > 0.3f)	//可以多射一次
			{
				tNextAttack8 = 0.0f;
				tDelta8 = 0.0f;

				if (m_iTimeSummon != 28)
				{
					m_iTimeSummon++;
				}

				if (m_iTimeSummon == 28)
				{
					m_iState = 3;
				}

				

				while ((pEntity = UTIL_FindEntityInSphere(pEntity, pev->origin, 79.37 * 1.5)) != NULL)
				{
					if (pEntity == m_pOwner)
						continue;

					if (pEntity->IsDormant())
						continue;

					if (pEntity->pev->takedamage == DAMAGE_NO)
						continue;

					if (g_pGameRules->PlayerRelationship(m_pOwner, pEntity) == GR_TEAMMATE)
						continue;

					pEntity->TakeDamage(pev, m_pOwner->pev, GetDamage(), DMG_BULLET | DMG_CRITICAL);

					return;
				}

			}
			tWorldTime8 = gpGlobals->time;

		}
		else if (m_iState == 3)
		{
			if (gpGlobals->time - tWorldTime7 < 1.0f)
			{
				tDelta7 += gpGlobals->time - tWorldTime7;
			}

			if (tNextAttack7 > 0.5f || (gpGlobals->time - tWorldTime7 > 0.5f) || tDelta7 > 0.5f)	//可以多射一次
			{
				tNextAttack7 = 0.0f;
				tDelta7 = 0.0f;

				pev->sequence = 4;

				if (m_iTimeSummonLife != 10)
					m_iTimeSummonLife++;

				if(m_iTimeSummonLife == 10)
					Remove();
			}
			tWorldTime7 = gpGlobals->time;			
		}

		if (m_flNextAnim != 0 && m_flNextAnim < gpGlobals->time)
		{
			pev->sequence = 3;
			//play model & spr

			MESSAGE_BEGIN(MSG_ALL, gmsgMPToCL, NULL, this->pev);
			WRITE_BYTE(18);
			WRITE_BYTE(m_iType ? 2 : 0);
			WRITE_SHORT(this->entindex());
			MESSAGE_END();

			m_iState = 2;
			m_flNextAnim = 0;
		}

		if (!m_iState)
		{
			pev->sequence = 2;
			ResetSequenceInfo();

			MESSAGE_BEGIN(MSG_ALL, gmsgMPToCL, NULL, this->pev);
			WRITE_BYTE(18);
			WRITE_BYTE(m_iType ? 3 : 1);
			WRITE_SHORT(this->entindex());
			MESSAGE_END();

			EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/dgaxe_skill2_exp.wav", VOL_NORM, ATTN_NORM);

			m_flNextAnim = gpGlobals->time + 0.2f;

			m_iState = 1;
		}	

	}
}