/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "wpn_quantumhorizon.h"
#include "gamemode/interface/interface_const.h"

#ifndef CLIENT_DLL
#include "gamemode/mods.h"
#include "util/u_range.hpp"
#endif


enum quant_e
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

LINK_ENTITY_TO_CLASS(weapon_quant, CQUANT)

void CQUANT::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_quant");

	Precache();
	m_iId = WEAPON_AK47;
	SET_MODEL(ENT(pev), "models/w_revivegun.mdl");

	m_iDefaultAmmo = QUANT_DEFAULT_GIVE;
	m_flAccuracy = 0.2;
	m_iShotsFired = 0;

	FallInit();
}

void CQUANT::Precache(void)
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

	m_iShell = PRECACHE_MODEL("models/rshell.mdl");
	m_usFireQuant = PRECACHE_EVENT(1, "events/quant.sc");
}

int CQUANT::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "QuantAmmo";
	p->iMaxAmmo1 = MAX_AMMO_QUANT;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = QUANT_MAX_CLIP;
	p->iSlot = 0;
	p->iPosition = 14;
	p->iId = m_iId = WEAPON_AK47;
	p->iFlags = 0;
	p->iWeight = QUANT_WEIGHT;


	return 1;
}

BOOL CQUANT::Deploy(void)
{
	m_flAccuracy = 0.1;
	m_iShotsFired = 0;
	iShellOn = 1;
	return DefaultDeploy("models/v_revivegun_a_fix.mdl", "models/p_revivegun.mdl", QUANT_DRAW, "draw", UseDecrement() != FALSE);
}

void CQUANT::SecondaryAttack(void)
{
	if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		QuantFire2(0.04 + (1.0) * m_flAccuracy, 0.0955, FALSE);
	else if (m_pPlayer->pev->velocity.Length2D() > 140)
		QuantFire2(0.04 + (1.0) * m_flAccuracy, 0.0955, FALSE);
	else
		QuantFire2((0.0275), 1.0, FALSE);
	if (m_iClip > 0)
	{
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/revivegun-2.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
		SendWeaponAnim(QUANT_SHOOT3, UseDecrement() != FALSE);
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.6;
	}
	
	
}

void CQUANT::PrimaryAttack(void)
{

	if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		QuantFire(0.04 + (0.4) * m_flAccuracy, 0.0955, FALSE);
	else if (m_pPlayer->pev->velocity.Length2D() > 140)
		QuantFire(0.04 + (0.4) * m_flAccuracy, 0.0955, FALSE);
	else
		QuantFire((0.0275), 0.4, FALSE);
	if (m_iClip > 0)
	{
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/revivegun-1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
		SendWeaponAnim(QUANT_SHOOT1, UseDecrement() != FALSE);
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.9;
	}
	
}

void FindHullIntersection4(const Vector& vecSrc, TraceResult& tr, float* pflMins, float* pfkMaxs, edict_t* pEntity)
{
	TraceResult trTemp;
	float flDistance = 2000000;
	float* pflMinMaxs[2] = { pflMins, pfkMaxs };
	Vector vecHullEnd = tr.vecEndPos;

	vecHullEnd = vecSrc + ((vecHullEnd - vecSrc) * 2);
	TRACE_LINE(vecSrc, vecHullEnd, dont_ignore_monsters, pEntity, &trTemp);

	if (trTemp.flFraction < 1)
	{
		tr = trTemp;
		return;
	}

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				Vector vecEnd;
				vecEnd.x = vecHullEnd.x + pflMinMaxs[i][0];
				vecEnd.y = vecHullEnd.y + pflMinMaxs[j][1];
				vecEnd.z = vecHullEnd.z + pflMinMaxs[k][2];

				TRACE_LINE(vecSrc, vecEnd, dont_ignore_monsters, pEntity, &trTemp);

				if (trTemp.flFraction < 1)
				{
					float flThisDistance = (trTemp.vecEndPos - vecSrc).Length();

					if (flThisDistance < flDistance)
					{
						tr = trTemp;
						flDistance = flThisDistance;
					}
				}
			}
		}
	}
}



