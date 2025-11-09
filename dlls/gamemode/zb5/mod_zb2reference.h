/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#ifndef MOD_ZB2R_H
#define MOD_ZB2R_H
#ifdef _WIN32
#pragma once
#endif

#include "mod_zb1reference.h"

#include "EventDispatcher.h"

#include "zb2reference/zb2_zclassreference.h"
#include "zb2reference/zb2_skillreference.h"
#include "zb3reference/zb3_heroreference.h"

class CSupplyBoxR;
class CSupSpawnR;

class CMod_ZombieMod2R : public CMod_ZombiR
{
public:
	CMod_ZombieMod2R();

public:
	void UpdateGameMode(CBasePlayer *pPlayer) override;
	void RestartRound() override;
	void Think() override;
	void CheckMapConditions() override;
	void PlayerSpawn(CBasePlayer *pPlayer) override;
	void PlayerThink(CBasePlayer *pPlayer) override;
	BOOL ClientCommand(CBasePlayer *pPlayer, const char *pcmd) override;
	void PickZombieOrigin() override;
public: // IBaseMod
	void InstallPlayerModStrategy(CBasePlayer *player) override;
	float GetAdjustedEntityDamage(CBaseEntity *victim, entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType) override;
	HitBoxGroup GetAdjustedTraceAttackHitgroup(CBaseEntity *victim, entvars_t * pevAttacker, float flDamage, const Vector & vecDir, TraceResult * ptr, int bitsDamageType) override;

	EventDispatcher<void(CBasePlayer* who)> m_eventBecomeHero;
	EventDispatcher<void()> m_eventRoundStart;

protected:
	void PickHero();
	void MakeHero(CBasePlayer* p) { m_eventBecomeHero.dispatch(p); }

	void MakeSupplyboxThink();
	void RemoveAllSupplybox();
	CSupplyBoxR *CreateSupplybox();
	CSupSpawnR* SelectSupplySpawnPoint();
	int SupplyboxCount();

public:
	void HumanInfectionByZombie(CBasePlayer *player, CBasePlayer *attacker) override;

public:
	std::vector<CSupSpawnR*> m_vecSupplySpawns;
	EventDispatcher<void(CBasePlayer *victim, CBasePlayer *attacker)> m_eventInfection;
	EventDispatcher<void(CBasePlayer *attacker, float &)> m_eventAdjustDamage;
	EventDispatcher<void(CBasePlayer *attacker, HitBoxGroup &)> m_eventAdjustHitgroup;

protected:
	float m_flTimeNextMakeSupplybox;
};

class CPlayerModStrategy_ZB2R : public CPlayerModStrategy_ZB1R
{
public:
	CPlayerModStrategy_ZB2R(CBasePlayer *player, CMod_ZombieMod2R *mp);

	bool ClientCommand(const char *pcmd) override;
	bool ClientCommand2(const char* pcmd) override;
	void OnSpawn() override;
	void OnThink() override;
	void Pain(int m_LastHitGroup, bool HasArmour) override;

protected:
	virtual bool CanUseZombieSkill();
	virtual void Zombie_HealthRecoveryThink();
	virtual void UpdatePlayerEvolutionHUD();
	virtual void CheckEvolution();
	virtual void EvolutionSound() const;

protected:
	void BecomeZombie(ZombieLevel iEvolutionLevel) override;
	void BecomeTank(ZombieLevel iEvolutionLevel);
	void BecomeSpeed(ZombieLevel iEvolutionLevel);
	void BecomeHeavy(ZombieLevel iEvolutionLevel);
	void BecomeHeal(ZombieLevel iEvolutionLevel);
	void BecomePsycho(ZombieLevel iEvolutionLevel);
	void BecomeDeimos(ZombieLevel iEvolutionLevel);
	void BecomeGanimed(ZombieLevel iEvolutionLevel);
	void BecomeBanchee(ZombieLevel iEvolutionLevel);
	void BecomeStamper(ZombieLevel iEvolutionLevel);
	void BecomeAksha(ZombieLevel iEvolutionLevel);
	void BecomeBoomer(ZombieLevel iEvolutionLevel);
	void BecomeBooster(ZombieLevel iEvolutionLevel);
	void BecomeChina(ZombieLevel iEvolutionLevel);
	void BecomeFly(ZombieLevel iEvolutionLevel);
	void BecomeResident(ZombieLevel iEvolutionLevel);
	void BecomeMeatwall(ZombieLevel iEvolutionLevel);
	void BecomeDeathknight(ZombieLevel iEvolutionLevel);
	void BecomeSpider(ZombieLevel iEvolutionLevel);

	void BecomeHuman() override;
	virtual void Event_OnInfection(CBasePlayer *victim, CBasePlayer *attacker);
	virtual void Event_AdjustHumanDamage(CBasePlayer *attacker, float &flDamage);
	virtual void Event_AdjustHumanHitgroup(CBasePlayer *attacker, HitBoxGroup &iHitgroup);
	EventListener m_eventInfectionListener;
	EventListener m_eventAdjustDamageListener;
	EventListener m_eventAdjustHitgroupListener;
	const std::vector<EventListener> m_eventListeners;

	virtual void Event_OnBecomeHero(CBasePlayer* who);

	void BecomeHero();

	bool IsHero() const { return m_pPlayer->m_bIsVIP; }

private:
	const EventListener m_eventBecomeHeroListener;
	std::shared_ptr<IHeroModeCharacterR> m_pCharacter_ZB3;

	CMod_ZombieMod2R * const m_pModZB2;

	//std::unique_ptr<IZombieSkill> m_pZombieSkill;
	std::shared_ptr<IZombieModeCharacter_ZB2_ExtraR> m_pCharacter_ZB2;
	float m_flTimeNextZombieHealthRecovery;
	int m_iZombieInfections;
};

#endif
