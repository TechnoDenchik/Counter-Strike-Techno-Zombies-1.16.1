/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "knife_twin_axes.h"
#include "gamemode/interface/interface_const.h"
#include "weapons/WeaponTemplate.hpp"


#define KNIFE_BODYHIT_VOLUME 128
#define KNIFE_WALLHIT_VOLUME 512

LINK_ENTITY_TO_CLASS(weapon_twinaxes, CSTwinShadowAxes)

enum twinaxes
{
	ANIM_IDLE,
	ANIM_IDLE2,

	ANIM_DRAW,

	ANIM_SLASH1,
	ANIM_SLASH2,
	ANIM_SLASH3,
	ANIM_SLASH4,

	ANIM_DUAL_SLASH1,
	ANIM_DUAL_SLASH2,

	ANIM_COMBO,

	ANIM_SKILL1,
	ANIM_SKILL2,
};

enum twin_summon
{
	ANIM_START,
	ANIM_END,

	ANIM_START_SUMMON,

	ANIM_LOOP_SUMMON,
	ANIM_END_SUMMON
};

void CSTwinShadowAxes::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_twinaxes");
	
	Precache();
	
	m_iId = WEAPON_KNIFE;
	//SET_MODEL(ENT(pev), "models/p_dgaxe_a.mdl");

	m_iWeaponState &= ~WPNSTATE_SHIELD_DRAWN;
	pev->skin = 0;
	
	FallInit();
}

void CSTwinShadowAxes::Precache(void)
{
	PRECACHE_MODEL("models/v_dgaxe.mdl");
	PRECACHE_MODEL("models/v_dgaxe_3.mdl");
	PRECACHE_MODEL("models/dgaxe_summon.mdl");


	PRECACHE_MODEL("sprites/ef_dgaxe_change.spr");
	PRECACHE_MODEL(Beam_SPR);
	m_iModelLight1 = PRECACHE_MODEL("sprites/muzzleflash203.spr");

	PRECACHE_SOUND("weapons/dgaxe_combo.wav");
	PRECACHE_SOUND("weapons/dgaxe_draw.wav");
	PRECACHE_SOUND("weapons/dgaxe_dualslash1.wav");
	PRECACHE_SOUND("weapons/dgaxe_dualslash2.wav");
	PRECACHE_SOUND("weapons/dgaxe_idle.wav");
	PRECACHE_SOUND("weapons/dgaxe_ready.wav");
	PRECACHE_SOUND("weapons/dgaxe_skill1.wav");
	PRECACHE_SOUND("weapons/dgaxe_skill1_exp.wav");
	PRECACHE_SOUND("weapons/dgaxe_skill2.wav");
	PRECACHE_SOUND("weapons/dgaxe_skill2_exp.wav");
	PRECACHE_SOUND("weapons/dgaxe_slash_hit1.wav");
	PRECACHE_SOUND("weapons/dgaxe_slash_hit2.wav");
	PRECACHE_SOUND("weapons/dgaxe_slash_hit3.wav");
	PRECACHE_SOUND("weapons/dgaxe_slash1.wav");
	PRECACHE_SOUND("weapons/dgaxe_slash2.wav");
	PRECACHE_SOUND("weapons/dgaxe_slash3.wav");
	PRECACHE_SOUND("weapons/dgaxe_slash4.wav");
	PRECACHE_SOUND("weapons/dgaxe_wall_metal1.wav");
	PRECACHE_SOUND("weapons/dgaxe_wall_metal2.wav");
	PRECACHE_SOUND("weapons/dgaxe_wall_stone1.wav");
	PRECACHE_SOUND("weapons/dgaxe_wall_stone2.wav");

	m_usKnife = PRECACHE_EVENT(1, "events/twinaxes.sc");
}

int CSTwinShadowAxes::GetItemInfo(ItemInfo* p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->pszAmmo3 = "TwinAmmo";
	p->iMaxAmmo3 = 100;
	p->pszAmmoGrenade = NULL;
	p->iMaxAmmoGrenade = -1;
	p->iMaxClip = TWINSHADOWAXES;
	p->iSlot = 2;
	p->iPosition = 0;
	p->iId = WEAPON_KNIFE;
	p->iFlags = 0;
	p->iWeight = TWINAXES_WEIGHT;
	p->iFlags = ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE;
	return 1;
}

BOOL CSTwinShadowAxes::Deploy(void)
{
	
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/dgaxe_draw.wav", VOL_NORM , ATTN_NORM);
	m_fMaxSpeed = 250;
	m_iSwing = 0;
	m_iSwing2 = 0;
	m_iSwing3 = 0;
	m_iSwing4 = 0;
	m_iSwing5 = 0;
	m_iSwing6 = 0;
	m_iSwing7 = 0;
	m_iSwing8 = 0;
	m_iSwing9 = 0;
	m_iSwing10 = 0;
	m_iSwing11 = 0;
	m_iWeaponState &= ~WPNSTATE_SHIELD_DRAWN;
	m_pPlayer->m_bShieldDrawn = false;
	if(setskin == true)
		return DefaultDeploy("models/v_dgaxe_3.mdl", "models/p_dgaxe_a.mdl", ANIM_DRAW, "knife", UseDecrement() != FALSE);
	else
		return DefaultDeploy("models/v_dgaxe.mdl", "models/p_dgaxe_a.mdl", ANIM_DRAW, "knife", UseDecrement() != FALSE);
#ifndef CLIENT_DLL
	MESSAGE_BEGIN(MSG_ONE, gmsgArbalestMsg, NULL);
	WRITE_BYTE(WPN_ARBALEST);
	WRITE_BYTE(0);
	MESSAGE_END();
#endif
}

