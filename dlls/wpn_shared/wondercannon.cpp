/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "wondercannon.h"
#include "gamemode/interface/interface_const.h"
#include "util/u_range.hpp"
#include "weapons/WeaponTemplate.hpp"

#ifndef CLIENT_DLL
#include "effects.h"
#include "customentity.h"
#include "monsters.h"
#endif

enum wondercannon_e
{
	ANIM_IDLE,
	ANIM_RELOAD,
	ANIM_DRAW,
	ANIM_SHOOT1,
	ANIM_SHOOT2,
	ANIM_SHOOT3,
	ANIM_BMODE_ON,
	ANIM_BMODE_START,
	ANIM_BMODE_IDLE,
	ANIM_BMODE_SHOOT,
};

LINK_ENTITY_TO_CLASS(weapon_wondercannon, CWonderCannon)

void CWonderCannon::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_wondercannon");

	Precache();
	m_iId = WEAPON_AK47;
	SET_MODEL(ENT(pev), "models/w_wondercannon.mdl");

	m_iDefaultAmmo = 30;
	m_flAccuracy = 0.2;
	m_iShotsFired = 0;
	WonderAmmo = 3;
	phs13 = 3;
	FallInit();
}

void CWonderCannon::Precache(void)
{
	PRECACHE_MODEL("models/v_wondercannon.mdl");
	PRECACHE_MODEL("models/p_wondercannon.mdl");
	PRECACHE_MODEL("models/w_wondercannon.mdl");

	PRECACHE_MODEL("models/ef_wondercannon_area.mdl");
	PRECACHE_MODEL("models/d_wondercannon.mdl");
	PRECACHE_MODEL("models/bomb_wondercannon.mdl");

	PRECACHE_SOUND("weapons/wondercannon_bmod_exp2.wav");
	PRECACHE_SOUND("weapons/wondercannon_bomd_exp.wav");
	PRECACHE_SOUND("weapons/wondercannon_bomd_exp2.wav");
	PRECACHE_SOUND("weapons/wondercannon_bomd_on.wav");
	PRECACHE_SOUND("weapons/wondercannon_bomd_on_exp.wav");
	PRECACHE_SOUND("weapons/wondercannon_clipin1.wav");
	PRECACHE_SOUND("weapons/wondercannon_clipout1.wav");
	PRECACHE_SOUND("weapons/wondercannon_cmod_charging.wav");
	PRECACHE_SOUND("weapons/wondercannon_comd_drop.wav");
	PRECACHE_SOUND("weapons/wondercannon_comd_exp.wav");
	PRECACHE_SOUND("weapons/wondercannon_comd_shoot.wav");
	PRECACHE_SOUND("weapons/wondercannon_comd_start.wav");
	PRECACHE_SOUND("weapons/wondercannon_draw.wav");
	PRECACHE_SOUND("weapons/wondercannon_idle.wav");
	PRECACHE_SOUND("weapons/wondercannon-1.wav");

	PRECACHE_MODEL("sprites/ef_wondercannon_bomb_ex.spr");
	PRECACHE_MODEL("sprites/ef_wondercannon_bomb_set.spr");
	PRECACHE_MODEL("sprites/ef_wondercannon_chain.spr");
	PRECACHE_MODEL("sprites/ef_wondercannon_hit.spr");
	PRECACHE_MODEL("sprites/ef_wondercannon_hit1.spr");
	PRECACHE_MODEL("sprites/ef_wondercannon_hit2.spr");
	PRECACHE_MODEL("sprites/ef_wondercannon_hit3.spr");
	PRECACHE_MODEL("sprites/ef_wondercannon_hit4.spr");

	PRECACHE_MODEL("sprites/muzzleflash111.spr");
	PRECACHE_MODEL("sprites/muzzleflash112.spr");
	PRECACHE_MODEL("sprites/muzzleflash113.spr");

	m_iShell = PRECACHE_MODEL("models/rshell.mdl");
	m_usFireWonderCannon = PRECACHE_EVENT(1, "events/wondercannon.sc");
	m_usFire2WonderCannon = PRECACHE_EVENT(1, "events/wondercannon2.sc");
}

int CWonderCannon::GetItemInfo(ItemInfo* p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "WonderCannonAmmo";
	p->iMaxAmmo1 = 400;
	p->pszAmmo2 = "WonderMine";
	p->iMaxAmmo2 = 0;
	p->pszAmmo3 = NULL;
	p->iMaxAmmo3 = -1;
	p->pszAmmoGrenade = NULL;
	p->iMaxAmmoGrenade = -1;
	p->iMaxClip = 30;
	p->iSlot = 0;
	p->iPosition = 1;
	p->iId = m_iId = WEAPON_AK47;
	p->iFlags = 0;
	p->iWeight = 5;

	return 1;
}

