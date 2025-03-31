/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#ifndef WPN_ARBALEST_H
#define WPN_ARBALEST_H
#ifdef _WIN32
#pragma once
#endif

#include <vector>
#include <array>

#define ARBALEST_MAX_SPEED			240
#define ARBALEST_DAMAGE			32
#define ARBALEST_RANGE_MODIFER		0.96
#define ARBALEST_RELOAD_TIME			2.3

class CBeam;

class CArbalest : public CBasePlayerWeapon
{
public:
	void Spawn() override;
	void Precache() override;
	int GetItemInfo(ItemInfo *p) override;
	BOOL Deploy() override;
	float GetMaxSpeed() override { return ARBALEST_MAX_SPEED; }
	int iItemSlot() override { return PRIMARY_WEAPON_SLOT; }
	bool HasSecondaryAttack() override { return true; }
	void PrimaryAttack() override;
	void ClearEffect();
	void Getsprite();
	void Holster(int skiplocal) override;
	void SecondaryAttack() override;
	void Reload() override;
	void WeaponIdle() override;
	void ItemPostFrame() override;
	BOOL UseDecrement() override {
#ifdef CLIENT_WEAPONS
		return TRUE;
#else
		return FALSE;
#endif
	}
	KnockbackData GetKnockBackData() override { return { 350.0f,250.0f,300.0f,100.0f,0.6f }; }

public:
	void ArbalestFire(float flSpread, duration_t flCycleTime, BOOL fUseAutoAim);
	void ArbalestFire2();
	void RadiusDamage(Vector vecAiming, float flDamage);
	void RadiusDamage2();
	void RadiusDamage3(Vector vecAiming, float flDamage);
	const char *GetCSModelName() override { return "models/w_halogun.mdl"; }
	Vector Get_ShootPosition(CBaseEntity *pevAttacker, Vector vecSrc);
	Vector Get_ShootPosition2(CBaseEntity* pevAttacker, Vector vecSrc);
	bool PrimaryAttack_CheckTargetAvailable(CBaseEntity* a2, Vector vecDirection);
	int ExtractAmmo(CBasePlayerWeapon* pWeapon) override;
	float GetDamage() const;
	void DestroyEffect();
	void PrimaryAttack_FindTargets();
	int m_iShell;
	int iShellOn;
	int m_iClip_c;
	bool isusable;
	bool m_fireuse2;
private:
	unsigned short m_usFireArbalestar;
	duration_t tNextAttack3;
	time_point_t tWorldTime3;
	duration_t tDelta3;

	duration_t tNextAttack4;
	time_point_t tWorldTime4;
	duration_t tDelta4;


	duration_t tNextAttack5;
	time_point_t tWorldTime5;
	duration_t tDelta5;

	float phs2;	// secondary attack start time
	float phs3; // primary attack start time
	float phs4;
	float phs5;// spear attack end time... cannot switch weapon?


	std::vector<EHANDLE> phs9_10_11;
	std::array<CBeam*, 5> phs5_6_7;
	float phs12;
};

#endif