void CSTwinShadowAxes::Holster(int skiplocal)
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.8;
}

void CSTwinShadowAxes::WeaponAnimation(int iAnimation)
{
	int flags;
#ifdef CLIENT_WEAPONS
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL(flags, ENT(m_pPlayer->pev), m_usKnife, 0, (float*)&g_vecZero, (float*)&g_vecZero, 0, 0, iAnimation, 2, 3, 4);
}

void FindHullIntersection3(const Vector& vecSrc, TraceResult& tr, float* pflMins, float* pfkMaxs, edict_t* pEntity)
{
	TraceResult trTemp;
	float flDistance = 2000000;
	float* pflMinMaxs[2] = { pflMins, pfkMaxs };
	Vector vecHullEnd = tr.vecEndPos;

	vecHullEnd = vecSrc + ((vecHullEnd - vecSrc) * 2);
	TRACE_LINE(vecSrc, vecHullEnd, dont_ignore_monsters, pEntity, &trTemp);

	if (trTemp.flFraction < 1)
	{
		tr = trTemp;
		return;
	}

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				Vector vecEnd;
				vecEnd.x = vecHullEnd.x + pflMinMaxs[i][0];
				vecEnd.y = vecHullEnd.y + pflMinMaxs[j][1];
				vecEnd.z = vecHullEnd.z + pflMinMaxs[k][2];

				TRACE_LINE(vecSrc, vecEnd, dont_ignore_monsters, pEntity, &trTemp);

				if (trTemp.flFraction < 1)
				{
					float flThisDistance = (trTemp.vecEndPos - vecSrc).Length();

					if (flThisDistance < flDistance)
					{
						tr = trTemp;
						flDistance = flThisDistance;
					}
				}
			}
		}
	}
}

void CSTwinShadowAxes::PrimaryAttack(void)
{
	if (setkombo == true)
	{
		kombo(TRUE);
		pev->nextthink = UTIL_WeaponTimeBase() + 1.0;
	}
	else if (m_pPlayer->m_rgAmmo[m_iKnifeAmmoType] > 50)
	{
		if (m_pPlayer->m_rgAmmo[m_iKnifeAmmoType] > 50)
		{
			if (setskill2 == true)
			{
				
				pev->nextthink = UTIL_WeaponTimeBase() + 1.0;
			}
		}
	}
	else
	{ 	
		Swing(TRUE);
		pev->nextthink = UTIL_WeaponTimeBase() + 1.0;
	}

}

void CSTwinShadowAxes::GiveSummon()
{

#ifndef CLIENT_DLL
	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	Vector vecSrcA = m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 10 + gpGlobals->v_right * 5;
	Vector vecSrcB = m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 10 + -gpGlobals->v_right * (-5);
	CTwinSummon* pEnt = static_cast<CTwinSummon*>(CBaseEntity::Create("twinsummlonexp", vecSrcA, m_pPlayer->pev->v_angle, ENT(m_pPlayer->pev)));
	if (pEnt)
	{
		pEnt->Init(gpGlobals->v_forward * 2000, GetDamage_SpearA(), GetDamage_SpearB(), GetDamage_SpearC(), 210, m_pPlayer->m_iTeam);
	}
#endif

	m_pPlayer->m_rgAmmo[m_iKnifeAmmoType] -= 50;

}

void CSTwinShadowAxes::SecondaryAttack(void)
{
	Stab(TRUE);
	if(setskill1 == true)
	{ 
		setkombo = false;
	}
	else
		setkombo = true;
	

	pev->nextthink = UTIL_WeaponTimeBase() + 1.0;
}

void CSTwinShadowAxes::Smack(void)
{
	DecalGunshot(&m_trHit, BULLET_PLAYER_CROWBAR, false, m_pPlayer->pev, false);
}

void CSTwinShadowAxes::GetSkin()
{
	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/dgaxe_ready.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
	SendWeaponAnim(ANIM_DRAW, UseDecrement() != FALSE);
}

void CSTwinShadowAxes::WeaponIdle(void)
{

	m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);
	
	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;
	
	switch ((m_iSwing10++) % 2)
	{
		case 0:	SendWeaponAnim(ANIM_IDLE, UseDecrement() != FALSE); break;
		case 1:	SendWeaponAnim(ANIM_IDLE2, UseDecrement() != FALSE); break;	
	}
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 15;
	#ifndef CLIENT_DLL
		MESSAGE_BEGIN(MSG_ALL, gmsgTwinAxesMsg, NULL, m_pPlayer->pev);
		WRITE_BYTE(TWINAXESMSG);
		MESSAGE_END();
	#endif // !CLIENT_DLL
	
}

