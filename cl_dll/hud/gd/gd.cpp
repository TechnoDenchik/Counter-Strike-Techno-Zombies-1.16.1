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

#include "gd.h"
#include "gd/WinHud.h"
#include "lvupdate.h"
#include "upweaponhud.h"
#include "gd/GDScoreboard.h"
#include "../dlls/gamemode/gd/gd_const.h"
#include <vector>

class CHudGunDeath::impl_t
	: public THudSubDispatcher<CHudGDScoreboard, CHudGDUpdateWeapon>
{
public:
};

DECLARE_MESSAGE(m_gd, GDMsg)
DECLARE_MESSAGE(m_gd, GDMsgLV)

int CHudGunDeath::MsgFunc_GDMsg(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);

	auto type = static_cast<GunDeath>(buf.ReadByte());
	switch (type)
	{
		case GD_UPDATE_LEVEL:
		{
			pimpl->get<CHudGDUpdateWeapon>().weaponups();
			break;
		}
		case GD_FINAL_ATTACK:
		{
			pimpl->get<CHudGDUpdateWeapon>().weaponupsmax();
			break;
		}
	}
	return 1;
}

int CHudGunDeath::MsgFunc_GDMsgLV(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);
	int type = buf.ReadByte(); // reserved.
	int m_iLevel = buf.ReadByte();

	pimpl->get<CHudGDScoreboard>().OnKillEnemy(m_iLevel);

	return 1;
}

int CHudGunDeath::Init()
{
	pimpl = new CHudGunDeath::impl_t;

	gHUD.AddHudElem(this);

	HOOK_MESSAGE(GDMsg);
	HOOK_MESSAGE(GDMsgLV);

	return 1;
}

int CHudGunDeath::VidInit()
{
	pimpl->for_each(&IBaseHudSub::VidInit);

	return 1;
}

int CHudGunDeath::Draw(float time)
{
	pimpl->for_each(&IBaseHudSub::Draw, time);
	return 1;
}

void CHudGunDeath::Think()
{
	pimpl->for_each(&IBaseHudSub::Think);
}

void CHudGunDeath::Reset()
{
	pimpl->for_each(&IBaseHudSub::Reset);
}

void CHudGunDeath::InitHUDData()
{
	pimpl->for_each(&IBaseHudSub::InitHUDData);
}

void CHudGunDeath::Shutdown()
{
	delete pimpl;
	pimpl = nullptr;
}