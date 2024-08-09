#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "game.h"
#include "client.h"
#include "bmodels.h"
#include "globals.h"

#include "mod_zbshelter_pve.h"
#include "player/zsh_updateday.h"
#include "zsh/zsh_const.h"
#include "zsh/resources.h"
#include "zsh/spawnres.h"

#include <vector>
#include <random>
#include <algorithm>

#include "player/csdm_randomspawn.h"
#include <dlls/util/u_range.hpp>

bool start = true;

class PlayerModStrategy_ZSH: public CPlayerModStrategy_Default
{
public:
	PlayerModStrategy_ZSH(CBasePlayer* player, CMod_ZombieShelter_coop* mp) : CPlayerModStrategy_Default(player), update(player)
	{
		
	}
	int  ComputeMaxAmmo(const char* szAmmoClassName, int iOriginalMax) override { return 100; }
	bool CanPlayerBuy(bool display) override
	{
		// is the player alive?
		if (m_pPlayer->pev->deadflag != DEAD_NO)
			return false;

		return true;
	}

	void OnSpawn() override
	{
		if (m_pPlayer->pev->health > 0)
		{
			update.spawn();

		}
		
	}

	void OnInitHUD() override
	{
		update.UpdateHUD();
	}

protected:
	EventListener m_listenerAdjustDamage;
	EventListener m_listenerMonsterKilled;
	ZSHUpdateDay update;

};

void CMod_ZombieShelter_coop::InstallPlayerModStrategy(CBasePlayer* player)
{
	player->m_pModStrategy.reset(new PlayerModStrategy_ZSH(player, this));
}

CMod_ZombieShelter_coop::CMod_ZombieShelter_coop()
{
	PRECACHE_SOUND("zsh/BGM_start.wav");

	UTIL_PrecacheOther("resources");

	CVAR_SET_FLOAT("sv_maxspeed", 990 );
	CVAR_SET_FLOAT("mp_freezetime", 25);
	for (int iIndex = 1; iIndex <= gpGlobals->maxClients; ++iIndex)
	{
		CBaseEntity* entity = UTIL_PlayerByIndex(iIndex);
		if (!entity)
			continue;
		CLIENT_COMMAND(entity->edict(), "mp_freezetime 25");

	}
}

void CMod_ZombieShelter_coop::CheckMapConditions()
{
	CVAR_SET_STRING("sv_skyname", "hk");

	CBaseEntity* fog = nullptr;
	while ((fog = UTIL_FindEntityByClassname(fog, "env_fog")) != nullptr)
	{
		REMOVE_ENTITY(fog->edict());
	}
	CClientFog* newfog = GetClassPtr<CClientFog>(nullptr);
	MAKE_STRING_CLASS("env_fog", newfog->pev);
	newfog->Spawn();
	newfog->m_fDensity = 0.0016f;
	newfog->pev->rendercolor = { 0,0,0 };

	LIGHT_STYLE(0, "g");
}

void CMod_ZombieShelter_coop::UpdateGameMode(CBasePlayer* pPlayer)
{
	MESSAGE_BEGIN(MSG_ONE, gmsgGameMode, nullptr, pPlayer->edict());
	WRITE_BYTE(MOD_ZSH);
	WRITE_BYTE(0);
	WRITE_BYTE(static_cast<int>(maxrounds.value)); 
	WRITE_BYTE(0);

	MESSAGE_END();
}

BOOL CMod_ZombieShelter_coop::ClientConnected(edict_t* pEntity, const char* pszName, const char* pszAddress, char* szRejectReason)
{
	return 1;
}

void CMod_ZombieShelter_coop::ClientDisconnected(edict_t* pClient)
{
	IBaseMod::ClientDisconnected(pClient);
}

void CMod_ZombieShelter_coop::WaitingSound()
{
	for (int iIndex = 1; iIndex <= gpGlobals->maxClients; ++iIndex)
	{
		CBaseEntity* entity = UTIL_PlayerByIndex(iIndex);
		if (!entity)
			continue;
		CLIENT_COMMAND(entity->edict(), "spk sound/zsh/BGM_start.wav\n");

	}
}

void CMod_ZombieShelter_coop::day()
{
	
}

void CMod_ZombieShelter_coop::night()
{
	
}

void CMod_ZombieShelter_coop::PlayerSpawn(CBasePlayer* pPlayer)
{
	IBaseMod::PlayerSpawn(pPlayer);
}

