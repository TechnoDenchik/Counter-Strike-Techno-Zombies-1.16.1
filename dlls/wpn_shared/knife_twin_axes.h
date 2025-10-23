/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */
#ifndef KNIFE_TWIN_AXES_H
#define KNIFE_TWIN_AXES_H
#ifdef _WIN32
#pragma once
#endif

#define KNIFE_BODYHIT_VOLUME		128
#define KNIFE_WALLHIT_VOLUME		512
#define KNIFE_MAX_SPEED			250
#define KNIFE_MAX_SPEED_SHIELD		180

#include "weapons/WeaponTemplate.hpp"
#ifndef CLIENT_DLL
#include "effects.h"
#include "customentity.h"
#include "monsters.h"
#include "gamemode/mods.h"
#include "util/u_range.hpp"
#endif
#include "weapons/KnifeAttack.h"
#include <vector>
#include <array>
class CBeam;

class CSTwinShadowAxes : public CBasePlayerWeapon
{
public:
	void Spawn() override;
	void Precache() override;
	int GetItemInfo(ItemInfo* p) override;
	BOOL CanDrop() override { return FALSE; }
	BOOL Deploy() override;
	void Skin();
	void Holster(int skiplocal) override;
	float GetMaxSpeed() override { return m_fMaxSpeed; }
	virtual int iItemSlot() { return KNIFE_SLOT; }
	void ItemPostFrame() override;
	void RadiusDamage();
	void GetSkin();
	void PrimaryAttack() override;
	void SecondaryAttack() override;
#ifndef CLIENT_DLL
	BOOL KnifeAttack1(Vector vecSrc, Vector vecDir, float flDamage, float flRadius, float flAngleDegrees, int bitsDamageType,
		entvars_t* pevInflictor, entvars_t* pevAttacker);
#endif
	BOOL UseDecrement() override {
#ifdef CLIENT_WEAPONS
		return TRUE;
#else
		return FALSE;
#endif
	}
	void WeaponIdle() override;

	float m_flNextSBarUpdateTime;
	float m_flStatusBarDisappearDelay;

	bool swing;
	bool setkombo;
	bool setammo;
	bool setskin;
	bool setskin2;
	bool setskill1;
	bool setskill2;
	float m_flNumFrames;
	double GetDamage_PrimaryAttack_Instant() const
	{
#ifndef CLIENT_DLL
		if (g_pModRunning->DamageTrack() == DT_ZBS)
			return 1092.0;
		if (g_pModRunning->DamageTrack() == DT_ZB)
			return 304.0;
#endif
		return 20.0;
	}

	short this_1_has_disconnected;
public:
	void EXPORT Smack();

	NOXREF void WeaponAnimation(int iAnimation);
	int Stab(int fFirst);
	int Swing(int fFirst);
	int kombo(int fFirst);
	int Skill1(int fFirst);
	int Skill2(int fFirst);


public:
	Vector Get_ShootPosition(CBaseEntity* pevAttacker, Vector vecSrc);
	int ExtractAmmo(CBasePlayerWeapon* pWeapon) override;
	int TwinAmmo;
	TraceResult m_trHit;
	TeamName m_iTeam;
	float GetDamage() const;
	char m_SbarString0[SBAR_STRING_SIZE];
	unsigned short m_usKnife;

private:
	int m_iuser1; // iAttackRecord
	int m_iuser3; // iSkillRecord
	int m_iuser4; // iPrimaryAttackRecord
	int m_sequence; // iThinkType
	float m_fuser1; // flNextTimeCanHolster
	int m_weaponanim; // iLastAnim
	int m_waterlevel; // iSecondaryAttackCount

	int ph4;
	int ph5;
	int ph6;
	float ph7;
	float ph8;
	float phs2;	// secondary attack start time
	float phs3; // primary attack start time
	float phs4; // spear attack end time... cannot switch weapon?
	std::array<CBeam*, 3> phs5_6_7; // EHANDLE ?
	// unsigned short phs8; // m_usFireGungnir
	std::vector<EHANDLE> phs9_10_11;
	duration_t tNextAttack2;
	time_point_t tWorldTime2;
	duration_t tDelta2;
	float phs12;
	unsigned short m_usFire;
};




class CTwinAxesSummon : public CGrenade
{
public:
	CTwinAxesSummon()
	{
		m_iState = 0;
		m_iSequence = 0;
		m_flNextAnim = 0;
	}

	static CTwinAxesSummon* Create(int iType, const Vector& vecOrigin, const Vector& vecAngles, edict_t* pentOwner);

	void Spawn(void);
	void Precache(void);
	void EXPORT MineThink();
	void Init(CBasePlayer* pOwner, Vector vecVelocity);
	void Remove();
	void GetSprite();
	void DoBombExp(bool IsManual);
	float GetDamage() const;
public:

	CBasePlayer* m_pOwner;
	CSTwinShadowAxes* m_pWeapon;
	int m_iState;
	int m_iTimeSummon;
	float m_iTimeSummonLife;
	int m_iType;

	float m_iRenderAmt;
private:
	int m_iExp[2];
	int m_iSequence;
	int m_iLifeModel;
	time_point_t m_flNextAnim;

	duration_t tNextAttack7;
	time_point_t tWorldTime7;
	duration_t tDelta7;

	duration_t tNextAttack8;
	time_point_t tWorldTime8;
	duration_t tDelta8;

	duration_t tNextAttack12;
	time_point_t tWorldTime12;
	duration_t tDelta12;

	bool hasmodel;

};
#endif