/*
player_msg.cpp - CSMoE Server : CBasePlayer messages
Copyright (C) 2019 Moemod Hyakuya

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "gamerules.h"

#include "player.h"

// global vars
int giPrecacheGrunt = 0;
int gmsgWeapPickup = 0;
int gmsgHudText = 0;
int gmsgHudTextArgs = 0;
int gmsgShake = 0;
int gmsgFade = 0;
int gmsgFlashlight = 0;
int gmsgFlashBattery = 0;
int gmsgResetHUD = 0;
int gmsgInitHUD = 0;
int gmsgViewMode = 0;
int gmsgShowGameTitle = 0;
int gmsgCurWeapon = 0;
int gmsgHealth = 0;
int gmsgDamage = 0;
int gmsgBattery = 0;
int gmsgTrain = 0;
int gmsgLogo = 0;
int gmsgWeaponList = 0;
int gmsgAmmoX = 0;
int gmsgDeathMsg = 0;
int gmsgScoreAttrib = 0;
int gmsgScoreInfo = 0;
int gmsgTeamInfo = 0;
int gmsgTeamScore = 0;
int gmsgGameMode = 0;
int gmsgMOTD = 0;
int gmsgServerName = 0;
int gmsgAmmoPickup = 0;
int gmsgItemPickup = 0;
int gmsgHideWeapon = 0;
int gmsgSayText = 0;
int gmsgTextMsg = 0;
int gmsgSetFOV = 0;
int gmsgShowMenu = 0;
int gmsgSendAudio = 0;
int gmsgRoundTime = 0;
int gmsgMoney = 0;
int gmsgBlinkAcct = 0;
int gmsgArmorType = 0;
int gmsgStatusValue = 0;
int gmsgStatusText = 0;
int gmsgStatusIcon = 0;
int gmsgWeaponList2 = 0;

int gmsgBarTime = 0;
int gmsgReloadSound = 0;
int gmsgCrosshair = 0;
int gmsgNVGToggle = 0;
int gmsgRadar = 0;
int gmsgSpectator = 0;
int gmsgVGUIMenu = 0;
int gmsgCZCareer = 0;
int gmsgCZCareerHUD = 0;
int gmsgTaskTime = 0;
int gmsgTutorText = 0;
int gmsgTutorLine = 0;
int gmsgShadowIdx = 0;
int gmsgTutorState = 0;
int gmsgTutorClose = 0;
int gmsgAllowSpec = 0;
int gmsgBombDrop = 0;
int gmsgBombPickup = 0;
int gmsgHostagePos = 0;
int gmsgHostageK = 0;
int gmsgGeigerRange = 0;
int gmsgSendCorpse = 0;
int gmsgHLTV = 0;
int gmsgSpecHealth = 0;
int gmsgForceCam = 0;
int gmsgADStop = 0;
int gmsgReceiveW = 0;
int gmsgScenarioIcon = 0;
int gmsgBotVoice = 0;
int gmsgBuyClose = 0;
int gmsgHitMsg = 0;
int gmsgItemStatus = 0;
int gmsgLocation = 0;
int gmsgSpecHealth2 = 0;
int gmsgBarTime2 = 0;
int gmsgBotProgress = 0;
int gmsgBrass = 0;
int gmsgFog = 0;
int gmsgShowTimer = 0;

int gmsgZBSTip = 0;
int gmsgZBSLevel = 0;
int gmsgZSHUpdateDay = 0;
int gmsgZSHUpdateRes = 0;
int gmsgGDUpdateLV = 0;
int gmsgGDMsg = 0;
int gmsgBTEWeapon = 0;
int gmsgZB2Msg = 0;
int gmsgZB3Msg = 0;
int gmsgZSHMsg = 0;
int gmsgZSHMsgText = 0;
int gmsgResetRound = 0;
int gmsgGunDeath = 0;
int gmsgSupplyText = 0;

// utils
inline int FNullEnt(CBaseEntity *ent) { return (!ent) || FNullEnt(ent->edict()); }

// main code

void LinkUserMessages()
{
	if (gmsgCurWeapon)
		return;

	gmsgCurWeapon = REG_USER_MSG("CurWeapon", -1); // 3
	gmsgGeigerRange = REG_USER_MSG("Geiger", 1);
	gmsgFlashlight = REG_USER_MSG("Flashlight", 2);
	gmsgFlashBattery = REG_USER_MSG("FlashBat", 1);
	gmsgHealth = REG_USER_MSG("Health", 2); // 1
	gmsgDamage = REG_USER_MSG("Damage", 12);
	gmsgBattery = REG_USER_MSG("Battery", 2);
	gmsgTrain = REG_USER_MSG("Train", 1);
	gmsgHudText = REG_USER_MSG("HudTextPro", -1);
	REG_USER_MSG("HudText", -1);
	gmsgSayText = REG_USER_MSG("SayText", -1);
	gmsgTextMsg = REG_USER_MSG("TextMsg", -1);
	gmsgWeaponList = REG_USER_MSG("WeaponList", -1);
	gmsgResetHUD = REG_USER_MSG("ResetHUD", 0);
	gmsgInitHUD = REG_USER_MSG("InitHUD", 0);
	gmsgViewMode = REG_USER_MSG("ViewMode", 0);
	gmsgShowGameTitle = REG_USER_MSG("GameTitle", 1);
	gmsgDeathMsg = REG_USER_MSG("DeathMsg", -1);
	gmsgScoreAttrib = REG_USER_MSG("ScoreAttrib", 2);
	gmsgScoreInfo = REG_USER_MSG("ScoreInfo", 9);
	gmsgTeamInfo = REG_USER_MSG("TeamInfo", -1);
	gmsgTeamScore = REG_USER_MSG("TeamScore", -1);
	gmsgGameMode = REG_USER_MSG("GameMode", -1); // modified
	gmsgMOTD = REG_USER_MSG("MOTD", -1);
	gmsgServerName = REG_USER_MSG("ServerName", -1);
	gmsgAmmoPickup = REG_USER_MSG("AmmoPickup", 2);
	gmsgWeapPickup = REG_USER_MSG("WeapPickup", 1);
	gmsgItemPickup = REG_USER_MSG("ItemPickup", -1);
	gmsgHideWeapon = REG_USER_MSG("HideWeapon", 1);
	gmsgSetFOV = REG_USER_MSG("SetFOV", 1);
	gmsgShowMenu = REG_USER_MSG("ShowMenu", -1);
	gmsgShake = REG_USER_MSG("ScreenShake", 6);
	gmsgFade = REG_USER_MSG("ScreenFade", 10);
	gmsgAmmoX = REG_USER_MSG("AmmoX", -1); // 2
	gmsgSendAudio = REG_USER_MSG("SendAudio", -1);
	gmsgRoundTime = REG_USER_MSG("RoundTime", 2);
	gmsgMoney = REG_USER_MSG("Money", 5);
	gmsgArmorType = REG_USER_MSG("ArmorType", 1);
	gmsgBlinkAcct = REG_USER_MSG("BlinkAcct", 1);
	gmsgStatusValue = REG_USER_MSG("StatusValue", -1);
	gmsgStatusText = REG_USER_MSG("StatusText", -1);
	gmsgWeaponList2 = REG_USER_MSG("WeaponList2", -1);
	gmsgStatusIcon = REG_USER_MSG("StatusIcon", -1);
	gmsgBarTime = REG_USER_MSG("BarTime", 2);
	gmsgReloadSound = REG_USER_MSG("ReloadSound", 2);
	gmsgCrosshair = REG_USER_MSG("Crosshair", 1);
	gmsgNVGToggle = REG_USER_MSG("NVGToggle", 1);
	gmsgRadar = REG_USER_MSG("Radar", 7);
	gmsgSpectator = REG_USER_MSG("Spectator", 2);
	gmsgVGUIMenu = REG_USER_MSG("VGUIMenu", -1);
	gmsgTutorText = REG_USER_MSG("TutorText", -1);
	gmsgTutorLine = REG_USER_MSG("TutorLine", -1);
	gmsgTutorState = REG_USER_MSG("TutorState", -1);
	gmsgTutorClose = REG_USER_MSG("TutorClose", -1);
	gmsgAllowSpec = REG_USER_MSG("AllowSpec", 1);
	gmsgBombDrop = REG_USER_MSG("BombDrop", 7);
	gmsgBombPickup = REG_USER_MSG("BombPickup", 0);
	gmsgSendCorpse = REG_USER_MSG("ClCorpse", -1);
	gmsgHostagePos = REG_USER_MSG("HostagePos", 8);
	gmsgHostageK = REG_USER_MSG("HostageK", 1);
	gmsgHLTV = REG_USER_MSG("HLTV", 2);
	gmsgSpecHealth = REG_USER_MSG("SpecHealth", 1);
	gmsgForceCam = REG_USER_MSG("ForceCam", 3);
	gmsgADStop = REG_USER_MSG("ADStop", 0);
	gmsgReceiveW = REG_USER_MSG("ReceiveW", 1);
	gmsgCZCareer = REG_USER_MSG("CZCareer", -1);
	gmsgCZCareerHUD = REG_USER_MSG("CZCareerHUD", -1);
	gmsgShadowIdx = REG_USER_MSG("ShadowIdx", 4);
	gmsgTaskTime = REG_USER_MSG("TaskTime", 4);
	gmsgScenarioIcon = REG_USER_MSG("Scenario", -1);
	gmsgBotVoice = REG_USER_MSG("BotVoice", 2);
	gmsgBuyClose = REG_USER_MSG("BuyClose", 0);
	gmsgSpecHealth2 = REG_USER_MSG("SpecHealth2", 2);
	gmsgHitMsg = REG_USER_MSG("HitMsg", -1);
	gmsgBarTime2 = REG_USER_MSG("BarTime2", 4);
	gmsgItemStatus = REG_USER_MSG("ItemStatus", 1);
	gmsgLocation = REG_USER_MSG("Location", -1);
	gmsgBotProgress = REG_USER_MSG("BotProgress", -1);
	gmsgBrass = REG_USER_MSG("Brass", -1);
	gmsgFog = REG_USER_MSG("Fog", 7);
	gmsgShowTimer = REG_USER_MSG("ShowTimer", 0);
	gmsgHudTextArgs = REG_USER_MSG("HudTextArgs", -1);

	gmsgZBSTip = REG_USER_MSG("ZBSTip", -1);
	gmsgZBSLevel = REG_USER_MSG("ZBSLevel", -1);
	gmsgZSHUpdateDay = REG_USER_MSG("ZSHUpdateDay", -1);
	gmsgZSHUpdateRes = REG_USER_MSG("ZSHUpdateRes", -1);
	gmsgGDUpdateLV = REG_USER_MSG("GDMsgLV", -1);
	gmsgGDMsg = REG_USER_MSG("GDMsg", -1);
	gmsgBTEWeapon = REG_USER_MSG("BTEWeapon", -1);
	gmsgZB2Msg = REG_USER_MSG("ZB2Msg", -1);
	gmsgZSHMsg = REG_USER_MSG("ZSHMsg", -1);
	gmsgZSHMsgText = REG_USER_MSG("ZSHMsgText", -1);
	gmsgZB3Msg = REG_USER_MSG("ZB3Msg", -1);
	gmsgResetRound = REG_USER_MSG("ResetRound", -1);
	gmsgGunDeath = REG_USER_MSG("GunDeath", -1);
	gmsgSupplyText = REG_USER_MSG("SupplyText", -1);
}

void WriteWeaponInfo2(CBasePlayer* pPlayer, const ItemInfo& II)
{
	const char* pszName;
	if (!II.pszName)
		pszName = "Empty";
	else
		pszName = II.pszName;

	MESSAGE_BEGIN(MSG_ONE, gmsgWeaponList2, NULL, pPlayer->pev);
	WRITE_STRING(pszName);
	WRITE_SHORT(CBasePlayer::GetAmmoIndex(II.pszAmmo1));
	WRITE_BYTE(II.iMaxAmmo1);
	WRITE_SHORT(CBasePlayer::GetAmmoIndex(II.pszAmmo2));
	WRITE_BYTE(II.iMaxAmmo2);
	WRITE_BYTE(II.iSlot);
	WRITE_BYTE(II.iPosition);
	WRITE_SHORT(II.iId);
	WRITE_BYTE(II.iFlags);
	MESSAGE_END();
}

void WriteWeaponInfo(CBasePlayer *pPlayer, const ItemInfo &II)
{
	const char *pszName;
	if (!II.pszName)
		pszName = "Empty";
	else
		pszName = II.pszName;

	MESSAGE_BEGIN(MSG_ONE, gmsgWeaponList, NULL, pPlayer->pev);
	WRITE_STRING(pszName);
	WRITE_BYTE(CBasePlayer::GetAmmoIndex(II.pszAmmo1));
	WRITE_BYTE(II.iMaxAmmo1);
	WRITE_BYTE(CBasePlayer::GetAmmoIndex(II.pszAmmo2));
	WRITE_BYTE(II.iMaxAmmo2);
	WRITE_BYTE(II.iSlot);
	WRITE_BYTE(II.iPosition);
	WRITE_BYTE(II.iId);
	WRITE_BYTE(II.iFlags);
	MESSAGE_END();
}

void WriteSigonMessages()
{
	// No need for this
	// WeaponList will be sent when player pick up weapon.

	/*for (int i = 0; i < MAX_WEAPONS; ++i)
	{
		ItemInfo &II = CBasePlayerItem::ItemInfoArray[i];

		if (!II.iId)
			continue;

		WriteWeaponInfo(II);
	}*/
}

void SendItemStatus(CBasePlayer *pPlayer)
{
	int itemStatus = 0;
	if (pPlayer->m_bHasNightVision)
		itemStatus |= ITEM_STATUS_NIGHTVISION;

	if (pPlayer->m_bHasDefuser)
		itemStatus |= ITEM_STATUS_DEFUSER;

	MESSAGE_BEGIN(MSG_ONE, gmsgItemStatus, NULL, pPlayer->pev);
	WRITE_BYTE(itemStatus);
	MESSAGE_END();
}

void SetScoreAttrib(CBasePlayer *dest, CBasePlayer *src)
{
	int state = 0;
	if (src->pev->deadflag != DEAD_NO)
		state |= PLAYER_DEAD;

	if (src->m_bHasC4)
		state |= PLAYER_HAS_C4;

	if (src->m_bIsVIP)
		state |= PLAYER_VIP;

	if (src->m_bIsZombie)
		state |= PLAYER_ZOMBIE;

	if (gmsgScoreAttrib)
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgScoreAttrib, NULL, dest->pev);
		WRITE_BYTE(src->entindex());
		WRITE_BYTE(state);
		MESSAGE_END();
	}
}