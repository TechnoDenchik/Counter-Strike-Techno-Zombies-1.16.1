#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"

#include "gamemode/mods.h"
#include "gamemode/mod_zb1.h"

#include "../wpn_sbmine.h"

LINK_ENTITY_TO_CLASS(weaponn_sbmine_get, CSBMineGet)

#define SB_MINE_EXP_RADIUS	69.37 * 5

CSBMineGet* CSBMineGet::Create(int iType, const Vector& vecOrigin, const Vector& vecAngles, edict_t* pentOwner)
{
	edict_t* pent = CREATE_NAMED_ENTITY(MAKE_STRING("weaponn_sbmine_get"));

	if (FNullEnt(pent))
	{
		ALERT(at_console, "NULL Ent in Create!\n");
		return NULL;
	}

	CSBMineGet* pMine = (CSBMineGet*)Instance(pent);

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

void CSBMineGet::Spawn(void)
{
	Precache();

	SET_MODEL(ENT(pev), "models/w_sbmine.mdl");
	UTIL_SetOrigin(pev, pev->origin);

	pev->angles = g_vecZero;
	pev->gravity = 2.5;
	pev->solid = SOLID_BBOX;
	pev->movetype = MOVETYPE_TOSS;
	pev->frame = 0;

	pev->classname = MAKE_STRING("weaponn_sbmine_get");
	pev->nextthink = gpGlobals->time + 0.01f;
	SetThink(&CSBMineGet::MineThink);
}

void CSBMineGet::Init(CBasePlayer* pOwner, Vector vecVelocity)
{
	m_pOwner = pOwner;
	m_iTeam = m_pOwner->m_iTeam;
	pev->velocity = std::move(vecVelocity);
}

void CSBMineGet::Precache(void)
{
	PRECACHE_MODEL("models/w_sbmine.mdl");

	PRECACHE_MODEL("models/ef_sbmine.mdl");
	PRECACHE_MODEL("models/ef_sbmine_explosion.mdl");
	PRECACHE_MODEL("models/ef_sbmine_lightning.mdl");

	PRECACHE_MODEL("sprites/ef_sbmine_debuff.spr");
	PRECACHE_MODEL("sprites/ef_sbmine_explosion.spr");
	
	PRECACHE_SOUND("weapons/sbmine_exp.wav");
	PRECACHE_SOUND("weapons/sbmine_warning.wav");
	PRECACHE_SOUND("weapons/sbmine_drop.wav");
}

void CSBMineGet::Remove()
{
	SetThink(nullptr);
	pev->effects |= EF_NODRAW; // 0x80u
	return UTIL_Remove(this);
}

void CSBMineGet::GetModel()
{
	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY, pev->origin);
	WRITE_BYTE(TE_MODEL);
	WRITE_COORD(pev->origin.x);
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z);
	WRITE_COORD(pev->velocity.x);
	WRITE_COORD(pev->velocity.y);
	WRITE_COORD(pev->velocity.z);
	WRITE_ANGLE(0);
	WRITE_SHORT(MODEL_INDEX("models/ef_sbmine.mdl"));
	WRITE_BYTE(0);
	WRITE_BYTE(40);
	MESSAGE_END();
}

void CSBMineGet::GetModelExplosion()
{
	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY, pev->origin);
	WRITE_BYTE(TE_MODEL);
	WRITE_COORD(pev->origin.x);
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z);
	WRITE_COORD(pev->velocity.x);
	WRITE_COORD(pev->velocity.y);
	WRITE_COORD(pev->velocity.z);
	WRITE_ANGLE(0);
	WRITE_SHORT(MODEL_INDEX("models/ef_sbmine_explosion.mdl"));
	WRITE_BYTE(0);
	WRITE_BYTE(40);
	MESSAGE_END();
}

void CSBMineGet::Explode(bool IsManual)
{
	if (!pev->owner)
		return;

	if (!m_pOwner->pev)
		return;

	if (!m_pOwner)
		return;

	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
	WRITE_BYTE(TE_EXPLOSION);
	WRITE_COORD(pev->origin.x);
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z);
	WRITE_SHORT(MODEL_INDEX("sprites/ef_sbmine_explosion.spr"));
	WRITE_BYTE(20);
	WRITE_BYTE(20);
	WRITE_BYTE(TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NOSOUND);
	MESSAGE_END();

	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
	WRITE_BYTE(TE_EXPLOSION);
	WRITE_COORD(pev->origin.x);
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z);
	WRITE_SHORT(MODEL_INDEX("sprites/ef_sbmine_explosion.spr"));
	WRITE_BYTE(20);
	WRITE_BYTE(20);
	WRITE_BYTE(TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NOSOUND);
	MESSAGE_END();

	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/sbmine_exp.wav", VOL_NORM, ATTN_NORM);

	m_iLifeModel = 0;
	DoBombExp(IsManual);
	GetModelExplosion();

	Remove();
}

void CSBMineGet::GetWarningDamage(bool IsManual)
{
	CBaseEntity* pEntity = NULL;
	float flRadius = SB_MINE_EXP_RADIUS;
	while ((pEntity = UTIL_FindEntityInSphere(pEntity, pev->origin, flRadius)) != NULL)
	{
		if (pEntity->IsDormant())
			continue;

		if (pEntity->pev->takedamage == DAMAGE_NO)
			continue;

		if (g_pGameRules->PlayerRelationship(m_pOwner, pEntity) == GR_TEAMMATE)
			continue;

		pEntity->TakeDamage(pev, m_pOwner->pev, 300, DMG_NEVERGIB | DMG_EXPLOSION);
		pEntity->TakeSprite(pev, m_pOwner->pev, 18);

		pEntity->pev->speed = 70;

		if (pEntity->IsPlayer())
		{
			CBasePlayer* player = (CBasePlayer*)pEntity;

			if (player->m_bIsZombie)
			{
				auto strategy = dynamic_cast<CPlayerModStrategy_ZB1*>(player->m_pModStrategy.get());
				if (strategy)
				{
				}
			}
		}
	}
}