int CSTwinShadowAxes::Swing(int fFirst)
{
	BOOL fDidHit = FALSE;
	UTIL_MakeVectors(m_pPlayer->pev->v_angle);
	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecEnd = vecSrc + gpGlobals->v_forward * 48;

	TraceResult tr;
	UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, ENT(m_pPlayer->pev), &tr);

	if (tr.flFraction >= 1)
	{
		UTIL_TraceHull(vecSrc, vecEnd, dont_ignore_monsters, head_hull, ENT(m_pPlayer->pev), &tr);

		if (tr.flFraction < 1)
		{
			CBaseEntity* pHit = CBaseEntity::Instance(tr.pHit);

			if (!pHit || pHit->IsBSPModel())
				FindHullIntersection3(vecSrc, tr, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX, ENT(m_pPlayer->pev));

			vecEnd = tr.vecEndPos;
		}
	}

	if (tr.flFraction >= 1)
	{
		if (fFirst)
		{
			if (m_pPlayer->HasShield() == false)
			{
				switch ((m_iSwing5++) % 2)
				{
					case 0: SendWeaponAnim(ANIM_DUAL_SLASH1, UseDecrement() != FALSE); break;
					case 1: SendWeaponAnim(ANIM_DUAL_SLASH2, UseDecrement() != FALSE); break;
				}

				m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.9;
				m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.9;
			}
			else
			{
				switch ((m_iSwing5++) % 2)
				{
					case 0: SendWeaponAnim(ANIM_DUAL_SLASH1, UseDecrement() != FALSE); break;
					case 1: SendWeaponAnim(ANIM_DUAL_SLASH2, UseDecrement() != FALSE); break;
				}
				m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.9;
				m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.9;
			}

			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2;

			switch ((m_iSwing7++) % 2)
			{
				case 0: EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/dgaxe_dualslash1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);break;
			
				case 1: EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/dgaxe_dualslash2.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM); break;
			
			}

#ifndef CLIENT_DLL
			m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif
		}
	}
	else
	{
		fDidHit = TRUE;

		if (m_pPlayer->HasShield() == false)
		{
			switch ((m_iSwing4++) % 2)
			{
				case 0: SendWeaponAnim(ANIM_DUAL_SLASH1, UseDecrement() != FALSE); break;
				case 1: SendWeaponAnim(ANIM_DUAL_SLASH2, UseDecrement() != FALSE); break;
			}

			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.9;
			m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.9;
		}
		else
		{
			switch ((m_iSwing12++) % 2)
			{
				case 0: SendWeaponAnim(ANIM_DUAL_SLASH1, UseDecrement() != FALSE); break;
				case 1: SendWeaponAnim(ANIM_DUAL_SLASH2, UseDecrement() != FALSE); break;
			}
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.9;
			m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.9;
		}

		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2;

		CBaseEntity* pEntity = CBaseEntity::Instance(tr.pHit);

#ifndef CLIENT_DLL
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif
	
		float flDamage = 15;
		if (m_flNextPrimaryAttack + 0.9 < UTIL_WeaponTimeBase())
			flDamage = 20;

#ifndef CLIENT_DLL
		if (g_pModRunning->DamageTrack() == DT_ZB)
			flDamage *= 900.5f;
		else if (g_pModRunning->DamageTrack() == DT_ZBS)
			flDamage *= 500.5f;


		hit_result_t iCallBack = KnifeAttack1(vecSrc, gpGlobals->v_forward, flDamage, 400, 100, DMG_NEVERGIB | DMG_BULLET, m_pPlayer->pev, m_pPlayer->pev, FALSE);
#endif

	//	ClearMultiDamage();
		if (pEntity)
		{
			float flDamage = 15;
			if (m_flNextPrimaryAttack + 0.9 < UTIL_WeaponTimeBase())
				flDamage = 20;

#ifndef CLIENT_DLL
			if (g_pModRunning->DamageTrack() == DT_ZB)
				flDamage *= 900.5f;
			else if (g_pModRunning->DamageTrack() == DT_ZBS)
				flDamage *= 500.5f;
#endif

		//	pEntity->TraceAttack(m_pPlayer->pev, flDamage, gpGlobals->v_forward, &tr, DMG_NEVERGIB | DMG_BULLET);
		}
		//ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);

		float flVol = 1;
#ifndef CLIENT_DLL
		int fHitWorld = TRUE;
#endif

		if (pEntity)
		{
			if (pEntity->Classify() != CLASS_NONE && pEntity->Classify() != CLASS_MACHINE)
			{
				switch ((m_iSwing8++) % 3)
				{
					case 0: EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/dgaxe_slash_hit1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM); break;
					case 1: EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/dgaxe_slash_hit2.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM); break;
					case 2: EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/dgaxe_slash_hit3.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM); break;
				}

				m_pPlayer->m_iWeaponVolume = KNIFE_BODYHIT_VOLUME;

				if (!pEntity->IsAlive())
					return TRUE;

				flVol = 0.1;
#ifndef CLIENT_DLL
				fHitWorld = FALSE;
#endif
			}
		}
		
#ifndef CLIENT_DLL

		switch ((m_iSwing9++) % 2)
		{
		case 0: EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/dgaxe_dualslash1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM); break;

		case 1: EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/dgaxe_dualslash2.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM); break;

		}
		if (fHitWorld)
		{
			/*if (TEXTURETYPE_PlaySound(&tr, vecSrc, vecSrc + (vecEnd - vecSrc) * 2, CHAR_TEX_METAL))
			{
				if(RANDOM_LONG(0,1))
					 EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/dgaxe_wall_metal1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
				else
					 EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/dgaxe_wall_metal2.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
			}else*/
			
				if (RANDOM_LONG(0, 1))
					EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/dgaxe_wall_stone1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
				else
					EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/dgaxe_wall_stone2.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
		}
#endif

		m_trHit = tr;
		m_pPlayer->m_iWeaponVolume = flVol * KNIFE_WALLHIT_VOLUME;

		//SetThink(&CSTwinShadowAxes::Smack);
		pev->nextthink = UTIL_WeaponTimeBase() + 0.2;
	}

	return fDidHit;
}

void CSTwinShadowAxes::FlyingThink()
{

}

bool CSTwinShadowAxes::SummonDamage(Vector vecSrc, entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, float flRadius, int iClassIgnore, int bitsDamageType)
{
	return 1;
}

