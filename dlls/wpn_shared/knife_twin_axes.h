/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */
#ifndef KNIFE_TWIN_AXES_H
#define KNIFE_TWIN_AXES_H
#ifdef _WIN32
#pragma once
#endif

#define KNIFE_BODYHIT_VOLUME		128
#define KNIFE_WALLHIT_VOLUME		512
#define KNIFE_MAX_SPEED			250
#define KNIFE_MAX_SPEED_SHIELD		180

#include "weapons/WeaponTemplate.hpp"
#ifndef CLIENT_DLL
#include "effects.h"
#include "customentity.h"
#include "monsters.h"
#include "gamemode/mods.h"
#include "util/u_range.hpp"
#endif

#include <vector>
#include <array>
class CBeam;

class CSTwinShadowAxes : public CBasePlayerWeapon
{
public:
	void Spawn() override;
	void Precache() override;
	void Think() override;
	int GetItemInfo(ItemInfo* p) override;
	BOOL CanDrop() override { return FALSE; }
	BOOL Deploy() override;
	void Skin();
	void Holster(int skiplocal) override;
	float GetMaxSpeed() override { return m_fMaxSpeed; }
	int iItemSlot() override { return KNIFE_SLOT; }
	void Reload() override;
	void FlyingThink();
	void GiveSummon();
	void RadiusDamage();
	void GetSummon();
	void CSTwinShadowAxes::FlyingTouch(CBaseEntity* pOther);
	void GetSkin();
	void PrimaryAttack() override;
	void SecondaryAttack() override;
	void TwinFire2(float flSpread, duration_t flCycleTime, BOOL fUseAutoAim);
	BOOL UseDecrement() override {
#ifdef CLIENT_WEAPONS
		return TRUE;
#else
		return FALSE;
#endif
	}
	bool SummonDamage(Vector vecSrc, entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, float flRadius, int iClassIgnore, int bitsDamageType);
	void WeaponIdle() override;
	bool setkombo;
	bool setammo;
	bool setskin;
	bool setskin2;
	bool setskill1;
	bool setskill2;
	int m_iModelLight1;
	float m_flNumFrames;
	double GetDamage_PrimaryAttack_Instant() const
	{
#ifndef CLIENT_DLL
		if (g_pModRunning->DamageTrack() == DT_ZBS)
			return 1092.0;
		if (g_pModRunning->DamageTrack() == DT_ZB)
			return 304.0;
#endif
		return 20.0;
	}

	double GetDamage_Summon() const
	{
#ifndef CLIENT_DLL
		if (g_pModRunning->DamageTrack() == DT_ZBS)
			return 1000.0;
		if (g_pModRunning->DamageTrack() == DT_ZB)
			return 900.0;
#endif
		return 50.0;
	}
	float GetDamage_ProjectileA() const
	{
#ifndef CLIENT_DLL
		if (g_pModRunning->DamageTrack() == DT_ZBS)
			return 5100.0f;
		if (g_pModRunning->DamageTrack() == DT_ZB)
			return 1100.0f;
#endif
		return 70.0f;
	}
	double GetDamage_ProjectileB() const
	{
#ifndef CLIENT_DLL
		if (g_pModRunning->DamageTrack() == DT_ZBS)
			return 1000.0;
		if (g_pModRunning->DamageTrack() == DT_ZB)
			return 900.0;
#endif
		return 50.0;
	}
	double GetDamage_SpearA() const
	{
#ifndef CLIENT_DLL
		if (g_pModRunning->DamageTrack() == DT_ZBS)
			return 27500.0;
		if (g_pModRunning->DamageTrack() == DT_ZB)
			return 6900.0;
#endif
		return 120.0;
	}
	double GetDamage_SpearB() const
	{
#ifndef CLIENT_DLL
		if (g_pModRunning->DamageTrack() == DT_ZBS)
			return 2500.0;
		if (g_pModRunning->DamageTrack() == DT_ZB)
			return 1500.0;
#endif
		return 80.0;
	}
	double GetDamage_SpearC() const
	{
#ifndef CLIENT_DLL
		if (g_pModRunning->DamageTrack() == DT_ZBS)
			return 4100.0;
		if (g_pModRunning->DamageTrack() == DT_ZB)
			return 2000.0;
#endif
		return 50.0;
	}
	short this_1_has_disconnected;
public:
	void EXPORT Smack();

