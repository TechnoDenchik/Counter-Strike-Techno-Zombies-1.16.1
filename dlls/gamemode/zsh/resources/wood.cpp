
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "monsters.h"
#include "trains.h"

#include "wood.h"
#include "zsh_subs.h"

#include "game.h"
#include "bot_include.h"

#include "../dlls/gamemode/zsh/zsh_const.h"
#include "wood_manager.h"
#include "gamemode/mods.h"

#include <future>
#include <atomic>

LINK_ENTITY_TO_CLASS(wood_entity, CWood);

class CWoodImprov : public CHostageImprov
{
public:
	CWoodImprov(CBaseEntity *entity) : CHostageImprov(entity),
		m_bCalculatingPath(false){}

	~CWoodImprov() {}

	void Crouch() override { }
	void OnMoveToFailure(const Vector &goal, MoveToFailureType reason) override { }

	void OnUpdate(float deltaT) override
	{
		if (!IsAlive())
			return;

		UpdateVision();
		m_behavior.Update();

		const float safeTime = 0.4f;

		if (!m_collisionTimer.HasStarted() || m_collisionTimer.IsGreaterThen(safeTime))
			SetKnownGoodPosition(m_lastPosition);

		m_lastPosition = m_hostage->pev->origin;
	}

	void OnUpkeep(float deltaT) override{}

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
		}
		if (FClassnameIs(other->pev, STRING(m_hostage->pev->classname)))
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

void CWood::Spawn()
{
	Precache();

	if (pev->classname)
	{
		RemoveEntityHashValue(pev, STRING(pev->classname), CLASSNAME);
	}

	MAKE_STRING_CLASS("wood_entity", pev);
	AddEntityHashValue(pev, STRING(pev->classname), CLASSNAME);

	SET_MODEL(edict(), "models/shelter/item_wood.mdl");

	pev->movetype = MOVETYPE_ANGLECLIP;
	pev->solid = SOLID_SLIDEBOX;
	pev->takedamage = DAMAGE_YES;
	pev->deadflag = DEAD_NO;
	pev->sequence = 0;
	pev->framerate = 1;
	pev->frame = 1;


	pev->max_health = 100;
	pev->health = pev->max_health;
	pev->view_ofs = VEC_VIEW;
	pev->velocity = Vector(6, 6, 18);
	//pev->shelteraxedamage = 0;
	

	if (pev->spawnflags & SF_MONSTER_HITMONSTERCLIP)
		pev->flags = FL_CUSTOMENTITY;

	UTIL_MakeVectors(pev->v_angle);
	m_flNextChange = 0;
	m_hTargetEnt = NULL;
	m_hStoppedTargetEnt = NULL;
	m_bRescueMe = FALSE;

	UTIL_SetSize(pev, Vector(6, 6, 18), Vector(6, 6, 18));

	TraceResult tr;
	TRACE_MONSTER_HULL(edict(), pev->origin, pev->origin, dont_ignore_monsters, edict(), &tr);

	if (tr.fStartSolid || tr.fAllSolid || !tr.fInOpen)
	{
		Killed(nullptr, GIB_NORMAL);
		return;
	}

	DROP_TO_FLOOR(edict());

	SetThink(&CWood::IdleThink);
	pev->nextthink = gpGlobals->time + RANDOM_FLOAT(0.1, 0.2);

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

	pev->team = TEAM_TERRORIST; 

	m_flAttackDist = 0.0f;
	m_flAttackDamage = 0.0f;
	m_flAttackRate = 0.0f;
	m_flAttackAnimTime = 0.0f;
	m_pWoodStrategy->OnSpawn();
}

void CWood::Precache()
{
	PRECACHE_MODEL("models/shelter/item_wood.mdl");
}

void CWood::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{ 
	return CBaseMonster::Use(pActivator, pCaller, useType, value); 
}

void CWood::Touch(CBaseEntity *pOther)
{
	if (m_improv != NULL)
	{
		m_improv->OnTouch(pOther);
	}
}

float CWood::GetModifiedDamage(float flDamage, int nHitGroup) const
{
	switch (nHitGroup)
	{
		case HITGROUP_GENERIC: flDamage *= 1; break;
		case HITGROUP_STOMACH: flDamage *= 1.25; break;
		default: flDamage *= 1; break;
	}

	return flDamage;
}

