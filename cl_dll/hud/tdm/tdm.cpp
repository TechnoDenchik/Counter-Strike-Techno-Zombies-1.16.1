#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"

#include "parsemsg.h"

#include "tdm.h"
#include "hud_sub_impl.h"
#include "player/player_const.h"
#include "gamemode/interface/interface_const.h"
#include "../dlls/gamemode/gd/gd_const.h"
#include "gd/RespawnBar.h"
#include <vector>

class CHudTeamDeathMatch_impl_t : public THudSubDispatcher<CHudGDRespawnBar>
{
public:
};

DECLARE_MESSAGE(m_tdm, RespawnBarTdm)


int CHudTeamDeathMatch::MsgFunc_RespawnBarTdm(const char* pszName, int iSize, void* pbuf)
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

int CHudTeamDeathMatch::Init()
{
	pimpl = new CHudTeamDeathMatch_impl_t;

	gHUD.AddHudElem(this);

	HOOK_MESSAGE(RespawnBarTdm);

	return 1;
}

int CHudTeamDeathMatch::VidInit()
{
	pimpl->for_each(&IBaseHudSub::VidInit);

	return 1;
}

int CHudTeamDeathMatch::Draw(float time)
{
	pimpl->for_each(&IBaseHudSub::Draw, time);
	return 1;
}

void CHudTeamDeathMatch::Think()
{
	pimpl->for_each(&IBaseHudSub::Think);
}

void CHudTeamDeathMatch::Reset()
{
	pimpl->for_each(&IBaseHudSub::Reset);
}

void CHudTeamDeathMatch::InitHUDData()
{
	pimpl->for_each(&IBaseHudSub::InitHUDData);
}

void CHudTeamDeathMatch::Shutdown()
{
	delete pimpl;
	pimpl = nullptr;
}