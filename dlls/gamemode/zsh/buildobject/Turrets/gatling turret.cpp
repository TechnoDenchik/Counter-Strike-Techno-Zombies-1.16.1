
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "monsters.h"
#include "trains.h"

#include "gatling turret.h"
#include "../dlls/gamemode/zbs/zs_subs.h"
#include "../dlls/gamemode/zsh/NPC/Zombie/zm_tank.h"
#include "../dlls/gamemode/zsh/NPC/Zombie/zm_light.h"
#include "game.h"
#include "bot_include.h"

#include "gatling turret_manager.h"
#include "gamemode/mods.h"

#include <future>
#include <atomic>

LINK_ENTITY_TO_CLASS(zsh_gatling, CZSHGatling);

#define TURRET_SHOTS	2
#define TURRET_RANGE	(100 * 12)
#define TURRET_SPREAD	Vector( 0, 0, 0 )
#define TURRET_TURNRATE	30		//angles per 0.1 second
#define TURRET_MAXWAIT	15		// seconds turret will stay active w/o a target
#define TURRET_MAXSPIN	5		// seconds turret barrel will spin w/o a target
#define TURRET_MACHINE_VOLUME	0.5

class CZMGatlingImprov : public CHostageImprov
{
public:
	CZMGatlingImprov(CBaseEntity *entity) : CHostageImprov(entity),
		m_bCalculatingPath(false)
	{

	}
	~CZMGatlingImprov() {} // virtual

	// remove some hostage staffs
	void Crouch() override { /* no code needed */ }
	void OnMoveToFailure(const Vector &goal, MoveToFailureType reason) override { /* no code needed */ }

	// jumping control
	bool GetSimpleGroundHeightWithFloor(const Vector *pos, float *height, Vector *normal) override
	{
		Vector to(*pos);
		to.z -=  9999.9f;

		TraceResult tr;

		UTIL_TraceLine(*pos, to, dont_ignore_monsters, dont_ignore_glass, NULL, &tr);

		if (tr.fStartSolid)
			return false;

		*height = tr.vecEndPos.z;

		if (normal != NULL)
		{
			*normal = tr.vecPlaneNormal;
		}

		UTIL_MakeVectors(m_hostage->pev->angles);

		// no need to jump down !
		if (DotProduct2D(tr.vecPlaneNormal, gpGlobals->v_forward) > 0)
			return false;

		if (m_lastKnownArea != NULL && m_lastKnownArea->IsOverlapping(pos))
			*height = Q_max((*height), m_lastKnownArea->GetZ(pos));

		return true;
	}

	// No animation update needed...
	void OnUpdate(float deltaT) override
	{
		if (!IsAlive())
			return;

		UpdateVision();

		m_behavior.Update();

		m_actualVel.x = m_hostage->pev->origin.x - m_lastPosition.x;
		m_actualVel.y = m_hostage->pev->origin.y - m_lastPosition.y;

		const float safeTime = 0.4f;

		if (!m_collisionTimer.HasStarted() || m_collisionTimer.IsGreaterThen(safeTime))
			SetKnownGoodPosition(m_lastPosition);

		m_lastPosition = m_hostage->pev->origin;
	}

	void OnUpkeep(float deltaT) override
	{
		if (IsAlive())
		{
			UpdatePosition(deltaT);
		}
	}

	void UpdatePosition(float deltaT)
	{


		CNavArea* area = TheNavAreaGrid.GetNavArea(&m_hostage->pev->origin);

		if (area != NULL)
		{
			m_lastKnownArea = area;
		}

		if (HasFaceTo() && FaceTowards(m_faceGoal, deltaT))
			ClearFaceTo();

		if (!IsAtMoveGoal() || m_path.GetSegmentCount() > 0)
		{
			if (m_path.GetSegmentCount() <= 0)
			{
				if (m_bCalculatingPath == false)
				{
					m_bCalculatingPath = true;
					m_fbCalcPathResult = std::async([&] {
						HostagePathCost pathCost;
						bool const result = m_path.Compute(&GetFeet(), &m_moveGoal, pathCost);
						m_bCalculatingPath = false;
						return result;
					});
				}

				if(m_fbCalcPathResult.valid() && m_fbCalcPathResult.get())
				{
					m_follower.SetPath(&m_path);
					m_follower.SetImprov(this);

					m_follower.Reset();
					m_follower.Debug(cv_hostage_debug.value > 0.0);
				}
			}

			m_follower.Update(deltaT, m_inhibitObstacleAvoidance.IsElapsed());

			if (m_moveType == Stopped)
			{
				m_follower.ResetStuck();
			}
		}

		constexpr auto friction = 3.0f;

		//float speed = m_vel.NormalizeInPlace();

		//const float maxSpeed = 285.0f;


		m_moveFlags = 0;

	}

