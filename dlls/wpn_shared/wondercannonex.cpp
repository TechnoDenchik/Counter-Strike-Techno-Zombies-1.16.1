/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "wondercannonex.h"
#include "gamemode/interface/interface_const.h"
#include "util/u_range.hpp"
#include "weapons/WeaponTemplate.hpp"

#ifndef CLIENT_DLL
#include "effects.h"
#include "customentity.h"
#include "monsters.h"
#endif

enum wondercannonex_e
{
	ANIM_IDLE,
	ANIM_RELOAD,
	ANIM_DRAW,
	ANIM_SHOOT1,
	ANIM_SHOOT2,
	ANIM_SHOOT3,
	ANIM_BMODE_ON,
	ANIM_CMODE_START,
	ANIM_CMODE_IDLE,
	ANIM_CMODE_SHOOT,
};

LINK_ENTITY_TO_CLASS(weapon_wondercannonex, CWonderCannonEx)

void CWonderCannonEx::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_wondercannonex");

	Precache();
	m_iId = WEAPON_AK47;
	SET_MODEL(ENT(pev), "models/w_wondercannonex.mdl");

	m_iDefaultAmmo = 30;
	m_flAccuracy = 0.2;
	m_iShotsFired = 0;
	WonderAmmo = 3;
	m_fireuse2 = false;
	WonderExp = 0;
	FallInit();
}

void CWonderCannonEx::Precache(void)
{
	PRECACHE_MODEL("models/v_wondercannonex.mdl");
	PRECACHE_MODEL("models/w_wondercannonex.mdl");

	PRECACHE_MODEL("models/s_wondercannonex.mdl");
	PRECACHE_MODEL("models/ef_wondercannonex_area.mdl");
	PRECACHE_MODEL("models/d_wondercannonex.mdl");
	PRECACHE_MODEL("models/bomb_wondercannonex.mdl");

	PRECACHE_SOUND("weapons/wondercannonex_bmod_exp.wav");
	PRECACHE_SOUND("weapons/wondercannon_bomd_exp.wav");
	PRECACHE_SOUND("weapons/wondercannonex_cmod_exp.wav");
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
	PRECACHE_SOUND("weapons/wondercannonex-1.wav");

	PRECACHE_MODEL("sprites/ef_wondercannon_bomb_ex.spr");
	PRECACHE_MODEL("sprites/ef_wondercannonex_bomb_set.spr");
	PRECACHE_MODEL("sprites/ef_wondercannonex_chain.spr");
	PRECACHE_MODEL("sprites/ef_wondercannon_hit.spr");
	PRECACHE_MODEL("sprites/ef_wondercannonex_hit1.spr");
	PRECACHE_MODEL("sprites/ef_wondercannonex_hit2.spr");
	PRECACHE_MODEL("sprites/ef_wondercannonex_hit3.spr");
	PRECACHE_MODEL("sprites/ef_wondercannonex_hit4.spr");

	PRECACHE_MODEL("sprites/muzzleflash255.spr");

	m_iShell = PRECACHE_MODEL("models/rshell.mdl");
	m_usFireWonderCannon = PRECACHE_EVENT(1, "events/wondercannonex.sc");
}

