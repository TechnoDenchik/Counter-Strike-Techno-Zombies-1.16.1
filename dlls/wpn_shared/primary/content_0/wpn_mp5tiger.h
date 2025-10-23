/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#ifndef WPN_MP5TIGER_H
#define WPN_MP5TIGER_H
#ifdef _WIN32
#pragma once
#endif

#define MP5TIGER_MAX_SPEED			250
#define MP5TIGER_DAMAGE			26
#define MP5TIGER_RANGE_MODIFER		0.84
#define MP5TIGER_RELOAD_TIME		2.63

class CMP5TIGER : public CBasePlayerWeapon
{
public:
	void Spawn() override;
	void Precache() override;
	int GetItemInfo(ItemInfo *p) override;
	BOOL Deploy() override;
	float GetMaxSpeed() override { return MP5TIGER_MAX_SPEED; }
	int iItemSlot() override { return PRIMARY_WEAPON_SLOT; }
	const char* GetCSModelName() override { return "models/w_mp5tiger.mdl"; }
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
	KnockbackData GetKnockBackData() override { return { 250.0f, 200.0f, 250.0f, 90.0f, 0.7f }; }

public:
	void MP5TIGERFire(float flSpread, float flCycleTime, BOOL fUseAutoAim);

	int m_iShell;
	int iShellOn;

private:
	unsigned short m_usFireMP5TIGER;
};

#endif