	void OnTouch(CBaseEntity *other) override
	{
		m_collisionTimer.Start();

		if (FClassnameIs(other->pev, "worldspawn"))
		{
			constexpr auto lookAheadRange = 30.0f;
			float ground;
			Vector normal = Vector(0, 0, 1);
			Vector alongFloor;
			TraceResult result;
			bool isStep = false;

			UTIL_MakeVectors(m_hostage->pev->angles);

			if (!GetSimpleGroundHeightWithFloor(&GetEyes(), &ground, &normal))
				return;

			if (cv_hostage_debug.value < 0.0)
			{
				UTIL_DrawBeamPoints(GetFeet() + normal * 50, GetFeet(), 2, 255, 255, 0);
			}

			alongFloor = CrossProduct(normal, gpGlobals->v_right);

			Vector const pos = alongFloor * lookAheadRange;

			for (double offset = 1.0f; offset <= 18.0f; offset += 3.0f)
			{
				Vector vecStart = GetFeet();
				vecStart.z += offset;

				UTIL_TraceLine(vecStart, vecStart + pos, ignore_monsters, dont_ignore_glass, m_hostage->pev->pContainingEntity, &result);

				if (result.flFraction < 1.0f && result.vecPlaneNormal[2] < 0.7f)
				{
					isStep = true;
					break;
				}
			}

			if (isStep)
			{
				float stepAheadGround = pos.z;
				Vector stepAheadNormal = Vector(0, 0, stepAheadGround);

				m_inhibitObstacleAvoidance.Start(0.5);

				for (float range = 1.0f; range <= 30.5f; range += 5.0f)
				{
					Vector stepAhead = GetFeet() + alongFloor * range;
					stepAhead.z = GetEyes().z;

					if (GetSimpleGroundHeightWithFloor(&stepAhead, &stepAheadGround, &stepAheadNormal))
					{
						float const dz = stepAheadGround - GetFeet().z;

						if (dz > 0.0f && dz < 18.0f)
						{
							m_hostage->pev->origin.z = stepAheadGround + 3.0f;
							break;
						}
					}
				}
			}
			else if (!IsMoving() && !IsUsingLadder())
			{
				bool isSeam = false;
				constexpr auto checkSeamRange = 50.0f;
				Vector posBehind;

				posBehind = GetEyes() - alongFloor * checkSeamRange;
				UTIL_TraceLine(posBehind, posBehind - Vector(0, 0, 9999), dont_ignore_monsters, dont_ignore_glass, m_hostage->pev->pContainingEntity, &result);

				if (result.flFraction < 1.0f && DotProduct(result.vecPlaneNormal, normal) < 1.0f)
				{
					isSeam = true;
				}
				else
				{
					Vector const posAhead = GetEyes() + alongFloor * checkSeamRange;
					UTIL_TraceLine(posAhead, posAhead - Vector(0, 0, 9999), dont_ignore_monsters, dont_ignore_glass, m_hostage->pev->pContainingEntity, &result);

					if (result.flFraction < 1.0f && DotProduct(result.vecPlaneNormal, normal) < 1.0f)
						isSeam = true;
				}

				if (isSeam)
				{
					if (cv_hostage_debug.value != 0.0)
					{
						CONSOLE_ECHO("Hostage stuck on seam.\n");
					}

					constexpr auto nudge = 3.0f;
					m_hostage->pev->origin.z += nudge;
				}
			}
		}
		else if (FClassnameIs(other->pev, STRING(m_hostage->pev->classname)))
		{
			constexpr auto pushForce = 10.0f;
			Vector2D to = (m_hostage->pev->origin - other->pev->origin).Make2D();
			to.NormalizeInPlace();

			m_vel.x += to.x * pushForce;
			m_vel.y += to.y * pushForce;
		}
	}

	std::atomic<bool> m_bCalculatingPath;
	std::future<bool> m_fbCalcPathResult;
};

