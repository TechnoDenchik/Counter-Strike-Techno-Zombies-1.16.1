
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "monsters.h"
#include "trains.h"

#include "zm_tank.h"
#include "../dlls/gamemode/zbs/zs_subs.h"

#include "game.h"
#include "bot_include.h"

#include "zm_tank_manager.h"
#include "gamemode/mods.h"

#include <future>
#include <atomic>

LINK_ENTITY_TO_CLASS(zm_tank, CZMTank);

class CZMTankImprov : public CHostageImprov
{
public:
	CZMTankImprov(CBaseEntity *entity) : CHostageImprov(entity),
		m_bCalculatingPath(false)
	{

	}
	~CZMTankImprov() {} // virtual

	// remove some hostage staffs
	void Crouch() override { /* no code needed */ }
	void OnMoveToFailure(const Vector &goal, MoveToFailureType reason) override { /* no code needed */ }

	// jumping control
	bool GetSimpleGroundHeightWithFloor(const Vector *pos, float *height, Vector *normal) override
	{
		Vector to(*pos);
		to.z -=  9999.9f;

		TraceResult tr;

		UTIL_TraceLine(*pos, to, ignore_monsters, dont_ignore_glass, NULL, &tr);

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
	bool CanJump() const override
	{
		CBasePlayer *player = NULL;
		if (IsFollowing())
		{
			player = (CBasePlayer *)GetFollowLeader();
			if ((m_hostage->Center() - player->Center()).Length() < 48.0)
				return false;
		}

		return CHostageImprov::CanJump();
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
		CNavArea *area = TheNavAreaGrid.GetNavArea(&m_hostage->pev->origin);

		if (area != NULL)
		{
			m_lastKnownArea = area;
		}

		if (IsJumping() && CanJump())
		{
			Vector dir;
			constexpr auto pushSpeed = 100.0f;

			if (!m_hasJumped)
			{
				m_hasJumped = true;
				m_hasJumpedIntoAir = false;
				m_hostage->pev->velocity.z += 300.0f;
			}
			else
				ResetJump();

			dir = m_jumpTarget - GetFeet();
			dir.z = 0;

			dir.NormalizeInPlace();

			m_hostage->pev->velocity.x = dir.x * pushSpeed;
			m_hostage->pev->velocity.y = dir.y * pushSpeed;

			m_hostage->SetBoneController(0, 0.0f);
			m_hostage->SetBoneController(1, 0.0f);

			FaceTowards(m_jumpTarget, deltaT);
			return;
		}

		if (m_isLookingAt)
		{
			Vector const angles = UTIL_VecToAngles(m_viewGoal - GetEyes());
			float pitch = angles.x - m_hostage->pev->angles.x;
			float yaw = angles.y - m_hostage->pev->angles.y;

			while (yaw > 180.0f)
				yaw -= 360.0f;

			while (yaw < -180.0f)
				yaw += 360.0f;

			while (pitch > 180.0f)
				pitch -= 360.0f;

			while (pitch < -180.0f)
				pitch += 360.0f;

			m_hostage->SetBoneController(0, yaw);
			m_hostage->SetBoneController(1, -pitch);

			if (IsAtMoveGoal() && !HasFaceTo())
			{
				if (yaw < -45.0f || yaw > 45.0f)
				{
					FaceTowards(m_viewGoal, deltaT);
				}
			}
		}
		else
		{
			m_hostage->SetBoneController(0, 0.0f);
			m_hostage->SetBoneController(1, 0.0f);
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

			if (m_follower.IsStuck())
			{
				Wiggle();
			}
		}

		constexpr auto friction = 3.0f;

		m_vel.x += m_vel.x * -friction * deltaT;
		m_vel.y += m_vel.y * -friction * deltaT;

		float speed = m_vel.NormalizeInPlace();

		//const float maxSpeed = 285.0f;
		const float maxSpeed = m_hostage->pev->maxspeed;
		if (speed > maxSpeed)
		{
			speed = maxSpeed;
		}

		m_vel.x = m_vel.x * speed;
		m_vel.y = m_vel.y * speed;

		KeepPersonalSpace spacer(this);
		ForEachPlayer(spacer);

		if (g_pHostages != NULL)
		{
			g_pHostages->ForEachHostage(spacer);
		}

		m_hostage->pev->velocity.x = m_vel.x;
		m_hostage->pev->velocity.y = m_vel.y;

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
				UTIL_TraceLine(posBehind, posBehind - Vector(0, 0, 9999), ignore_monsters, dont_ignore_glass, m_hostage->pev->pContainingEntity, &result);

				if (result.flFraction < 1.0f && DotProduct(result.vecPlaneNormal, normal) < 1.0f)
				{
					isSeam = true;
				}
				else
				{
					Vector const posAhead = GetEyes() + alongFloor * checkSeamRange;
					UTIL_TraceLine(posAhead, posAhead - Vector(0, 0, 9999), ignore_monsters, dont_ignore_glass, m_hostage->pev->pContainingEntity, &result);

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

void CZMTank::Spawn()
{
	Precache();

	if (pev->classname)
	{
		RemoveEntityHashValue(pev, STRING(pev->classname), CLASSNAME);
	}

	MAKE_STRING_CLASS("zm_tank", pev);
	AddEntityHashValue(pev, STRING(pev->classname), CLASSNAME);

	pev->movetype = MOVETYPE_STEP;
	pev->solid = SOLID_SLIDEBOX;
	pev->takedamage = DAMAGE_YES;
	pev->flags |= FL_MONSTER;
	pev->deadflag = DEAD_NO;
	pev->max_health = 100;
	pev->health = pev->max_health;
	pev->gravity = 1;
	pev->view_ofs = VEC_VIEW;
	pev->velocity = Vector(0, 0, 0);
	pev->maxspeed = 160.0f;

	if (pev->spawnflags & SF_MONSTER_HITMONSTERCLIP)
		pev->flags |= FL_MONSTERCLIP;

	if (pev->skin < 0)
		pev->skin = 0;

	SET_MODEL(edict(), "models/player/zombi_origin/zombi_origin.mdl");
	SetAnimation(MONSTERANIM_IDLE);

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

	SetBoneController(0, UTIL_VecToYaw(gpGlobals->v_forward));
	SetBoneController(1, 0);
	SetBoneController(2, 0);
	SetBoneController(3, 0);
	SetBoneController(4, 0);

	DROP_TO_FLOOR(edict());

	SetThink(&CZMTank::IdleThink);
 	SetThink(&CZMTank::ZMThink);
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

	m_pTankStrategy->OnSpawn();
}

void CZMTank::Precache()
{
	PRECACHE_SOUND("zbs/zbs_death_1.wav");
	PRECACHE_SOUND("zb3/zombi_death_2.wav");
	PRECACHE_SOUND("zb3/zombi_hurt_01.wav");
	PRECACHE_SOUND("zb3/zombi_hurt_02.wav");
	PRECACHE_SOUND("zb3/zombi_attack_1.wav");
	PRECACHE_SOUND("zb3/zombi_attack_2.wav");
	PRECACHE_SOUND("zb3/zombi_attack_3.wav");
	PRECACHE_MODEL("models/player/zombi_origin/zombi_origin.mdl");
}

void CZMTank::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{ 
	// skip CHostage::Use
	return CBaseMonster::Use(pActivator, pCaller, useType, value); 
}

void CZMTank::Touch(CBaseEntity *pOther)
{
	if (m_improv != NULL)
	{
		m_improv->OnTouch(pOther);
	}
}

float CZMTank::GetModifiedDamage(float flDamage, int nHitGroup) const
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

int CZMTank::TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType)
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

	if (CBaseEntity::Instance(pevAttacker)->IsPlayer()) {
		int hitBits = 0;
		if (m_LastHitGroup == HITGROUP_HEAD) hitBits |= (1 << 0);
		if (bitsDamageType & DMG_CRITICAL) hitBits |= (1 << 1);
		if (bitsDamageType & DMG_BACKATK) hitBits |= (1 << 2);
		if (bitsDamageType & DMG_BURN) hitBits |= (1 << 3);
		MESSAGE_BEGIN(MSG_ONE, gmsgHitMsg, NULL, pevAttacker);
		WRITE_LONG((long)flDamage);
		WRITE_SHORT(ENTINDEX(edict()));
		WRITE_BYTE(hitBits);
		WRITE_BYTE(0);
		MESSAGE_END();
	}
	if (pev->health > 0)
	{
		// if attacking then dont play flinch anim
		if (m_flNextAttack <= gpGlobals->time)
		{
			m_flFlinchTime = gpGlobals->time + 0.25f;
			//SetFlinchActivity();
			SetAnimation(MONSTERANIM_FLINCH);
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

void CZMTank::BecomeDead(void)
{ 
	pev->health = 0;
	pev->movetype = MOVETYPE_TOSS;
	pev->flags &= ~FL_ONGROUND;

	pev->takedamage = DAMAGE_NO;
	pev->deadflag = DEAD_DEAD;
	pev->solid = SOLID_NOT;
}

void CZMTank::Killed(entvars_t *pevAttacker, int iGib)
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
	SetAnimation(MONSTERANIM_DIE);

	pev->nextthink = gpGlobals->time + 3;
	SetThink(&CZMTank::Remove);

	m_flTimeLastActive = -1;

	m_pTankStrategy->OnKilled(pevAttacker, iGib);
}

void CZMTank::Remove()
{
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_NOT;
	pev->takedamage = DAMAGE_NO;

	UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0));
	pev->nextthink = -1;
	m_flNextFullThink = -1;

	SUB_Remove();
}

CZMTank::CZMTank() : CHostage(),
	m_iKillBonusMoney(500), 
	m_iKillBonusFrags(1), 
	m_flTimeLastActive(0.0f), 
	m_pTankStrategy(new CZMTankModStrategy_Default(this))
{
	TankManager().OnEntityAdd(this);
}

CZMTank::~CZMTank()
{
	TankManager().OnEntityRemove(this);
}

void CZMTank::IdleThink()
{
	constexpr float upkeepRate = 0.03f;
	constexpr float giveUpTime = (1 / 30.0f);
	constexpr float updateRate = 0.1f;

	if (!m_improv)
	{
		m_improv = new CZMTankImprov(this);
	}

	pev->nextthink = gpGlobals->time + giveUpTime;

	// Outer...
	const float flInterval = StudioFrameAdvance(0);
	std::future<void> handleDispatchAnimEvents = std::async(&CZMTank::DispatchAnimEvents, this, flInterval);
	std::future<void> handleUpkeepImprov = std::async(&CHostageImprov::OnUpkeep, m_improv, upkeepRate);
	// wait for them...
	handleDispatchAnimEvents.get();
	handleUpkeepImprov.get();

	if (gpGlobals->time > m_flNextFullThink)
	{
		m_flNextFullThink = gpGlobals->time + 0.1;
		std::future<bool> results[] = {
			std::async(&CZMTank::CheckTarget, this),
			std::async(&CZMTank::CheckAttack, this),
			std::async(&CZMTank::CheckSequence, this),
			std::async([=] {return m_improv->OnUpdate(updateRate), false; })
		};

		bool bActive = false;
		for (auto &&f : results)
			bActive |= f.get();
		if (bActive)
			m_flTimeLastActive = gpGlobals->time;
	}

	m_pTankStrategy->OnThink();
}

void CZMTank::ZMThink()
{
	if (pev->deadflag != DEAD_DEAD && !(pev->effects & EF_NODRAW))
	{
		if (m_flNextRadarTime <= gpGlobals->time)
		{
			SendPositionMsg();
			m_flNextRadarTime = gpGlobals->time + 1;
		}
	}
}

void CZMTank::SendPositionMsg()
{
	CBaseEntity* pEntity = NULL;

	while ((pEntity = UTIL_FindEntityByClassname(pEntity, "player")) != NULL)
	{
		if (FNullEnt(pEntity->edict()))
			break;

		if (!pEntity->IsPlayer())
			continue;

		if (pEntity->pev->flags == FL_DORMANT)
			continue;

		CBasePlayer* pTempPlayer = static_cast<CBasePlayer*>(pEntity);

		if (pTempPlayer->pev->deadflag == DEAD_NO && pTempPlayer->m_iTeam == CT)
		{
			if (pev->effects & EF_NODRAW)
			{
				MESSAGE_BEGIN(MSG_ONE, gmsgZSHMsgZmPosK, NULL, pTempPlayer->pev);
				WRITE_BYTE(m_iZMIndex);
				MESSAGE_END();

			}
			else
			{
				MESSAGE_BEGIN(MSG_ONE, gmsgZSHMsgZmPos, NULL, pTempPlayer->pev);
				WRITE_BYTE(0);
				WRITE_BYTE(m_iZMIndex);
				WRITE_COORD(pev->origin.x);
				WRITE_COORD(pev->origin.y);
				WRITE_COORD(pev->origin.z);
				MESSAGE_END();
			}

		}
	}
}

bool CZMTank::CheckTarget()
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
			m_improv->MoveTo(player->Center());
		}

