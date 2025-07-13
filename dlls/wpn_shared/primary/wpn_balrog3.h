/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#ifndef WPN_BALROG3_H
#define WPN_BALROG3_H
#ifdef _WIN32
#pragma once
#endif

//p90
#define P90_MAX_SPEED		245
#define P90_DAMAGE		21
#define P90_RANGE_MODIFER	0.885
#define P90_RELOAD_TIME		3.4

class CBalrog3 : public CBasePlayerWeapon
{
public:
	void Spawn() override;
	void Precache() override;
	int GetItemInfo(ItemInfo *p) override;
	BOOL Deploy() override;
	float GetMaxSpeed() override { return P90_MAX_SPEED; }
	int iItemSlot() override { return PRIMARY_WEAPON_SLOT; }
	void PrimaryAttack() override;
	void Reload() override;
	void WeaponIdle() override;
	const char* GetCSModelName() override { return "models/w_balrog3.mdl"; }
	
	BOOL UseDecrement() override {
#ifdef CLIENT_WEAPONS
		return TRUE;
#else
		return FALSE;
#endif
	}
	KnockbackData GetKnockBackData() override { return { 250.0f, 200.0f, 250.0f, 90.0f, 0.7f }; }
	void RadiusDamage(Vector vecAiming, float flDamage);
	Vector Get_ShootPosition(CBaseEntity* pevAttacker, Vector vecSrc);

public:
	void BALROGFire(float flSpread, float flCycleTime, BOOL fUseAutoAim);
	void BALROGFire2(float flSpread, float flCycleTime, BOOL fUseAutoAim);

	int m_iShell;
	int iShellOn;
	int FireClip;
	bool FireClip2;
	float BALROG_DAMAGE;
	float BALROG_DAMAGE2;
private:
	unsigned short m_usFireBALROG3;
	unsigned short m_usFire2BALROG3;
};

#endif