	NOXREF void WeaponAnimation(int iAnimation);
	int Stab(int fFirst);
	int Swing(int fFirst);
	int kombo(int fFirst);
	int Skill1(int fFirst);
	int Skill2(int fFirst);


public:
	Vector Get_ShootPosition(CBaseEntity* pevAttacker, Vector vecSrc);
	TraceResult m_trHit;
	float GetDamage() const;
	unsigned short m_usKnife;

private:
	int m_iuser1; // iAttackRecord
	int m_iuser3; // iSkillRecord
	int m_iuser4; // iPrimaryAttackRecord
	int m_sequence; // iThinkType
	float m_fuser1; // flNextTimeCanHolster
	int m_weaponanim; // iLastAnim
	int m_waterlevel; // iSecondaryAttackCount

	static constexpr const char* Beam_SPR = "sprites/ef_gungnir_xbeam.spr";
	int ph4;
	int ph5;
	int ph6;
	float ph7;
	float ph8;
	float phs2;	// secondary attack start time
	float phs3; // primary attack start time
	float phs4; // spear attack end time... cannot switch weapon?
	std::array<CBeam*, 3> phs5_6_7; // EHANDLE ?
	// unsigned short phs8; // m_usFireGungnir
	std::vector<EHANDLE> phs9_10_11;
	duration_t tNextAttack;
	time_point_t tWorldTime;
	duration_t tDelta;
	float phs12;
	unsigned short m_usFire;
};









#ifndef CLIENT_DLL

class CTwinSummon : public CBaseAnimating
{
public:
	void Spawn() override
	{
		Precache();
		ph4 = 0;
		ph5 = 0;
		ph6 = 0;

		m_fSequenceLoops = 0;
		//ph26 = 0;
		SetTouch(&CTwinSummon::OnTouch);
		SetThink(&CTwinSummon::FlyThink);

		//ph32 = ?
		pev->solid = SOLID_CUSTOM; // 5
		pev->movetype = MOVETYPE_FLY; // 5
		pev->nextthink = gpGlobals->time + 0.0099999998;
		ph7 = gpGlobals->time + 1.0f;
		ph8 = 300.0;
		this_1_has_disconnected = 0;
		UTIL_SetSize(pev, { -6, -6, -6 }, { 6, 6, 6 });

		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
		WRITE_BYTE(TE_GLOWSPRITE);
		WRITE_SHORT(this->entindex());  // short (entity:attachment to follow)
		WRITE_SHORT(m_iModelLight1);// short (sprite index)
		WRITE_BYTE(20);// byte (life in 0.1's) 
		WRITE_BYTE(30);// byte (line width in 0.1's) 
		WRITE_BYTE(255);// byte,byte,byte (color)
		WRITE_BYTE(255);
		WRITE_BYTE(255);
		WRITE_BYTE(255);// byte (brightness)
		MESSAGE_END();

		/*Vector vecAiming;
		MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
		WRITE_BYTE(TE_EXPLOSION);
		WRITE_COORD(vecAiming[0]);
		WRITE_COORD(vecAiming[1]);
		WRITE_COORD(vecAiming[2]);
		WRITE_SHORT(MODEL_INDEX("sprites/ef_dgaxe_explo.spr"));
		WRITE_BYTE(6);
		WRITE_BYTE(25);
		WRITE_BYTE(TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOSOUND);
		MESSAGE_END();*/
	}

	void Precache() override
	{
		m_iModelExplo = PRECACHE_MODEL("sprites/ef_gungnir_chargeexplo.spr");
		m_iModelLight1 = PRECACHE_MODEL("sprites/ef_gungnir_lightline1.spr");
		m_iModelLight2 = PRECACHE_MODEL("sprites/ef_gungnir_lightline2.spr");
	}

