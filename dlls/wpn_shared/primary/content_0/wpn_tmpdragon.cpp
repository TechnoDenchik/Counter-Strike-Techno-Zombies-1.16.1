/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "wpn_tmpdragon.h"

enum tmpdragon_e
{
	TMPDRAGON_IDLE1,
	TMPDRAGON_RELOAD,
	TMPDRAGON_DRAW,
	TMPDRAGON_SHOOT1,
	TMPDRAGON_SHOOT2,
	TMPDRAGON_SHOOT3
};

LINK_ENTITY_TO_CLASS(weapon_tmpdragon, CTMPDRAGON)

void CTMPDRAGON::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_tmpdragon");

	Precache();
	m_iId = WEAPON_TMP;
	SET_MODEL(ENT(pev), "models/w_tmpdragon.mdl");

	m_iDefaultAmmo = TMP_DEFAULT_GIVE;
	m_flAccuracy = 0.2;
	m_iShotsFired = 0;
	m_bDelayFire = false;

	FallInit();
}

void CTMPDRAGON::Precache(void)
{
	PRECACHE_MODEL("models/v_tmpdragon.mdl");
	PRECACHE_MODEL("models/p_tmpdragon.mdl");
	PRECACHE_MODEL("models/w_tmpdragon.mdl");

	PRECACHE_SOUND("weapons/tmpdragon_clipin.wav");
	PRECACHE_SOUND("weapons/tmpdragon_clipout.wav");
	PRECACHE_SOUND("weapons/tmp-1.wav");
	PRECACHE_SOUND("weapons/tmp-2.wav");

	m_iShell = PRECACHE_MODEL("models/pshell.mdl");
	m_usFireTMPDRAGON = PRECACHE_EVENT(1, "events/tmpdragon.sc");
}

int CTMPDRAGON::GetItemInfo(ItemInfo *p)
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
	p->iMaxClip = TMP_MAX_CLIP;
	p->iSlot = 0;
	p->iPosition = 11;
	p->iId = m_iId = WEAPON_TMP;
	p->iFlags = 0;
	p->iWeight = TMP_WEIGHT;

	return 1;
}

BOOL CTMPDRAGON::Deploy(void)
{
	m_iShotsFired = 0;
	m_bDelayFire = false;
	m_flAccuracy = 0.2;
	iShellOn = 1;

	return DefaultDeploy("models/v_tmpdragon.mdl", "models/p_tmpdragon.mdl", TMPDRAGON_DRAW, "onehanded", UseDecrement() != FALSE);
}

void CTMPDRAGON::PrimaryAttack(void)
{
	if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		TMPDRAGONFire((0.25) * m_flAccuracy, 0.07, FALSE);
	else
		TMPDRAGONFire((0.03) * m_flAccuracy, 0.07, FALSE);
}

void CTMPDRAGON::TMPDRAGONFire(float flSpread, float flCycleTime, BOOL fUseAutoAim)
{
	m_bDelayFire = true;
	m_iShotsFired++;
	m_flAccuracy = ((float)(m_iShotsFired * m_iShotsFired * m_iShotsFired) / 200) + 0.55;

	if (m_flAccuracy > 1.4)
		m_flAccuracy = 1.4;

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
		EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/tmp-1.wav", VOL_NORM, ATTN_NORM);
		break;
	case 2:
		EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/tmp-2.wav", VOL_NORM, ATTN_NORM);
		break;
	}

	switch (RANDOM_LONG(1, 3))
	{
	case 1:
		SendWeaponAnim(TMPDRAGON_SHOOT1, UseDecrement() != FALSE);
		break;
	case 2:
		SendWeaponAnim(TMPDRAGON_SHOOT2, UseDecrement() != FALSE);
		break;
	case 3:
		SendWeaponAnim(TMPDRAGON_SHOOT3, UseDecrement() != FALSE);
		break;
	}

	m_iClip--;
	m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
#ifndef CLIENT_DLL
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecDir = m_pPlayer->FireBullets3(vecSrc, gpGlobals->v_forward, flSpread, 8192, 1, BULLET_PLAYER_9MM, 20, 0.85, m_pPlayer->pev, FALSE, m_pPlayer->random_seed);

	int flags;
#ifdef CLIENT_WEAPONS
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usFireTMPDRAGON, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, (int)(m_pPlayer->pev->punchangle.x * 100), (int)(m_pPlayer->pev->punchangle.y * 100), 5, FALSE);
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;
#ifndef CLIENT_DLL
	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
#endif
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2;

	if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		KickBack(1.1, 0.5, 0.35, 0.045, 4.5, 3.5, 6);
	else if (m_pPlayer->pev->velocity.Length2D() > 0)
		KickBack(0.8, 0.4, 0.2, 0.03, 3.0, 2.5, 7);
	else if (FBitSet(m_pPlayer->pev->flags, FL_DUCKING))
		KickBack(0.7, 0.35, 0.125, 0.025, 2.5, 2.0, 10);
	else
		KickBack(0.725, 0.375, 0.15, 0.025, 2.75, 2.25, 9);
}

void CTMPDRAGON::Reload(void)
{
	if (m_pPlayer->ammo_9mm <= 0)
		return;

	if (DefaultReload(TMP_MAX_CLIP, TMPDRAGON_RELOAD, 2.12))
	{
#ifndef CLIENT_DLL
		m_pPlayer->SetAnimation(PLAYER_RELOAD);
#endif
		m_flAccuracy = 0.2;
		m_iShotsFired = 0;
	}
}

void CTMPDRAGON::WeaponIdle(void)
{
	ResetEmptySound();
	m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20;
	SendWeaponAnim(TMPDRAGON_IDLE1, UseDecrement() != FALSE);
}