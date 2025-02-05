
#ifndef MONSTER_ENTITY_H
#define MONSTER_ENTITY_H
#ifdef _WIN32
#pragma once
#endif

#include "hostage/hostage.h"
#include <memory>
#include <map>
#include <string>
#include <mutex>

enum TankAnim
{
	MONSTERANIM_IDLE,
	MONSTERANIM_WALK,
	MONSTERANIM_JUMP,
	MONSTERANIM_DIE,
	
	MONSTERANIM_FLINCH,
	MONSTERANIM_LARGE_FLINCH,
	MONSTERANIM_ATTACK,
	MONSTERANIM_SKILL,
};

class CZMTank;

struct BaseTankExtra
{
public:
	BaseTankExtra(CZMTank*p) : m_pZMTank(p) {}

public:
	CZMTank* const m_pZMTank;
};

class IBaseTankStrategy : public BaseTankExtra
{
public:
	IBaseTankStrategy(CZMTank*p) : BaseTankExtra(p) {}
	virtual ~IBaseTankStrategy() = 0;

	virtual void OnSpawn() = 0;
	virtual void OnThink() = 0;
	virtual void OnKilled(entvars_t *pKiller, int iGib) = 0;

	virtual void DeathSound() const = 0;
	virtual bool IsTeamMate(CBaseEntity *that) const = 0;
};

inline IBaseTankStrategy::~IBaseTankStrategy() {}

class CZMTankModStrategy_Default : public IBaseTankStrategy
{
public:
	CZMTankModStrategy_Default(CZMTank*p) : IBaseTankStrategy(p) {}

	void OnSpawn() override;
	void OnThink() override;
	void OnKilled(entvars_t *pKiller, int iGib) override;

	void DeathSound() const override;
	bool IsTeamMate(CBaseEntity *that) const override { return false; }
};

class CZMTank : public CHostage
{
public:
	friend class CShelter;
	void Spawn() override;
	void Precache() override;
	int ObjectCaps() override { return (CBaseMonster::ObjectCaps() | FCAP_MUST_SPAWN); }
	int Classify() override { return CLASS_PLAYER_ALLY; }
	int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType) override;
	void BecomeDead(void) override;
	void Killed(entvars_t *pevAttacker, int iGib) override;
	int BloodColor() override { return BLOOD_COLOR_RED; }
	void Touch(CBaseEntity *pOther) override;
	void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value) override;
	void EXPORT ZMThink();
	void SendPositionMsg();
	// RAII support to prevent memory leak.
	CZMTank();
	~CZMTank() override;

public:
	int LookupSequence(const char *label);
	EHANDLE m_hTankEnt;
public:
	void EXPORT IdleThink();
	void Remove();

	void Wander();
	
	CBaseEntity *CheckTraceHullAttack(float flDist, int iDamage, int iDmgType);
	bool ShouldAttack(CBaseEntity *target) const;
	bool ShouldAttack2(CBaseEntity* target) const;

	void SetAnimation(TankAnim anim);

	bool CheckTarget();
	bool CheckAttack();
	bool CheckSequence();


protected:
	// pTarget, bCanSee
	std::pair<CBasePlayer *, bool> FindTarget() const;
	CBasePlayer *GetClosestPlayer(bool bVisible) const;
	float GetModifiedDamage(float flDamage, int nHitGroup) const;

public:
	float m_flAttackDist;
	float m_flAttackRate;
	float m_flAttackAnimTime;
	float m_flAttackDamage;
	int m_iKillBonusMoney;
	int m_iKillBonusFrags;
	float m_flTimeLastActive;
	float m_flTargetChange;
	int m_iZMIndex;
	std::map<std::string, int> m_mapLookupSequenceCache;
	std::mutex m_mutexSetAnimation;

public:
	std::unique_ptr<IBaseTankStrategy> m_pTankStrategy;

};

#endif
