/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "game.h"
#include "client.h"
#include "bmodels.h"
#include "globals.h"

#include "mod_zbshelter_pve.h"

#include "zsh/zsh_const.h"
#include "zsh/resources/resources.h"
#include "zsh/spawnres.h"
#include "zsh/resources/meat.h"
#include "zsh/resources/meat_manager.h"
#include "zsh/resources/resource_group.h"
#include "zsh/resources/wood.h"
#include "zsh/resources/wood_manager.h"
#include "zsh/resources/resrandomspawn.h"
#include "zsh/buildobject/Generator/male_generator.h"
#include "zsh/NPC/Zombie/zm_tank.h"
#include "zsh/NPC/Zombie/zm_light.h"
#include "zsh/resources/zsh_subs.h"
#include "zsh/build.h"
#include "zsh/base.h"
#include "zsh/buildobject/Turrets/automatic turret.h"
#include "zsh/buildobject/Turrets/gatling turret.h"
#include "zsh/buildobject/Turrets/gatling turret_manager.h"
#include "../dlls/gamemode/zsh/NPC/zsh_zm.h"

#include "../dlls/gamemode/zbs/zs_subs.h"
#include "player/csdm_randomspawn.h"
#include "game_shared2/GameEvent.h"	

#include <vector>
#include <random>
#include <algorithm>
#include <dlls/util/u_range.hpp>