BOOL CWonderCannon::Deploy(void)
{
	m_flAccuracy = 0.2;
	m_iShotsFired = 0;
	iShellOn = 1;
	phs2 = -1;
	phs3 = -1;
	phs4 = -1;
	phs12 = -1;
	phs13 = 3;

	return DefaultDeploy("models/v_wondercannon.mdl", "models/p_wondercannon.mdl", ANIM_DRAW, "ak47", UseDecrement() != FALSE);
}

void CWonderCannon::Holster(int skiplocal)
{
	phs2 = -1;
	phs3 = -1;
	phs4 = -1;
	phs12 = -1;
	phs13 = 3;

	return CBasePlayerWeapon::Holster(skiplocal);
}

#ifndef CLIENT_DLL
void CWonderCannon::OnHitTarget(CBaseEntity* pHit)
{
	if (!pHit || !m_pPlayer)
		return;

	CUtlVector<CBaseEntity*>* pEnemyList = new CUtlVector<CBaseEntity*>();
	pEnemyList->AddToTail(pHit);

	CWonderCannonChain* pChain = CWonderCannonChain::Create(0, m_pPlayer->GetGunPosition(), m_pPlayer->pev->angles, m_pPlayer->edict());

	if (pChain)
	{
		pChain->Init(m_pPlayer, pHit, pEnemyList);
	}
	else
	{
		delete pEnemyList;
	}
}
#endif

void CWonderCannon::PrimaryAttack(void)
{
	if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		WonderCannonFire(0.04 + (0.4) * m_flAccuracy, 0.7, FALSE);
	else if (m_pPlayer->pev->velocity.Length2D() > 140)
		WonderCannonFire(0.04 + (0.07) * m_flAccuracy, 0.7, FALSE);
	else
		WonderCannonFire((0.0275), 0.7, FALSE);

	if (phs2 > 0.0f)
		phs2 = -1.0f; // 0xBF800000
	bool v6 = gpGlobals->time > phs12 + 1.0f;
}

void CWonderCannon::SecondaryAttack(void)
{
#ifndef CLIENT_DLL
	
	CBaseEntity* sp = UTIL_FindEntityByClassname(nullptr, "weapon_wondercannon_mine");

	if (sp)
	{
		CWonderCannonMine* bomb = dynamic_cast<CWonderCannonMine*>(sp);
		if (bomb)
		{
			if (WonderBomb == 3)
			{
				phs14 = 3;
				m_firebomb = true;
				SendWeaponAnim(ANIM_BMODE_ON, UseDecrement() != FALSE);
				m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 3.0;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3.0f;
			}
		}
	}
#endif
	
	if (m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] > 0)
	{

		if (phs2 > 0.0f)
			phs2 = -1.0f; // 0xBF800000

		if (gpGlobals->time - tWorldTime6 < 99.0f)
		{
			tDelta6 += gpGlobals->time - tWorldTime6;
		}

		if (tNextAttack6 > 0.5f || (gpGlobals->time - tWorldTime6 > 0.5f) || tDelta6 > 0.5f)
		{
			tNextAttack6 = 0.0f;
			tDelta6 = 0.0f;

			phs13--;

			static int iLastCountDown = -1;
			int iCountDown = phs13;

			if (iCountDown > 0)
			{
				if (iCountDown != iLastCountDown)
				{
					iLastCountDown = iCountDown;

					if (iCountDown == 2)
					{
						EMIT_SOUND_DYN(ENT(pev), CHAN_ITEM, "weapons/wondercannon_cmod_charging.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
					}

					if (iCountDown < 2)
					{
						if (phs3 == -1.0f)
						{

							SendWeaponAnim(ANIM_BMODE_START, UseDecrement() != FALSE);
							phs3 = gpGlobals->time + 0.43;
							m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 99999.0;
							m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 99999.0;
						}

						bool v6 = gpGlobals->time > phs12 + 1.0f;
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
				}
			}
		}
		tWorldTime6 = gpGlobals->time;
	}
}

void CWonderCannon::WonderCannonFire(float flSpread, float flCycleTime, BOOL fUseAutoAim)
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

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);
	Vector vecEnd = vecSrc + vecAiming * 8192;

	TraceResult tr;
	UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, m_pPlayer->edict(), &tr);

	if (tr.flFraction < 1.0 && tr.pHit)
	{
		CBaseEntity* pHit = CBaseEntity::Instance(tr.pHit);

		if (pHit && pHit->pev->takedamage != DAMAGE_NO)
		{
#ifndef CLIENT_DLL
			OnHitTarget(pHit);
#endif
		}
	}

	m_iClip--;
	m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/wondercannon-1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);

	switch (RANDOM_LONG(1,3))
	{

	case 1:
		SendWeaponAnim(ANIM_SHOOT1, UseDecrement() != FALSE);
		break;
		
	case 2:
		
		SendWeaponAnim(ANIM_SHOOT2, UseDecrement() != FALSE);
		break;
		
	case 3:
		SendWeaponAnim(ANIM_SHOOT3, UseDecrement() != FALSE);
		break;
	}