void CMod_ZombieShelter_coop::Think()
{
	if (CheckGameOver())
		return;

	TeamCheck();
	CheckLevelInitialized();

		if (CheckGameOver())
			return;

		if (CheckTimeLimit())
			return;
		
		if (IsFreezePeriod())
		{

			static int iLastCountDown = -1;
			int iCountDown = TimeRemaining();

			if (iCountDown > 0)
			{
				if (iCountDown != iLastCountDown)
				{
					iLastCountDown = iCountDown;
					if (iCountDown > 0 && iCountDown < 20)
					{
						UTIL_ClientPrintAll(HUD_PRINTCENTER, "Waiting for Round Start: %s1 sec(s)", UTIL_dtos1(iCountDown)); // #CSO_ZBS_StartCount
					}

					if (iCountDown == 19)
					{
						WaitingSound();
					}
					if (iCountDown == 1)
					{
						day();
						MakeSpawnresThink();
					}
				}
			}


			CheckFreezePeriodExpired();

		}
		
	//if (m_fTeamCount != 0.0f && m_fTeamCount <= gpGlobals->time)
	//{
	//	RestartRound();
	//}

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
		m_iMaxRounds = (int)maxrounds.value;

		if (m_iMaxRounds < 0)
		{
			m_iMaxRounds = 0;
			CVAR_SET_FLOAT("mp_maxrounds", 2);
		}

		m_iMaxRoundsWon = (int)winlimit.value;

		if (m_iMaxRoundsWon < 0)
		{
			m_iMaxRoundsWon = 0;
			CVAR_SET_FLOAT("mp_winlimit", 1);
		}
	}

}

BOOL CMod_ZombieShelter_coop::FInfectionStarted()
{
	const int iCountDown = static_cast<int>(gpGlobals->time - m_fRoundCount);
	return iCountDown > 20;
}

void CMod_ZombieShelter_coop::MakeSpawnresThink()
{
	if (!FInfectionStarted())
		return;
	if (gpGlobals->time < m_flTimeNextMakeSupplybox)
		return;
	m_flTimeNextMakeSupplybox = gpGlobals->time + RANDOM_FLOAT(20.0f, 30.0f);

	RemoveAllSupplybox();

	int iSupplyboxCount = SpawnresCount();
	for (int i = 0; i < iSupplyboxCount; ++i)
	{
		CSpawnres* sb = CreateSpawnres();
		if (!sb)
			continue;
		sb->m_iSupplyboxIndex = i + 1;

		for (CBasePlayer* player : moe::range::PlayersList())
		{
			if (player->m_bIsZombie)
				continue;

			MESSAGE_BEGIN(MSG_ALL, gmsgHostagePos, nullptr, player->pev);
			WRITE_BYTE(1);
			WRITE_BYTE(sb->m_iSupplyboxIndex);
			WRITE_COORD(sb->pev->origin.x);
			WRITE_COORD(sb->pev->origin.y);
			WRITE_COORD(sb->pev->origin.z);
			MESSAGE_END();
		}
	}

}

int CMod_ZombieShelter_coop::SpawnresCount()
{
	int NumDeadCT, NumDeadTerrorist, NumAliveTerrorist, NumAliveCT;
	InitializePlayerCounts(NumAliveTerrorist, NumAliveCT, NumDeadTerrorist, NumDeadCT);
	int iSupplyboxCount = (NumAliveTerrorist + NumAliveCT + NumDeadTerrorist) / 10 + 1;
	return iSupplyboxCount;
}

void CMod_ZombieShelter_coop::RemoveAllSupplybox()
{
	CBaseEntity* ent = nullptr;
	while ((ent = UTIL_FindEntityByClassname(ent, "resoures")) != nullptr)
	{
		CSpawnres* sb = dynamic_ent_cast<CSpawnres*>(ent);
		sb->pev->effects |= EF_NODRAW;
		sb->pev->flags |= FL_KILLME;
		sb->SendPositionMsg();
		sb->SetThink(&CBaseEntity::SUB_Remove);
	}
}

CSpawnres* CMod_ZombieShelter_coop::CreateSpawnres()
{
	auto supplybox = CreateClassPtr<CSpawnres>();

	Vector backup_v_angle = supplybox->pev->v_angle;
	CSDM_DoRandomSpawn(supplybox);

	supplybox->pev->spawnflags |= SF_NORESPAWN;

	DispatchSpawn(supplybox->edict());
	return supplybox;
}

BOOL CMod_ZombieShelter_coop::FRoundStarted()
{
	return !IsFreezePeriod();
}

void CMod_ZombieShelter_coop::TeamCheck()
{
	for (int iIndex = 1; iIndex <= gpGlobals->maxClients; ++iIndex)
	{
		CBaseEntity* entity = UTIL_PlayerByIndex(iIndex);
		if (!entity)
			continue;
		CBasePlayer* player = dynamic_cast<CBasePlayer*>(entity);

		if (player->m_iTeam == TERRORIST)
		{
			player->m_iTeam = CT;
			TeamChangeUpdate(player, player->m_iTeam);
			start = true;
		}
	}
}