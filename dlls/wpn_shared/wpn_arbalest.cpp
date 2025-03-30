/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "wpn_arbalest.h"
#include "util/u_range.hpp"
#include "weapons/WeaponTemplate.hpp"
#include "gamemode/interface/interface_const.h"
#ifndef CLIENT_DLL
#include "effects.h"
#include "customentity.h"
#include "monsters.h"
#endif

#ifndef CLIENT_DLL
#include "gamemode/mods.h"
#endif

enum arbalest
{
	ARBALEST_IDLE,
	ARBALEST_RELOAD,
	ARBALEST_DRAW,
	ARBALEST_SHOOT_A,
	ARBALEST_SHOOT_B_START,
	ARBALEST_SHOOT_B_LOOP,
	ARBALEST_SHOOT_B_END,
	ARBALEST_SHOOT_B_CHARGING_LOOP,
	ARBALEST_SHOOT_B_CHARGING_END,
	ARBALEST_SHOOT_C,
	ARBALEST_SHOOT_B_CHARGE,
};

LINK_ENTITY_TO_CLASS(weapon_arbalest, CArbalest)

void CArbalest::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_arbalest");

	Precache();
	m_iId = WEAPON_AUG;
	SET_MODEL(ENT(pev), "models/w_halogun.mdl");

	m_iDefaultAmmo = ARBALEST_DEFAULT_GIVE;
	m_flAccuracy = 0.0;
	m_iShotsFired = 0;
	m_iClip_c = 0;
	phs2 = -1;
	phs3 = -1;
	phs4 = -1;
	phs12 = -1;
	FallInit();
}

void CArbalest::Precache(void)
{
	PRECACHE_MODEL("models/v_halogun.mdl");
	PRECACHE_MODEL("models/w_halogun.mdl");
	PRECACHE_MODEL("models/p_halogun.mdl");
	PRECACHE_MODEL("models/d_halogun.mdl");
	PRECACHE_MODEL("models/ef_halogun_chargingshot.mdl");
	PRECACHE_MODEL("models/ef_halogun_chargingshot2.mdl");
	PRECACHE_MODEL("models/ef_halogun_projectile.mdl");

	PRECACHE_MODEL("sprites/ef_halogun_expA.spr");
	PRECACHE_MODEL("sprites/ef_halogun_projectile.spr");
	PRECACHE_MODEL("sprites/ef_halogun_shootA_hit.spr");
	PRECACHE_MODEL("sprites/ef_halogun_shootB_hit.spr");
	PRECACHE_MODEL("sprites/muzzleflash281.spr");
	
	PRECACHE_SOUND("weapons/halogun_draw.wav");
	PRECACHE_SOUND("weapons/halogun_fx.wav");
	PRECACHE_SOUND("weapons/halogun_idle.wav");
	PRECACHE_SOUND("weapons/halogun_reload.wav");
	PRECACHE_SOUND("weapons/halogun_shootB_charging_end.wav");
	PRECACHE_SOUND("weapons/halogun_shootB_end.wav");
	PRECACHE_SOUND("weapons/halogun_shootB_start.wav");
	PRECACHE_SOUND("weapons/halogun-1.wav");
	PRECACHE_SOUND("weapons/halogun-1_exp.wav");
	PRECACHE_SOUND("weapons/halogun-2.wav");
	PRECACHE_SOUND("weapons/halogun-2_end.wav");
	PRECACHE_SOUND("weapons/halogun-2_exp1.wav");
	PRECACHE_SOUND("weapons/halogun-2_exp2.wav");
	PRECACHE_SOUND("weapons/halogun-3.wav");

	m_iShell = PRECACHE_MODEL("models/rshell.mdl");
	m_usFireArbalestar = PRECACHE_EVENT(1, "events/starchaserar.sc");
}

int CArbalest::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "556Nato";
	p->iMaxAmmo1 = 100;
	p->pszAmmo2 = "arbalestammo";
	p->iMaxAmmo2 = 50;
	p->pszAmmo3 = NULL;
	p->iMaxAmmo3 = -1;
	p->pszAmmoGrenade = NULL;
	p->iMaxAmmoGrenade = -1;
	p->iMaxClip = ARBALEST_MAX_CLIP;
	p->iSlot = 0;
	p->iPosition = 14;
	p->iId = m_iId = WEAPON_AUG;
	p->iFlags = 0;
	p->iWeight = AUG_WEIGHT;

	return 1;
}