void CSBMineGet::DoBombExp(bool IsManual)
{
	CBaseEntity* pEntity = NULL;
	float flRadius = SB_MINE_EXP_RADIUS;
	while ((pEntity = UTIL_FindEntityInSphere(pEntity, pev->origin, flRadius)) != NULL)
	{
		if (pEntity->IsDormant())
			continue;

		if (pEntity->pev->takedamage == DAMAGE_NO)
			continue;

		if (g_pGameRules->PlayerRelationship(m_pOwner, pEntity) == GR_TEAMMATE)
			continue;

		pEntity->TakeDamage(pev, m_pOwner->pev, GetDamage(IsManual), DMG_NEVERGIB | DMG_EXPLOSION);

		pEntity->pev->speed = 390;

		if (pEntity->IsPlayer())
		{
			CBasePlayer* player = (CBasePlayer*)pEntity;

			if (player->m_bIsZombie)
			{
				auto strategy = dynamic_cast<CPlayerModStrategy_ZB1*>(player->m_pModStrategy.get());
				if (strategy)
				{
					//strategy->SetStunSpeedTime(3.5f, 55.0);
					//strategy->SetStunGravityTime(3.5f);
				}
			}
		}
	}
}

float CSBMineGet::GetDamage(bool IsManual) const
{
	float flDamage = 1.0f;
	if (m_iType)
	{
		flDamage = IsManual ? 110.0f : 100.0f;
		if (g_pModRunning->DamageTrack() == DT_ZB)
			flDamage = IsManual ? 29200.0f : 19200.0f;
		else if (g_pModRunning->DamageTrack() == DT_ZBS)
			flDamage = IsManual ? 40000.0f : 25000.0f;
	}
	else
	{
		flDamage = IsManual ? 110.0f : 100.0f;
		if (g_pModRunning->DamageTrack() == DT_ZB)
			flDamage = IsManual ? 36900.0f : 45900.0f;
		else if (g_pModRunning->DamageTrack() == DT_ZBS)
			flDamage = IsManual ? 439400.0f : 324400.0f;
	}
	return flDamage;
}

void EXPORT CSBMineGet::MineThink(void)
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

	if (pev->flags & FL_ONGROUND)
	{
		if (m_iState == 2)
		{
			CBaseEntity* pEntity = NULL;
			if (!m_iType)
			{
				while ((pEntity = UTIL_FindEntityInSphere(pEntity, pev->origin, 69.37 * 1.5)) != NULL)
				{
					if (pEntity != m_pOwner)
						continue;
					return;
				}
			}

			while ((pEntity = UTIL_FindEntityInSphere(pEntity, pev->origin, 69.37 * 2.8)) != NULL)
			{
				if (pEntity->IsDormant())
					continue;

				if (pEntity->pev->takedamage == DAMAGE_NO)
					continue;

				if (pEntity == m_pOwner)
					continue;

				if (g_pGameRules->PlayerRelationship(m_pOwner, pEntity) == GR_TEAMMATE)
					continue;

				istarget = true;
			}
		}

		if (m_iState == 3)
		{
			istarget = false;
			Explode(false);
		}

		if (m_flNextAnim != 0 && m_flNextAnim < gpGlobals->time)
		{
			pev->sequence = 1;

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
			pev->sequence = 0;
			pev->frame = 0;
			ResetSequenceInfo();

			MESSAGE_BEGIN(MSG_ALL, gmsgMPToCL, NULL, this->pev);
			WRITE_BYTE(18);
			WRITE_BYTE(m_iType ? 3 : 1);
			WRITE_SHORT(this->entindex());
			MESSAGE_END();

			EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/sbmine_drop.wav", VOL_NORM, ATTN_NORM);

			m_flNextAnim = gpGlobals->time + 0.5f;

			m_iState = 1;
		}
		if (istarget == true)
		{
			if (gpGlobals->time - tWorldTime7 < 1.0f)
			{
				tDelta7 += gpGlobals->time - tWorldTime7;
			}

			if (tNextAttack7 > 4.0f || (gpGlobals->time - tWorldTime7 > 4.0f) || tDelta7 > 4.0f)	//可以多射一次
			{
				tNextAttack7 = 0.0f;
				tDelta7 = 0.0f;

				EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/sbmine_warning.wav", 1.5, ATTN_NORM);
				GetModel();
			}
			tWorldTime7 = gpGlobals->time;

			if (gpGlobals->time - tWorldTime8 < 1.0f)
			{
				tDelta8 += gpGlobals->time - tWorldTime8;
			}

			if (tNextAttack8 > 1.0f || (gpGlobals->time - tWorldTime8 > 1.0f) || tDelta8 > 1.0f)	//可以多射一次
			{
				tNextAttack8 = 0.0f;
				tDelta8 = 0.0f;

				GetWarningDamage(false);

				if (isexploderenine < 4)
				{
					isexploderenine++;
				}

				if (isexploderenine == 4)
				{
					m_iState = 3;
				}
				
			}
			tWorldTime8 = gpGlobals->time;
		}
	}
}