		m_flTargetChange = gpGlobals->time + RANDOM_FLOAT(10.0f, 20.0f);
		return result.second;
	}

	if (m_hTargetEnt && !m_hTargetEnt->IsAlive())
	{
		m_flTargetChange = gpGlobals->time + 0.2f;
		return false;
	}
	return false;
}

std::pair<CBasePlayer *, bool> CZMTank::FindTarget() const
{
	std::future<CBasePlayer *> fpNear = std::async(&CZMTank::GetClosestPlayer, this, true);
	std::future<CBasePlayer *> fpFar = std::async(&CZMTankImprov::GetClosestPlayerByTravelDistance, m_improv, UNASSIGNED, nullptr);

	CBasePlayer *player = nullptr;

	if ((player = fpNear.get()))
	{
		// abandon fpFar...
		return { player, true };
	}
	else if ((player = fpFar.get()))
	{
		return { player, false };
	}

	return { nullptr, false};
}

bool CZMTank::CheckAttack()
{
	if (m_flNextAttack > gpGlobals->time)
		return false;

	CBaseEntity *const pHit = CheckTraceHullAttack(m_flAttackDist, m_flAttackDamage, DMG_BULLET);

	if (!pHit)
		return false;

	SetAnimation(MONSTERANIM_ATTACK);

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

bool CZMTank::CheckSequence()
{
	// sequence settings.
	if (gpGlobals->time >= m_flFlinchTime)
	{
		if (pev->velocity.Length() > 15)
		{
			SetAnimation(MONSTERANIM_WALK);
		}
		else
		{
			SetAnimation(MONSTERANIM_IDLE);
		}
	}
	return false;
}

void CZMTank::Wander()
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

void CZMTank::SetAnimation(TankAnim anim) // similar to CBasePlayer::SetAnimation
{
	int animDesired = 0;
	float speed;
	char szAnim[64];

	if (!pev->modelindex)
		return;

	if (anim != MONSTERANIM_FLINCH && anim != MONSTERANIM_LARGE_FLINCH && m_flFlinchTime > gpGlobals->time && pev->health > 0.0f)
		return;

	// may be called from both CheckAttack and CheckSequence
	std::lock_guard<std::mutex> lock(m_mutexSetAnimation);

	speed = pev->velocity.Length2D();

	if (pev->flags & FL_FROZEN)
	{
		speed = 0;
		anim = MONSTERANIM_IDLE;
	}

	switch (anim)
	{
	case MONSTERANIM_JUMP:
	{
		if (m_Activity == ACT_SWIM || m_Activity == ACT_DIESIMPLE || m_Activity == ACT_HOVER)
			m_IdealActivity = m_Activity;
		else
			m_IdealActivity = ACT_HOP;
		break;
	}
	case MONSTERANIM_DIE:
	{
		m_IdealActivity = ACT_DIESIMPLE;
		m_pTankStrategy->DeathSound();
		break;
	}
	case MONSTERANIM_ATTACK:
	{
		if (m_Activity == ACT_SWIM || m_Activity == ACT_DIESIMPLE || m_Activity == ACT_HOVER)
			m_IdealActivity = m_Activity;
		else
			m_IdealActivity = ACT_RANGE_ATTACK1;
		break;
	}
	case MONSTERANIM_IDLE:
		if (pev->flags & FL_ONGROUND || (m_Activity != ACT_HOP && m_Activity != ACT_LEAP))
		{
			if (pev->waterlevel <= 1)
				m_IdealActivity = ACT_IDLE;

			else if (speed == 0.0f)
				m_IdealActivity = ACT_HOVER;

			else
				m_IdealActivity = ACT_SWIM;
		}
		else
			m_IdealActivity = m_Activity;
		break;
	case MONSTERANIM_WALK:
	{
		if (pev->flags & FL_ONGROUND || (m_Activity != ACT_HOP && m_Activity != ACT_LEAP))
		{
			if (pev->waterlevel <= 1)
				m_IdealActivity = ACT_WALK;

			else if (speed == 0.0f)
				m_IdealActivity = ACT_HOVER;

			else
				m_IdealActivity = ACT_SWIM;
		}
		else
			m_IdealActivity = m_Activity;
		break;
	}
	case MONSTERANIM_FLINCH:
		m_IdealActivity = ACT_FLINCH;
		break;
	case MONSTERANIM_LARGE_FLINCH:
		m_IdealActivity = ACT_LARGE_FLINCH;
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
	case ACT_RANGE_ATTACK2:
	{
		if (speed < 15.0f)
			Q_strcpy(szAnim, "ref_shoot_knife");
		else if (speed < 135.0f)
			Q_strcpy(szAnim, "zbs_attack_walk");
		else
			Q_strcpy(szAnim, "zbs_attack2_run");

		animDesired = LookupSequence(szAnim);
		if (animDesired == -1)
			animDesired = 0;

		pev->sequence = animDesired;
		pev->frame = 0;

		ResetSequenceInfo();
		m_Activity = m_IdealActivity;
		break;
	}
	case ACT_IDLE:
	case ACT_RUN:
	case ACT_WALK:
	{
		if ((m_Activity != ACT_RANGE_ATTACK1 || m_fSequenceFinished)
			&& (m_Activity != ACT_RANGE_ATTACK2 || m_fSequenceFinished)
			&& (m_Activity != ACT_FLINCH || m_fSequenceFinished)
			&& (m_Activity != ACT_LARGE_FLINCH || m_fSequenceFinished)
			&& (m_Activity != ACT_RELOAD || m_fSequenceFinished))
		{
			if (speed < 15.0f)
				Q_strcpy(szAnim, "zbs_idle1");
			else if (speed < 135.0f)
				Q_strcpy(szAnim, "zbs_walk");
			else
				Q_strcpy(szAnim, "zbs_run");

			animDesired = LookupSequence(szAnim);
			if (animDesired == -1)
				animDesired = 0;

			m_Activity = m_IdealActivity;
		}
		else
			animDesired = pev->sequence;
		break;
	}
	case ACT_FLINCH:
	case ACT_LARGE_FLINCH:
	{
		m_Activity = m_IdealActivity;

		switch (m_LastHitGroup)
		{
		case HITGROUP_GENERIC:
		{
			if (RANDOM_LONG(0, 1))
				animDesired = LookupSequence("head_flinch");
			else
				animDesired = LookupSequence("gut_flinch");
			break;
		}
		case HITGROUP_HEAD:
		case HITGROUP_CHEST:
			animDesired = LookupSequence("head_flinch");
			break;
		case HITGROUP_SHIELD:
			animDesired = 0;
			break;
		default:
			animDesired = LookupSequence("gut_flinch");
			break;
		}

		if (animDesired == -1)
			animDesired = 0;

		break;
	}
	case ACT_DIESIMPLE:
	{
		if (m_Activity == m_IdealActivity)
			return;

		m_Activity = m_IdealActivity;

		switch (m_LastHitGroup)
		{
		case HITGROUP_GENERIC:
		{
			switch (RANDOM_LONG(0, 8))
			{
			case 0:
				animDesired = LookupActivity(ACT_DIE_HEADSHOT);
				break;
			case 1:
				animDesired = LookupActivity(ACT_DIE_GUTSHOT);
				break;
			case 2:
				animDesired = LookupActivity(ACT_DIE_BACKSHOT);
				break;
			case 3:
				animDesired = LookupActivity(ACT_DIESIMPLE);
				break;
			case 4:
				animDesired = LookupActivity(ACT_DIEBACKWARD);
				break;
			case 5:
				animDesired = LookupActivity(ACT_DIEFORWARD);
				break;
			case 6:
				animDesired = LookupActivity(ACT_DIE_CHESTSHOT);
				break;
			case 7:
				animDesired = LookupActivity(ACT_DIE_GUTSHOT);
				break;
			case 8:
				animDesired = LookupActivity(ACT_DIE_HEADSHOT);
				break;
			default:
				break;
			}
			break;
		}
		case HITGROUP_HEAD:
			animDesired = LookupActivity(ACT_DIE_HEADSHOT);
			break;
		case HITGROUP_CHEST:
			animDesired = LookupActivity(ACT_DIE_CHESTSHOT);
			break;
		case HITGROUP_STOMACH:
			animDesired = LookupActivity(ACT_DIE_GUTSHOT);
			break;
		case HITGROUP_LEFTARM:
			animDesired = LookupSequence("left");
			break;
		case HITGROUP_RIGHTARM:
			animDesired = LookupSequence("right");
			break;
		default:
			animDesired = LookupActivity(ACT_DIESIMPLE);
			break;
		}

		if (animDesired == -1)
			animDesired = 0;

		break;
	}
	default:
	{
		if (m_Activity == m_IdealActivity)
			return;

		m_Activity = m_IdealActivity;
		animDesired = LookupActivity(m_IdealActivity);

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

int CZMTank::LookupSequence(const char *label)
{
	// only called from SetAnimation, no need to lock...

	auto iter = m_mapLookupSequenceCache.find(label);
	if(iter == m_mapLookupSequenceCache.end())
	{
		iter = m_mapLookupSequenceCache.emplace(label, CBaseAnimating::LookupSequence(label)).first;
	}
	return iter->second;
}

CBaseEntity * CZMTank::CheckTraceHullAttack(float flDist, int iDamage, int iDmgType)
{
	TraceResult tr;

	if (IsPlayer())
		UTIL_MakeVectors(pev->angles);
	else
		UTIL_MakeAimVectors(pev->angles);

	Vector vecStart = pev->origin;
	Vector vecEnd = vecStart + (gpGlobals->v_forward * flDist);

	UTIL_TraceHull(vecStart, vecEnd, dont_ignore_monsters, head_hull, ENT(pev), &tr);

	if (tr.pHit)
	{
		CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);

		if (!ShouldAttack(pEntity))
			return nullptr;

		if (iDamage > 0)
		{
			pEntity->TakeDamage(pev, pev, iDamage, iDmgType);
		}

		return pEntity;
	}

	return NULL;
}

bool CZMTank::ShouldAttack(CBaseEntity *target) const
{
	if (target->pev->takedamage == DAMAGE_NO)
		return false;

	if (target->IsPlayer() && !m_pTankStrategy->IsTeamMate(target))
		return true;

	CZBSBreak*zbs_break = dynamic_cast<CZBSBreak*>(target);
	if (zbs_break)
		return zbs_break->m_flZombiDamageRatio > 0.0f;

	return false;
}

CBasePlayer * CZMTank::GetClosestPlayer(bool bVisible) const
{
	if (!m_improv)
		return NULL;

	CBasePlayer *close = NULL;
	float closeRangeSq = 1e8f;

	for (int i = 1; i <= gpGlobals->maxClients; ++i)
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
	}

	return close;
}


void CZMTankModStrategy_Default::OnSpawn()
{
	
}

void CZMTankModStrategy_Default::OnThink()
{
	
}

void CZMTankModStrategy_Default::OnKilled(entvars_t *pKiller, int iGib)
{

}

void CZMTankModStrategy_Default::DeathSound() const
{
	EMIT_SOUND(ENT(m_pZMTank->pev), CHAN_VOICE, "zbs/zbs_death_1.wav", VOL_NORM, ATTN_NORM);
}