void CArbalest::UpdateHUD() const
{
#ifndef CLIENT_DLL
	MESSAGE_BEGIN(MSG_ONE, gmsgArbalestMsg, NULL, m_pPlayer->pev);
	WRITE_BYTE(WPN_ARBALEST);
	WRITE_BYTE(m_iClip_c);
	MESSAGE_END();
#endif
}

BOOL CArbalest::Deploy(void)
{
	m_flAccuracy = 0.2;
	m_iShotsFired = 0;
	iShellOn = 1;
	phs2 = -1;
	phs3 = -1;
	phs4 = -1;
	phs12 = -1; // 0xBF800000
	return DefaultDeploy("models/v_halogun.mdl", "models/p_halogun.mdl", ARBALEST_DRAW, "ak47", UseDecrement() != FALSE);
	UpdateHUD();
}

void CArbalest::Holster(int skiplocal)
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

void CArbalest::UndoHUD() const
{
#ifndef CLIENT_DLL
	
#endif
}

void CArbalest::Drop()
{

}



void CArbalest::PrimaryAttack_FindTargets()
{
	const float flRadius = 500;

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

bool CArbalest::PrimaryAttack_CheckTargetAvailable(CBaseEntity* a2, Vector vecAngleDirection)
{
	const float flRadius = 500;

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

void CArbalest::SecondaryAttack(void)
{
	if (m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] != 0)
	{
		
		
	Getsprite();
	++m_iShotsFired;
	m_bDelayFire = 1;
	if (m_iClip <= 0)
	{
		if (m_fFireOnEmpty)
		{
			PlayEmptySound();
			m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.2;
		}
		return; // sth ignored
	}
	if (phs2 > 0.0f)
		phs2 = -1.0f; // 0xBF800000
	
		if (phs3 == -1.0f)
		{
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/halogun-2.wav", 0, 0, 0, PITCH_NORM);
			SendWeaponAnim(ARBALEST_SHOOT_B_START, UseDecrement() != FALSE); // 3
			phs3 = gpGlobals->time + 0.23;
			m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 99999.0;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 99999.0;
		}
	
	
#ifndef CLIENT_DLL
	m_pPlayer->SetAnimation(PLAYER_ATTACK1); // 5
#endif
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME; // 600
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH; // 512
	m_pPlayer->pev->effects |= EF_MUZZLEFLASH; // 2u
	
	bool v6 = gpGlobals->time > phs12 + 1.0f;
	int flags;
#ifdef CLIENT_WEAPONS
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif
	PLAYBACK_EVENT_FULL(1, m_pPlayer->edict(), m_usFireArbalestar, 0, (float*)&g_vecZero, (float*)&g_vecZero, v6 /*0*/, 0, v6, 0, FALSE, FALSE);

	if (v6)
		phs12 = v6 = gpGlobals->time;
	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.12f;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.9f;
	}
	PrimaryAttack_FindTargets();
}

void CArbalest::PrimaryAttack(void)
{
	if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		ArbalestFire(0, 0.2f - 0.04f, FALSE);
	else if (m_pPlayer->pev->velocity.Length2D() > 140)
		ArbalestFire(0, 0.2f - 0.04f, FALSE);
	else
		ArbalestFire(0, 0.2f - 0.04f, FALSE);

	if (phs2 > 0.0f)
		phs2 = -1.0f; // 0xBF800000
	bool v6 = gpGlobals->time > phs12 + 1.0f;
}

#ifndef CLIENT_DLL
void CArbalest::RadiusDamage3(Vector vecAiming, float flDamage)
{
	float flRadius = 500.0f;

	if (g_pModRunning->DamageTrack() == DT_ZBS)
		flRadius = 500.0f;
	if (g_pModRunning->DamageTrack() == DT_ZB)
		flRadius = 600.0f;
	
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
			}
		}
	}

	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
	WRITE_BYTE(TE_EXPLOSION);
	WRITE_COORD(vecAiming[0]);
	WRITE_COORD(vecAiming[1]);
	WRITE_COORD(vecAiming[2]);
	WRITE_SHORT(MODEL_INDEX("sprites/ef_halogun_shootA_hit.spr"));
	WRITE_BYTE(2);
	WRITE_BYTE(30);
	WRITE_BYTE(TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOSOUND);
	MESSAGE_END();

	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
	WRITE_BYTE(TE_EXPLOSION);
	WRITE_COORD(vecAiming[0]);
	WRITE_COORD(vecAiming[1]);
	WRITE_COORD(vecAiming[2]);
	WRITE_SHORT(MODEL_INDEX("sprites/ef_halogun_shootA_hit.spr"));
	WRITE_BYTE(2);
	WRITE_BYTE(30);
	WRITE_BYTE(TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOSOUND);
	MESSAGE_END();

	

}

