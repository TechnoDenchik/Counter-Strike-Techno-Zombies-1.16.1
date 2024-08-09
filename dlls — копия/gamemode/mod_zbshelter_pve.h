#ifndef MOD_ZSH_H
#define MOD_ZSH_H
#ifdef _WIN32
#pragma once
#endif

#include "mod_base.h"
#include <vector>
#include "EventDispatcher.h"
//#include "mod_zb1.h"

//class CBaseEntity;
//class CBasePlayer;

class CSpawnres;

typedef struct entvars_s entvars_t;

class CMod_ZombieShelter_coop : public TBaseMod_RemoveObjects<>
{ 
public:
	 CMod_ZombieShelter_coop();
public:
	void TeamCheck();
	BOOL IsTeamplay(void) override { return TRUE; }
	void CheckMapConditions() override;
	void UpdateGameMode(CBasePlayer* pPlayer) override;
	void WaitingSound();
	void day();
	void night();
	void PlayerSpawn(CBasePlayer* pPlayer) override;
	void Think() override;
	BOOL FRoundStarted();
	void InstallPlayerModStrategy(CBasePlayer* player) override;
	BOOL ClientConnected(edict_t* pEntity, const char* pszName, const char* pszAddress, char* szRejectReason) override;
	void ClientDisconnected(edict_t* pClient) override;

	//float GetAdjustedEntityDamage(CBaseEntity* victim, entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType) override;
	//HitBoxGroup GetAdjustedTraceAttackHitgroup(CBaseEntity* victim, entvars_t* pevAttacker, float flDamage, const Vector& vecDir, TraceResult* ptr, int bitsDamageType) override;
//	int IPointsForKill(CBasePlayer* pAttacker, CBasePlayer* pKilled) override;

	//EventDispatcher<void(CBasePlayer* attacker, float&)> m_eventAdjustDamage;
	//EventDispatcher<void(CBasePlayer* attacker, float&)> m_eventAdjustDamage2;
	//EventDispatcher<void(ZSHUpdateRes* victim, CBaseEntity* attacker)> m_eventMonsterKilled;
	//EventDispatcher<void(ZSHUpdateRes* victim, CBaseEntity* attacker)> m_eventMonsterKilled2;

	//BOOL FPlayerCanTakeDamage(CBasePlayer* pPlayer, CBaseEntity* pAttacker) override;	
	//void RestartRound() override;
	//void CheckWinConditions() override;
	
	//void PlayerKilled(CBasePlayer* pVictim, entvars_t* pKiller, entvars_t* pInflictor) override;
protected:

	void MakeSpawnresThink();
	void RemoveAllSupplybox();
	CSpawnres* CreateSpawnres();
	int SpawnresCount();
	BOOL FInfectionStarted();

protected:
	float m_flTimeNextMakeSupplybox;
};

#endif