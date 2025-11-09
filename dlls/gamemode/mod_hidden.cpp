#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "game.h"
#include "client.h"
#include "bmodels.h"

#include "mod_hidden.h"

#include <algorithm>

#include "hidden/Entity/supplyboxh.h"
#include "gamemode/interface/interface_const.h"
#include "gamemode/hidden/hidden_const.h"
#include "player/csdm_randomspawn.h"

#include "bot_include.h"
#include "util/u_range.hpp"
#include <vector>
#include <random>

CMod_Hidden::CMod_Hidden()
{
	PRECACHE_SOUND("vox/hidden_death.wav");
	PRECACHE_SOUND("vox/hidden_laugh1.wav");
	PRECACHE_SOUND("vox/hidden_laugh2.wav");
	PRECACHE_SOUND("vox/hidden_laugh3.wav");
	PRECACHE_SOUND("vox/hidden_speak1.wav");
	PRECACHE_SOUND("vox/hidden_speak2.wav");
	PRECACHE_SOUND("vox/hidden_speak3.wav");
	PRECACHE_SOUND("vox/hidden_speak4.wav");
	PRECACHE_SOUND("vox/hidden_speak5.wav");
	PRECACHE_SOUND("vox/hidden_speak6.wav");
	PRECACHE_SOUND("vox/hidden_speak7.wav");
	PRECACHE_SOUND("zb3/zombi_box.wav");
	PRECACHE_GENERIC("sound/Zombi_Ambience.mp3");

	UTIL_PrecacheOther("supplyboxh");
	CVAR_SET_FLOAT("sv_maxspeed", 390);
}

void CMod_Hidden::CheckMapConditions()
{
	Base::CheckMapConditions();

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

	CBaseEntity* sp = nullptr;

	m_vecSupplySpawns.clear();
	while ((sp = UTIL_FindEntityByClassname(sp, "info_player_start")) != nullptr)
	{
		m_vecSupplySpawns.push_back(static_cast<CSupSpawnHidden*>(sp));
	}
}

BOOL CMod_Hidden::ClientConnected(edict_t* pEntity, const char* pszName, const char* pszAddress, char* szRejectReason)
{
	CLIENT_COMMAND(pEntity, "mp3 loop sound/Zombi_Ambience.mp3\n");

	return IBaseMod::ClientConnected(pEntity, pszName, pszAddress, szRejectReason);
}

void CMod_Hidden::ClientDisconnected(edict_t* pClient)
{
	CLIENT_COMMAND(pClient, "mp3 stop\n");

	IBaseMod::ClientDisconnected(pClient);
}

void CMod_Hidden::PlayerSpawn(CBasePlayer* pPlayer)
{
	pPlayer->m_bIsTratior = false;
	pPlayer->m_bIsZombie = false;

	pPlayer->m_bNotKilled = false;
	IBaseMod::PlayerSpawn(pPlayer);
	pPlayer->AddAccount(32000);

	if (!pPlayer->m_bIsZombie)
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgZB3InventorySet, nullptr, pPlayer->pev);
		WRITE_BYTE(WPN_INVENTORY);
		MESSAGE_END();
	}
}

