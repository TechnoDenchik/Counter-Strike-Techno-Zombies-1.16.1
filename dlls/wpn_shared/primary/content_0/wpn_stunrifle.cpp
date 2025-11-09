/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "wpn_stunrifle.h"

enum stunrifle_e
{
	STUNRIFLE_IDLEA,
	STUNRIFLE_IDLEB,
	STUNRIFLE_IDLEC,
	STUNRIFLE_SHOOTA,
	STUNRIFLE_SHOOTB,
	STUNRIFLE_SHOOTC,
	STUNRIFLE_RELOADA,
	STUNRIFLE_RELOADB,
	STUNRIFLE_RELOADC,
	STUNRIFLE_DRAWA,
	STUNRIFLE_DRAWB,
	STUNRIFLE_DRAWC,
};

LINK_ENTITY_TO_CLASS(weapon_stunrifle, CSTUNRIFLE)

void CSTUNRIFLE::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_stunrifle");

	Precache();
	m_iId = WEAPON_AK47;
	SET_MODEL(ENT(pev), "models/w_stunrifle.mdl");

	m_iDefaultAmmo = AK47_DEFAULT_GIVE;
	m_flAccuracy = 0.2;
	m_iShotsFired = 0;

	FallInit();
}

void CSTUNRIFLE::Precache(void)
{
	PRECACHE_MODEL("models/v_stunrifle.mdl");
	PRECACHE_MODEL("models/p_stunrifle.mdl");
	PRECACHE_MODEL("models/w_stunrifle.mdl");

	PRECACHE_MODEL("sprites/muzzleflash67.spr");
	PRECACHE_MODEL("sprites/lgtning.spr");

	PRECACHE_SOUND("weapons/stunrifle_drawa.wav");
	PRECACHE_SOUND("weapons/stunrifle_drawb.wav");
	PRECACHE_SOUND("weapons/stunrifle_drawc.wav");
	PRECACHE_SOUND("weapons/stunrifle_idlea.wav");
	PRECACHE_SOUND("weapons/stunrifle_idleb.wav");
	PRECACHE_SOUND("weapons/stunrifle_idlec.wav");
	PRECACHE_SOUND("weapons/stunrifle_lowbattery.wav");
	PRECACHE_SOUND("weapons/stunrifle_reloada.wav");
	PRECACHE_SOUND("weapons/stunrifle_reloadb.wav");
	PRECACHE_SOUND("weapons/stunrifle_reloadc.wav");
	PRECACHE_SOUND("weapons/stunrifle-1.wav");
	PRECACHE_SOUND("weapons/stunrifle-2.wav");

	m_iShell = PRECACHE_MODEL("models/rshell.mdl");
	m_usFireSTUNRIFLE = PRECACHE_EVENT(1, "events/stunrifle.sc");
}

int CSTUNRIFLE::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "762Nato";
	p->iMaxAmmo1 = MAX_AMMO_762NATO;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->pszAmmo3 = NULL;
	p->iMaxAmmo3 = -1;
	p->pszAmmoGrenade = NULL;
	p->iMaxAmmoGrenade = -1;
	p->iMaxClip = AK47_MAX_CLIP;
	p->iSlot = 0;
	p->iPosition = 1;
	p->iId = m_iId = WEAPON_AK47;
	p->iFlags = 0;
	p->iWeight = AK47_WEIGHT;

	return 1;
}

BOOL CSTUNRIFLE::Deploy(void)
{
	m_flAccuracy = 0.2;
	m_iShotsFired = 0;
	iShellOn = 1;
	return DefaultDeploy("models/v_stunrifle.mdl", "models/p_stunrifle.mdl", STUNRIFLE_DRAWA, "ak47", UseDecrement() != FALSE);
}

void CSTUNRIFLE::PrimaryAttack(void)
{
	if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		STUNRIFLEFire(0.04 + (0.4) * m_flAccuracy, 0.0955, FALSE);
	else if (m_pPlayer->pev->velocity.Length2D() > 140)
		STUNRIFLEFire(0.04 + (0.07) * m_flAccuracy, 0.0955, FALSE);
	else
		STUNRIFLEFire((0.0275), 0.0955, FALSE);
}

void CSTUNRIFLE::STUNRIFLEFire(float flSpread, float flCycleTime, BOOL fUseAutoAim)
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
			PlayEmptySound();
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2;
		}

		return;
	}

	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/stunrifle-1.wav", VOL_NORM, ATTN_NORM);
	SendWeaponAnim(STUNRIFLE_SHOOTA, UseDecrement() != FALSE);
	
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

	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usFireSTUNRIFLE, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, (int)(m_pPlayer->pev->punchangle.x * 100), (int)(m_pPlayer->pev->punchangle.y * 100), FALSE, FALSE);

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

void CSTUNRIFLE::Reload(void)
{
	if (m_pPlayer->ammo_762nato <= 0)
		return;

	if (DefaultReload(AK47_MAX_CLIP, STUNRIFLE_RELOADA, 2.45))
	{
#ifndef CLIENT_DLL
		m_pPlayer->SetAnimation(PLAYER_RELOAD);
#endif
		m_flAccuracy = 0.2;
		m_iShotsFired = 0;
		m_bDelayFire = false;
	}
}

void CSTUNRIFLE::ItemPostFrame()
{
	return CBasePlayerWeapon::ItemPostFrame();
}

void CSTUNRIFLE::WeaponIdle(void)
{
	ResetEmptySound();
	m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20;
	SendWeaponAnim(STUNRIFLE_IDLEA, UseDecrement() != FALSE);
}