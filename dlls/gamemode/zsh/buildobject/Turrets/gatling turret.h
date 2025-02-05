
#ifndef GATLING_ENTITY_H
#define GATLING_ENTITY_H
#ifdef _WIN32
#pragma once
#endif

#include "hostage/hostage.h"
#include <memory>
#include <map>
#include <string>
#include <mutex>



enum GatlingAnim
{
	GATLINGANIM_IDLE,
	GATLINGANIM_FIRE,
	GATLINGANIM_SPIN,
	GATLINGANIM_DEPLOY,
	
	GATLINGRANIM_RETRIE,
	GATLINGANIM_DIE,
};

class CZSHGatling;
class CSprite;

struct BaseGatlingExtra
{
public:
	BaseGatlingExtra(CZSHGatling*p) : m_pZSHGatling(p) {}

public:
	CZSHGatling* const m_pZSHGatling;
};

class IBaseGatlingStrategy : public BaseGatlingExtra
{
public:
	IBaseGatlingStrategy(CZSHGatling*p) : BaseGatlingExtra(p) {}
	virtual ~IBaseGatlingStrategy() = 0;

	virtual void OnSpawn() = 0;
	virtual void OnThink() = 0;
	virtual void OnKilled(entvars_t *pKiller, int iGib) = 0;

	virtual void DeathSound() const = 0;
	virtual bool IsTeamMate(CBaseEntity *that) const = 0;
};

inline IBaseGatlingStrategy::~IBaseGatlingStrategy() {}

class CZMGatlingModStrategy_Default : public IBaseGatlingStrategy
{
public:
	CZMGatlingModStrategy_Default(CZSHGatling* p) : IBaseGatlingStrategy(p) {}

	void OnSpawn() override;
	void OnThink() override;
	void OnKilled(entvars_t *pKiller, int iGib) override;

	void DeathSound() const override;
	bool IsTeamMate(CBaseEntity *that) const override { return false; }
};

class CZSHGatling : public CHostage
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
	int BloodColor() override { return DONT_BLEED; }
	void Touch(CBaseEntity *pOther) override;
	void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value) override;
	void EXPORT Initialize(void);
	void EXPORT AutoSearchThink(void);
	void EXPORT Deploy(void);
	void EXPORT Retire(void);
	void SpinUpCall(void);
	void SpinDownCall(void);
	int MoveTurret(void);
	void Shoot(Vector& vecSrc, Vector& vecDirToEnemy);

	// RAII support to prevent memory leak.
	CZSHGatling();
	~CZSHGatling() override;

	float m_flMaxSpin;		// Max time to spin the barrel w/o a target
	int m_iSpin;

	CSprite* m_pEyeGlow;
	int		m_eyeBrightness;
	int	m_iDeployHeight;
	int	m_iRetractHeight;
	int m_iMinPitch;
	int m_iStartSpin;
	int m_iBaseTurnRate;	// angles per second
	float m_fTurnRate;		// actual turn rate
	int m_iOrientation;		// 0 = floor, 1 = Ceiling
	int	m_iOn;
	int m_fBeserk;			// Sometimes this bitch will just freak out
	int m_iAutoStart;		// true if the turret auto deploys when a target
	// enters its range

	Vector m_vecLastSight;
	float m_flLastSight;	// Last time we saw a target
	float m_flMaxWait;		// Max time to seach w/o a target
	int m_iSearchSpeed;		// Not Used!

	// movement
	float	m_flStartYaw;
	Vector	m_vecCurAngles;
	Vector	m_vecGoalAngles;


	float	m_flPingTime;	// Time until the next ping, used when searching
	float	m_flSpinUpTime;	// Amount of time until the barrel should spin down when searching

public:
	int LookupSequence(const char *label);
	EHANDLE m_hTankEnt;
public:
	void EXPORT IdleThink();
	void Remove();

	void Wander();
	
	CBaseEntity *CheckTraceHullAttack(float flDist, int iDamage, int iDmgType);
	bool ShouldAttack(CBaseEntity *target) const;

	void SetAnimation(GatlingAnim anim);

	bool CheckTarget();
	bool CheckAttack();
	bool CheckSequence();


protected:
	// pTarget, bCanSee
	std::pair<CBasePlayer *, bool> FindTarget() const;
	CBaseEntity *GetClosestPlayer(bool bVisible) const;
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

	std::map<std::string, int> m_mapLookupSequenceCache;
	std::mutex m_mutexSetAnimation;

public:
	std::unique_ptr<IBaseGatlingStrategy> m_pGatlingStrategy;

};

#endif
