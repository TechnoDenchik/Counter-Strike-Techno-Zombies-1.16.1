/*
zb2_zclass_tank.cpp - CSMoE Gameplay server : Zombie Mod 2
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

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"

#include "gamemode/hidden/hidden_class.h"

CJoker_Hidden::CJoker_Hidden(CBasePlayer* player) : BasePlayerExtra(player)
{
	m_pPlayer->m_bIsZombie = true;
	m_pPlayer->m_bIsTratior = false;
	m_pPlayer->m_bNotKilled = false;

	m_pPlayer->pev->body = 0;

	m_pPlayer->pev->rendermode = kRenderTransAdd;
	m_pPlayer->pev->renderamt = 10.0;

	const char* szModel = "joker";
	SET_CLIENT_KEY_VALUE(m_pPlayer->entindex(), GET_INFO_BUFFER(m_pPlayer->edict()), "model", szModel);

	static char szModelPath[64];
	Q_snprintf(szModelPath, sizeof(szModelPath), "models/player/%s/%s.mdl", szModel, szModel);
	m_pPlayer->SetNewPlayerModel(szModelPath);

	UTIL_LogPrintf("\"%s<%i><%s><CT>\" triggered \"Became_ZOMBIE\"\n", STRING(m_pPlayer->pev->netname), GETPLAYERUSERID(m_pPlayer->edict()), GETPLAYERAUTHID(m_pPlayer->edict()));

	m_pPlayer->m_bNightVisionOn = true;
	m_pPlayer->GiveDefaultItems();
	CLIENT_COMMAND(m_pPlayer->edict(), "nightvision");

	m_pPlayer->pev->health = m_pPlayer->pev->max_health = 800;
	m_pPlayer->pev->armortype = ARMOR_TYPE_HELMET;
	m_pPlayer->pev->armorvalue = 500;
	m_pPlayer->pev->gravity = 0.73f;
	m_pPlayer->ResetMaxSpeed();
	m_pPlayer->GiveNamedItem("knife_joker");
}

void CJoker_Hidden::ResetMaxSpeed() const
{
	m_pPlayer->pev->maxspeed = 1290;
}

void CJoker_Hidden::Pain_Joker(int m_LastHitGroup, bool HasArmour)
{
	switch (RANDOM_LONG(0, 2))
	{
	case 0: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "player/bhit_flesh-1.wav", VOL_NORM, ATTN_NORM); break;
	case 1: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "player/bhit_flesh-2.wav", VOL_NORM, ATTN_NORM); break;
	case 2: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "player/bhit_flesh-3.wav", VOL_NORM, ATTN_NORM); break;
	}
}

void CJoker_Hidden::DeathSound_Joker() 
{
	switch (RANDOM_LONG(0, 2))
	{
	case 0: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "player/bhit_flesh-1.wav", VOL_NORM, ATTN_NORM); break;
	case 1: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "player/bhit_flesh-2.wav", VOL_NORM, ATTN_NORM); break;
	case 2: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "player/bhit_flesh-3.wav", VOL_NORM, ATTN_NORM); break;
	}
}