void CZSHGatling::Spawn()
{
	Precache();

	if (pev->classname)
	{
		RemoveEntityHashValue(pev, STRING(pev->classname), CLASSNAME);
	}

	MAKE_STRING_CLASS("zsh_gatling", pev);
	AddEntityHashValue(pev, STRING(pev->classname), CLASSNAME);


	pev->nextthink = gpGlobals->time + 1;
	pev->movetype = MOVETYPE_STEP;
	pev->sequence = 0;
	pev->frame = 0;
	pev->solid = SOLID_SLIDEBOX;
	pev->takedamage = DAMAGE_YES;

	SetBits(pev->flags, FL_MONSTER);

	pev->max_health = 100;
	pev->health = pev->max_health;

	pev->velocity = Vector(0, 0, 0);
	pev->maxspeed = 0;

	ResetSequenceInfo();
	SetBoneController(0, 0);
	SetBoneController(1, 0);
	m_flFieldOfView = VIEW_FIELD_FULL;

	if (pev->spawnflags & SF_MONSTER_HITMONSTERCLIP)
		pev->flags |= FL_MONSTERCLIP;

	if (pev->skin < 0)
		pev->skin = 0;

	SET_MODEL(edict(), "models/shelter/turret_bg.mdl");
	SetAnimation(GATLINGANIM_IDLE);

	m_flNextChange = 0;
	m_State = STAND;
	m_hTargetEnt = NULL;
	m_hStoppedTargetEnt = NULL;
	m_vPathToFollow[0] = Vector(0, 0, 0);
	m_flFlinchTime = 0;
	m_bRescueMe = FALSE;

	UTIL_SetSize(pev, VEC_HULL_MIN, VEC_HULL_MAX);

	TraceResult tr;
	TRACE_MONSTER_HULL(edict(), pev->origin, pev->origin, dont_ignore_monsters, edict(), &tr);

	if (tr.fStartSolid || tr.fAllSolid || !tr.fInOpen)
	{
		Killed(nullptr, GIB_NORMAL);
		return;
	}

	UTIL_MakeVectors(pev->v_angle);

	DROP_TO_FLOOR(edict());

	SetThink(&CZSHGatling::IdleThink);
	pev->nextthink = gpGlobals->time + RANDOM_FLOAT(0.1, 0.2);

	m_flNextFullThink = gpGlobals->time + RANDOM_FLOAT(0.1, 0.2);
	m_vStart = pev->origin;
	m_vStartAngles = pev->angles;
	m_vOldPos = Vector(9999, 9999, 9999);
	m_iHostageIndex = ++g_iHostageNumber;

	nTargetNode = -1;
	m_fHasPath = FALSE;

	m_flLastPathCheck = -1;
	m_flPathAcquired = -1;
	m_flPathCheckInterval = 3.0f;
	m_flNextRadarTime = gpGlobals->time + RANDOM_FLOAT(0, 1);

	m_LocalNav = new CLocalNav(this);
	m_bStuck = FALSE;
	m_flStuckTime = 0;

	if (m_improv)
		delete m_improv;
	m_improv = NULL;

	m_flNextAttack = 0;

	pev->team = TEAM_TERRORIST; // allow bot attack...
	m_flAttackDist = 35.0f;
	m_flAttackDamage = 2;
	m_flAttackRate = 2.0f;
	m_flAttackAnimTime = 0.6f;
	m_flTimeLastActive = gpGlobals->time;

	m_pGatlingStrategy->OnSpawn();
}

void CZSHGatling::Initialize(void)
{
	m_iOn = 1;
	m_fBeserk = 0;
	m_iSpin = 0;

	SetBoneController(0, 0);
	SetBoneController(1, 0);

	if (m_iBaseTurnRate == 0) m_iBaseTurnRate = TURRET_TURNRATE;
	if (m_flMaxWait == 0) m_flMaxWait = TURRET_MAXWAIT;
	m_flStartYaw = pev->angles.y;
	if (m_iOrientation == 1)
	{
		pev->idealpitch = 180;
		pev->angles.x = 180;
		pev->view_ofs.z = -pev->view_ofs.z;
		pev->effects |= EF_INVLIGHT;
		pev->angles.y = pev->angles.y + 180;
		if (pev->angles.y > 360)
			pev->angles.y = pev->angles.y - 360;
	}

	m_vecGoalAngles.x = 0;

	if (m_iAutoStart)
	{
		m_flLastSight = gpGlobals->time + m_flMaxWait;
		SetThink(&CZSHGatling::AutoSearchThink);
		pev->nextthink = gpGlobals->time + .1;
	}
	else
		SetThink(&CZSHGatling::SUB_DoNothing);
}

void CZSHGatling::Precache()
{
	PRECACHE_SOUND("zsh/crash.wav");
	PRECACHE_SOUND("zsh/turret-1.wav");
	PRECACHE_MODEL("models/shelter/turret_bg.mdl");
}

void CZSHGatling::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{ 
	// skip CHostage::Use
	return CBaseMonster::Use(pActivator, pCaller, useType, value); 
}

void CZSHGatling::Touch(CBaseEntity *pOther)
{
	if (m_improv != NULL)
	{
		m_improv->OnTouch(pOther);
	}
}