int CWonderCannonEx::GetItemInfo(ItemInfo* p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "WonderCannonExAmmo";
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

BOOL CWonderCannonEx::Deploy(void)
{
	m_flAccuracy = 0.2;
	m_iShotsFired = 0;
	iShellOn = 1;
	return DefaultDeploy("models/v_wondercannonex.mdl", "models/p_wondercannonex.mdl", ANIM_DRAW, "ak47", UseDecrement() != FALSE);
}

void CWonderCannonEx::Holster(int skiplocal)
{
	ClearEffect();
	DestroyEffect();

	// clear target list ?
	return CBasePlayerWeapon::Holster(skiplocal);
}

void CWonderCannonEx::PrimaryAttack_FindTargets()
{
	const float flRadius = 350;

	phs9_10_11.clear();
#ifndef CLIENT_DLL
	CBaseEntity* pEntity = NULL;
	while ((pEntity = UTIL_FindEntityInSphere(pEntity, m_pPlayer->pev->origin, flRadius)) != nullptr)
	{
		if (PrimaryAttack_CheckTargetAvailable(pEntity, m_pPlayer->pev->v_angle))
		{
			EHANDLE eh;
			eh.Set(pEntity->edict());
			phs9_10_11.push_back(eh);
		}
	}
#endif
}

bool CWonderCannonEx::PrimaryAttack_CheckTargetAvailable(CBaseEntity* a2, Vector vecAngleDirection)
{
	const float flRadius = 350;

	if (!a2->IsAlive())
		return false;

#ifndef CLIENT_DLL
	if (g_pGameRules->PlayerRelationship(m_pPlayer, a2) == GR_TEAMMATE)
		return false;
#endif

	if (a2->pev == m_pPlayer->pev)
		return false;

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecDelta = (a2->Center() - vecSrc).Normalize();

	UTIL_MakeVectors(vecAngleDirection);

	if (DotProduct(gpGlobals->v_forward, vecDelta) < 0.5)
		return false;

	return true;
}

void CWonderCannonEx::DestroyEffect()
{
#ifndef CLIENT_DLL
	for (CBeam* p : phs5_6_7)
	{
		if (p)
			p->SUB_Remove();
	}
#endif
}

void CWonderCannonEx::ClearEffect()
{
#ifndef CLIENT_DLL
	for (CBeam* pBeam : phs5_6_7)
	{
		if (pBeam)
		{
			pBeam->SetBrightness(0);
			pev->effects |= EF_NODRAW; // 0x80
		}
	}
#endif
	phs9_10_11.clear();
}

void CWonderCannonEx::Getsprite()
{
	const float flRadius = 350;

	phs9_10_11.clear();
#ifndef CLIENT_DLL
	CBaseEntity* pEntity = NULL;
	while ((pEntity = UTIL_FindEntityInSphere(pEntity, m_pPlayer->pev->origin, flRadius)) != nullptr)
	{
		if (PrimaryAttack_CheckTargetAvailable(pEntity, m_pPlayer->pev->v_angle))
		{
			EHANDLE eh;
			eh.Set(pEntity->edict());
			phs9_10_11.push_back(eh);
		}
	}
#endif
}

#ifndef CLIENT_DLL
void CWonderCannonEx::RadiusDamage2()
{
	BOOL fDidHit = FALSE;
	UTIL_MakeVectors(m_pPlayer->pev->v_angle);
	Vector vecSrc = m_pPlayer->GetGunPosition();

	if (phs9_10_11.empty())
	{
		ClearEffect();
		if (!phs5_6_7.empty())
		{
			// ???
		}
		return;
	}

	size_t v8 = 0;
	for (CBaseEntity* pEntity : phs9_10_11)
	{
		if (v8 >= 6)
			break;
		if (!pEntity)
			continue;
		Vector vecDirection = (pEntity->pev->origin - m_pPlayer->pev->origin).Normalize();

		TraceResult tr;
		UTIL_TraceLine(m_pPlayer->pev->origin, pEntity->pev->origin, dont_ignore_monsters, ENT(m_pPlayer->pev), &tr);

		ClearMultiDamage();
		pEntity->TraceAttack(m_pPlayer->pev, 400, vecDirection, &tr, DMG_BULLET);
		ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);


		if (v8 < 6)
		{
#ifndef CLIENT_DLL

			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/wondercannonex_bmod_exp.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);

			if (WonderExp < 14)
			{
				if (WonderExp == 1)
				{
					MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY, pEntity->pev->origin);
				 	WRITE_BYTE(TE_EXPLOSION);
					WRITE_COORD(pEntity->pev->origin.x);
					WRITE_COORD(pEntity->pev->origin.y);
					WRITE_COORD(pEntity->pev->origin.z);
					WRITE_SHORT(MODEL_INDEX("sprites/ef_wondercannonex_hit1.spr"));
					WRITE_BYTE(5);
					WRITE_BYTE(40);
					WRITE_BYTE(TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NOSOUND);
					MESSAGE_END();

					MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY, pEntity->pev->origin);
					WRITE_BYTE(TE_EXPLOSION);
					WRITE_COORD(pEntity->pev->origin.x);
					WRITE_COORD(pEntity->pev->origin.y);
					WRITE_COORD(pEntity->pev->origin.z);
					WRITE_SHORT(MODEL_INDEX("sprites/ef_wondercannonex_hit1.spr"));
					WRITE_BYTE(5);
					WRITE_BYTE(40);
					WRITE_BYTE(TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NOSOUND);
					MESSAGE_END();
				}
				else
				{

					switch ((m_iSwing13++) % 2)
					{

					case 0:
						MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY, pEntity->pev->origin);
						WRITE_BYTE(TE_EXPLOSION);
						WRITE_COORD(pEntity->pev->origin.x);
						WRITE_COORD(pEntity->pev->origin.y);
						WRITE_COORD(pEntity->pev->origin.z);
						WRITE_SHORT(MODEL_INDEX("sprites/ef_wondercannonex_hit4.spr"));
						WRITE_BYTE(6);
						WRITE_BYTE(40);
						WRITE_BYTE(TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NOSOUND);
						MESSAGE_END();

						MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY, pEntity->pev->origin);
						WRITE_BYTE(TE_EXPLOSION);
						WRITE_COORD(pEntity->pev->origin.x);
						WRITE_COORD(pEntity->pev->origin.y);
						WRITE_COORD(pEntity->pev->origin.z);
						WRITE_SHORT(MODEL_INDEX("sprites/ef_wondercannonex_hit4.spr"));
						WRITE_BYTE(6);
						WRITE_BYTE(40);
						WRITE_BYTE(TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NOSOUND);
						MESSAGE_END();			
						break;

					case 1:
						MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY, pEntity->pev->origin);
						WRITE_BYTE(TE_EXPLOSION);
						WRITE_COORD(pEntity->pev->origin.x);
						WRITE_COORD(pEntity->pev->origin.y);
						WRITE_COORD(pEntity->pev->origin.z);
						WRITE_SHORT(MODEL_INDEX("sprites/ef_wondercannonex_hit3.spr"));
						WRITE_BYTE(6);
						WRITE_BYTE(40);
						WRITE_BYTE(TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NOSOUND);
						MESSAGE_END();

						MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY, pEntity->pev->origin);
						WRITE_BYTE(TE_EXPLOSION);
						WRITE_COORD(pEntity->pev->origin.x);
						WRITE_COORD(pEntity->pev->origin.y);
						WRITE_COORD(pEntity->pev->origin.z);
						WRITE_SHORT(MODEL_INDEX("sprites/ef_wondercannonex_hit3.spr"));
						WRITE_BYTE(6);
						WRITE_BYTE(40);
						WRITE_BYTE(TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NOSOUND);
						MESSAGE_END();
						break;
					}
					
				}
			}
			else
			{
				switch ((m_iSwing14++) % 2)
				{

				case 0:
					MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY, pEntity->pev->origin);
					WRITE_BYTE(TE_EXPLOSION);
					WRITE_COORD(pEntity->pev->origin.x);
					WRITE_COORD(pEntity->pev->origin.y);
					WRITE_COORD(pEntity->pev->origin.z);
					WRITE_SHORT(MODEL_INDEX("sprites/ef_wondercannonex_hit2.spr"));
					WRITE_BYTE(6);
					WRITE_BYTE(40);
					WRITE_BYTE(TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NOSOUND);
					MESSAGE_END();

					MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY, pEntity->pev->origin);
					WRITE_BYTE(TE_EXPLOSION);
					WRITE_COORD(pEntity->pev->origin.x);
					WRITE_COORD(pEntity->pev->origin.y);
					WRITE_COORD(pEntity->pev->origin.z);
					WRITE_SHORT(MODEL_INDEX("sprites/ef_wondercannonex_hit2.spr"));
					WRITE_BYTE(6);
					WRITE_BYTE(40);
					WRITE_BYTE(TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NOSOUND);
					MESSAGE_END();
					break;

				case 1:
					MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY, pEntity->pev->origin);
					WRITE_BYTE(TE_EXPLOSION);
					WRITE_COORD(pEntity->pev->origin.x);
					WRITE_COORD(pEntity->pev->origin.y);
					WRITE_COORD(pEntity->pev->origin.z);
					WRITE_SHORT(MODEL_INDEX("sprites/ef_wondercannonex_hit1.spr"));
					WRITE_BYTE(6);
					WRITE_BYTE(40);
					WRITE_BYTE(TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NOSOUND);
					MESSAGE_END();

					MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY, pEntity->pev->origin);
					WRITE_BYTE(TE_EXPLOSION);
					WRITE_COORD(pEntity->pev->origin.x);
					WRITE_COORD(pEntity->pev->origin.y);
					WRITE_COORD(pEntity->pev->origin.z);
					WRITE_SHORT(MODEL_INDEX("sprites/ef_wondercannonex_hit1.spr"));
					WRITE_BYTE(6);
					WRITE_BYTE(40);
					WRITE_BYTE(TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NOSOUND);
					MESSAGE_END();
					break;
				}
			}