void CSTwinShadowAxes::FlyingTouch(CBaseEntity* pOther)
{
	
}

void CSTwinShadowAxes::ItemPostFrame()
{
	int usableButtons = m_pPlayer->pev->button;
	TraceResult tr;
	if ((usableButtons & (IN_RELOAD)))
	{
		UTIL_MakeVectors(m_pPlayer->pev->v_angle);
		Vector vecSrc = m_pPlayer->GetGunPosition();

#ifndef CLIENT_DLL

		Vector vecPlayerOrigin;
		vecPlayerOrigin = m_pPlayer->pev->origin;

		CBaseEntity* pEntity = NULL;
		while ((pEntity = UTIL_FindEntityInSphere(pEntity, vecPlayerOrigin, 8192)) != NULL)
		{
			if (pEntity->pev->takedamage != DAMAGE_NO)
			{
				if (pEntity->pev == m_pPlayer->pev)
					continue;

				if (pEntity->IsBSPModel())
					continue;

				if (pEntity->pev->solid == SOLID_TRIGGER)
					continue;

				if (pEntity->pev->solid == SOLID_NOT)
					continue;

				if (pEntity->IsPlayer())
				{
					int iAnim = RANDOM_LONG(0, 1);
					hit_result_t iCallBack = KnifeAttack1(vecSrc, gpGlobals->v_forward, 0, 2000, 100, DMG_NEVERGIB | DMG_BULLET, m_pPlayer->pev, m_pPlayer->pev, iAnim == 1);
					m_pPlayer->SetAnimation(PLAYER_ATTACK2);
					PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usKnife, 0.0, (float*)&g_vecZero, (float*)&g_vecZero, 2000, float(iAnim), iAnim, 2, iCallBack == HIT_PLAYER, m_iClip > 0);


					pev->iuser3 = iAnim;
				}
			}
		}
#endif
	

		pev->iuser1 = 0;
	}
	else
	{
		if (m_pPlayer->m_rgAmmo[m_iKnifeAmmoType] < 50)
		{
			setskill1 = false;
			setskin2 = false;
		}
	}

	if (gpGlobals->time - tWorldTime2 < 99.0f)
	{
		tDelta2 += gpGlobals->time - tWorldTime2;
	}
	if (tNextAttack2 > 0.3f || (gpGlobals->time - tWorldTime2 > 0.3f) || tDelta2 > 0.3f)
	{
		tNextAttack2 = 0.0f;
		tDelta2 = 0.0f;

		if (m_pPlayer->m_rgAmmo[m_iKnifeAmmoType] < 100)
		{
			m_pPlayer->m_rgAmmo[m_iKnifeAmmoType]++;
		}

		if (m_pPlayer->m_rgAmmo[m_iKnifeAmmoType] == 50)
		{
			GetSkin();
		}
	}
	tWorldTime2 = gpGlobals->time;
	
	return CBasePlayerWeapon::ItemPostFrame();
}

#ifndef CLIENT_DLL

hit_result_t CSTwinShadowAxes::KnifeAttack1(Vector vecSrc, Vector vecDir, float flDamage, float flRadius, float flAngleDegrees, int bitsDamageType,
	entvars_t* pevInflictor, entvars_t* pevAttacker, BOOL iAnim)
{
	TraceResult tr;
	hit_result_t result = HIT_NONE;

	vecSrc.z += 1;

	if (!pevAttacker)
		pevAttacker = pevInflictor;

	Vector vecEnd = vecSrc + vecDir.Normalize() * flRadius;
	UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, ENT(pevAttacker), &tr);

	if (tr.flFraction >= 1) {
		UTIL_TraceHull(vecSrc, vecEnd, dont_ignore_monsters, head_hull, ENT(pevAttacker), &tr);

		if (tr.flFraction < 1) {
			CBaseEntity* pHit = CBaseEntity::Instance(tr.pHit);

			if (!pHit || pHit->IsBSPModel()) {
				FindHullIntersection(vecSrc, tr, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX, ENT(pevAttacker));
			}

			vecEnd = tr.vecEndPos;
		}
	}

	if (tr.flFraction < 1) 
	{
		CBaseEntity* pHit = CBaseEntity::Instance(tr.pHit);
		if (pHit && pHit->IsBSPModel() && pHit->pev->takedamage != DAMAGE_NO) {
			const float flAdjustedDamage = flDamage;
			ClearMultiDamage();
			pHit->TraceAttack(pevInflictor, flAdjustedDamage, (tr.vecEndPos - vecSrc).Normalize(), &tr, bitsDamageType);
			ApplyMultiDamage(pevInflictor, pevAttacker);
		}

		float flVol = 1;
		BOOL fHitWorld = TRUE;
		if (pHit && pHit->Classify() != CLASS_NONE && pHit->Classify() != CLASS_MACHINE) {
			flVol = 0.1f;
			fHitWorld = FALSE;
		}

		if (fHitWorld) {
			TEXTURETYPE_PlaySound(&tr, vecSrc, vecSrc + (vecEnd - vecSrc) * 2, BULLET_PLAYER_CROWBAR);

			setskill1 = false;
			result = HIT_WALL;
		}
	}

	//if (m_pPlayer->m_rgAmmo[m_iKnifeAmmoType] > 50)
	//{
	//	Skill1(TRUE);
	//	setskill1 = true;
	//	setskin2 = true;
	//	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.15f;
	//	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.5f;
	//}

	CBaseEntity* pEntity = nullptr;
	while ((pEntity = UTIL_FindEntityInSphere(pEntity, vecSrc, flRadius)) != nullptr) {
		if (pEntity->pev->takedamage != DAMAGE_NO) {
			if (pEntity->IsBSPModel())
				continue;

			if (pEntity->pev == pevAttacker)
				continue;

			Vector vecSpot = pEntity->BodyTarget(vecSrc);
			vecSpot.z = vecEnd.z;
			UTIL_TraceLine(vecSrc, vecSpot, missile, ENT(pevInflictor), &tr);

			if (AngleBetweenVectors(tr.vecEndPos - vecSrc, vecDir) > flAngleDegrees)
				continue;

			if (tr.flFraction == 1.0f || tr.pHit == pEntity->edict()) {
				if (tr.fStartSolid) {
					tr.vecEndPos = vecSrc;
					tr.flFraction = 0;
				}

				if (tr.flFraction == 1.0f) {
					pEntity->TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
				}

				Vector vecRealDir = (tr.vecEndPos - vecSrc).Normalize();

				ClearMultiDamage();
				pEntity->TraceAttack(pevInflictor, flDamage, vecRealDir, &tr, bitsDamageType);
				ApplyMultiDamage(pevInflictor, pevAttacker);


				

				PLAYBACK_EVENT_FULL(0, ENT(pEntity->pev), m_usKnife, 0.0, (float*)&g_vecZero, (float*)&g_vecZero, 0.0, 0.0, 0, 1, iAnim, m_iClip > 0);

				result = HIT_PLAYER;

			}
		}
	}

	return result;
}
#endif // !CLIENT_DLL