void CArbalest::RadiusDamage(Vector vecAiming, float flDamage)
{
	float flRadius = 500.0f;

	if (g_pModRunning->DamageTrack() == DT_ZBS)
		flRadius = 500.0f;
	if (g_pModRunning->DamageTrack() == DT_ZB)
		flRadius = 600.0f;

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
	WRITE_SHORT(MODEL_INDEX("sprites/ef_halogun_expA.spr"));
	WRITE_BYTE(5);
	WRITE_BYTE(30);
	WRITE_BYTE(TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOSOUND);
	MESSAGE_END();

	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
	WRITE_BYTE(TE_EXPLOSION);
	WRITE_COORD(vecAiming[0]);
	WRITE_COORD(vecAiming[1]);
	WRITE_COORD(vecAiming[2]);
	WRITE_SHORT(MODEL_INDEX("sprites/ef_halogun_expA.spr"));
	WRITE_BYTE(5);
	WRITE_BYTE(30);
	WRITE_BYTE(TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOSOUND);
	MESSAGE_END();

	EMIT_SOUND_DYN(this->edict(), CHAN_WEAPON, "weapons/halogun-1_exp.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);

}

void CArbalest::RadiusDamage2()
{
	BOOL fDidHit = FALSE;
	UTIL_MakeVectors(m_pPlayer->pev->v_angle);
	Vector vecSrc = m_pPlayer->GetGunPosition();


	if (phs9_10_11.empty())
	{
		ClearEffect();
		if (!phs5_6_7.empty())
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

			MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pEntity->pev->origin);
			WRITE_BYTE(TE_EXPLOSION);
			WRITE_COORD(pEntity->pev->origin.x);
			WRITE_COORD(pEntity->pev->origin.y);
			WRITE_COORD(pEntity->pev->origin.z);
			WRITE_SHORT(MODEL_INDEX("sprites/ef_halogun_shootB_hit.spr"));
			WRITE_BYTE(6);
			WRITE_BYTE(40);
			WRITE_BYTE(TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NOSOUND);
			MESSAGE_END();

			MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pEntity->pev->origin);
			WRITE_BYTE(TE_EXPLOSION);
			WRITE_COORD(pEntity->pev->origin.x);
			WRITE_COORD(pEntity->pev->origin.y);
			WRITE_COORD(pEntity->pev->origin.z);
			WRITE_SHORT(MODEL_INDEX("sprites/ef_halogun_shootB_hit.spr"));
			WRITE_BYTE(6);
			WRITE_BYTE(40);
			WRITE_BYTE(TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NOSOUND);
			MESSAGE_END();
#endif
			++v8;
		}
	}

}
#endif	

void CArbalest::DestroyEffect()
{
#ifndef CLIENT_DLL
	for (CBeam* p : phs5_6_7)
	{
		if (p)
			p->SUB_Remove();
	}
#endif
}

void CArbalest::ClearEffect()
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
#endif
	phs9_10_11.clear();
}

void CArbalest::Getsprite()
{
	const float flRadius = 500;

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

Vector CArbalest::Get_ShootPosition(CBaseEntity *pevAttacker, Vector Start)
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
			/*MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
			WRITE_BYTE(TE_EXPLOSION);
			WRITE_COORD(GunFire[0]);
			WRITE_COORD(GunFire[1]);
			WRITE_COORD(GunFire[2]);
			WRITE_SHORT(MODEL_INDEX("sprites/muzzleflash281.spr"));
			WRITE_BYTE(5);
			WRITE_BYTE(15 + round(15.0 * fPrecent));
			WRITE_BYTE(TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOSOUND | TE_EXPLFLAG_NOPARTICLES);
			MESSAGE_END();*/
			iCount--;
		}
	}