void CZSHGatling::AutoSearchThink(void)
{
	// ensure rethink
	StudioFrameAdvance();
	pev->nextthink = gpGlobals->time + 0.3;

	// If we have a target and we're still healthy

	if (m_hTargetEnt != NULL)
	{
		if (!m_hTargetEnt->IsAlive())
			m_hTargetEnt = NULL;// Dead enemy forces a search for new one
	}

	// Acquire Target

	if (m_hTargetEnt = NULL)
	{
		Look(TURRET_RANGE);
		m_hTargetEnt = BestVisibleEnemy();
	}

	if (m_hTargetEnt != NULL)
	{
		SetThink(&CZSHGatling::Deploy);
		EMIT_SOUND(ENT(pev), CHAN_BODY, "turret/tu_alert.wav", TURRET_MACHINE_VOLUME, ATTN_NORM);
	}
}

void CZSHGatling::Deploy(void)
{
	pev->nextthink = gpGlobals->time + 0.1;
	StudioFrameAdvance();

	if (pev->sequence != GATLINGANIM_DEPLOY)
	{
		m_iOn = 1;
	
		EMIT_SOUND(ENT(pev), CHAN_BODY, "turret/tu_deploy.wav", TURRET_MACHINE_VOLUME, ATTN_NORM);
		SUB_UseTargets(this, USE_ON, 0);
	}

	if (m_fSequenceFinished)
	{
		pev->maxs.z = m_iDeployHeight;
		pev->mins.z = -m_iDeployHeight;
		UTIL_SetSize(pev, pev->mins, pev->maxs);

		m_vecCurAngles.x = 0;

		if (m_iOrientation == 1)
		{
			m_vecCurAngles.y = UTIL_AngleMod(pev->angles.y + 180);
		}
		else
		{
			m_vecCurAngles.y = UTIL_AngleMod(pev->angles.y);
		}

		
		pev->framerate = 0;
	//	SetThink(&CZSHGatling::SearchThink);
	}

	m_flLastSight = gpGlobals->time + m_flMaxWait;
}

int CZSHGatling::MoveTurret(void)
{
	int state = 0;
	// any x movement?

	if (m_vecCurAngles.x != m_vecGoalAngles.x)
	{
		float flDir = m_vecGoalAngles.x > m_vecCurAngles.x ? 1 : -1;

		m_vecCurAngles.x += 0.1 * m_fTurnRate * flDir;

		// if we started below the goal, and now we're past, peg to goal
		if (flDir == 1)
		{
			if (m_vecCurAngles.x > m_vecGoalAngles.x)
				m_vecCurAngles.x = m_vecGoalAngles.x;
		}
		else
		{
			if (m_vecCurAngles.x < m_vecGoalAngles.x)
				m_vecCurAngles.x = m_vecGoalAngles.x;
		}

		if (m_iOrientation == 0)
			SetBoneController(1, -m_vecCurAngles.x);
		else
			SetBoneController(1, m_vecCurAngles.x);
		state = 1;
	}

	if (m_vecCurAngles.y != m_vecGoalAngles.y)
	{
		float flDir = m_vecGoalAngles.y > m_vecCurAngles.y ? 1 : -1;
		float flDist = fabs(m_vecGoalAngles.y - m_vecCurAngles.y);

		if (flDist > 180)
		{
			flDist = 360 - flDist;
			flDir = -flDir;
		}
		if (flDist > 30)
		{
			if (m_fTurnRate < m_iBaseTurnRate * 10)
			{
				m_fTurnRate += m_iBaseTurnRate;
			}
		}
		else if (m_fTurnRate > 45)
		{
			m_fTurnRate -= m_iBaseTurnRate;
		}
		else
		{
			m_fTurnRate += m_iBaseTurnRate;
		}

		m_vecCurAngles.y += 0.1 * m_fTurnRate * flDir;

		if (m_vecCurAngles.y < 0)
			m_vecCurAngles.y += 360;
		else if (m_vecCurAngles.y >= 360)
			m_vecCurAngles.y -= 360;

		if (flDist < (0.05 * m_iBaseTurnRate))
			m_vecCurAngles.y = m_vecGoalAngles.y;

		//ALERT(at_console, "%.2f -> %.2f\n", m_vecCurAngles.y, y);
		if (m_iOrientation == 0)
			SetBoneController(0, m_vecCurAngles.y - pev->angles.y);
		else
			SetBoneController(0, pev->angles.y - 180 - m_vecCurAngles.y);
		state = 1;
	}

	if (!state)
		m_fTurnRate = m_iBaseTurnRate;

	//ALERT(at_console, "(%.2f, %.2f)->(%.2f, %.2f)\n", m_vecCurAngles.x, 
	//	m_vecCurAngles.y, m_vecGoalAngles.x, m_vecGoalAngles.y);
	return state;
}

