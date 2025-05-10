#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"

#include "parsemsg.h"

#include "hud_sub_impl.h"

#include "zbs.h"
#include "zbs_level.h"
#include "zbs_scoreboard.h"
#include "zbs_roundclear.h"

#include "zbs_kill.h"

#include "gamemode/zbs/zbs_const.h"

class CHudZBS::impl_t
	: public THudSubDispatcher<CHudZBSLevel, CHudZBSScoreBoard, CHudZBSKill, CHudZBSRoundClear, CHudZBSMsgLevel, CHudTextZBS> {};

DECLARE_MESSAGE(m_ZBS, ZBSTip)
DECLARE_MESSAGE(m_ZBS, ZBSLevel)
DECLARE_MESSAGE(m_ZBS, ZBSMsgLevel)
DECLARE_MESSAGE(m_ZBS, ZBSRenMsg)

int CHudZBS::MsgFunc_ZBSTip(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);
	ZBSTipType type = static_cast<ZBSTipType>(buf.ReadByte());
	int szkill = buf.ReadByte();

	switch (type)
	{
	case ZBS_TIP_KILL:
		pimpl->get<CHudZBSKill>().OnKillType(szkill);
		pimpl->get<CHudZBSKill>().OnKillMessage();
		break;
	case ZBS_TIP_ROUNDCLEAR:
		pimpl->get<CHudZBSRoundClear>().OnRoundClear();
		break;
	case ZBS_TIP_ROUNDFAIL:
		pimpl->get<CHudZBSRoundClear>().OnRoundFail();
		break;
	}

	return 1;
}

int CHudZBS::MsgFunc_ZBSLevel(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);
	int type = buf.ReadByte(); // reserved.
	int hp = buf.ReadByte();
	int att = buf.ReadByte();
	int wall = buf.ReadByte();

	pimpl->get<CHudZBSLevel>().UpdateLevel(hp, att, wall);

	return 1;
}

int CHudZBS::MsgFunc_ZBSMsgLevel(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);
	int type = buf.ReadByte(); // reserved.
	int lvl = buf.ReadByte();

	pimpl->get<CHudZBSMsgLevel>().UpdateLevel(lvl);
	pimpl->get<CHudZBSMsgLevel>().Setlv();

	return 1;
}

int CHudZBS::MsgFunc_ZBSRenMsg(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);
	int type = buf.ReadByte();
	int time = buf.ReadByte();

	pimpl->get<CHudTextZBS>().renaining(time);

	switch (type)
	{
	case ZBS_TIP_RENAINING:
	{
		pimpl->get<CHudTextZBS>().Settext();
		break;
	}
	}

	return 1;
}

int CHudZBS::Init(void)
{
	pimpl = new CHudZBS::impl_t;

	gHUD.AddHudElem(this);

	HOOK_MESSAGE(ZBSTip);
	HOOK_MESSAGE(ZBSLevel);
	HOOK_MESSAGE(ZBSMsgLevel);
	HOOK_MESSAGE(ZBSRenMsg);

	return 1;
}

int CHudZBS::VidInit(void)
{
	pimpl->for_each(&IBaseHudSub::VidInit);
	return 1;
}

int CHudZBS::Draw(float time)
{
	pimpl->for_each(&IBaseHudSub::Draw, time);
	return 1;
}

void CHudZBS::Think(void)
{
	pimpl->for_each(&IBaseHudSub::Think);
}

void CHudZBS::Reset(void)
{
	pimpl->for_each(&IBaseHudSub::Reset);
}

void CHudZBS::InitHUDData(void)
{
	pimpl->for_each(&IBaseHudSub::InitHUDData);
}

void CHudZBS::Shutdown(void)
{
	delete pimpl;
	pimpl = nullptr;
}

