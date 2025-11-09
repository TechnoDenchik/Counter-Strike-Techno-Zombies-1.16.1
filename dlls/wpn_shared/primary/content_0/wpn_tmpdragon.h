/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#ifndef WPN_TMPDRAGON_H
#define WPN_TMPDRAGON_H
#ifdef _WIN32
#pragma once
#endif

#define TMPDRAGON_MAX_SPEED			250
#define TMPDRAGON_DAMAGE			20
#define TMPDRAGON_RANGE_MODIFER		0.85
#define TMPDRAGON_RELOAD_TIME			2.12

class CTMPDRAGON : public CBasePlayerWeapon
{
public:
	void Spawn() override;
	void Precache() override;
	int GetItemInfo(ItemInfo *p) override;
	BOOL Deploy() override;
	float GetMaxSpeed() override { return TMPDRAGON_MAX_SPEED; }
	int iItemSlot() override { return PRIMARY_WEAPON_SLOT; }
	const char* GetCSModelName() override { return "models/w_tmpdragon.mdl"; }
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
	void TMPDRAGONFire(float flSpread, float flCycleTime, BOOL fUseAutoAim);

public:
	int m_iShell;
	int iShellOn;

private:
	unsigned short m_usFireTMPDRAGON;
};

#endif