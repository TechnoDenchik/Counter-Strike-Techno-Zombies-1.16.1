/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */
#ifndef WPN_WONDERCANNON_H
#define WPN_WONDERCANNON_H
#ifdef _WIN32
#pragma once
#endif

#define WONDERCANNON_MAX_SPEED			221
#define WONDERCANNON_DAMAGE			36
#define WONDERCANNON_RANGE_MODIFER		0.98
#define WONDERCANNON_RELOAD_TIME		2.45

#include <vector>
#include <array>
#include "weapons/KnifeAttack.h"
class CBeam;

class CWonderCannon : public CBasePlayerWeapon
{
public:
	void Spawn() override;
	void Precache() override;
	int GetItemInfo(ItemInfo* p) override;
	BOOL Deploy() override;
	float GetMaxSpeed() override { return WONDERCANNON_MAX_SPEED; }
	int iItemSlot() override { return PRIMARY_WEAPON_SLOT; }
	bool HasSecondaryAttack() override { return true; }
	void PrimaryAttack() override;
	void SecondaryAttack() override;
	void Reload() override;
	void ItemPostFrame() override;
	void ClearEffect();
	void Getsprite();
	void Holster(int skiplocal) override;
	void WeaponIdle() override;
	BOOL UseDecrement() override {
#ifdef CLIENT_WEAPONS
		return TRUE;
#else
		return FALSE;
#endif
	}
	const char* GetCSModelName() override { return "models/w_wondercannon.mdl"; }
	KnockbackData GetKnockBackData() override { return { 350.0f, 250.0f, 200.0f, 100.0f, 0.6f }; }
#ifndef CLIENT_DLL
	WeaponBuyAmmoConfig GetBuyAmmoConfig() override { return { "ammo_WonderCannonAmmo"}; }
#endif
public:
	void DestroyEffect();
	void PrimaryAttack_FindTargets();
	bool PrimaryAttack_CheckTargetAvailable(CBaseEntity* a2, Vector vecDirection);
	void RadiusDamage2();
	void WonderCannonFire(float flSpread, float flCycleTime, BOOL fUseAutoAim);
	void WonderCannonFire2(float flSpread, float flCycleTime, BOOL fUseAutoAim);
	int ExtractAmmo(CBasePlayerWeapon* pWeapon) override;
#ifndef CLIENT_DLL
	hit_result_t KnifeAttack1(Vector vecSrc, Vector vecDir, float flDamage, float flRadius, float flAngleDegrees, int bitsDamageType,
		entvars_t* pevInflictor, entvars_t* pevAttacker, BOOL iAnim);
#endif
	int m_iSwing;
	int m_iShell;
	int iShellOn;
	int WonderAmmo;
	int WonderExp;
	bool m_fireuse2;
	int m_iSwing2;

private:
	duration_t tNextAttack3;
	time_point_t tWorldTime3;
	duration_t tDelta3;

	duration_t tNextAttack4;
	time_point_t tWorldTime4;
	duration_t tDelta4;

	duration_t tNextAttack5;
	time_point_t tWorldTime5;
	duration_t tDelta5;
	std::vector<EHANDLE> phs9_10_11;
	std::array<CBeam*, 5> phs5_6_7;
	unsigned short m_usFireWonderCannon;
	unsigned short m_usFire2WonderCannon;
};
#endif