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

#include "zb5.h"
#include "zb5_morale.h"
#include "hud/legacy/hud_scoreboard_legacy.h"
#include "zb5_rage.h"
#include "player/player_const.h"
#include "zb5/TextSetZb5.h"
#include "InventorySetZb5.h"
#include "gamemode/zb5/zb5_const.h"
#include "Webmsprites.h"
#include "gamemode/interface/interface_const.h"

#include <vector>

class CHudZB5::impl_t
	: public THudSubDispatcher<CHudTextZB5, CHudText2ZB5, CInventorySetZb5, CHudWebm, CHudTextNum, CHudKillZB5>
{
public:
};

DECLARE_MESSAGE(m_ZB5, ZB5Msg)
DECLARE_MESSAGE(m_ZB5, ZB5SkillUsed)
DECLARE_MESSAGE(m_ZB5, ZB5SkillUsed2)
DECLARE_MESSAGE(m_ZB5, ZB5Inventory)
DECLARE_MESSAGE(m_ZB5, ZB5Evolevel)
DECLARE_MESSAGE(m_ZB5, ZB5Getlocationbox)
DECLARE_MESSAGE(m_ZB5, ZB5GetlocationboxK)

int CHudZB5::MsgFunc_ZB5Msg(const char *pszName, int iSize, void *pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);
	auto type = static_cast<ZB5MessageType>(buf.ReadByte());
	int frags = buf.ReadByte();

	switch (type)
	{
		case ZB5_MESSAGE_KILL:
		{
			pimpl->get<CHudKillZB5>().getgo();
			pimpl->get<CHudKillZB5>().ikills(frags);
			break;
		}
	}
	
	return 1;
}

int CHudZB5::MsgFunc_ZB5SkillUsed(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);	
	auto type = static_cast<INTMessage>(buf.ReadByte());
	int time = buf.ReadByte();

	pimpl->get<CHudTextZB5>().renaining(time);

	switch (type)
	{
		case ZB5_USED_MSG:
		{
			pimpl->get<CHudTextZB5>().Settext();
			break;
		}
	}

	return 1;
}

int CHudZB5::MsgFunc_ZB5SkillUsed2(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);
	auto type = static_cast<INTMessage>(buf.ReadByte());

	switch (type)
	{
		case ZB5_USED_MSG2:
		{
			pimpl->get<CHudText2ZB5>().Settext();
			break;
		}
	}

	return 1;
}

int CHudZB5::MsgFunc_ZB5Inventory(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);
	auto type = static_cast<INTMessage>(buf.ReadByte());

	switch (type)
	{
	case WPN_INVENTORY:
	{
		pimpl->get<CInventorySetZb5>().SetWeaponKit();
		break;
	}
	}

	return 1;
}

int CHudZB5::MsgFunc_ZB5Evolevel(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);
	auto type = static_cast<ZB5MessageType>(buf.ReadByte());
	int evlv = buf.ReadByte();

	switch (type)
	{
		case ZB5_MESSAGE_EVOLUTION:
		{
			pimpl->get<CHudTextNum>().evolution(evlv);
			break;
		}
	}

	return 1;
}

int CHudZB5::MsgFunc_ZB5Getlocationbox(const char* pszName, int iSize, void* pbuf)
{
	BufferReader reader(pszName, pbuf, iSize);
	int Flag = reader.ReadByte();
	int idx = reader.ReadByte();

	if (idx <= MAX_BOX)
	{
		g_ZombieBoxInfo[idx].origin.x = reader.ReadCoord();
		g_ZombieBoxInfo[idx].origin.y = reader.ReadCoord();
		g_ZombieBoxInfo[idx].origin.z = reader.ReadCoord();
		g_ZombieBoxInfo[idx].dead = false;

		if (Flag == 1)
		{
			g_ZombieBoxInfo[idx].radarflashes = 99999;
			g_ZombieBoxInfo[idx].radarflashtime = gHUD.m_flTime;
			g_ZombieBoxInfo[idx].radarflashtimedelta = 0.5f;
		}
	}

	return 1;
}

int CHudZB5::MsgFunc_ZB5GetlocationboxK(const char* pszName, int iSize, void* pbuf)
{
	BufferReader reader(pszName, pbuf, iSize);
	int idx = reader.ReadByte();
	if (idx <= MAX_BOX)
	{
		g_ZombieBoxInfo[idx].dead = true;
		g_ZombieBoxInfo[idx].radarflashtime = gHUD.m_flTime;
		g_ZombieBoxInfo[idx].radarflashes = 15;
		g_ZombieBoxInfo[idx].radarflashtimedelta = 0.1f;
	}

	return 1;
}

int CHudZB5::Init(void)
{
	pimpl = new CHudZB5::impl_t;

	gHUD.AddHudElem(this);

	HOOK_MESSAGE(ZB5Msg);
	HOOK_MESSAGE(ZB5SkillUsed);
	HOOK_MESSAGE(ZB5SkillUsed2);
	HOOK_MESSAGE(ZB5Inventory);
	HOOK_MESSAGE(ZB5Evolevel);
	HOOK_MESSAGE(ZB5Getlocationbox);
	HOOK_MESSAGE(ZB5GetlocationboxK);

	return 1;
}

int CHudZB5::VidInit(void)
{
	pimpl->for_each(&IBaseHudSub::VidInit);
	return 1;
}

int CHudZB5::Draw(float time)
{
	pimpl->for_each(&IBaseHudSub::Draw, time);
	return 1;
}

void CHudZB5::Think(void)
{
	pimpl->for_each(&IBaseHudSub::Think);
}

void CHudZB5::Reset(void)
{
	for (int i = 0; i < 34; i++)
	{
		g_PlayerExtraInfo[i].radarflashes = 0;

		if (i <= MAX_HOSTAGES)
		{
			g_ZombieBoxInfo[i].radarflashes = 0;
			g_ZombieBoxInfo[i].dead = true;
		}
	}
	pimpl->for_each(&IBaseHudSub::Reset);
}

void CHudZB5::InitHUDData(void)
{
	pimpl->for_each(&IBaseHudSub::InitHUDData);
}

void CHudZB5::Shutdown(void)
{
	delete pimpl;
	pimpl = nullptr;
}