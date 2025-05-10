/*
mod_zb2.h - CSMoE Gameplay server : Zombie Mod 2
Copyright (C) 2018 Moemod Hyakuya

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#ifndef MOD_ZB2_H
#define MOD_ZB2_H
#ifdef _WIN32
#pragma once
#endif

#include "mod_zb1.h"

#include "EventDispatcher.h"

#include "zb2/zb2_zclass.h"
#include "zb2/zb2_skill.h"
#include "zb3/zb3_hero.h"

class CSupplyBox;
class CSupSpawn;

class CMod_ZombieMod2 : public CMod_Zombi
{
public:
	CMod_ZombieMod2();

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
	CSupplyBox *CreateSupplybox();
	CSupSpawn* SelectSupplySpawnPoint();
	int SupplyboxCount();

public:
	void HumanInfectionByZombie(CBasePlayer *player, CBasePlayer *attacker) override;

public:
	std::vector<CSupSpawn*> m_vecSupplySpawns;
	EventDispatcher<void(CBasePlayer *victim, CBasePlayer *attacker)> m_eventInfection;
	EventDispatcher<void(CBasePlayer *attacker, float &)> m_eventAdjustDamage;
	EventDispatcher<void(CBasePlayer *attacker, HitBoxGroup &)> m_eventAdjustHitgroup;

protected:
	float m_flTimeNextMakeSupplybox;
};

class CPlayerModStrategy_ZB2 : public CPlayerModStrategy_ZB1
{
public:
	CPlayerModStrategy_ZB2(CBasePlayer *player, CMod_ZombieMod2 *mp);

	bool ClientCommand(const char *pcmd) override;
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
	std::shared_ptr<IHeroModeCharacter> m_pCharacter_ZB3;

	CMod_ZombieMod2 * const m_pModZB2;

	//std::unique_ptr<IZombieSkill> m_pZombieSkill;
	std::shared_ptr<IZombieModeCharacter_ZB2_Extra> m_pCharacter_ZB2;
	float m_flTimeNextZombieHealthRecovery;
	int m_iZombieInfections;
};

#endif
