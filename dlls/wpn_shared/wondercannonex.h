/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#ifndef WPN_WONDERCANNONEX_H
#define WPN_WONDERCANNONEX_H
#ifdef _WIN32
#pragma once
#endif

#define WONDERCANNONEX_MAX_SPEED			221
#define WONDERCANNONEX_DAMAGE			36
#define WONDERCANNONEX_RANGE_MODIFER		0.98
#define WONDERCANNONEX_RELOAD_TIME		2.45

#include <vector>
#include <array>

class CWonderCannonEx : public CBasePlayerWeapon
{
public:
	void Spawn() override;
	void Precache() override;
	int GetItemInfo(ItemInfo* p) override;
	BOOL Deploy() override;
	float GetMaxSpeed() override { return WONDERCANNONEX_MAX_SPEED; }
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
	const char* GetCSModelName() override { return "models/w_wondercannonex.mdl"; }
	KnockbackData GetKnockBackData() override { return { 350.0f, 250.0f, 200.0f, 100.0f, 0.6f }; }
#ifndef CLIENT_DLL
	WeaponBuyAmmoConfig GetBuyAmmoConfig() override { return { "ammo_WonderCannonExAmmo"}; }
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

private:

	duration_t tNextAttack11;
	time_point_t tWorldTime11;
	duration_t tDelta11;

	duration_t tNextAttack6;
	time_point_t tWorldTime6;
	duration_t tDelta6;

	float phs2;
	float phs3; 
	float phs4;
	float phs5;
	float phs12;
	float phs13;
	float phs14;

	unsigned short m_usFireWonderCannon;
	unsigned short m_usFire2WonderCannon;
};

class CWonderCannonExMine : public CGrenade
{
public:
	CWonderCannonExMine()
	{
		m_iState = 0;
		m_iSequence = 0;
		m_flNextAnim = 0;
	}

	static CWonderCannonExMine* Create(int iType, const Vector& vecOrigin, const Vector& vecAngles, edict_t* pentOwner);

	void Spawn(void);
	void Precache(void);
	void EXPORT MineThink();
	void Init(CBasePlayer* pOwner, Vector vecVelocity);
	void Remove();
	void GetSprite();
	void GetModel();
	void Explode(bool IsManual);
	void BombExp(bool IsManual, CBasePlayer* m_pPlayer);
	void DoBombExp(bool IsManual);
	float GetDamage(bool IsManual) const;
public:

	CBasePlayer* m_pOwner;
	CWonderCannonEx* m_pWeapon;
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