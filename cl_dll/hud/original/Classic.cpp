#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"

#include "parsemsg.h"

#include "Classic.h"
#include "hud_sub_impl.h"
#include "player/player_const.h"
#include "gamemode/interface/interface_const.h"
#include "TextOrig.h"
#include "OrigWinhud.h"
#include <vector>

class CHudClassic_impl_t : public THudSubDispatcher<CHudGameStart, CHudBombPlant, CHudBombDefuse, CHudOrigWins, CHudRoundDraw>
{
public:
};

DECLARE_MESSAGE(m_CLS, StartGame)
DECLARE_MESSAGE(m_CLS, BombPlant)
DECLARE_MESSAGE(m_CLS, BombDefuse)
DECLARE_MESSAGE(m_CLS, CTWIN)
DECLARE_MESSAGE(m_CLS, TRWIN)
DECLARE_MESSAGE(m_CLS, RoundDraw)

int CHudClassic::MsgFunc_StartGame(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);

	auto type = static_cast<INTMessage>(buf.ReadByte());
	switch (type)
	{
		case ORIG_START_MSG:
		{
			pimpl->get<CHudGameStart>().Settext();
			break;
		}
	}
	return 1;
}

int CHudClassic::MsgFunc_BombPlant(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);

	auto type = static_cast<INTMessage>(buf.ReadByte());
	switch (type)
	{
		case ORIG_BOMB_MSG:
		{
			pimpl->get<CHudBombPlant>().Settext();
			break;
		}
	}
	return 1;
}

int CHudClassic::MsgFunc_BombDefuse(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);

	auto type = static_cast<INTMessage>(buf.ReadByte());
	switch (type)
	{
		case ORIG_BOMB2_MSG:
		{
			pimpl->get<CHudBombDefuse>().Settext();
			break;
		}
	}
	return 1;
}

int CHudClassic::MsgFunc_CTWIN(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);

	auto type = static_cast<INTMessage>(buf.ReadByte());
	switch (type)
	{
		case ORIG_CTWIN_MSG:
		{
			pimpl->get<CHudOrigWins>().winsct();
			break;
		}
	}
	return 1;
}

int CHudClassic::MsgFunc_TRWIN(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);

	auto type = static_cast<INTMessage>(buf.ReadByte());
	switch (type)
	{
		case ORIG_TRWIN_MSG:
		{
			pimpl->get<CHudOrigWins>().winst();
			break;
		}
	}
	return 1;
}

int CHudClassic::MsgFunc_RoundDraw(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);

	auto type = static_cast<INTMessage>(buf.ReadByte());
	switch (type)
	{
	case ORIG_RDRAW_MSG:
	{
		pimpl->get<CHudRoundDraw>().Settext();
		break;
	}
	}
	return 1;
}

int CHudClassic::Init()
{
	pimpl = new CHudClassic_impl_t;

	gHUD.AddHudElem(this);

	HOOK_MESSAGE(StartGame);
	HOOK_MESSAGE(BombPlant);
	HOOK_MESSAGE(BombDefuse);
	HOOK_MESSAGE(CTWIN);
	HOOK_MESSAGE(TRWIN);
	HOOK_MESSAGE(RoundDraw);

	return 1;
}

int CHudClassic::VidInit()
{
	pimpl->for_each(&IBaseHudSub::VidInit);

	return 1;
}

int CHudClassic::Draw(float time)
{
	pimpl->for_each(&IBaseHudSub::Draw, time);
	return 1;
}

void CHudClassic::Think()
{
	pimpl->for_each(&IBaseHudSub::Think);
}

void CHudClassic::Reset()
{
	pimpl->for_each(&IBaseHudSub::Reset);
}

void CHudClassic::InitHUDData()
{
	pimpl->for_each(&IBaseHudSub::InitHUDData);
}

void CHudClassic::Shutdown()
{
	delete pimpl;
	pimpl = nullptr;
}