void CQUANT::QuantFire(float flSpread, float flCycleTime, BOOL fUseAutoAim)
{
	m_bDelayFire = true;
	m_iShotsFired++;
	m_flAccuracy = ((float)(m_iShotsFired * m_iShotsFired * m_iShotsFired) / 200.0) + 0.35;

	if (m_flAccuracy > 1.25)
		m_flAccuracy = 1.25;

	if (m_iClip <= 0)
	{	
		if (m_fFireOnEmpty)
		{
			
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.6;
		}
		
		return;
	}

	m_iClip--;
	m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
#ifndef CLIENT_DLL
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif
	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecDir = m_pPlayer->FireBullets3(vecSrc, gpGlobals->v_forward, flSpread, 8192, 2, BULLET_PLAYER_762MM, 36, 0.98, m_pPlayer->pev, FALSE, m_pPlayer->random_seed);

	int flags;
#ifdef CLIENT_WEAPONS
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usFireQuant, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, (int)(m_pPlayer->pev->punchangle.x * 100), (int)(m_pPlayer->pev->punchangle.y * 100), FALSE, FALSE);

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;

#ifndef CLIENT_DLL
	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
#endif
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.9;

	if (m_pPlayer->pev->velocity.Length2D() > 0)
		KickBack(1.5, 0.45, 0.225, 0.05, 6.5, 2.5, 7);
	else if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		KickBack(2.0, 1.0, 0.5, 0.35, 9.0, 6.0, 5);
	else if (FBitSet(m_pPlayer->pev->flags, FL_DUCKING))
		KickBack(0.9, 0.35, 0.15, 0.025, 5.5, 1.5, 9);
	else
		KickBack(1.0, 0.375, 0.175, 0.0375, 5.75, 1.75, 8);
}

void CQUANT::QuantFire2(float flSpread, float flCycleTime, BOOL fUseAutoAim)
{
	m_bDelayFire = true;
	m_iShotsFired++;
	m_flAccuracy = ((float)(m_iShotsFired * m_iShotsFired * m_iShotsFired) / 200.0) + 0.35;

	if (m_flAccuracy > 1.25)
		m_flAccuracy = 1.25;

	if (m_iClip <= 0)
	{
		if (m_fFireOnEmpty)
		{

			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2;
		}

		return;
	}

	m_iClip--;
	m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
#ifndef CLIENT_DLL
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif
	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecDir = m_pPlayer->FireBullets3(vecSrc, gpGlobals->v_forward, flSpread, 8192, 2, BULLET_PLAYER_762MM, 36, 0.98, m_pPlayer->pev, FALSE, m_pPlayer->random_seed);

	int flags;
#ifdef CLIENT_WEAPONS
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usFireQuant, 0, (float*)&g_vecZero, (float*)&g_vecZero, vecDir.x, vecDir.y, (int)(m_pPlayer->pev->punchangle.x * 100), (int)(m_pPlayer->pev->punchangle.y * 100), FALSE, FALSE);

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;

#ifndef CLIENT_DLL
	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
#endif
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.9;

	if (m_pPlayer->pev->velocity.Length2D() > 0)
		KickBack(1.5, 0.45, 0.225, 0.05, 6.5, 2.5, 7);
	else if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		KickBack(2.0, 1.0, 0.5, 0.35, 9.0, 6.0, 5);
	else if (FBitSet(m_pPlayer->pev->flags, FL_DUCKING))
		KickBack(0.9, 0.35, 0.15, 0.025, 5.5, 1.5, 9);
	else
		KickBack(1.0, 0.375, 0.175, 0.0375, 5.75, 1.75, 8);
}

void CQUANT::WeaponIdle(void)
{

	ResetEmptySound();
	m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20;
	SendWeaponAnim(QUANT_IDLE1, UseDecrement() != FALSE);
}

void CQUANT::Reload(void)
{
	if (m_pPlayer->ammo_QuantAmmo <= 0)
		return;

	if (DefaultReload(QUANT_MAX_CLIP, QUANT_RELOAD, 2.45))
	{
#ifndef CLIENT_DLL
		m_pPlayer->SetAnimation(PLAYER_RELOAD);
	
#endif
		m_flAccuracy = 0.2;
		m_iShotsFired = 0;
		m_bDelayFire = false;
	}
}


