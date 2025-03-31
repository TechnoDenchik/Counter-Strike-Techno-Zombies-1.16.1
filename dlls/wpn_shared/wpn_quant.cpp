/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "wpn_quant.h"
#include "weapons/KnifeAttack.h"
#include "util/u_range.hpp"

#ifndef CLIENT_DLL
#include "effects.h"
#include "customentity.h"
#include "monsters.h"
#include "gamemode/mods.h"
#endif

enum quantum_e
{
	QUANT_IDLE1,
	QUANT_RELOAD,
	QUANT_DRAW,

	QUANT_SHOOT1,
	QUANT_SHOOT2,
	QUANT_SHOOT3,

	QUANT_REV_IDLE,
	QUANT_REV_RELOAD,

	QUANT_REV_DRAW,
	QUANT_REV_SHOOT1,
	QUANT_REV_SHOOT2,
	QUANT_REV_SHOOT3,

	QUANT_REV
};

LINK_ENTITY_TO_CLASS(weapon_quantum, CQuantum)

void CQuantum::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_quantum");

	Precache();
	m_iId = WEAPON_AK47;
	SET_MODEL(ENT(pev), "models/w_revivegun.mdl");

	m_iDefaultAmmo = QUANT_DEFAULT_GIVE;
	m_flAccuracy = 0.2;
	m_iShotsFired = 0;
	QuantAmmo = 0;
	phs2 = -1;
	phs3 = -1;
	phs4 = -1;
	phs12 = -1;

	FallInit();
}

void CQuantum::Precache(void)
{
	PRECACHE_MODEL("models/v_revivegun.mdl");

	PRECACHE_MODEL("models/w_revivegun.mdl");

	PRECACHE_SOUND("weapons/revivegun_clipin1.wav");
	PRECACHE_SOUND("weapons/revivegun_clipoutA_1.wav");
	PRECACHE_SOUND("weapons/revivegun_clipoutB_1.wav");
	PRECACHE_SOUND("weapons/revivegun_draw.wav");
	PRECACHE_SOUND("weapons/revivegun_idle1.wav");
	PRECACHE_SOUND("weapons/revivegun_idle2.wav");
	PRECACHE_SOUND("weapons/revivegun_revive_in1.wav");
	PRECACHE_SOUND("weapons/revivegun_revive_out1.wav");
	PRECACHE_SOUND("weapons/revivegun_shootB.wav");
	PRECACHE_SOUND("weapons/revivegun_shootB_start.wav");
	PRECACHE_SOUND("weapons/revivegun-1.wav");
	PRECACHE_SOUND("weapons/revivegun-1_exp.wav");
	PRECACHE_SOUND("weapons/revivegun-2.wav");

	PRECACHE_MODEL("sprites/ef_revivegun_draw1.spr");
	PRECACHE_MODEL("sprites/ef_revivegun_draw1_left.spr");
	PRECACHE_MODEL("sprites/ef_revivegun_draw2.spr");
	PRECACHE_MODEL("sprites/ef_revivegun_draw2_left.spr");
	PRECACHE_MODEL("sprites/ef_revivegun_expA.spr");
	PRECACHE_MODEL("sprites/ef_revivegun_expB.spr");
	PRECACHE_MODEL("sprites/ef_revivegun_idle1.spr");
	PRECACHE_MODEL("sprites/ef_revivegun_idle1_left.spr");
	PRECACHE_MODEL("sprites/ef_revivegun_idle2.spr");
	PRECACHE_MODEL("sprites/ef_revivegun_idle2_left.spr");
	PRECACHE_MODEL(Beam_SPR);

	PRECACHE_MODEL("sprites/muzzleflash223.spr");
	PRECACHE_MODEL("sprites/muzzleflash224.spr");
	PRECACHE_MODEL("sprites/muzzleflash226.spr");

	m_iShell = PRECACHE_MODEL("models/rshell.mdl");
	m_usFire = PRECACHE_EVENT(1, "events/quant.sc");
}

int CQuantum::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "762Nato";
	p->iMaxAmmo1 = MAX_AMMO_762NATO;
	p->pszAmmo2 = "QuantAmmo";
	p->iMaxAmmo2 = 7;
	p->pszAmmo3 = NULL;
	p->iMaxAmmo3 = -1;
	p->pszAmmoGrenade = NULL;
	p->iMaxAmmoGrenade = -1;
	p->iMaxClip = QUANT_MAX_CLIP;
	p->iSlot = 0;
	p->iPosition = 14;
	p->iId = m_iId = WEAPON_AK47;
	p->iFlags = 0;
	p->iWeight = QUANT_WEIGHT;

	return 1;
}