#ifndef CLIENT_DLL
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif
	
	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	Vector vecDir = m_pPlayer->FireBullets3(vecSrc, gpGlobals->v_forward, flSpread, 8192, 2, BULLET_PLAYER_762MM, 36, 0.98, m_pPlayer->pev, FALSE, m_pPlayer->random_seed);

	int flags;
#ifdef CLIENT_WEAPONS
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usFireWonderCannon, 0, (float*)&g_vecZero, (float*)&g_vecZero, vecDir.x, vecDir.y, 0, 0, FALSE, FALSE);

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

void CWonderCannon::WonderCannonFire2(float flSpread, float flCycleTime, BOOL fUseAutoAim)
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

	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usFireWonderCannon, 0, (float*)&g_vecZero, (float*)&g_vecZero, vecDir.x, vecDir.y, 0, 0, FALSE, FALSE);

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

void CWonderCannon::Reload(void)
{
	if (DefaultReload(30, ANIM_RELOAD, 2.35))
	{
#ifndef CLIENT_DLL
		m_pPlayer->SetAnimation(PLAYER_RELOAD);
#endif
		m_flAccuracy = 0.2;
		m_iShotsFired = 0;
		m_bDelayFire = false;
		phs13 = 3;
	}
}

void CWonderCannon::ItemPostFrame()
{
	int usableButtons = m_pPlayer->pev->button;

	int flags;
#ifdef CLIENT_WEAPONS
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif
	
	if (m_firebomb == true)
	{
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

					if (iCountDown == 1)
					{
						m_firebombinit = true;
					}
				}
			}
		}

		tWorldTime11 = gpGlobals->time;
	}

	if (m_firebombinit == true)
	{
#ifndef CLIENT_DLL
		
		CBaseEntity* sp = UTIL_FindEntityByClassname(nullptr, "weapon_wondercannon_mine");

		if (sp)
		{
			CWonderCannonMine* bomb = dynamic_cast<CWonderCannonMine*>(sp);
			if (bomb)
			{
				bomb->BombExp(false, pev->owner, m_pWeapon);
			}
		}

		WonderBomb = 0;
#endif
	}

	if (phs4 > 0.0f && gpGlobals->time > phs4)
	{
		phs4 = -1;

		return;
	}

	if (phs2 <= 0.0f)
	{
		if (phs3 > 0.0f)
		{
			if (this->m_pPlayer->pev->button & IN_ATTACK2 && this->m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] > 0)
			{
				if (gpGlobals->time > phs3)
				{
					this->SendWeaponAnim(ANIM_BMODE_IDLE, UseDecrement() != FALSE); // 4
					phs3 = gpGlobals->time + 10.0f;

					return CBasePlayerWeapon::ItemPostFrame();
				}
			}
			else
			{
				EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "weapons/wondercannon_comd_shoot.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
				this->SendWeaponAnim(ANIM_BMODE_SHOOT, UseDecrement() != FALSE); // 5
				m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType]--;
				phs3 = -1;
				m_flNextPrimaryAttack = m_flNextSecondaryAttack = m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.7f;
				phs12 = -1;
				phs13 = 3;

				UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
				CBaseEntity* pevAttacker = this->m_pPlayer;
				Vector vecSrc = m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 10;
			#ifndef CLIENT_DLL
				CWonderCannonMine* pEnt = static_cast<CWonderCannonMine*>(CWonderCannonMine::Create(vecSrc, pev->angles, pev->owner));
				if (pEnt)
				{
					pEnt->Init(m_pPlayer, gpGlobals->v_forward * 700, m_pWeapon);
				}
			#endif
				WonderBomb++;

			}
		}
	}

	return CBasePlayerWeapon::ItemPostFrame();
}

void CWonderCannon::WeaponIdle(void)
{
	ResetEmptySound();
	m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20;
	SendWeaponAnim(ANIM_IDLE, UseDecrement() != FALSE);
}

int CWonderCannon::ExtractAmmo(CBasePlayerWeapon* pWeapon)
{
	if (WonderAmmo)
	{
		m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] = WonderAmmo;
		WonderAmmo = 0;
	}
	return CBasePlayerWeapon::ExtractAmmo(pWeapon);
}