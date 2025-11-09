/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#ifndef WPN_BALROG1_H
#define WPN_BALROG1_H
#ifdef _WIN32
#pragma once
#endif

//Deagle
#define DEAGLE_MAX_SPEED	250

#define DEAGLE_RANGE_MODIFER	0.81
#define DEAGLE_RELOAD_TIME	2.2

class CBalrog1 : public CBasePlayerWeapon
{
public:
	void Spawn() override;
	void Precache() override;
	int GetItemInfo(ItemInfo *p) override;
	BOOL Deploy() override;
	float GetMaxSpeed() override { return m_fMaxSpeed; }
	int iItemSlot() override { return PISTOL_SLOT; }
	bool HasSecondaryAttack() override { return true; }
	void PrimaryAttack() override;
	void SecondaryAttack() override;
	void Reload() override;
	void WeaponIdle() override;
	void ItemPostFrame() override;
	void RadiusDamage(Vector vecAiming, float flDamage);
	Vector Get_ShootPosition(CBaseEntity* pevAttacker, Vector vecSrc);
	const char* GetCSModelName() override { return "models/w_balrog1.mdl"; }
	BOOL UseDecrement() override {
#ifdef CLIENT_WEAPONS
		return TRUE;
#else
		return FALSE;
#endif
	}
	
	BOOL IsPistol() override { return TRUE; }
	KnockbackData GetKnockBackData() override { return { 350.0f, 250.0f, 350.0f, 100.0f, 0.6f }; }
	int BalrogAmmo;
	bool m_fireuse2;
public:
	int ExtractAmmo(CBasePlayerWeapon* pWeapon) override;
	void BALROGFire(float flSpread, float flCycleTime, BOOL fUseSemi);
	void BALROGFire2(float flSpread, float flCycleTime, BOOL fUseSemi);

	int m_iShell;
	float BALROG_DAMAGE;
	float BALROG_DAMAGE2;
private:
	unsigned short m_usFireBALROG1;
	unsigned short m_usFire2BALROG1;
};

#endif