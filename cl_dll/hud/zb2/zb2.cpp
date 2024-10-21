﻿/*
zb2.cpp - CSMoE Client HUD : Zombie Mod 2
Copyright (C) 2019 Moemod Yanase

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"

#include "parsemsg.h"

#include "zb2.h"
#include "Winhud.h"
#include "Textset2.h"
#include "zb3/TextSet.h"
#include "zb2_skill.h"
#include "hud_sub_impl.h"
#include "player/player_const.h"

#include "gamemode/zb2/zb2_const.h"
#include "gamemode/interface/interface_const.h"
#include <vector>

class CHudZB2_impl_t : public THudSubDispatcher<CHudZB2_Skill, CHudWinhudZB1, CHudTextZB1, CHudTextZB3, CHudInfection, CHudMakeZombies>
{
public:
	SharedTexture m_pTexture_RageRetina;
	SharedTexture m_pTexture_SprintRetina;
	SharedTexture m_pTexture_DamageDoubleRetina;
	
	std::vector<CHudRetina::MagicNumber> m_RetinaIndexes;
};

DECLARE_MESSAGE(m_ZB2, ZB2Msg)
//DECLARE_MESSAGE(m_ZB2, SupplyText)
DECLARE_MESSAGE(m_ZB2, ZB3RenMsg)

int CHudZB2::MsgFunc_ZB2Msg(const char *pszName, int iSize, void *pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);
	

	auto type = static_cast<ZB2MessageType>(buf.ReadByte());
	switch (type)
	{

		case ZB2_MESSAGE_HEALTH_RECOVERY:
		{
			pimpl->get<CHudZB2_Skill>().OnHealthRecovery();
			break;
		}

		case ZB2_MESSAGE_SKILL_INIT:
		{
			ZombieClassType zclass = ZOMBIE_CLASS_HUMAN;
			if(!buf.Eof())
				zclass = static_cast<ZombieClassType>(buf.ReadByte());
			ZombieSkillType skills[4]{};
			for (int i = 0; i < 4 && !buf.Eof(); ++i)
				skills[i] = static_cast<ZombieSkillType>(buf.ReadByte());
			pimpl->get<CHudZB2_Skill>().OnSkillInit(zclass, skills[0], skills[1], skills[2], skills[3]);

			for (auto x : pimpl->m_RetinaIndexes)
			{
				gHUD.m_Retina.RemoveItem(x);
			}
			pimpl->m_RetinaIndexes.clear();
			break;
		}

		case ZB2_MESSAGE_SKILL_ACTIVATE:
		{
			auto skilltype = static_cast<ZombieSkillType>(buf.ReadByte());
			float flHoldTime = buf.ReadShort();
			float flFreezeTime = buf.ReadShort();
			pimpl->get<CHudZB2_Skill>().OnSkillActivate(skilltype, flHoldTime, flFreezeTime);
			if (skilltype == ZOMBIE_SKILL_CRAZY || skilltype == ZOMBIE_SKILL_CRAZY2)
				pimpl->m_RetinaIndexes.push_back(gHUD.m_Retina.AddItem(pimpl->m_pTexture_RageRetina, CHudRetina::RETINA_DRAW_TYPE_BLINK, flHoldTime));
			else if (skilltype == ZOMBIE_SKILL_SPRINT)
				pimpl->m_RetinaIndexes.push_back(gHUD.m_Retina.AddItem(pimpl->m_pTexture_SprintRetina, CHudRetina::RETINA_DRAW_TYPE_BLINK | CHudRetina::RETINA_DRAW_TYPE_QUARTER, flHoldTime));
			else if (skilltype == ZOMBIE_SKILL_HEADSHOT || skilltype == ZOMBIE_SKILL_KNIFE2X)
				pimpl->m_RetinaIndexes.push_back(gHUD.m_Retina.AddItem(pimpl->m_pTexture_DamageDoubleRetina, CHudRetina::RETINA_DRAW_TYPE_BLINK | CHudRetina::RETINA_DRAW_TYPE_QUARTER, flHoldTime));
		
			break;
		}
	
		case ZB2_MESSAGE_WINHUDHM:
		{
			pimpl->get<CHudWinhudZB1>().WinHuman();
			break;
		}
		case ZB2_MESSAGE_WINHUDZB:
		{
			pimpl->get<CHudWinhudZB1>().WinZombie();
			break;
		}
		case ZB3_MESSAGE_USED_STATUS:
		{
			pimpl->get<CHudTextZB3>().Settext();
			break;
		}
		case ZOMBIE_INFECTION:
		{
			pimpl->get<CHudInfection>().infected();
			break;
		}
		case MAKE_ZOMBIES:
		{
			pimpl->get<CHudMakeZombies>().Make();
			break;
		}
		case ZB2_MESSAGE_ALARM:
		{
			int iType = buf.ReadByte();
			float flValue = buf.ReadCoord();
			int iValue2 = buf.ReadByte();
			SendAlarmState(iType, flValue, iValue2);
			break;
		}
	}
	return 1;
}

int CHudZB2::Init()
{
	pimpl = new CHudZB2_impl_t;

	gHUD.AddHudElem(this);

	HOOK_MESSAGE(ZB2Msg);
//	HOOK_MESSAGE(SupplyText);
	HOOK_MESSAGE(ZB3RenMsg);

	return 1;
}

int CHudZB2::VidInit()
{
	pimpl->for_each(&IBaseHudSub::VidInit);

	R_InitTexture(pimpl->m_pTexture_RageRetina, "resource/zb3/zombicrazy");
	R_InitTexture(pimpl->m_pTexture_SprintRetina, "resource/zb3/zombispeedup");
	R_InitTexture(pimpl->m_pTexture_DamageDoubleRetina, "resource/zb3/damagedouble");
	return 1;
}

int CHudZB2::MsgFunc_ZB3RenMsg(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);
	int type = buf.ReadByte();
	int time = buf.ReadByte();

	pimpl->get<CHudTextZB1>().renaining(time);

	switch (type)
	{
		case ZB3_REN_MSG:
		{
			pimpl->get<CHudTextZB1>().Settext();
			break;
		}
	}

	return 1;
}

int CHudZB2::Draw(float time)
{
	pimpl->for_each(&IBaseHudSub::Draw, time);
	return 1;
}

void CHudZB2::Think()
{
	pimpl->for_each(&IBaseHudSub::Think);
}

void CHudZB2::Reset()
{
	pimpl->for_each(&IBaseHudSub::Reset);
}

void CHudZB2::InitHUDData()
{
	pimpl->for_each(&IBaseHudSub::InitHUDData);
}

void CHudZB2::Shutdown()
{
	delete pimpl;
	pimpl = nullptr;
}

bool CHudZB2::ActivateSkill(int iSlot)
{
	if (iSlot == 5)
	{
		gEngfuncs.pfnClientCmd("MoE_HumanSkill1");
		return true;
	}
	else if (iSlot == 6)
	{
		gEngfuncs.pfnClientCmd("MoE_HumanSkill2");
		return true;
	}
	else if (iSlot == 7)
	{
		gEngfuncs.pfnClientCmd("MoE_HumanSkill3");
		return true;
	}
	else if (iSlot == 8)
	{
		gEngfuncs.pfnClientCmd("MoE_HumanSkill4");
		return true;
	}

	return false;
}

void CHudZB2::SendAlarmState(int iType, float flValue, int iValue2)
{
	if (!iType)
	{
		m_flAliveTime = 0.0;
		for (int i = 0; i < MAX_CLIENTS + 1; i++)
		{
			//g_fLastAssist[(int)flValue][i] = 0.0;
		}
	}
	else
	{
		switch (iType)
		{
		case 1:
		{
			m_flAliveTime = gHUD.m_flTime + flValue;
			break;
		}
		case 2:
		{
			if (flValue)
			{
				if ((int)flValue == gEngfuncs.GetLocalPlayer()->index)
					NewAlarm().SetAlarm(ALARM_ZOMBIETANKER);
			}
			break;
		}
		case 3:
		{
			//g_fLastAssist[(int)flValue][gEngfuncs.GetLocalPlayer()->index] = gHUD.m_flTime + 5.0f;
			break;
		}
		case 4:
		{
			if ((int)flValue == gEngfuncs.GetLocalPlayer()->index)
				NewAlarm().SetAlarm(ALARM_ATTACKER);
			break;
		}
		case 5:
		{
			if ((int)flValue == gEngfuncs.GetLocalPlayer()->index)
				NewAlarm().SetAlarm(ALARM_BESTMOMENT);
			break;
		}
		case 6:
		{
			if ((int)flValue == gEngfuncs.GetLocalPlayer()->index)
				NewAlarm().SetAlarm(ALARM_HOLDOUT);
			break;
		}
		case 7:
		{
			g_PlayerExtraInfoEx[iValue2].assisttime[1][(int)flValue] = gHUD.m_flTime + 5.0f;
			break;
		}
		case 8:
		{
			if ((int)flValue == gEngfuncs.GetLocalPlayer()->index)
				NewAlarm().SetAlarm(ALARM_DROPWEAPON);
			break;
		}
		case 9:
		{
			if ((int)flValue == gEngfuncs.GetLocalPlayer()->index)
				NewAlarm().SetAlarm(ALARM_HIDE);
			break;
		}
		case 10:
		{
			float flRecovery = flValue;
			if (m_flRecoveryAmount < 4500.0)
				m_flRecoveryAmount += flRecovery;
			else
			{
				m_flRecoveryAmount = 0.0;
				if (iValue2 == gEngfuncs.GetLocalPlayer()->index)
					NewAlarm().SetAlarm(ALARM_HEALER);
			}
			break;
		}
		case 11:
		{
			if (iValue2 == gEngfuncs.GetLocalPlayer()->index)
				NewAlarm().SetAlarm(ALARM_BATPULL);
			break;
		}
		}
	}
}