/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"

#include "parsemsg.h"

#include "hud_sub_impl.h"

#include "zsh.h"
#include "ZSHScoreboard.h"
#include "baseresourcesmenu.h"
#include "buildmenu.h"
#include "buymenu.h"
#include "menuspecifed.h"
#include "skillsmenu.h"
#include "zsh_kill.h"
#include "messageset.h"

#include "gamemode/zsh/zsh_const.h"

class CHudZSH::impl_t
	: public THudSubDispatcher< CHudZSHScoreboard, CHudZSHKill, CHudTextZSH, CHudTextDayZSH, CHudWinhudZSH> {};

DECLARE_MESSAGE(m_ZSH, ZSHMsg)
DECLARE_MESSAGE(m_ZSH, ZSHUpdateDay)
DECLARE_MESSAGE(m_ZSH, ZSHUpdateTime)
DECLARE_MESSAGE(m_ZSH, ZSHUpdateRes)
DECLARE_MESSAGE(m_ZSH, ZSHUpdateResHome)
DECLARE_MESSAGE(m_ZSH, ZSHMsgText)
DECLARE_MESSAGE(m_ZSH, ZSHMsgTextNextDay)
DECLARE_MESSAGE(m_ZSH, ZSHMsgRound)
DECLARE_MESSAGE(m_ZSH, ZSHMentalityHealth)
DECLARE_MESSAGE(m_ZSH, ZSHResPos)
DECLARE_MESSAGE(m_ZSH, ZSHRes2Pos)
DECLARE_MESSAGE(m_ZSH, ZSHZmPos)

int CHudZSH::MsgFunc_ZSHMsg(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);
	auto type = static_cast<ZSHTipKill>(buf.ReadByte());
	switch (type)
	{
		case ZSHKill:
			pimpl->get<CHudZSHKill>().OnKillMessage();
		break;
	}
	return 1;
}

int CHudZSH::MsgFunc_ZSHMsgText(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);
	int type = buf.ReadByte();
	int time = buf.ReadByte();

	pimpl->get<CHudTextZSH>().renaining(time);

	switch (type)
	{
		case ZSHMessageTime:
		{
			pimpl->get<CHudTextZSH>().Settext();
			break;
		}
	}
	return 1;
}

int CHudZSH::MsgFunc_ZSHUpdateDay(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);
	int type = buf.ReadByte(); // reserved.
	int dayses = buf.ReadByte();
	pimpl->get<CHudZSHScoreboard>().UpdateDay(dayses);

	return 1;
}

int CHudZSH::MsgFunc_ZSHMsgTextNextDay(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);
	int type = buf.ReadByte(); // reserved.
	int dayses = buf.ReadByte();
	pimpl->get<CHudTextDayZSH>().nexting(dayses);

	switch (type)
	{
		case ZSHMessagebox:
		{
			pimpl->get<CHudTextDayZSH>().Settext();
			break;
		}
	}

	return 1;
}

int CHudZSH::MsgFunc_ZSHUpdateRes(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);
	int type = buf.ReadByte(); // reserved.
	int woods = buf.ReadByte();
	int metal = buf.ReadByte();
	int maxwood = buf.ReadByte();
	int maxmeat = buf.ReadByte();
	int energy = buf.ReadByte();
	int maxenergy = buf.ReadByte();
	pimpl->get<CHudZSHScoreboard>().UpdateRes(woods, metal, maxwood, maxmeat, energy, maxenergy);

	return 1;
}

int CHudZSH::MsgFunc_ZSHUpdateTime(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);
	int type = buf.ReadByte();
	int daytimes = buf.ReadByte();
	int nighttimes = buf.ReadByte();
	bool daytimer = buf.ReadByte();
	int dayseconds = buf.ReadByte();
	int nightsecond = buf.ReadByte();
	pimpl->get<CHudZSHScoreboard>().UpdateTime(daytimes, nighttimes, daytimer, dayseconds, nightsecond);

	return 1;
}

int CHudZSH::MsgFunc_ZSHUpdateResHome(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);
	int type = buf.ReadByte();
	int homewood = buf.ReadByte();
	int homemaxwood = buf.ReadByte();
	int homemeat = buf.ReadByte();
	int homemaxmeat = buf.ReadByte();
	int mentalityhealth = buf.ReadByte();
	pimpl->get<CHudZSHScoreboard>().UpdateResHome(homewood, homemaxwood, homemeat, homemaxmeat, mentalityhealth);

	return 1;
}

int CHudZSH::MsgFunc_ZSHMentalityHealth(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);
	int type = buf.ReadByte();
	int mentalityhealth = buf.ReadByte();
	pimpl->get<CHudZSHScoreboard>().UpdateMentality(mentalityhealth);

	return 1;
}

int CHudZSH::MsgFunc_ZSHResPos(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);
	int type = buf.ReadByte();
	int mentalityhealth = buf.ReadByte();
	pimpl->get<CHudZSHScoreboard>().UpdateMentality(mentalityhealth);

	return 1;
}

int CHudZSH::MsgFunc_ZSHRes2Pos(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);
	int type = buf.ReadByte();
	int mentalityhealth = buf.ReadByte();
	pimpl->get<CHudZSHScoreboard>().UpdateMentality(mentalityhealth);

	return 1;
}

int CHudZSH::MsgFunc_ZSHZmPos(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);
	int type = buf.ReadByte();
	int mentalityhealth = buf.ReadByte();
	pimpl->get<CHudZSHScoreboard>().UpdateMentality(mentalityhealth);

	return 1;
}

int CHudZSH::MsgFunc_ZSHMsgRound(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);
	auto type = static_cast<ZSHMessageUi>(buf.ReadByte());
	switch (type)
	{
		case ZSHSurvivallose:
			pimpl->get<CHudWinhudZSH>().SurvivalLose();
			break;

		case ZSHSurvivalwin:
			pimpl->get<CHudWinhudZSH>().SurvivalWin();
			break;
	}
	return 1;
}

int CHudZSH::Init(void)
{
	pimpl = new CHudZSH::impl_t;

	gHUD.AddHudElem(this);

	HOOK_MESSAGE(ZSHMsg);
	HOOK_MESSAGE(ZSHUpdateDay);	
	HOOK_MESSAGE(ZSHUpdateTime);
	HOOK_MESSAGE(ZSHUpdateRes);
	HOOK_MESSAGE(ZSHUpdateResHome);
	HOOK_MESSAGE(ZSHMsgText);
	HOOK_MESSAGE(ZSHMsgTextNextDay);
	HOOK_MESSAGE(ZSHMsgRound);
	HOOK_MESSAGE(ZSHMentalityHealth);
	HOOK_MESSAGE(ZSHResPos);
	HOOK_MESSAGE(ZSHRes2Pos);
	HOOK_MESSAGE(ZSHZmPos);
	return 1;
}

int CHudZSH::VidInit(void)
{
	pimpl->for_each(&IBaseHudSub::VidInit);
	return 1;
}

int CHudZSH::Draw(float time)
{
	pimpl->for_each(&IBaseHudSub::Draw, time);
	return 1;
}

void CHudZSH::Think(void)
{
	pimpl->for_each(&IBaseHudSub::Think);
}

void CHudZSH::Reset(void)
{
	pimpl->for_each(&IBaseHudSub::Reset);
}

void CHudZSH::InitHUDData(void)
{
	pimpl->for_each(&IBaseHudSub::InitHUDData);
}

void CHudZSH::Shutdown(void)
{
	delete pimpl;
	pimpl = nullptr;
}