#ifndef MOD_ZSH_H
#define MOD_ZSH_H
#ifdef _WIN32
#pragma once
#endif

#include "mod_base.h"
#include <vector>
#include "EventDispatcher.h"

class CBaseEntity;
class CBasePlayer;

class CResSpawn;
class CWood;
class CMeat;
class CSentry;

class CSpawnres;

typedef struct entvars_s entvars_t;

class CMod_ZombieShelter_coop : public TBaseMod_RemoveObjects<>
{ 
public:
	 CMod_ZombieShelter_coop();
public:
	friend class CShelter;
	friend class ZSHUpdateDay;

	void TeamCheck();
	BOOL IsTeamplay(void) override { return TRUE; }
	void CheckMapConditions() override;
	void UpdateGameMode(CBasePlayer* pPlayer) override;
	void WaitingSound();
	void day();
	void night();
	void PlayerSpawn(CBasePlayer* pPlayer) override;
	void Think() override;
	void RestartRound() override;
	void CheckRoundTimeExpired();
	void CheckFreezePeriodExpired();
	void ReadMultiplayCvars(CHalfLifeMultiplay* mp);
	BOOL FRoundStarted();
	BOOL NightRound();
	BOOL DayRound();
	void DaySound();
	void NightSound();
	void Thinks();
	void Thinks2();
	
	bool HasRoundTimeExpired();
	void InstallPlayerModStrategy(CBasePlayer* player) override;
	BOOL ClientConnected(edict_t* pEntity, const char* pszName, const char* pszAddress, char* szRejectReason) override;
	void ClientDisconnected(edict_t* pClient) override;
	float GetAdjustedEntityDamage(CBaseEntity* victim, entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType) override;
	float GetAdjustedEntityDamage2(CBaseEntity* victim, entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage2, int bitsDamageType2) override;
public:
	float m_flTimeNextMakeSupplybox;
//	void MakeSpawnresThink();
	//void RemoveAllSupplybox();
	//CSpawnres* CreateSpawnres();
	//int SpawnresCount();

	CResSpawn* SelectZombieSpawnPoint();
	CBaseEntity* MakeResources();
	CBaseEntity* MakeResources2();
	CBaseEntity* MakeResources3();
	
	BOOL FInfectionStarted();
public:



	float m_flNextSpawnNPC;
	float m_flNextDay;
	float m_flNextDay2;
	float m_flNextDay3;
	float m_flNextDay4;
	std::vector<CResSpawn*> m_vecZombieSpawns;
	
	EventDispatcher<void(CBasePlayer* attacker, float&)> m_eventAdjustDamage;
	EventDispatcher<void(CBasePlayer* attacker2, float&)> m_eventAdjustDamage2;
	EventDispatcher<void(CWood* victim, CBaseEntity* attacker)> m_eventWoodKilled;
	EventDispatcher<void(CMeat* victim, CBaseEntity* attacker)> m_eventMeatKilled;

	
};

#endif