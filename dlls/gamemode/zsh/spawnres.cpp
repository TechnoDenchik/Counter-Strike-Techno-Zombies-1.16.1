#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "client.h"
#include "spawnres.h"
#include <utility>

static std::pair<const char*, void(*)(CBasePlayer* p)> g_SupplyboxItems[] =
{
	{ "Dual MP7A1", [](CBasePlayer* p) {
			DropPrimary(p);
			p->GiveNamedItem("weapon_svdex");
			int iAmount = p->m_pModStrategy->ComputeMaxAmmo("46mm", MAX_AMMO_46MM);
			p->GiveAmmo(iAmount, "46mm", iAmount);

			DropSecondary(p);
			p->GiveNamedItem("weapon_infinityex2");
			int iAmount2 = p->m_pModStrategy->ComputeMaxAmmo("50ae", MAX_AMMO_50AE);
			p->GiveAmmo(iAmount2, "762Nato", iAmount2);

			p->GiveNamedItem("knife_dualsword");
		}
	}
};

LINK_ENTITY_TO_CLASS(spawnres, CSpawnres);

void CSpawnres::Precache()
{
	PRECACHE_MODEL("models/shelter/item_wood.mdl");
}

void CSpawnres::Spawn()
{
	Precache();
	CSpawnres* spawns = GetClassPtr<CSpawnres>(nullptr);
	if (pev->classname)
	{
		RemoveEntityHashValue(pev, STRING(pev->classname), CLASSNAME);
	}

	MAKE_STRING_CLASS("resoures", pev);
	AddEntityHashValue(pev, STRING(pev->classname), CLASSNAME);

	//pev->movetype = MOVETYPE_TOSS;
	//pev->solid = SOLID_BSP;

	SET_MODEL(edict(), "models/shelter/item_wood.mdl");
	pev->solid = SOLID_SLIDEBOX;
	pev->sequence = 0;
	pev->framerate = 1;
	pev->frame = 1;
	pev->health = 1000;
	pev->movetype = MOVETYPE_ANGLECLIP;
	pev->takedamage = DAMAGE_YES;
	pev->view_ofs = VEC_VIEW;
	pev->velocity = Vector(-16, 16, 0);

	//UTIL_SetSize(pev, Vector(-16, -16, -0), Vector(16, 16, 16));
	//UTIL_SetOrigin(pev, pev->origin);
	SetTouch(&CSpawnres::SpawnresTouch);
	SetThink(&CSpawnres::SpawnresThink);

	UTIL_MakeVectors(pev->v_angle);

	

	UTIL_SetSize(spawns->pev, VEC_HULL_MIN, VEC_HULL_MAX);
	m_flNextRadarTime = gpGlobals->time + RANDOM_FLOAT(0, 1);
}

void CSpawnres::SpawnresTouch(CBaseEntity *pOther)
{
	if (!pOther->IsPlayer())
		return;

	CBasePlayer *p = static_cast<CBasePlayer *>(pOther);

	if (p->m_bIsVIP || p->m_bIsZombie)
		return;

	auto &nf = g_SupplyboxItems[RANDOM_LONG(0, std::extent<decltype(g_SupplyboxItems)>::value - 1)];
	nf.second(p);

	pev->effects |= EF_NODRAW;
	SendPositionMsg();
	SUB_Remove();
}

void CSpawnres::SpawnresThink()
{
	if (pev->deadflag != DEAD_DEAD )
	{
		if (m_flNextRadarTime <= gpGlobals->time)
		{
			SendPositionMsg();
			m_flNextRadarTime = gpGlobals->time + 1;
		}
	}
}

void CSpawnres::SendPositionMsg()
{
	CBaseEntity *pEntity = NULL;

	while ((pEntity = UTIL_FindEntityByClassname(pEntity, "player")) != NULL)
	{
		if (FNullEnt(pEntity->edict()))
			break;

		if (!pEntity->IsPlayer())
			continue;

		if (pEntity->pev->flags == FL_DORMANT)
			continue;

		CBasePlayer *pTempPlayer = static_cast<CBasePlayer *>(pEntity);

		if (pTempPlayer->pev->deadflag == DEAD_NO && pTempPlayer->m_iTeam == CT)
		{
			if (pev->effects & EF_NODRAW)
			{
				MESSAGE_BEGIN(MSG_ONE, gmsgHostageK, NULL, pTempPlayer->pev);
				WRITE_BYTE(m_iSupplyboxIndex);
				MESSAGE_END();
			}
			else
			{
				MESSAGE_BEGIN(MSG_ONE, gmsgHostagePos, NULL, pTempPlayer->pev);
				WRITE_BYTE(0);
				WRITE_BYTE(m_iSupplyboxIndex);
				WRITE_COORD(pev->origin.x);
				WRITE_COORD(pev->origin.y);
				WRITE_COORD(pev->origin.z);
				MESSAGE_END();
			}
			
		}
	}
}