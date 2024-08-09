/*
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


#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"

#include "parsemsg.h"

#include "interface.h"
#include "interface_used.h"
#include "hud_sub_impl.h"
#include "player/player_const.h"

#include "gamemode/interface/interface_const.h"

#include <vector>

class CHudInterface_impl_t : public THudSubDispatcher<CHudInterface_used>
{
public:
	SharedTexture m_pTexture_RageRetina;
	SharedTexture m_pTexture_SprintRetina;
	SharedTexture m_pTexture_DamageDoubleRetina;
	
	std::vector<CHudRetina::MagicNumber> m_RetinaIndexes;
};

DECLARE_MESSAGE(m_HudInterface, HudInterfaceMsg)

int CHudInterface::MsgFunc_HudInterfaceMsg(const char *pszName, int iSize, void *pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);
	
	auto type = static_cast<INTMessageType>(buf.ReadByte());
	switch (type)
	{
	case ZB2_MESSAGE_HEALTH_RECOVERY:
	{
		pimpl->get<CHudInterface_used>().OnHealthRecovery();
		break;
	}
	case ZB2_MESSAGE_SKILL_INIT:
	{
		INTClassType zclass = ZOMBIE_CLASS_HUMAN;
		if(!buf.Eof())
			zclass = static_cast<INTClassType>(buf.ReadByte());
		INTSkillType skills[4]{};
		for (int i = 0; i < 4 && !buf.Eof(); ++i)
			skills[i] = static_cast<INTSkillType>(buf.ReadByte());
		pimpl->get<CHudInterface_used>().OnSkillInit(zclass, skills[0], skills[1], skills[2], skills[3]);

		// remove retinas...
		for (auto x : pimpl->m_RetinaIndexes)
		{
			gHUD.m_Retina.RemoveItem(x);
		}
		pimpl->m_RetinaIndexes.clear();
		break;
	}
	case ZB2_MESSAGE_SKILL_ACTIVATE:
	{
		auto skilltype = static_cast<INTSkillType>(buf.ReadByte());
		float flHoldTime = buf.ReadShort();
		float flFreezeTime = buf.ReadShort();
		pimpl->get<CHudInterface_used>().OnSkillActivate(skilltype, flHoldTime, flFreezeTime);
		if (skilltype == ZOMBIE_SKILL_CRAZY || skilltype == ZOMBIE_SKILL_CRAZY2)
			pimpl->m_RetinaIndexes.push_back(gHUD.m_Retina.AddItem(pimpl->m_pTexture_RageRetina, CHudRetina::RETINA_DRAW_TYPE_BLINK, flHoldTime));
		else if (skilltype == ZOMBIE_SKILL_SPRINT)
			pimpl->m_RetinaIndexes.push_back(gHUD.m_Retina.AddItem(pimpl->m_pTexture_SprintRetina, CHudRetina::RETINA_DRAW_TYPE_BLINK | CHudRetina::RETINA_DRAW_TYPE_QUARTER, flHoldTime));
		else if (skilltype == ZOMBIE_SKILL_HEADSHOT || skilltype == ZOMBIE_SKILL_KNIFE2X)
			pimpl->m_RetinaIndexes.push_back(gHUD.m_Retina.AddItem(pimpl->m_pTexture_DamageDoubleRetina, CHudRetina::RETINA_DRAW_TYPE_BLINK | CHudRetina::RETINA_DRAW_TYPE_QUARTER, flHoldTime));
		
		break;
	}
	
	}
	return 1;
}

int CHudInterface::Init()
{
	pimpl = new CHudInterface_impl_t;

	gHUD.AddHudElem(this);

	HOOK_MESSAGE(HudInterfaceMsg);

	return 1;
}

int CHudInterface::VidInit()
{
	pimpl->for_each(&IBaseHudSub::VidInit);

	R_InitTexture(pimpl->m_pTexture_RageRetina, "resource/zb3/zombicrazy");
	R_InitTexture(pimpl->m_pTexture_SprintRetina, "resource/zb3/zombispeedup");
	R_InitTexture(pimpl->m_pTexture_DamageDoubleRetina, "resource/zb3/damagedouble");
	return 1;
}

int CHudInterface::Draw(float time)
{
	pimpl->for_each(&IBaseHudSub::Draw, time);
	return 1;
}

void CHudInterface::Think()
{
	pimpl->for_each(&IBaseHudSub::Think);
}

void CHudInterface::Reset()
{
	pimpl->for_each(&IBaseHudSub::Reset);
}

void CHudInterface::InitHUDData()
{
	pimpl->for_each(&IBaseHudSub::InitHUDData);
}

void CHudInterface::Shutdown()
{
	delete pimpl;
	pimpl = nullptr;
}

bool CHudInterface::ActivateSkill(int iSlot)
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
}*/
