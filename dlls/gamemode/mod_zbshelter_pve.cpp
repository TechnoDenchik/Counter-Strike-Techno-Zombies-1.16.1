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
#include "zsh/resources/resources.h"
#include "zsh/spawnres.h"
#include "zsh/resources/meat.h"
#include "zsh/resources/meat_manager.h"
#include "zsh/resources/resource_group.h"
#include "zsh/resources/wood.h"
#include "zsh/resources/wood_manager.h"
#include "zsh/resources/zsh_subs.h"
#include "zsh/resources/resrandomspawn.h"

#include <vector>
#include <random>
#include <algorithm>

#include <dlls/util/u_range.hpp>

bool start = true;


class PlayerModStrategy_ZSH: public CPlayerModStrategy_Default
{
public:
	PlayerModStrategy_ZSH(CBasePlayer* pPlayer, CMod_ZombieShelter_coop* mp) : CPlayerModStrategy_Default(pPlayer), update(pPlayer), updateres(pPlayer)
	{
		/*m_listenerAdjustDamage = mp->m_eventAdjustDamage.subscribe(
			[=](CBasePlayer* attacker, float& out)
			{
				updateres.UpdateWoods();
			}
		);*/
		m_listenerMonsterKilled2 = mp->m_eventWoodKilled.subscribe(
			[=](CWood* victim, CBaseEntity* attacker)
			{
				if (attacker == m_pPlayer)
				{
					if (victim->m_iKillBonusFrags)
						m_pPlayer->AddPoints(victim->m_iKillBonusFrags, FALSE);
					updateres.UpdateWoods();
					
				}
			}

		);

		m_listenerMonsterKilled = mp->m_eventMeatKilled.subscribe(
			[=](CMeat* victim, CBaseEntity* attacker)
			{
				if (attacker == m_pPlayer)
				{
					if (victim->m_iKillBonusFrags2)
						m_pPlayer->AddPoints(victim->m_iKillBonusFrags2, FALSE);
					updateres.UpdateMetal();

				}
			}

		);
	}
	int  ComputeMaxAmmo(const char* szAmmoClassName, int iOriginalMax) override { return 100; }
	bool CanPlayerBuy(bool display) override
	{
		// is the player alive?
		if (m_pPlayer->pev->deadflag != DEAD_NO)
			
		
			return false;

		return true;
	}

	CMod_ZombieShelter_coop add;

	void OnSpawn() override
	{
		if (m_pPlayer->pev->health > 1)
		{
			update.spawn();
		}
	}
	
	void GiveDefaultItems() override
	{
		for (int i = 0; i < 1; i++)
		{
			if (m_pPlayer->pev->health > 1)
			{
				GiveWeaponsToPlayer();
				update.spawn();
			}
		}
	}

	void OnThink() override
	{
		
	}

	void OnInitHUD() override
	{
		updateres.Reset();
		update.UpdateHUD();
	}
	
	void GiveWeaponsToPlayer() const
	{
		m_pPlayer->RemoveAllItems(false);
		m_pPlayer->GiveNamedItem("weapon_shelteraxe");
		//m_pPlayer->GiveNamedItem("weapon_c4shelter");
	}

protected:
	EventListener m_listenerAdjustDamage;
	EventListener m_listenerMonsterKilled;
	EventListener m_listenerAdjustDamage2;
	EventListener m_listenerMonsterKilled2;
	ZSHUpdateDay update;
	ZSHUpdateRes updateres;
	
};


class PlayerModStrategy_ZSH2 : public CPlayerModStrategy_Default
{
public:
	PlayerModStrategy_ZSH2(CBasePlayer* pPlayer, CMod_ZombieShelter_coop* mp) : CPlayerModStrategy_Default(pPlayer), updateres(pPlayer)
	{
		

	}

	void OnInitHUD() override
	{
		updateres.Reset();
	}

protected:
	
	ZSHUpdateRes updateres;
};

