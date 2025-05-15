#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "client.h"

#include "supplyboxreference.h"
#include "gamemode/mods.h"

#include <utility>

static std::pair<const char *, void(*)(CBasePlayer *p)> g_SupplyboxItems[]=
{
	{ "Human", [](CBasePlayer *p) {
			DropPrimary(p);
			p->GiveNamedItem("weapon_starchaserar");
			int iAmount = p->m_pModStrategy->ComputeMaxAmmo("556Nato", MAX_AMMO_556NATOBOX);
			p->GiveAmmo(iAmount, "556Nato", iAmount);
			
			DropSecondary(p);
			p->GiveNamedItem("weapon_infinityex2");
			int iAmount2 = p->m_pModStrategy->ComputeMaxAmmo("45acp", MAX_AMMO_45ACP);
			p->GiveAmmo(iAmount2, "45acp", iAmount2);

			p->GiveNamedItem("knife_dragonsword");
			p->GiveNamedItem("weapon_hegrenade");
		}
	}
};

LINK_ENTITY_TO_CLASS(supplyboxR, CSupplyBoxR);

void CSupplyBoxR::Precache()
{
	PRECACHE_SOUND("zb3/get_box.wav");
	PRECACHE_MODEL("models/supplybox.mdl");
}

void CSupplyBoxR::Spawn()
{
	Precache();

	if (pev->classname)
	{
		RemoveEntityHashValue(pev, STRING(pev->classname), CLASSNAME);
	}

	MAKE_STRING_CLASS("supplybox", pev);
	AddEntityHashValue(pev, STRING(pev->classname), CLASSNAME);

	pev->movetype = MOVETYPE_TOSS;
	pev->solid = SOLID_TRIGGER;

	UTIL_SetSize(pev, Vector(-16, -16, 0), Vector(16, 16, 16));
	UTIL_SetOrigin(pev, pev->origin);
	SetTouch(&CSupplyBoxR::SupplyboxTouch);
	SetThink(&CSupplyBoxR::SupplyboxThink);

	SET_MODEL(edict(), "models/supplybox.mdl");

	m_flNextRadarTime = gpGlobals->time + RANDOM_FLOAT(0, 1);
}

void CSupplyBoxR::SupplyboxTouch(CBaseEntity *pOther)
{
	if (!pOther->IsPlayer())
		return;

	CBasePlayer *p = static_cast<CBasePlayer *>(pOther);

	if (p->m_bIsZombie)
		return;

	auto &nf = g_SupplyboxItems[RANDOM_LONG(0, std::extent<decltype(g_SupplyboxItems)>::value - 1)];
	nf.second(p);

	EMIT_SOUND(ENT(p->pev), CHAN_BODY, "zb3/get_box.wav", VOL_NORM, ATTN_NORM);

	pev->effects |= EF_NODRAW;
	SendPositionMsg();
	SUB_Remove();
}

void CSupplyBoxR::SupplyboxThink()
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

void CSupplyBoxR::SendPositionMsg()
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

void CSupSpawnR::Spawn()
{
	return CPointEntity::Spawn();
}

void CSupSpawnR::KeyValue(KeyValueData* pkvd)
{
	
}

BOOL CSupSpawnR::IsTriggered(CBaseEntity* pEntity)
{
	BOOL master = UTIL_IsMasterTriggered(pev->netname, pEntity);

	return master;
}