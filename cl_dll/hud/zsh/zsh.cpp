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

#include "gamemode/zsh/zsh_const.h"

class CHudZSH::impl_t
	: public THudSubDispatcher< CHudZSHScoreboard > {};

DECLARE_MESSAGE(m_ZSH, ZSHMsg)
DECLARE_MESSAGE(m_ZSH, ZSHUpdateDay)
DECLARE_MESSAGE(m_ZSH, ZSHUpdateRes)

int CHudZSH::MsgFunc_ZSHMsg(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);
	auto type = static_cast<ZSHMessageTypes>(buf.ReadByte());
	auto type2 = static_cast<ZSHMessageBuild>(buf.ReadByte());
	auto type3 = static_cast<ZSHMessageSkills>(buf.ReadByte());
	auto type4 = static_cast<ZSHMessageUi>(buf.ReadByte());

	switch (type)
	{
		case ZSHScoreboard: 
		{
			
			break;
		}
		case ZSHWeaponboard:
		{
			//auto morale_type = static_cast<>(buf.ReadByte());
			//int morale_level = buf.ReadByte();
			//pimpl->get<>().();
			break;
		}
		case ZSHMessagebox:
		{
			//auto morale_type = static_cast<>(buf.ReadByte());
			//int morale_level = buf.ReadByte();
			//pimpl->get<>().();
			break;
		}
	}

	switch (type2)
	{
		case ZSHTurret:
		{
			//auto morale_type = static_cast<>(buf.ReadByte());
			//int morale_level = buf.ReadByte();
			//pimpl->get<>().();
			break;
		}
		case ZSHBase:
		{
			//auto morale_type = static_cast<>(buf.ReadByte());
			//int morale_level = buf.ReadByte();
			//pimpl->get<>().();
			break;
		}
		case ZSHBase2:
		{
			//auto morale_type = static_cast<>(buf.ReadByte());
			//int morale_level = buf.ReadByte();
			//pimpl->get<>().();
			break;
		}
		case ZSHBase3:
		{
			//auto morale_type = static_cast<>(buf.ReadByte());
			//int morale_level = buf.ReadByte();
			//pimpl->get<>().();
			break;
		}
		case ZSHBase4:
		{
			//auto morale_type = static_cast<>(buf.ReadByte());
			//int morale_level = buf.ReadByte();
			//pimpl->get<>().();
			break;
		}
		case ZSHPost:
		{
			//auto morale_type = static_cast<>(buf.ReadByte());
			//int morale_level = buf.ReadByte();
			//pimpl->get<>().();
			break;
		}
		case ZSHDodgers:
		{
			//auto morale_type = static_cast<>(buf.ReadByte());
			//int morale_level = buf.ReadByte();
			//pimpl->get<>().();
			break;
		}
		case ZSHFences:
		{
			//auto morale_type = static_cast<>(buf.ReadByte());
			//int morale_level = buf.ReadByte();
			//pimpl->get<>().();
			break;
		}
		case ZSHGate:
		{
			//auto morale_type = static_cast<>(buf.ReadByte());
			//int morale_level = buf.ReadByte();
			//pimpl->get<>().();
			break;
		}
		case ZSHStorage:
		{
			//auto morale_type = static_cast<>(buf.ReadByte());
			//int morale_level = buf.ReadByte();
			//pimpl->get<>().();
			break;
		}
		case ZSHGenerator:
		{
			//auto morale_type = static_cast<>(buf.ReadByte());
			//int morale_level = buf.ReadByte();
			//pimpl->get<>().();
			break;
		}
		case ZSHRecovery:
		{
			//auto morale_type = static_cast<>(buf.ReadByte());
			//int morale_level = buf.ReadByte();
			//pimpl->get<>().();
			break;
		}
		case ZSHTechnical:
		{
			//auto morale_type = static_cast<>(buf.ReadByte());
			//int morale_level = buf.ReadByte();
			//pimpl->get<>().();
			break;
		}
		
	}

	switch (type3)
	{
	   case ZSHWarrior:
	   {
			//auto morale_type = static_cast<>(buf.ReadByte());
			//int morale_level = buf.ReadByte();
			//pimpl->get<>().();
			break;
	   }
	   case ZSHSurvival:
	   {
		  // auto morale_type = static_cast<>(buf.ReadByte());
		  // int morale_level = buf.ReadByte();
		  // pimpl->get<>().();
		   break;
	   }
	   case ZSHEngineer:
	   {
		   //auto morale_type = static_cast<>(buf.ReadByte());
		   //int morale_level = buf.ReadByte();
		   //pimpl->get<>().();
		   break;
	   }
	}

	switch (type4)
	{
	   case ZSHBuildbord:
	   {
			//auto morale_type = static_cast<>(buf.ReadByte());
			//int morale_level = buf.ReadByte();
			//pimpl->get<>().();
			break;
	   }
	   case ZSHSkillsboard:
	   {
		   //auto morale_type = static_cast<>(buf.ReadByte());
		   //int morale_level = buf.ReadByte();
		  // pimpl->get<>().();
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

int CHudZSH::MsgFunc_ZSHUpdateRes(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);
	int type = buf.ReadByte(); // reserved.
	int woods = buf.ReadByte();
	int metal = buf.ReadByte();
	pimpl->get<CHudZSHScoreboard>().UpdateRes(woods, metal);

	return 1;
}

int CHudZSH::Init(void)
{
	pimpl = new CHudZSH::impl_t;

	gHUD.AddHudElem(this);

	HOOK_MESSAGE(ZSHMsg);
	HOOK_MESSAGE(ZSHUpdateDay);
	HOOK_MESSAGE(ZSHUpdateRes);

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