class PlayerModStrategy_ZSH: public CPlayerModStrategy_Default
{
public:
	PlayerModStrategy_ZSH(CBasePlayer* pPlayer, CMod_ZombieShelter_coop* mp) : CPlayerModStrategy_Default(pPlayer), updateres()
	{
		m_listenerMonsterKilled2 = mp->m_eventWoodKilled.subscribe(
			[=](CWood* victim, CBaseEntity* attacker)
			{
				if (attacker == m_pPlayer)
				{
					updateres.UpdateWoods(m_pPlayer);
				}
			}
		);

		m_listenerMonsterKilled = mp->m_eventMeatKilled.subscribe(
			[=](CMeat* victim, CBaseEntity* attacker)
			{
				if (attacker == m_pPlayer)
				{
					updateres.UpdateMetal(m_pPlayer);
				}
			}
		);

		m_listenerMonsterKilled3 = mp->m_eventTankKilled.subscribe(
			[=](CZMTank* victim, CBaseEntity* attacker)
			{
				if (attacker == m_pPlayer)
				{
					if (victim->m_iKillBonusFrags)
						m_pPlayer->AddPoints(victim->m_iKillBonusFrags, FALSE);
				
					MESSAGE_BEGIN(MSG_ONE, gmsgZSHMsg, NULL, m_pPlayer->pev);
					WRITE_BYTE(ZSHKill);
					MESSAGE_END();
				}
			}
		);

		m_listenerMonsterKilled4 = mp->m_eventLightKilled.subscribe(
			[=](CZMLight* victim, CBaseEntity* attacker)
			{
				if (attacker == m_pPlayer)
				{
					if (victim->m_iKillBonusFrags)
						m_pPlayer->AddPoints(victim->m_iKillBonusFrags, FALSE);

					MESSAGE_BEGIN(MSG_ONE, gmsgZSHMsg, NULL, m_pPlayer->pev);
					WRITE_BYTE(ZSHKill);
					MESSAGE_END();
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

	void OnThink() override
	{
	
	}

	void OnSpawn() override
	{
		updateres.UpdateHUDBar(m_pPlayer);
		updateres.UpdateHUDBarHome();
	}
	
	void GiveDefaultItems() override
	{
		for (int i = 0; i < 1; i++)
		{
			if (m_pPlayer->pev->health > 1)
			{
				GiveWeaponsToPlayer();
				//update.spawn();
			}
		}
	}

	void OnInitHUD() override
	{
		updateres.ResetRes();
		updateres.ResetResHome();
		//shelter.shelterattack == true;
		//updateres.shelt(shelter);
	}
	
	void GiveWeaponsToPlayer() const
	{
		m_pPlayer->GiveNamedItem("weapon_shelteraxe");
		//m_pPlayer->GiveNamedItem("weapon_c4shelter");
	}

protected:

	float m_flRagePercent;
	float m_flDeadTime;
	float m_flBackupMaxHealth;
	float m_flBackupArmor;

	EventListener m_listenerMonsterKilled;
	EventListener m_listenerMonsterKilled2;
	EventListener m_listenerMonsterKilled3;
	EventListener m_listenerMonsterKilled4;

	EventListener m_listenerAdjustDamage;
	EventListener m_listenerAdjustDamage2;
	EventListener m_listenerAdjustDamage3;	
	EventListener m_listenerAdjustDamage4;
	CMod_ZombieShelter_coop updateres;
	CShelter *shelter;
};

class PlayerModStrategy_ZSH2 : public CPlayerModStrategy_Default
{
public:
	PlayerModStrategy_ZSH2(CBasePlayer* pPlayer, CMod_ZombieShelter_coop* mp) : CPlayerModStrategy_Default(pPlayer) {}

	void OnInitHUD() override{}

protected:
	
};

class CResModStrategy_ZSH : public CWoodModStrategy_Default
{
	friend class PlayerModStrategy_ZBS;

public:
	CResModStrategy_ZSH(CWood* p, CMod_ZombieShelter_coop* pGameRules) : CWoodModStrategy_Default(p), mp(pGameRules){}

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
	CResModStrategy_ZSH2(CMeat* p, CMod_ZombieShelter_coop* pGameRules) : CMeatModStrategy_Default(p), mp(pGameRules){}

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

class CZMTankModStrategy_ZSH : public CZMTankModStrategy_Default
{
	friend class PlayerModStrategy_ZSH;

public:
	CZMTankModStrategy_ZSH(CZMTank* p, CMod_ZombieShelter_coop* pGameRules) : CZMTankModStrategy_Default(p), mp(pGameRules){}

	void OnKilled(entvars_t* pevKiller, int iGib) override
	{
		CZMTankModStrategy_Default::OnKilled(pevKiller, iGib);
		if (pevKiller)
		{
			CBaseEntity* pKiller = CBaseEntity::Instance(pevKiller);
			mp->m_eventTankKilled.dispatch(m_pZMTank, pKiller);
		}
	}

protected:
	CMod_ZombieShelter_coop* const mp;
};

class CZMLightModStrategy_ZSH : public CZMLightModStrategy_Default
{
	friend class PlayerModStrategy_ZSH;

public:
	CZMLightModStrategy_ZSH(CZMLight* p, CMod_ZombieShelter_coop* pGameRules) : CZMLightModStrategy_Default(p), mp(pGameRules){}

	void OnKilled(entvars_t* pevKiller, int iGib) override
	{
		CZMLightModStrategy_Default::OnKilled(pevKiller, iGib);
		if (pevKiller)
		{
			CBaseEntity* pKiller = CBaseEntity::Instance(pevKiller);
			mp->m_eventLightKilled.dispatch(m_pZMLight, pKiller);
		}
	}

protected:
	CMod_ZombieShelter_coop* const mp;
};

class CZMTurretModStrategy_ZSH : public CZMTurretModStrategy_Default
{
	friend class PlayerModStrategy_ZSH;

public:
	CZMTurretModStrategy_ZSH(CTurret* p, CMod_ZombieShelter_coop* pGameRules) : CZMTurretModStrategy_Default(p), mp(pGameRules) {}
	

protected:
	CMod_ZombieShelter_coop* const mp;
};

class CBDGatlingModStrategy_ZSH : public CZMGatlingModStrategy_Default
{
	friend class PlayerModStrategy_ZSH;

public:
   	CBDGatlingModStrategy_ZSH(CZSHGatling* p, CMod_ZombieShelter_coop* pGameRules) : CZMGatlingModStrategy_Default(p), mp(pGameRules) {}

	void OnKilled(entvars_t* pevKiller, int iGib) override
	{
		CZMGatlingModStrategy_Default::OnKilled(pevKiller, iGib);
		if (pevKiller)
		{
			CBaseEntity* pKiller = CBaseEntity::Instance(pevKiller);
			mp->m_eventGatlingKilled.dispatch(m_pZSHGatling, pKiller);
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

	for (int iIndex = 1; iIndex <= gpGlobals->maxClients; ++iIndex)
	{
		CBaseEntity* entity = UTIL_PlayerByIndex(iIndex);
		if (!entity)
			continue;
		CLIENT_COMMAND(entity->edict(), "spk zsh/skill_bonus.wav\n");
	}
	
	pPlayer->m_iKevlar = ARMOR_TYPE_HELMET;
	pPlayer->pev->health = 100;
	pPlayer->pev->armorvalue = 100;
	pPlayer->SpawnProtection_Start(3.0f);
}

CMod_ZombieShelter_coop::CMod_ZombieShelter_coop()
{
	m_iRoundTimeSecs = m_iIntroRoundTime = 20 + 2;
	m_iMaxMapTime = 19999;
	m_fIntroRoundCount = 19999;
	mentalitysecond = 5;
	//mentalityhealth = 50;
	start = false;

	PRECACHE_SOUND("zsh/BGM_start.wav");
	PRECACHE_SOUND("zsh/zsh_dead.wav");
	PRECACHE_GENERIC("sound/zsh/BGM_zombie_attack.mp3");

	CVAR_SET_FLOAT("sv_maxspeed", 990 );

	UTIL_PrecacheOther("info_resources");
	UTIL_PrecacheOther("monster_turret");
	UTIL_PrecacheOther("monster_miniturret");
	UTIL_PrecacheOther("mentality");

	Reset();
	UpdateHUD();
}

void CMod_ZombieShelter_coop::CheckMapConditions()
{
	CBaseEntity* fog = nullptr;
	while ((fog = UTIL_FindEntityByClassname(fog, "env_fog")) != nullptr)
	{
		REMOVE_ENTITY(fog->edict());
	}
	CBaseEntity* sp = nullptr;

	m_vecWoodSpawns.clear();
	while ((sp = UTIL_FindEntityByClassname(sp, "info_resources")) != nullptr)
	{
		m_vecWoodSpawns.push_back(static_cast<CResSpawn*>(sp));
	}

	m_vecMetalSpawns.clear();
	while ((sp = UTIL_FindEntityByClassname(sp, "info_resources2")) != nullptr)
	{
		m_vecMetalSpawns.push_back(static_cast<CResSpawn2*>(sp));
	}

	m_ZombieSpawns.clear();
	while ((sp = UTIL_FindEntityByClassname(sp, "zombiespawn")) != nullptr)
	{
		m_ZombieSpawns.push_back(static_cast<CZombieSpawn*>(sp));
	}

	m_vecMenSpawns.clear();
	while ((sp = UTIL_FindEntityByClassname(sp, "morale_static")) != nullptr)
	{
		m_vecMenSpawns.push_back(static_cast<CMenSpawn*>(sp));
	}

	while ((sp = UTIL_FindEntityByClassname(sp, "info_tr_start")) != NULL)
		++m_iSpawnPointCount_Terrorist;

	while ((sp = UTIL_FindEntityByClassname(sp, "info_ct_start")) != NULL)
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

void CMod_ZombieShelter_coop::DaySound()
{
	for (int iIndex = 1; iIndex <= gpGlobals->maxClients; ++iIndex)
	{
		CBaseEntity* entity = UTIL_PlayerByIndex(iIndex);
		if (!entity)
			continue;
		CLIENT_COMMAND(entity->edict(), "spk zsh/skill_bonus.wav\n");	
	}
}

void CMod_ZombieShelter_coop::NightSound()
{
	for (int iIndex = 1; iIndex <= gpGlobals->maxClients; ++iIndex)
	{
		CBaseEntity* entity = UTIL_PlayerByIndex(iIndex);
		if (!entity)
			continue;
		CLIENT_COMMAND(entity->edict(), "mp3 loop sound/zsh/BGM_zombie_attack.mp3\n");
		CLIENT_COMMAND(entity->edict(), "spk sound/zsh/monster/zombies_amb.wav\n");
	}
}

void CMod_ZombieShelter_coop::UpdateDay()
{
	dayses++;
	UpdateHUD();
	DaySound();
}

void CMod_ZombieShelter_coop::Reset()
{
	dayses = 1;
}

void CMod_ZombieShelter_coop::UpdateHUD()
{
	MESSAGE_BEGIN(MSG_ALL, gmsgZSHUpdateDay, NULL);
	WRITE_BYTE(0);
	WRITE_BYTE(dayses);
	MESSAGE_END();

	MESSAGE_BEGIN(MSG_ALL, gmsgZSHMsgTextNextDay, NULL);
	WRITE_BYTE(ZSHMessagebox);
	WRITE_BYTE(dayses);
	MESSAGE_END();
}

void CMod_ZombieShelter_coop::ResetTime()
{
	daytimes = 240;
	nighttimes = 120;
}

void CMod_ZombieShelter_coop::ResetTime2()
{
	dayseconds = 20;
	nightseconds = 60;
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
		ResetTime();
		ResetTime2();
		daytimer = true;
	}
}

void CMod_ZombieShelter_coop::UpdateBarMentality()
{
	MESSAGE_BEGIN(MSG_ONE, gmsgZSHMsgMentality, NULL);
	WRITE_BYTE(0);
	WRITE_BYTE(mentalityhealth);
	MESSAGE_END();
}

void CMod_ZombieShelter_coop::Think()
{
	UpdateBarMentality();

	if (CheckGameOver())
		return;
	
	TeamCheck();
	CheckLevelInitialized();
	CheckRoundTimeExpired();
	CheckMentality();
	MakeMenThink();

	MESSAGE_BEGIN(MSG_ALL, gmsgZSHUpdateTime, NULL);
	WRITE_BYTE(0);
	WRITE_BYTE(dayminutes);
	WRITE_BYTE(nightminutes);
	WRITE_BYTE(daytimer);
	WRITE_BYTE(dayseconds);
	WRITE_BYTE(nightseconds);
	MESSAGE_END();

	if (nighttimer == true)
	{
		if (nighttimes > 118)
		{
			ClearDayZombie();
			dayzombies = false;
		}
	}

	if (daytimes == 240)
	{
		dayminutes = 4;
		dayseconds = 0;
	}
	else if (daytimes > 180)
	{
		dayminutes = 3;	
	}
	else if (daytimes > 120)
	{
		dayminutes = 2;	
	}
	else if (daytimes > 60)
	{
		dayminutes = 1;
	}
	else
	{
		dayminutes = 0;
	}

	if (nighttimes == 120)
	{
		nightminutes = 2;		
	}
	else if (nighttimes > 60)
	{
		nightminutes = 1;
	}
	else
	{
		nightminutes = 0;		
	}

	if (gpGlobals->time - tWorldTime5 < 1.0f)
	{
		tDelta5 += gpGlobals->time - tWorldTime5;
	}
	if (tNextAttack5 > 1.0f || (gpGlobals->time - tWorldTime5 > 1.0f) || tDelta5 > 1.0f)
	{
		tNextAttack5 = 0.0f;
		tDelta5 = 0.0f;

		if (start == true)
		{
			if (nightseconds == 0)
			{
				nightseconds = 60;
			}

			if (dayseconds == 0)
			{
				dayseconds = 60;
			}
			
			if (daytimes > 1)
			{
				nighttimer = false;
				daytimer = true;
			}
			else
			{
				daytimer = false;
				nighttimer = true;
			}
			
			if (daytimer == true)
			{
				daytimes--;
				dayseconds--;
				dayzombies = true;
			}

			if (nighttimer == true)
			{
				nighttimes--;
				nightseconds--;
				
				if (daytimer == false)
				{
					if (nighttimes > 118)
					{
						NightSound();
					}
					if (nighttimes < 57)
					{
						for (int iIndex = 1; iIndex <= gpGlobals->maxClients; ++iIndex)
						{
							CBaseEntity* entity = UTIL_PlayerByIndex(iIndex);
							if (!entity)
								continue;
							CLIENT_COMMAND(entity->edict(), "mp3 stop\n");
						}
					}
					if (nighttimes == 0)
					{
						ResetTime();
						UpdateDay();
						ClearResources();
						daytimer = true;
						nighttimer = false;
						dayzombies = true;
					}
				}
			}
		}
	}
	tWorldTime5 = gpGlobals->time;
	
	if (IsFreezePeriod())
	{
		CheckRoundTimeExpired();

		static int iLastCountDown = -1;
		int iCountDown = TimeRemaining();

		if (iCountDown > 0)
		{
			if (iCountDown != iLastCountDown)
			{
				iLastCountDown = iCountDown;
				if (iCountDown > 0 && iCountDown < 20)
				{
					MESSAGE_BEGIN(MSG_ALL, gmsgZSHMsgText);
					WRITE_BYTE(ZSHMessageTime);
					WRITE_BYTE(iCountDown);
					MESSAGE_END();
				}

				if (iCountDown == 18)
				{
					CreateMentality();
					WaitingSound();
				}
			}
		}
		CheckFreezePeriodExpired();
	}
	if (FRoundStarted() && !m_bRoundTerminating)
	{
		start = true;
		if (gpGlobals->time > m_flNextSpawnRes)
		{
			MakeResources();
			MakeResources2();
			
			//MakeTurret();
			//MakeGatling();
			m_flNextSpawnRes = gpGlobals->time + 10.0f;
		}
		if (gpGlobals->time > m_flNextSpawnNPC)
		{
			if (dayzombies == true)
			{
				MakeZmTank();
				MakeZmLight();
			}
			else
			{
				MakeZmTankNight();
			}
			
			if (dayzombies == true)
			{
				m_flNextSpawnNPC = gpGlobals->time + 30.0f;
			}
			else
				m_flNextSpawnNPC = gpGlobals->time + 7.0f;			
		}
		if (gpGlobals->time > m_flNextSpawnMen)
		{
			ClearResources();
			m_flNextSpawnMen = gpGlobals->time + 250.0f;
		}
	}

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
	for (int iIndex = 1; iIndex <= gpGlobals->maxClients; ++iIndex)
	{
		CBaseEntity* entity = UTIL_PlayerByIndex(iIndex);
		CBasePlayer* player = static_cast<CBasePlayer*>(entity);
		const char* pcmd = CMD_ARGV_(0);
		int slot = Q_atoi(CMD_ARGV_(1));

		if (!entity)
			continue;

		if (FStrEq(pcmd, "zsh_mentality1"))
		{
			mentality = false;
		}
		else if (FStrEq(pcmd, "zsh_mentality0"))
		{
			mentality = true;
		}

		if (FStrEq(pcmd, "survel"))
		{
			for (int iIndex = 1; iIndex <= gpGlobals->maxClients; ++iIndex)
			{
				CBaseEntity* entity = UTIL_PlayerByIndex(iIndex);
				if (!entity)
					continue;
				CLIENT_COMMAND(entity->edict(), "spk zsh/skill_bonus.wav\n");
			}
			ShowMenu(player, (MENU_KEY_1 | MENU_KEY_2 | MENU_KEY_0), -1, 0, "#ZbsUpgrade");
			//SurvSkills(player, slot);			
			return;
		}
		if (HandleSkillsAliasCommands(player, pcmd))
				return;
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

		if (gpGlobals->time < player->m_fDeadTime + 15.0f)
			continue;

		player->RoundRespawn();
	}
}

void CMod_ZombieShelter_coop::SurvSkills(CBasePlayer* player, int slot)
{
	switch (slot)
	{
	case 1:
		//player->Skill("%!MRAD_DEX1", "#CstzUI_DEX");
		for (int iIndex = 1; iIndex <= gpGlobals->maxClients; ++iIndex)
		{
			CBaseEntity* entity = UTIL_PlayerByIndex(iIndex);
			if (!entity)
				continue;
			CLIENT_COMMAND(entity->edict(), "spk zsh/skill_bonus.wav\n");
		}
		break;
	case 2:
		player->Skill("%!MRAD_Master1", "#CstzUI_Master");
		break;
	case 3:
		player->Skill("%!MRAD_Searching1", "#CstzUI_Searching");
		break;
	default:
		break;
	}
}

BOOL CMod_ZombieShelter_coop::HandleSkillsAliasCommands(CBasePlayer* pPlayer, const char* pszCommand)
{
	BOOL bRetVal = FALSE;

	if (FStrEq(pszCommand, "dex"))
	{
		bRetVal = TRUE;
		SurvSkills(pPlayer, 1);
	}
	else if (FStrEq(pszCommand, "surv"))
	{
		bRetVal = TRUE;
		SurvSkills(pPlayer, 2);
	}
	else if (FStrEq(pszCommand, "search"))
	{
		bRetVal = TRUE;
		SurvSkills(pPlayer, 3);
	}

	else if (FStrEq(pszCommand, "generator"))
	{
		CreateGenerator();
		bRetVal = TRUE;
	}

	return bRetVal;
}

void CMod_ZombieShelter_coop::MakeMenThink()
{

	if (gpGlobals->time < m_flTimeNextMakeSupplybox)
		return;
	m_flTimeNextMakeSupplybox = gpGlobals->time + 1.0f;
	RemoveAllMen();
	

	int iSupplyboxCount = 2;
	for (int i = 0; i < iSupplyboxCount; ++i)
	{
		CMenSpawn* sb = SelectMenSpawn();
		//sb->m_iMentalityIndex = i + 1;

		for (CBasePlayer* player : moe::range::PlayersList())
		{
			if (player->m_bIsZombie)
				continue;


			//MESSAGE_BEGIN(MSG_ALL, gmsgZSHMsgShelterPos, nullptr, player->pev);
		//	WRITE_BYTE(1);
		//	WRITE_BYTE(sb->m_iId);
		//	WRITE_COORD(sb->pev->origin.x);
		//	WRITE_COORD(sb->pev->origin.y);
		//	WRITE_COORD(sb->pev->origin.z);
		//	MESSAGE_END();
		}
	}
}

void CMod_ZombieShelter_coop::RemoveAllMen()
{
	CBaseEntity* ent = nullptr;
	while ((ent = UTIL_FindEntityByClassname(ent, "mentality")) != nullptr)
	{
		CMentalityHealth* sb = dynamic_ent_cast<CMentalityHealth*>(ent);
		sb->SendPositionMsg();
		sb->SetThink(&CBaseEntity::SUB_Remove);
	}
}

void CMod_ZombieShelter_coop::CheckFreezePeriodExpired()
{
	if (TimeRemaining() > 0)
		return;

	// Log this information
	UTIL_LogPrintf("World triggered \"Round_Start\"\n");

	// Freeze period expired: kill the flag
	m_bFreezePeriod = FALSE;

	char CT_sentence[40];
	char T_sentence[40];

	switch (RANDOM_LONG(0, 3))
	{
	case 0:
		Q_strncpy(CT_sentence, "%!MRAD_MOVEOUT", sizeof(CT_sentence));
		Q_strncpy(T_sentence, "%!MRAD_MOVEOUT", sizeof(T_sentence));
		break;
	case 1:
		Q_strncpy(CT_sentence, "%!MRAD_LETSGO", sizeof(CT_sentence));
		Q_strncpy(T_sentence, "%!MRAD_LETSGO", sizeof(T_sentence));
		break;
	case 2:
		Q_strncpy(CT_sentence, "%!MRAD_LOCKNLOAD", sizeof(CT_sentence));
		Q_strncpy(T_sentence, "%!MRAD_LOCKNLOAD", sizeof(T_sentence));
		break;
	default:
		Q_strncpy(CT_sentence, "%!MRAD_GO", sizeof(CT_sentence));
		Q_strncpy(T_sentence, "%!MRAD_GO", sizeof(T_sentence));
		break;
	}

	// More specific radio commands for the new scenarios : Prison & Assasination
	if (m_bMapHasEscapeZone)
	{
		Q_strncpy(CT_sentence, "%!MRAD_ELIM", sizeof(CT_sentence));
		Q_strncpy(T_sentence, "%!MRAD_GETOUT", sizeof(T_sentence));
	}
	else if (m_iMapHasVIPSafetyZone == MAP_HAVE_VIP_SAFETYZONE_YES)
	{
		Q_strncpy(CT_sentence, "%!MRAD_VIP", sizeof(CT_sentence));
		Q_strncpy(T_sentence, "%!MRAD_LOCKNLOAD", sizeof(T_sentence));
	}

	bool bCTPlayed = false;
	bool bTPlayed = false;

	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		CBasePlayer* plr = static_cast<CBasePlayer*>(UTIL_PlayerByIndex(i));

		if (!plr || plr->pev->flags == FL_DORMANT)
		{
			continue;
		}

		if (plr->m_iJoiningState == JOINED)
		{
			if (plr->m_iTeam == CT && !bCTPlayed)
			{
				plr->Radio(CT_sentence);
				bCTPlayed = true;
			}
			else if (plr->m_iTeam == TERRORIST && !bTPlayed)
			{
				plr->Radio(T_sentence);
				bTPlayed = true;
			}

			if (plr->m_iTeam != SPECTATOR)
			{
				plr->ResetMaxSpeed();
				plr->m_bCanShoot = true;
			}
		}
	}
}

void CMod_ZombieShelter_coop::CheckRoundTimeExpired()
{
	if (!HasRoundTimeExpired())
		return;
}

bool CMod_ZombieShelter_coop::HasRoundTimeExpired()
{
	return false;
}

BOOL CMod_ZombieShelter_coop::FInfectionStarted()
{
	const int iCountDown = static_cast<int>(gpGlobals->time - m_fRoundCount);
	return iCountDown > 20;
}

BOOL CMod_ZombieShelter_coop::FRoundStarted()
{
	return !IsFreezePeriod();
}

void CMod_ZombieShelter_coop::RoundEndScore(int iWinStatus)
{
	for (CBasePlayer* player : moe::range::PlayersList())
	{
		if (player->m_iTeam == TEAM_UNASSIGNED || player->m_iTeam == TEAM_SPECTATOR)
			continue;

		if (iWinStatus == WINSTATUS_CTS)
		{
			if (player->IsAlive() && !player->m_bIsZombie)
			{
				player->pev->frags += 3;

				MESSAGE_BEGIN(MSG_BROADCAST, gmsgScoreInfo);
				WRITE_BYTE(ENTINDEX(player->edict()));
				WRITE_SHORT((int)player->pev->frags);
				WRITE_SHORT(player->m_iDeaths);
				WRITE_SHORT(0);
				WRITE_SHORT(player->m_iTeam);
				MESSAGE_END();
			}
		}
		else if (iWinStatus == WINSTATUS_TERRORISTS)
		{
			if (player->m_bIsZombie)
			{
				player->pev->frags += 1;

				MESSAGE_BEGIN(MSG_BROADCAST, gmsgScoreInfo);
				WRITE_BYTE(ENTINDEX(player->edict()));
				WRITE_SHORT((int)player->pev->frags);
				WRITE_SHORT(player->m_iDeaths);
				WRITE_SHORT(0);
				WRITE_SHORT(player->m_iTeam);
				MESSAGE_END();
			}
		}
	}
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
		}
	}
}

CGenSpawn* CMod_ZombieShelter_coop::SelectGenSpawnPoint()
{
	size_t const iSize = m_vecGenSpawns.size();
	if (!iSize)
		return nullptr;
	return m_vecGenSpawns[RANDOM_LONG(0, iSize - 1)];
}

CResSpawn* CMod_ZombieShelter_coop::SelectWoodSpawnPoint()
{
	size_t const iSize = m_vecWoodSpawns.size();
	if (!iSize)
		return nullptr;
	return m_vecWoodSpawns[RANDOM_LONG(0, iSize - 1)];
}

CResSpawn2* CMod_ZombieShelter_coop::SelectMetalSpawnPoint()
{
	size_t const iSize = m_vecMetalSpawns.size();
	if (!iSize)
		return nullptr;
	return m_vecMetalSpawns[RANDOM_LONG(0, iSize - 1)];
}

CMenSpawn* CMod_ZombieShelter_coop::SelectMenSpawn()
{
	size_t const iSize = m_vecMenSpawns.size();
	if (!iSize)
		return nullptr;
	return m_vecMenSpawns[RANDOM_LONG(0, iSize - 1)];
}

CZombieSpawn* CMod_ZombieShelter_coop::SelectZombieSpawn()
{
	size_t const iSize = m_ZombieSpawns.size();
	if (!iSize)
		return nullptr;
	return m_ZombieSpawns[RANDOM_LONG(0, iSize - 1)];
}

CMentalityHealth* CMod_ZombieShelter_coop::CreateMentality()
{
	//CMentalityHealth* supplybox = GetClassPtr<CMentalityHealth>(nullptr);
	auto supplybox = CreateClassPtr<CMentalityHealth>();
	if (!supplybox)
		return nullptr;

	CMenSpawn* sp = SelectMenSpawn();
	if (sp)
	{
		supplybox->pev->origin = sp->pev->origin;
		supplybox->pev->angles = sp->pev->angles;
	}
	else
	{
		Vector backup_v_angle = supplybox->pev->v_angle;
		RES_DoRandomSpawn(supplybox);
		supplybox->pev->v_angle = backup_v_angle;
	}

	supplybox->pev->spawnflags |= SF_NORESPAWN;

	DispatchSpawn(supplybox->edict());
}

CGeneratorMale* CMod_ZombieShelter_coop::CreateGenerator()
{
	//CGeneratorMale* generator = GetClassPtr<CGeneratorMale>(nullptr);
	auto generator = CreateClassPtr<CGeneratorMale>();
	if (!generator)
		return nullptr;

	CMenSpawn* sp = SelectMenSpawn();
	if (sp)
	{
		generator->pev->origin = sp->pev->origin;
		generator->pev->angles = sp->pev->angles;
	}
	else
	{
		Vector backup_v_angle = generator->pev->v_angle;
		RES_DoRandomSpawn(generator);
		//generator->pev->v_angle = backup_v_angle;
	}

	generator->pev->spawnflags |= SF_NORESPAWN;

	DispatchSpawn(generator->edict());
}

CBaseEntity* CMod_ZombieShelter_coop::MakeResources()
{
	CWood* monster = GetClassPtr<CWood>(nullptr);

	if (!monster)
		return nullptr;

	edict_t* pent = monster->edict();

	CResSpawn* sp = SelectWoodSpawnPoint();
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

	if (m_iNumCTWins < 5 || RANDOM_LONG(0, 3))
	{
		switch (RANDOM_LONG(1, 10))
		{

		case 1:
			monster->pev->health = monster->pev->max_health = 29;
			break;

		case 2:
			monster->pev->health = monster->pev->max_health = 42;
			break;

		case 3:
			monster->pev->health = monster->pev->max_health = 85;
			break;

		case 4:
			monster->pev->health = monster->pev->max_health = 49;
			break;

		case 5:
			monster->pev->health = monster->pev->max_health = 10;
			break;

		case 6:
			monster->pev->health = monster->pev->max_health = 32;
			break;

		case 7:
			monster->pev->health = monster->pev->max_health = 93;
			break;

		case 8:
			monster->pev->health = monster->pev->max_health = 97;
			break;

		case 9:
			monster->pev->health = monster->pev->max_health = 34;
			break;

		case 10:
			monster->pev->health = monster->pev->max_health = 70;
			break;
		default:
			monster->pev->health = monster->pev->max_health = 50;
			break;
		}
		
		SET_MODEL(monster->edict(), "models/shelter/item_wood.mdl");
		//monster->pev->rendermode = kRenderTransAdd;
		//monster->pev->renderamt = 200;
		//monster->pev->renderfx = 200;
		UTIL_SetSize(monster->pev, VEC_HULL_MIN, VEC_HULL_MAX);

		MESSAGE_BEGIN(MSG_ALL, gmsgHeadIcon);
		WRITE_BYTE(14);
		WRITE_SHORT(ENTINDEX(monster->edict()));
		MESSAGE_END();

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

	CResSpawn2* sp = SelectMetalSpawnPoint();
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

	if (m_iNumCTWins < 5 || RANDOM_LONG(0, 3))
	{
		switch (RANDOM_LONG(1, 10))
		{

		case 1:
			monster->pev->health = monster->pev->max_health = 29;
			break;

		case 2:
			monster->pev->health = monster->pev->max_health = 42;
			break;

		case 3:
			monster->pev->health = monster->pev->max_health = 85;
			break;

		case 4:
			monster->pev->health = monster->pev->max_health = 49;
			break;

		case 5:
			monster->pev->health = monster->pev->max_health = 10;
			break;

		case 6:
			monster->pev->health = monster->pev->max_health = 32;
			break;

		case 7:
			monster->pev->health = monster->pev->max_health = 93;
			break;

		case 8:
			monster->pev->health = monster->pev->max_health = 97;
			break;

		case 9:
			monster->pev->health = monster->pev->max_health = 34;
			break;

		case 10:
			monster->pev->health = monster->pev->max_health = 170;
			break;
		default:
			monster->pev->health = monster->pev->max_health = 100;
			break;
		}
		SET_MODEL(monster->edict(), "models/shelter/item_hbeam.mdl");

		MESSAGE_BEGIN(MSG_ALL, gmsgHeadIcon);
		WRITE_BYTE(13);
		WRITE_SHORT(ENTINDEX(monster->edict()));
		MESSAGE_END();

		//monster->pev->rendermode = kRenderTransAdd;
		//monster->pev->renderamt = 200;
		//monster->pev->renderfx = 200;
		UTIL_SetSize(monster->pev, VEC_HULL_MIN, VEC_HULL_MAX);
	}

	monster->m_pMeatStrategy.reset(new CResModStrategy_ZSH2(monster, this));

	return monster;
}

CBaseEntity* CMod_ZombieShelter_coop::MakeTurret()
{
	CTurret* monster = GetClassPtr<CTurret>(nullptr);

	if (!monster)
		return nullptr;
	
	edict_t* pent = monster->edict();
	

	CZombieSpawn* sp = SelectZombieSpawn();
	if (sp)
	{
		monster->pev->origin = sp->pev->origin;
		monster->pev->angles = sp->pev->angles;
	}
	else
	{
		Vector backup_v_angle = monster->pev->v_angle;
		CSDM_DoRandomSpawn(monster);
		monster->pev->v_angle = backup_v_angle;
	}

	pent->v.spawnflags |= SF_NORESPAWN;

	DispatchSpawn(pent);
	monster->pev->health = monster->pev->max_health = 100 + m_iNumCTWins * 15;
	monster->pev->maxspeed = 140.0f + (m_iNumCTWins / static_cast<float>(3)) * 15;
	//monster->m_flAttackDamage = 4;

	if (m_iNumCTWins < 5 || RANDOM_LONG(0, 3))
	{
		monster->pev->health = monster->pev->max_health = monster->pev->max_health / 2;

		UTIL_SetSize(monster->pev, VEC_HULL_MIN, VEC_HULL_MAX);
	}

	monster->m_pTurretStrategy.reset(new CZMTurretModStrategy_ZSH(monster, this));

	return monster;
}

CBaseEntity* CMod_ZombieShelter_coop::MakeGatling()
{
	CZSHGatling* monster = GetClassPtr<CZSHGatling>(nullptr);

	if (!monster)
		return nullptr;

	edict_t* pent = monster->edict();


	CZombieSpawn* sp = SelectZombieSpawn();
	if (sp)
	{
		monster->pev->origin = sp->pev->origin;
		monster->pev->angles = sp->pev->angles;
	}
	else
	{
		Vector backup_v_angle = monster->pev->v_angle;
		CSDM_DoRandomSpawn(monster);
		monster->pev->v_angle = backup_v_angle;
	}

	pent->v.spawnflags |= SF_NORESPAWN;

	DispatchSpawn(pent);
	monster->pev->health = monster->pev->max_health = 100 + m_iNumCTWins * 15;
	monster->pev->maxspeed = 140.0f + (m_iNumCTWins / static_cast<float>(3)) * 15;
	//monster->m_flAttackDamage = 4;

	if (m_iNumCTWins < 5 || RANDOM_LONG(0, 3))
	{
		monster->pev->health = monster->pev->max_health = monster->pev->max_health / 2;

		UTIL_SetSize(monster->pev, VEC_HULL_MIN, VEC_HULL_MAX);
	}

	monster->m_pGatlingStrategy.reset(new CBDGatlingModStrategy_ZSH(monster, this));

	return monster;
}

CBaseEntity* CMod_ZombieShelter_coop::MakeZmTank()
{
	CZMTank* monster = GetClassPtr<CZMTank>(nullptr);

	if (!monster)
		return nullptr;

	edict_t* pent = monster->edict();

	CZombieSpawn* sp = SelectZombieSpawn();
	if (sp)
	{
		monster->pev->origin = sp->pev->origin;
		monster->pev->angles = sp->pev->angles;
	}
	else
	{
		Vector backup_v_angle = monster->pev->v_angle;
		CSDM_DoRandomSpawn(monster);
		monster->pev->v_angle = backup_v_angle;
	}

	pent->v.spawnflags |= SF_NORESPAWN;

	DispatchSpawn(pent);

	monster->pev->health = monster->pev->max_health = 100 + m_iNumCTWins * 15;
	monster->pev->maxspeed = 70.0f + (m_iNumCTWins / static_cast<float>(3)) * 15;
	monster->m_flAttackDamage = 2;
	
	//monster->pev->renderamt = 100.0;

	if (m_iNumCTWins < 5 || RANDOM_LONG(0, 3))
	{
		monster->pev->health = monster->pev->max_health = monster->pev->max_health / 2;
		switch (RANDOM_LONG(1, 2))
		{
		case 1: SET_MODEL(monster->edict(), "models/player/zombi_origin/zombi_origin.mdl"); break;
		case 2: SET_MODEL(monster->edict(), "models/player/zombi_host/zombi_host.mdl"); break;

		default:
			break;
		}

		UTIL_SetSize(monster->pev, VEC_HULL_MIN, VEC_HULL_MAX);
	}

	monster->m_pTankStrategy.reset(new CZMTankModStrategy_ZSH(monster, this));

	return monster;
}

CBaseEntity* CMod_ZombieShelter_coop::MakeZmTankNight()
{
	CZMTank* monster = GetClassPtr<CZMTank>(nullptr);

	if (!monster)
		return nullptr;

	edict_t* pent = monster->edict();

	CZombieSpawn* sp = SelectZombieSpawn();
	if (sp)
	{
		monster->pev->origin = sp->pev->origin;
		monster->pev->angles = sp->pev->angles;
	}
	else
	{
		Vector backup_v_angle = monster->pev->v_angle;
		CSDM_DoRandomSpawn(monster);
		monster->pev->v_angle = backup_v_angle;
	}

	pent->v.spawnflags |= SF_NORESPAWN;

	DispatchSpawn(pent);

	monster->pev->health = monster->pev->max_health = 100 + m_iNumCTWins * 15;
	monster->pev->maxspeed = 240.0f + (m_iNumCTWins / static_cast<float>(3)) * 15;
	monster->m_flAttackDamage = 6;

	if (m_iNumCTWins < 5 || RANDOM_LONG(0, 3))
	{
		monster->pev->health = monster->pev->max_health = monster->pev->max_health / 2;

		SET_MODEL(monster->edict(), "models/player/zombi_origin/zombi_origin5.mdl");

		UTIL_SetSize(monster->pev, VEC_HULL_MIN, VEC_HULL_MAX);
	}

	monster->m_pTankStrategy.reset(new CZMTankModStrategy_ZSH(monster, this));

	return monster;
}

CBaseEntity* CMod_ZombieShelter_coop::MakeZmLight()
{
	CZMLight* monster = GetClassPtr<CZMLight>(nullptr);

	if (!monster)
		return nullptr;

	edict_t* pent = monster->edict();

	CZombieSpawn* sp = SelectZombieSpawn();
	if (sp)
	{
		monster->pev->origin = sp->pev->origin;
		monster->pev->angles = sp->pev->angles;
	}
	else
	{
		Vector backup_v_angle = monster->pev->v_angle;
		CSDM_DoRandomSpawn(monster);
		monster->pev->v_angle = backup_v_angle;
	}

	pent->v.spawnflags |= SF_NORESPAWN;

	DispatchSpawn(pent);

	monster->pev->health = monster->pev->max_health = 200 + m_iNumCTWins * 15;
	monster->pev->maxspeed = 80.0f + (m_iNumCTWins / static_cast<float>(3)) * 15;
	monster->m_flAttackDamage = 3;

	if (m_iNumCTWins < 5 || RANDOM_LONG(0, 3))
	{
		monster->pev->health = monster->pev->max_health = monster->pev->max_health / 2;
		switch (RANDOM_LONG(1, 2))
		{
		case 1: SET_MODEL(monster->edict(), "models/player/speed_zombi_origin/speed_zombi_origin.mdl"); break;
		case 2: SET_MODEL(monster->edict(), "models/player/speed_zombi_host/speed_zombi_host.mdl"); break;

		default:
			break;
		}
		UTIL_SetSize(monster->pev, VEC_HULL_MIN, VEC_HULL_MAX);
	}

	monster->m_pLightStrategy.reset(new CZMLightModStrategy_ZSH(monster, this));

	return monster;
}

void CMod_ZombieShelter_coop::ClearDayZombie()
{
	dayzombies = false;
	CBaseEntity* npc = nullptr;
	while ((npc = UTIL_FindEntityByClassname(npc, "zm_tank")) != nullptr)
	{
		npc->Killed(nullptr, GIB_NORMAL);
	}
	CBaseEntity* npc2 = nullptr;
	while ((npc2 = UTIL_FindEntityByClassname(npc2, "zm_light")) != nullptr)
	{
		npc2->Killed(nullptr, GIB_NORMAL);
	}
}

void CMod_ZombieShelter_coop::ClearResources()
{
	CBaseEntity* npc = nullptr;
	while ((npc = UTIL_FindEntityByClassname(npc, "Wood_entity")) != nullptr)
	{
		npc->Killed(nullptr, GIB_NORMAL);
	}
	CBaseEntity* npc2 = nullptr;
	while ((npc2 = UTIL_FindEntityByClassname(npc2, "Meat_entity")) != nullptr)
	{
		npc2->Killed(nullptr, GIB_NORMAL);
	}
}