int CSTwinShadowAxes::Stab(int fFirst)
{
	BOOL fDidHit = FALSE;
	UTIL_MakeVectors(m_pPlayer->pev->v_angle);
	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecEnd = vecSrc + gpGlobals->v_forward * 32;

	TraceResult tr;
	UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, ENT(m_pPlayer->pev), &tr);
	
	if (tr.flFraction >= 1)
	{
		UTIL_TraceHull(vecSrc, vecEnd, dont_ignore_monsters, head_hull, ENT(m_pPlayer->pev), &tr);

		if (tr.flFraction < 1)
		{
			CBaseEntity* pHit = CBaseEntity::Instance(tr.pHit);

			if (!pHit || pHit->IsBSPModel())
				FindHullIntersection3(vecSrc, tr, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX, ENT(m_pPlayer->pev));

			vecEnd = tr.vecEndPos;
		}
	}

	if (tr.flFraction >= 1)
	{
		if (fFirst)
		{
			switch ((m_iSwing2++) % 4)
			{
				case 0: SendWeaponAnim(ANIM_SLASH1, UseDecrement() != FALSE); break;
				case 1: SendWeaponAnim(ANIM_SLASH2, UseDecrement() != FALSE); break;
				case 2: SendWeaponAnim(ANIM_SLASH3, UseDecrement() != FALSE); break;
				case 3: SendWeaponAnim(ANIM_SLASH4, UseDecrement() != FALSE); break;
			}
			

			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.7;
			m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.1645;

			switch ((m_iSwing3++) % 4)
			{
				case 0: EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/dgaxe_slash1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM); break;

				case 1: EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/dgaxe_slash2.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM); break;

				case 2: EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/dgaxe_slash3.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM); break;

				case 3: EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/dgaxe_slash4.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM); break;
			}
#ifndef CLIENT_DLL
			m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif
		}
	}
	else
	{
		fDidHit = TRUE;

#ifndef CLIENT_DLL
		switch ((m_iSwing4++) % 4)
		{
			case 0: SendWeaponAnim(ANIM_SLASH1, UseDecrement() != FALSE); break;
			case 1: SendWeaponAnim(ANIM_SLASH2, UseDecrement() != FALSE); break;
			case 2: SendWeaponAnim(ANIM_SLASH3, UseDecrement() != FALSE); break;
			case 3: SendWeaponAnim(ANIM_SLASH4, UseDecrement() != FALSE); break;
		}
#endif
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.7;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.1645;

		CBaseEntity* pEntity = CBaseEntity::Instance(tr.pHit);

#ifndef CLIENT_DLL
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif
		float flDamage = 65.0;

		if (pEntity && pEntity->IsPlayer())
		{
			Vector2D vec2LOS;
			Vector vecForward = gpGlobals->v_forward;

			// ???
			UTIL_MakeVectors(pEntity->pev->angles);

			vec2LOS = vecForward.Make2D();
			vec2LOS = vec2LOS.Normalize();

			if (DotProduct(vec2LOS, gpGlobals->v_forward.Make2D()) > 0.8)
				flDamage *= 3.0;
		}



		UTIL_MakeVectors(m_pPlayer->pev->v_angle);

		if (m_flNextPrimaryAttack + 0.9 < UTIL_WeaponTimeBase())
			flDamage = 20;

#ifndef CLIENT_DLL
		if (g_pModRunning->DamageTrack() == DT_ZB)
			flDamage *= 900.5f;
		else if (g_pModRunning->DamageTrack() == DT_ZBS)
			flDamage *= 500.5f;


		hit_result_t iCallBack = KnifeAttack1(vecSrc, gpGlobals->v_forward, flDamage, 400, 100, DMG_NEVERGIB | DMG_BULLET, m_pPlayer->pev, m_pPlayer->pev, FALSE);
#endif

		float flVol = 1;
#ifndef CLIENT_DLL
		int fHitWorld = TRUE;
#endif
		if (pEntity)
		{
			if (pEntity->Classify() != CLASS_NONE && pEntity->Classify() != CLASS_MACHINE)
			{
				switch (RANDOM_LONG(0, 2))
				{
					case 0: EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/dgaxe_slash_hit1.wav", VOL_NORM, ATTN_NORM, 0 ,PITCH_NORM); break;
					case 1: EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/dgaxe_slash_hit2.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM); break;
					case 2: EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/dgaxe_slash_hit3.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM); break;
				}
				m_pPlayer->m_iWeaponVolume = KNIFE_BODYHIT_VOLUME;

				if (!pEntity->IsAlive())
					return TRUE;

				flVol = 0.1;
#ifndef CLIENT_DLL
				fHitWorld = FALSE;
#endif
			}
		}

