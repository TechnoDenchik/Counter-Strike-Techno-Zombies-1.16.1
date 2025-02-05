#include "hostage/hostage.h"
#include <map>
#include <mutex>
#include "hostage/hostage.h"
#include <memory>
#include <string>

typedef enum
{
	TURRET_ANIM_NONE = 0,
	TURRET_ANIM_FIRE,
	TURRET_ANIM_SPIN,
	TURRET_ANIM_DEPLOY,
	TURRET_ANIM_RETIRE,
	TURRET_ANIM_DIE,
} TURRET_ANIM;

class CSprite;
class CShelter;
class CBasePlayer;

class CBaseTurret : public CBaseMonster
{
public:
	void Spawn() override;
	void Precache()override;
	void KeyValue(KeyValueData* pkvd) override;
	void TurretUse(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value);

	void TraceAttack(entvars_t* pevAttacker, float flDamage, Vector vecDir, TraceResult* ptr, int bitsDamageType) override;
	int	 TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType) override;
	int	 Classify(void) override;

	int BloodColor(void) { return DONT_BLEED; }
	void GibMonster(void) {}	// UNDONE: Throw turret gibs?

	// Think functions

	void EXPORT ActiveThink(void);
	void EXPORT SearchThink(void);
	void EXPORT AutoSearchThink(void);
	void EXPORT TurretDeath(void);

	virtual void EXPORT SpinDownCall(void) { m_iSpin = 0; }
	virtual void EXPORT SpinUpCall(void) { m_iSpin = 1; }

	// void SpinDown(void);
	// float EXPORT SpinDownCall( void ) { return SpinDown(); }

	// virtual float SpinDown(void) { return 0;}
	// virtual float Retire(void) { return 0;}

	void EXPORT Deploy(void);
	void EXPORT Retire(void);

	void EXPORT Initialize(void);

	virtual void Ping(void);
	virtual void EyeOn(void);
	virtual void EyeOff(void);

	int		Save(CSave& save) override;
	int		Restore(CRestore& restore) override;

	static	TYPEDESCRIPTION m_SaveData[];

	// other functions
	void SetTurretAnim(TURRET_ANIM anim);
	int MoveTurret(void);
	virtual void Shoot(Vector& vecSrc, Vector& vecDirToEnemy) { };

	float m_flMaxSpin;		// Max time to spin the barrel w/o a target
	int m_iSpin;

	CSprite* m_pEyeGlow;
	int		m_eyeBrightness;
	int	m_iDeployHeight;
	int	m_iRetractHeight;
	int m_iMinPitch;

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
};

class CTurret;

struct BaseTurretExtra
{
public:
	BaseTurretExtra(CTurret* p) : m_pZMTurret(p) {}

public:
	CTurret* const m_pZMTurret;
};

class IBaseTurretStrategy : public BaseTurretExtra
{
public:
	IBaseTurretStrategy(CTurret* p) : BaseTurretExtra(p) {}
	virtual ~IBaseTurretStrategy() = 0;

	//virtual void OnSpawn() = 0;
	//virtual void OnThink() = 0;
	//virtual void OnKilled(entvars_t* pKiller, int iGib) = 0;

	//virtual void DeathSound() const = 0;
	//virtual bool IsTeamMate(CBaseEntity* that) const = 0;
};

inline IBaseTurretStrategy::~IBaseTurretStrategy() {}

class CZMTurretModStrategy_Default : public IBaseTurretStrategy
{
public:
	CZMTurretModStrategy_Default(CTurret* p) : IBaseTurretStrategy(p) {}

	//void OnSpawn() override;
//	void OnThink() override;
	//void OnKilled(entvars_t* pKiller, int iGib) override;

	//void DeathSound() const override;
	//bool IsTeamMate(CBaseEntity* that) const override { return false; }
};

class CTurret : public CBaseTurret
{
public:
	void Spawn() override;
	void Precache() override;
	// Think functions
	void SpinUpCall(void);
	void SpinDownCall(void);

	int		Save(CSave& save) override;
	int		Restore(CRestore& restore) override;

	static	TYPEDESCRIPTION m_SaveData[];

	// other functions
	void Shoot(Vector& vecSrc, Vector& vecDirToEnemy);
	std::unique_ptr<IBaseTurretStrategy> m_pTurretStrategy;
private:
	int m_iStartSpin;

	
};