#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "client.h"

#include "zbs_box.h"
#include "gamemode/mods.h"

#include <utility>

static std::pair<const char *, void(*)(CBasePlayer *p)> g_SupplyboxItems[]=
{
	{ "Dual MP7A1", [](CBasePlayer *p) {
			DropPrimary(p);
			p->GiveNamedItem("weapon_quantum");
			int iAmount = p->m_pModStrategy->ComputeMaxAmmo("46mm", MAX_AMMO_46MM);
			p->GiveAmmo(iAmount, "46mm", iAmount);
			
			DropSecondary(p);
			p->GiveNamedItem("weapon_gunkata");
			int iAmount2 = p->m_pModStrategy->ComputeMaxAmmo("50ae", MAX_AMMO_50AE);
			p->GiveAmmo(iAmount2, "762Nato", iAmount2);

			p->GiveNamedItem("weapon_twinaxes");
		}
	}
};

LINK_ENTITY_TO_CLASS(zbssupplybox, CZbsSupplyBox);

void CZbsSupplyBox::Precache()
{
	PRECACHE_SOUND("zb3/get_box.wav");
	PRECACHE_MODEL("models/zbs_box.mdl");
}

void CZbsSupplyBox::Spawn()
{
	Precache();

	if (pev->classname)
	{
		RemoveEntityHashValue(pev, STRING(pev->classname), CLASSNAME);
	}

	MAKE_STRING_CLASS("zbssupplybox", pev);
	AddEntityHashValue(pev, STRING(pev->classname), CLASSNAME);

	pev->movetype = MOVETYPE_TOSS;
	pev->solid = SOLID_TRIGGER;

	UTIL_SetSize(pev, Vector(-16, -16, 0), Vector(16, 16, 16));
	UTIL_SetOrigin(pev, pev->origin);
	SetTouch(&CZbsSupplyBox::SupplyboxTouch);
	SetThink(&CZbsSupplyBox::SupplyboxThink);

	SET_MODEL(edict(), "models/zbs_box.mdl");

	m_flNextRadarTime = gpGlobals->time + RANDOM_FLOAT(0, 1);
}

void CZbsSupplyBox::SupplyboxTouch(CBaseEntity *pOther)
{
	if (!pOther->IsPlayer())
		return;

	CBasePlayer *p = static_cast<CBasePlayer *>(pOther);

	if (p->m_bIsZombie)
		return;

	//auto &nf = g_SupplyboxItems[RANDOM_LONG(0, std::extent<decltype(g_SupplyboxItems)>::value - 1)];
	//nf.second(p);

	EMIT_SOUND(ENT(p->pev), CHAN_BODY, "zb3/get_box.wav", VOL_NORM, ATTN_NORM);

	pev->effects |= EF_NODRAW;
	SUB_Remove();
}

void CZbsSupplyBox::SupplyboxThink()
{
	if (pev->deadflag != DEAD_DEAD && !(pev->effects & EF_NODRAW))
	{
		if (m_flNextRadarTime <= gpGlobals->time)
		{
			m_flNextRadarTime = gpGlobals->time + 1;
		}
	}
}