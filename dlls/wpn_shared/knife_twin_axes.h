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


class CSTwinShadowAxes : public CBasePlayerWeapon
{
public:
	void Spawn() override;
	void Precache() override;
	int GetItemInfo(ItemInfo* p) override;
	BOOL CanDrop() override { return FALSE; }
	BOOL Deploy() override;
	void Holster(int skiplocal) override;
	float GetMaxSpeed() override { return m_fMaxSpeed; }
	int iItemSlot() override { return KNIFE_SLOT; }
	void Reload() override;
	void PrimaryAttack() override;
	void SecondaryAttack() override;
	BOOL UseDecrement() override {
#ifdef CLIENT_WEAPONS
		return TRUE;
#else
		return FALSE;
#endif
	}
	void WeaponIdle() override;
	bool setkombo;
	bool setskill1;
	bool setskill2;
	
public:
	void EXPORT SwingAgain();
	void EXPORT Smack();

	NOXREF void WeaponAnimation(int iAnimation);
	int Stab(int fFirst);
	int Swing(int fFirst);
	int kombo(int fFirst);
	int Skill1(int fFirst);
	int Skill2(int fFirst);

public:
	bool ShieldSecondaryFire(int iUpAnim, int iDownAnim);
	void SetPlayerShieldAnim();
	void ResetPlayerShieldAnim();

public:
	TraceResult m_trHit;
	unsigned short m_usKnife;

private:
	int m_iuser1; // iAttackRecord
	int m_iuser3; // iSkillRecord
	int m_iuser4; // iPrimaryAttackRecord
	int m_sequence; // iThinkType
	float m_fuser1; // flNextTimeCanHolster
	int m_weaponanim; // iLastAnim
	int m_waterlevel; // iSecondaryAttackCount
};

#endif