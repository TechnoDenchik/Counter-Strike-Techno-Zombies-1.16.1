#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "gamerules.h"
#include "client.h"

#include "gamemode/mods.h"
#include "gamemode/hidden/hidden_class.h"

CTratior_Hidden::CTratior_Hidden(CBasePlayer *player) : BasePlayerExtra(player)
{
	m_pPlayer->m_bIsZombie = false;
	m_pPlayer->m_bIsTratior = true;
	m_pPlayer->m_bNotKilled = false;

	m_pPlayer->m_iKevlar = ARMOR_TYPE_HELMET;
	m_pPlayer->pev->health = m_pPlayer->pev->max_health= 100;
	m_pPlayer->pev->gravity = 0.86f;
	m_pPlayer->pev->armorvalue = 100;
	m_pPlayer->pev->maxspeed = 550;
}