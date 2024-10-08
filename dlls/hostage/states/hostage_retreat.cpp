
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "player.h"
#include "gamerules.h"
#include "training_gamerules.h"
#include "hltv.h"
#include "game.h"
#include "trains.h"
#include "vehicle.h"
#include "globals.h"

#include "pm_shared.h"
#include "utllinkedlist.h"

// CSBOT and Nav
#include "game_shared2/GameEvent.h"		// Game event enum used by career mode, tutor system, and bots
#include "game_shared2/bot/bot_util.h"
#include "game_shared2/bot/simple_state_machine.h"

#include "game_shared2/steam_util.h"

#include "game_shared2/bot/bot_manager.h"
#include "game_shared2/bot/bot_constants.h"
#include "game_shared2/bot/bot.h"

#include "game_shared2/shared_util.h"
#include "game_shared2/bot/bot_profile.h"

#include "game_shared2/bot/nav.h"
#include "game_shared2/bot/improv.h"
#include "game_shared2/bot/nav_node.h"
#include "game_shared2/bot/nav_area.h"
#include "game_shared2/bot/nav_file.h"
#include "game_shared2/bot/nav_path.h"

#include "airtank.h"
#include "h_ai.h"
#include "h_cycler.h"
#include "h_battery.h"

// Hostage
#include "hostage/hostage.h"
#include "hostage/hostage_localnav.h"
#include "hostage/hostage_improv.h"

#include "bot/cs_bot.h"

// Tutor
#include "tutor.h"
#include "tutor_base_states.h"
#include "tutor_base_tutor.h"
#include "tutor_cs_states.h"
#include "tutor_cs_tutor.h"

#include "gamerules.h"
#include "career_tasks.h"
#include "maprules.h"

void HostageRetreatState::OnEnter(CHostageImprov *improv)
{
	improv->Walk();
	improv->MoveTo(improv->GetEntity()->m_vStart);
}

void HostageRetreatState::OnUpdate(CHostageImprov *improv)
{
	if (improv->IsAtHome())
	{
		improv->Stop();
		improv->Idle();
		return;
	}

	CBasePlayer *player = improv->GetClosestVisiblePlayer(UNASSIGNED);

	if (player != NULL)
	{
		const float farRange = 400.0f;
		if ((player->pev->origin - improv->GetCentroid()).IsLengthGreaterThan(farRange))
		{
			if (player->m_iTeam == CT && !improv->IsScared())
			{
				improv->Stop();
				improv->Idle();
				return;
			}
		}
	}

	if (improv->IsScared() && improv->GetScareIntensity() == CHostageImprov::TERRIFIED)
		improv->Run();
	else
		improv->Walk();
}

void HostageRetreatState::OnExit(CHostageImprov *improv)
{
	;
}
