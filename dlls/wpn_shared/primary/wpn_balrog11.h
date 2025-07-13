/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#ifndef WPN_BALROG11_H
#define WPN_BALROG11_H
#ifdef _WIN32
#pragma once
#endif

//xm1014
#define XM1014_MAX_SPEED	240
#define XM1014_CONE_VECTOR	Vector(0.0725, 0.0725, 0.0)	// special shotgun spreads

class CBalrog11 : public CBasePlayerWeapon
{
public:
	void Spawn() override;
	void Precache() override;
	int GetItemInfo(ItemInfo *p) override;
	BOOL Deploy() override;
	float GetMaxSpeed() override { return XM1014_MAX_SPEED; }
	int iItemSlot() override { return PRIMARY_WEAPON_SLOT; }
	void PrimaryAttack() override;
	void Reload() override;
	void WeaponIdle() override;
	const char* GetCSModelName() override { return "models/w_balrog11.mdl"; }
	BOOL UseDecrement() override {
#ifdef CLIENT_WEAPONS
		return TRUE;
#else
		return FALSE;
#endif
	}
	KnockbackData GetKnockBackData() override { return { 700.0f, 450.0f, 600.0f, 450.0f, 0.4f }; }

public:
	int m_iShell;
	float m_flPumpTime;

private:
	unsigned short m_usFireXM1014;
};

#endif