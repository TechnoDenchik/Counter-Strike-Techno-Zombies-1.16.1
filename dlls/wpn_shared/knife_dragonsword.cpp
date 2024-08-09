/*
knife_dragonsword.cpp
Copyright (C) 2019 JustANoobQAQ

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#ifndef CLIENT_DLL
#include "gamemode/mods.h"
#endif

#include "weapons/WeaponTemplate.hpp"
#include "weapons/KnifeAttack.h"


#define KNIFE_BODYHIT_VOLUME 128
#define KNIFE_WALLHIT_VOLUME 512

	class CKnifeDragonsword : public LinkWeaponTemplate<CKnifeDragonsword,
		TGeneralData,
		BuildTGetItemInfoFromCSW<WEAPON_KNIFE>::template type,
		TWeaponIdleDefault,
		TDeployDefault
	>
	{
	public:
		int iAnim = 2;
		static constexpr const char *ClassName = "knife_dragonsword";
		static constexpr const char *V_Model = "models/v_dragonsword.mdl";
		static constexpr const char *P_Model = "models/p_dragonsword.mdl";
		static constexpr const char *W_Model = "models/w_knife.mdl";

		static constexpr int MaxClip = -1;
		static constexpr auto ItemSlot = KNIFE_SLOT;
		static constexpr const char *AnimExtension = "dragonsword";
		KnockbackData KnockBack = { .0f, .0f, .0f, .0f, 1.0f };

	public:

		enum dragonsword_e
		{
			ANIM_IDLE1 = 0,
			ANIM_SLASH1,
			ANIM_SLASH2,
			ANIM_DRAW,
			ANIM_STAB,
			ANIM_STAB_HIT,
		};
		void Precache() override
		{
			Base::Precache();

			PRECACHE_SOUND("weapons/dragonsword_draw.wav");
			PRECACHE_SOUND("weapons/skullaxe_wall.wav");
			PRECACHE_SOUND("weapons/dragonsword_hit1.wav");
			PRECACHE_SOUND("weapons/dragonsword_hit2.wav");
			PRECACHE_SOUND("weapons/dragonsword_idle.wav");
			PRECACHE_SOUND("weapons/dragonsword_slash1.wav");
			PRECACHE_SOUND("weapons/dragonsword_slash2.wav");
			PRECACHE_SOUND("weapons/dragonsword_stab_hit.wav");
		}

		BOOL Deploy() override
		{
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/dragonsword_draw.wav", 0.3, 2.4);

			iAnim = 2;
			m_fMaxSpeed = 250;
			m_iSwing = 0;
			m_iWeaponState &= ~WPNSTATE_SHIELD_DRAWN;
			m_pPlayer->m_bShieldDrawn = false;
			m_flTimeWeaponIdle = 1.5f;
			return Base::Deploy();
		}

		float GetMaxSpeed() override { return m_fMaxSpeed; }
		void PrimaryAttack() override;
		void SecondaryAttack() override;
		BOOL CanDrop() override { return false; }

	public:
		void DelayPrimaryAttack();
		void DelaySecondaryAttack();
		float GetPrimaryAttackDamage() const
		{
			float flDamage = 90;
#ifndef CLIENT_DLL
			if (g_pModRunning->DamageTrack() == DT_ZB)
				flDamage *= 9.5f;
			else if (g_pModRunning->DamageTrack() == DT_ZBS)
				flDamage *= 5.5f;
#endif
			return flDamage;
		}
		float GetSecondaryAttackDamage() const
		{
			float flDamage = 85;
#ifndef CLIENT_DLL
			if (g_pModRunning->DamageTrack() == DT_ZB)
				flDamage *= 9.5f;
			else if (g_pModRunning->DamageTrack() == DT_ZBS)
				flDamage *= 5.5f;
#endif
			return flDamage;
		}
	};

	LINK_ENTITY_TO_CLASS(knife_dragonsword, CKnifeDragonsword)

		void CKnifeDragonsword::SecondaryAttack(void)
	{
#ifndef CLIENT_DLL
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif
		SendWeaponAnim(ANIM_STAB, UseDecrement() != FALSE);

		SetThink(&CKnifeDragonsword::DelaySecondaryAttack);
		pev->nextthink = gpGlobals->time + 0.65f;

		m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.49f;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2;
	}

	void CKnifeDragonsword::PrimaryAttack(void)
	{
#ifndef CLIENT_DLL
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif
		iAnim = iAnim % 2;
		SendWeaponAnim(ANIM_SLASH1 + iAnim, UseDecrement() != FALSE);
		iAnim = iAnim + 1;

		SetThink(&CKnifeDragonsword::DelayPrimaryAttack);
		if (iAnim ==2 )
			pev->nextthink = gpGlobals->time + 0.3f;
		else
			pev->nextthink = gpGlobals->time + 0.6f;
		
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.25f;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2;
	}

	void CKnifeDragonsword::DelaySecondaryAttack()
	{
		BOOL fDidHit = FALSE;
		UTIL_MakeVectors(m_pPlayer->pev->v_angle);
		Vector vecSrc = m_pPlayer->GetGunPosition();
		Vector vecEnd = vecSrc + gpGlobals->v_forward * 48;

		TraceResult tr;
		UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, ENT(m_pPlayer->pev), &tr);

#ifndef CLIENT_DLL
		switch (KnifeAttack3(vecSrc, gpGlobals->v_forward, GetPrimaryAttackDamage(), 95, 120, DMG_NEVERGIB | DMG_BULLET, m_pPlayer->pev, m_pPlayer->pev))
		{
		case HIT_NONE:
		{
			SendWeaponAnim(ANIM_STAB_HIT, UseDecrement() != FALSE);		
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/dragonsword_stab_hit.wav", VOL_NORM, ATTN_NORM, 0, 94);
			break;
		}
		case HIT_PLAYER:
		{
			SendWeaponAnim(ANIM_STAB_HIT, UseDecrement() != FALSE);
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/dragonsword_hit1.wav", VOL_NORM, ATTN_NORM, 0, 94);
			m_pPlayer->m_iWeaponVolume = KNIFE_BODYHIT_VOLUME;
			fDidHit = TRUE;
			break;
		}
		case HIT_WALL:
		{
			SendWeaponAnim(ANIM_STAB_HIT, UseDecrement() != FALSE);
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/skullaxe_wall.wav", VOL_NORM, ATTN_NORM, 0, 94);
			m_pPlayer->m_iWeaponVolume = KNIFE_WALLHIT_VOLUME * 0.5;
			fDidHit = TRUE;
			break;
		}
		}
#endif

		SetThink(nullptr);

		//return fDidHit;
	}

	void CKnifeDragonsword::DelayPrimaryAttack()
	{
		BOOL fDidHit = FALSE;
		UTIL_MakeVectors(m_pPlayer->pev->v_angle);
		Vector vecSrc = m_pPlayer->GetGunPosition();
		Vector vecEnd = vecSrc + gpGlobals->v_forward * 48;

		TraceResult tr;
		UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, ENT(m_pPlayer->pev), &tr);

#ifndef CLIENT_DLL
		switch (KnifeAttack3(vecSrc, gpGlobals->v_forward, GetSecondaryAttackDamage(), 50, 90, DMG_NEVERGIB | DMG_BULLET, m_pPlayer->pev, m_pPlayer->pev))
		{
		case HIT_NONE:
		{
			if (iAnim ==2 )
				EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/dragonsword_slash2.wav", VOL_NORM, ATTN_NORM, 0, 94);
			else
				EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/dragonsword_slash1.wav", VOL_NORM, ATTN_NORM, 0, 94);
			break;
		}
		case HIT_PLAYER:
		{
			if (iAnim == 2)
				EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/dragonsword_hit2.wav", VOL_NORM, ATTN_NORM, 0, 94);
			else
				EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/dragonsword_hit1.wav", VOL_NORM, ATTN_NORM, 0, 94);
			m_pPlayer->m_iWeaponVolume = KNIFE_BODYHIT_VOLUME;
			fDidHit = TRUE;
			break;
		}
		case HIT_WALL:
		{
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/skullaxe_wall.wav", VOL_NORM, ATTN_NORM, 0, 94);
			m_pPlayer->m_iWeaponVolume = KNIFE_WALLHIT_VOLUME * 0.1;
			fDidHit = TRUE;
			break;
		}
		}
#endif

		SetThink(nullptr);

		//return fDidHit;
	}