#endif
			++v8;
		}
	}

}
#endif	

void CWonderCannonEx::PrimaryAttack(void)
{
	if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		WonderCannonFire(0.04 + (0.4) * m_flAccuracy, 0.5, FALSE);
	else if (m_pPlayer->pev->velocity.Length2D() > 140)
		WonderCannonFire(0.04 + (0.07) * m_flAccuracy, 0.5, FALSE);
	else
		WonderCannonFire((0.0275), 0.5, FALSE);
	PrimaryAttack_FindTargets();
}

void CWonderCannonEx::SecondaryAttack(void)
{
	//if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
	//	WonderCannonFire2(0.04 + (0.4) * m_flAccuracy, 0.0955, FALSE);
	//else if (m_pPlayer->pev->velocity.Length2D() > 140)
	//	WonderCannonFire2(0.04 + (0.07) * m_flAccuracy, 0.0955, FALSE);
	//else
	//	WonderCannonFire2((0.0275), 0.0955, FALSE);
}

void CWonderCannonEx::WonderCannonFire(float flSpread, float flCycleTime, BOOL fUseAutoAim)
{
	m_bDelayFire = true;
	m_iShotsFired++;
	m_flAccuracy = ((float)(m_iShotsFired * m_iShotsFired * m_iShotsFired) / 200.0) + 0.35;

	if (m_flAccuracy > 1.25)
		m_flAccuracy = 1.25;

	m_fireuse2 = true;

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

	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/wondercannonex-1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);

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