void CMod_Hidden::Think()
{
	int NumDeadCT, NumDeadTerrorist, NumAliveTerrorist, NumAliveCT;
	InitializePlayerCounts(NumAliveTerrorist, NumAliveCT, NumDeadTerrorist, NumDeadCT);

	static int iLastCountDown = -1;
	int iCountDown = static_cast<int>(gpGlobals->time - m_fRoundCount);

	if (iCountDown != iLastCountDown)
	{
		iLastCountDown = iCountDown;
		if (iCountDown > 0 && iCountDown < 20 && !m_bFreezePeriod)
		{
			MESSAGE_BEGIN(MSG_ALL, gmsgZB3RenMsg);
			WRITE_BYTE(0);
			WRITE_BYTE(20 - iCountDown);
			WRITE_BYTE(ZB3_REN_MSG);
			MESSAGE_END();

			static const char* szCountDownSound[11] = {
				"", "one", "two", "three", "four", "five", "six",
				"seven", "eight", "nine", "ten"
			};
			if (iCountDown >= 10)
			{
				for (CBasePlayer* player : moe::range::PlayersList())
					CLIENT_COMMAND(player->edict(), "spk %s\n", szCountDownSound[20 - iCountDown]);

			}
		}
		else if (iCountDown == 20)
		{
			PickJokerOrigin();
		}
		TeamCheck();
	}

	if (m_bShouldPickNewJoker && gpGlobals->time >= m_flNextJokerPickTime)
	{
		PickNewJokerAfterDeath();
		m_bShouldPickNewJoker = false;
		m_flNextJokerPickTime = 0;
	}

	if (deathjoker)
	{
		PickJokerOrigin();
		deathjoker = false;
	}

	if (IsFreezePeriod())
	{
		CheckFreezePeriodExpired();
	}

	if (m_fTeamCount != 0.0f && m_fTeamCount <= gpGlobals->time)
	{
		//RestartRound();
	}

	CheckLevelInitialized();
	MakeSupplyboxThink();

	if (gpGlobals->time > m_tmNextPeriodicThink)
	{
		//CheckRestartRound();
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

		m_iMaxRounds = (int)timelimit.value;

		if (m_iMaxRounds < 0)
		{
			m_iMaxRounds = 0;
			CVAR_SET_FLOAT("mp_timelimit", 0);
		}

		m_iMaxRoundsWon = (int)winlimit.value;

		if (m_iMaxRoundsWon < 0)
		{
			m_iMaxRoundsWon = 0;
			CVAR_SET_FLOAT("mp_winlimit", 0);
		}
	}
}