#endif
	return end;
}

Vector CArbalest::Get_ShootPosition2(CBaseEntity* pevAttacker, Vector Start)
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
			/*MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
			WRITE_BYTE(TE_EXPLOSION);
			WRITE_COORD(GunFire[0]);
			WRITE_COORD(GunFire[1]);
			WRITE_COORD(GunFire[2]);
			WRITE_SHORT(MODEL_INDEX("sprites/muzzleflash281.spr"));
			WRITE_BYTE(5);
			WRITE_BYTE(15 + round(15.0 * fPrecent));
			WRITE_BYTE(TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOSOUND | TE_EXPLFLAG_NOPARTICLES);
			MESSAGE_END();*/
			iCount--;
		}
	}
#endif
	return end;
}

void CArbalest::ArbalestFire(float flSpread, duration_t flCycleTime, BOOL fUseAutoAim)
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
			PlayEmptySound();
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2f;
		}

		return;
	}

	m_iClip--;
	
	if (CanDeploy())
	{
		UpdateHUD();
	}
	else
	{
		UndoHUD();
	}
	m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
#ifndef CLIENT_DLL
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecDir = m_pPlayer->FireBullets3(vecSrc, gpGlobals->v_forward, flSpread, 8192, 2, BULLET_PLAYER_556MM, GetDamage(), 0.96, m_pPlayer->pev, FALSE, m_pPlayer->random_seed);

	if (gpGlobals->time - tWorldTime4 < 1.0f)
	{
		tDelta4 += gpGlobals->time - tWorldTime4;
	}
	SendWeaponAnim(ARBALEST_SHOOT_A, UseDecrement() != FALSE);
	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/halogun-1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);

	CBaseEntity* pevAttacker = this->m_pPlayer;
	auto vecShootPosition = Get_ShootPosition(pevAttacker, vecSrc);
#ifndef CLIENT_DLL
	RadiusDamage3(vecShootPosition, g_pModRunning->DamageTrack() == DT_NONE ? 30 : 300);
#endif

	if (tNextAttack4 > 1.0f || (gpGlobals->time - tWorldTime4 > 1.0f) || tDelta4 > 1.0f)
	{
		tNextAttack4 = 0.0f;
		tDelta4 = 0.0f;
		CBaseEntity *pevAttacker = this->m_pPlayer;
		auto vecShootPosition = Get_ShootPosition(pevAttacker, vecSrc);
#ifndef CLIENT_DLL
		RadiusDamage(vecShootPosition, g_pModRunning->DamageTrack() == DT_NONE ? 30 : 300);
#endif
		
	}

	tWorldTime4 = gpGlobals->time;

	int flags;
#ifdef CLIENT_WEAPONS
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usFireArbalestar, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, (int)(m_pPlayer->pev->punchangle.x * 100), (int)(m_pPlayer->pev->punchangle.y * 100), FALSE, FALSE);
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;

#ifndef CLIENT_DLL
	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
#endif
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.9f;

}

void CArbalest::ArbalestFire2()
{
	m_bDelayFire = true;
	m_iShotsFired++;

	if (m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] <= 0)
	{
		if (m_fFireOnEmpty)
		{
			PlayEmptySound();
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2f;
		}

		return;
	}

	
	

#ifndef CLIENT_DLL
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	Vector vecSrc = m_pPlayer->GetGunPosition();

	CBaseEntity* pevAttacker = this->m_pPlayer;
	auto vecShootPosition = Get_ShootPosition2(pevAttacker, vecSrc);

	tWorldTime4 = gpGlobals->time;

	int flags;
#ifdef CLIENT_WEAPONS
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usFireArbalestar, 0, (float*)&g_vecZero, (float*)&g_vecZero, vecSrc.x, vecSrc.y, (int)(m_pPlayer->pev->punchangle.x * 100), (int)(m_pPlayer->pev->punchangle.y * 100), FALSE, FALSE);
	
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.58;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.9f;
}