	KnockbackData GetKnockBackData()
	{
		return { 1100.0f, 500.0f, 700.0f, 400.0f, 0.89999998f };
	}

	void EXPORT FlyThink()
	{
		if (gpGlobals->time < ph7)
		{
			this->pev->nextthink = gpGlobals->time + 0.0099999998;
			this_1_m_iSwing = gpGlobals->time;

			if (pev->solid == SOLID_NOT)
			{
				PenetrateEnd();
			}
		}
		else
		{
			TouchWall();
		}
	}

	void EXPORT OnTouch(CBaseEntity* pOther)
	{
		if (gpGlobals->time <= this_1_m_iSwing)
		{
			if (pOther && pOther->IsBSPModel())
			{
				TouchWall();
			}
			else if (pOther->pev->pContainingEntity != this->pev->owner)
			{
				TouchEntity(pOther);
			}
		}
	}

	void TouchWall()
	{
		pev->velocity = {};
		RadiusDamage();
	}

	void TouchEntity(CBaseEntity* pOther)
	{
		if (pev->owner == pOther->edict())
			return;

		CBaseEntity* pAttacker = CBaseEntity::Instance(pev->owner);
		CBasePlayer* pAttackePlayer = nullptr;
		if (pAttacker && pAttacker->IsPlayer())
			pAttackePlayer = static_cast<CBasePlayer*>(pAttacker);

		if (pAttackePlayer &&
			pOther->pev->takedamage != DAMAGE_NO &&
			pOther->IsAlive()
			)
		{
			Vector vecDirection = (pOther->pev->origin - pev->origin).Normalize();

			TraceResult tr;
			UTIL_TraceLine(pev->origin, pOther->pev->origin, missile, ENT(pAttackePlayer->pev), &tr);
			tr.iHitgroup = HITGROUP_CHEST; // ...

			ClearMultiDamage();
			pOther->TraceAttack(pAttackePlayer->pev, m_flTouchDamage, vecDirection, &tr, DMG_BULLET);
			ApplyMultiDamage(pAttackePlayer->pev, pAttackePlayer->pev);

			PenetrateStart();
		}
		pev->velocity = {};
	}

	void PenetrateStart()
	{
		pev->origin = pev->origin + m_vecStartVelocity.Normalize() * 42;
		pev->velocity = {};

		pev->solid = SOLID_NOT;

		pev->nextthink = gpGlobals->time + 0.05f;
	}

	void PenetrateEnd()
	{
		pev->velocity = m_vecStartVelocity;
		pev->solid = SOLID_CUSTOM;

		pev->nextthink = gpGlobals->time + 0.001f;
	}

	void RadiusDamage()
	{
		const float flRadius = m_flExplodeRadius;
		const float flDamage = m_flExplodeDamage;
		const Vector vecSrc = pev->origin;
		entvars_t* const pevAttacker = VARS(pev->owner);
		entvars_t* const pevInflictor = this->pev;
		int bitsDamageType = DMG_BULLET;

		TraceResult tr;
		const float falloff = flRadius ? flDamage / flRadius : 1;
		const int bInWater = (UTIL_PointContents(vecSrc) == CONTENTS_WATER);

		CBaseEntity* pEntity = NULL;
		while ((pEntity = UTIL_FindEntityInSphere(pEntity, vecSrc, flRadius)) != NULL)
		{
			if (pEntity->pev->takedamage != DAMAGE_NO)
			{
				if (bInWater && !pEntity->pev->waterlevel)
					continue;

				if (!bInWater && pEntity->pev->waterlevel == 3)
					continue;

				if (pEntity->IsBSPModel())
					continue;

				if (pEntity->pev == pevAttacker)
					continue;

				Vector vecSpot = pEntity->BodyTarget(vecSrc);
				UTIL_TraceLine(vecSrc, vecSpot, missile, ENT(pevInflictor), &tr);

				if (tr.flFraction == 1.0f || tr.pHit == pEntity->edict())
				{
					if (tr.fStartSolid)
					{
						tr.vecEndPos = vecSrc;
						tr.flFraction = 0;
					}
					float flAdjustedDamage = flDamage - (vecSrc - pEntity->pev->origin).Length() * falloff;
					flAdjustedDamage = Q_max(0, flAdjustedDamage);

					if (tr.flFraction == 1.0f)
					{
						pEntity->TakeDamage(pevInflictor, pevAttacker, flAdjustedDamage, bitsDamageType);
					}
					else
					{
						tr.iHitgroup = HITGROUP_CHEST;
						ClearMultiDamage();
						pEntity->TraceAttack(pevInflictor, flAdjustedDamage, (tr.vecEndPos - vecSrc).Normalize(), &tr, bitsDamageType);
						ApplyMultiDamage(pevInflictor, pevAttacker);
					}

				}
			}
		}

		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
		WRITE_BYTE(TE_GLOWSPRITE);
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z);
		WRITE_SHORT(m_iModelLight2);
		WRITE_BYTE(10);
		WRITE_BYTE(30);
		WRITE_BYTE(TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NOSOUND);
		MESSAGE_END();