float CZSHGatling::GetModifiedDamage(float flDamage, int nHitGroup) const
{
	switch (nHitGroup)
	{
		case HITGROUP_GENERIC: flDamage *= 1; break;
		case HITGROUP_HEAD: flDamage *= 4; break;
		case HITGROUP_CHEST: flDamage *= 1; break;
		case HITGROUP_STOMACH: flDamage *= 1.25; break;
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM: flDamage *= 1; break;
		case HITGROUP_LEFTLEG:
		case HITGROUP_RIGHTLEG: flDamage *= 0.75; break;
		default: flDamage *= 1; break;
	}

	return flDamage;
}

int CZSHGatling::TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType)
{
	float flActualDamage;
	CBasePlayer *pAttacker = NULL;

	flActualDamage = GetModifiedDamage(flDamage, m_LastHitGroup);

	if (pevAttacker != NULL)
	{
		CBaseEntity *pAttackingEnt = GetClassPtr<CBaseEntity>(pevAttacker);

		if (pAttackingEnt->Classify() == CLASS_VEHICLE)
		{
			CBaseEntity *pDriver = ((CFuncVehicle *)pAttackingEnt)->m_pDriver;

			if (pDriver != NULL)
			{
				pevAttacker = pDriver->pev;
			}
		}

		if (pAttackingEnt->IsPlayer())
		{
			pAttacker = GetClassPtr<CBasePlayer>(pevAttacker);
		}
	}

	flActualDamage = g_pModRunning->GetAdjustedEntityDamage(this, pevInflictor, pevAttacker, flActualDamage, bitsDamageType);

	

	if (flActualDamage > pev->health)
		flActualDamage = pev->health;

	
	pev->health -= flActualDamage;

	if (m_improv != NULL)
	{
		m_improv->OnInjury(flActualDamage);
	}

	PlayPainSound();
	m_flTimeLastActive = gpGlobals->time;

	MESSAGE_BEGIN(MSG_ONE, gmsgHitMsg, NULL, pevAttacker);
		WRITE_LONG((long)flDamage);
		WRITE_SHORT(ENTINDEX(edict()));
	 	WRITE_BYTE(0);
		MESSAGE_END();
	if (pev->health > 0)
	{
		// if attacking then dont play flinch anim
		if (m_flNextAttack <= gpGlobals->time)
		{
			m_flFlinchTime = gpGlobals->time + 0.25f;
			//SetFlinchActivity();
			//SetAnimation(MONSTERANIM_FLINCH);
		}

		if (pAttacker != NULL)
		{
			// Player damaged monster
			return 1;
		}
	}
	else
	{
		Killed(pevAttacker, GIB_NORMAL);
	}

	
	return 0;
}

void CZSHGatling::BecomeDead(void)
{ 
	pev->health = 0;
	pev->movetype = MOVETYPE_TOSS;
	pev->flags &= ~FL_ONGROUND;

	pev->takedamage = DAMAGE_NO;
	pev->deadflag = DEAD_DEAD;
	pev->solid = SOLID_NOT;
}

void CZSHGatling::Killed(entvars_t *pevAttacker, int iGib)
{
	if (!IsAlive())
		return;
	// unsigned int cCount = 0;
	// BOOL fDone = FALSE;

	// clear the deceased's sound channels.(may have been firing or reloading when killed)
	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "common/null.wav", VOL_NORM, ATTN_NORM);
	m_IdealMonsterState = MONSTERSTATE_DEAD;
	// Make sure this condition is fired too (TakeDamage breaks out before this happens on death)
	SetConditions(bits_COND_LIGHT_DAMAGE);

	// tell owner ( if any ) that we're dead.This is mostly for MonsterMaker functionality.
	CBaseEntity *pOwner = CBaseEntity::Instance(pev->owner);
	if (pOwner)
	{
		pOwner->DeathNotice(pev);
	}

	SetTouch(NULL);
	BecomeDead();

	//pev->enemy = ENT(pevAttacker);//why? (sjb)
	m_IdealMonsterState = MONSTERSTATE_DEAD;

	//SetDeathActivity();
	SetAnimation(GATLINGANIM_DIE);

	pev->nextthink = gpGlobals->time + 3;
	SetThink(&CZSHGatling::Remove);

	m_flTimeLastActive = -1;

	m_pGatlingStrategy->OnKilled(pevAttacker, iGib);
}

