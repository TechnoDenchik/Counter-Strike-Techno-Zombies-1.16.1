/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"

#include "gamemode/interface/interface_const.h"
#include "gamemode/zb2/zb2_const.h"
#include "gamemode/zb5/zb2reference/zb2_zclassreference.h"
#include "gamemode/zb5/zb2reference/zb2_skillreference.h"


constexpr bool ENABLE_SPRINT = true;
constexpr bool ENABLE_HEADSHOT = true;
constexpr bool ENABLE_KNIFE2X = true;

void HumanSkill_PrecacheR()
{
	PRECACHE_SOUND("zb3/speedup.wav");
	PRECACHE_SOUND("zb3/human_breath_male.wav");
	PRECACHE_SOUND("zb3/speedup_heartbeat.wav");
}

class SprintSkillR : protected CZombieSkill_BaseR
{
public:
	explicit SprintSkillR(CBasePlayer * player) : CZombieSkill_BaseR(player), m_flTimeZombieSkillEffect(0.0f), m_bDebuffStatus(false) {}

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
	explicit HeadShotSkillR(CBasePlayer * player) : CZombieSkill_BaseR(player) {}

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
	explicit Knife2xSkillR(CBasePlayer * player) : CZombieSkill_BaseR(player) {}

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
	}

	void OnSkillEnd() override
	{
		m_iZombieSkillStatus = SKILL_STATUS_USED;
	}

	using CZombieSkill_BaseR::GetStatus;
	using CZombieSkill_BaseR::Think;
};

class CHuman_ZB2R::impl_t : public BasePlayerExtra
{
public:
	explicit impl_t(CBasePlayer *player) : BasePlayerExtra(player), spd(player), hs(player), k2x(player) {}
	 SprintSkillR spd;
	HeadShotSkillR hs;
	Knife2xSkillR k2x;
};

CHuman_ZB2R::CHuman_ZB2R(CBasePlayer * player) : CHuman_ZB1R(player), pimpl(std::unique_ptr<impl_t>(new impl_t(player)))
{
	m_pPlayer->m_bIsZombie = false;
	m_pPlayer->m_bNotKilled = false;
	m_pPlayer->m_bIsVIP = false;
	m_pPlayer->m_bIsHero = false;

	m_pPlayer->pev->body = 0;

	const char* szModel = "rb";
	SET_CLIENT_KEY_VALUE(m_pPlayer->entindex(), GET_INFO_BUFFER(m_pPlayer->edict()), "model", const_cast<char*>(szModel));

	static char szModelPath[64];
	Q_snprintf(szModelPath, sizeof(szModelPath), "models/player/%s/%s.mdl", szModel, szModel);
	m_pPlayer->SetNewPlayerModel(szModelPath);


	// remove guns & give nvg
	m_pPlayer->m_bNightVisionOn = false;
}

void CHuman_ZB2R::ActivateSkill(ZombieSkillSlot which)
{
	if (which == SKILL_SLOT_1 && ENABLE_SPRINT)
		pimpl->spd.Activate();
	else if (which == SKILL_SLOT_2 && ENABLE_HEADSHOT)
		pimpl->hs.Activate();
	else if (which == SKILL_SLOT_3 && ENABLE_KNIFE2X)
		pimpl->k2x.Activate();
}

void CHuman_ZB2R::InitHUD() const
{
	MESSAGE_BEGIN(MSG_ONE, gmsgZB2Msg, NULL, m_pPlayer->pev);
	WRITE_BYTE(ZB2_MESSAGE_SKILL_INIT);
	WRITE_BYTE(ZOMBIE_CLASS_HUMAN);
	WRITE_BYTE(ENABLE_SPRINT ? ZOMBIE_SKILL_SPRINT : ZOMBIE_SKILL_EMPTY);
	WRITE_BYTE(ENABLE_HEADSHOT ? ZOMBIE_SKILL_HEADSHOT : ZOMBIE_SKILL_EMPTY);
	WRITE_BYTE(ENABLE_KNIFE2X ? ZOMBIE_SKILL_KNIFE2X : ZOMBIE_SKILL_EMPTY);
	MESSAGE_END();
}

void CHuman_ZB2R::InitHUD2() const
{
	MESSAGE_BEGIN(MSG_ONE, gmsgZB2Msg, NULL, m_pPlayer->pev);
	WRITE_BYTE(ZB2_MESSAGE_SKILL_INIT);
	WRITE_BYTE(ZOMBIE_CLASS_HUMAN);
	WRITE_BYTE(m_pPlayer->m_bIsSkillHeadK2x ? ZOMBIE_SKILL_KNIFE2X : ZOMBIE_SKILL_EMPTY);
	WRITE_BYTE(m_pPlayer->m_bIsSkillHeal ? ZOMBIE_SKILL_HEAL : ZOMBIE_SKILL_EMPTY);
	MESSAGE_END();
}

void CHuman_ZB2R::Think()
{
	if(ENABLE_SPRINT)
		pimpl->spd.Think();

	if(ENABLE_HEADSHOT)
		pimpl->hs.Think();

	if(ENABLE_KNIFE2X)
		pimpl->k2x.Think();
}

void CHuman_ZB2R::ResetMaxSpeed() const
{
	if (ENABLE_KNIFE2X)
		pimpl->spd.ResetMaxSpeed();
}

ZombieSkillStatus CHuman_ZB2R::GetSkillStatus(ZombieSkillSlot which) const
{
	if (which == SKILL_SLOT_1 && ENABLE_SPRINT)
		return pimpl->spd.GetStatus();
	if (which == SKILL_SLOT_2 && ENABLE_HEADSHOT)
		return pimpl->hs.GetStatus();
	if (which == SKILL_SLOT_3 && ENABLE_KNIFE2X)
		return pimpl->k2x.GetStatus();
	return SKILL_STATUS_USED;
}

CHuman_ZB2R::~CHuman_ZB2R() = default;
