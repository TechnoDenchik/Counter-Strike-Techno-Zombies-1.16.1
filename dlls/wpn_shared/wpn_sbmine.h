/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#ifndef WPN_SBMINE_H
#define WPN_SBMINE_H
#ifdef _WIN32
#pragma once
#endif

#define SBMINE_MAX_SPEED		250
#define SBMINE_MAX_SPEED_SHIELD	180

class CSBMINE : public CBasePlayerWeapon
{
public:
	void Spawn() override;
	void Precache() override;
	int GetItemInfo(ItemInfo *p) override;
	BOOL CanDrop() override { return FALSE; }
	BOOL Deploy() override;
	void Holster(int skiplocal) override;
	float GetMaxSpeed() { return m_fMaxSpeed; }
	int iItemSlot() override { return GRENADE_SLOT; }
	bool HasSecondaryAttack() override { return true; }
	void PrimaryAttack() override;
	void ItemPostFrame() override;
	void SecondaryAttack() override;
	void WeaponIdle() override;
	BOOL UseDecrement() override
	{
#ifdef CLIENT_WEAPONS
		return TRUE;
#else
		return FALSE;
#endif
	}

public:
	bool ShieldSecondaryFire(int iUpAnim, int iDownAnim);
	void SetPlayerShieldAnim();
	void ResetPlayerShieldAnim();

public:

	duration_t tNextAttack6;
	time_point_t tWorldTime6;
	duration_t tDelta6;

	duration_t tNextAttack11;
	time_point_t tWorldTime11;
	duration_t tDelta11;

	float phs2;	// secondary attack start time
	float phs3; // primary attack start time
	float phs4;
	float phs5;// spear attack end time... cannot switch weapon?
	float phs12;
	float phs13;
	float phs14;

	bool shootmine;

	unsigned short m_usCreateExplosion;
};

class CSBMineGet : public CGrenade
{
public:
	CSBMineGet()
	{
		m_iState = 0;
		m_iSequence = 0;
		m_flNextAnim = 0;
	}

	static CSBMineGet* Create(int iType, const Vector& vecOrigin, const Vector& vecAngles, edict_t* pentOwner);

	void Spawn(void);
	void Precache(void);
	void EXPORT MineThink();
	void Init(CBasePlayer* pOwner, Vector vecVelocity);
	void Remove();
	void GetModel();
	void GetModelExplosion();
	void Explode(bool IsManual);
	void DoBombExp(bool IsManual);
	void GetWarningDamage(bool IsManual);
	float GetDamage(bool IsManual) const;
public:
	CBasePlayer* m_pOwner;
	CSBMINE* m_pWeapon;
	int m_iState;
	int m_iType;
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

	int isexploderenine;
	bool istarget;

};
#endif