class CResModStrategy_ZSH : public CWoodModStrategy_Default
{
	friend class PlayerModStrategy_ZBS;

public:
	CResModStrategy_ZSH(CWood* p, CMod_ZombieShelter_coop* pGameRules) : CWoodModStrategy_Default(p), mp(pGameRules)
	{

	}

	void OnKilled(entvars_t* pevKiller, int iGib) override
	{
		CWoodModStrategy_Default::OnKilled(pevKiller, iGib);
		if (pevKiller)
		{
			CBaseEntity* pKiller = CBaseEntity::Instance(pevKiller);
			mp->m_eventWoodKilled.dispatch(m_pWood, pKiller);
		}
	}

protected:
	CMod_ZombieShelter_coop* const mp;
};

class CResModStrategy_ZSH2 : public CMeatModStrategy_Default
{
	friend class PlayerModStrategy_ZSH;

public:
	CResModStrategy_ZSH2(CMeat* p, CMod_ZombieShelter_coop* pGameRules) : CMeatModStrategy_Default(p), mp(pGameRules)
	{

	}

	void OnKilled2(entvars_t* pevKiller, int iGib) override
	{
		CMeatModStrategy_Default::OnKilled2(pevKiller, iGib);
		if (pevKiller)
		{
			CBaseEntity* pKiller = CBaseEntity::Instance(pevKiller);
			mp->m_eventMeatKilled.dispatch(m_pMeat, pKiller);
		}
	}

protected:
	CMod_ZombieShelter_coop* const mp;
};

void CMod_ZombieShelter_coop::InstallPlayerModStrategy(CBasePlayer* player)
{
	player->m_pModStrategy.reset(new PlayerModStrategy_ZSH(player, this));	
	player->m_pModStrategy2.reset(new PlayerModStrategy_ZSH2(player, this));
}

float CMod_ZombieShelter_coop::GetAdjustedEntityDamage(CBaseEntity* victim, entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType)
{
	flDamage = Base::GetAdjustedEntityDamage(victim, pevInflictor, pevAttacker, flDamage, bitsDamageType);

	if (pevAttacker)
	{
		CBaseEntity* pAttackingEnt = CBaseEntity::Instance(pevAttacker);
		if (pAttackingEnt->IsPlayer())
		{
			CBasePlayer* pAttacker = dynamic_cast<CBasePlayer*>(pAttackingEnt);
			m_eventAdjustDamage.dispatch(pAttacker, flDamage);
		}
	}

	return flDamage;
}

float CMod_ZombieShelter_coop::GetAdjustedEntityDamage2(CBaseEntity* victim, entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage2, int bitsDamageType2)
{
	flDamage2 = Base::GetAdjustedEntityDamage2(victim, pevInflictor, pevAttacker, flDamage2, bitsDamageType2);

	if (pevAttacker)
	{
		CBaseEntity* pAttackingEnt = CBaseEntity::Instance(pevAttacker);
		if (pAttackingEnt->IsPlayer())
		{
			CBasePlayer* pAttacker = dynamic_cast<CBasePlayer*>(pAttackingEnt);
			m_eventAdjustDamage2.dispatch(pAttacker, flDamage2);
		}
	}

	return flDamage2;
}

void CMod_ZombieShelter_coop::PlayerSpawn(CBasePlayer* pPlayer)
{
	IBaseMod::PlayerSpawn(pPlayer);
	pPlayer->AddAccount(800);

	// Give Armor
	pPlayer->m_iKevlar = ARMOR_TYPE_HELMET;
	pPlayer->pev->health = 100;
	pPlayer->pev->armorvalue = 100;
	pPlayer->SpawnProtection_Start(3.0f);
}

CMod_ZombieShelter_coop::CMod_ZombieShelter_coop()
{
	m_iRoundTimeSecs = m_iIntroRoundTime = 20 + 2;
	PRECACHE_SOUND("zsh/BGM_start.wav");
	PRECACHE_GENERIC("sound/zsh/BGM_zombie_attack.mp3");
	UTIL_PrecacheOther("resources");
	UTIL_PrecacheOther("monster_sentry2");

	CVAR_SET_FLOAT("sv_maxspeed", 990 );
	//CVAR_SET_FLOAT("mp_freezetime", 25);
	
	
}