int CWood::TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType)
{

	float flActualDamage;
	CBasePlayer *pAttacker = NULL;

	flActualDamage = GetModifiedDamage(flDamage, m_LastHitGroup);

	if (pevAttacker != NULL)
	{
		CBaseEntity *pAttackingEnt = GetClassPtr<CBaseEntity>(pevAttacker);

		if (pAttackingEnt->Classify() == CLASS_MACHINE)
		{
			CBaseEntity *pDriver = ((CFuncVehicle *)pAttackingEnt)->m_pDriver;

			if (pDriver != NULL)
			{
				pevAttacker = pDriver->pev;
			}
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

		if (pev->health--)
		{
			Killed2(pevAttacker, GIB_NORMAL);
			return 1;
		}

		if (pev->health > 0)
		{
			if (pAttacker != NULL)
			{
				return 1;
			}
		}
		else
		{
			Killed(pevAttacker, GIB_NORMAL);
		}
	
	return 0;
}

void CWood::BecomeDead(void)
{ 
	pev->health = 0;
	pev->movetype = MOVETYPE_NONE;
	pev->flags &= ~FL_ONGROUND;

	pev->takedamage = DAMAGE_NO;
	pev->deadflag = DEAD_DEAD;
	pev->solid = SOLID_NOT;
}

void CWood::Killed(entvars_t *pevAttacker, int iGib)
{
	if (!IsAlive())
		return;

	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "common/null.wav", VOL_NORM, ATTN_NORM);

	SetConditions(bits_COND_LIGHT_DAMAGE);

	CBaseEntity *pOwner = CBaseEntity::Instance(pev->owner);
	if (pOwner)
	{
		pOwner->DeathNotice(pev);
	}

	SetTouch(NULL);
	BecomeDead();

	pev->nextthink = gpGlobals->time + 0;
	SetThink(&CWood::Remove);

	m_pWoodStrategy->OnKilled(pevAttacker, iGib);
}

void CWood::Killed2(entvars_t* pevAttacker, int iGib)
{
	if (!IsAlive())
		return;

	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "common/null.wav", VOL_NORM, ATTN_NORM);

	SetConditions(bits_COND_LIGHT_DAMAGE);

	CBaseEntity* pOwner = CBaseEntity::Instance(pev->owner);
	if (pOwner)
	{
		pOwner->DeathNotice(pev);
	}

	m_pWoodStrategy->OnKilled(pevAttacker, iGib);
}

void CWood::Remove()
{
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_NOT;
	pev->takedamage = DAMAGE_NO;

	UTIL_SetSize(pev, Vector(6, 6, 18), Vector(6, 6, 18));
	pev->nextthink = -1;
	m_flNextFullThink = -1;

	SUB_Remove();
}

CWood::CWood() : CHostage(), 
	m_iKillBonusFrags(1), 
	m_pWoodStrategy(new CWoodModStrategy_Default(this))
	
{
	
	CWoodManager().OnEntityAdd(this);
}

CWood::~CWood()
{
	CWoodManager().OnEntityRemove(this);
}

void CWood::IdleThink()
{
	constexpr float upkeepRate = 0.0f;
	constexpr float giveUpTime = 0;
	constexpr float updateRate = 0.0f;

	if (!m_improv)
	{
		m_improv = new CWoodImprov(this);
	}

	const float flInterval = StudioFrameAdvance(0);

	pev->velocity = Vector(-16, 16, 0);
	UTIL_SetSize(pev, Vector(6, 6, 18), Vector(6, 6, 18));

	m_pWoodStrategy->OnThink();
}

bool CWood::CheckTarget()
{
	return false;
}

std::pair<CBasePlayer *, bool> CWood::FindTarget() const
{
	return { nullptr, false};
}

void CWood::Wander()
{
	if (m_improv)
	{
		CBaseEntity *target = NULL;

		float shorestDistance = 0.0f;
		CBaseEntity *shorestTarget = NULL;

	}
}

CBaseEntity * CWood::CheckTraceHullAttack(float flDist, int iDamage, int iDmgType)
{
	TraceResult tr;

	if (IsPlayer())
		UTIL_MakeVectors(pev->angles);
	else
		UTIL_MakeAimVectors(pev->angles);

	pev->velocity = Vector(6, 6, 18);
	UTIL_SetSize(pev, Vector(6, 6, 18), Vector(6, 6, 18));
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

bool CWood::ShouldAttack(CBaseEntity *target) const
{
	if (target->pev->takedamage == DAMAGE_NO)
		return false;

	if (target->IsPlayer() && !m_pWoodStrategy->IsTeamMate(target))
		return true;

	CZSHBreak*zsh_break = dynamic_cast<CZSHBreak*>(target);
	if (zsh_break)
		return zsh_break->m_flZombiDamageRatio > 0.0f;

	return false;
}

CBasePlayer * CWood::GetClosestPlayer(bool bVisible) const
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

void CWoodModStrategy_Default::OnSpawn(){}
void CWoodModStrategy_Default::OnThink(){}
void CWoodModStrategy_Default::OnKilled(entvars_t *pKiller, int iGib){}