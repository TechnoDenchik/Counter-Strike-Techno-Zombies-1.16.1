
#ifndef PLAYER_ZOMBIE_SKILLR_H
#define PLAYER_ZOMBIE_SKILLR_H
#ifdef _WIN32
#pragma once
#endif

#include "player/player_mod_strategy.h"
#include "gamemode/zb2/zb2_const.h"
#include "gamemode/interface/interface_const.h"

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










class SprintSkillR : protected CZombieSkill_BaseR
{
public:
	explicit SprintSkillR(CBasePlayer* player) : CZombieSkill_BaseR(player), m_flTimeZombieSkillEffect(0.0f), m_bDebuffStatus(false) {}

	void Think() override
	{
		CZombieSkill_BaseR::Think();

		if (m_iZombieSkillStatus == SKILL_STATUS_USING && gpGlobals->time > m_flTimeZombieSkillEffect)
		{
			OnCrazyEffect();
		}
	}

	void Activate() override
	{
		if (m_iZombieSkillStatus != SKILL_STATUS_READY)
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgZB3UsedMsg2, NULL, m_pPlayer->pev);
			WRITE_BYTE(ZB3_USED_MSG2);
			MESSAGE_END();
			return;
		}

		m_iZombieSkillStatus = SKILL_STATUS_USING;
		m_flTimeZombieSkillEnd = gpGlobals->time + 10.0f;
		m_flTimeZombieSkillNext = -1;
		m_flTimeZombieSkillEffect = gpGlobals->time + 1.0f;
		m_bDebuffStatus = false;
		m_pPlayer->SpawnProtection_Start(4.0f);
		m_pPlayer->ResetMaxSpeed();

		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_BODY, "zb3/speedup.wav", VOL_NORM, ATTN_NORM);

		MESSAGE_BEGIN(MSG_ONE, gmsgZB2Msg, NULL, m_pPlayer->pev);
		WRITE_BYTE(ZB2_MESSAGE_SKILL_ACTIVATE);
		WRITE_BYTE(ZOMBIE_SKILL_SPRINT);
		WRITE_SHORT(10);
		WRITE_SHORT(-1);
		MESSAGE_END();
	}

	void ResetMaxSpeed() override
	{
		if (m_iZombieSkillStatus == SKILL_STATUS_USING)
		{
			if (m_bDebuffStatus)
			{
				m_pPlayer->pev->maxspeed = 100;
			}
			else
			{
				m_pPlayer->pev->maxspeed = 450;
			}
		}
	}

	using CZombieSkill_BaseR::GetStatus;

protected:
	void OnSkillEnd() override
	{
		if (m_bDebuffStatus)
		{
			m_iZombieSkillStatus = SKILL_STATUS_USED;
			m_bDebuffStatus = false;
		}
		else
		{
			m_iZombieSkillStatus = SKILL_STATUS_USING;
			m_bDebuffStatus = true;
			m_flTimeZombieSkillEnd = gpGlobals->time + 5.0f;
		}
		m_pPlayer->ResetMaxSpeed();
	}

	void OnCrazyEffect()
	{
		m_flTimeZombieSkillEffect = gpGlobals->time + 1.0f;

		if (m_bDebuffStatus)
			CLIENT_COMMAND(m_pPlayer->edict(), "spk zb3/human_breath_male.wav\n");
		else
			CLIENT_COMMAND(m_pPlayer->edict(), "spk zb3/speedup_heartbeat.wav\n");
	}

private:
	float m_flTimeZombieSkillEffect;
	bool m_bDebuffStatus;
};

class HeadShotSkillR : protected CZombieSkill_BaseR
{
public:
	explicit HeadShotSkillR(CBasePlayer* player) : CZombieSkill_BaseR(player) {}

	void Activate() override
	{
		if (m_iZombieSkillStatus != SKILL_STATUS_READY)
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgZB3UsedMsg2, NULL, m_pPlayer->pev);
			WRITE_BYTE(ZB3_USED_MSG2);
			MESSAGE_END();
			return;
		}
		m_iZombieSkillStatus = SKILL_STATUS_USING;
		m_flTimeZombieSkillEnd = gpGlobals->time + 4.5f;
		m_flTimeZombieSkillNext = -1;

		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_BODY, "zb3/speedup.wav", VOL_NORM, ATTN_NORM);

		MESSAGE_BEGIN(MSG_ONE, gmsgZB2Msg, NULL, m_pPlayer->pev);
		WRITE_BYTE(ZB2_MESSAGE_SKILL_ACTIVATE);
		WRITE_BYTE(ZOMBIE_SKILL_HEADSHOT);
		WRITE_SHORT(5);
		WRITE_SHORT(-1);
		MESSAGE_END();

		MESSAGE_BEGIN(MSG_ALL, gmsgHeadIcon);
		WRITE_BYTE(2);
		WRITE_SHORT(ENTINDEX(m_pPlayer->edict()));
		MESSAGE_END();
	}

	void OnSkillEnd() override
	{
		m_iZombieSkillStatus = SKILL_STATUS_USED;
	}

	using CZombieSkill_BaseR::GetStatus;
	using CZombieSkill_BaseR::Think;
};

class Knife2xSkillR : protected CZombieSkill_BaseR
{
public:
	explicit Knife2xSkillR(CBasePlayer* player) : CZombieSkill_BaseR(player) {}

	void Activate() override
	{
		if (m_iZombieSkillStatus != SKILL_STATUS_READY)
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgZB3UsedMsg2, NULL, m_pPlayer->pev);
			WRITE_BYTE(ZB3_USED_MSG2);
			MESSAGE_END();
			return;
		}
		m_iZombieSkillStatus = SKILL_STATUS_USING;
		m_flTimeZombieSkillEnd = gpGlobals->time + 10.f;
		m_flTimeZombieSkillNext = -1;

		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_BODY, "zb3/speedup.wav", VOL_NORM, ATTN_NORM);

		MESSAGE_BEGIN(MSG_ONE, gmsgZB2Msg, NULL, m_pPlayer->pev);
		WRITE_BYTE(ZB2_MESSAGE_SKILL_ACTIVATE);
		WRITE_BYTE(ZOMBIE_SKILL_KNIFE2X);
		WRITE_SHORT(10);
		WRITE_SHORT(-1);
		MESSAGE_END();

		MESSAGE_BEGIN(MSG_ALL, gmsgHeadIcon);
		WRITE_BYTE(1);
		WRITE_SHORT(ENTINDEX(m_pPlayer->edict()));
		MESSAGE_END();
	}

	void OnSkillEnd() override
	{
		m_iZombieSkillStatus = SKILL_STATUS_USED;
	}

	using CZombieSkill_BaseR::GetStatus;
	using CZombieSkill_BaseR::Think;
};

#endif