#ifndef CLIENT_DLL
		if (fHitWorld)
		{
		
				if (RANDOM_LONG(0, 1))
					EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/dgaxe_wall_stone1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
				else
					EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/dgaxe_wall_stone2.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);

				switch ((m_iSwing3++) % 4)
				{
				case 0: EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/dgaxe_slash1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM); break;

				case 1: EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/dgaxe_slash2.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM); break;

				case 2: EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/dgaxe_slash3.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM); break;

				case 3: EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/dgaxe_slash4.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM); break;
				}
		}
#endif

		m_trHit = tr;
		m_pPlayer->m_iWeaponVolume = flVol * KNIFE_WALLHIT_VOLUME;

		SetThink(&CSTwinShadowAxes::Smack);
		pev->nextthink = UTIL_WeaponTimeBase() + 0.2;
	}

	return fDidHit;
}

int CSTwinShadowAxes::kombo(int fFirst)
{

	#ifndef CLIENT_DLL
		m_pPlayer->SpawnProtection_Start(3.0f);
	#endif 

	setkombo = false;
	BOOL fDidHit = FALSE;
	UTIL_MakeVectors(m_pPlayer->pev->v_angle);
	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecEnd = vecSrc + gpGlobals->v_forward * 48;

	int v8 = m_waterlevel % 4;
	SendWeaponAnim(v8 + 1);
	m_weaponanim = v8 + 1;



	
	if (m_iuser3)
	{
		if (v8 == m_iuser3 - 1)
			m_iuser3 = (v8 + 2);
		else if (v8 == m_iuser3 - 7)
			m_iuser3 = (v8 + 8);
		else
			m_iuser3 = (0);
	}

	m_waterlevel++;

	TraceResult tr;
	UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, ENT(m_pPlayer->pev), &tr);

	if (tr.flFraction >= 1)
	{
		UTIL_TraceHull(vecSrc, vecEnd, dont_ignore_monsters, head_hull, ENT(m_pPlayer->pev), &tr);

		if (tr.flFraction < 1)
		{
			CBaseEntity* pHit = CBaseEntity::Instance(tr.pHit);

			if (!pHit || pHit->IsBSPModel())
				FindHullIntersection3(vecSrc, tr, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX, ENT(m_pPlayer->pev));

			vecEnd = tr.vecEndPos;
		}
	}

	if (tr.flFraction >= 1)
	{
		if (fFirst)
		{
			if (m_pPlayer->HasShield() == false)
			{
				SendWeaponAnim(ANIM_COMBO, UseDecrement() != FALSE);

				m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 2.0;
				m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 2.0;
			}
			else
			{
				SendWeaponAnim(ANIM_COMBO, UseDecrement() != FALSE);
				m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 2.0;
				m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 2.0;
			}

			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 6;

			
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/dgaxe_combo.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);

			

#ifndef CLIENT_DLL
			m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif
		}
	}
	else
	{
		fDidHit = TRUE;

		if (m_pPlayer->HasShield() == false)
		{
			SendWeaponAnim(ANIM_COMBO, UseDecrement() != FALSE);
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 2.0;
			m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 2.0;
		}
		else
		{
			SendWeaponAnim(ANIM_COMBO, UseDecrement() != FALSE);
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 2.0;
			m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 2.0;
		}
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/dgaxe_combo.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 6;

		CBaseEntity* pEntity = CBaseEntity::Instance(tr.pHit);
		SetPlayerShieldAnim();

#ifndef CLIENT_DLL
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif

		float flDamage = 15;
		if (m_flNextPrimaryAttack + 0.9 < UTIL_WeaponTimeBase())
			flDamage = 20;

#ifndef CLIENT_DLL
		if (g_pModRunning->DamageTrack() == DT_ZB)
			flDamage *= 900.5f;
		else if (g_pModRunning->DamageTrack() == DT_ZBS)
			flDamage *= 500.5f;


		hit_result_t iCallBack = KnifeAttack1(vecSrc, gpGlobals->v_forward, flDamage, 400, 100, DMG_NEVERGIB | DMG_BULLET, m_pPlayer->pev, m_pPlayer->pev, FALSE);
#endif

		float flVol = 1;
#ifndef CLIENT_DLL
		int fHitWorld = TRUE;
#endif

		if (pEntity)
		{
			if (pEntity->Classify() != CLASS_NONE && pEntity->Classify() != CLASS_MACHINE)
			{
				m_pPlayer->m_iWeaponVolume = KNIFE_BODYHIT_VOLUME;

				if (!pEntity->IsAlive())
					return TRUE;
				EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/dgaxe_combo.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
				SendWeaponAnim(ANIM_COMBO, UseDecrement() != FALSE);
				flVol = 0.1;
#ifndef CLIENT_DLL
				fHitWorld = FALSE;
#endif
			}
		}

		m_trHit = tr;
		m_pPlayer->m_iWeaponVolume = flVol * KNIFE_WALLHIT_VOLUME;

		SetThink(&CSTwinShadowAxes::Smack);
		pev->nextthink = UTIL_WeaponTimeBase() + 2.2;
		SetPlayerShieldAnim();
	}

	return fDidHit;
}

