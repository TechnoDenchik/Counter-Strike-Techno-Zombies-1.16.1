/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#ifndef MOD_ZSH_H
#define MOD_ZSH_H
#ifdef _WIN32
#pragma once
#endif

#include "mod_base.h"
#include <vector>
#include "EventDispatcher.h"
#include "func_shelter.h"

class CBaseEntity;
class CBasePlayer;

class CResSpawn;
class CResSpawn2;
class CWood;
class CMeat;
class CTurret;

class CZMTank;
class CZMLight;
class CZSHGatling;
class CZombieSpawn;

class CSpawnres;
class CGenSpawn;
class CMenSpawn;
class CMentalityHealth;
class CShelter;
typedef struct entvars_s entvars_t;

class CMod_ZombieShelter_coop : public TBaseMod_RemoveObjects<>
{ 	
public:
	 CMod_ZombieShelter_coop();
public:
	CShelter* sheltattack{};
	BOOL _IBaseMod_RemoveObjects_IsAllowedToSpawn_impl(IBaseMod* mod, CBaseEntity* pEntity);
	void _IBaseMod_RemoveObjects_CheckMapConditions_impl(IBaseMod* mod);
	void TeamCheck();
	//BOOL IsTeamplay(void) override { return TRUE; }
	BOOL IsShelter(void) override { return TRUE; }
	void CheckMapConditions() override;
	void UpdateGameMode(CBasePlayer* pPlayer) override;
	void WaitingSound();
	void PlayerSpawn(CBasePlayer* pPlayer) override;
	void UpdateBarMentality();
	void Think() override;
	void ResetTime();
	void ResetTime2();
	void CheckRoundTimeExpired();
	void CheckFreezePeriodExpired();
	void SurvSkills(CBasePlayer* player, int slot);
	//void MakeGeneratorThink();
	//void RemoveAllGenerator();

	//void shelt(CShelter* sheltattack)
	//{
	//	sheltattack->shelterattack = true;
	//}

	void MakeMenThink();
	void RemoveAllMen();
	
	int MenCount();
	void ReadMultiplayCvars(CCstrikeTechnoZombies* mp);
	BOOL FRoundStarted();
	void DaySound();

	void UpdateDay();
	void spawn();
	void Reset();
	void UpdateHUD();

	void UpdateWoods(CBasePlayer* m_pPlayer);
	void UpdateMetal(CBasePlayer* m_pPlayer);

	void CheckMentality();

	void UpdateWoodsHome();
	void UpdateMetalHome();

	void ResetRes();
	void UpdateHUDBar(CBasePlayer* m_pPlayer);

	void ResetResHome();
	void UpdateHUDBarHome();
	CMentalityHealth* CreateMentality();

	void NightSound();
	virtual void RoundEndScore(int iWinStatus);
	bool HasRoundTimeExpired();
	void InstallPlayerModStrategy(CBasePlayer* player) override;
	BOOL ClientConnected(edict_t* pEntity, const char* pszName, const char* pszAddress, char* szRejectReason) override;
	void ClientDisconnected(edict_t* pClient) override;
	float GetAdjustedEntityDamage(CBaseEntity* victim, entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType) override;
	float GetAdjustedEntityDamage2(CBaseEntity* victim, entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage2, int bitsDamageType2) override;
public:
	float m_flTimeNextMakeSupplybox;
	
	int dayses;
	
	CResSpawn* SelectWoodSpawnPoint();
	CResSpawn2* SelectMetalSpawnPoint();
	CGenSpawn* SelectGenSpawnPoint();
	CZombieSpawn* SelectZombieSpawn();
	CMenSpawn* SelectMenSpawn();

	CBaseEntity* MakeResources();
	CBaseEntity* MakeResources2();
	CBaseEntity* MakeResources3();
	CBaseEntity* MakeTurret();
	CBaseEntity* MakeGatling();

	CBaseEntity* MakeZmTank();
	CBaseEntity* MakeZmTankNight();
	CBaseEntity* MakeZmLight();
	CBaseEntity* MakeZmLightNight();
	
	void ClearDayZombie();
	void ClearResources();
	bool start;
	BOOL FInfectionStarted();
	BOOL HandleSkillsAliasCommands(CBasePlayer* pPlayer, const char* pszCommand);
public:

	int daytimes;
	int nighttimes;

	int dayseconds;
	int nightseconds;

	int dayminutes;
	int nightminutes;

	int mentalityhealth;
	int mentalitysecond;

	bool daytimer;
	bool nighttimer;
	bool survover;
	bool mentality;
	bool dayzombies;

	int maxwoods;
	int maxmetal;
	int woods;
	int metal;

	int maxwoodshome;
	int maxmetalhome;
	int woodshome;
	int metalhome;

	int energy;
	int maxenergy;
	int m_iId;
	float m_flNextSpawnRes;
	float m_flNextSpawnNPC;
	float m_flNextSpawnMen;
	float m_flNextDay;
	float m_flNextDay2;
	float m_flNextDay3;
	float m_flNextDay4;
	std::vector<CResSpawn*> m_vecWoodSpawns;
	std::vector<CResSpawn2*> m_vecMetalSpawns;
	std::vector<CGenSpawn*> m_vecGenSpawns;
	std::vector<CMenSpawn*> m_vecMenSpawns;
	std::vector<CZombieSpawn*> m_ZombieSpawns;

	duration_t tNextAttack5;
	time_point_t tWorldTime5;
	duration_t tDelta5;

	duration_t tNextAttackMen;
	time_point_t tWorldTimeMen;
	duration_t tDeltaMen;

	EventDispatcher<void(CBasePlayer* attacker, float&)> m_eventAdjustDamage;
	EventDispatcher<void(CBasePlayer* attacker2, float&)> m_eventAdjustDamage2;
	EventDispatcher<void(CBasePlayer* attacker3, float&)> m_eventAdjustDamage3;
	EventDispatcher<void(CBasePlayer* attacker4, float&)> m_eventAdjustDamage4;
	EventDispatcher<void(CWood* victim, CBaseEntity* attacker)> m_eventWoodKilled;
	EventDispatcher<void(CMeat* victim, CBaseEntity* attacker2)> m_eventMeatKilled;

	EventDispatcher<void(CZMTank* victim, CBaseEntity* attacker3)> m_eventTankKilled;
	EventDispatcher<void(CZMLight* victim, CBaseEntity* attacker4)> m_eventLightKilled;
	EventDispatcher<void(CTurret* victim, CBaseEntity* attacker5)> m_eventTurretKilled;
	EventDispatcher<void(CZSHGatling* victim, CBaseEntity* attacker6)> m_eventGatlingKilled;

};

 #endif