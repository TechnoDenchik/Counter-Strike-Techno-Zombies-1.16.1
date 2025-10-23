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

	PRECACHE_SOUND("weapons/sbmine_shoot_ready.wav");
	PRECACHE_SOUND("weapons/sbmine_shoot02.wav");
	PRECACHE_SOUND("weapons/sbmine_timeout.wav");
	PRECACHE_SOUND("weapons/sbmine_trigger_draw.wav");
	PRECACHE_SOUND("weapons/sbmine_trigger_shoot.wav");
	

	m_usCreateExplosion = PRECACHE_EVENT(1, "events/createexplo.sc");
}

int CSBMINE::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->pszAmmo3 = NULL;
	p->iMaxAmmo3 = -1;
	p->pszAmmoGrenade = "HEGrenade";
	p->iMaxAmmoGrenade = MAX_AMMO_HEGRENADE;
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
	m_fMaxSpeed = 250;
	m_iWeaponState &= ~WPNSTATE_SHIELD_DRAWN;
	phs2 = -1;
	phs3 = -1;
	phs4 = -1;
	phs12 = -1;
	phs13 = 3;
	return DefaultDeploy("models/v_sbmine.mdl", "models/p_sbmine.mdl", SBMINE_DRAW, "grenade", UseDecrement() != FALSE);
}

void CSBMINE::Holster(int skiplocal)
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;

	if (!m_pPlayer->m_rgAmmo[m_iGrenadeAmmoType])
	{
		m_pPlayer->pev->weapons &= ~(1 << WEAPON_HEGRENADE);
		DestroyItem();
	}

	phs2 = -1;
	phs3 = -1;
	phs4 = -1;
	phs12 = -1;
	phs13 = 3;
}

void CSBMINE::PrimaryAttack(void)
{
	if (phs2 > 0.0f)
		phs2 = -1.0f; // 0xBF800000
	bool v6 = gpGlobals->time > phs12 + 1.0f;

	if (phs3 == -1.0f)
	{
		phs14 = 3;
		SendWeaponAnim(SBMINE_READY_START, UseDecrement() != FALSE);
		phs3 = gpGlobals->time + 0.43;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 99999.0;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 99999.0;
	}

	int flags;
#ifdef CLIENT_WEAPONS
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	if (v6)
		phs12 = v6 = gpGlobals->time;
	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.12f;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.9f;
}

void CSBMINE::SecondaryAttack(void)
{
	
}

void CSBMINE::WeaponIdle(void)
{
	ResetEmptySound();
	m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20;
	SendWeaponAnim(SBMINE_IDLE, UseDecrement() != FALSE);
}

void CSBMINE::ItemPostFrame()
{
	int flags;
#ifdef CLIENT_WEAPONS
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	if (phs4 > 0.0f && gpGlobals->time > phs4)
	{
		phs4 = -1;

		return;
	}

	if (phs2 <= 0.0f)
	{
		if (phs3 > 0.0f)
		{
			if (this->m_pPlayer->pev->button & IN_ATTACK && this->m_pPlayer->m_rgAmmo[m_iGrenadeAmmoType] > 0)
			{
				if (gpGlobals->time > phs3)
				{
					this->SendWeaponAnim(SBMINE_READY_IDLE, UseDecrement() != FALSE); // 4
					phs3 = gpGlobals->time + 10.0f;

					return CBasePlayerWeapon::ItemPostFrame();
				}
			}
			else
			{
				//EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "weapons/wondercannon_comd_shoot.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
				
				m_pPlayer->m_rgAmmo[m_iGrenadeAmmoType]--;

				if (gpGlobals->time - tWorldTime11 < 1.0f)
				{
					tDelta11 += gpGlobals->time - tWorldTime11;
				}

				if (tNextAttack11 > 1.0f || (gpGlobals->time - tWorldTime11 > 1.0f) || tDelta11 > 1.0f)	//可以多射一次
				{
					tNextAttack11 = 0.0f;
					tDelta11 = 0.0f;

					phs14--;

					static int iLastCountDown = -1;
					int iCountDown = phs14;

					if (iCountDown > 0)
					{
						if (iCountDown != iLastCountDown)
						{
							iLastCountDown = iCountDown;

							if (iCountDown == 3)
							{
								this->SendWeaponAnim(SBMINE_SHOOT_START, UseDecrement() != FALSE); // 5
							}

							if (iCountDown == 1)
							{
								this->SendWeaponAnim(SBMINE_SHOOT_END, UseDecrement() != FALSE); // 5

								phs3 = -1;
								m_flNextPrimaryAttack = m_flNextSecondaryAttack = m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.7f;
								phs12 = -1;
								phs13 = 3;

								UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
								CBaseEntity* pevAttacker = this->m_pPlayer;
								Vector vecSrc = m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 10;
#ifndef CLIENT_DLL
								CSBMineGet* pEnt = static_cast<CSBMineGet*>(CBaseEntity::Create("weaponn_sbmine_get", vecSrc, pev->angles, ENT(m_pPlayer->pev)));
								if (pEnt)
								{
									pEnt->Init(m_pPlayer, gpGlobals->v_forward * 700);
								}
#endif

								shootmine = true;
							}
						}
					}
				}
				tWorldTime11 = gpGlobals->time;


				if (shootmine == true)
				{
			
				}
			}
		}
	}

	return CBasePlayerWeapon::ItemPostFrame();
}