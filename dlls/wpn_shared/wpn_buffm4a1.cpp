/* =================================================================================== *
			  * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "wpn_buffm4a1.h"

enum buffm4a1_e
{
	M4A1_IDLE,
	M4A1_RELOAD,
	M4A1_DRAW,
	M4A1_SHOOT1,
	M4A1_SHOOT2,
	M4A1_SHOOT3,
};

LINK_ENTITY_TO_CLASS(weapon_buffm4a1, CM4A1Buff)

void CM4A1Buff::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_buffm4a1");

	Precache();
	m_iId = WEAPON_M4A1;
	SET_MODEL(ENT(pev), "models/w_buffm4.mdl");

	m_iDefaultAmmo = M4A1_DEFAULT_GIVE;
	m_flAccuracy = 0.2;
	m_iShotsFired = 0;
	m_bDelayFire = true;

	FallInit();
}

void CM4A1Buff::Precache(void)
{
	PRECACHE_MODEL("models/v_buffm4.mdl");
	PRECACHE_MODEL("models/p_buffm4.mdl");
	PRECACHE_MODEL("models/w_buffm4.mdl");

	PRECACHE_SOUND("weapons/m4a1buff-1.wav");
	PRECACHE_SOUND("weapons/m4a1buff-2.wav");
	PRECACHE_SOUND("weapons/m4a1buff_idle.wav");
	PRECACHE_SOUND("weapons/m4a1buff_clipin1.wav");
	PRECACHE_SOUND("weapons/m4a1buff_clipin2.wav");
	PRECACHE_SOUND("weapons/m4a1buff_clipout.wav");
	PRECACHE_MODEL("weapons/muzzleflash43.spr");
	PRECACHE_MODEL("weapons/muzzleflash45.spr");

	m_iShell = PRECACHE_MODEL("models/rshell.mdl");
	m_usFireM4A1Buff = PRECACHE_EVENT(1, "events/buffm4a1.sc");
}

int CM4A1Buff::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "556Nato";
	p->iMaxAmmo1 = MAX_AMMO_556NATO;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->pszAmmo3 = NULL;
	p->iMaxAmmo3 = -1;
	p->pszAmmoGrenade = NULL;
	p->iMaxAmmoGrenade = -1;
	p->iMaxClip = M4A1_MAX_CLIP;
	p->iSlot = 0;
	p->iPosition = 6;
	p->iId = m_iId = WEAPON_M4A1;
	p->iFlags = 0;
	p->iWeight = M4A1_WEIGHT;

	return 1;
}

BOOL CM4A1Buff::Deploy(void)
{
	m_bDelayFire = true;
	iShellOn = 1;
	m_flAccuracy = 0.2;
	m_iShotsFired = 0;

	return DefaultDeploy("models/v_buffm4.mdl", "models/p_buffm4.mdl", M4A1_DRAW, "rifle", UseDecrement() != FALSE);
}

void CM4A1Buff::SecondaryAttack(void)
{
	if (m_pPlayer->m_iFOV != 90)
	{
		secondaryattack = false;
		m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 90;
	}
	else
	{
		secondaryattack = true;
		m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 80;
	}
		
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.5;
}

void CM4A1Buff::PrimaryAttack(void)
{
	if (secondaryattack == false)
	{
		if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
			M4A1BuffFire(0.035 + (0.4) * m_flAccuracy, 0.0875, FALSE);
		else if (m_pPlayer->pev->velocity.Length2D() > 140)
			M4A1BuffFire(0.035 + (0.07) * m_flAccuracy, 0.0875, FALSE);
		else
			M4A1BuffFire((0.025) * m_flAccuracy, 0.0875, FALSE);
	}
	else
	{
		if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
			M4A1BuffFire2(0.035 + (0.4) * m_flAccuracy, 0.250, FALSE);
		else if (m_pPlayer->pev->velocity.Length2D() > 140)
			M4A1BuffFire2(0.035 + (0.07) * m_flAccuracy, 0.250, FALSE);
		else
			M4A1BuffFire2((0.02) * m_flAccuracy, 0.250, FALSE);
	}
}

void CM4A1Buff::M4A1BuffFire(float flSpread, float flCycleTime, BOOL fUseAutoAim)
{
	m_bDelayFire = true;
	m_iShotsFired++;
	m_flAccuracy = ((float)(m_iShotsFired * m_iShotsFired * m_iShotsFired) / 220) + 0.3;

	if (m_flAccuracy > 1)
		m_flAccuracy = 1;

	if (m_iClip <= 0)
	{
		if (m_fFireOnEmpty)
		{
			PlayEmptySound();
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2;
		}

		return;
	}

	m_iClip--;
#ifndef CLIENT_DLL
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecDir;

	if (m_iWeaponState & WPNSTATE_M4A1_SILENCED)
	{
		vecDir = m_pPlayer->FireBullets3(vecSrc, gpGlobals->v_forward, flSpread, 8192, 2, BULLET_PLAYER_556MM, 33, 0.95, m_pPlayer->pev, FALSE, m_pPlayer->random_seed);
	}
	else
	{
		vecDir = m_pPlayer->FireBullets3(vecSrc, gpGlobals->v_forward, flSpread, 8192, 2, BULLET_PLAYER_556MM, 32, 0.97, m_pPlayer->pev, FALSE, m_pPlayer->random_seed);
		m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
	}

	int flags;
#ifdef CLIENT_WEAPONS
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif
	SendWeaponAnim(M4A1_SHOOT1, UseDecrement() != FALSE);
	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/m4a1buff-1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usFireM4A1Buff, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, (int)(m_pPlayer->pev->punchangle.x * 100), (int)(m_pPlayer->pev->punchangle.y * 100), (m_iWeaponState & WPNSTATE_M4A1_SILENCED) ? true : false, FALSE);
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;
#ifndef CLIENT_DLL
	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
#endif
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.5;

	if (m_pPlayer->pev->velocity.Length2D() > 0)
		KickBack(1.0, 0.45, 0.28, 0.045, 3.75, 3.0, 7);
	else if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		KickBack(1.2, 0.5, 0.23, 0.15, 5.5, 3.5, 6);
	else if (FBitSet(m_pPlayer->pev->flags, FL_DUCKING))
		KickBack(0.6, 0.3, 0.2, 0.0125, 3.25, 2.0, 7);
	else
		KickBack(0.65, 0.35, 0.25, 0.015, 3.5, 2.25, 7);
}

void CM4A1Buff::M4A1BuffFire2(float flSpread, float flCycleTime, BOOL fUseAutoAim)
{
	m_bDelayFire = true;
	m_iShotsFired++;
	m_flAccuracy = ((float)(m_iShotsFired * m_iShotsFired * m_iShotsFired) / 220) + 0.3;

	if (m_flAccuracy > 1)
		m_flAccuracy = 1;

	if (m_iClip <= 0)
	{
		if (m_fFireOnEmpty)
		{
			PlayEmptySound();
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2;
		}

		return;
	}

	m_iClip--;
#ifndef CLIENT_DLL
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecDir;

	if (m_iWeaponState & WPNSTATE_M4A1_SILENCED)
	{
		vecDir = m_pPlayer->FireBullets3(vecSrc, gpGlobals->v_forward, flSpread, 8192, 2, BULLET_PLAYER_556MM, 33, 0.95, m_pPlayer->pev, FALSE, m_pPlayer->random_seed);
	}
	else
	{
		vecDir = m_pPlayer->FireBullets3(vecSrc, gpGlobals->v_forward, flSpread, 8192, 2, BULLET_PLAYER_556MM, 32, 0.97, m_pPlayer->pev, FALSE, m_pPlayer->random_seed);
		m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
	}

	int flags;
#ifdef CLIENT_WEAPONS
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif
	SendWeaponAnim(M4A1_SHOOT3, UseDecrement() != FALSE);
	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/m4a1buff-2.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usFireM4A1Buff, 0, (float*)&g_vecZero, (float*)&g_vecZero, vecDir.x, vecDir.y, (int)(m_pPlayer->pev->punchangle.x * 100), (int)(m_pPlayer->pev->punchangle.y * 100), (m_iWeaponState & WPNSTATE_M4A1_SILENCED) ? true : false, FALSE);
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;
#ifndef CLIENT_DLL
	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
#endif
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.5;

	if (m_pPlayer->pev->velocity.Length2D() > 0)
		KickBack(1.0, 0.45, 0.28, 0.045, 3.75, 3.0, 7);
	else if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		KickBack(1.2, 0.5, 0.23, 0.15, 5.5, 3.5, 6);
	else if (FBitSet(m_pPlayer->pev->flags, FL_DUCKING))
		KickBack(0.6, 0.3, 0.2, 0.0125, 3.25, 2.0, 7);
	else
		KickBack(0.65, 0.35, 0.25, 0.015, 3.5, 2.25, 7);
}

void CM4A1Buff::Reload(void)
{
	if (m_pPlayer->ammo_556nato <= 0)
		return;

	if (DefaultReload(M4A1_MAX_CLIP, M4A1_RELOAD, 2.20))
	{
#ifndef CLIENT_DLL
		m_pPlayer->SetAnimation(PLAYER_RELOAD);
#endif
		if(m_pPlayer->m_iFOV != 90)
		{
			secondaryattack = false;
			m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 90;
		}
		m_flAccuracy = 0.2;
		m_iShotsFired = 0;
		m_bDelayFire = false;
	}
}

void CM4A1Buff::WeaponIdle(void)
{
	ResetEmptySound();
	m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20;

	SendWeaponAnim(M4A1_IDLE, UseDecrement() != FALSE);	
}