		SetThink(&CTwinSummon::AdditionalDamageThink);
		pev->nextthink = gpGlobals->time + 1.1f;
	}

	void EXPORT AdditionalDamageThink()
	{

		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
		WRITE_BYTE(TE_GLOWSPRITE);
		WRITE_SHORT(this->entindex()); // short (start entity) 
		WRITE_COORD(m_vecStartOrigin.x); // coord coord coord (end position) 
		WRITE_COORD(m_vecStartOrigin.y);
		WRITE_COORD(m_vecStartOrigin.z);
		WRITE_SHORT(m_iModelExplo);
		WRITE_BYTE(0); // byte (starting frame) 
		WRITE_BYTE(30); // byte (frame rate in 0.1's) 
		WRITE_BYTE(30); // byte (life in 0.1's) 
		WRITE_BYTE(250); // byte (line width in 0.1's) 
		WRITE_BYTE(0); // byte (noise amplitude in 0.01's) 
		WRITE_BYTE(255); // byte,byte,byte (color)
		WRITE_BYTE(255);
		WRITE_BYTE(255);
		WRITE_BYTE(255); // byte (brightness)
		WRITE_BYTE(40); // byte (scroll speed in 0.1's)
		MESSAGE_END();

		CBaseEntity::FireBullets3(pev->origin, (m_vecStartOrigin - pev->origin).Normalize(), 0.0f, (m_vecStartOrigin - pev->origin).Length(), 9, BULLET_NONE, m_flAdditionalDamage, 1.0f, this->pev, FALSE);
	}

	void Init(Vector vecVelocity, float flTouchDamage, float flExplodeDamage, float flAdditionalDamage, float flExplodeRadius, TeamName iTeam)
	{
		std::tie(m_flTouchDamage, m_flExplodeDamage, m_flAdditionalDamage, m_flExplodeRadius, m_iTeam) = std::make_tuple(flTouchDamage, flExplodeDamage, flAdditionalDamage, flExplodeRadius, iTeam);
		m_vecStartVelocity = pev->velocity = std::move(vecVelocity);
		m_vecStartOrigin = pev->origin;
	}


	int ph4;
	int ph5;
	int ph6;
	float ph7;
	float ph8; // m_pfnThink?
	float this_1_m_iSwing;
	short this_1_has_disconnected;

	float m_flTouchDamage;
	float m_flExplodeDamage;
	float m_flAdditionalDamage;
	float m_flExplodeRadius;
	TeamName m_iTeam;

	Vector m_vecStartOrigin;
	Vector m_vecStartVelocity;
	int m_iModelExplo;
	int m_iModelLight1;
	int m_iModelLight2;

protected:
	void Remove()
	{
		SetThink(nullptr);
		SetTouch(nullptr);
		pev->effects |= EF_NODRAW; // 0x80u
		return UTIL_Remove(this);
	}
};
LINK_ENTITY_TO_CLASS(twin_summon, CTwinSummon)
#endif
#endif