void CArbalest::ItemPostFrame()
{
	int usableButtons = m_pPlayer->pev->button;
	
	if (!(usableButtons & (IN_ATTACK2)))
	{
		m_fireuse2 = false;
	}
	else
	{
		m_fireuse2 = true;
	}
	if (gpGlobals->time - tWorldTime3 < 99.0f)
	{
		tDelta3 += gpGlobals->time - tWorldTime3;
	}
	if (tNextAttack3 > 0.3f || (gpGlobals->time - tWorldTime3 > 0.3f) || tDelta3 > 0.3f)	//可以多射一次
	{
		tNextAttack3 = 0.0f;
		tDelta3 = 0.0f;
		if (m_fireuse2 == false)
		{
			if (m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] < 50)
			{
				m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType]++;
				//UpdateHUD();
			}
			if (m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] > 40)
			{
				pev->skin = 1;
			}
		}
	}
	tWorldTime3 = gpGlobals->time;
	
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
				if (gpGlobals->time > phs3)
				{
					this->SendWeaponAnim(ARBALEST_SHOOT_B_LOOP, UseDecrement() != FALSE); // 4
					phs3 = gpGlobals->time + 9.0f;
					
					EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/halogun-2.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
				
					return CBasePlayerWeapon::ItemPostFrame();
				}
				if (m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] != 0)
				{

					if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
						ArbalestFire2();
					else if (m_pPlayer->pev->velocity.Length2D() > 140)
						ArbalestFire2();
					else
						ArbalestFire2();


					if (gpGlobals->time - tWorldTime5 < 1.0f)
					{
						tDelta5 += gpGlobals->time - tWorldTime5;
					}
					if (tNextAttack5 > 0.5f || (gpGlobals->time - tWorldTime5 > 0.5f) || tDelta5 > 0.5f)	//可以多射一次
					{
						tNextAttack5 = 0.0f;
						tDelta5 = 0.0f;

#ifndef CLIENT_DLL
						RadiusDamage2();
#endif
						m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType]--;
					}
					tWorldTime5 = gpGlobals->time;
				}

			}
			else
			{
				EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/halogun-2.wav", 0, 0, 0, PITCH_NORM);
				this->SendWeaponAnim(ARBALEST_SHOOT_B_END, UseDecrement() != FALSE); // 5
				phs3 = -1;
				m_flNextPrimaryAttack = m_flNextSecondaryAttack = m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.4f;
				
				PLAYBACK_EVENT_FULL(1, m_pPlayer->edict(), m_usFireArbalestar, 0, (float*)&g_vecZero, (float*)&g_vecZero, 2 /*0*/, 0, 2, 0, FALSE, FALSE);
				phs12 = -1;
			}
		}
	}

	
	return CBasePlayerWeapon::ItemPostFrame();
}

void CArbalest::Reload(void)
{
	if (m_pPlayer->ammo_556nato <= 0)
		return;

	if (DefaultReload(ARBALEST_MAX_CLIP, ARBALEST_RELOAD, 2.3f))
	{
#ifndef CLIENT_DLL
		m_pPlayer->SetAnimation(PLAYER_RELOAD);
#endif

		m_flAccuracy = 0;
		m_iShotsFired = 0;
		m_bDelayFire = false;
	}
}

void CArbalest::WeaponIdle(void)
{
	ResetEmptySound();
	m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);
	
	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/halogun_idle.wav", VOL_NORM, ATTN_NORM);
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 6.0;
	SendWeaponAnim(ARBALEST_IDLE, UseDecrement() != FALSE);
	
}

float CArbalest::GetDamage() const
{
	float flDamage = 32.0f;
#ifndef CLIENT_DLL
	if (g_pModRunning->DamageTrack() == DT_ZB)
		flDamage = 90.0f;
	else if (g_pModRunning->DamageTrack() == DT_ZBS)
		flDamage = 140.0f;
#endif
	return flDamage;
}

int CArbalest::ExtractAmmo(CBasePlayerWeapon* pWeapon)
{
	if (m_iClip_c)
	{
		m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] = m_iClip_c;
		m_iClip_c = 0;
	}
	return CBasePlayerWeapon::ExtractAmmo(pWeapon);
}