void CMod_Hidden::MakeSupplyboxThink()
{
	if (gpGlobals->time < m_flTimeNextMakeSupplybox)
		return;
	m_flTimeNextMakeSupplybox = gpGlobals->time + RANDOM_FLOAT(20.0f, 30.0f);

	RemoveAllSupplybox();

	int iSupplyboxCount = SupplyboxCount();
	for (int i = 0; i < iSupplyboxCount; ++i)
	{
		CSupplyBoxHidden* sb = CreateSupplybox();
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

	//UTIL_ClientPrintAll(HUD_PRINTCENTER, "A Supply Box has arrived!"); // #CSO_SupportItemSpawned

	for (CBasePlayer* player : moe::range::PlayersList())
	{
		if (player->m_bIsZombie)
			continue;


	}

	for (int iIndex = 1; iIndex <= gpGlobals->maxClients; ++iIndex)
	{
		CBaseEntity* entity = UTIL_PlayerByIndex(iIndex);
		if (!entity)
			continue;
		CLIENT_COMMAND(entity->edict(), "spk zb3/zombi_box.wav\n");

	}

}

CSupSpawnHidden* CMod_Hidden::SelectSupplySpawnPoint()
{
	size_t const iSize = m_vecSupplySpawns.size();
	if (!iSize)
		return nullptr;
	return m_vecSupplySpawns[RANDOM_LONG(0, iSize - 1)];
}

int CMod_Hidden::SupplyboxCount()
{
	int NumDeadCT, NumDeadTerrorist, NumAliveTerrorist, NumAliveCT;
	InitializePlayerCounts(NumAliveTerrorist, NumAliveCT, NumDeadTerrorist, NumDeadCT);
	int iSupplyboxCount = (NumAliveTerrorist + NumAliveCT + NumDeadTerrorist) / 10 + 1;
	return iSupplyboxCount;
}

void CMod_Hidden::RemoveAllSupplybox()
{
	CBaseEntity* ent = nullptr;
	while ((ent = UTIL_FindEntityByClassname(ent, "supplyboxh")) != nullptr)
	{
		CSupplyBoxHidden* sb = dynamic_ent_cast<CSupplyBoxHidden*>(ent);
		sb->pev->effects |= EF_NODRAW;
		sb->pev->flags |= FL_KILLME;
		sb->SendPositionMsg();
		sb->SetThink(&CBaseEntity::SUB_Remove);
	}
}

CSupplyBoxHidden* CMod_Hidden::CreateSupplybox()
{
	auto supplybox = CreateClassPtr<CSupplyBoxHidden>();

	CSupSpawnHidden* sp = SelectSupplySpawnPoint();
	if (sp)
	{
		supplybox->pev->origin = sp->pev->origin;
		supplybox->pev->angles = sp->pev->angles;
	}
	else
	{
		Vector backup_v_angle = supplybox->pev->v_angle;
		CSDM_DoRandomSpawn(supplybox);
		supplybox->pev->v_angle = backup_v_angle;
	}

	supplybox->pev->spawnflags |= SF_NORESPAWN;

	DispatchSpawn(supplybox->edict());
	return supplybox;
}

void CMod_Hidden::UpdateGameMode(CBasePlayer *pPlayer)
{
	MESSAGE_BEGIN(MSG_ONE, gmsgGameMode, NULL, pPlayer->edict());
	WRITE_BYTE(MOD_HIDDEN);
	WRITE_BYTE(0); // Reserved. (weapon restriction? )
	WRITE_BYTE(maxrounds.value); // MaxRound (mp_roundlimit)
	WRITE_BYTE(0); // Reserved. (MaxTime?)
	MESSAGE_END();
}

void CMod_Hidden::TeamCheck()
{
	for (CBasePlayer* player : moe::range::PlayersList())
	{
		if ((player->m_bIsZombie && player->m_iTeam != TERRORIST) || (!player->m_bIsZombie && player->m_iTeam != CT))
		{
			player->m_iTeam = player->m_bIsZombie ? TERRORIST : CT;
			TeamChangeUpdate(player, player->m_iTeam);

			TheBots->OnEvent(EVENT_PLAYER_CHANGED_TEAM, player);
		}
	}
}

void CMod_Hidden::PickNewJokerAfterDeath()
{
	// Проверяем, есть ли еще живые люди для превращения
	int humanCount = 0;
	for (CBasePlayer* player : moe::range::PlayersList())
	{
		if (player && player->IsAlive() && !player->m_bIsZombie)
		{
			humanCount++;
		}
	}

	if (humanCount > 0)
	{
		MakeRandomPlayerJoker();

		// Воспроизводим звук появления нового Joker
		for (CBasePlayer* player : moe::range::PlayersList())
			CLIENT_COMMAND(player->edict(), "spk vox/hidden_speak%d\n", RANDOM_LONG(1, 7));
	}
	else
	{
		// Если живых людей не осталось, заканчиваем раунд
		CheckWinConditions();
	}
}

void CMod_Hidden::MakeRandomPlayerJoker()
{
	// Если уже есть живой Joker, не создаем нового
	if (IsJokerAlive())
		return;

	// Создаем список живых людей (не зомби)
	std::vector<CBasePlayer*> availableHumans;

	for (CBasePlayer* player : moe::range::PlayersList())
	{
		if (player &&
			player->IsAlive() &&
			!player->m_bIsZombie &&
			player != m_pLastKilledJoker) // Исключаем только что убитого Joker
		{
			availableHumans.push_back(player);
		}
	}

	// Если есть доступные люди, выбираем случайного
	if (!availableHumans.empty())
	{
		// Перемешиваем список
		std::shuffle(availableHumans.begin(), availableHumans.end(),
			std::default_random_engine(static_cast<unsigned>(gpGlobals->time)));

		// Выбираем первого из перемешанного списка
		CBasePlayer* newJoker = availableHumans[0];
		if (newJoker)
		{
			MakeJoker(newJoker);
			m_pCurrentJoker = newJoker; // Сохраняем ссылку на нового Joker

			// Сообщение всем игрокам о новом Joker
			UTIL_ClientPrintAll(HUD_PRINTCENTER, "A new Hidden has appeared!");
		}
	}
}

void CMod_Hidden::PlayerKilled(CBasePlayer* pVictim, entvars_t* pKiller, entvars_t* pInflictor)
{
	if (pVictim->m_bIsZombie)
	{
		CBasePlayer* pAttackerPlayer = dynamic_ent_cast<CBasePlayer*>(pKiller);
		if (pAttackerPlayer)
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgHIDMsg, NULL, pAttackerPlayer->edict());
			WRITE_BYTE(HIDDEN_MSG_KILL);
			WRITE_BYTE(0);
			MESSAGE_END();
		}

		pVictim->m_iDeaths++;

		KillJoker();
		
		m_pLastKilledJoker = pVictim;

		m_bShouldPickNewJoker = true;
		m_flNextJokerPickTime = gpGlobals->time + 5.0f;	

		for (CBasePlayer* player : moe::range::PlayersList())
			CLIENT_COMMAND(player->edict(), "spk vox/hidden_death\n");
	}
	else if (!pVictim->m_bIsZombie)
	{
		CBasePlayer* pAttackerPlayer = dynamic_ent_cast<CBasePlayer*>(pKiller);
		if (pAttackerPlayer)
		{
			HumanInfectionByJoker(pVictim, pAttackerPlayer);
		}	
	}
	return IBaseMod::PlayerKilled(pVictim, pKiller, pInflictor);
}

