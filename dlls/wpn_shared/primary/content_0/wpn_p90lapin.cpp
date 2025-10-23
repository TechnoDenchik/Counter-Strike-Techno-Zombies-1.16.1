/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "wpn_p90lapin.h"

enum p90lapin_e
{
	P90LAPIN_IDLE1,
	P90LAPIN_RELOAD,
	P90LAPIN_DRAW,
	P90LAPIN_SHOOT1,
	P90LAPIN_SHOOT2,
	P90LAPIN_SHOOT3
};

LINK_ENTITY_TO_CLASS(weapon_p90lapin, CP90LAPIN)

void CP90LAPIN::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_p90lapin");

	Precache();
	m_iId = WEAPON_P90;
	SET_MODEL(ENT(pev), "models/w_p90lapin.mdl");

	m_iDefaultAmmo = P90_DEFAULT_GIVE;
	m_flAccuracy = 0.2;
	m_iShotsFired = 0;
	m_bDelayFire = false;

	FallInit();
}

void CP90LAPIN::Precache(void)
{
	PRECACHE_MODEL("models/v_p90lapin.mdl");
	PRECACHE_MODEL("models/p_p90lapin.mdl");
	PRECACHE_MODEL("models/w_p90lapin.mdl");

	PRECACHE_SOUND("weapons/p90-1.wav");
	PRECACHE_SOUND("weapons/p90_clipout.wav");
	PRECACHE_SOUND("weapons/p90_clipin.wav");
	PRECACHE_SOUND("weapons/p90_boltpull.wav");
	PRECACHE_SOUND("weapons/p90_cliprelease.wav");

	m_iShell = PRECACHE_MODEL("models/rshell.mdl");
	m_usFireP90LAPIN = PRECACHE_EVENT(1, "events/p90lapin.sc");
}

int CP90LAPIN::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "57mm";
	p->iMaxAmmo1 = MAX_AMMO_57MM;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->pszAmmo3 = NULL;
	p->iMaxAmmo3 = -1;
	p->pszAmmoGrenade = NULL;
	p->iMaxAmmoGrenade = -1;
	p->iMaxClip = P90_MAX_CLIP;
	p->iSlot = 0;
	p->iPosition = 8;
	p->iId = m_iId = WEAPON_P90;
	p->iFlags = 0;
	p->iWeight = P90_WEIGHT;

	return 1;
}

BOOL CP90LAPIN::Deploy(void)
{
	m_iShotsFired = 0;
	m_bDelayFire = false;
	m_flAccuracy = 0.2;

	return DefaultDeploy("models/v_p90lapin.mdl", "models/p_p90lapin.mdl", P90LAPIN_DRAW, "carbine", UseDecrement() != FALSE);
}

void CP90LAPIN::PrimaryAttack(void)
{
	if (!(m_pPlayer->pev->flags & FL_ONGROUND))
		P90LAPINFire((0.3) * m_flAccuracy, 0.066, FALSE);
	else if (m_pPlayer->pev->velocity.Length2D() > 170)
		P90LAPINFire((0.115) * m_flAccuracy, 0.066, FALSE);
	else
		P90LAPINFire((0.045) * m_flAccuracy, 0.066, FALSE);
}

void CP90LAPIN::P90LAPINFire(float flSpread, float flCycleTime, BOOL fUseAutoAim)
{
	m_bDelayFire = true;
	m_iShotsFired++;
	m_flAccuracy = ((float)(m_iShotsFired * m_iShotsFired * m_iShotsFired) / 175) + 0.45;

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
	
	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/p90-1.wav", VOL_NORM, ATTN_NORM);
	
	switch (RANDOM_LONG(1, 3))
	{
	case 1:
		SendWeaponAnim(P90LAPIN_SHOOT1, UseDecrement() != FALSE);
		break;
	case 2:
		SendWeaponAnim(P90LAPIN_SHOOT2, UseDecrement() != FALSE);
		break;
	case 3:
		SendWeaponAnim(P90LAPIN_SHOOT3, UseDecrement() != FALSE);
		break;
	}

	m_iClip--;
	m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
#ifndef CLIENT_DLL
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecDir = m_pPlayer->FireBullets3(vecSrc, gpGlobals->v_forward, flSpread, 8192, 1, BULLET_PLAYER_57MM, 21, 0.885, m_pPlayer->pev, FALSE, m_pPlayer->random_seed);

	int flags;
#ifdef CLIENT_WEAPONS
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usFireP90LAPIN, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, (int)(m_pPlayer->pev->punchangle.x * 100), (int)(m_pPlayer->pev->punchangle.y * 100), 5, FALSE);
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;
#ifndef CLIENT_DLL
	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
#endif
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2;

	if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		KickBack(0.9, 0.45, 0.35, 0.04, 5.25, 3.5, 4);
	else if (m_pPlayer->pev->velocity.Length2D() > 0)
		KickBack(0.45, 0.3, 0.2, 0.0275, 4.0, 2.25, 7);
	else if (FBitSet(m_pPlayer->pev->flags, FL_DUCKING))
		KickBack(0.275, 0.2, 0.125, 0.02, 3.0, 1.0, 9);
	else
		KickBack(0.3, 0.225, 0.125, 0.02, 3.25, 1.25, 8);
}

void CP90LAPIN::Reload(void)
{
	if (m_pPlayer->ammo_57mm <= 0)
		return;

	if (DefaultReload(P90_MAX_CLIP, P90LAPIN_RELOAD, 3.4))
	{
#ifndef CLIENT_DLL
		m_pPlayer->SetAnimation(PLAYER_RELOAD);
#endif
		m_flAccuracy = 0.2;
		m_iShotsFired = 0;
	}
}

void CP90LAPIN::WeaponIdle(void)
{
	ResetEmptySound();
	m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20;
	SendWeaponAnim(P90LAPIN_IDLE1, UseDecrement() != FALSE);
}