/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */
#ifndef WPN_CQUANT_H
#define WPN_CQUANT_H
#ifdef _WIN32
#pragma once
#endif

//AK47
#define QUANT_MAX_SPEED			221
#define QUANT_DAMAGE			36
#define QUANT_RANGE_MODIFER		0.98
#define QUANT_RELOAD_TIME		2.45

class CQUANT : public CBasePlayerWeapon
{
public:
	void Spawn() override;
	void Precache() override;
	int GetItemInfo(ItemInfo* p) override;
	BOOL Deploy() override;
	float GetMaxSpeed() override { return QUANT_MAX_SPEED; }
	int iItemSlot() override { return PRIMARY_WEAPON_SLOT; }
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
	KnockbackData GetKnockBackData() override { return { 350.0f,250.0f,300.0f,100.0f,0.6f }; }

public:
	void QuantFire(float flSpread, float flCycleTime, BOOL fUseAutoAim);
	void QuantFire2(float flSpread, float flCycleTime, BOOL fUseAutoAim);
	int m_iShell;
	int iShellOn;

private:
	unsigned short m_usFireQuant;
};

#endif