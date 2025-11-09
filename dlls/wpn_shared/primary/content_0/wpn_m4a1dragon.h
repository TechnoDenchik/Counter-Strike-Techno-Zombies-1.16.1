/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#ifndef WPN_M4A1DRAGON_H
#define WPN_M4A1DRAGON_H
#ifdef _WIN32
#pragma once
#endif

#define M4A1DRAGON_MAX_SPEED		230
#define M4A1DRAGON_DAMAGE		32
#define M4A1DRAGON_DAMAGE_SIL		33
#define M4A1DRAGON_RANGE_MODIFER      0.97
#define M4A1DRAGON_RANGE_MODIFER_SIL  0.95
#define M4A1DRAGON_RELOAD_TIME	3.05

class CM4A1DRAGON : public CBasePlayerWeapon
{
public:
	void Spawn() override;
	void Precache() override;
	int GetItemInfo(ItemInfo *p) override;
	BOOL Deploy() override;
	float GetMaxSpeed() override { return M4A1DRAGON_MAX_SPEED; }
	int iItemSlot() override { return PRIMARY_WEAPON_SLOT; }
	const char* GetCSModelName() override { return "models/w_m4a1dragon.mdl"; }
	void PrimaryAttack() override;
	void SecondaryAttack() override;
	void Reload() override;
	void WeaponIdle() override;
	BOOL UseDecrement() override {
#ifdef CLIENT_WEAPONS
		return TRUE;
#else
		return FALSE;
#endif
	}
	KnockbackData GetKnockBackData() override { return { 350.0f, 250.0f, 300.0f, 100.0f, 0.6f }; }

public:
	void M4A1DRAGONFire(float flSpread, float flCycleTime, BOOL fUseAutoAim);

	int m_iShell;
	int iShellOn;

private:
	unsigned short m_usFireM4A1DRAGON;
};

#endif