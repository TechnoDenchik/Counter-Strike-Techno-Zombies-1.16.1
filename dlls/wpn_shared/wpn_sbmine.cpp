/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */


#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "wpn_sbmine.h"

enum sbmine_e
{
	SBMINE_IDLE,
	SBMINE_READY_START,
	SBMINE_READY_IDLE,
	SBMINE_SHOOT_START,
	SBMINE_SHOOT_END,
	SBMINE_DRAW,
	SBMINE_TRIGGER_IDLE,
	SBMINE_TRIGGER_START,
	SBMINE_TRIGGER_DRAW,
};

LINK_ENTITY_TO_CLASS(weapon_sbmine, CSBMINE)

void CSBMINE::Spawn(void)
{
	Precache();
	m_iId = WEAPON_HEGRENADE;
	SET_MODEL(ENT(pev), "models/w_sbmine.mdl");

	pev->dmg = 4;
	m_iDefaultAmmo = SBMINE_DEFAULT_GIVE;
	m_flStartThrow = 0;
	m_flReleaseThrow = -1;

	FallInit();
}

void CSBMINE::Precache(void)
{
	PRECACHE_MODEL("models/v_sbmine.mdl");
	PRECACHE_MODEL("models/d_sbmine.mdl");
	PRECACHE_MODEL("models/ef_sbmine.mdl");
	PRECACHE_MODEL("models/ef_sbmine_explosion.mdl");
	PRECACHE_MODEL("models/ef_sbmine_lightning.mdl");
	PRECACHE_MODEL("models/p_sbmine.mdl");
	PRECACHE_MODEL("models/p_sbmine_trigger.mdl");

	PRECACHE_MODEL("sprites/ef_sbmine_debuff.spr");
	PRECACHE_MODEL("sprites/ef_sbmine_explosion.spr");

	PRECACHE_SOUND("weapons/sbmine_beep.wav");
	PRECACHE_SOUND("weapons/sbmine_draw.wav");
	PRECACHE_SOUND("weapons/sbmine_drop.wav");
	PRECACHE_SOUND("weapons/sbmine_exp.wav");
	PRECACHE_SOUND("weapons/sbmine_shoot_ready.wav");
	PRECACHE_SOUND("weapons/sbmine_shoot02.wav");
	PRECACHE_SOUND("weapons/sbmine_timeout.wav");
	PRECACHE_SOUND("weapons/sbmine_trigger_draw.wav");
	PRECACHE_SOUND("weapons/sbmine_trigger_shoot.wav");
	PRECACHE_SOUND("weapons/sbmine_warning.wav");

	m_usCreateExplosion = PRECACHE_EVENT(1, "events/createexplo.sc");
}

int CSBMINE::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "HEGrenade";
	p->iMaxAmmo1 = MAX_AMMO_HEGRENADE;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 3;
	p->iPosition = 1;
	p->iId = m_iId = WEAPON_HEGRENADE;
	p->iWeight = HEGRENADE_WEIGHT;
	p->iFlags = ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE;

	return 1;
}

BOOL CSBMINE::Deploy(void)
{
	m_flReleaseThrow = -1;
	m_fMaxSpeed = 250;
	m_iWeaponState &= ~WPNSTATE_SHIELD_DRAWN;
	return DefaultDeploy("models/v_sbmine.mdl", "models/p_sbmine.mdl", SBMINE_DRAW, "grenade", UseDecrement() != FALSE);
	//EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/sbmine_draw.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
}

BOOL CSBMINE::CanHolster(void)
{
	return m_flStartThrow == 0;
}

void CSBMINE::Holster(int skiplocal)
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;

	if (!m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
	{
		m_pPlayer->pev->weapons &= ~(1 << WEAPON_HEGRENADE);
		DestroyItem();
	}

	m_flStartThrow = 0;
	m_flReleaseThrow = -1;
}

void CSBMINE::PrimaryAttack(void)
{
	if (!m_flStartThrow && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0)
	{
		m_flStartThrow = gpGlobals->time;
		m_flReleaseThrow = 0;
		SendWeaponAnim(SBMINE_READY_START, UseDecrement() != FALSE);
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
	}
}

void CSBMINE::SecondaryAttack(void)
{
	
}

void CSBMINE::WeaponIdle(void)
{
	if (!m_flReleaseThrow && m_flStartThrow)
		m_flReleaseThrow = gpGlobals->time;

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	if (m_flStartThrow)
	{
		Vector angThrow = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;

		if (angThrow.x < 0)
			angThrow.x = -10 + angThrow.x * ((90 - 10) / 90.0);
		else
			angThrow.x = -10 + angThrow.x * ((90 + 10) / 90.0);

		float flVel = (90 - angThrow.x) * 6;

		if (flVel > 750)
			flVel = 750;

		UTIL_MakeVectors(angThrow);
		Vector vecSrc = m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 16;
		Vector vecThrow = gpGlobals->v_forward * flVel + m_pPlayer->pev->velocity;
		float time = 1.5;
		CGrenade::ShootTimedSbmine(m_pPlayer->pev, vecSrc, vecThrow, time, TERRORIST, m_usCreateExplosion);

		SendWeaponAnim(SBMINE_SHOOT_END, UseDecrement() != FALSE);

#ifndef CLIENT_DLL
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif
		m_flStartThrow = 0;
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.75;
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;

		if (!m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
			m_flTimeWeaponIdle = m_flNextSecondaryAttack = m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;

		return;
	}
	else if (m_flReleaseThrow > 0)
	{
		m_flStartThrow = 0;

		if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
		{
			SendWeaponAnim(SBMINE_DRAW, UseDecrement() != FALSE);
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/sbmine_draw.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);

			m_flReleaseThrow = -1;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + RANDOM_FLOAT(10, 15);
		}
		else
			RetireWeapon();

		return;
	}

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
	{
		SendWeaponAnim(SBMINE_IDLE, UseDecrement() != FALSE);
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + RANDOM_FLOAT(10, 15);
	}
}

BOOL CSBMINE::CanDeploy(void)
{
	return m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] != 0;
}
