#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "game.h"
#include "globals.h"
#include "trains.h"
#include "bmodels.h"

#include "mod_dm.h"

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

void CMod_DeathMatch::InstallPlayerModStrategy(CBasePlayer *player)
{
	class MyPlayerModStrategy : public CPlayerModStrategy_Default
	{
	public:
		MyPlayerModStrategy(CBasePlayer *player) : CPlayerModStrategy_Default(player) {}
		void CheckBuyZone() override { m_pPlayer->m_signals.Signal(SIGNAL_BUY); };
		bool CanPlayerBuy(bool display) override { return true; }
	};

	std::unique_ptr<MyPlayerModStrategy> up(new MyPlayerModStrategy(player));
	player->m_pModStrategy = std::move(up);
}

CMod_DeathMatch::CMod_DeathMatch()
{
	m_VoiceGameMgr.Init(&g_GameMgrHelper, gpGlobals->maxClients);

	m_flIntermissionEndTime = 0;
	m_flIntermissionStartTime = 0;

	m_iMaxRoundsWon = (int)maxkills.value;
}

void CMod_DeathMatch::Think(void)
{
	m_VoiceGameMgr.Update(gpGlobals->frametime);\

	if (CheckGameOver())
		return;
	
	if (CheckTimeLimit())
		return;
	
	if (CheckWinLimitDM())
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

		if(gpGlobals->time < player->m_fDeadTime + 5.0f)
			continue;

		player->RoundRespawn();
	}
}

void CMod_DeathMatch::PlayerKilled(CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor)
{
	DeathNotice(pVictim, pKiller, pInflictor);

	pVictim->m_afPhysicsFlags &= ~PFLAG_ONTRAIN;
	pVictim->m_iDeaths++;
	pVictim->m_bNotKilled = false;
	pVictim->m_bEscaped = false;
	pVictim->m_iTrain = (TRAIN_NEW | TRAIN_OFF);
	SET_VIEW(ENT(pVictim->pev), ENT(pVictim->pev));

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

	FireTargets("game_playerdie", pVictim, pVictim, USE_TOGGLE, 0);

	if (pVictim->pev == pKiller)
	{
		pKiller->frags -= 1;
	}
	else if (peKiller && peKiller->IsPlayer())
	{
		CBasePlayer *killer = peKiller;
		bool killedByFFA = false;

		pKiller->frags += IPointsForKill(peKiller, pVictim);
		killer->AddAccount(REWARD_KILLED_ENEMY);

		if (!(killer->m_flDisplayHistory & DHF_ENEMY_KILLED))
		{
			killer->m_flDisplayHistory |= DHF_ENEMY_KILLED;
			killer->HintMessage("#Hint_win_round_by_killing_enemy");
		}

		FireTargets("game_playerkill", peKiller, peKiller, USE_TOGGLE, 0);
	}
	else
	{
		pKiller->frags -= 1;
	}

	MESSAGE_BEGIN(MSG_BROADCAST, gmsgScoreInfo);
	WRITE_BYTE(ENTINDEX(pVictim->edict()));
	WRITE_SHORT((int)pVictim->pev->frags);
	WRITE_SHORT(pVictim->m_iDeaths);
	WRITE_SHORT(0);
	WRITE_SHORT(pVictim->m_iTeam);
	MESSAGE_END();

	CBaseEntity *ep = CBaseEntity::Instance(pKiller);

	if (ep && ep->Classify() == CLASS_PLAYER)
	{
		CBasePlayer *PK = static_cast<CBasePlayer *>(ep);

		MESSAGE_BEGIN(MSG_ALL, gmsgScoreInfo);
		WRITE_BYTE(ENTINDEX(PK->edict()));
		WRITE_SHORT((int)PK->pev->frags);
		WRITE_SHORT(PK->m_iDeaths);
		WRITE_SHORT(0);
		WRITE_SHORT(PK->m_iTeam);
		MESSAGE_END();

		PK->m_flNextDecalTime = gpGlobals->time;
	}
}

int CMod_DeathMatch::PlayerRelationship(CBasePlayer *pPlayer, CBaseEntity *pTarget)
{
	if (!pPlayer || !pTarget)
	{
		return GR_NOTTEAMMATE;
	}

	if (!pTarget->IsPlayer())
	{
		return GR_NOTTEAMMATE;
	}

	if (pPlayer == pTarget)
	{
		return GR_TEAMMATE;
	}

	return GR_NOTTEAMMATE;
}

BOOL CMod_DeathMatch::FPlayerCanTakeDamage(CBasePlayer *pPlayer, CBaseEntity *pAttacker)
{
	if (pAttacker && PlayerRelationship(pPlayer, pAttacker) == GR_TEAMMATE)
	{
		if ((friendlyfire.value == 0) && (pAttacker != pPlayer))
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CMod_DeathMatch::FPlayerCanRespawn(CBasePlayer *pPlayer)
{
	if (pPlayer->m_iMenu == Menu_ChooseAppearance)
	{
		return FALSE;
	}

	return TRUE;
}

void CMod_DeathMatch::UpdateGameMode(CBasePlayer *pPlayer)
{
	MESSAGE_BEGIN(MSG_ONE, gmsgGameMode, NULL, pPlayer->edict());
	WRITE_BYTE(MOD_DM);
	WRITE_BYTE(0);
	WRITE_BYTE(maxkills.value);
	WRITE_BYTE(0);
	MESSAGE_END();
}

void CMod_DeathMatch::PlayerSpawn(CBasePlayer *pPlayer)
{
	IBaseMod::PlayerSpawn(pPlayer);
	pPlayer->AddAccount(16000);

	pPlayer->m_iKevlar = ARMOR_TYPE_HELMET;
	pPlayer->pev->armorvalue = 100;
	pPlayer->SpawnProtection_Start(3.0f);
}

bool CMod_DeathMatch::CheckWinLimitDM()
{
	if (m_iMaxRoundsWon != 0 && (CalcLeaderFrags() >= m_iMaxRoundsWon))
	{
			ALERT(at_console, "Changing maps...someone has won the specified number of frags\n");
			GoToIntermission();
			return true;
	}

	return false;
}

int CMod_DeathMatch::CalcLeaderFrags()
{
	int iFrags = 0, iMaxFrags = 0;
	for (int iIndex = 1; iIndex <= gpGlobals->maxClients; ++iIndex)
	{
		CBaseEntity *entity = UTIL_PlayerByIndex(iIndex);

		if (!entity)
			continue;

		CBasePlayer *player = static_cast<CBasePlayer *>(entity);

		if (player->m_iTeam == TEAM_UNASSIGNED || player->m_iTeam == TEAM_SPECTATOR)
			continue;

		iFrags = player->pev->frags;
		if (iMaxFrags >= iFrags)
			continue;

		iMaxFrags = iFrags;
	}
	return iMaxFrags;
}