/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"

#include "zb3_hero.h"
#include "../mod_zb3.h"
#include "gamemode/zb3/zb3_const.h"

CHero_ZB1::CHero_ZB1(CBasePlayer* player) : BasePlayerExtra(player)
{
	m_pPlayer->m_bIsZombie = false;
	m_pPlayer->m_bNotKilled = false;

	m_pPlayer->pev->body = 0;

	const char* szModel = "hero";
	SET_CLIENT_KEY_VALUE(m_pPlayer->entindex(), GET_INFO_BUFFER(m_pPlayer->edict()), "model", const_cast<char*>(szModel));

	static char szModelPath[64];
	Q_snprintf(szModelPath, sizeof(szModelPath), "models/player/%s/%s.mdl", szModel, szModel);
	m_pPlayer->SetNewPlayerModel(szModelPath);

	UTIL_LogPrintf("\"%s<%i><%s><CT>\" triggered \"Became_Hero\"\n", STRING(m_pPlayer->pev->netname), GETPLAYERUSERID(m_pPlayer->edict()), GETPLAYERAUTHID(m_pPlayer->edict()));

	// remove guns & give nvg
	m_pPlayer->m_bNightVisionOn = false;

	// set default property
	m_pPlayer->pev->health = m_pPlayer->pev->max_health = 3000;
	m_pPlayer->pev->armortype = ARMOR_TYPE_HELMET;
	m_pPlayer->pev->armorvalue = 2000;
	m_pPlayer->pev->gravity = 0.83f;
	m_pPlayer->ResetMaxSpeed();

}

void CHero_ZB1::ResetMaxSpeed() const
{
	m_pPlayer->pev->maxspeed = 310;
}

void CHero_ZB1::DeathSound_Hero()
{
	switch (RANDOM_LONG(1, 3))
	{
	case 1: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "player/die1.wav", VOL_NORM, ATTN_NORM); break;
	case 2: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "player/die2.wav", VOL_NORM, ATTN_NORM); break;
	case 3: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "player/die3.wav", VOL_NORM, ATTN_NORM); break;

	default:break;
	}
}

void CHero_ZB1::Pain_Hero(int m_LastHitGroup, bool HasArmour)
{
	switch (RANDOM_LONG(0, 3))
	{
	case 0: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "player/bhit_flesh-1.wav", VOL_NORM, ATTN_NORM); break;
	case 1: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "player/bhit_flesh-2.wav", VOL_NORM, ATTN_NORM); break;
	case 2: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "player/bhit_flesh-3.wav", VOL_NORM, ATTN_NORM); break;
	case 3: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "player/bhit_kevlar-1.wav", VOL_NORM, ATTN_NORM); break;

	default:break;
	}
}

void PlayerHero_Precache()
{

}