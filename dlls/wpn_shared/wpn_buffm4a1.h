/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#ifndef WPN_CM4A1Buff_H
#define WPN_CM4A1Buff_H
#ifdef _WIN32
#pragma once
#endif

//m4a1
#define M4A1BuffMAX_SPEED		230
#define M4A1_DAMAGE		32
#define M4A1_DAMAGE_SIL		33
#define M4A1_RANGE_MODIFER      0.97
#define M4A1_RANGE_MODIFER_SIL  0.95
#define M4A1_RELOAD_TIME	3.05

class CM4A1Buff : public CBasePlayerWeapon
{
public:
	void Spawn() override;
	void Precache() override;
	int GetItemInfo(ItemInfo *p) override;
	BOOL Deploy() override;
	float GetMaxSpeed() override { return M4A1BuffMAX_SPEED; }
	int iItemSlot() override { return PRIMARY_WEAPON_SLOT; }
	bool HasSecondaryAttack() override { return true; }
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
	const char* GetCSModelName() override { return "models/w_buffm4.mdl"; }
	KnockbackData GetKnockBackData() override { return { 350.0f, 250.0f, 300.0f, 100.0f, 0.6f }; }

public:
	void M4A1BuffFire(float flSpread, float flCycleTime, BOOL fUseAutoAim);
	void M4A1BuffFire2(float flSpread, float flCycleTime, BOOL fUseAutoAim);
	bool secondaryattack;
	int m_iShell;
	int iShellOn;

private:
	unsigned short m_usFireM4A1Buff;
};

#endif