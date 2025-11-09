#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"

#include "parsemsg.h"

#include "dm.h"
#include "hud_sub_impl.h"
#include "player/player_const.h"
#include "gamemode/interface/interface_const.h"
#include "../dlls/gamemode/gd/gd_const.h"
#include "gd/RespawnBar.h"
#include <vector>

class CHudDeathMatch_impl_t : public THudSubDispatcher<CHudGDRespawnBar>
{
public:
};

DECLARE_MESSAGE(m_dm, RespawnBarDm)


int CHudDeathMatch::MsgFunc_RespawnBarDm(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);

	auto type = static_cast<INTMessage>(buf.ReadByte());

	int time = buf.ReadByte();
	float timeFloat = (float)time;

	switch (type)
	{
	case GD_RESPAWN_BAR:
	{
		pimpl->get<CHudGDRespawnBar>().StartRespawn(timeFloat);
		break;
	}
	}
	return 1;
}

int CHudDeathMatch::Init()
{
	pimpl = new CHudDeathMatch_impl_t;

	gHUD.AddHudElem(this);

	HOOK_MESSAGE(RespawnBarDm);

	return 1;
}

int CHudDeathMatch::VidInit()
{
	pimpl->for_each(&IBaseHudSub::VidInit);

	return 1;
}

int CHudDeathMatch::Draw(float time)
{
	pimpl->for_each(&IBaseHudSub::Draw, time);
	return 1;
}

void CHudDeathMatch::Think()
{
	pimpl->for_each(&IBaseHudSub::Think);
}

void CHudDeathMatch::Reset()
{
	pimpl->for_each(&IBaseHudSub::Reset);
}

void CHudDeathMatch::InitHUDData()
{
	pimpl->for_each(&IBaseHudSub::InitHUDData);
}

void CHudDeathMatch::Shutdown()
{
	delete pimpl;
	pimpl = nullptr;
}