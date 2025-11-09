/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "wpn_guitar.h"

enum guitar_e
{
	GUITAR_IDLE,
	GUITAR_RELOAD,
	GUITAR_DRAW,
	GUITAR_SHOOT1,
	GUITAR_SHOOT2,
	GUITAR_SHOOT3
};

LINK_ENTITY_TO_CLASS(weapon_guitar, CGuitar)

void CGuitar::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_guitar");

	Precache();
	m_iId = WEAPON_M4A1;
	SET_MODEL(ENT(pev), "models/w_guitar.mdl");

	m_iDefaultAmmo = M4A1_DEFAULT_GIVE;
	m_flAccuracy = 0.2;
	m_iShotsFired = 0;
	m_bDelayFire = true;

	FallInit();
}

void CGuitar::Precache(void)
{
	PRECACHE_MODEL("models/v_guitar.mdl");
	PRECACHE_MODEL("models/p_guitar.mdl");
	PRECACHE_MODEL("models/w_guitar.mdl");

	PRECACHE_SOUND("weapons/gt.wav");
	PRECACHE_SOUND("weapons/gt_clipin.wav");
	PRECACHE_SOUND("weapons/gt_clipon.wav");
	PRECACHE_SOUND("weapons/gt_clipout.wav");
	PRECACHE_SOUND("weapons/gt_draw.wav");
	PRECACHE_MODEL("sprites/muzzleflash8.spr");

	m_iShell = PRECACHE_MODEL("models/rshell.mdl");
	m_usFireGuitar = PRECACHE_EVENT(1, "events/Guitar.sc");
}

int CGuitar::GetItemInfo(ItemInfo *p)
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

BOOL CGuitar::Deploy(void)
{
	m_bDelayFire = true;
	iShellOn = 1;
	m_flAccuracy = 0.2;
	m_iShotsFired = 0;

	return DefaultDeploy("models/v_guitar.mdl", "models/p_guitar.mdl", GUITAR_DRAW, "rifle", UseDecrement() != FALSE);
}

void CGuitar::PrimaryAttack(void)
{
	if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		GUITARFire(0.035 + (0.4) * m_flAccuracy, 0.0975, FALSE);
	else if (m_pPlayer->pev->velocity.Length2D() > 140)
		GUITARFire(0.035 + (0.07) * m_flAccuracy, 0.0975, FALSE);
	else
		GUITARFire((0.02) * m_flAccuracy, 0.0975, FALSE);
}

void CGuitar::GUITARFire(float flSpread, float flCycleTime, BOOL fUseAutoAim)
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

	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/gt.wav", VOL_NORM, ATTN_NORM);

	switch (RANDOM_LONG(1, 3))
	{
	case 1:
		SendWeaponAnim(GUITAR_SHOOT1, UseDecrement() != FALSE);
		break;
	case 2:
		SendWeaponAnim(GUITAR_SHOOT2, UseDecrement() != FALSE);
		break;
	case 3:
		SendWeaponAnim(GUITAR_SHOOT3, UseDecrement() != FALSE);
		break;
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

	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usFireGuitar, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, (int)(m_pPlayer->pev->punchangle.x * 100), (int)(m_pPlayer->pev->punchangle.y * 100), (m_iWeaponState & WPNSTATE_M4A1_SILENCED) ? true : false, FALSE);
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

void CGuitar::Reload(void)
{
	if (m_pPlayer->ammo_556nato <= 0)
		return;

	int iAnim;

	iAnim = GUITAR_RELOAD;
	

	if (DefaultReload(M4A1_MAX_CLIP, iAnim, 3.05))
	{
#ifndef CLIENT_DLL
		m_pPlayer->SetAnimation(PLAYER_RELOAD);
#endif
		m_flAccuracy = 0.2;
		m_iShotsFired = 0;
		m_bDelayFire = false;
	}
}

void CGuitar::WeaponIdle(void)
{
	ResetEmptySound();
	m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20;

	SendWeaponAnim(GUITAR_IDLE, UseDecrement() != FALSE);
}