BOOL CQuantum::Deploy(void)
{
	m_flAccuracy = 0.2;
	m_iShotsFired = 0;
	iShellOn = 1;
	phs2 = -1;
	phs3 = -1;
	phs4 = -1;
	phs12 = -1;
	CreateEffect();
	return DefaultDeploy("models/v_revivegun.mdl", "models/p_revivegun.mdl", QUANT_DRAW, "ak47", UseDecrement() != FALSE);
}

void CQuantum::Holster(int skiplocal)
{
	phs2 = -1;
	phs3 = -1;
	phs4 = -1;
	phs12 = -1;
	ClearEffect();
	DestroyEffect();
	// clear target list ?
	return CBasePlayerWeapon::Holster(skiplocal);
}

void CQuantum::CreateEffect()
{
#ifndef CLIENT_DLL
	for (size_t i = 0; i < 3; ++i)
	{
		CBeam* pBeam = CBeam::BeamCreate(Beam_SPR, 300);
		pBeam->SetColor(255, 255, 255);
		pBeam->SetScrollRate(40);
		pBeam->SetBrightness(255);
		pBeam->SetNoise(10);
		pBeam->SetWidth(200);
		pev->effects |= EF_NODRAW;

		phs12_13_14[i] = pBeam;
	}
#endif
}

void CQuantum::SecondaryAttack(void)
{
	if (m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] > 0)
	{
		Getsprite();

		if (phs2 > 0.0f)
			phs2 = -1.0f; // 0xBF800000

		if (phs3 == -1.0f)
		{
			SendWeaponAnim(QUANT_SHOOT2, UseDecrement() != FALSE); // 3
			phs3 = gpGlobals->time + 0.23;

			m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME; // 600

			m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 99999.0;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 99999.0;
		}

#ifndef CLIENT_DLL
		m_pPlayer->SetAnimation(PLAYER_ATTACK1); // 5
#endif

		bool v6 = gpGlobals->time > phs12 + 1.0f;
		int flags;
#ifdef CLIENT_WEAPONS
		flags = FEV_NOTHOST;
#else
		flags = 0;
#endif

		if (v6)
			phs12 = v6 = gpGlobals->time;
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.12f;

		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.9f;
	}
	else
	{
		for (CBasePlayer* player : moe::range::PlayersList())
			CLIENT_COMMAND(player->edict(), "spk weapons/revivegun_clipoutB_1\n");
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
	}
}

void CQuantum::PrimaryAttack_FindTargets()
{
	const float flRadius = 450;

	phs9_10_11.clear();
#ifndef CLIENT_DLL
	CBaseEntity* pEntity = NULL;
	while ((pEntity = UTIL_FindEntityInSphere(pEntity, m_pPlayer->pev->origin, flRadius)) != nullptr)
	{
		if (PrimaryAttack_CheckTargetAvailable(pEntity, m_pPlayer->pev->v_angle))
		{
			EHANDLE eh;
			eh.Set(pEntity->edict());
			phs9_10_11.push_back(eh);
		}
	}
#endif
}

bool CQuantum::PrimaryAttack_CheckTargetAvailable(CBaseEntity* a2, Vector vecAngleDirection)
{
	const float flRadius = 450;

	if (!a2->IsAlive())
		return false;

#ifndef CLIENT_DLL
	if (g_pGameRules->PlayerRelationship(m_pPlayer, a2) == GR_TEAMMATE)
		return false;
#endif

	if (a2->pev == m_pPlayer->pev)
		return false;

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecDelta = (a2->Center() - vecSrc).Normalize();

	UTIL_MakeVectors(vecAngleDirection);

	if (DotProduct(gpGlobals->v_forward, vecDelta) < 0.5)
		return false;

	return true;
}

void CQuantum::PrimaryAttack(void)
{
	if (m_iClip > 0)
	{
		m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME; // 600
		if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
			QuantFire(0.035 + (0.4) * m_flAccuracy, 0.2f, FALSE);
		else if (m_pPlayer->pev->velocity.Length2D() > 140)
			QuantFire(0.035 + (0.07) * m_flAccuracy, 0.2f, FALSE);
		else if (m_pPlayer->pev->fov == 90)
			QuantFire((0.02) * m_flAccuracy, 0.2f, FALSE);
		else
			QuantFire((0.02) * m_flAccuracy, 0.2f, FALSE);
		if (phs2 > 0.0f)
			phs2 = -1.0f; // 0xBF800000
		bool v6 = gpGlobals->time > phs12 + 1.0f;
	}
	else
	{

		for (CBasePlayer* player : moe::range::PlayersList())
			CLIENT_COMMAND(player->edict(), "spk weapons/revivegun_clipoutB_1\n");
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
	}
}

