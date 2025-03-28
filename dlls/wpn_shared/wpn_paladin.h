/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#ifndef WPN_AK47PALADIN_H
#define WPN_AK47PALADIN_H
#ifdef _WIN32
#pragma once
#endif

//AK47
#define AK47PALADIN_MAX_SPEED			221
#define AK47PALADIN_DAMAGE			36
#define AK47PALADIN_RANGE_MODIFER		0.98
#define AK47PALADIN_RELOAD_TIME		2.45

class CAK47Paladin : public CBasePlayerWeapon
{
public:
	void Spawn() override;
	void Precache() override;
	int GetItemInfo(ItemInfo *p) override;
	BOOL Deploy() override;
	float GetMaxSpeed() override { return AK47PALADIN_MAX_SPEED; }
	int iItemSlot() override { return PRIMARY_WEAPON_SLOT; }
	bool HasSecondaryAttack() override { return true; }
	void RadiusDamage(Vector vecAiming, float flDamage);
	void PrimaryAttack() override;
	void SecondaryAttack() override;
	
	void Reload() override;
	void ItemPostFrame() override;
	void WeaponIdle() override;
	BOOL UseDecrement() override {
#ifdef CLIENT_WEAPONS
		return TRUE;
#else
		return FALSE;
#endif
	}
	const char* GetCSModelName() override { return "models/w_buffak.mdl"; }
	Vector Get_ShootPosition(CBaseEntity* pevAttacker, Vector vecSrc);
	KnockbackData GetKnockBackData() override { return { 350.0f, 250.0f, 200.0f, 100.0f, 0.6f }; }

public:
	void AK47PaladinFire(float flSpread, float flCycleTime, BOOL fUseAutoAim);
	void AK47PaladinFire2(float flSpread, float flCycleTime, BOOL fUseAutoAim);
	int m_iShell;
	int iShellOn;
	float GetDamage() const;
private:
	unsigned short m_usFirePaladin;
	unsigned short m_usFirePaladin2;
};

#endif