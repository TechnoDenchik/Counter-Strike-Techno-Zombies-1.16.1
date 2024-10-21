#ifndef MOD_ZBS_H
#define MOD_ZBS_H
#ifdef _WIN32
#pragma once
#endif

#include "mod_base.h"
#include <vector>
#include "EventDispatcher.h"

class CZombieSpawn;
class CMonster;
class CMonster2;
class CMonsterBoss;

class CMod_ZombieScenario : public TBaseMod_RemoveObjects<>
{
public:
	CMod_ZombieScenario();
public:
	BOOL IsTeamplay(void) override { return TRUE; }
	void UpdateGameMode(CBasePlayer *pPlayer) override;
	void RestartRound() override;
	void PlayerSpawn(CBasePlayer *pPlayer) override;
	void Think() override;
	void CheckWinConditions() override;
	void CheckMapConditions() override;
	bool boss_clear;
public:
	DamageTrack_e DamageTrack() override { return DT_ZBS; }
	void InstallPlayerModStrategy(CBasePlayer *player) override;
	float GetAdjustedEntityDamage(CBaseEntity *victim, entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType) override;

public:
	void TeamCheck();
	void WaitingSound();
	void RoundStart();
	void DeimosSpawn();
	void HumanWin();
	void ZombieWin();
	BOOL FRoundStarted();

	CZombieSpawn *SelectZombieSpawnPoint();
	CBaseEntity *MakeZombieNPC();
	CBaseEntity* MakeZombieNPC2();
	CBaseEntity* MakeZombieNPC3();
	CBaseEntity* MakeZombieNPC4();
	CBaseEntity* MakeZombieNPC5();
	CBaseEntity* MakeZombieNPC6();
	CBaseEntity* MakeZombieNPC7();
	CBaseEntity* MakeZombieNPC8();
	CBaseEntity* MakeZombieNPC9();
	CBaseEntity* MakeZombieNPC10();
	CBaseEntity* MakeZombieNPC11();
	CBaseEntity* MakeZombieBoss();
	void ClearZombieNPC();

public:
	std::vector<CZombieSpawn *> m_vecZombieSpawns;
	float m_flNextSpawnNPC;

	EventDispatcher<void(CBasePlayer *attacker, float &)> m_eventAdjustDamage;
	EventDispatcher<void(CBasePlayer *attacker, float &)> m_eventAdjustDamage2;
	EventDispatcher<void(CMonster *victim, CBaseEntity *attacker)> m_eventMonsterKilled;
	EventDispatcher<void(CMonster2* victim, CBaseEntity* attacker)> m_eventMonsterKilled2;
	EventDispatcher<void(CMonsterBoss* victim, CBaseEntity* attacker)> m_eventMonsterKilled3;
};

#endif
