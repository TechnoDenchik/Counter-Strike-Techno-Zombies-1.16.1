/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */
#ifndef WPN_WONDERCANNON_H
#define WPN_WONDERCANNON_H
#ifdef _WIN32
#pragma once
#endif

#define WONDERCANNON_MAX_SPEED			221
#define WONDERCANNON_DAMAGE			36
#define WONDERCANNON_RANGE_MODIFER		0.98
#define WONDERCANNON_RELOAD_TIME		2.45

#define WONDERCANNONEX_MAX_SPEED			221
#define WONDERCANNONEX_DAMAGE			36
#define WONDERCANNONEX_RANGE_MODIFER		0.98
#define WONDERCANNONEX_RELOAD_TIME		2.45

#include <vector>
#include <array>
#include "weapons/KnifeAttack.h"

class CBeam;
class CSprite;

class CWonderCannon : public CBasePlayerWeapon
{
public:
	void Spawn() override;
	void Precache() override;
	int GetItemInfo(ItemInfo* p) override;
	BOOL Deploy() override;
	float GetMaxSpeed() override { return WONDERCANNON_MAX_SPEED; }
	int iItemSlot() override { return PRIMARY_WEAPON_SLOT; }
	bool HasSecondaryAttack() override { return true; }
	void PrimaryAttack() override;
	void SecondaryAttack() override;
	void Reload() override;
	void ItemPostFrame() override;
	void Holster(int skiplocal) override;
	void WeaponIdle() override;
	BOOL UseDecrement() override {
#ifdef CLIENT_WEAPONS
		return TRUE;
#else
		return FALSE;
#endif
	}
	const char* GetCSModelName() override { return "models/w_wondercannon.mdl"; }
	//const char* GetCSGunDropModelName() override { return "models/ef_gundrop.mdl"; }
	KnockbackData GetKnockBackData() override { return { 350.0f, 250.0f, 200.0f, 100.0f, 0.6f }; }
#ifndef CLIENT_DLL
	WeaponBuyAmmoConfig GetBuyAmmoConfig() override { return { "ammo_WonderCannonAmmo"}; }
#endif
public:
	void WonderCannonFire(float flSpread, float flCycleTime, BOOL fUseAutoAim);
	void WonderCannonFire2(float flSpread, float flCycleTime, BOOL fUseAutoAim);
	int ExtractAmmo(CBasePlayerWeapon* pWeapon) override;
	void OnHitTarget(CBaseEntity* pHit);

	int m_iSwing;
	int m_iShell;
	int iShellOn;
	int WonderAmmo;
	int WonderBomb;
	bool m_firebomb;
	bool m_firebombinit;
	int m_iSwing2;

	duration_t tNextAttack11;
	time_point_t tWorldTime11;
	duration_t tDelta11;

private:

	duration_t tNextAttack6;
	time_point_t tWorldTime6;
	duration_t tDelta6;

	CWonderCannon* m_pWeapon;

	float phs2;	// secondary attack start time
	float phs3; // primary attack start time
	float phs4;
	float phs5;// spear attack end time... cannot switch weapon?
	float phs12;
	float phs13;
	float phs14;

	CSprite* m_pEyeGlow;

	unsigned short m_usFireWonderCannon;
	unsigned short m_usFire2WonderCannon;

	CUtlVector<CBaseEntity*>* enemyList = new CUtlVector<CBaseEntity*>();
};





class CWonderCannonChain : public CBaseEntity
{
public:
	enum DamageType
	{
		EXPTYPE_BASE,
		EXPTYPE_SINGLE,
		EXPTYPE_MULTI,
		EXPTYPE_FINAL,
	};

	CWonderCannonChain()
	{
		m_flNextDamage = 0;
		m_tTimeRemove = 0;
		m_iExpTime = 0;
		m_iCount = 0;
		m_iAttachedEntCount = 0;
		m_tTimeRemove = gpGlobals->time + 5.0f;
	}

	static CWonderCannonChain* Create(int iType, const Vector& vecOrigin, const Vector& vecAngles, edict_t* pentOwner);
	void Spawn();
	void Precache();

	void EXPORT OnThink(void);
	void Init(CBasePlayer* pOwner, CBaseEntity* pAttachedEnt, CUtlVector<CBaseEntity*>* pList);
	void DoExpDamage(float flDamage, CBaseEntity* pAttachedEnt);
	void Remove();
	float GetArmorRatioModifier() { return 1.3; }
	KnockbackData GetKnockBackData() { return { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f }; }
	int m_iAttachedEntCount;
	int m_iExpTime;
	int m_iCount;
	int m_iType;
	TeamName m_iTeam;

private:
	float GetDamage(DamageType iType) const;

	time_point_t m_flNextDamage;
	time_point_t m_tTimeRemove;

	int m_iCache_Exp[4];
	int m_iTotalExpCount;
	CUtlVector<CBaseEntity*>* m_pEnemyList;
	CBasePlayer* m_pOwner;
	CBaseEntity* m_pAttachedEnt;
	CWonderCannon* m_pWeapon;
};

class CWonderCannonMine : public CGrenade
{
public:
	CWonderCannonMine()
	{
		m_iState = 0;
		m_iSequence = 0;
		m_flNextAnim = 0;
	}

	static CWonderCannonMine* Create(const Vector& vecOrigin, const Vector& vecAngles, edict_t* pentOwner);

	void Spawn(void);
	void Precache(void);
	void EXPORT MineThink();
	void Init(CBasePlayer* pOwner,Vector vecVelocity, CWonderCannon* pWeapon);
	void Remove();
	void GetSprite();
	void GetModel();
	void Explode(bool IsManual);
	void BombExp(bool IsManual, edict_t* pentOwner, CWonderCannon* pWeapon);
	void DoBombExp(bool IsManual);
	float GetDamage(bool IsManual) const;
public:

	CBasePlayer* m_pOwner;
	CWonderCannon* m_pWeapon;
	int m_iState;
	int m_iType;
private:
	int m_iExp[2];
	int m_iSequence;
	int m_iLifeModel;
	time_point_t m_flNextAnim;

	duration_t tNextAttack7;
	time_point_t tWorldTime7;
	duration_t tDelta7;

	duration_t tNextAttack8;
	time_point_t tWorldTime8;
	duration_t tDelta8;

	bool hasmodel;

};
#endif