/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#ifndef WPN_BUFFAUG_H
#define WPN_BUFFAUG_H
#ifdef _WIN32
#pragma once
#endif

//BUFFAUG
#define BUFFAUG_MAX_SPEED			240
#define BUFFAUG_RANGE_MODIFER		0.96
#define BUFFAUG_RELOAD_TIME			3.3

class CBUFFAUG : public CBasePlayerWeapon
{
public:
	void Spawn() override;
	void Precache() override;
	int GetItemInfo(ItemInfo *p) override;
	BOOL Deploy() override;
	float GetMaxSpeed() override { return BUFFAUG_MAX_SPEED; }
	int iItemSlot() override { return PRIMARY_WEAPON_SLOT; }
	void PrimaryAttack() override;
	void SecondaryAttack() override;
	void Reload() override;
	void WeaponIdle() override;
	const char* GetCSModelName() override { return "models/w_buffaug.mdl"; }
	BOOL UseDecrement() override {
#ifdef CLIENT_WEAPONS
		return TRUE;
#else
		return FALSE;
#endif
	}
	KnockbackData GetKnockBackData() override { return { 350.0f,250.0f,300.0f,100.0f,0.6f }; }

public:
	void BUFFAUGFire(float flSpread, float flCycleTime, BOOL fUseAutoAim);

	int m_iShell;
	int iShellOn;
	float BUFFAUG_DAMAGE;

private:
	unsigned short m_usFireBuffAUG;
};

#endif