void CMod_ZombieShelter_coop::CheckMapConditions()
{
	CVAR_SET_STRING("sv_skyname", "hk");

	CBaseEntity* fog = nullptr;
	while ((fog = UTIL_FindEntityByClassname(fog, "env_fog")) != nullptr)
	{
		REMOVE_ENTITY(fog->edict());
	}

	m_vecZombieSpawns.clear();
	CBaseEntity* sp = nullptr;
	CBaseEntity* ent = NULL;
	while ((sp = UTIL_FindEntityByClassname(sp, "zombiespawn")) != nullptr)
	{
		m_vecZombieSpawns.push_back(static_cast<CResSpawn*>(sp));
	}

	while ((ent = UTIL_FindEntityByClassname(ent, "playerspawn")) != NULL)
		++m_iSpawnPointCount_Terrorist;

	while ((ent = UTIL_FindEntityByClassname(ent, "playerspawn")) != NULL)
		++m_iSpawnPointCount_CT;

	UTIL_FindEntityByClassname(sp, "shelter_team_barricade2");
	UTIL_FindEntityByClassname(sp, "playerspawn");

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

					if (iCountDown == 18)
					{

						WaitingSound();
						
					}
					if (iCountDown == 15)
					{
						day();
					}
				}
			}


			CheckFreezePeriodExpired();

		}
		if (FRoundStarted() && !m_bRoundTerminating)
		{

			if (gpGlobals->time > m_flNextSpawnNPC)
			{
				MakeResources();
				MakeResources2();
				m_flNextSpawnNPC = gpGlobals->time + 18.0f;
			}
		}
		if (FRoundStarted() && !m_bRoundTerminating)
		{

			if (gpGlobals->time > m_flNextDay)
			{
				m_flNextDay = gpGlobals->time + 18.0f;

				
				
			}
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

	for (int iIndex = 1; iIndex <= gpGlobals->maxClients; ++iIndex)
	{
		CBaseEntity* entity = UTIL_PlayerByIndex(iIndex);

		if (!entity)
			continue;

		CBasePlayer* player = static_cast<CBasePlayer*>(entity);

		if (player->pev->deadflag != DEAD_DEAD && player->pev->deadflag != DEAD_RESPAWNABLE)
			continue;

		if (player->m_iTeam == TEAM_UNASSIGNED || player->m_iTeam == TEAM_SPECTATOR)
			continue;

		if (gpGlobals->time < player->m_fDeadTime + 5.0f)
			continue;

		player->RoundRespawn();
	}
}

BOOL CMod_ZombieShelter_coop::FInfectionStarted()
{
	const int iCountDown = static_cast<int>(gpGlobals->time - m_fRoundCount);
	return iCountDown > 20;
}

/*
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
}*/

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

CResSpawn* CMod_ZombieShelter_coop::SelectZombieSpawnPoint()
{
	size_t const iSize = m_vecZombieSpawns.size();
	if (!iSize)
		return nullptr;
	return m_vecZombieSpawns[RANDOM_LONG(0, iSize - 1)];

}

