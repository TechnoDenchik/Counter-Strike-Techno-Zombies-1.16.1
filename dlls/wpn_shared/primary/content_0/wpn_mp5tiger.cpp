/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "wpn_mp5tiger.h"

enum mp5tiger_e
{
	MP5TIGER_IDLE1,
	MP5TIGER_RELOAD,
	MP5TIGER_DRAW,
	MP5TIGER_SHOOT1,
	MP5TIGER_SHOOT2,
	MP5TIGER_SHOOT3
};

LINK_ENTITY_TO_CLASS(weapon_mp5tiger, CMP5TIGER)

void CMP5TIGER::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_mp5tiger");

	Precache();
	m_iId = WEAPON_MP5N;
	SET_MODEL(ENT(pev), "models/w_mp5tiger.mdl");

	m_iDefaultAmmo = MP5NAVY_DEFAULT_GIVE;
	m_flAccuracy = 0;
	m_bDelayFire = false;

	FallInit();
}

void CMP5TIGER::Precache(void)
{
	PRECACHE_MODEL("models/v_mp5tiger.mdl");
	PRECACHE_MODEL("models/p_mp5tiger.mdl");
	PRECACHE_MODEL("models/w_mp5tiger.mdl");

	PRECACHE_SOUND("weapons/mp5-1.wav");
	PRECACHE_SOUND("weapons/mp5-2.wav");
	PRECACHE_SOUND("weapons/mp5_clipout.wav");
	PRECACHE_SOUND("weapons/mp5_clipin.wav");
	PRECACHE_SOUND("weapons/mp5_slideback.wav");

	m_iShell = PRECACHE_MODEL("models/pshell.mdl");
	m_usFireMP5TIGER = PRECACHE_EVENT(1, "events/mp5tiger.sc");
}

int CMP5TIGER::GetItemInfo(ItemInfo *p)
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

BOOL CMP5TIGER::Deploy(void)
{
	m_flAccuracy = 0;
	m_bDelayFire = false;
	iShellOn = 1;

	return DefaultDeploy("models/v_mp5tiger.mdl", "models/p_mp5tiger.mdl", MP5TIGER_DRAW, "mp5", UseDecrement() != FALSE);
}

void CMP5TIGER::PrimaryAttack(void)
{
	if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		MP5TIGERFire((0.2) * m_flAccuracy, 0.075, FALSE);
	else
		MP5TIGERFire((0.04) * m_flAccuracy, 0.075, FALSE);
}

void CMP5TIGER::MP5TIGERFire(float flSpread, float flCycleTime, BOOL fUseAutoAim)
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

	switch (RANDOM_LONG(1, 2))
	{
	case 1:
		EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/mp5-1.wav", VOL_NORM, ATTN_NORM);
		break;
	case 2:
		EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/mp5-2.wav", VOL_NORM, ATTN_NORM);
		break;
	}

	switch (RANDOM_LONG(1, 3))
	{
	case 1:
		SendWeaponAnim(MP5TIGER_SHOOT1, UseDecrement() != FALSE);
		break;
	case 2:
		SendWeaponAnim(MP5TIGER_SHOOT2, UseDecrement() != FALSE);
		break;
	case 3:
		SendWeaponAnim(MP5TIGER_SHOOT3, UseDecrement() != FALSE);
		break;
	}

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

	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usFireMP5TIGER, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, (int)(m_pPlayer->pev->punchangle.x * 100), (int)(m_pPlayer->pev->punchangle.y * 100), FALSE, FALSE);

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

void CMP5TIGER::Reload(void)
{
	if (m_pPlayer->ammo_9mm <= 0)
		return;

	if (DefaultReload(MP5N_MAX_CLIP, MP5TIGER_RELOAD, 2.63))
	{
#ifndef CLIENT_DLL
		m_pPlayer->SetAnimation(PLAYER_RELOAD);
#endif
		m_flAccuracy = 0;
		m_iShotsFired = 0;
	}
}

void CMP5TIGER::WeaponIdle(void)
{
	ResetEmptySound();
	m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20;
	SendWeaponAnim(MP5TIGER_IDLE1, UseDecrement() != FALSE);
}