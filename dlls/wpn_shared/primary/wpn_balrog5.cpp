/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "wpn_balrog5.h"

enum balrog_e
{
	BALROG_IDLE1,
	BALROG_SHOOT1,
	BALROG_SHOOT2,
	BALROG_SHOOT3,
	BALROG_RELOAD,
	BALROG_DRAW
};

LINK_ENTITY_TO_CLASS(weapon_balrog5, CBalrog5)

void CBalrog5::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_balrog5");

	Precache();
	m_iId = WEAPON_MP5N;
	SET_MODEL(ENT(pev), "models/w_balrog5.mdl");

	m_iDefaultAmmo = MP5NAVY_DEFAULT_GIVE;
	m_flAccuracy = 0;
	m_bDelayFire = false;

	FallInit();
}

void CBalrog5::Precache(void)
{
	PRECACHE_MODEL("models/v_balrog5.mdl");
	PRECACHE_MODEL("models/w_balrog5.mdl");
	PRECACHE_MODEL("sprites/balrog5stack.spr");

	PRECACHE_SOUND("weapons/balrog5_clipin1.wav");
	PRECACHE_SOUND("weapons/balrog5_clipin2.wav");
	PRECACHE_SOUND("weapons/balrog5_clipout1.wav");
	PRECACHE_SOUND("weapons/balrog5-1.wav");
	PRECACHE_SOUND("weapons/balrog5-2.wav");
	PRECACHE_SOUND("weapons/balrog5-3.wav");

	m_iShell = PRECACHE_MODEL("models/pshell.mdl");
	m_usFireMP5N = PRECACHE_EVENT(1, "events/balrog5.sc");
}

int CBalrog5::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "9mm";
	p->iMaxAmmo1 = MAX_AMMO_9MM;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->pszAmmo3 = NULL;
	p->iMaxAmmo3 = -1;
	p->pszAmmoGrenade = NULL;
	p->iMaxAmmoGrenade = -1;
	p->iMaxClip = MP5N_MAX_CLIP;
	p->iSlot = 0;
	p->iPosition = 7;
	p->iId = m_iId = WEAPON_MP5N;
	p->iFlags = 0;
	p->iWeight = MP5NAVY_WEIGHT;

	return 1;
}

BOOL CBalrog5::Deploy(void)
{
	m_flAccuracy = 0;
	m_bDelayFire = false;
	iShellOn = 1;

	return DefaultDeploy("models/v_balrog5.mdl", "models/p_balrog5.mdl", BALROG_DRAW, "mp5", UseDecrement() != FALSE);
}

void CBalrog5::PrimaryAttack(void)
{
	if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		BALROGFire((0.2) * m_flAccuracy, 0.075, FALSE);
	else
		BALROGFire((0.04) * m_flAccuracy, 0.075, FALSE);
}

void CBalrog5::BALROGFire(float flSpread, float flCycleTime, BOOL fUseAutoAim)
{
	m_bDelayFire = true;
	m_iShotsFired++;
	m_flAccuracy = ((float)(m_iShotsFired * m_iShotsFired * m_iShotsFired) / 220.0) + 0.45;

	if (m_flAccuracy > 0.75)
		m_flAccuracy = 0.75;

	if (m_iClip <= 0)
	{
		if (m_fFireOnEmpty)
		{
			PlayEmptySound();
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2;
		}

		return;
	}
	
	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/balrog5-1.wav", VOL_NORM, ATTN_NORM);

	SendWeaponAnim(BALROG_SHOOT1, UseDecrement() != FALSE);

	m_iClip--;
	m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
#ifndef CLIENT_DLL
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecDir = m_pPlayer->FireBullets3(vecSrc, gpGlobals->v_forward, flSpread, 8192, 1, BULLET_PLAYER_9MM, 26, 0.84, m_pPlayer->pev, FALSE, m_pPlayer->random_seed);

	int flags;
#ifdef CLIENT_WEAPONS
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usFireMP5N, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, (int)(m_pPlayer->pev->punchangle.x * 100), (int)(m_pPlayer->pev->punchangle.y * 100), FALSE, FALSE);

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;
#ifndef CLIENT_DLL
	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
#endif
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2;

	if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		KickBack(0.9, 0.475, 0.35, 0.0425, 5.0, 3.0, 6);
	else if (m_pPlayer->pev->velocity.Length2D() > 0)
		KickBack(0.5, 0.275, 0.2, 0.03, 3.0, 2.0, 10);
	else if (FBitSet(m_pPlayer->pev->flags, FL_DUCKING))
		KickBack(0.225, 0.15, 0.1, 0.015, 2.0, 1.0, 10);
	else
		KickBack(0.25, 0.175, 0.125, 0.02, 2.25, 1.25, 10);
}

void CBalrog5::Reload(void)
{
	if (m_pPlayer->ammo_9mm <= 0)
		return;

	if (DefaultReload(MP5N_MAX_CLIP, BALROG_RELOAD, 2.63))
	{
#ifndef CLIENT_DLL
		m_pPlayer->SetAnimation(PLAYER_RELOAD);
#endif
		m_flAccuracy = 0;
		m_iShotsFired = 0;
	}
}

void CBalrog5::WeaponIdle(void)
{
	ResetEmptySound();
	m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20;
	SendWeaponAnim(BALROG_IDLE1, UseDecrement() != FALSE);
}
