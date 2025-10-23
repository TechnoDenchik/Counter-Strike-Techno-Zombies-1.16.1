#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"

#include "parsemsg.h"

#include "hud_sub_impl.h"
#include "calcscreen.h"
#include "zb3.h"
#include "zb3_morale.h"
#include "NewScoreboard.h"
#include "hud/legacy/hud_scoreboard_legacy.h"
#include "zb3_rage.h"
#include "player/player_const.h"
#include "zb3/TextSet.h"
#include "InventorySet.h"
#include "gamemode/zb3/zb3_const.h"
#include "gamemode/interface/interface_const.h"

#include <vector>

class CHudZB3::impl_t
	: public THudSubDispatcher<CHudZB3Morale, CHudZB3Rage, CHudTextZB3, CHudText2ZB3, CInventorySet, CHudHitDamage>
{
public:
};

DECLARE_MESSAGE(m_ZB3, ZB3Msg)
DECLARE_MESSAGE(m_ZB3, ZB3SkillUsed)
DECLARE_MESSAGE(m_ZB3, ZB3SkillUsed2)
DECLARE_MESSAGE(m_ZB3, ZB3Inventory)
DECLARE_MESSAGE(m_ZB3, HitDamageMsgZB3)

int CHudZB3::MsgFunc_ZB3Msg(const char *pszName, int iSize, void *pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);
	auto type = static_cast<ZB3MessageType>(buf.ReadByte());

	switch (type)
	{
		case ZB3_MESSAGE_MORALE:
		{
			auto morale_type = static_cast<ZB3HumanMoraleType_e>(buf.ReadByte());
			int morale_level = buf.ReadByte();
			pimpl->get<CHudZB3Morale>().UpdateLevel(morale_type, morale_level);
			break;
		}
		case ZB3_MESSAGE_RAGE:
		{
			auto zombie_level = static_cast<ZombieLevel>(buf.ReadByte());
			int percent = buf.ReadByte();
			pimpl->get<CHudZB3Rage>().SetZombieLevel(zombie_level);
			pimpl->get<CHudZB3Rage>().SetPercent(percent);
			break;
		}
	}
	
	return 1;
}

int CHudZB3::MsgFunc_ZB3SkillUsed(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);	
	auto type = static_cast<INTMessage>(buf.ReadByte());
	int time = buf.ReadByte();

	pimpl->get<CHudTextZB3>().renaining(time);

	switch (type)
	{
		case ZB3_USED_MSG:
		{
			pimpl->get<CHudTextZB3>().Settext();
			break;
		}
	}

	return 1;
}

int CHudZB3::MsgFunc_ZB3SkillUsed2(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);
	auto type = static_cast<INTMessage>(buf.ReadByte());

	switch (type)
	{
		case ZB3_USED_MSG2:
		{
			pimpl->get<CHudText2ZB3>().Settext();
			break;
		}
	}

	return 1;
}

int CHudZB3::MsgFunc_ZB3Inventory(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);
	auto type = static_cast<INTMessage>(buf.ReadByte());

	switch (type)
	{
	case WPN_INVENTORY:
	{
		pimpl->get<CInventorySet>().SetWeaponKit();
		break;
	}
	}

	return 1;
}

int CHudZB3::MsgFunc_HitDamageMsgZB3(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);
	auto type = static_cast<INTMessage>(buf.ReadByte());

	switch (type)
	{
	case ZB3_HIT:
	{
		pimpl->get<CHudHitDamage>().Settext();
		break;
	}
	}

	return 1;
}

int CHudZB3::Init(void)
{
	pimpl = new CHudZB3::impl_t;

	gHUD.AddHudElem(this);

	HOOK_MESSAGE(ZB3Msg);
	HOOK_MESSAGE(ZB3SkillUsed);
	HOOK_MESSAGE(ZB3SkillUsed2);
	HOOK_MESSAGE(ZB3Inventory);
	HOOK_MESSAGE(HitDamageMsgZB3);

	return 1;
}

int CHudZB3::VidInit(void)
{
	pimpl->for_each(&IBaseHudSub::VidInit);

	R_InitTexture(m_iHero, "resource/helperhud/hero_s");

	return 1;
}

int CHudZB3::Draw(float time)
{
	pimpl->for_each(&IBaseHudSub::Draw, time);

	DrawHeroIcon();

	return 1;
}

void CHudZB3::Think(void)
{
	pimpl->for_each(&IBaseHudSub::Think);
}

void CHudZB3::Reset(void)
{
	pimpl->for_each(&IBaseHudSub::Reset);
}

void CHudZB3::InitHUDData(void)
{
	pimpl->for_each(&IBaseHudSub::InitHUDData);
}

void CHudZB3::Shutdown(void)
{
	delete pimpl;
	pimpl = nullptr;
}

int CHudZB3::DrawHeroIcon()
{
	if (gHUD.m_iIntermission || gEngfuncs.IsSpectateOnly())
		return 0;

	if (g_iUser1)
		return 0;

	if (!(gHUD.m_iWeaponBits & (1 << (WEAPON_SUIT))))
		return 0;

	int x = 0, y = 0;

	for (int i = 0; i < 33; i++)
	{
		if (!g_PlayerExtraInfo[i].vip)
			continue;

		if (g_PlayerExtraInfo[i].zombie || g_PlayerExtraInfo[i].dead)
			continue;

		cl_entity_t* ent = gEngfuncs.GetEntityByIndex(i);

		cl_entity_t* pLocal = gEngfuncs.GetLocalPlayer();

		if (!ent)
			continue;

		int iDistance = (ent->origin - pLocal->origin).Length() * 0.0254f;

		float xyScreen[2];
		if (CalcScreen(ent->origin, xyScreen))
		{
			if (iDistance > 10 && iDistance < 30)
			{
				m_iHero->Draw2DQuadScaled(xyScreen[0] - 18, xyScreen[1] - 18, xyScreen[0] + 19, xyScreen[1] + 19);

				char szBuffer[16];
				sprintf(szBuffer, "[%im]", iDistance);

				int textlen = DrawUtils::HudStringLen(szBuffer);
				int r = 182, g = 167, b = 254;
				DrawUtils::ScaleColors(r, g, b, 255);
				DrawUtils::DrawHudString(xyScreen[0] - textlen * 0.5f, xyScreen[1] + 25, gHUD.m_scrinfo.iWidth, szBuffer, r, g, b, 255, 255);
			}
		}
	}

	return 1;


}