void CZSHGatling::Remove()
{
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_NOT;
	pev->takedamage = DAMAGE_NO;

	UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0));
	pev->nextthink = -1;
	m_flNextFullThink = -1;

	SUB_Remove();
}

CZSHGatling::CZSHGatling() : CHostage(),
	m_iKillBonusMoney(500), 
	m_iKillBonusFrags(1), 
	m_flTimeLastActive(0.0f), 
	m_pGatlingStrategy(new CZMGatlingModStrategy_Default(this))
{
	GatlingManager().OnEntityAdd(this);
}

CZSHGatling::~CZSHGatling()
{
	GatlingManager().OnEntityRemove(this);
}

void CZSHGatling::IdleThink()
{
	constexpr float upkeepRate = 0.03f;
	constexpr float giveUpTime = (1 / 30.0f);
	constexpr float updateRate = 0.1f;
	MoveTurret();
	if (!m_improv)
	{
		m_improv = new CZMGatlingImprov(this);
	}

	pev->nextthink = gpGlobals->time + giveUpTime;

	// Outer...
	const float flInterval = StudioFrameAdvance(0);
	std::future<void> handleDispatchAnimEvents = std::async(&CZSHGatling::DispatchAnimEvents, this, flInterval);
	std::future<void> handleUpkeepImprov = std::async(&CHostageImprov::OnUpkeep, m_improv, upkeepRate);
	// wait for them...
	handleDispatchAnimEvents.get();
	handleUpkeepImprov.get();

	if (gpGlobals->time > m_flNextFullThink)
	{
		m_flNextFullThink = gpGlobals->time + 0.1;
		std::future<bool> results[] = {
			std::async(&CZSHGatling::CheckTarget, this),
			std::async(&CZSHGatling::CheckAttack, this),
			std::async(&CZSHGatling::CheckSequence, this),
			std::async([=] {return m_improv->OnUpdate(updateRate), false; })
		};

		bool bActive = false;
		for (auto &&f : results)
			bActive |= f.get();
		if (bActive)
			m_flTimeLastActive = gpGlobals->time;
	}

	m_pGatlingStrategy->OnThink();
}

void CZSHGatling::SpinUpCall(void)
{
	StudioFrameAdvance();
	pev->nextthink = gpGlobals->time + 0.1;

	// Are we already spun up? If not start the two stage process.
	if (!m_iSpin)
	{
		SetAnimation(GATLINGANIM_SPIN);
		// for the first pass, spin up the the barrel
		if (!m_iStartSpin)
		{
			pev->nextthink = gpGlobals->time + 1.0; // spinup delay
			EMIT_SOUND(ENT(pev), CHAN_BODY, "turret/tu_spinup.wav", TURRET_MACHINE_VOLUME, ATTN_NORM);
			m_iStartSpin = 1;
			pev->framerate = 0.1;
		}
		// after the barrel is spun up, turn on the hum
		else if (pev->framerate >= 1.0)
		{
			pev->nextthink = gpGlobals->time + 0.1; // retarget delay
			EMIT_SOUND(ENT(pev), CHAN_STATIC, "turret/tu_active2.wav", TURRET_MACHINE_VOLUME, ATTN_NORM);
			SetThink(&CZSHGatling::IdleThink);
			m_iStartSpin = 0;
			m_iSpin = 1;
		}
		else
		{
			pev->framerate += 0.075;
		}
	}
}

void CZSHGatling::Shoot(Vector& vecSrc, Vector& vecDirToEnemy)
{
	FireBullets(1, vecSrc, vecDirToEnemy, TURRET_SPREAD, TURRET_RANGE, BULLET_PLAYER_45ACP, 1);
	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "turret/tu_fire1.wav", 1, 0.6);
	pev->effects = pev->effects | EF_MUZZLEFLASH;
}

bool CZSHGatling::CheckTarget()
{
	if (m_flTargetChange <= gpGlobals->time)
	{
		auto result = FindTarget();

		CBasePlayer *player = result.first;

		if(!player)
		{
			Wander();
			m_hTargetEnt = player;
		}
		else if (result.second)
		{
			m_improv->Follow(player);
			m_improv->SetFollowRange(6000.0f, 3000.0f, 20.0f);
			m_hTargetEnt = player;
		}
		else
		{
			m_hTargetEnt = player;
			SetAnimation(GATLINGANIM_FIRE);

			Vector vecSrc, vecAng;
			GetAttachment(0, vecSrc, vecAng);

			Shoot(vecSrc, gpGlobals->v_forward);
			//m_improv->MoveTo(player->Center());
		}

		m_flTargetChange = gpGlobals->time + 1;
		return result.second;
	}

	if (m_hTargetEnt && !m_hTargetEnt->IsAlive())
	{
		m_flTargetChange = gpGlobals->time + 0.2f;
		return false;
	}
	return false;
}

