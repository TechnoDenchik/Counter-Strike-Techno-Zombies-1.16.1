/*
#pragma once

#include "interface.h"
#include "gamemode/interface/interface_const.h"
#include "hud_sub.h"
#include "r_texture.h"

class CHudInterface_used : public IBaseHudSub
{
public:
	CHudInterface_used(void);
	int VidInit(void) override;
	void Reset(void) override;
	int Draw(float time) override;
	void Think(void) override;

public:
	// returns x
	int DrawHealthRecoveryIcon(float time, int x, int y) const;
	int DrawSkillBoard(float time, int x, int y) const;
	void DrawSkillTip(float time) const;
	void DrawSkillBoardNew(float time) const;

public:
	void OnHealthRecovery();
	void OnSkillInit(INTClassType zclass = ZOMBIE_CLASS_HUMAN, INTSkillType skill1 = ZOMBIE_SKILL_EMPTY, INTSkillType skill2 = ZOMBIE_SKILL_EMPTY, INTSkillType skill3 = ZOMBIE_SKILL_EMPTY, INTSkillType skill4 = ZOMBIE_SKILL_EMPTY);
	void OnSkillActivate(INTSkillType skill, float flHoldTime, float flFreezeTime);

protected:
	int m_HUD_zombirecovery;

	int m_HUD_zombieGKey;
	int m_HUD_SkillIcons[MAX_ZOMBIE_SKILL];
	int m_HUD_ClassIcons[MAX_ZOMBIE_CLASS];
	UniqueTexture m_pTexture_SkillTips[MAX_ZOMBIE_SKILL];

	UniqueTexture m_pTexture_skillslotkeybg;
	UniqueTexture m_pTexture_skillslotbg;
	UniqueTexture m_pTexture_NewSkillIcons[MAX_ZOMBIE_SKILL];
	UniqueTexture m_pTexture_NewClassIcons[MAX_ZOMBIE_CLASS];

protected:
	float m_flRecoveryBeginTime;
	INTClassType m_iCurrentClass;

	struct INTSkillHudIcon
	{
		INTSkillType m_iCurrentSkill;
		INTSkillStatus m_iCurrentSkillStatus;
		float m_flTimeSkillStart;
		float m_flTimeSkillReady;
		float m_flTimeSkillBlink;
	} m_ZombieSkillHudIcons[4];
	int DrawSkillIcon(float time, int x, int y, const INTSkillHudIcon &icon) const;
	
private:
	struct Config;
};*/