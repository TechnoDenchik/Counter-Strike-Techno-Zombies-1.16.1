/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "wpn_balrog7.h"

enum balrog_e
{
	BALROG_IDLE1,
	BALROG_SHOOT1,
	BALROG_SHOOT2,
	BALROG_SHOOT3,
	BALROG_RELOAD,
	BALROG_DRAW
};

LINK_ENTITY_TO_CLASS(weapon_balrog7, CBalrog7)

void CBalrog7::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_balrog7");

	Precache();
	m_iId = WEAPON_M249;
	SET_MODEL(ENT(pev), "models/w_balrog7.mdl");

	m_iDefaultAmmo = M249_DEFAULT_GIVE;
	m_flAccuracy = 0.2;
	m_iShotsFired = 0;

	FallInit();
}

void CBalrog7::Precache(void)
{
	PRECACHE_MODEL("models/v_balrog7.mdl");
	PRECACHE_MODEL("models/w_balrog7.mdl");

	PRECACHE_SOUND("weapons/balrog7_clipin1.wav");
	PRECACHE_SOUND("weapons/balrog7_clipin2.wav");
	PRECACHE_SOUND("weapons/balrog7_clipin3.wav");
	PRECACHE_SOUND("weapons/balrog7_clipout1.wav");
	PRECACHE_SOUND("weapons/balrog7_clipout2.wav");
	PRECACHE_SOUND("weapons/balrog7-1.wav");
	PRECACHE_SOUND("weapons/balrog7-2.wav");

	m_iShell = PRECACHE_MODEL("models/rshell.mdl");
	m_usFireM249 = PRECACHE_EVENT(1, "events/balrog7.sc");
}

int CBalrog7::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "556NatoBox";
	p->iMaxAmmo1 = MAX_AMMO_556NATOBOX;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->pszAmmo3 = NULL;
	p->iMaxAmmo3 = -1;
	p->pszAmmoGrenade = NULL;
	p->iMaxAmmoGrenade = -1;
	p->iMaxClip = M249_MAX_CLIP;
	p->iSlot = 0;
	p->iPosition = 4;
	p->iId = m_iId = WEAPON_M249;
	p->iFlags = 0;
	p->iWeight = M249_WEIGHT;

	return 1;
}

BOOL CBalrog7::Deploy(void)
{
	m_flAccuracy = 0.2;
	m_iShotsFired = 0;
	iShellOn = 1;

	return DefaultDeploy("models/v_balrog7.mdl", "models/p_balrog7.mdl", BALROG_DRAW, "m249", UseDecrement() != FALSE);
}

void CBalrog7::PrimaryAttack(void)
{
	if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		BALROGFire(0.045 + (0.5) * m_flAccuracy, 0.1, FALSE);
	else if (m_pPlayer->pev->velocity.Length2D() > 140)
		BALROGFire(0.045 + (0.095) * m_flAccuracy, 0.1, FALSE);
	else
		BALROGFire((0.03) * m_flAccuracy, 0.1, FALSE);
}

void CBalrog7::BALROGFire(float flSpread, float flCycleTime, BOOL fUseAutoAim)
{
	m_bDelayFire = true;
	m_iShotsFired++;
	m_flAccuracy = ((m_iShotsFired * m_iShotsFired * m_iShotsFired) / 175.0) + 0.4;

	if (m_flAccuracy > 0.9)
		m_flAccuracy = 0.9;

	if (m_iClip <= 0)
	{
		if (m_fFireOnEmpty)
		{
			PlayEmptySound();
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2;
		}

		return;
	}

	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/balrog7-1.wav", VOL_NORM, ATTN_NORM);

	SendWeaponAnim(BALROG_SHOOT1, UseDecrement() != FALSE);
	
	m_iClip--;
	m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
#ifndef CLIENT_DLL
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecDir = m_pPlayer->FireBullets3(vecSrc, gpGlobals->v_forward, flSpread, 8192, 2, BULLET_PLAYER_556MM, 32, 0.97, m_pPlayer->pev, FALSE, m_pPlayer->random_seed);

	int flags;
#ifdef CLIENT_WEAPONS
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usFireM249, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, (int)(m_pPlayer->pev->punchangle.x * 100), (int)(m_pPlayer->pev->punchangle.y * 100), FALSE, FALSE);
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;

#ifndef CLIENT_DLL
	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
#endif
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.6;

	if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		KickBack(1.8, 0.65, 0.45, 0.125, 5.0, 3.5, 8);
	else if (m_pPlayer->pev->velocity.Length2D() > 0)
		KickBack(1.1, 0.5, 0.3, 0.06, 4.0, 3.0, 8);
	else if (FBitSet(m_pPlayer->pev->flags, FL_DUCKING))
		KickBack(0.75, 0.325, 0.25, 0.025, 3.5, 2.5, 9);
	else
		KickBack(0.8, 0.35, 0.3, 0.03, 3.75, 3.0, 9);
}

void CBalrog7::Reload(void)
{
	if (m_pPlayer->ammo_556natobox <= 0)
		return;

	if (DefaultReload(M249_MAX_CLIP, BALROG_RELOAD, 4.7))
	{
#ifndef CLIENT_DLL
		m_pPlayer->SetAnimation(PLAYER_RELOAD);
#endif
		m_flAccuracy = 0.2;
		m_bDelayFire = false;
		m_iShotsFired = 0;
	}
}

void CBalrog7::WeaponIdle(void)
{
	ResetEmptySound();
	m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20;
	SendWeaponAnim(BALROG_IDLE1, UseDecrement() != FALSE);
}