std::pair<CBasePlayer *, bool> CZSHGatling::FindTarget() const
{
	std::future<CBaseEntity *> fpNear = std::async(&CZSHGatling::GetClosestPlayer, this, true);
	std::future<CBasePlayer *> fpFar = std::async(&CZMGatlingImprov::GetClosestPlayerByTravelDistance, m_improv, UNASSIGNED, nullptr);

	CBaseEntity *player = nullptr;

	/*if ((player = fpNear.get()))
	{
		// abandon fpFar...
		return { player, true };
	}
	else if ((player = fpFar.get()))
	{
		return { player, false };
	}*/

	return { nullptr, false};
}

bool CZSHGatling::CheckAttack()
{
	if (m_flNextAttack > gpGlobals->time)
		return false;

	CBaseEntity *const pHit = CheckTraceHullAttack(m_flAttackDist, m_flAttackDamage, DMG_BULLET);

	if (!pHit)
		return false;

	SetAnimation(GATLINGANIM_SPIN);

	m_flNextAttack = gpGlobals->time + m_flAttackRate;
	m_flNextFullThink = gpGlobals->time + m_flAttackAnimTime;

	switch (RANDOM_LONG(1, 2))
	{
	case 1: EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/bhit_helmet-1.wav", VOL_NORM, ATTN_NORM); break;
	case 2: EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/bhit_kevlar-1.wav", VOL_NORM, ATTN_NORM); break;
	
	default:
		break;
	}

	return true;
}

bool CZSHGatling::CheckSequence()
{
	// sequence settings.
	if (gpGlobals->time >= m_flFlinchTime)
	{
		if (pev->velocity.Length() > 15)
		{
			//SetAnimation(MONSTERANIM_WALK);
		}
		else
		{
			SetAnimation(GATLINGANIM_IDLE);
		}
	}
	return false;
}

void CZSHGatling::Wander()
{
	if (m_improv)
	{
		CBaseEntity *target = NULL;

		float shorestDistance = 9.9999998e10f;
		CBaseEntity *shorestTarget = NULL;

		while ((target = UTIL_FindEntityByClassname(target, "func_buyzone")) != NULL)
		{
			ShortestPathCost cost;
			Vector const vecCenter = target->Center();

			float const range = NavAreaTravelDistance(m_improv->GetLastKnownArea(), TheNavAreaGrid.GetNearestNavArea(&vecCenter), cost);

			if (range < shorestDistance)
			{
				shorestDistance = range;
				shorestTarget = target;
			}
		}

		if (shorestTarget)
		{
			m_improv->MoveTo(shorestTarget->Center());
			m_improv->SetFollowRange(6000.0f, 3000.0f, 50.0f);
			return;
		}
	}
}

void CZSHGatling::SetAnimation(GatlingAnim anim) // similar to CBasePlayer::SetAnimation
{
	int animDesired = 0;
	float speed;
	char szAnim[64];

	if (!pev->modelindex)
		return;

	//if (anim != MONSTERANIM_FLINCH && anim != MONSTERANIM_LARGE_FLINCH && m_flFlinchTime > gpGlobals->time && pev->health > 0.0f)
		//return;

	// may be called from both CheckAttack and CheckSequence
	std::lock_guard<std::mutex> lock(m_mutexSetAnimation);

	speed = pev->velocity.Length2D();

	if (pev->flags & FL_FROZEN)
	{
		speed = 0;
		anim = GATLINGANIM_IDLE;
	}

	switch (anim)
	{
	case GATLINGANIM_DIE:
	{
		//m_IdealActivity = ACT_DIESIMPLE;
		m_pGatlingStrategy->DeathSound();
		break;
	}
	case GATLINGANIM_FIRE:
	{
	//	if (m_Activity == ACT_SWIM || m_Activity == ACT_DIESIMPLE || m_Activity == ACT_HOVER)
		//	m_IdealActivity = m_Activity;
	//	else
		//	m_IdealActivity = ACT_RANGE_ATTACK1;
	//	break;
	}
	case GATLINGANIM_IDLE:
		
		break;

	default:
		break;
	}

	switch (m_IdealActivity)
	{
	case ACT_HOP:
	case ACT_LEAP:
	{
		if (m_Activity == m_IdealActivity)
			return;

		animDesired = LookupSequence("zbs_jump");
		if (animDesired == -1)
			animDesired = 0;

		if (pev->sequence != animDesired || !m_fSequenceLoops)
			pev->frame = 0;

		if (!m_fSequenceLoops)
			pev->effects |= EF_NOINTERP;

		m_Activity = m_IdealActivity;
		break;
	}
	case ACT_RANGE_ATTACK1:
	{
		if (speed == 0.0f)
			Q_strcpy(szAnim, "ref_shoot_knife");
		else if (speed < 135.0f)
			Q_strcpy(szAnim, "zbs_attack_walk");
		else
			Q_strcpy(szAnim, "zbs_attack1_run");

		animDesired = LookupSequence(szAnim);
		if (animDesired == -1)
			animDesired = 0;

		pev->sequence = animDesired;
		pev->frame = 0;

		ResetSequenceInfo();
		m_Activity = m_IdealActivity;
		break;
	}
	}
	
	if (pev->sequence != animDesired)
	{
		pev->sequence = animDesired;
		pev->frame = 0;

		ResetSequenceInfo();
	}

}

