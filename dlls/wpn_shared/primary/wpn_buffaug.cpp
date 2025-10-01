/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "wpn_buffaug.h"
#ifndef CLIENT_DLL
#include "gamemode/mods.h"
#endif

enum buffaug_e
{
	AUG_IDLE1,
	AUG_RELOAD,
	AUG_DRAW,
	AUG_SHOOT1,
	AUG_SHOOT2,
	AUG_SHOOT3
};

LINK_ENTITY_TO_CLASS(weapon_buffaug, CBUFFAUG)

void CBUFFAUG::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_buffaug");

	Precache();
	m_iId = WEAPON_AUG;
	SET_MODEL(ENT(pev), "models/w_buffaug.mdl");

	m_iDefaultAmmo = AUG_DEFAULT_GIVE;
	m_flAccuracy = 0.2;
	m_iShotsFired = 0;

	FallInit();
}

void CBUFFAUG::Precache(void)
{
	PRECACHE_MODEL("models/v_buffaug.mdl");
	PRECACHE_MODEL("models/p_buffaug.mdl");
	PRECACHE_MODEL("models/w_buffaug.mdl");
	PRECACHE_MODEL("sprites/ef_buffaugsmoke.spr");

	PRECACHE_SOUND("weapons/buffaug_draw.wav");
	PRECACHE_SOUND("weapons/buffaug_idle.wav");
	PRECACHE_SOUND("weapons/buffaug_reload.wav");
	PRECACHE_SOUND("weapons/buffaug-1.wav");

	m_iShell = PRECACHE_MODEL("models/rshell.mdl");
	m_usFireBuffAUG = PRECACHE_EVENT(1, "events/buffaug.sc");
}

int CBUFFAUG::GetItemInfo(ItemInfo *p)
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
	p->iMaxClip = AUG_MAX_CLIP;
	p->iSlot = 0;
	p->iPosition = 14;
	p->iId = m_iId = WEAPON_AUG;
	p->iFlags = 0;
	p->iWeight = AUG_WEIGHT;

	return 1;
}

BOOL CBUFFAUG::Deploy(void)
{
	m_flAccuracy = 0.2;
	m_iShotsFired = 0;
	iShellOn = 1;

	return DefaultDeploy("models/v_buffaug.mdl", "models/p_buffaug.mdl", AUG_DRAW, "carbine", UseDecrement() != FALSE);
}

void CBUFFAUG::SecondaryAttack(void)
{
	if (m_pPlayer->m_iFOV != 90)
		m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 90;
	else
		m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 55;

	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.3;
}

void CBUFFAUG::PrimaryAttack(void)
{
	if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		BUFFAUGFire(0.035 + (0.4) * m_flAccuracy, 0.0825, FALSE);
	else if (m_pPlayer->pev->velocity.Length2D() > 140)
		BUFFAUGFire(0.035 + (0.07) * m_flAccuracy, 0.0825, FALSE);
	else if (m_pPlayer->pev->fov == 90)
		BUFFAUGFire((0.02) * m_flAccuracy, 0.0825, FALSE);
	else
		BUFFAUGFire((0.02) * m_flAccuracy, 0.135, FALSE);
}

void CBUFFAUG::BUFFAUGFire(float flSpread, float flCycleTime, BOOL fUseAutoAim)
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
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2;
		}

		return;
	}

	m_iClip--;
	m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
#ifndef CLIENT_DLL
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

#ifndef CLIENT_DLL
	if (g_pModRunning->DamageTrack() == DT_ZBS)
		BUFFAUG_DAMAGE = 240.0f;
	if (g_pModRunning->DamageTrack() == DT_ZB)
		BUFFAUG_DAMAGE = 125.0f;
	else
		BUFFAUG_DAMAGE = 44.0f;
#endif

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecDir = m_pPlayer->FireBullets3(vecSrc, gpGlobals->v_forward, flSpread, 8192, 2, BULLET_PLAYER_556MM, BUFFAUG_DAMAGE, 0.96, m_pPlayer->pev, FALSE, m_pPlayer->random_seed);

	int flags;
#ifdef CLIENT_WEAPONS
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/buffaug-1.wav", VOL_NORM, ATTN_NORM);

	switch (RANDOM_LONG(1,3))
	{
	case 1:
		SendWeaponAnim(AUG_SHOOT1, UseDecrement() != FALSE);
		break;
	case 2:
		SendWeaponAnim(AUG_SHOOT2, UseDecrement() != FALSE);
		break;
	case 3:
		SendWeaponAnim(AUG_SHOOT3, UseDecrement() != FALSE);
		break;
	}

	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usFireBuffAUG, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, (int)(m_pPlayer->pev->punchangle.x * 100), (int)(m_pPlayer->pev->punchangle.y * 100), FALSE, FALSE);
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;

#ifndef CLIENT_DLL
	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
#endif
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.9;

	if (m_pPlayer->pev->velocity.Length2D() > 0)
		KickBack(1.0, 0.45, 0.275, 0.05, 4.0, 2.5, 7);
	else if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		KickBack(1.25, 0.45, 0.22, 0.18, 5.5, 4.0, 5);
	else if (FBitSet(m_pPlayer->pev->flags, FL_DUCKING))
		KickBack(0.575, 0.325, 0.2, 0.011, 3.25, 2.0, 8);
	else
		KickBack(0.625, 0.375, 0.25, 0.0125, 3.5, 2.25, 8);
}

void CBUFFAUG::Reload(void)
{
	if (m_pPlayer->ammo_556nato <= 0)
		return;

	if (DefaultReload(AUG_MAX_CLIP, AUG_RELOAD, 2.0))
	{
#ifndef CLIENT_DLL
		m_pPlayer->SetAnimation(PLAYER_RELOAD);
#endif
		if (m_pPlayer->m_iFOV != 90)
			SecondaryAttack();

		m_flAccuracy = 0;
		m_iShotsFired = 0;
		m_bDelayFire = false;
	}
}

void CBUFFAUG::WeaponIdle(void)
{
	ResetEmptySound();
	m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20;
	SendWeaponAnim(AUG_IDLE1, UseDecrement() != FALSE);
}