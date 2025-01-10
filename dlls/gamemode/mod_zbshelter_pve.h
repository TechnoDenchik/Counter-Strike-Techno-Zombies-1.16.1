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
	void ResetTime();
	void ResetTime2();
	void RestartRound() override;
	void CheckRoundTimeExpired();
	void CheckFreezePeriodExpired();
	void ReadMultiplayCvars(CHalfLifeMultiplay* mp);
	BOOL FRoundStarted();
	BOOL NightRound();
	BOOL DayRound();
	void DaySound();

	void UpdateDay();
	void spawn();
	void Reset();
	void UpdateHUD();

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

	CResSpawn* SelectZombieSpawnPoint();
	CBaseEntity* MakeResources();
	CBaseEntity* MakeResources2();
	CBaseEntity* MakeResources3();
	bool start;
	BOOL FInfectionStarted();
public:

	int daytimes;
	int nighttimes;

	int dayseconds;
	int nightseconds;

	int dayminutes;
	int nightminutes;

	bool daytimer;
	bool nighttimer;

	float m_flNextSpawnNPC;
	float m_flNextDay;
	float m_flNextDay2;
	float m_flNextDay3;
	float m_flNextDay4;
	std::vector<CResSpawn*> m_vecZombieSpawns;

	duration_t tNextAttack5;
	time_point_t tWorldTime5;
	duration_t tDelta5;

	duration_t tNextAttack6;
	time_point_t tWorldTime6;
	duration_t tDelta6;

	EventDispatcher<void(CBasePlayer* attacker, float&)> m_eventAdjustDamage;
	EventDispatcher<void(CBasePlayer* attacker2, float&)> m_eventAdjustDamage2;
	EventDispatcher<void(CWood* victim, CBaseEntity* attacker)> m_eventWoodKilled;
	EventDispatcher<void(CMeat* victim, CBaseEntity* attacker)> m_eventMeatKilled;

};

#endif