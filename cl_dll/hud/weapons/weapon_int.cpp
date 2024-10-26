#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"

#include "parsemsg.h"

#include "hud_sub_impl.h"
#include "player/player_const.h"
#include "gamemode/interface/interface_const.h"
#include "weapon_int.h"
#include <vector>

#include "epic/TwinShadowAxes.h"
#include "epic/QuantumHorizon.h"

class CWeaponInt_impl_t : public THudSubDispatcher<CHudTwinAxes, CHudQuantum>
{
public:
};

DECLARE_MESSAGE(m_WPI, HudTwinAxesMsg)
DECLARE_MESSAGE(m_WPI, HudQuantumMsg)

int CWeaponInt::MsgFunc_HudTwinAxesMsg(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);

	auto type = static_cast<INTWeaponMsg>(buf.ReadByte());
	switch (type)
	{
		case TWINAXESMSG:
		{
			pimpl->get<CHudTwinAxes>().Setint();
			break;
		}
	}
	return 1;
}

int CWeaponInt::MsgFunc_HudQuantumMsg(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);

	auto type = static_cast<INTWeaponMsg>(buf.ReadByte());
	switch (type)
	{
	case QUANTUMMSG:
	{
		pimpl->get<CHudQuantum>().Setint();
		break;
	}
	}
	return 1;
}

int CWeaponInt::Init()
{
	pimpl = new CWeaponInt_impl_t;

	gHUD.AddHudElem(this);

	HOOK_MESSAGE(HudTwinAxesMsg);
	HOOK_MESSAGE(HudQuantumMsg);

	return 1;
}

int CWeaponInt::VidInit()
{
	pimpl->for_each(&IBaseHudSub::VidInit);

	return 1;
}

int CWeaponInt::Draw(float time)
{
	pimpl->for_each(&IBaseHudSub::Draw, time);
	return 1;
}

void CWeaponInt::Think()
{
	pimpl->for_each(&IBaseHudSub::Think);
}

void CWeaponInt::Reset()
{
	pimpl->for_each(&IBaseHudSub::Reset);
}

void CWeaponInt::InitHUDData()
{
	pimpl->for_each(&IBaseHudSub::InitHUDData);
}

void CWeaponInt::Shutdown()
{
	delete pimpl;
	pimpl = nullptr;
}