/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#ifndef WPN_QUANT_H
#define WPN_QUANT_H
#ifdef _WIN32
#pragma once
#endif

#include <vector>
#include <array>

//AUG
#define QUANT_MAX_SPEED			240
#define QUANT_DAMAGE			32
#define QUANT_RANGE_MODIFER		0.96
#define QUANT_RELOAD_TIME			3.3

class CBeam;

class CQuantum : public CBasePlayerWeapon
{
public:
	void Spawn() override;
	void Precache() override;
	int GetItemInfo(ItemInfo *p) override;
	BOOL Deploy() override;
	float GetMaxSpeed() override { return QUANT_MAX_SPEED; }
	int iItemSlot() override { return PRIMARY_WEAPON_SLOT; }
	bool HasSecondaryAttack() override { return true; }
	void PrimaryAttack() override;
	bool PrimaryAttack_CheckTargetAvailable(CBaseEntity* a2, Vector vecDirection);
	void SecondaryAttack() override;
	void CreateEffect();
	void DestroyEffect();
	void Holster(int skiplocal) override;
	
	void PrimaryAttack_FindTargets();
	//void ItemPostFrame() override;
	void Reload() override;
	void WeaponIdle() override;
	void ClearEffect();
	void Getsprite();
	void ItemPostFrame() override;
	bool fireon;
	bool fireon2;
	bool fireon3;
	BOOL UseDecrement() override {
#ifdef CLIENT_WEAPONS
		return TRUE;
#else
		return FALSE;
#endif
	}
	KnockbackData GetKnockBackData() override { return { 350.0f,250.0f,300.0f,100.0f,0.6f }; }

public:
	void QuantFire(float flSpread, duration_t flCycleTime, BOOL fUseAutoAim);
	void QuantFire2(float flSpread, duration_t flCycleTime, BOOL fUseAutoAim);
	void RadiusDamage(Vector vecAiming, float flDamage);
	void RadiusDamage3(Vector vecAiming, float flDamage);
	void RadiusDamage2();
	const char *GetCSModelName() override { return "models/w_revivegun.mdl"; }
	static constexpr const char* Beam_SPR = "sprites/ef_revivegun_laser.spr"; //
	Vector Get_ShootPosition(CBaseEntity *pevAttacker, Vector vecSrc);
	Vector Get_ShootPosition2(CBaseEntity* pevAttacker, Vector vecSrc);
	int ExtractAmmo(CBasePlayerWeapon* pWeapon) override;
	float GetDamage() const;
	int m_iShell;
	int iShellOn;
	int QuantAmmo;
	
	bool m_fireuse2;

private:
	unsigned short m_usFire;
	duration_t tNextAttack;
	time_point_t tWorldTime;
	duration_t tDelta;

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
	float phs3;
	float phs4;
	float phs5;// spear attack end time... cannot switch weapon?
	float phs12;

	std::vector<EHANDLE> phs9_10_11;
	std::array<CBeam*, 5> phs5_6_7;
	std::array<CBeam*, 3> phs12_13_14;
};
#endif