#ifndef CLIENT_DLL
void CQuantum::RadiusDamage(Vector vecAiming, float flDamage)
{
	float flRadius = 85.0f;

	if (g_pModRunning->DamageTrack() == DT_ZBS)
		flRadius = 140.0f;
	if (g_pModRunning->DamageTrack() == DT_ZB)
		flRadius = 125.0f;
	
	const Vector vecSrc = vecAiming;
	entvars_t * const pevAttacker = VARS(pev->owner);
	entvars_t * const pevInflictor = this->pev;
	int bitsDamageType = DMG_BULLET;

	TraceResult tr;
	//const float falloff = flRadius ? flDamage / flRadius : 1;
	const int bInWater = (UTIL_PointContents(vecSrc) == CONTENTS_WATER);

	CBaseEntity *pEntity = NULL;
	while ((pEntity = UTIL_FindEntityInSphere(pEntity, vecSrc, flRadius)) != NULL)
	{
		if (pEntity->pev->takedamage != DAMAGE_NO)
		{
			if (bInWater && !pEntity->pev->waterlevel)
				continue;

			if (!bInWater && pEntity->pev->waterlevel == 3)
				continue;

			if (pEntity->IsBSPModel())
				continue;

			if (pEntity->pev == pevAttacker)
				continue;

			Vector vecSpot = pEntity->BodyTarget(vecSrc);
			UTIL_TraceLine(vecSrc, vecSpot, missile, ENT(pevInflictor), &tr);

			if (tr.flFraction == 1.0f || tr.pHit == pEntity->edict())
			{
				if (tr.fStartSolid)
				{
					tr.vecEndPos = vecSrc;
					tr.flFraction = 0;
				}
				/*float flAdjustedDamage = flDamage - (vecSrc - pEntity->pev->origin).Length() * falloff;
				flAdjustedDamage = Q_max(0, flAdjustedDamage);*/

				if (tr.flFraction == 1.0f)
				{
					pEntity->TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
				}
				else
				{
					tr.iHitgroup = HITGROUP_CHEST;
					ClearMultiDamage();
					pEntity->TraceAttack(pevInflictor, flDamage, (tr.vecEndPos - vecSrc).Normalize(), &tr, bitsDamageType);
					ApplyMultiDamage(pevInflictor, pevAttacker);
				}

				/*CBasePlayer *pVictim = dynamic_cast<CBasePlayer *>(pEntity);
				if (pVictim->m_bIsZombie) // Zombie Knockback...
				{
				ApplyKnockbackData(pVictim, vecSpot - vecSrc, GetKnockBackData());
				}*/
			}
		}
	}

	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
	WRITE_BYTE(TE_EXPLOSION);
	WRITE_COORD(vecAiming[0]);
	WRITE_COORD(vecAiming[1]);
	WRITE_COORD(vecAiming[2]);
	WRITE_SHORT(MODEL_INDEX("sprites/ef_revivegun_expA.spr"));
	WRITE_BYTE(8);
	WRITE_BYTE(40);
	WRITE_BYTE(TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOSOUND);
	MESSAGE_END();

	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
	WRITE_BYTE(TE_EXPLOSION);
	WRITE_COORD(vecAiming[0]);
	WRITE_COORD(vecAiming[1]);
	WRITE_COORD(vecAiming[2]);
	WRITE_SHORT(MODEL_INDEX("sprites/ef_revivegun_expA.spr"));
	WRITE_BYTE(8);
	WRITE_BYTE(40);
	WRITE_BYTE(TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOSOUND);
	MESSAGE_END();


}
#endif	

