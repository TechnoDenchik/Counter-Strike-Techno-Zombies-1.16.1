#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "client.h"
#include "male_generator.h"
#include <utility>
#include "monsters.h"
#include "game.h"
#include "bot_include.h"

LINK_ENTITY_TO_CLASS(generatormale, CGeneratorMale);

CGeneratorMale* CGeneratorMale::Create(const Vector& vecOrigin, const Vector& vecAngles, edict_t* pentOwner)
{
	edict_t* pent = CREATE_NAMED_ENTITY(MAKE_STRING("generatormale"));

	if (FNullEnt(pent))
	{
		ALERT(at_console, "NULL Ent in Create!\n");
		return NULL;
	}

	CGeneratorMale* pMine = (CGeneratorMale*)Instance(pent);

	if (pMine)
	{
		pMine->pev->owner = pentOwner;
		pMine->pev->origin = vecOrigin;
		pMine->pev->angles = vecAngles;
		pMine->Spawn();
	}

	return pMine;
}

void CGeneratorMale::Precache()
{
	PRECACHE_MODEL("models/zsh_deadcity/zsh_generator_1.mdl");
	PRECACHE_SOUND("zsh/build2.wav");
}

void CGeneratorMale::Init(CBasePlayer* pOwner, Vector vecVelocity)
{
	m_pOwner = pOwner;
	m_iTeam = m_pOwner->m_iTeam;
	pev->velocity = std::move(vecVelocity);
}

void CGeneratorMale::Remove()
{
	SetThink(nullptr);
	pev->effects |= EF_NODRAW; // 0x80u
	return UTIL_Remove(this);
}


void CGeneratorMale::Spawn()
{
	Precache();
	CGeneratorMale* spawns = GetClassPtr<CGeneratorMale>(nullptr);
	if (pev->classname)
	{
		RemoveEntityHashValue(pev, STRING(pev->classname), CLASSNAME);
	}

	MAKE_STRING_CLASS("generatormale", pev);
	AddEntityHashValue(pev, STRING(pev->classname), CLASSNAME);

	SET_MODEL(edict(), "models/zsh_deadcity/zsh_generator_1.mdl");
	
	SetThink(&CGeneratorMale::GeneratorThink);
	
	UTIL_SetOrigin(pev, pev->origin);

	pev->angles = g_vecZero;
	pev->gravity = 2.5;
	pev->solid = SOLID_BBOX;
	pev->movetype = MOVETYPE_TOSS;

	pev->takedamage = DAMAGE_YES;
	pev->max_health = 100;
	pev->health = pev->max_health;

	if (pev->spawnflags & SF_MONSTER_HITMONSTERCLIP)
		pev->flags |= FL_MONSTERCLIP;

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

}

void CGeneratorMale::GeneratorTouch(CBaseEntity* pOther)
{
	if (!pOther->IsPlayer())
		return;

	CBasePlayer* p = static_cast<CBasePlayer*>(pOther);


}

void CGeneratorMale::GeneratorThink()
{
	if (pev->flags & FL_ONGROUND)
	{
		if (m_iState == 2)
		{
			if (m_iState == 3)
			{

			}

			if (m_flNextAnim != 0 && m_flNextAnim < gpGlobals->time)
			{
				pev->sequence = 1;

				m_iState = 2;
				m_flNextAnim = 0;
			}

			if (!m_iState)
			{
				pev->sequence = 0;
				pev->frame = 0;
				//ResetSequenceInfo();

				EMIT_SOUND(ENT(pev), CHAN_WEAPON, "zsh/build2.wav", VOL_NORM, ATTN_NORM);

				m_flNextAnim = gpGlobals->time + 0.5f;

				m_iState = 1;
			}
		}
	}

	if (m_flNextRadarTime <= gpGlobals->time)
	{
		m_flNextRadarTime = gpGlobals->time + 1;
	}
}

float CGeneratorMale::GetModifiedDamage(float flDamage, int nHitGroup) const
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

int CGeneratorMale::TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType)
{
	CBasePlayer* pAttacker = NULL;

	//flActualDamage = GetModifiedDamage(flDamage, m_LastHitGroup);

	if (pevAttacker != NULL)
	{
		CBaseEntity* pAttackingEnt = GetClassPtr<CBaseEntity>(pevAttacker);


		if (pAttackingEnt->IsPlayer())
		{
			pAttacker = GetClassPtr<CBasePlayer>(pevAttacker);
		}
	}

	//flActualDamage = g_pModRunning->GetAdjustedEntityDamage(this, pevInflictor, pevAttacker, flActualDamage, bitsDamageType);

	if (pev->health > 0)
	{
		if (pAttacker != NULL)
		{
			
			return 1;
		}
	}
	else
	{

	}

	return 0;
}

void CGeneratorMale::PlayDeadSound()
{
	 EMIT_SOUND(edict(), CHAN_VOICE, "zsh/crash.wav", VOL_NORM, ATTN_NORM);
}