void CMod_Hidden::InstallPlayerModStrategy(CBasePlayer* player)
{
	std::unique_ptr<CPlayerModStrategy_Hidden> up(new CPlayerModStrategy_Hidden(player, this));
	player->m_pModStrategy = std::move(up);
}

void CPlayerModStrategy_Hidden::OnSpawn()
{
	BecomeHuman();
	return CPlayerModStrategy_Default::OnSpawn();
}

bool CPlayerModStrategy_Hidden::CanPlayerBuy(bool display)
{
	// is the player alive?
	if (m_pPlayer->pev->deadflag != DEAD_NO)
		return false;

	return !m_pPlayer->m_bIsZombie;
}

int CPlayerModStrategy_Hidden::ComputeMaxAmmo(const char* szAmmoClassName, int iOriginalMax)
{
	int ret = iOriginalMax * 2;

	// do not *2 for machine-guns.
	if (Q_strstr(szAmmoClassName, "box"))
		ret = iOriginalMax;

	return ret;
}

void CPlayerModStrategy_Hidden::OnThink()
{
	m_pCharacter->Think();

	if (!m_pPlayer->IsAlive())
	{
		if (gpGlobals->time > m_flDeadTime + 3.0f)
		{
			m_pPlayer->RoundRespawn();
			BecomeHuman();

			m_pPlayer->pev->max_health = 100;
			m_pPlayer->pev->armorvalue = 100;
		}
	}
	

	return CPlayerModStrategy_Zombie::OnThink();
}

void CPlayerModStrategy_Hidden::OnKilled(entvars_t* pKiller, entvars_t* pInflictor)
{
	m_flDeadTime = gpGlobals->time;
	m_flDeadJoker = gpGlobals->time;

	MESSAGE_BEGIN(MSG_ONE, gmsgHIDMsg, NULL, m_pPlayer->edict());
	WRITE_BYTE(HIDDEN_MSG_RESPAWN);
	WRITE_BYTE(3);
	MESSAGE_END();

	return CPlayerModStrategy_Zombie::OnKilled(pKiller, pInflictor);
}

void CPlayerModStrategy_Hidden::Event_OnBecomeJoker(CBasePlayer* who)
{
	if (m_pPlayer != who)
		return;

	BecomeJoker();
	m_pPlayer->OnBecomeJoker();

	jokerdead = false;
}

void CPlayerModStrategy_Hidden::Event_OnKillJoker()
{

	jokerdead = true;
}

void CPlayerModStrategy_Hidden::BecomeJoker()
{
	m_pCharacter = std::make_shared<CJoker_Hidden>(m_pPlayer);

	m_pPlayer->pev->rendermode = kRenderTransAdd;
	m_pPlayer->pev->renderamt = 10.0;
}

void CPlayerModStrategy_Hidden::BecomeHuman()
{
	m_pCharacter = std::make_shared<CHuman_Hidden>(m_pPlayer);
	m_pPlayer->pev->renderfx = kRenderFxNone;
	m_pPlayer->pev->rendermode = kRenderNormal;

}

CPlayerModStrategy_Hidden::CPlayerModStrategy_Hidden(CBasePlayer* player, CMod_Hidden* mp)
	: CPlayerModStrategy_Zombie(player),
	m_eventBecomeJokerListener(mp->m_eventBecomeJoker.subscribe(&CPlayerModStrategy_Hidden::Event_OnBecomeJoker, this)),
	m_eventKilledJokerListener(mp->m_eventKilledJoker.subscribe(&CPlayerModStrategy_Hidden::Event_OnKillJoker, this))
{

}

float CPlayerModStrategy_Hidden::AdjustDamageTaken(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType)
{
	flDamage = m_pCharacter->AdjustDamageTaken(pevInflictor, pevAttacker, flDamage, bitsDamageType);
	return CPlayerModStrategy_Zombie::AdjustDamageTaken(pevInflictor, pevAttacker, flDamage, bitsDamageType);
}

void CPlayerModStrategy_Hidden::Pain(int m_LastHitGroup, bool HasArmour)
{
	if (m_pPlayer->m_bIsZombie)
		return m_pCharacter->Pain_Joker(m_LastHitGroup, HasArmour);
	return CPlayerModStrategy_Zombie::Pain(m_LastHitGroup, HasArmour);
}

