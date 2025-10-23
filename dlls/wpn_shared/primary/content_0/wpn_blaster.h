/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#ifndef WPN_BLASTER_H
#define WPN_BLASTER_H
#ifdef _WIN32
#pragma once
#endif

#define BLASTER_MAX_SPEED			220
#define BLASTER_DAMAGE			32
#define BLASTER_RANGE_MODIFER		0.97
#define BLASTER_RELOAD_TIME		4.7

class CBLASTER : public CBasePlayerWeapon
{
public:
	void Spawn() override;
	void Precache() override;
	int GetItemInfo(ItemInfo *p) override;
	BOOL Deploy() override;
	float GetMaxSpeed() override { return BLASTER_MAX_SPEED; }
	int iItemSlot() override { return PRIMARY_WEAPON_SLOT; }
	const char* GetCSModelName() override { return "models/w_blaster.mdl"; }
	void PrimaryAttack() override;
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
	void BLASTERFire(float flSpread, float flCycleTime, BOOL fUseAutoAim);

	int m_iShell;
	int iShellOn;

private:
	unsigned short m_usFireBLASTER;
};

#endif