/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#ifndef MOD_ZB1R_H
#define MOD_ZB1R_H
#ifdef _WIN32
#pragma once
#endif

#include "../mod_base.h"
#include "gamemode/zb5/zb1reference/zb1_zclassreference.h"

#include <vector>
#include <utility>

#include "EventDispatcher.h"

class CMod_ZombiR : public TBaseMod_RemoveObjects<TBaseMod_RandomSpawn<>>
{
public:
	CMod_ZombiR();

public: // CHalfLifeMultiplay
	void CheckMapConditions() override;
	BOOL IsTeamplay(void) override { return TRUE; }
	void UpdateGameMode(CBasePlayer *pPlayer) override;
	void RestartRound() override;
	void PlayerSpawn(CBasePlayer *pPlayer) override;
	void Think() override;
	BOOL ClientConnected(edict_t *pEntity, const char *pszName, const char *pszAddress, char *szRejectReason) override;
	void ClientDisconnected(edict_t *pClient) override;
	BOOL FPlayerCanTakeDamage(CBasePlayer *pPlayer, CBaseEntity *pAttacker) override;
	void CheckWinConditions() override;
	int IPointsForKill(CBasePlayer *pAttacker, CBasePlayer *pKilled) override;
	void PlayerKilled(CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor) override;
	
public: // IBaseMod
	DamageTrack_e DamageTrack() override { return DT_ZB; }
	void InstallPlayerModStrategy(CBasePlayer *player) override;

protected:
	virtual size_t ZombieOriginNum();
	virtual void PickZombieOrigin();
	virtual void HumanInfectionByZombie(CBasePlayer *player, CBasePlayer *attacker);
	virtual void RoundEndScore(int iWinStatus);
	
	bool iszombiemod1;
protected:
	void TeamCheck();
	void InfectionSound();

	void HumanWin();
	void ZombieWin();

	BOOL FInfectionStarted();

	void MakeZombie(CBasePlayer *player, ZombieLevel iEvolutionLevel) 
	{ 
		m_eventBecomeZombie.dispatch(player, iEvolutionLevel); 
	}

public:
	EventDispatcher<void(CBasePlayer *who, ZombieLevel iEvolutionLevel)> m_eventBecomeZombie;
};

class CPlayerModStrategy_ZB1R : public CPlayerModStrategy_Zombie
{
public:
	CPlayerModStrategy_ZB1R(CBasePlayer *player, CMod_ZombiR *mp);
	void CheckBuyZone() override { m_pPlayer->m_signals.Signal(SIGNAL_BUY); };
	bool CanPlayerBuy(bool display) override;
	int ComputeMaxAmmo(const char *szAmmoClassName, int iOriginalMax) override;

	void OnSpawn() override;
	void OnThink() override { m_pCharacter->Think(); return CPlayerModStrategy_Zombie::OnThink(); }
	void OnResetMaxSpeed() override { m_pCharacter->ResetMaxSpeed(); return CPlayerModStrategy_Zombie::OnResetMaxSpeed(); }
	bool ApplyKnockback(CBasePlayer *attacker, const KnockbackData &data) override { return m_pCharacter->ApplyKnockback(attacker, data); }
	float AdjustDamageTaken(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType) override;
	void Pain(int m_LastHitGroup, bool HasArmour) override;
	void DeathSound() override;

private:
	void Event_OnBecomeZombie(CBasePlayer *who, ZombieLevel iEvolutionLevel);
	const EventListener m_eventBecomeZombieListener;

public:
	virtual void BecomeZombie(ZombieLevel iEvolutionLevel);
	virtual void BecomeHuman();

	std::shared_ptr<IZombieModeCharacterR> m_pCharacter;
};

#endif
