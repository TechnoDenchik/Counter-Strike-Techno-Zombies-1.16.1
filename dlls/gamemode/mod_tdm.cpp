#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "game.h"
#include "globals.h"
#include "trains.h"
#include "bmodels.h"

#include "gd/gd_const.h"
#include "mod_tdm.h"

class CMultiplayGameMgrHelper : public IVoiceGameMgrHelper
{
public:
	virtual bool CanPlayerHearPlayer(CBasePlayer *pListener, CBasePlayer *pTalker)
	{
		if (g_pGameRules->IsTeamplay())
		{
			if (g_pGameRules->PlayerRelationship(pListener, pTalker) != GR_TEAMMATE)
			{
				return false;
			}
		}
		return true;
	}
};
static CMultiplayGameMgrHelper g_GameMgrHelper;

void CMod_TeamDeathMatch::InstallPlayerModStrategy(CBasePlayer *player)
{
	class MyPlayerModStrategy : public CPlayerModStrategy_Default
	{
	public:
		MyPlayerModStrategy(CBasePlayer *player) : CPlayerModStrategy_Default(player) {}
		bool CanPlayerBuy(bool display) override { return true; }

		void OnKilled(entvars_t* pKiller, entvars_t* pInflictor) override
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgTDMRespawnBar, nullptr, m_pPlayer->pev);
			WRITE_BYTE(GD_RESPAWN_BAR);
			WRITE_BYTE(3);
			MESSAGE_END();

			return CPlayerModStrategy_Default::OnKilled(pKiller, pInflictor);
		}
	};

	std::unique_ptr<MyPlayerModStrategy> up(new MyPlayerModStrategy(player));
	player->m_pModStrategy = std::move(up);
}

CMod_TeamDeathMatch::CMod_TeamDeathMatch()
{
	m_VoiceGameMgr.Init(&g_GameMgrHelper, gpGlobals->maxClients);

	m_flIntermissionEndTime = 0;
	m_flIntermissionStartTime = 0;

	m_iMaxRoundsWon = (int)maxkills.value;
}

void CMod_TeamDeathMatch::Think(void)
{
	m_VoiceGameMgr.Update(gpGlobals->frametime);\

	if (CheckGameOver())
		return;
	
	if (CheckTimeLimit())
		return;

	m_iTotalRoundsPlayed = m_iNumCTWins + m_iNumTerroristWins;

	if (CheckWinLimit())
		return;
	
	if (IsFreezePeriod())
	{
		CheckFreezePeriodExpired();
	}

	CheckLevelInitialized();

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
	
		m_iMaxRoundsWon = (int)maxkills.value;

		if (m_iMaxRoundsWon < 0)
		{
			m_iMaxRoundsWon = 0;
			CVAR_SET_FLOAT("mp_maxkills", 0);
		}
	}

	for (int iIndex = 1; iIndex <= gpGlobals->maxClients; ++iIndex)
	{
		CBaseEntity *entity = UTIL_PlayerByIndex(iIndex);

		if (!entity)
			continue;

		CBasePlayer *player = static_cast<CBasePlayer *>(entity);

		if (player->pev->deadflag != DEAD_DEAD && player->pev->deadflag != DEAD_RESPAWNABLE)
			continue;

		if (player->m_iTeam == TEAM_UNASSIGNED  || player->m_iTeam == TEAM_SPECTATOR)
			continue;

		if (gpGlobals->time < player->m_fDeadTime + 3.0f)
			continue;

		player->RoundRespawn();
	}
}

void CMod_TeamDeathMatch::PlayerKilled(CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor)
{
	IBaseMod::PlayerKilled(pVictim, pKiller, pInflictor);

	CBasePlayer *peKiller = NULL;
	CBaseEntity *ktmp = CBaseEntity::Instance(pKiller);

	if (ktmp && ktmp->Classify() == CLASS_PLAYER)
	{
		peKiller = static_cast<CBasePlayer *>(ktmp);
	}
	else if (ktmp && ktmp->Classify() == CLASS_VEHICLE)
	{
		CBasePlayer *pDriver = static_cast<CBasePlayer *>(((CFuncVehicle *)ktmp)->m_pDriver);

		if (pDriver != NULL)
		{
			pKiller = pDriver->pev;
			peKiller = static_cast<CBasePlayer *>(pDriver);
		}
	}
	if (peKiller && peKiller->IsPlayer())
	{
		if (pVictim->m_iTeam != peKiller->m_iTeam)
		{
			switch (peKiller->m_iTeam)
			{
			case TEAM_CT:
				++m_iNumCTWins;
				break;
			case TEAM_TERRORIST:
				++m_iNumTerroristWins;
				break;
			default:
				break;
			}
			UpdateTeamScores();
		}
	}
}

BOOL CMod_TeamDeathMatch::FPlayerCanTakeDamage(CBasePlayer *pPlayer, CBaseEntity *pAttacker)
{
	if (pAttacker && PlayerRelationship(pPlayer, pAttacker) == GR_TEAMMATE)
	{
		if ((friendlyfire.value == 0) && (pAttacker != pPlayer))
		{
			return FALSE;
		}
	}
	return CCstrikeTechnoZombies::FPlayerCanTakeDamage(pPlayer, pAttacker);
}

BOOL CMod_TeamDeathMatch::FPlayerCanRespawn(CBasePlayer *pPlayer)
{
	if (gpGlobals->time < pPlayer->m_fDeadTime + 3.0f)
	{
		return FALSE;
	}

	if (pPlayer->m_iMenu == Menu_ChooseAppearance)
	{
		return FALSE;
	}
	return TRUE;
}

void CMod_TeamDeathMatch::UpdateGameMode(CBasePlayer *pPlayer)
{
	MESSAGE_BEGIN(MSG_ONE, gmsgGameMode, NULL, pPlayer->edict());
	WRITE_BYTE(MOD_TDM);
	WRITE_BYTE(0);
	WRITE_BYTE(maxkills.value);
	WRITE_BYTE(0);
	MESSAGE_END();
}

void CMod_TeamDeathMatch::PlayerSpawn(CBasePlayer *pPlayer)
{
	IBaseMod::PlayerSpawn(pPlayer);
	pPlayer->AddAccount(16000);

	pPlayer->m_iKevlar = ARMOR_TYPE_HELMET;
	pPlayer->pev->armorvalue = 100;
	pPlayer->SpawnProtection_Start(3.0f);
}