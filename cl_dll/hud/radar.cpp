/*
zbs.cpp - CSMoE Client HUD : Radar
Copyright (C) 2019 Moemod Yanase

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "draw_util.h"
#include "triangleapi.h"

#include "hud_sub_impl.h"
#include "gamemode/mods_const.h"

#include "legacy/hud_radar_legacy.h"
#include "modern/hud_radar_modern.h"


DECLARE_COMMAND(m_Radar, ShowRadar)
DECLARE_COMMAND(m_Radar, HideRadar)
DECLARE_MESSAGE(m_Radar, Radar)
DECLARE_MESSAGE(m_Radar, HostageK)
DECLARE_MESSAGE(m_Radar, HostagePos)

DECLARE_MESSAGE(m_Radar, ShelterK)
DECLARE_MESSAGE(m_Radar, ShelterPos)

DECLARE_MESSAGE(m_Radar, WoodK)
DECLARE_MESSAGE(m_Radar, WoodPos)

DECLARE_MESSAGE(m_Radar, MetalK)
DECLARE_MESSAGE(m_Radar, MetalPos)

DECLARE_MESSAGE(m_Radar, ZombieK)
DECLARE_MESSAGE(m_Radar, ZombiePos)

DECLARE_MESSAGE(m_Radar, BuyZoneK)
DECLARE_MESSAGE(m_Radar, BuyZonePos)

DECLARE_MESSAGE(m_Radar, BombDrop)
DECLARE_MESSAGE(m_Radar, BombPickup)
DECLARE_MESSAGE(m_Radar, Location)

char g_szLocation[2048];

struct CHudRadar::impl_t : THudSubDispatcher<CHudRadarLegacy, CHudRadarModern> {};

CHudRadar::CHudRadar() = default;
CHudRadar::~CHudRadar() = default;

int CHudRadar::Init()
{
	pimpl.reset(new impl_t());

	HOOK_MESSAGE(Radar);
	HOOK_COMMAND("drawradar", ShowRadar);
	HOOK_COMMAND("hideradar", HideRadar);
	HOOK_MESSAGE(HostageK);
	HOOK_MESSAGE(HostagePos);

	HOOK_MESSAGE(ShelterK);
	HOOK_MESSAGE(ShelterPos);

	HOOK_MESSAGE(WoodK);
	HOOK_MESSAGE(WoodPos);

	HOOK_MESSAGE(MetalK);
	HOOK_MESSAGE(MetalPos);

	HOOK_MESSAGE(ZombieK);
	HOOK_MESSAGE(ZombiePos);

	HOOK_MESSAGE(BuyZoneK);
	HOOK_MESSAGE(BuyZonePos);

	HOOK_MESSAGE(BombDrop);
	HOOK_MESSAGE(BombPickup);
	HOOK_MESSAGE(Location);

	m_iFlags = HUD_DRAW;

	memset(g_szLocation, 0, sizeof(g_szLocation));

	gHUD.AddHudElem(this);
	return 1;
}

void CHudRadar::Reset()
{
	// make radar don't draw old players after new map
	for (int i = 0; i < 34; i++)
	{
		g_PlayerExtraInfo[i].radarflashes = 0;

		if (i <= MAX_HOSTAGES)
		{
			g_HostageInfo[i].radarflashes = 0;
			g_HostageInfo[i].dead = true;

			g_ZombieInfo[i].radarflashes = 0;
			g_ZombieInfo[i].dead = true;

			g_WoodInfo[i].radarflashes = 0;
			g_WoodInfo[i].dead = true;

			g_MetalInfo[i].radarflashes = 0;
			g_MetalInfo[i].dead = true;

			g_ShelterInfo[i].radarflashes = 0;
			g_ShelterInfo[i].dead = true;
		
			g_BuyZoneInfo[i].radarflashes = 0;
			g_BuyZoneInfo[i].dead = true;
		}
	}
	pimpl->for_each(&IBaseHudSub::Reset);
}

void CHudRadar::InitHUDData(void)
{
	pimpl->for_each(&IBaseHudSub::InitHUDData);
}


void CHudRadar::Shutdown(void)
{
	pimpl = nullptr;
}

int CHudRadar::VidInit(void)
{
	pimpl->for_each(&IBaseHudSub::VidInit);
	return 1;
}

int CHudRadar::Draw(float time)
{
		//pimpl->for_each(&IBaseHudSub::Draw, time);
		auto& modern = pimpl->get<CHudRadarModern>();
		auto& legacy = pimpl->get<CHudRadarLegacy>();
		if (modern.Available() && gHUD.m_iModRunning != MOD_NONE)
		{
			modern.Draw(time);
			gHUD.m_bMordenRadar = TRUE;
		}
		else
		{

			legacy.Draw(time);
			gHUD.m_bMordenRadar = FALSE;
		}
		return 1;
	
}

void CHudRadar::Think(void)
{
	pimpl->for_each(&IBaseHudSub::Think);
}

void CHudRadar::UserCmd_HideRadar()
{
	m_iFlags &= ~HUD_DRAW;
}

void CHudRadar::UserCmd_ShowRadar()
{
	m_iFlags |= HUD_DRAW;
}

int CHudRadar::MsgFunc_Radar(const char* pszName, int iSize, void* pbuf)
{
	BufferReader reader(pszName, pbuf, iSize);

	int index = reader.ReadByte();
	g_PlayerExtraInfo[index].origin.x = reader.ReadCoord();
	g_PlayerExtraInfo[index].origin.y = reader.ReadCoord();
	g_PlayerExtraInfo[index].origin.z = reader.ReadCoord();
	return 1;
}

int CHudRadar::MsgFunc_BombDrop(const char* pszName, int iSize, void* pbuf)
{
	BufferReader reader(pszName, pbuf, iSize);

	g_PlayerExtraInfo[33].origin.x = reader.ReadCoord();
	g_PlayerExtraInfo[33].origin.y = reader.ReadCoord();
	g_PlayerExtraInfo[33].origin.z = reader.ReadCoord();

	g_PlayerExtraInfo[33].radarflashes = 99999;
	g_PlayerExtraInfo[33].radarflashtime = gHUD.m_flTime;
	g_PlayerExtraInfo[33].radarflashtimedelta = 0.5f;
	strncpy(g_PlayerExtraInfo[33].teamname, "TERRORIST", MAX_TEAM_NAME);
	g_PlayerExtraInfo[33].dead = false;
	g_PlayerExtraInfo[33].nextflash = true;

	int Flag = reader.ReadByte();
	g_PlayerExtraInfo[33].playerclass = Flag;

	if (Flag) // bomb planted
	{
		gHUD.m_SpectatorGui.m_bBombPlanted = 0;
		gHUD.m_Timer.m_iFlags = 0;
	}
	return 1;
}

int CHudRadar::MsgFunc_BombPickup(const char* pszName, int iSize, void* pbuf)
{
	g_PlayerExtraInfo[33].radarflashes = false;
	g_PlayerExtraInfo[33].dead = true;

	return 1;
}

int CHudRadar::MsgFunc_HostagePos(const char* pszName, int iSize, void* pbuf)
{
	BufferReader reader(pszName, pbuf, iSize);
	int Flag = reader.ReadByte();
	int idx = reader.ReadByte();
	if (idx <= MAX_HOSTAGES)
	{
		g_HostageInfo[idx].origin.x = reader.ReadCoord();
		g_HostageInfo[idx].origin.y = reader.ReadCoord();
		g_HostageInfo[idx].origin.z = reader.ReadCoord();
		g_HostageInfo[idx].dead = false;

		if (Flag == 1) // first message about this hostage, start flashing
		{
			g_HostageInfo[idx].radarflashes = 99999;
			g_HostageInfo[idx].radarflashtime = gHUD.m_flTime;
			g_HostageInfo[idx].radarflashtimedelta = 0.5f;
		}
	}

	return 1;
}

int CHudRadar::MsgFunc_HostageK(const char* pszName, int iSize, void* pbuf)
{
	BufferReader reader(pszName, pbuf, iSize);
	int idx = reader.ReadByte();
	if (idx <= MAX_HOSTAGES)
	{
		g_HostageInfo[idx].dead = true;
		g_HostageInfo[idx].radarflashtime = gHUD.m_flTime;
		g_HostageInfo[idx].radarflashes = 15;
		g_HostageInfo[idx].radarflashtimedelta = 0.1f;
	}

	return 1;
}

int CHudRadar::MsgFunc_ShelterPos(const char* pszName, int iSize, void* pbuf)
{
	BufferReader reader(pszName, pbuf, iSize);
	int Flag = reader.ReadByte();
	int idx = reader.ReadByte();
	if (idx <= MAX_HOSTAGES)
	{

		g_ShelterInfo[idx].origin.x = reader.ReadCoord();
		g_ShelterInfo[idx].origin.y = reader.ReadCoord();
		g_ShelterInfo[idx].origin.z = reader.ReadCoord();
		g_ShelterInfo[idx].dead = false;

		if (Flag == 1) // first message about this hostage, start flashing
		{
			g_ShelterInfo[idx].radarflashes = 99999;
			g_ShelterInfo[idx].radarflashtime = gHUD.m_flTime;
			g_ShelterInfo[idx].radarflashtimedelta = 0.5f;
		}
	}

	return 1;
}

int CHudRadar::MsgFunc_ShelterK(const char* pszName, int iSize, void* pbuf)
{
	BufferReader reader(pszName, pbuf, iSize);
	int idx = reader.ReadByte();
	if (idx <= MAX_HOSTAGES)
	{
		g_ShelterInfo[idx].dead = true;
		g_ShelterInfo[idx].radarflashtime = gHUD.m_flTime;
		g_ShelterInfo[idx].radarflashes = 15;
		g_ShelterInfo[idx].radarflashtimedelta = 0.1f;
	}

	return 1;
}

int CHudRadar::MsgFunc_WoodPos(const char* pszName, int iSize, void* pbuf)
{
	BufferReader reader(pszName, pbuf, iSize);
	int Flag = reader.ReadByte();
	int idx = reader.ReadByte();
	if (idx <= MAX_HOSTAGES)
	{
		g_WoodInfo[idx].origin.x = reader.ReadCoord();
		g_WoodInfo[idx].origin.y = reader.ReadCoord();
		g_WoodInfo[idx].origin.z = reader.ReadCoord();
		g_WoodInfo[idx].dead = false;

		if (Flag == 1) // first message about this hostage, start flashing
		{
			g_WoodInfo[idx].radarflashes = 99999;
			g_WoodInfo[idx].radarflashtime = gHUD.m_flTime;
			g_WoodInfo[idx].radarflashtimedelta = 0.5f;
		}
	}

	return 1;
}

int CHudRadar::MsgFunc_WoodK(const char* pszName, int iSize, void* pbuf)
{
	BufferReader reader(pszName, pbuf, iSize);
	int idx = reader.ReadByte();
	if (idx <= MAX_HOSTAGES)
	{
		g_WoodInfo[idx].dead = true;
		g_WoodInfo[idx].radarflashtime = gHUD.m_flTime;
		g_WoodInfo[idx].radarflashes = 15;
		g_WoodInfo[idx].radarflashtimedelta = 0.1f;
	}

	return 1;
}

int CHudRadar::MsgFunc_MetalPos(const char* pszName, int iSize, void* pbuf)
{
	BufferReader reader(pszName, pbuf, iSize);
	int Flag = reader.ReadByte();
	int idx = reader.ReadByte();
	if (idx <= MAX_HOSTAGES)
	{
		g_MetalInfo[idx].origin.x = reader.ReadCoord();
		g_MetalInfo[idx].origin.y = reader.ReadCoord();
		g_MetalInfo[idx].origin.z = reader.ReadCoord();
		g_MetalInfo[idx].dead = false;

		if (Flag == 1) // first message about this hostage, start flashing
		{
			g_MetalInfo[idx].radarflashes = 99999;
			g_MetalInfo[idx].radarflashtime = gHUD.m_flTime;
			g_MetalInfo[idx].radarflashtimedelta = 0.5f;
		}
	}

	return 1;
}

int CHudRadar::MsgFunc_MetalK(const char* pszName, int iSize, void* pbuf)
{
	BufferReader reader(pszName, pbuf, iSize);
	int idx = reader.ReadByte();
	if (idx <= MAX_HOSTAGES)
	{
		g_MetalInfo[idx].dead = true;
		g_MetalInfo[idx].radarflashtime = gHUD.m_flTime;
		g_MetalInfo[idx].radarflashes = 15;
		g_MetalInfo[idx].radarflashtimedelta = 0.1f;
	}

	return 1;
}

int CHudRadar::MsgFunc_ZombiePos(const char* pszName, int iSize, void* pbuf)
{
	BufferReader reader(pszName, pbuf, iSize);
	int Flag = reader.ReadByte();
	int idx = reader.ReadByte();
	if (idx <= MAX_HOSTAGES)
	{
		g_ZombieInfo[idx].origin.x = reader.ReadCoord();
		g_ZombieInfo[idx].origin.y = reader.ReadCoord();
		g_ZombieInfo[idx].origin.z = reader.ReadCoord();
		g_ZombieInfo[idx].dead = false;

		if (Flag == 1) // first message about this hostage, start flashing
		{
			g_ZombieInfo[idx].radarflashes = 99999;
			g_ZombieInfo[idx].radarflashtime = gHUD.m_flTime;
			g_ZombieInfo[idx].radarflashtimedelta = 0.5f;
		}
	}

	return 1;
}

int CHudRadar::MsgFunc_ZombieK(const char* pszName, int iSize, void* pbuf)
{
	BufferReader reader(pszName, pbuf, iSize);
	int idx = reader.ReadByte();
	if (idx <= MAX_HOSTAGES)
	{
		g_ZombieInfo[idx].dead = true;
		g_ZombieInfo[idx].radarflashtime = gHUD.m_flTime;
		g_ZombieInfo[idx].radarflashes = 15;
		g_ZombieInfo[idx].radarflashtimedelta = 0.1f;
	}

	return 1;
}

int CHudRadar::MsgFunc_BuyZonePos(const char* pszName, int iSize, void* pbuf)
{
	BufferReader reader(pszName, pbuf, iSize);
	int Flag = reader.ReadByte();
	int idx = reader.ReadByte();
	if (idx <= MAX_HOSTAGES)
	{
		g_BuyZoneInfo[idx].origin.x = reader.ReadCoord();
		g_BuyZoneInfo[idx].origin.y = reader.ReadCoord();
		g_BuyZoneInfo[idx].origin.z = reader.ReadCoord();
		g_BuyZoneInfo[idx].dead = false;

		if (Flag == 1) // first message about this hostage, start flashing
		{
			g_BuyZoneInfo[idx].radarflashes = 99999;
			g_BuyZoneInfo[idx].radarflashtime = gHUD.m_flTime;
			g_BuyZoneInfo[idx].radarflashtimedelta = 0.5f;
		}
	}

	return 1;
}

int CHudRadar::MsgFunc_BuyZoneK(const char* pszName, int iSize, void* pbuf)
{
	BufferReader reader(pszName, pbuf, iSize);
	int idx = reader.ReadByte();
	if (idx <= MAX_HOSTAGES)
	{
		g_BuyZoneInfo[idx].dead = true;
		g_BuyZoneInfo[idx].radarflashtime = gHUD.m_flTime;
		g_BuyZoneInfo[idx].radarflashes = 15;
		g_BuyZoneInfo[idx].radarflashtimedelta = 0.1f;
	}

	return 1;
}

int CHudRadar::MsgFunc_Location(const char* pszName, int iSize, void* pbuf)
{
	BufferReader reader(pszName, pbuf, iSize);
	int iPlayerID = reader.ReadByte();
	if (iPlayerID == gEngfuncs.GetLocalPlayer()->index)
		strcpy(g_szLocation, reader.ReadString());

	return 1;
}

int CHudRadar::GetRadarSize() const
{
	if (ScreenWidth >= 640)
		return 128;
	else
		return 64;
}