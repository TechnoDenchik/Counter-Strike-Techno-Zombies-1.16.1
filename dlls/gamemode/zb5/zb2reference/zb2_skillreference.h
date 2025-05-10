
#ifndef PLAYER_ZOMBIE_SKILLR_H
#define PLAYER_ZOMBIE_SKILLR_H
#ifdef _WIN32
#pragma once
#endif

#include "player/player_mod_strategy.h"
#include "gamemode/zb2/zb2_const.h"

class IZombieSkillR
{
public:
	virtual ~IZombieSkillR() = 0;

	virtual void Think() = 0;
	virtual void Activate() = 0;
	virtual void Reset() = 0;
	virtual void ResetMaxSpeed() = 0;
	virtual float GetDamageRatio() const = 0;
	virtual ZombieSkillStatus GetStatus() const = 0;
	
};

inline IZombieSkillR::~IZombieSkillR() = default;

class CZombieSkill_BaseR : public BasePlayerExtra, public IZombieSkillR
{
public:
	explicit CZombieSkill_BaseR(CBasePlayer *player);

	void Think() override;
	void Activate() override {}
	void Reset() override { m_iZombieSkillStatus = SKILL_STATUS_READY; }
	void ResetMaxSpeed() override {}
	float GetDamageRatio() const override { return 1.0f; }
	ZombieSkillStatus GetStatus() const override {	return m_iZombieSkillStatus; }

protected:
	virtual void OnSkillEnd() {}
	virtual void OnSkillReady() {}

protected:
	ZombieSkillStatus m_iZombieSkillStatus;
	float m_flTimeZombieSkillEnd;
	float m_flTimeZombieSkillNext;

};

class CZombieSkill_EmptyR : public BasePlayerExtra, public IZombieSkillR
{
public:
	explicit CZombieSkill_EmptyR(CBasePlayer *player)  : BasePlayerExtra(player) {}

	void Think() override {}
	void Activate() override {}
	void Reset() override {}
	void ResetMaxSpeed() override {}
	float GetDamageRatio() const override { return 1.0f; }
	ZombieSkillStatus GetStatus() const override {	return SKILL_STATUS_USED; }
};

class CZombieSkill_ZombieCrazyR : public CZombieSkill_BaseR
{
public:
	explicit CZombieSkill_ZombieCrazyR(CBasePlayer *player);

public:
	void Think() override;
	void Activate() override;
	void ResetMaxSpeed()  override;
	void OnSkillEnd() override;
	float GetDamageRatio() const override;

protected:
	void OnCrazyEffect();
	float GetDurationTime() const;
	float GetCooldownTime() const;

	float m_flTimeZombieSkillEffect;
};

class CHeroSkillR : public BasePlayerExtra, public IZombieSkillR
{
public:
	explicit CHeroSkillR(CBasePlayer* player);

	void Think() override;
	void Activate() override {}
	void Reset() override { m_iZombieSkillStatus = SKILL_STATUS_READY; }
	void ResetMaxSpeed() override {}
	float GetDamageRatio() const override { return 1.0f; }
	ZombieSkillStatus GetStatus() const override { return m_iZombieSkillStatus; }

protected:
	virtual void OnSkillEnd() {}
	virtual void OnSkillReady() {}

protected:
	ZombieSkillStatus m_iZombieSkillStatus;
	float m_flTimeZombieSkillEnd;
	float m_flTimeZombieSkillNext;

};

class CHeroSkill_EmptyR : public BasePlayerExtra, public IZombieSkillR
{
public:
	explicit CHeroSkill_EmptyR(CBasePlayer* player) : BasePlayerExtra(player) {}

	void Think() override {}
	void Activate() override {}
	void Reset() override {}
	void ResetMaxSpeed() override {}
	float GetDamageRatio() const override { return 1.0f; }
	ZombieSkillStatus GetStatus() const override { return SKILL_STATUS_USED; }
};

class CHeroSkill_sprintR : public CHeroSkillR
{
public:
	explicit CHeroSkill_sprintR(CBasePlayer* player);

public:
	void Think() override;
	void Activate() override;
	void ResetMaxSpeed()  override;
	void OnSkillEnd() override;
	float GetDamageRatio() const override;

protected:
	void OnCrazyEffect();
	float GetDurationTime() const;
	float GetCooldownTime() const;

	float m_flTimeZombieSkillEffect;
};

#endif