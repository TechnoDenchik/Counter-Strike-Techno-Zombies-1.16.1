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

/*LINK_ENTITY_TO_CLASS(generatormale, CGeneratorMale);

void CGeneratorMale::Precache()
{
	PRECACHE_MODEL("models/zsh_deadcity/zsh_generator_1.mdl");
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

	SetTouch(&CGeneratorMale::GeneratorTouch);
	SetThink(&CGeneratorMale::GeneratorThink);

	UTIL_MakeVectors(pev->v_angle);

	UTIL_SetSize(spawns->pev, VEC_HULL_MIN, VEC_HULL_MAX);
	m_flNextRadarTime = gpGlobals->time + RANDOM_FLOAT(0, 1);

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

	pev->effects |= EF_NODRAW;
	SUB_Remove();
}

void CGeneratorMale::GeneratorThink()
{
	if (pev->deadflag != DEAD_DEAD)
	{
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

	//PlayPainSound();

	MESSAGE_BEGIN(MSG_ONE, gmsgHitMsg, NULL, pevAttacker);
	WRITE_LONG((long)flDamage);
	WRITE_SHORT(ENTINDEX(edict()));
	WRITE_BYTE(0);
	MESSAGE_END();
	if (pev->health > 0)
	{
		if (pAttacker != NULL)
		{
			// Player damaged monster
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

void CGeneratorMale::SendPositionMsg()
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
				MESSAGE_BEGIN(MSG_ONE, gmsgHostageK, NULL, pTempPlayer->pev);
				WRITE_BYTE(m_iGeneratorIndex);
				MESSAGE_END();
			}
			else
			{
				MESSAGE_BEGIN(MSG_ONE, gmsgHostagePos, NULL, pTempPlayer->pev);
				WRITE_BYTE(0);
				WRITE_BYTE(m_iGeneratorIndex);
				WRITE_COORD(pev->origin.x);
				WRITE_COORD(pev->origin.y);
				WRITE_COORD(pev->origin.z);
				MESSAGE_END();
			}

		}
	}
}*/