
#ifndef ENTITY_MONSTER2_H
#define ENTITY_MONSTER2_H
#ifdef _WIN32
#pragma once
#endif

#include "zm_tank.h"
#include "hostage/hostage.h"
#include <memory>
#include <map>
#include <string>
#include <mutex>


class CZMLight;

struct BaseLightExtra
{
public:
	BaseLightExtra(CZMLight* p) : m_pZMLight(p) {}

public:
	CZMLight* const m_pZMLight;
};

class IBaseLightStrategy : public BaseLightExtra
{
public:
	IBaseLightStrategy(CZMLight* p) : BaseLightExtra(p) {}
	virtual ~IBaseLightStrategy() = 0;

	virtual void OnSpawn() = 0;
	virtual void OnThink() = 0;
	virtual void OnKilled(entvars_t* pKiller, int iGib) = 0;

	virtual void DeathSound() const = 0;
	virtual bool IsTeamMate(CBaseEntity* that) const = 0;
};

inline IBaseLightStrategy::~IBaseLightStrategy() {}

class CZMLightModStrategy_Default : public IBaseLightStrategy
{
public:
	CZMLightModStrategy_Default(CZMLight* p) : IBaseLightStrategy(p) {}

	void OnSpawn() override;
	void OnThink() override;
	void OnKilled(entvars_t* pKiller, int iGib) override;

	void DeathSound() const override;
	bool IsTeamMate(CBaseEntity* that) const override { return false; }
};

class CZMLight : public CHostage
{
public:
	void Spawn() override;
	void Precache() override;
	int ObjectCaps() override { return (CBaseMonster::ObjectCaps() | FCAP_MUST_SPAWN); }
	int Classify() override { return CLASS_PLAYER_ALLY; }
	int TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType) override;
	void BecomeDead(void) override;
	void Killed(entvars_t* pevAttacker, int iGib) override;
	int BloodColor() override { return BLOOD_COLOR_RED; }
	void Touch(CBaseEntity* pOther) override;
	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value) override;
	void EXPORT ZM2Think();
	void SendPositionMsg();
	// RAII support to prevent memory leak.
	CZMLight();
	~CZMLight() override;

public:
	int LookupSequence(const char* label);

public:
	void EXPORT IdleThink();
	void Remove();

	void Wander();

	CBaseEntity* CheckTraceHullAttack(float flDist, int iDamage, int iDmgType);
	bool ShouldAttack(CBaseEntity* target) const;

 	void SetAnimation(TankAnim anim);

	bool CheckTarget();
	bool CheckAttack();
	bool CheckSequence();

protected:
	// pTarget, bCanSee
	std::pair<CBasePlayer*, bool> FindTarget() const;
	CBasePlayer* GetClosestPlayer(bool bVisible) const;
	float GetModifiedDamage(float flDamage, int nHitGroup) const;

public:
	float m_flAttackDist;
	float m_flAttackRate;
	float m_flAttackAnimTime;
	float m_flAttackDamage;
	int m_iKillBonusMoney;
	int m_iKillBonusFrags;
	int m_iKillBonusMoney2;
	int m_iKillBonusFrags2;
	float m_flTimeLastActive;
	float m_flTargetChange;
	int m_iZM2Index;
	std::map<std::string, int> m_mapLookupSequenceCache;
	std::mutex m_mutexSetAnimation;

public:
	std::unique_ptr<IBaseLightStrategy> m_pLightStrategy;

};

#endif