#ifndef CLIENT_DLL
void CQuantum::RadiusDamage3(Vector vecAiming, float flDamage)
{
	float flRadius = 85.0f;

	if (g_pModRunning->DamageTrack() == DT_ZBS)
		flRadius = 140.0f;
	if (g_pModRunning->DamageTrack() == DT_ZB)
		flRadius = 125.0f;

	const Vector vecSrc = vecAiming;
	entvars_t* const pevAttacker = VARS(pev->owner);
	entvars_t* const pevInflictor = this->pev;
	int bitsDamageType = DMG_BULLET;

	TraceResult tr;
	//const float falloff = flRadius ? flDamage / flRadius : 1;
	const int bInWater = (UTIL_PointContents(vecSrc) == CONTENTS_WATER);

	CBaseEntity* pEntity = NULL;
	while ((pEntity = UTIL_FindEntityInSphere(pEntity, vecSrc, flRadius)) != NULL)
	{
		if (pEntity->pev->takedamage != DAMAGE_NO)
		{
			if (bInWater && !pEntity->pev->waterlevel)
				continue;

			if (!bInWater && pEntity->pev->waterlevel == 3)
				continue;

			if (pEntity->IsBSPModel())
				continue;

			if (pEntity->pev == pevAttacker)
				continue;

			Vector vecSpot = pEntity->BodyTarget(vecSrc);
			UTIL_TraceLine(vecSrc, vecSpot, missile, ENT(pevInflictor), &tr);

			if (tr.flFraction == 1.0f || tr.pHit == pEntity->edict())
			{
				if (tr.fStartSolid)
				{
					tr.vecEndPos = vecSrc;
					tr.flFraction = 0;
				}
				/*float flAdjustedDamage = flDamage - (vecSrc - pEntity->pev->origin).Length() * falloff;
				flAdjustedDamage = Q_max(0, flAdjustedDamage);*/

				if (tr.flFraction == 1.0f)
				{
					pEntity->TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
				}
				else
				{
					tr.iHitgroup = HITGROUP_CHEST;
					ClearMultiDamage();
					pEntity->TraceAttack(pevInflictor, flDamage, (tr.vecEndPos - vecSrc).Normalize(), &tr, bitsDamageType);
					ApplyMultiDamage(pevInflictor, pevAttacker);
				}

				/*CBasePlayer *pVictim = dynamic_cast<CBasePlayer *>(pEntity);
				if (pVictim->m_bIsZombie) // Zombie Knockback...
				{
				ApplyKnockbackData(pVictim, vecSpot - vecSrc, GetKnockBackData());
				}*/
			}
		}
	}

	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
	WRITE_BYTE(TE_EXPLOSION);
	WRITE_COORD(vecAiming[0]);
	WRITE_COORD(vecAiming[1]);
	WRITE_COORD(vecAiming[2]);
	WRITE_SHORT(MODEL_INDEX("sprites/ef_revivegun_expB.spr"));
	WRITE_BYTE(8);
	WRITE_BYTE(40);
	WRITE_BYTE(TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOSOUND);
	MESSAGE_END();

	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
	WRITE_BYTE(TE_EXPLOSION);
	WRITE_COORD(vecAiming[0]);
	WRITE_COORD(vecAiming[1]);
	WRITE_COORD(vecAiming[2]);
	WRITE_SHORT(MODEL_INDEX("sprites/ef_revivegun_expB.spr"));
	WRITE_BYTE(8);
	WRITE_BYTE(40);
	WRITE_BYTE(TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOSOUND);
	MESSAGE_END();


}
#endif	