CBaseEntity* CMod_ZombieShelter_coop::MakeResources()
{
	CWood* monster = GetClassPtr<CWood>(nullptr);

	if (!monster)
		return nullptr;

	edict_t* pent = monster->edict();

	CResSpawn* sp = SelectZombieSpawnPoint();
	//if (sp)
	//{
		//monster->pev->origin = sp->pev->origin;
		//monster->pev->angles = sp->pev->angles;
	//}
	//else
	//{
		Vector backup_v_angle = monster->pev->v_angle;
		RES_DoRandomSpawn(monster);
		monster->pev->v_angle = backup_v_angle;
	//}

	pent->v.spawnflags |= SF_NORESPAWN;

	DispatchSpawn(pent);

	// default settings
	
//	monster->pev->maxspeed = 180.0f + (m_iNumCTWins / static_cast<float>(3)) * 15;
	//monster->m_flAttackDamage = (0.2f * m_iNumCTWins + 1) * (0.2f * m_iNumCTWins + 1);
	int health[10] = { 230, 164, 135, 176, 152, 164, 178, 182, 192, 130 };
	int max_health = 0;
	monster->pev->health = max_health + m_iNumCTWins * 15;
	switch (RANDOM_LONG(1, 10))
	{
		monster->pev->max_health = max_health;

	case 1: max_health = health[0];
		break;
		monster->pev->max_health = max_health;
	case 2: max_health = health[1];
		break;
		monster->pev->max_health = max_health;
	case 3: max_health = health[2];
		break;
		monster->pev->max_health = max_health;
	case 4: max_health = health[3];
		break;
		monster->pev->max_health = max_health;
	case 5: max_health = health[4];
		break;
		monster->pev->max_health = max_health;
	case 6: max_health = health[5];
		break;
		monster->pev->max_health = max_health;
	case 7: max_health = health[6];
		break;
		monster->pev->max_health = max_health;
	case 8: max_health = health[7];
		break;
		monster->pev->max_health = max_health;
	case 9: max_health = health[8];
		break;
		monster->pev->max_health = max_health;
	case 10: max_health = health[9];
		break;
	}
	if (m_iNumCTWins < 5 || RANDOM_LONG(0, 3))
	{
		//monster->pev->health = monster->pev->max_health = monster->pev->max_health / 2;
		
		SET_MODEL(monster->edict(), "models/shelter/item_wood.mdl");

		UTIL_SetSize(monster->pev, VEC_HULL_MIN, VEC_HULL_MAX);
	}

	monster->m_pWoodStrategy.reset(new CResModStrategy_ZSH(monster, this));

	return monster;
}

CBaseEntity* CMod_ZombieShelter_coop::MakeResources2()
{
	CMeat* monster = GetClassPtr<CMeat>(nullptr);

	if (!monster)
		return nullptr;

	edict_t* pent = monster->edict();

	CResSpawn* sp = SelectZombieSpawnPoint();
	if (sp)
	{
		monster->pev->origin = sp->pev->origin;
		monster->pev->angles = sp->pev->angles;
	}
	else
	{
		Vector backup_v_angle = monster->pev->v_angle;
		RES_DoRandomSpawn(monster);
		monster->pev->v_angle = backup_v_angle;
	}

	pent->v.spawnflags |= SF_NORESPAWN;

	DispatchSpawn(pent);

	// default settings
	monster->pev->health = monster->pev->max_health = 100 + m_iNumCTWins * 15;
	//monster->pev->maxspeed = 180.0f + (m_iNumCTWins / static_cast<float>(3)) * 15;
	//monster->m_flAttackDamage = (0.2f * m_iNumCTWins + 1) * (0.2f * m_iNumCTWins + 1);

	int health[10] = { 100, 64, 35, 76, 52, 64, 78, 82, 92, 130 };
	int max_health = 0;

	switch (RANDOM_LONG(1, 10))
	{
	monster->pev->health = max_health;

	case 1: max_health = health[0];
		break;
		monster->pev->health = max_health;
	case 2: max_health = health[1];
		break;
		monster->pev->health = max_health;
	case 3: max_health = health[2];
		break;
		monster->pev->health = max_health;
	case 4: max_health = health[3];
		break;
		monster->pev->health = max_health;
	case 5: max_health = health[4];
		break;
		monster->pev->health = max_health;
	case 6: max_health = health[5];
		break;
		monster->pev->health = max_health;
	case 7: max_health = health[6];
		break;
		monster->pev->health = max_health;
	case 8: max_health = health[7];
		break;
		monster->pev->health = max_health;
	case 9: max_health = health[8];
		break;
		monster->pev->health = max_health;
	case 10: max_health = health[9];
		break;
	}

	if (m_iNumCTWins < 5 || RANDOM_LONG(0, 3))
	{
		//monster->pev->health = monster->pev->max_health = monster->pev->max_health / 2;

		SET_MODEL(monster->edict(), "models/shelter/item_hbeam.mdl");

		UTIL_SetSize(monster->pev, VEC_HULL_MIN, VEC_HULL_MAX);
	}

	monster->m_pMeatStrategy.reset(new CResModStrategy_ZSH2(monster, this));

	return monster;
}