//	m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
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

void CWonderCannonEx::WonderCannonFire2(float flSpread, float flCycleTime, BOOL fUseAutoAim)
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
	//m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
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

void CWonderCannonEx::Reload(void)
{
	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		if (DefaultReload(30, ANIM_RELOAD, 2.25))
		{
#ifndef CLIENT_DLL
			m_pPlayer->SetAnimation(PLAYER_RELOAD);
#endif
			m_flAccuracy = 0.2;
			m_iShotsFired = 0;
			m_bDelayFire = false;
		}
	}
}

void CWonderCannonEx::ItemPostFrame()
{
	int usableButtons = m_pPlayer->pev->button;

	int flags;
#ifdef CLIENT_WEAPONS
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif
	if (gpGlobals->time - tWorldTime5 < 99.0f)
	{
		tDelta5 += gpGlobals->time - tWorldTime5;
	}

	if (tNextAttack5 > 0.3f || (gpGlobals->time - tWorldTime5 > 0.3f) || tDelta5 > 0.3f)	//可以多射一次
	{
		tNextAttack5 = 0.0f;
		tDelta5 = 0.0f;
	
	}
	tWorldTime5 = gpGlobals->time;

		if (m_fireuse2 == true)
		{
			if (gpGlobals->time - tWorldTime3 < 99.0f)
			{
				tDelta3 += gpGlobals->time - tWorldTime3;
			}
		
			if (tNextAttack3 > 0.3f || (gpGlobals->time - tWorldTime3 > 0.3f) || tDelta3 > 0.3f)	//可以多射一次
			{
				tNextAttack3 = 0.0f;
				tDelta3 = 0.0f;
				if (m_fireuse2 == true)
				{	
					if (gpGlobals->time - tWorldTime4 < 99.0f)
					{
						tDelta4 += gpGlobals->time - tWorldTime4;
					}
					if(WonderExp < 14)
					{ 
					PrimaryAttack_FindTargets();
#ifndef CLIENT_DLL
					WonderExp++;
					RadiusDamage2();
#endif
					}
					if (WonderExp == 14)
					{
						EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/wondercannon_bomd_exp2.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
						WonderExp = 0;
						m_fireuse2 = false;
					}

					/*if (tNextAttack4 > 5.0f || (gpGlobals->time - tWorldTime4 > 5.0f) || tDelta4 > 5.0f)
					{
						tNextAttack4 = 0.0f;
						tDelta4 = 0.0f;
						EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/wondercannon_bomd_exp2.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
						WonderExp = 0;
						m_fireuse2 = false;
					}
					tWorldTime4 = gpGlobals->time;*/
				}
			}

			tWorldTime3 = gpGlobals->time;

			
			
		}
	


	//m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] = 1;

	return CBasePlayerWeapon::ItemPostFrame();
}

void CWonderCannonEx::WeaponIdle(void)
{
	ResetEmptySound();
	m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20;
	SendWeaponAnim(ANIM_IDLE, UseDecrement() != FALSE);
}

int CWonderCannonEx::ExtractAmmo(CBasePlayerWeapon* pWeapon)
{
	if (WonderAmmo)
	{
		m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] = WonderAmmo;
		WonderAmmo = 0;
	}
	return CBasePlayerWeapon::ExtractAmmo(pWeapon);
}