void CPlayerModStrategy_Hidden::DeathSound()
{
	if (m_pPlayer->m_bIsZombie)
		return m_pCharacter->DeathSound_Joker();
	return CPlayerModStrategy_Zombie::DeathSound();
}

size_t CMod_Hidden::JokerOriginNum()
{
	moe::range::PlayersList list;
	return static_cast<size_t>(std::distance(list.begin(), list.end()) / 10 + 1);
}

void CMod_Hidden::PickJokerOrigin()
{
	// Если уже есть живой Joker, не создаем нового
	if (IsJokerAlive())
		return;

	auto iNumZombies = JokerOriginNum();

	// Создаем список живых игроков (не зомби, не спектаторы)
	moe::range::PlayersList list;
	std::vector<CBasePlayer*> players;

	for (CBasePlayer* player : list)
	{
		if (player &&
			player->IsAlive() &&
			(player->m_iTeam == TEAM_CT || player->m_iTeam == TEAM_TERRORIST) &&
			!player->m_bIsZombie)
		{
			players.push_back(player);
		}
	}

	// Проверяем, есть ли кандидаты
	if (players.empty())
	{
		return;
	}

	// Перемешиваем используя системное время для рандома
	std::shuffle(players.begin(), players.end(),
		std::default_random_engine(static_cast<unsigned>(gpGlobals->time)));

	// Превращаем только ОДНОГО игрока в Joker
	if (!players.empty())
	{
		CBasePlayer* chosenPlayer = players[0];
		if (chosenPlayer && chosenPlayer->IsAlive())
		{
			MakeJoker(chosenPlayer);
			m_pCurrentJoker = chosenPlayer; // Сохраняем ссылку на текущего Joker
		}
	}

	// sound effect
	for (CBasePlayer* player : moe::range::PlayersList())
		CLIENT_COMMAND(player->edict(), "spk vox/hidden_speak%d\n", RANDOM_LONG(1, 7));

	CheckWinConditions();
}

void CMod_Hidden::HumanInfectionByJoker(CBasePlayer* player, CBasePlayer* attacker)
{
	InfectionSound();

	SetScoreAttrib(player, player);
	TeamCheck();
	CheckWinConditions();

	player->m_iDeaths += 1;
	player->AddPoints(0, FALSE);
}

void CMod_Hidden::InfectionSound()
{
	for (CBasePlayer* player : moe::range::PlayersList())
		CLIENT_COMMAND(player->edict(), "spk vox/hidden_laugh%d\n", RANDOM_LONG(1, 3));
}

BOOL CMod_Hidden::FPlayerCanTakeDamage(CBasePlayer* pPlayer, CBaseEntity* pAttacker)
{
	int iReturn = FALSE;

	if (!pAttacker || PlayerRelationship(pPlayer, pAttacker) != GR_TEAMMATE)
	{
		iReturn = TRUE;
	}

	if (CVAR_GET_FLOAT("mp_friendlyfire") != 0 || pAttacker == pPlayer)
	{
		iReturn = TRUE;
	}

	CBasePlayer* pAttackerPlayer = dynamic_ent_cast<CBasePlayer*>(pAttacker);
	if (pAttackerPlayer)
	{
		if (pAttackerPlayer->m_bIsZombie && pAttackerPlayer->m_bIsTratior && !pPlayer->m_bIsZombie && !pPlayer->m_bIsTratior)
		{		
			iReturn = true;
		}
	}
	return iReturn;
}

void PlayerJoker_Precache()
{
	PRECACHE_SOUND("vox/hidden_death.wav");
	PRECACHE_SOUND("vox/hidden_laugh1.wav");
	PRECACHE_SOUND("vox/hidden_laugh2.wav");
	PRECACHE_SOUND("vox/hidden_laugh3.wav");
	PRECACHE_SOUND("vox/hidden_speak1.wav");
	PRECACHE_SOUND("vox/hidden_speak2.wav");
	PRECACHE_SOUND("vox/hidden_speak3.wav");
	PRECACHE_SOUND("vox/hidden_speak4.wav");
	PRECACHE_SOUND("vox/hidden_speak5.wav");
	PRECACHE_SOUND("vox/hidden_speak6.wav");
	PRECACHE_SOUND("vox/hidden_speak7.wav");
	PRECACHE_GENERIC("sound/Zombi_Ambience.mp3");
}