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

class SprintSkillH : protected CZombieSkill_BaseR
{
public:
	explicit SprintSkillH(CBasePlayer * player) : CZombieSkill_BaseR(player), m_flTimeZombieSkillEffect(0.0f), m_bDebuffStatus(false) {}

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

class HeadShotSkillH : protected CZombieSkill_BaseR
{
public:
	explicit HeadShotSkillH(CBasePlayer * player) : CZombieSkill_BaseR(player) {}

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

class Knife2xSkillH : protected CZombieSkill_BaseR
{
public:
	explicit Knife2xSkillH(CBasePlayer * player) : CZombieSkill_BaseR(player) {}

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

class HealSkillH : protected CZombieSkill_BaseR
{
public:
	explicit HealSkillH(CBasePlayer* player) : CZombieSkill_BaseR(player) {}

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
		m_flTimeZombieSkillNext = 10.f;
		
		usingskill = true;
		countused = 0;
	}

	void Think() override
	{
		if (usingskill == true)
		{

			if (countused < 5)
			{
				if (gpGlobals->time - tWorldUsing < 99.0f)
				{
					tDeltaUsing += gpGlobals->time - tWorldUsing;
				}
				if (tNextUsing > 1.0f || (gpGlobals->time - tWorldUsing > 1.0f) || tDeltaUsing > 1.0f)
				{
					tNextUsing = 0.0f;
					tDeltaUsing = 0.0f;

					countused += 1;

					CLIENT_COMMAND(m_pPlayer->edict(), "spk zb3/td_buff.wav\n");

					MESSAGE_BEGIN(MSG_ONE, gmsgZB2Msg, NULL, m_pPlayer->pev);
					WRITE_BYTE(ZB2_MESSAGE_SKILL_ACTIVATE);
					WRITE_BYTE(ZOMBIE_SKILL_HEAL);
					WRITE_SHORT(1);
					WRITE_SHORT(10);
					MESSAGE_END();

					m_pPlayer->pev->health += 100;

				}
				tWorldUsing = gpGlobals->time;
			}
			else
			{
				usingskill = false;
			}
		}
		if (m_iZombieSkillStatus == SKILL_STATUS_USING && gpGlobals->time > m_flTimeZombieSkillEnd)
		{
			OnSkillEnd();
		}

		if (m_iZombieSkillStatus == SKILL_STATUS_FREEZING && gpGlobals->time > m_flTimeZombieSkillNext)
		{
			m_iZombieSkillStatus = SKILL_STATUS_READY;
			OnSkillReady();
		}
	}

	void OnSkillEnd() override
	{
		m_iZombieSkillStatus = SKILL_STATUS_FREEZING;
	}	

	bool usingskill;

	duration_t tNextUsing;
	time_point_t tWorldUsing;
	duration_t tDeltaUsing;

	time_point_t countused;

