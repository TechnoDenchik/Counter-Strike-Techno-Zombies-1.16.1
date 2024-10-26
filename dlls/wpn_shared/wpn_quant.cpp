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

enum starchaserar_e
{
	QUANT_IDLE1,
	QUANT_RELOAD,
	QUANT_DRAW,

	QUANT_SHOOT1,
	QUANT_SHOOT2,
	QUANT_SHOOT3,
	QUANT_SHOOT4,

	QUANT_REV_IDLE,
	QUANT_REV_RELOAD,

	QUANT_REV_DRAW,
	QUANT_REV_SHOOT1,
	QUANT_REV_SHOOT2,
	QUANT_REV_SHOOT3,
	QUANT_REV_SHOOT4,

	QUANT_REV
};

LINK_ENTITY_TO_CLASS(weapon_quantum, CQuantum)

void CQuantum::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_quantum");

	Precache();
	m_iId = WEAPON_QUANTUM;
	SET_MODEL(ENT(pev), "models/p_revivegun.mdl");

	m_iDefaultAmmo = QUANT_DEFAULT_GIVE;
	m_flAccuracy = 0.2;
	m_iShotsFired = 0;

	FallInit();
}

void CQuantum::Precache(void)
{
	PRECACHE_MODEL("models/v_revivegun_a_fix.mdl");
	PRECACHE_MODEL("models/v_revivegun_b_fix.mdl");

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
	PRECACHE_MODEL("sprites/ef_revivegun_laser.spr");

	PRECACHE_MODEL("sprites/muzzleflash223.spr");
	PRECACHE_MODEL("sprites/muzzleflash224.spr");
	PRECACHE_MODEL("sprites/muzzleflash226.spr");

	m_iShell = PRECACHE_MODEL("models/rshell.mdl");
	m_usFireQuant = PRECACHE_EVENT(1, "events/quant.sc");
}

int CQuantum::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "QuantAmmo";
	p->iMaxAmmo1 = MAX_AMMO_762NATO;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = QUANT_MAX_CLIP;
	p->iSlot = 0;
	p->iPosition = 14;
	p->iId = m_iId = WEAPON_QUANTUM;
	p->iFlags = 0;
	p->iWeight = QUANT_WEIGHT;

	return 1;
}

BOOL CQuantum::Deploy(void)
{
	m_flAccuracy = 0.2;
	m_iShotsFired = 0;
	iShellOn = 1;

	return DefaultDeploy("models/v_revivegun_a_fix.mdl", "models/w_revivegun.mdl", QUANT_DRAW, "draw", UseDecrement() != FALSE);
}

void CQuantum::SecondaryAttack(void)
{
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME; // 600
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.9f;
	Getsprite();
	RadiusDamage2();
}

bool CQuantum::PrimaryAttack_CheckTargetAvailable(CBaseEntity* a2, Vector vecAngleDirection)
{
	const float flRadius = 600;

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
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME; // 600
	if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		QuantFire(0.035 + (0.4) * m_flAccuracy, 0.2f, FALSE);
	else if (m_pPlayer->pev->velocity.Length2D() > 140)
		QuantFire(0.035 + (0.07) * m_flAccuracy, 0.2f, FALSE);
	else if (m_pPlayer->pev->fov == 90)
		QuantFire((0.02) * m_flAccuracy, 0.2f, FALSE);
	else
		QuantFire((0.02) * m_flAccuracy, 0.2f, FALSE);
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

void CQuantum::RadiusDamage2()
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
		if (v8 >= 5)
			break;
		if (!pEntity)
			continue;
		Vector vecDirection = (pEntity->pev->origin - m_pPlayer->pev->origin).Normalize();

		TraceResult tr;
		UTIL_TraceLine(m_pPlayer->pev->origin, pEntity->pev->origin, missile, ENT(m_pPlayer->pev), &tr);

		ClearMultiDamage();
		pEntity->TraceAttack(m_pPlayer->pev, 700, vecDirection, &tr, DMG_BULLET);
		ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);


		if (v8 < 3)
		{
#ifndef CLIENT_DLL
			CBeam* pBeam = phs5_6_7[v8];
			if (pBeam)
			{
				pBeam->EntsInit(ENTINDEX(m_pPlayer->edict()), ENTINDEX(pEntity->edict()));
				pBeam->SetType(BEAM_ENTS);
				pBeam->SetStartEntity(ENTINDEX(m_pPlayer->edict()));
				pBeam->SetEndEntity(ENTINDEX(pEntity->edict()));
				pBeam->SetStartAttachment(1);
				pBeam->SetEndAttachment(0);
				pBeam->RelinkBeam();
				pBeam->SetBrightness(230);
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
#endif
	phs9_10_11.clear();
}

void CQuantum::Getsprite()
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

	if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		QuantFire2(0.01 + (0.3) * m_flAccuracy, 0.9f, TRUE);
	else if (m_pPlayer->pev->velocity.Length2D() > 140)
		QuantFire2(0.01 + (0.3) * m_flAccuracy, 0.9f, TRUE);
	else if (m_pPlayer->pev->fov == 90)
		QuantFire2((0.03) * m_flAccuracy, 0.9f, TRUE);
	else
		QuantFire2((0.03) * m_flAccuracy, 0.9f, TRUE);
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

	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usFireQuant, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, (int)(m_pPlayer->pev->punchangle.x * 100), (int)(m_pPlayer->pev->punchangle.y * 100), FALSE, FALSE);
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

	m_iClip--;
	m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
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

	SendWeaponAnim(QUANT_SHOOT4, UseDecrement() != FALSE);
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

	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usFireQuant, 0, (float*)&g_vecZero, (float*)&g_vecZero, vecDir.x, vecDir.y, (int)(m_pPlayer->pev->punchangle.x * 100), (int)(m_pPlayer->pev->punchangle.y * 100), FALSE, FALSE);
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
	if (m_pPlayer->ammo_QuantAmmo <= 0)
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

					//EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/revivegun_clipin1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
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
	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/revivegun_idle1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 4.0;
	SendWeaponAnim(QUANT_IDLE1, UseDecrement() != FALSE);
}

float CQuantum::GetDamage() const
{
	float flDamage = 32.0f;
#ifndef CLIENT_DLL
	if (g_pModRunning->DamageTrack() == DT_ZB)
		flDamage = 700.0f;
	else if (g_pModRunning->DamageTrack() == DT_ZBS)
		flDamage = 1300.0f;
#endif
	return flDamage;
}