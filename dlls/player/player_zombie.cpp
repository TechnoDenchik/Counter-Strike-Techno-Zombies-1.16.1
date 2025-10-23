/*
player_zombie.cpp - CSMoE Gameplay server : CBasePlayer impl for zombies
Copyright (C) 2018 Moemod Hyakuya

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
#include "gamerules.h"
#include "client.h"

#include "gamemode/mods.h"
#include "gamemode/zb1/zb1_zclass.h"

#include "gamemode/zb5/zb1reference/zb1_zclassreference.h"

CHuman_ZB1::CHuman_ZB1(CBasePlayer *player) : BasePlayerExtra(player)
{
	m_pPlayer->m_bIsZombie = false;
	// Give Armor
	m_pPlayer->pev->health = m_pPlayer->pev->max_health= 1000;
	m_pPlayer->pev->gravity = 0.86f;
	m_pPlayer->m_iKevlar = ARMOR_TYPE_HELMET;
	m_pPlayer->pev->armorvalue = 500;
	m_pPlayer->pev->maxspeed = 450;
}

//void CBasePlayer::MakeZombie(ZombieLevel iEvolutionLevel)
CZombie_ZB1::CZombie_ZB1(CBasePlayer *player, ZombieLevel iEvolutionLevel) : BasePlayerExtra(player)
{
	m_pPlayer->m_bIsZombie = true;
	m_pPlayer->m_bIsZombieTank = false;
	m_pPlayer->m_bIsZombieFemale = false;
	m_pPlayer->m_bIsZombieHeavy = false;
	m_pPlayer->m_bIsZombieHeal = false;
	m_pPlayer->m_bIsZombiePc = false;
	m_pPlayer->m_bIsZombieDeimos = false;
	m_pPlayer->m_bIsZombieGanimed = false;
	m_pPlayer->m_bIsZombieBanchee = false;
	m_pPlayer->m_bIsZombieStamp = false;
	m_pPlayer->m_bNotKilled = false;
	m_pPlayer->m_iZombieLevel = iEvolutionLevel;
	if(m_pPlayer->m_bIsZombieMod1 == true)
	{
		m_pPlayer->GiveNamedItem("knife_zombi"); 
	}

	m_pPlayer->pev->body = 0;
	m_pPlayer->m_iModelName = iEvolutionLevel ? MODEL_ZOMBIE_ORIGIN : MODEL_ZOMBIE_HOST;

	const char *szModel = iEvolutionLevel ? "zombi_origin" : "zombi_host";
	SET_CLIENT_KEY_VALUE(m_pPlayer->entindex(), GET_INFO_BUFFER(m_pPlayer->edict()), "model", const_cast<char *>(szModel));

	static char szModelPath[64];
	Q_snprintf(szModelPath, sizeof(szModelPath), "models/player/%s/%s.mdl", szModel, szModel);
	m_pPlayer->SetNewPlayerModel(szModelPath);

	UTIL_LogPrintf("\"%s<%i><%s><CT>\" triggered \"Became_ZOMBIE\"\n", STRING(m_pPlayer->pev->netname), GETPLAYERUSERID(m_pPlayer->edict()), GETPLAYERAUTHID(m_pPlayer->edict()));

	// remove guns & give nvg
	m_pPlayer->GiveDefaultItems();
	m_pPlayer->m_bNightVisionOn = false;
	m_pPlayer->ClientCommand("nightvision");

	// set default property
	m_pPlayer->pev->health = m_pPlayer->pev->max_health = 25000;
	m_pPlayer->pev->armortype = ARMOR_TYPE_HELMET;
	m_pPlayer->pev->armorvalue = 800;
	m_pPlayer->pev->gravity = 0.83f;
	m_pPlayer->ResetMaxSpeed();
	
}

void CZombie_ZB1::ResetMaxSpeed() const
{
	m_pPlayer->pev->maxspeed = 290;
}

void CZombie_ZB1::DeathSound_Zombie()
{
	// temporarily using pain sounds for death sounds
	switch (RANDOM_LONG(1, 2))
	{
		case 1: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "zb3/zombi_death_1.wav", VOL_NORM, ATTN_NORM); break;
		case 2: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "zb3/zombi_death_2.wav", VOL_NORM, ATTN_NORM); break;
		default:break;
	}
}

void CZombie_ZB1::Pain_Zombie(int m_LastHitGroup, bool HasArmour)
{
	switch (RANDOM_LONG(0, 1))
	{
		case 0: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "zb3/zombi_hurt_01.wav", VOL_NORM, ATTN_NORM); break;
		case 1: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "zb3/zombi_hurt_02.wav", VOL_NORM, ATTN_NORM); break;
		default:break;
	}
}

void PlayerZombie_Precache()
{
	PRECACHE_SOUND("zb3/zombi_death_1.wav");
	PRECACHE_SOUND("zb3/zombi_death_2.wav");
	PRECACHE_SOUND("zb3/zombi_hurt_01.wav");
	PRECACHE_SOUND("zb3/zombi_hurt_02.wav");
	PRECACHE_SOUND("zb3/zombi_hurt_banshee_1.wav");
	PRECACHE_SOUND("zb3/zombi_death_banshee_1.wav");
	PRECACHE_SOUND("zb3/zombi_hurt_heavy_1.wav");
	PRECACHE_SOUND("zb3/zombi_hurt_heavy_2.wav");
	PRECACHE_SOUND("zb3/zombi_death_heavy_1.wav");
	PRECACHE_SOUND("zb3/zombi_death_heavy_2.wav");
	PRECACHE_SOUND("zb3/zombi_hurt_female_1.wav");
	PRECACHE_SOUND("zb3/zombi_hurt_female_2.wav");
	PRECACHE_SOUND("zb3/zombi_death_female_1.wav");
	PRECACHE_SOUND("zb3/zombi_death_female_2.wav");
	PRECACHE_SOUND("zb3/zombi_hurt_stamper_1.wav");
	PRECACHE_SOUND("zb3/zombi_hurt_stamper_2.wav");
	PRECACHE_SOUND("zb3/zombi_death_stamper_1.wav");
	PRECACHE_SOUND("zb3/zombi_death_stamper_2.wav");
	PRECACHE_SOUND("zb5/deathknight_hurt1.wav");
	PRECACHE_SOUND("zb5/deathknight_hurt2.wav");
	PRECACHE_SOUND("zb5/deathknight_death1.wav");
	PRECACHE_SOUND("zb5/deathknight_death2.wav");
	PRECACHE_SOUND("zb5/meatwallzombie_hurt1.wav");
	PRECACHE_SOUND("zb5/meatwallzombie_hurt2.wav");
	PRECACHE_SOUND("zb5/meatwallzombie_death1.wav");
	PRECACHE_SOUND("zb5/meatwallzombie_death2.wav");
	PRECACHE_SOUND("zb5/spider_hurt1.wav");
	PRECACHE_SOUND("zb5/spider_hurt2.wav");
	PRECACHE_SOUND("zb5/spider_death1.wav");
	PRECACHE_SOUND("zb5/spider_death2.wav");
	PRECACHE_SOUND("zb5/zombi_heal_female.wav");
	PRECACHE_SOUND("zb5/zombi_heal_meatwall.wav");
	PRECACHE_SOUND("zb5/zombi_heal.wav");
	PRECACHE_SOUND("zb3/zombi_heal.wav");
	PRECACHE_SOUND("zb3/resident_death.wav");
	PRECACHE_SOUND("zb3/resident_hurt1.wav");
	PRECACHE_SOUND("zb3/resident_hurt2.wav");
	PRECACHE_SOUND("zb3/resident_skill1.wav");
	PRECACHE_SOUND("zb3/resident_skill2.wav");
	PRECACHE_SOUND("zb3/resident_stab.wav");
	PRECACHE_SOUND("zb3/resident_stab_miss.wav");
	PRECACHE_SOUND("zb3/resident_tw.wav");
	PRECACHE_SOUND("zb3/flyzombie_death1.wav");
	PRECACHE_SOUND("zb3/flyzombie_death2.wav");
	PRECACHE_SOUND("zb3/flyzombie_downhill_start.wav");
	PRECACHE_SOUND("zb3/flyzombie_fly_idle.wav");
	PRECACHE_SOUND("zb3/flyzombie_fly_start.wav");
	PRECACHE_SOUND("zb3/flyzombie_hurt1.wav");
	PRECACHE_SOUND("zb3/flyzombie_hurt2.wav");
	PRECACHE_SOUND("zb3/zombi_chinese_death.wav");
	PRECACHE_SOUND("zb3/zombi_chinese_hurt.wav");
	PRECACHE_SOUND("zb3/zombi_chinese_jump.wav");
	PRECACHE_SOUND("zb3/boosterzombie_booster.wav");
	PRECACHE_SOUND("zb3/boosterzombie_death1.wav");
	PRECACHE_SOUND("zb3/boosterzombie_death2.wav");
	PRECACHE_SOUND("zb3/boosterzombie_hurt1.wav");
	PRECACHE_SOUND("zb3/boosterzombie_hurt2.wav");
	PRECACHE_SOUND("zb3/boosterzombie_stab.wav");
	PRECACHE_SOUND("zb3/boomer_hurt1.wav");
	PRECACHE_SOUND("zb3/boomer_hurt2.wav");
	PRECACHE_SOUND("zb3/boomer_death.wav");
	PRECACHE_SOUND("zb3/akshazombie_death1.wav");
	PRECACHE_SOUND("zb3/akshazombie_death2.wav");
	PRECACHE_SOUND("zb3/akshazombie_hurt1.wav");
	PRECACHE_SOUND("zb3/akshazombie_hurt2.wav");
	PRECACHE_SOUND("zb3/boomer_draw.wav");
	PRECACHE_SOUND("zb3/boomer_skill.wav");
	PRECACHE_SOUND("zombi/zombi_bomb_exp.wav");
	PRECACHE_SOUND("zombi/zombi_bomb_bounce_1.wav");
	PRECACHE_SOUND("zombi/zombi_bomb_bounce_2.wav");

	PRECACHE_MODEL("models/v_zombibomb_trans.mdl");

	PRECACHE_MODEL("models/v_zombibomb_aksha_trans.mdl");
	PRECACHE_MODEL("models/v_zombibomb_boomer_trans.mdl");
	PRECACHE_MODEL("models/v_zombibomb_booster_trans.mdl");
	PRECACHE_MODEL("models/v_zombibomb_china_trans.mdl");
	PRECACHE_MODEL("models/v_zombibomb_deathknight_trans.mdl");
	PRECACHE_MODEL("models/v_zombibomb_deimos_trans.mdl");
	PRECACHE_MODEL("models/v_zombibomb_fly_trans.mdl");
	PRECACHE_MODEL("models/v_zombibomb_h_trans.mdl");
	PRECACHE_MODEL("models/v_zombibomb_heal_trans.mdl");
	PRECACHE_MODEL("models/v_zombibomb_meatwall.mdl");
	PRECACHE_MODEL("models/v_zombibomb_meatwall_trans.mdl");
	PRECACHE_MODEL("models/v_zombibomb_pass_trans.mdl");
	PRECACHE_MODEL("models/v_zombibomb_resident_trans.mdl");
	PRECACHE_MODEL("models/v_zombibomb_revival_trans.mdl");
	PRECACHE_MODEL("models/v_zombibomb_s_trans.mdl");
	PRECACHE_MODEL("models/v_zombibomb_spider_trans.mdl");
	PRECACHE_MODEL("models/v_zombibomb_teleport.mdl");
	PRECACHE_MODEL("models/v_zombibomb_teleport_host.mdl");
	PRECACHE_MODEL("models/v_zombibomb_undertaker.mdl");
	PRECACHE_MODEL("models/v_zombibomb_witch_trans.mdl");
	PRECACHE_MODEL("models/v_zombibomb-deimos.mdl");
	PRECACHE_MODEL("models/v_zombibomb-deimos_host.mdl");
	PRECACHE_MODEL("models/v_zombibomb-deimos2.mdl");
	PRECACHE_MODEL("models/v_zombibombpc.mdl");

	PRECACHE_MODEL("sprites/zb_meleeup.spr");
	PRECACHE_MODEL("sprites/deathres_zombie.spr");
	PRECACHE_MODEL("sprites/zb_skill_headshot.spr");
	PRECACHE_MODEL("sprites/zombihealer.spr");
	PRECACHE_MODEL("sprites/zombiheal_head.spr");
	PRECACHE_MODEL("sprites/flame_burn01.spr");
	PRECACHE_MODEL("sprites/dmgreiteration01.spr");
	PRECACHE_MODEL("sprites/dmgreiteration02.spr");
	PRECACHE_MODEL("sprites/dmgreiteration03.spr");
	PRECACHE_MODEL("sprites/dmgreiteration04.spr");
	PRECACHE_MODEL("sprites/dmgreiteration05.spr");
	PRECACHE_MODEL("sprites/ef_teleportzombieout.spr");

	PRECACHE_MODEL("sprites/ef_teleportzombie.spr");
	PRECACHE_MODEL("sprites/holybomb_burn.spr");
	PRECACHE_MODEL("sprites/ef_lance_hit.spr");
	PRECACHE_MODEL("sprites/zbt_slow.spr");
}

CHuman_ZB1R::CHuman_ZB1R(CBasePlayer* player) : BasePlayerExtra(player)
{
	m_pPlayer->m_bIsZombie = false;
	// Give Armor
	m_pPlayer->pev->health = m_pPlayer->pev->max_health = 1000;
	m_pPlayer->pev->gravity = 0.86f;
	m_pPlayer->m_iKevlar = ARMOR_TYPE_HELMET;
	m_pPlayer->pev->armorvalue = 500;
	m_pPlayer->pev->maxspeed = 650;
}

//void CBasePlayer::MakeZombie(ZombieLevel iEvolutionLevel)
CZombie_ZB1R::CZombie_ZB1R(CBasePlayer* player, ZombieLevel iEvolutionLevel) : BasePlayerExtra(player)
{
	m_pPlayer->m_bIsZombie = true;
	m_pPlayer->m_bIsZombieTank = false;
	m_pPlayer->m_bIsZombieFemale = false;
	m_pPlayer->m_bIsZombieHeavy = false;
	m_pPlayer->m_bIsZombieHeal = false;
	m_pPlayer->m_bIsZombiePc = false;
	m_pPlayer->m_bIsZombieDeimos = false;
	m_pPlayer->m_bIsZombieGanimed = false;
	m_pPlayer->m_bIsZombieBanchee = false;
	m_pPlayer->m_bIsZombieStamp = false;

	m_pPlayer->m_bIsZombieMeatWall = false;
	m_pPlayer->m_bIsZombieDeathKnight = false;
	m_pPlayer->m_bIsZombieSpider = false;

	m_pPlayer->m_bIsZombieAksha = false;
	m_pPlayer->m_bIsZombieBoomer = false;
	m_pPlayer->m_bIsZombieBooster = false;
	m_pPlayer->m_bIsZombieChina = false;
	m_pPlayer->m_bIsZombieFlying = false;
	m_pPlayer->m_bIsZombieResident = false;

	m_pPlayer->m_bNotKilled = false;

	m_pPlayer->m_iZombieLevel = iEvolutionLevel;
	if (m_pPlayer->m_bIsZombieMod1 == true)
	{
		m_pPlayer->GiveNamedItem("knife_zombi");
	}

	m_pPlayer->pev->body = 0;
	m_pPlayer->m_iModelName = iEvolutionLevel ? MODEL_ZOMBIE_ORIGIN : MODEL_ZOMBIE_HOST;

	const char* szModel = iEvolutionLevel ? "zombi_origin" : "zombi_host";
	SET_CLIENT_KEY_VALUE(m_pPlayer->entindex(), GET_INFO_BUFFER(m_pPlayer->edict()), "model", const_cast<char*>(szModel));

	static char szModelPath[64];
	Q_snprintf(szModelPath, sizeof(szModelPath), "models/player/%s/%s.mdl", szModel, szModel);
	m_pPlayer->SetNewPlayerModel(szModelPath);

	UTIL_LogPrintf("\"%s<%i><%s><CT>\" triggered \"Became_ZOMBIE\"\n", STRING(m_pPlayer->pev->netname), GETPLAYERUSERID(m_pPlayer->edict()), GETPLAYERAUTHID(m_pPlayer->edict()));

	// remove guns & give nvg
	m_pPlayer->GiveDefaultItems();
	m_pPlayer->m_bNightVisionOn = false;
	m_pPlayer->ClientCommand("nightvision");

	// set default property
	m_pPlayer->pev->health = m_pPlayer->pev->max_health = 60000;
	m_pPlayer->pev->armortype = ARMOR_TYPE_HELMET;
	m_pPlayer->pev->armorvalue = 50000;
	m_pPlayer->pev->gravity = 0.83f;
	m_pPlayer->ResetMaxSpeed();

}

void CZombie_ZB1R::ResetMaxSpeed() const
{
	m_pPlayer->pev->maxspeed = 290;
}

void CZombie_ZB1R::DeathSound_Zombie()
{
	// temporarily using pain sounds for death sounds
	switch (RANDOM_LONG(1, 2))
	{
	case 1: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "zb3/zombi_death_1.wav", VOL_NORM, ATTN_NORM); break;
	case 2: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "zb3/zombi_death_2.wav", VOL_NORM, ATTN_NORM); break;
	default:break;
	}
}

void CZombie_ZB1R::Pain_Zombie(int m_LastHitGroup, bool HasArmour)
{
	switch (RANDOM_LONG(0, 1))
	{
	case 0: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "zb3/zombi_hurt_01.wav", VOL_NORM, ATTN_NORM); break;
	case 1: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "zb3/zombi_hurt_02.wav", VOL_NORM, ATTN_NORM); break;
	default:break;
	}
}