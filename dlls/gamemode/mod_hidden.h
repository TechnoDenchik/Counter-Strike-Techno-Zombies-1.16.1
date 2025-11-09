
#ifndef MOD_HIDDEN_H
#define MOD_HIDDEN_H
#ifdef _WIN32
#pragma once
#endif

#include "mod_base.h"
#include "hidden/hidden_class.h"

#include <vector>
#include <utility>

#include "EventDispatcher.h"

class CSupplyBoxHidden;
class CSupSpawnHidden;

class CMod_Hidden : public TBaseMod_RemoveObjects<TBaseMod_RandomSpawn<>>
{
public:
	CMod_Hidden();
	BOOL IsTeamplay(void) override { return TRUE; }
	void CheckMapConditions() override;
	void UpdateGameMode(CBasePlayer *pPlayer) override;
	//void RestartRound() override;
	void PlayerSpawn(CBasePlayer* pPlayer) override;
	void Think() override;
	BOOL ClientConnected(edict_t* pEntity, const char* pszName, const char* pszAddress, char* szRejectReason) override;
	void ClientDisconnected(edict_t* pClient) override;
	BOOL FPlayerCanTakeDamage(CBasePlayer* pPlayer, CBaseEntity* pAttacker) override;
	void PlayerKilled(CBasePlayer* pVictim, entvars_t* pKiller, entvars_t* pInflictor) override;
	size_t JokerOriginNum();
	void PickJokerOrigin();
	void TeamCheck();
	void HumanInfectionByJoker(CBasePlayer* player, CBasePlayer* attacker);
	void InfectionSound();

	float m_flNextJokerPickTime = 0;
	bool m_bShouldPickNewJoker = false;
	CBasePlayer* m_pLastKilledJoker = nullptr;
	CBasePlayer* m_pCurrentJoker = nullptr; // Текущий активный Joker
	void PickNewJokerAfterDeath();
	void MakeRandomPlayerJoker();
	CBasePlayer* GetCurrentJoker() const { return m_pCurrentJoker; }

	void MakeSupplyboxThink();
	void RemoveAllSupplybox();
	CSupplyBoxHidden* CreateSupplybox();
	CSupSpawnHidden* SelectSupplySpawnPoint();
	int SupplyboxCount();

public:
	std::vector<CSupSpawnHidden*> m_vecSupplySpawns;
	DamageTrack_e DamageTrack() override { return DT_NONE; }
	void InstallPlayerModStrategy(CBasePlayer* player) override;

	void MakeJoker(CBasePlayer* player)
	{
		m_eventBecomeJoker.dispatch(player);
	}
	void KillJoker()
	{
		m_eventKilledJoker.dispatch();
	}

	float m_flTimeNextMakeSupplybox;

	bool deathjoker;
	bool IsJokerAlive() const { return m_pCurrentJoker && m_pCurrentJoker->IsAlive() && m_pCurrentJoker->m_bIsZombie; }
public:
	EventDispatcher<void(CBasePlayer* who)> m_eventBecomeJoker;
	EventDispatcher<void()> m_eventKilledJoker;
};

class CPlayerModStrategy_Hidden : public CPlayerModStrategy_Zombie
{
public:
	CPlayerModStrategy_Hidden(CBasePlayer* player, CMod_Hidden* mp);
	void CheckBuyZone() override { m_pPlayer->m_signals.Signal(SIGNAL_BUY); };
	bool CanPlayerBuy(bool display) override;
	int ComputeMaxAmmo(const char* szAmmoClassName, int iOriginalMax) override;

	void OnSpawn() override;
	void OnThink() override;
	void OnResetMaxSpeed() override { m_pCharacter->ResetMaxSpeed(); return CPlayerModStrategy_Zombie::OnResetMaxSpeed(); }
	bool ApplyKnockback(CBasePlayer* attacker, const KnockbackData& data) override { return m_pCharacter->ApplyKnockback(attacker, data); }
	float AdjustDamageTaken(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType) override;
	void Pain(int m_LastHitGroup, bool HasArmour) override;
	void DeathSound() override;
	void OnKilled(entvars_t* pKiller, entvars_t* pInflictor) override;

private:
	void Event_OnBecomeJoker(CBasePlayer* who);
	void Event_OnKillJoker();
	const EventListener m_eventBecomeJokerListener;
	const EventListener m_eventKilledJokerListener;

public:
	

	void BecomeJoker();
	void BecomeHuman();

	float m_flDeadTime;
	float m_flDeadJoker;
	bool jokerdead;
	CMod_Hidden m_ModHidden;
	std::shared_ptr<IJokerModeCharacter> m_pCharacter;
};

void PlayerJoker_Precache();
#endif