int CZSHGatling::LookupSequence(const char *label)
{
	// only called from SetAnimation, no need to lock...

	auto iter = m_mapLookupSequenceCache.find(label);
	if(iter == m_mapLookupSequenceCache.end())
	{
		iter = m_mapLookupSequenceCache.emplace(label, CBaseAnimating::LookupSequence(label)).first;
	}
	return iter->second;
}

CBaseEntity * CZSHGatling::CheckTraceHullAttack(float flDist, int iDamage, int iDmgType)
{
	TraceResult tr;

	if (IsPlayer())
		UTIL_MakeVectors(pev->angles);
	else
		UTIL_MakeAimVectors(pev->angles);

	Vector vecStart = pev->origin;
	Vector vecEnd = vecStart + (gpGlobals->v_forward * flDist);

	UTIL_TraceHull(vecStart, vecEnd, dont_ignore_monsters, head_hull, ENT(pev), &tr);

	CBasePlayer* close = NULL;
	float closeRangeSq = 1e8f;
	CBaseEntity* pEntity = CBaseEntity::Instance(tr.pHit);
	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		CBasePlayer* player = dynamic_cast<CBasePlayer*>(UTIL_PlayerByIndex(i));
		
	}

	if (tr.pHit)
	{
		CBaseMonster *pEntity = CBaseMonster::GetMonsterPointer(tr.pHit);

		if (!ShouldAttack(pEntity))
			return nullptr;

		if (iDamage > 0)
		{
			pEntity->TakeDamage(pev, pev, iDamage, iDmgType);
			SetAnimation(GATLINGANIM_FIRE);

			Vector vecSrc, vecAng;
			GetAttachment(0, vecSrc, vecAng);

			Shoot(vecSrc, gpGlobals->v_forward);
		}

		return pEntity;
	}

	return NULL;
}

bool CZSHGatling::ShouldAttack(CBaseEntity *target) const
{
	if (m_pGatlingStrategy->IsTeamMate(target))
		return true;

	return false;
}

CBaseEntity * CZSHGatling::GetClosestPlayer(bool bVisible) const
{
	int hostages_ = 0;
	CBaseEntity* zombie = NULL;

	while ((zombie = UTIL_FindEntityByClassname(zombie, "zm_light")) != NULL)
	{
		if (zombie->pev->takedamage != DAMAGE_YES)
			continue;

		CZMLight* hostage = static_cast<CZMLight*>(zombie);

		if (!hostage->IsFollowingSomeone())
			continue;

		if (hostage->IsValid() && hostage->m_target == zombie)
			++hostages_;
	}

	/*for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		CBasePlayer *player = dynamic_cast<CBasePlayer *>(UTIL_PlayerByIndex(i));

		if (player == NULL)
			continue;

		if (FNullEnt(player->pev))
			continue;

		if (FStrEq(STRING(player->pev->netname), ""))
			continue;

		if (!player->IsAlive())
			continue;

		if (bVisible && !m_improv->IsVisible(m_improv->GetEyes(), true))
			continue;

		float const rangeSq = (m_improv->GetCentroid() - player->pev->origin).LengthSquared();

		if (rangeSq < closeRangeSq)
		{
			closeRangeSq = rangeSq;
			close = player;
		}
	}*/

	return zombie;
}


void CZMGatlingModStrategy_Default::OnSpawn()
{
	
}

void CZMGatlingModStrategy_Default::OnThink()
{
	
}

void CZMGatlingModStrategy_Default::OnKilled(entvars_t *pKiller, int iGib)
{

}

void CZMGatlingModStrategy_Default::DeathSound() const
{
	EMIT_SOUND(ENT(m_pZSHGatling->pev), CHAN_VOICE, "zsh/crash.wav", VOL_NORM, ATTN_NORM);
}