void _IBaseMod_RemoveObjects_CheckMapConditions_impl2(IBaseMod* mod)
{
	//CHalfLifeMultiplay::CheckMapConditions();

	// Check to see if this map has a bomb target in it
	mod->m_bMapHasBombTarget = false;
	mod->m_bMapHasBombZone = false;

	// Check to see if this map has hostage rescue zones
	mod->m_bMapHasRescueZone = false;

	// See if the map has func_buyzone entities
	// Used by CBasePlayer::HandleSignals() to support maps without these entities
	mod->m_bMapHasBuyZone = true;

	// GOOSEMAN : See if this map has func_escapezone entities
	mod->m_bMapHasEscapeZone = false;

	// Check to see if this map has VIP safety zones
	mod->m_iMapHasVIPSafetyZone = MAP_HAVE_VIP_SAFETYZONE_NO;

	// Hostage
	/*CBaseEntity *hostage = nullptr;
	while ((hostage = UTIL_FindEntityByClassname(hostage, "hostage_entity")) != nullptr)
	{
		// should be removed.
		REMOVE_ENTITY(hostage->edict());
	}*/
}

BOOL _IBaseMod_RemoveObjects_IsAllowedToSpawn_impl2(IBaseMod* mod, CBaseEntity* pEntity) {
	if (!Q_strcmp(STRING(pEntity->pev->classname), "func_bomb_target") ||
		!Q_strcmp(STRING(pEntity->pev->classname), "info_bomb_target")) {
		return FALSE;
	}
	if (!Q_strcmp(STRING(pEntity->pev->classname), "func_hostage_rescue")) {
		return FALSE;
	}
	if (!Q_strcmp(STRING(pEntity->pev->classname), "func_escapezone")) {
		return FALSE;
	}
	if (!Q_strcmp(STRING(pEntity->pev->classname), "func_vip_safetyzone")) {
		return FALSE;
	}
	if (!Q_strcmp(STRING(pEntity->pev->classname), "hostage_entity")) {
		return FALSE;
	}
	return TRUE;
}

edict_t* _IBaseMod_RandomSpawn_GetPlayerSpawnSpot_impl2(IBaseMod* mod, CBasePlayer* pPlayer)
{
	// completely rewrites it

	// select spawnpoint from both teams.
	TeamName iBackupTeam = pPlayer->m_iTeam;
	pPlayer->m_iTeam = static_cast<TeamName>(RANDOM_LONG(TERRORIST, CT));

	// gat valid spawn point
	edict_t* pentSpawnSpot = EntSelectSpawnPoint(pPlayer);

	pPlayer->m_iTeam = iBackupTeam;

	// Move the player to the place it said.
	// Note that here has been modified
	if (!RES_DoRandomSpawn(pPlayer))
	{
		pPlayer->pev->origin = VARS(pentSpawnSpot)->origin + Vector(0, 0, 1);
		pPlayer->pev->v_angle = g_vecZero;
		pPlayer->pev->velocity = g_vecZero;
		pPlayer->pev->angles = VARS(pentSpawnSpot)->angles;
	}

	pPlayer->pev->punchangle = g_vecZero;
	pPlayer->pev->fixangle = 1;

	if (mod->IsMultiplayer())
	{
		if (pentSpawnSpot->v.target)
		{
			FireTargets(STRING(pentSpawnSpot->v.target), pPlayer, pPlayer, USE_TOGGLE, 0);
		}
	}

	return pentSpawnSpot;
}