void CQuantum::ItemPostFrame()
{
	int usableButtons = m_pPlayer->pev->button;
	if ((usableButtons & (IN_ATTACK2)))
	{

		if (m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] > 0)
		{
			//m_fireuse2 = true;
		}
		
	}






	if (m_fireuse2 == true)
	{
		if (m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] > 0)
		{

			if (phs2 > 0.0f)
				phs2 = -1.0f; // 0xBF800000

			if (phs3 == -1.0f)
			{
				phs3 = gpGlobals->time + 0.23;

				m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME; // 600



				m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 99999.0;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 99999.0;
			}

#ifndef CLIENT_DLL
			m_pPlayer->SetAnimation(PLAYER_ATTACK1); // 5
#endif

			bool v6 = gpGlobals->time > phs12 + 1.0f;
			int flags;
#ifdef CLIENT_WEAPONS
			flags = FEV_NOTHOST;
#else
			flags = 0;
#endif

			if (v6)
				phs12 = v6 = gpGlobals->time;
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.12f;

			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.9f;
		}
	}















	
	if (phs4 > 0.0f && gpGlobals->time > phs4)
	{
		phs4 = -1;

		return;
	}

	if (phs2 <= 0.0f)
	{
		if (phs3 > 0.0f)
		{
			if (this->m_pPlayer->pev->button & IN_ATTACK2 && this->m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] > 0)
			{	
				fireon3 == false;
				if (gpGlobals->time > phs3)
				{
					this->SendWeaponAnim(QUANT_SHOOT3, UseDecrement() != FALSE); // 4
					phs3 = gpGlobals->time + 1.0f;

					#ifndef CLIENT_DLL
						RadiusDamage2();
						m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType]--;
					#endif 
					return CBasePlayerWeapon::ItemPostFrame();
				}
				if (m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] != 0)
				{
					m_fireuse2 = true;
				}
			}
			else
			{
				
				phs3 = -1;
				m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.6f;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.5f;
				PLAYBACK_EVENT_FULL(1, m_pPlayer->edict(), m_usFire, 0, (float*)&g_vecZero, (float*)&g_vecZero, 2 /*0*/, 0, 2, 0, FALSE, FALSE);
				phs12 = -1;
			}
		}
	}
	if (m_fireuse2 == true)
	{
		if (gpGlobals->time - tWorldTime3 < 99.0f)
		{
			tDelta3 += gpGlobals->time - tWorldTime3;
		}

		if (tNextAttack3 > 0.3f || (gpGlobals->time - tWorldTime3 > 0.3f) || tDelta3 > 0.3f)
		{
			tNextAttack3 = 0.0f;
			tDelta3 = 0.0f;

			PrimaryAttack_FindTargets();
		}
		tWorldTime3 = gpGlobals->time;

		if (gpGlobals->time - tWorldTime4 < 99.0f)
		{
			tDelta4 += gpGlobals->time - tWorldTime4;
		}

		if (tNextAttack4 > 1.0f || (gpGlobals->time - tWorldTime4 > 1.0f) || tDelta4 > 1.0f)
		{
			tNextAttack4 = 0.0f;
			tDelta4 = 0.0f;
			ClearEffect();
			m_fireuse2 = false;
		}
		tWorldTime4 = gpGlobals->time;
	}
	else
	{
		if (gpGlobals->time - tWorldTime5 < 99.0f)
		{
			tDelta5 += gpGlobals->time - tWorldTime5;
		}

		if (tNextAttack5 > 1.8f || (gpGlobals->time - tWorldTime5 > 1.8f) || tDelta5 > 1.8f)
		{
			tNextAttack5 = 0.0f;
			tDelta5 = 0.0f;
			if (m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] != 7)
			{
				fireon2 = false;
				fireon3 = true;
				m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType]++;
			}
		}
		tWorldTime5 = gpGlobals->time;
	}
	return CBasePlayerWeapon::ItemPostFrame();
}

void CQuantum::RadiusDamage2()
{
	BOOL fDidHit = FALSE;
	UTIL_MakeVectors(m_pPlayer->pev->v_angle);
	Vector vecSrc = m_pPlayer->GetGunPosition();

	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/revivegun-2.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);

	if (phs9_10_11.empty())
	{
		//ClearEffect();
		if (!phs5_6_7.empty())
		{
			// ???
		}
		if (!phs12_13_14.empty())
		{
			// ???
		}
		return;
	}

	size_t v8 = 0;
	for (CBaseEntity* pEntity : phs9_10_11)
	{
		if (v8 >= 6)
			break;
		if (!pEntity)
			continue;
		Vector vecDirection = (pEntity->pev->origin - m_pPlayer->pev->origin).Normalize();

		TraceResult tr;
		UTIL_TraceLine(m_pPlayer->pev->origin, pEntity->pev->origin, missile, ENT(m_pPlayer->pev), &tr);

		ClearMultiDamage();
		pEntity->TraceAttack(m_pPlayer->pev, 700, vecDirection, &tr, DMG_BULLET);
		ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);


		if (v8 < 6)
		{
#ifndef CLIENT_DLL
			CBeam* pBeam = phs12_13_14[v8];
			if (pBeam)
			{
				//pBeam->EntsInit(ENTINDEX(m_pPlayer->edict()), ENTINDEX(pEntity->edict()));
				pBeam->SetType(BEAM_ENTS);
				pBeam->SetStartEntity(ENTINDEX(m_pPlayer->edict()));
				pBeam->SetEndEntity(ENTINDEX(pEntity->edict()));
				pBeam->SetStartAttachment(0);
				pBeam->SetEndAttachment(0);
				pBeam->SetWidth(200);
				pBeam->RelinkBeam();
				pBeam->SetBrightness(255);
				pBeam->pev->effects &= ~EF_NODRAW;

			}


			MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pEntity->pev->origin);
			WRITE_BYTE(TE_EXPLOSION);
			WRITE_COORD(pEntity->pev->origin.x);
			WRITE_COORD(pEntity->pev->origin.y);
			WRITE_COORD(pEntity->pev->origin.z);
			WRITE_SHORT(MODEL_INDEX("sprites/ef_revivegun_expB.spr"));
			WRITE_BYTE(3);
			WRITE_BYTE(40);
			WRITE_BYTE(TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NOSOUND);
			MESSAGE_END();

			MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pEntity->pev->origin);
			WRITE_BYTE(TE_EXPLOSION);
			WRITE_COORD(pEntity->pev->origin.x);
			WRITE_COORD(pEntity->pev->origin.y);
			WRITE_COORD(pEntity->pev->origin.z);
			WRITE_SHORT(MODEL_INDEX("sprites/ef_revivegun_expB.spr"));
			WRITE_BYTE(3);
			WRITE_BYTE(40);
			WRITE_BYTE(TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NOSOUND);
			MESSAGE_END();
