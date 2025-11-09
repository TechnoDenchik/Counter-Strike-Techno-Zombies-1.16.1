/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"
#include "hud_sub_impl.h"
#include "parsemsg.h"

#include "hidden.h"
#include "JokerKill.h"
#include "RespawnBarH.h"
#include "../dlls/gamemode/hidden/hidden_const.h"
#include <vector>

class CHudHidden::impl_t
	: public THudSubDispatcher<CHudHiddenKillWEBM, CHudHidRespawnBar>
{
public:
};

DECLARE_MESSAGE(m_hid, HIDMsg)

int CHudHidden::MsgFunc_HIDMsg(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);

	auto type = static_cast<HiddenModType>(buf.ReadByte());

	int time = buf.ReadByte();
	float timeFloat = (float)time;

	switch (type)
	{
		case HIDDEN_MSG_RESPAWN:
		{
			pimpl->get<CHudHidRespawnBar>().StartRespawn(timeFloat);
			break;
		}
		case HIDDEN_MSG_KILL:
		{
			pimpl->get<CHudHiddenKillWEBM>().SetWebm();
			break;
		}
		case HIDDEN_MSG_MASK:
		{
			
			break;
		}
	}
	return 1;
}

int CHudHidden::Init()
{
	pimpl = new CHudHidden::impl_t;

	gHUD.AddHudElem(this);

	HOOK_MESSAGE(HIDMsg);

	return 1;
}

int CHudHidden::VidInit()
{
	pimpl->for_each(&IBaseHudSub::VidInit);

	return 1;
}

int CHudHidden::Draw(float time)
{
	pimpl->for_each(&IBaseHudSub::Draw, time);
	return 1;
}

void CHudHidden::Think()
{
	pimpl->for_each(&IBaseHudSub::Think);
}

void CHudHidden::Reset()
{
	pimpl->for_each(&IBaseHudSub::Reset);
}

void CHudHidden::InitHUDData()
{
	pimpl->for_each(&IBaseHudSub::InitHUDData);
}

void CHudHidden::Shutdown()
{
	delete pimpl;
	pimpl = nullptr;
}