	using CZombieSkill_BaseR::GetStatus;
	using CZombieSkill_BaseR::Think;
};









class CHuman_ZB5::impl_t : public BasePlayerExtra
{
public:
	explicit impl_t(CBasePlayer *player) : BasePlayerExtra(player), spd(player), hs(player), k2x(player), hlup(player) {}
	SprintSkillH spd;
	HeadShotSkillH hs;
	Knife2xSkillH k2x;
	HealSkillH hlup;
};

CHuman_ZB5::CHuman_ZB5(CBasePlayer * player) : CHuman_ZB2R(player), pimpl(std::unique_ptr<impl_t>(new impl_t(player)))
{
	m_pPlayer->m_bIsZombie = false;
	m_pPlayer->m_bNotKilled = false;
	m_pPlayer->m_bIsVIP = false;
	m_pPlayer->m_bIsHero = false;
	m_pPlayer->pev->body = 0;

	static char szModelPath[64];

	const char* szModel = "";

	switch (RANDOM_LONG(1,20))
	{
	case 1:
		szModel = "gerrard";
		SET_CLIENT_KEY_VALUE(m_pPlayer->entindex(), GET_INFO_BUFFER(m_pPlayer->edict()), "model", const_cast<char*>(szModel));


		Q_snprintf(szModelPath, sizeof(szModelPath), "models/player/%s/%s.mdl", szModel, szModel);
		m_pPlayer->SetNewPlayerModel(szModelPath);
		break;
	case 2:
		szModel = "arctic";
		SET_CLIENT_KEY_VALUE(m_pPlayer->entindex(), GET_INFO_BUFFER(m_pPlayer->edict()), "model", const_cast<char*>(szModel));

		
		Q_snprintf(szModelPath, sizeof(szModelPath), "models/player/%s/%s.mdl", szModel, szModel);
		m_pPlayer->SetNewPlayerModel(szModelPath);
		break;
	case 3:
		szModel = "davidblack";
		SET_CLIENT_KEY_VALUE(m_pPlayer->entindex(), GET_INFO_BUFFER(m_pPlayer->edict()), "model", const_cast<char*>(szModel));

		
		Q_snprintf(szModelPath, sizeof(szModelPath), "models/player/%s/%s.mdl", szModel, szModel);
		m_pPlayer->SetNewPlayerModel(szModelPath);
		break;
	case 4:
		szModel = "eta";
		SET_CLIENT_KEY_VALUE(m_pPlayer->entindex(), GET_INFO_BUFFER(m_pPlayer->edict()), "model", const_cast<char*>(szModel));

		
		Q_snprintf(szModelPath, sizeof(szModelPath), "models/player/%s/%s.mdl", szModel, szModel);
		m_pPlayer->SetNewPlayerModel(szModelPath);
		break;
	case 5:
		szModel = "fernando";
		SET_CLIENT_KEY_VALUE(m_pPlayer->entindex(), GET_INFO_BUFFER(m_pPlayer->edict()), "model", const_cast<char*>(szModel));

		
		Q_snprintf(szModelPath, sizeof(szModelPath), "models/player/%s/%s.mdl", szModel, szModel);
		m_pPlayer->SetNewPlayerModel(szModelPath);
		break;
	case 6:
		szModel = "gign";
		SET_CLIENT_KEY_VALUE(m_pPlayer->entindex(), GET_INFO_BUFFER(m_pPlayer->edict()), "model", const_cast<char*>(szModel));

	
		Q_snprintf(szModelPath, sizeof(szModelPath), "models/player/%s/%s.mdl", szModel, szModel);
		m_pPlayer->SetNewPlayerModel(szModelPath);
		break;
	case 7:
		szModel = "gsg9";
		SET_CLIENT_KEY_VALUE(m_pPlayer->entindex(), GET_INFO_BUFFER(m_pPlayer->edict()), "model", const_cast<char*>(szModel));

		
		Q_snprintf(szModelPath, sizeof(szModelPath), "models/player/%s/%s.mdl", szModel, szModel);
		m_pPlayer->SetNewPlayerModel(szModelPath);
		break;
	case 8:
		szModel = "guerilla";
		SET_CLIENT_KEY_VALUE(m_pPlayer->entindex(), GET_INFO_BUFFER(m_pPlayer->edict()), "model", const_cast<char*>(szModel));

		
		Q_snprintf(szModelPath, sizeof(szModelPath), "models/player/%s/%s.mdl", szModel, szModel);
		m_pPlayer->SetNewPlayerModel(szModelPath);
		break;
	case 9:
		szModel = "leet";
		SET_CLIENT_KEY_VALUE(m_pPlayer->entindex(), GET_INFO_BUFFER(m_pPlayer->edict()), "model", const_cast<char*>(szModel));


		Q_snprintf(szModelPath, sizeof(szModelPath), "models/player/%s/%s.mdl", szModel, szModel);
		m_pPlayer->SetNewPlayerModel(szModelPath);
		break;
	case 10:
		szModel = "magui";
		SET_CLIENT_KEY_VALUE(m_pPlayer->entindex(), GET_INFO_BUFFER(m_pPlayer->edict()), "model", const_cast<char*>(szModel));

		
		Q_snprintf(szModelPath, sizeof(szModelPath), "models/player/%s/%s.mdl", szModel, szModel);
		m_pPlayer->SetNewPlayerModel(szModelPath);
		break;
	case 11:
		szModel = "militia";
		SET_CLIENT_KEY_VALUE(m_pPlayer->entindex(), GET_INFO_BUFFER(m_pPlayer->edict()), "model", const_cast<char*>(szModel));

		
		Q_snprintf(szModelPath, sizeof(szModelPath), "models/player/%s/%s.mdl", szModel, szModel);
		m_pPlayer->SetNewPlayerModel(szModelPath);
		break;
	case 12:
		szModel = "rb";
		SET_CLIENT_KEY_VALUE(m_pPlayer->entindex(), GET_INFO_BUFFER(m_pPlayer->edict()), "model", const_cast<char*>(szModel));


		Q_snprintf(szModelPath, sizeof(szModelPath), "models/player/%s/%s.mdl", szModel, szModel);
		m_pPlayer->SetNewPlayerModel(szModelPath);
		break;
	case 13:
		szModel = "saf";
		SET_CLIENT_KEY_VALUE(m_pPlayer->entindex(), GET_INFO_BUFFER(m_pPlayer->edict()), "model", const_cast<char*>(szModel));


		Q_snprintf(szModelPath, sizeof(szModelPath), "models/player/%s/%s.mdl", szModel, szModel);
		m_pPlayer->SetNewPlayerModel(szModelPath);
		break;
	case 14:
		szModel = "sas";
		SET_CLIENT_KEY_VALUE(m_pPlayer->entindex(), GET_INFO_BUFFER(m_pPlayer->edict()), "model", const_cast<char*>(szModel));


		Q_snprintf(szModelPath, sizeof(szModelPath), "models/player/%s/%s.mdl", szModel, szModel);
		m_pPlayer->SetNewPlayerModel(szModelPath);
		break;
	case 15:
		szModel = "sozo";
		SET_CLIENT_KEY_VALUE(m_pPlayer->entindex(), GET_INFO_BUFFER(m_pPlayer->edict()), "model", const_cast<char*>(szModel));


		Q_snprintf(szModelPath, sizeof(szModelPath), "models/player/%s/%s.mdl", szModel, szModel);
		m_pPlayer->SetNewPlayerModel(szModelPath);
		break;
	case 16:
		szModel = "sdefence";
		SET_CLIENT_KEY_VALUE(m_pPlayer->entindex(), GET_INFO_BUFFER(m_pPlayer->edict()), "model", const_cast<char*>(szModel));


		Q_snprintf(szModelPath, sizeof(szModelPath), "models/player/%s/%s.mdl", szModel, szModel);
		m_pPlayer->SetNewPlayerModel(szModelPath);
		break;
	case 17:
		szModel = "scvip";
		SET_CLIENT_KEY_VALUE(m_pPlayer->entindex(), GET_INFO_BUFFER(m_pPlayer->edict()), "model", const_cast<char*>(szModel));


		Q_snprintf(szModelPath, sizeof(szModelPath), "models/player/%s/%s.mdl", szModel, szModel);
		m_pPlayer->SetNewPlayerModel(szModelPath);
		break;
	case 18:
		szModel = "spetsnaz";
		SET_CLIENT_KEY_VALUE(m_pPlayer->entindex(), GET_INFO_BUFFER(m_pPlayer->edict()), "model", const_cast<char*>(szModel));


		Q_snprintf(szModelPath, sizeof(szModelPath), "models/player/%s/%s.mdl", szModel, szModel);
		m_pPlayer->SetNewPlayerModel(szModelPath);
		break;
	case 19:
		szModel = "terror";
		SET_CLIENT_KEY_VALUE(m_pPlayer->entindex(), GET_INFO_BUFFER(m_pPlayer->edict()), "model", const_cast<char*>(szModel));


		Q_snprintf(szModelPath, sizeof(szModelPath), "models/player/%s/%s.mdl", szModel, szModel);
		m_pPlayer->SetNewPlayerModel(szModelPath);
		break;
	case 20:
		szModel = "urban";
		SET_CLIENT_KEY_VALUE(m_pPlayer->entindex(), GET_INFO_BUFFER(m_pPlayer->edict()), "model", const_cast<char*>(szModel));


		Q_snprintf(szModelPath, sizeof(szModelPath), "models/player/%s/%s.mdl", szModel, szModel);
		m_pPlayer->SetNewPlayerModel(szModelPath);
		break;
	}

	m_pPlayer->m_bNightVisionOn = false;
}

void CHuman_ZB5::ActivateSkill(ZombieSkillSlot which)
{
	if (which == SKILL_SLOT_1 && ENABLE_SPRINT)
		pimpl->spd.Activate();
	else if (which == SKILL_SLOT_2 && ENABLE_HEADSHOT)
		pimpl->hs.Activate();
	

	else if (which == SKILL_SLOT_3 && m_pPlayer->m_bIsSkillHeadK2x)
		pimpl->k2x.Activate();
	else if (which == SKILL_SLOT_4 && m_pPlayer->m_bIsSkillHeal)
		pimpl->hlup.Activate();
}

void CHuman_ZB5::InitHUD() const
{
	MESSAGE_BEGIN(MSG_ONE, gmsgZB2Msg, NULL, m_pPlayer->pev);
	WRITE_BYTE(ZB2_MESSAGE_SKILL_INIT);
	WRITE_BYTE(ZOMBIE_CLASS_HUMAN);
	WRITE_BYTE(ENABLE_SPRINT ? ZOMBIE_SKILL_SPRINT : ZOMBIE_SKILL_EMPTY);
	WRITE_BYTE(ENABLE_HEADSHOT ? ZOMBIE_SKILL_HEADSHOT : ZOMBIE_SKILL_EMPTY);
	WRITE_BYTE(m_pPlayer->m_bIsSkillHeadK2x ? ZOMBIE_SKILL_KNIFE2X : ZOMBIE_SKILL_EMPTY);
	WRITE_BYTE(m_pPlayer->m_bIsSkillHeal ? ZOMBIE_SKILL_HEAL : ZOMBIE_SKILL_EMPTY);
	MESSAGE_END();
}

void CHuman_ZB5::InitHUD2() const
{

}


void CHuman_ZB5::Think()
{
	if(ENABLE_SPRINT)
		pimpl->spd.Think();

	if(ENABLE_HEADSHOT)
		pimpl->hs.Think();

	if(m_pPlayer->m_bIsSkillHeadK2x)
		pimpl->k2x.Think();

	if (m_pPlayer->m_bIsSkillHeal)
		pimpl->hlup.Think();
}

void CHuman_ZB5::ResetMaxSpeed() const
{
	if (m_pPlayer->m_bIsSkillHeadK2x)
		pimpl->spd.ResetMaxSpeed();
}

ZombieSkillStatus CHuman_ZB5::GetSkillStatus(ZombieSkillSlot which) const
{
	if (which == SKILL_SLOT_1 && ENABLE_SPRINT)
		return pimpl->spd.GetStatus();
	if (which == SKILL_SLOT_2 && ENABLE_HEADSHOT)
		return pimpl->hs.GetStatus();
	if (which == SKILL_SLOT_3 && m_pPlayer->m_bIsSkillHeadK2x)
		return pimpl->k2x.GetStatus();
	if (which == SKILL_SLOT_4 && m_pPlayer->m_bIsSkillHeal)
		return pimpl->hlup.GetStatus();
	return SKILL_STATUS_USED;
}

CHuman_ZB5::~CHuman_ZB5() = default;