#endif
			++v8;
		}
	}
}

void CQuantum::DestroyEffect()
{
#ifndef CLIENT_DLL
	for (CBeam* p : phs5_6_7)
	{
		if (p)
			p->SUB_Remove();
	}
	for (CBeam* p : phs12_13_14)
	{
		if (p)
			p->SUB_Remove();
	}
#endif
}

void CQuantum::ClearEffect()
{
#ifndef CLIENT_DLL
	for (CBeam* pBeam : phs5_6_7)
	{
		if (pBeam)
		{
			pBeam->SetBrightness(0);
			pev->effects |= EF_NODRAW; // 0x80
		}
	}
	for (CBeam* pBeam : phs12_13_14)
	{
		if (pBeam)
		{
			pBeam->SetBrightness(0);
			pev->effects |= EF_NODRAW; // 0x80
		}
	}

#endif
	phs9_10_11.clear();
}

void CQuantum::Getsprite()
{
	const float flRadius = 450;

	phs9_10_11.clear();
#ifndef CLIENT_DLL
	CBaseEntity* pEntity = NULL;
	while ((pEntity = UTIL_FindEntityInSphere(pEntity, m_pPlayer->pev->origin, flRadius)) != nullptr)
	{
		if (PrimaryAttack_CheckTargetAvailable(pEntity, m_pPlayer->pev->v_angle))
		{
			EHANDLE eh;
			eh.Set(pEntity->edict());
			phs9_10_11.push_back(eh);
		}
	}
#endif

	/*if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		QuantFire2(0.01 + (0.3) * m_flAccuracy, 0.9f, TRUE);
	else if (m_pPlayer->pev->velocity.Length2D() > 140)
		QuantFire2(0.01 + (0.3) * m_flAccuracy, 0.9f, TRUE);
	else if (m_pPlayer->pev->fov == 90)
		QuantFire2((0.03) * m_flAccuracy, 0.9f, TRUE);
	else
		QuantFire2((0.03) * m_flAccuracy, 0.9f, TRUE);*/
}

Vector CQuantum::Get_ShootPosition(CBaseEntity *pevAttacker, Vector Start)
{
	CBaseEntity* pEntity = nullptr;
	Vector end, vecforward, GunFire;
	
	end = pevAttacker->pev->v_angle;
	UTIL_MakeVectors(end);
	GunFire[0] = Start[0] + gpGlobals->v_forward[0] * 160.0 + gpGlobals->v_right[0] * 40.0 + gpGlobals->v_up[0] * -40.0;
	GunFire[1] = Start[1] + gpGlobals->v_forward[1] * 160.0 + gpGlobals->v_right[1] * 40.0 + gpGlobals->v_up[1] * -40.0;
	GunFire[2] = Start[2] + gpGlobals->v_forward[2] * 160.0 + gpGlobals->v_right[2] * 40.0 + gpGlobals->v_up[2] * -40.0;
	end = gpGlobals->v_forward;

	end = end * 8192.0;
	end = Start + end;


	TraceResult tr;
	UTIL_TraceLine(Start, end, dont_ignore_monsters, pevAttacker->edict(), &tr);
	end = tr.vecEndPos;

#ifndef CLIENT_DLL
	vecforward = (end - GunFire) / 4.5;
	if ((GunFire - end).Length() > 40.0)
	{

		int iCount = round((end - GunFire).Length() / 4.5);
		int iCount2 = iCount;
		while (iCount)
		{

			Vector vecEffectPos = pev->origin - pev->velocity.Normalize() * 10;

			/*MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
			WRITE_BYTE(TE_EXPLOSION);
			WRITE_COORD(GunFire[0]);
			WRITE_COORD(GunFire[1]);
			WRITE_COORD(GunFire[2]);
			WRITE_SHORT(MODEL_INDEX("sprites/muzzleflash224.spr"));
			WRITE_BYTE(3);
			WRITE_BYTE(30);
			WRITE_BYTE(TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOSOUND | TE_EXPLFLAG_NOPARTICLES);
			MESSAGE_END();*/

			iCount--;
		}
	}

#endif
	return end;
}