int CSTwinShadowAxes::Skill1(int fFirst)
{
	DefaultDeploy("models/v_dgaxe.mdl", "models/p_dgaxe_a.mdl", ANIM_DRAW, "", UseDecrement() != FALSE);
	if (m_pPlayer->m_rgAmmo[m_iKnifeAmmoType] < 50)
	{
		setskin = false;
	}
	
	setskill1 = false;
	BOOL fDidHit = FALSE;
	UTIL_MakeVectors(m_pPlayer->pev->v_angle);
	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecEnd = vecSrc + gpGlobals->v_forward * 48;

	int v8 = m_waterlevel % 4;
	SendWeaponAnim(v8 + 1);
	m_weaponanim = v8 + 1;

	if (m_iuser3)
	{
		if (v8 == m_iuser3 - 1)
			m_iuser3 = (v8 + 2);
		else if (v8 == m_iuser3 - 7)
			m_iuser3 = (v8 + 8);
		else
			m_iuser3 = (0);
	}

	m_waterlevel++;

	TraceResult tr;
	UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, ENT(m_pPlayer->pev), &tr);

	if (tr.flFraction >= 1)
	{
		UTIL_TraceHull(vecSrc, vecEnd, dont_ignore_monsters, head_hull, ENT(m_pPlayer->pev), &tr);

		if (tr.flFraction < 1)
		{
			CBaseEntity* pHit = CBaseEntity::Instance(tr.pHit);

			if (!pHit || pHit->IsBSPModel())
				FindHullIntersection3(vecSrc, tr, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX, ENT(m_pPlayer->pev));

			vecEnd = tr.vecEndPos;
		}
	}

	if (tr.flFraction >= 1)
	{
		if (fFirst)
		{
			if (m_pPlayer->HasShield() == false)
			{
				if (m_pPlayer->m_rgAmmo[m_iKnifeAmmoType] > 50)
				{
					GiveSummon();
					SendWeaponAnim(ANIM_SKILL1, UseDecrement() != FALSE); 
					m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 2.0;
					m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 2.0;
				}
			}
			else
			{
				if (m_pPlayer->m_rgAmmo[m_iKnifeAmmoType] > 50)
				{
					GiveSummon();
					SendWeaponAnim(ANIM_SKILL1, UseDecrement() != FALSE);
					m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 2.0;
					m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 2.0;
				}
			}

			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 6;


			//EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/dgaxe_skill1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
			//EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/dgaxe_skill1_exp.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);

#ifndef CLIENT_DLL
			m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif
		}
	}
	else
	{
		fDidHit = TRUE;

		if (m_pPlayer->HasShield() == false)
		{
			if (m_pPlayer->m_rgAmmo[m_iKnifeAmmoType] > 50)
			{
				GiveSummon();
				SendWeaponAnim(ANIM_SKILL1, UseDecrement() != FALSE);
				m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 2.0;
				m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 2.0;
			}
		}
		else
		{
			if (m_pPlayer->m_rgAmmo[m_iKnifeAmmoType] > 50)
			{
				GiveSummon();
				SendWeaponAnim(ANIM_SKILL1, UseDecrement() != FALSE);
				m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 2.0;
				m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 2.0;
			}
		}

		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 6;

		CBaseEntity* pEntity = CBaseEntity::Instance(tr.pHit);
		SetPlayerShieldAnim();

#ifndef CLIENT_DLL
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif
		ClearMultiDamage();
		if (pEntity)
		{
			float flDamage = 15;
			if (m_flNextPrimaryAttack + 2.0 < UTIL_WeaponTimeBase())
				flDamage = 20;

#ifndef CLIENT_DLL
			if (g_pModRunning->DamageTrack() == DT_ZB)
				flDamage *= 900.5f;
			else if (g_pModRunning->DamageTrack() == DT_ZBS)
				flDamage *= 500.5f;
#endif

			pEntity->TraceAttack(m_pPlayer->pev, flDamage, gpGlobals->v_forward, &tr, DMG_NEVERGIB | DMG_BULLET);
		}
		ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);

		float flVol = 1;
#ifndef CLIENT_DLL
		int fHitWorld = TRUE;
#endif

		if (pEntity)
		{
			if (pEntity->Classify() != CLASS_NONE && pEntity->Classify() != CLASS_MACHINE)
			{
				m_pPlayer->m_iWeaponVolume = KNIFE_BODYHIT_VOLUME;

				if (!pEntity->IsAlive())
					return TRUE;
				if (m_pPlayer->m_rgAmmo[m_iKnifeAmmoType] > 50)
				{
					GiveSummon();
					SendWeaponAnim(ANIM_SKILL1, UseDecrement() != FALSE);
				}
				flVol = 0.1;
#ifndef CLIENT_DLL
				fHitWorld = FALSE;
#endif
			}
		}

		m_trHit = tr;
		m_pPlayer->m_iWeaponVolume = flVol * KNIFE_WALLHIT_VOLUME;

		SetThink(&CSTwinShadowAxes::Smack);
		pev->nextthink = UTIL_WeaponTimeBase() + 2.0;
		SetPlayerShieldAnim();
	}

	return fDidHit;
}

