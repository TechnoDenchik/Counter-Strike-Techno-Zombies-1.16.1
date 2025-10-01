/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
			* =================== Mod Background UI =================== *
 * =================================================================================== */

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "game.h"
#include "bmodels.h"
#include "util/u_range.hpp"

#include "mod_backui.h"

#include <algorithm>

CMod_BackUI::CMod_BackUI()
{
	m_iMaxMapTime = 19999;
	CVAR_SET_FLOAT("maxplayers", 1);
}

void CMod_BackUI::CheckMapConditions()
{
	IBaseMod::CheckMapConditions();
	
	CBaseEntity *pEntity = nullptr;

	while ((pEntity = UTIL_FindEntityByClassname(pEntity, "func_bomb_target")))
	{
		REMOVE_ENTITY(pEntity->edict());
	}
		
	while ((pEntity = UTIL_FindEntityByClassname(pEntity, "info_bomb_target")))
	{
		REMOVE_ENTITY(pEntity->edict());
	}

	m_bMapHasBombTarget = false;
	m_bMapHasBombZone = false;
	m_bMapHasRescueZone = false;

	m_bMapHasBuyZone = (UTIL_FindEntityByClassname(NULL, "func_buyzone") != NULL);
	m_bMapHasEscapeZone = false;
	m_iMapHasVIPSafetyZone = MAP_HAVE_VIP_SAFETYZONE_NO;

	CBaseEntity *hostage = nullptr;
	while ((hostage = UTIL_FindEntityByClassname(hostage, "hostage_entity")) != nullptr)
	{
		// should be removed.
		REMOVE_ENTITY(hostage->edict());
	}
}

void CMod_BackUI::Think()
{
	if (gpGlobals->time > m_tmNextPeriodicThink)
	{
		CheckRestartRound();
		m_tmNextPeriodicThink = gpGlobals->time + 1.0f;

		if (g_psv_accelerate->value != 5.0f)
		{
			CVAR_SET_FLOAT("sv_accelerate", 5.0);
		}

		if (g_psv_friction->value != 4.0f)
		{
			CVAR_SET_FLOAT("sv_friction", 4.0);
		}

		if (g_psv_stopspeed->value != 75.0f)
		{
			CVAR_SET_FLOAT("sv_stopspeed", 75.0);
		}
		m_iMaxRounds = (int)maxrounds.value;

		if (m_iMaxRounds < 0)
		{
			m_iMaxRounds = 0;
			CVAR_SET_FLOAT("mp_maxrounds", 100);
		}

		m_iMaxRoundsWon = (int)winlimit.value;

		if (m_iMaxRoundsWon < 0)
		{
			m_iMaxRoundsWon = 0;
			CVAR_SET_FLOAT("mp_winlimit", 990);
		}
	}
}

void CMod_BackUI::UpdateGameMode(CBasePlayer *pPlayer)
{
	MESSAGE_BEGIN(MSG_ONE, gmsgGameMode, NULL, pPlayer->edict());
	WRITE_BYTE(MOD_BACKUI);
	WRITE_BYTE(0); // Reserved. (weapon restriction? )
	WRITE_BYTE(maxrounds.value); // MaxRound (mp_roundlimit)
	WRITE_BYTE(99); // Reserved. (MaxTime?)
	MESSAGE_END();
}