Vector CQuantum::Get_ShootPosition2(CBaseEntity* pevAttacker, Vector Start)
{
	Vector end, vecforward, GunFire;

	end = pevAttacker->pev->v_angle;
	UTIL_MakeVectors(end);
	GunFire[0] = Start[0] + gpGlobals->v_forward[0] * 160.0 + gpGlobals->v_right[0] * 40.0 + gpGlobals->v_up[0] * -40.0;
	GunFire[1] = Start[1] + gpGlobals->v_forward[1] * 160.0 + gpGlobals->v_right[1] * 40.0 + gpGlobals->v_up[1] * -40.0;
	GunFire[2] = Start[2] + gpGlobals->v_forward[2] * 160.0 + gpGlobals->v_right[2] * 40.0 + gpGlobals->v_up[2] * -40.0;
	end = gpGlobals->v_forward;

	end = end * 8192.0;
	end = Start + end;

	TraceResult tr;
	UTIL_TraceLine(Start, end, dont_ignore_monsters, pevAttacker->edict(), &tr);
	end = tr.vecEndPos;

#ifndef CLIENT_DLL
	vecforward = (end - GunFire) / 17.5;
	if ((GunFire - end).Length() > 125.0)
	{
		int iCount = round((end - GunFire).Length() / 17.5);
		int iCount2 = iCount;
		while (iCount)
		{
			float fPrecent = float(iCount / iCount2);
			GunFire + vecforward;

			iCount--;
		}
	}
#endif
	return end;
}

void CQuantum::QuantFire(float flSpread, duration_t flCycleTime, BOOL fUseAutoAim)
{
	m_bDelayFire = true;
	m_iShotsFired++;
	m_flAccuracy = ((float)(m_iShotsFired * m_iShotsFired * m_iShotsFired) / 215.0) + 0.3;

	if (m_flAccuracy > 1)
		m_flAccuracy = 1;

	if (m_iClip <= 0)
	{
		if (m_fFireOnEmpty)
		{
			
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2f;
		}

		return;
	}

	SendWeaponAnim(QUANT_SHOOT1, UseDecrement() != FALSE);
	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/revivegun-1.wav", 0.80, ATTN_NORM, 0, PITCH_NORM);

	m_iClip--;
#ifndef CLIENT_DLL
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecDir = m_pPlayer->FireBullets3(vecSrc, gpGlobals->v_forward, flSpread, 8192, 2, BULLET_PLAYER_556MM, GetDamage(), 0.96, m_pPlayer->pev, FALSE, m_pPlayer->random_seed);

	if (gpGlobals->time - tWorldTime < 1.0f)
	{
		tDelta += gpGlobals->time - tWorldTime;
	}

	tNextAttack = 0.0f;
	tDelta = 0.0f;
	CBaseEntity *pevAttacker = this->m_pPlayer;
	auto vecShootPosition = Get_ShootPosition(pevAttacker, vecSrc);
#ifndef CLIENT_DLL
	RadiusDamage(vecShootPosition, g_pModRunning->DamageTrack() == DT_NONE ? 30 : 300);
#endif
		
	tWorldTime = gpGlobals->time;

	int flags;
#ifdef CLIENT_WEAPONS
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usFire, 0, (float*)&g_vecZero, (float*)&g_vecZero, vecDir.x, vecDir.y, (int)(m_pPlayer->pev->punchangle.x * 100), (int)(m_pPlayer->pev->punchangle.y * 100), FALSE, FALSE);


	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;

#ifndef CLIENT_DLL
	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
#endif
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.9f;

	if (m_pPlayer->pev->velocity.Length2D() > 0)
		KickBack(1.0, 0.45, 0.275, 0.05, 4.0, 2.5, 7);
	else if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		KickBack(1.25, 0.45, 0.22, 0.18, 5.5, 4.0, 5);
	else if (FBitSet(m_pPlayer->pev->flags, FL_DUCKING))
		KickBack(0.575, 0.325, 0.2, 0.011, 3.25, 2.0, 8);
	else
		KickBack(0.625, 0.375, 0.25, 0.0125, 3.5, 2.25, 8);
}