int CSTwinShadowAxes::Skill2(int fFirst)
{
	setskill2 = false;
	BOOL fDidHit = FALSE;
	UTIL_MakeVectors(m_pPlayer->pev->v_angle);
	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecEnd = vecSrc + gpGlobals->v_forward * 48;

	int v8 = m_waterlevel % 4;
	SendWeaponAnim(v8 + 1);
	m_weaponanim = v8 + 1;

	if (m_iuser3)
	{
		if (v8 == m_iuser3 - 1)
			m_iuser3 = (v8 + 2);
		else if (v8 == m_iuser3 - 7)
			m_iuser3 = (v8 + 8);
		else
			m_iuser3 = (0);
	}

	m_waterlevel++;

	TraceResult tr;
	UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, ENT(m_pPlayer->pev), &tr);

	if (tr.flFraction >= 1)
	{
		UTIL_TraceHull(vecSrc, vecEnd, dont_ignore_monsters, head_hull, ENT(m_pPlayer->pev), &tr);

		if (tr.flFraction < 1)
		{
			CBaseEntity* pHit = CBaseEntity::Instance(tr.pHit);

			if (!pHit || pHit->IsBSPModel())
				FindHullIntersection3(vecSrc, tr, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX, ENT(m_pPlayer->pev));

			vecEnd = tr.vecEndPos;
		}
	}

	if (tr.flFraction >= 1)
	{
		if (fFirst)
		{
			if (m_pPlayer->HasShield() == false)
			{
				switch (RANDOM_LONG(0,1))
				{
					case 0:	SendWeaponAnim(ANIM_SKILL2, UseDecrement() != FALSE); break;
					case 1:	
						EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/dgaxe_dualslash1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
						SendWeaponAnim(ANIM_DUAL_SLASH1, UseDecrement() != FALSE); 
						break;
				}
				
				m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 2.0;
				m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 2.0;
			}
			else
			{
				switch (RANDOM_LONG(0, 1))
				{
				case 0:	SendWeaponAnim(ANIM_SKILL2, UseDecrement() != FALSE); break;
				case 1:
					EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/dgaxe_dualslash1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
					SendWeaponAnim(ANIM_DUAL_SLASH1, UseDecrement() != FALSE);
					break;
				}
				m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 2.0;
				m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 2.0;
			}

			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 6;

			
				EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/dgaxe_skill2.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
				 EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/dgaxe_skill2_exp.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);

#ifndef CLIENT_DLL
			m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif
		}
	}
	else
	{
		fDidHit = TRUE;

		if (m_pPlayer->HasShield() == false)
		{
			switch (RANDOM_LONG(0, 1))
			{
			case 0:	SendWeaponAnim(ANIM_SKILL2, UseDecrement() != FALSE); break;
			case 1:
				EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/dgaxe_dualslash1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
				SendWeaponAnim(ANIM_DUAL_SLASH1, UseDecrement() != FALSE);
				break;
			}
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 2.0;
			m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 2.0;
		}
		else
		{
			switch (RANDOM_LONG(0, 1))
			{
			case 0:	SendWeaponAnim(ANIM_SKILL2, UseDecrement() != FALSE); break;
			case 1:
				EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/dgaxe_dualslash1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
				SendWeaponAnim(ANIM_DUAL_SLASH1, UseDecrement() != FALSE);
				break;
			}
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 2.0;
				m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 2.0;
		}

		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 6;

		CBaseEntity* pEntity = CBaseEntity::Instance(tr.pHit);
		SetPlayerShieldAnim();

#ifndef CLIENT_DLL
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif
		ClearMultiDamage();
		if (pEntity)
		{
			float flDamage = 15;
			if (m_flNextPrimaryAttack + 0.9 < UTIL_WeaponTimeBase())
				flDamage = 20;

#ifndef CLIENT_DLL
			if (g_pModRunning->DamageTrack() == DT_ZB)
				flDamage *= 900.5f;
			else if (g_pModRunning->DamageTrack() == DT_ZBS)
				flDamage *= 500.5f;
#endif

			pEntity->TraceAttack(m_pPlayer->pev, flDamage, gpGlobals->v_forward, &tr, DMG_NEVERGIB | DMG_BULLET);
		}
		ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);

		float flVol = 1;
#ifndef CLIENT_DLL
		int fHitWorld = TRUE;
#endif

		if (pEntity)
		{
			if (pEntity->Classify() != CLASS_NONE && pEntity->Classify() != CLASS_MACHINE)
			{
				m_pPlayer->m_iWeaponVolume = KNIFE_BODYHIT_VOLUME;

				if (!pEntity->IsAlive())
					return TRUE;
				switch (RANDOM_LONG(0, 1))
				{
				case 0:	SendWeaponAnim(ANIM_SKILL2, UseDecrement() != FALSE); break;
				case 1:
					EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/dgaxe_dualslash1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
					SendWeaponAnim(ANIM_DUAL_SLASH1, UseDecrement() != FALSE);
					break;
				}
				flVol = 0.1;
#ifndef CLIENT_DLL
				fHitWorld = FALSE;
#endif
			}
		}

		m_trHit = tr;
		m_pPlayer->m_iWeaponVolume = flVol * KNIFE_WALLHIT_VOLUME;

		SetThink(&CSTwinShadowAxes::Smack);
		pev->nextthink = UTIL_WeaponTimeBase() + 2.2;
		SetPlayerShieldAnim();
	}

	return fDidHit;
}

int CSTwinShadowAxes::ExtractAmmo(CBasePlayerWeapon* pWeapon)
{
	if (TwinAmmo)
	{
		m_pPlayer->m_rgAmmo[m_iKnifeAmmoType] = TwinAmmo;
		TwinAmmo = 0;
	}
	return CBasePlayerWeapon::ExtractAmmo(pWeapon);
}
