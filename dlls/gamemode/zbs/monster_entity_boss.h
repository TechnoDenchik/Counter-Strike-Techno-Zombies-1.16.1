
#ifndef ENTITY_MONSTER3_H
#define ENTITY_MONSTER3_H
#ifdef _WIN32
#pragma once
#endif

#include "hostage/hostage.h"
#include "gamemode/zbs/monster_entity.h"
#include <memory>
#include <map>
#include <string>
#include <mutex>


class CMonsterBoss;

struct BaseMonsterExtra3
{
public:
	BaseMonsterExtra3(CMonsterBoss* p) : m_pMonster(p) {}

public:
	CMonsterBoss* const m_pMonster;
};

class IBaseMonsterStrategy3 : public BaseMonsterExtra3
{
public:
	IBaseMonsterStrategy3(CMonsterBoss* p) : BaseMonsterExtra3(p) {}
	virtual ~IBaseMonsterStrategy3() = 0;

	virtual void OnSpawn() = 0;
	virtual void OnThink() = 0;
	virtual void OnKilled(entvars_t* pKiller, int iGib) = 0;

	virtual void DeathSound() const = 0;
	virtual bool IsTeamMate(CBaseEntity* that) const = 0;
};

inline IBaseMonsterStrategy3::~IBaseMonsterStrategy3() {}

class CMonsterBossStrategy : public IBaseMonsterStrategy3
{
public:
	CMonsterBossStrategy(CMonsterBoss* p) : IBaseMonsterStrategy3(p) {}

	void OnSpawn() override;
	void OnThink() override;
	void OnKilled(entvars_t* pKiller, int iGib) override;

	void DeathSound() const override;
	bool IsTeamMate(CBaseEntity* that) const override { return false; }
};

class CMonsterBoss : public CHostage
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

	// RAII support to prevent memory leak.
	CMonsterBoss();
	~CMonsterBoss() override;

public:
	int LookupSequence(const char* label);

public:
	void EXPORT IdleThink();
	void Remove();

	void Wander();

	CBaseEntity* CheckTraceHullAttack(float flDist, int iDamage, int iDmgType);
	bool ShouldAttack(CBaseEntity* target) const;

	void SetAnimation(MonsterAnim anim);

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

	std::map<std::string, int> m_mapLookupSequenceCache;
	std::mutex m_mutexSetAnimation;

public:
	std::unique_ptr<IBaseMonsterStrategy3> m_pMonsterStrategy3;

};

#endif