void CQuantum::QuantFire2(float flSpread, duration_t flCycleTime, BOOL fUseAutoAim)
{
	m_bDelayFire = true;
	m_iShotsFired++;
	m_flAccuracy = ((float)(m_iShotsFired * m_iShotsFired * m_iShotsFired) / 215.0) + 0.3;

	if (m_flAccuracy > 1)
		m_flAccuracy = 1;

	if (m_iClip <= 0)
	{
		if (m_fFireOnEmpty)
		{

		}
		return;
	}

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2f;

	//m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
#ifndef CLIENT_DLL
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecDir = m_pPlayer->FireBullets3(vecSrc, gpGlobals->v_forward, flSpread, 8192, 2, BULLET_PLAYER_556MM, GetDamage(), 0.96, m_pPlayer->pev, FALSE, m_pPlayer->random_seed);

	if (gpGlobals->time - tWorldTime < 1.0f)
	{
		tDelta += gpGlobals->time - tWorldTime;
	}

	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/revivegun-2.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);

	if (tNextAttack > 1.0f || (gpGlobals->time - tWorldTime > 1.0f) || tDelta > 1.0f)
	{
		tNextAttack = 0.0f;
		tDelta = 0.0f;
		CBaseEntity* pevAttacker = this->m_pPlayer;
		auto vecShootPosition = Get_ShootPosition2(pevAttacker, vecSrc);

		#ifndef CLIENT_DLL
			RadiusDamage2();
		#endif
	}

	tWorldTime = gpGlobals->time;

	int flags;
#ifdef CLIENT_WEAPONS
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usFire, 0, (float*)&g_vecZero, (float*)&g_vecZero, vecDir.x, vecDir.y, (int)(m_pPlayer->pev->punchangle.x * 100), (int)(m_pPlayer->pev->punchangle.y * 100), FALSE, FALSE);

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;

#ifndef CLIENT_DLL
	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
#endif
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.9f;

	if (m_pPlayer->pev->velocity.Length2D() > 0)
		KickBack(1.0, 0.45, 0.275, 0.05, 4.0, 2.5, 7);
	else if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		KickBack(1.25, 0.45, 0.22, 0.18, 5.5, 4.0, 5);
	else if (FBitSet(m_pPlayer->pev->flags, FL_DUCKING))
		KickBack(0.575, 0.325, 0.2, 0.011, 3.25, 2.0, 8);
	else
		KickBack(0.625, 0.375, 0.25, 0.0125, 3.5, 2.25, 8);
}

void CQuantum::Reload(void)
{
	if (m_pPlayer->ammo_762nato <= 0)
		return;

	if (DefaultReload(QUANT_MAX_CLIP, QUANT_RELOAD, 2.3f))
	{
		#ifndef CLIENT_DLL
			m_pPlayer->SetAnimation(PLAYER_RELOAD);
		#endif

		if (m_pPlayer->m_iFOV != 90)
			SecondaryAttack();
	
		m_flAccuracy = 0;
		m_iShotsFired = 0;
		m_bDelayFire = false;	

		static int sun = 4.0f;
		static int iLastCountDown = -1.0f;
		int iCountDown = static_cast<int>(gpGlobals->time - sun);

		if (iCountDown > 0.0f)
		{
			if (iCountDown != iLastCountDown)
			{
				if (iCountDown == 1)
				{
					for (CBasePlayer* player : moe::range::PlayersList())
						CLIENT_COMMAND(player->edict(), "spk weapons/revivegun_clipin1\n");
				}
			}
		}
	}
}

void CQuantum::WeaponIdle(void)
{
	fireon = false;
	ResetEmptySound();
	m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;
	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/revivegun_idle1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 4.0;
	SendWeaponAnim(QUANT_IDLE1, UseDecrement() != FALSE);
}

float CQuantum::GetDamage() const
{
	float flDamage = 32.0f;
#ifndef CLIENT_DLL
	if (g_pModRunning->DamageTrack() == DT_ZB)
		flDamage = 500.0f;
	else if (g_pModRunning->DamageTrack() == DT_ZBS)
		flDamage = 1300.0f;
#endif
	return flDamage;
}

int CQuantum::ExtractAmmo(CBasePlayerWeapon* pWeapon)
{
	if (QuantAmmo)
	{
		m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] = QuantAmmo;
		QuantAmmo = 0;
	}
	return CBasePlayerWeapon::ExtractAmmo(pWeapon);
}