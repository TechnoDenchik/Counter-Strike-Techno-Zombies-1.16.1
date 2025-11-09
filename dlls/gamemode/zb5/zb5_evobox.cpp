#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "client.h"

#include "zb5_evobox.h"
#include "gamemode/mods.h"
#include "mod_zb5.h"

#include <utility>

static std::pair<const char *, void(*)(CBasePlayer *p)> g_SupplyboxItems[]=
{
	{ 
		"Dual MP7A1", [](CBasePlayer *p) 
		{

			CLIENT_COMMAND(p->edict(), "CST_Getevo");
		
		}
	}
};

LINK_ENTITY_TO_CLASS(zb5evobox, CZb5EvoBox);

void CZb5EvoBox::Precache()
{
	PRECACHE_SOUND("zb5/level_up.wav");
	PRECACHE_MODEL("models/zb5_virusbox.mdl");
}

void CZb5EvoBox::Spawn()
{
	Precache();

	if (pev->classname)
	{
		RemoveEntityHashValue(pev, STRING(pev->classname), CLASSNAME);
	}

	MAKE_STRING_CLASS("zb5evobox", pev);
	AddEntityHashValue(pev, STRING(pev->classname), CLASSNAME);

	pev->movetype = MOVETYPE_TOSS;
	pev->solid = SOLID_TRIGGER;

	UTIL_SetSize(pev, Vector(-16, -16, 0), Vector(16, 16, 16));
	UTIL_SetOrigin(pev, pev->origin);
	SetTouch(&CZb5EvoBox::EvoboxTouch);
	SetThink(&CZb5EvoBox::EvoboxThink);

	SET_MODEL(edict(), "models/zb5_virusbox.mdl");

	m_flNextRadarTime = gpGlobals->time + RANDOM_FLOAT(0, 1);
}

void CZb5EvoBox::EvoboxTouch(CBaseEntity *pOther)
{
	if (!pOther->IsPlayer())
		return;

	CBasePlayer *p = static_cast<CBasePlayer *>(pOther);

	if (p->m_bEvolutionProtected)
		return;

	if (p->m_bIsVIP || !p->m_bIsZombie)
		return;

	auto &nf = g_SupplyboxItems[RANDOM_LONG(0, std::extent<decltype(g_SupplyboxItems)>::value - 1)];
	nf.second(p);

	CLIENT_COMMAND(p->edict(), "CST_Getevo\n");

	CLIENT_COMMAND(p->edict(), "spk zb5/human_ability.wav\n");

	MESSAGE_BEGIN(MSG_ONE, gmsgZB5GetLocationBoxK, NULL, p->pev);
	WRITE_BYTE(m_iSupplyboxIndex);
	MESSAGE_END();

	pev->effects |= EF_NODRAW;
	SendPositionMsg();
	SUB_Remove();
}

void CZb5EvoBox::EvoboxThink()
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

void CZb5EvoBox::SendPositionMsg()
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

		if (pTempPlayer->pev->deadflag == DEAD_NO && pTempPlayer->m_iTeam == TERRORIST)
		{
			if (pev->effects & EF_NODRAW)
			{
				MESSAGE_BEGIN(MSG_ONE, gmsgZB5GetLocationBoxK, NULL, pTempPlayer->pev);
				WRITE_BYTE(m_iSupplyboxIndex);
				MESSAGE_END();
			}
			else
			{
				MESSAGE_BEGIN(MSG_ONE, gmsgZB5GetLocationBox, NULL, pTempPlayer->pev);
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