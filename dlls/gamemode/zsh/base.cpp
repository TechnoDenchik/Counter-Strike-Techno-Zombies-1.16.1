#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "client.h"

#include "base.h"
#include "gamemode/mods.h"
#include "gamemode/mod_zbshelter_pve.h"
#include <dlls/util/u_range.hpp>
#include <utility>

static std::pair<const char*, void(*)(CBasePlayer* p)> g_SupplyboxItems[] =
{
	{ "Dual MP7A1", [](CBasePlayer* p) {
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

LINK_ENTITY_TO_CLASS(mentality, CMentalityHealth);

void CMentalityHealth::Precache()
{
	PRECACHE_SOUND("zb3/get_box.wav");
	PRECACHE_MODEL("models/supplybox.mdl");
}

void CMentalityHealth::Spawn()
{
	Precache();

	if (pev->classname)
	{
		RemoveEntityHashValue(pev, STRING(pev->classname), CLASSNAME);
	}

	MAKE_STRING_CLASS("mentality", pev);
	AddEntityHashValue(pev, STRING(pev->classname), CLASSNAME);

	pev->movetype = MOVETYPE_TOSS;
	pev->solid = SOLID_TRIGGER;

	UTIL_SetSize(pev, Vector(-16, -16, 0), Vector(16, 16, 16));
	UTIL_SetOrigin(pev, pev->origin);
	SetTouch(&CMentalityHealth::MentalityTouch);
	SetThink(&CMentalityHealth::MentalityThink);

	//SET_MODEL(edict(), "models/supplybox.mdl");

	m_flNextRadarTime = gpGlobals->time + RANDOM_FLOAT(0, 1);
}

void CMentalityHealth::MentalityTouch(CBaseEntity* pOther)
{
	if (!pOther->IsPlayer())
		return;

	CBasePlayer* p = static_cast<CBasePlayer*>(pOther);

	if ( p->m_bIsZombie)
		return;

	pev->effects |= EF_NODRAW;
	SendPositionMsg();
	SUB_Remove();
}

void CMentalityHealth::MentalityThink()
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

void CMentalityHealth::SendPositionMsg()
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
				MESSAGE_BEGIN(MSG_ONE, gmsgZSHMsgShelterPosK, NULL, pTempPlayer->pev);
				WRITE_BYTE(m_iMentalityIndex);
				MESSAGE_END();
				
			}
			else
			{
				MESSAGE_BEGIN(MSG_ONE, gmsgZSHMsgShelterPos, NULL, pTempPlayer->pev);
				WRITE_BYTE(0);
				WRITE_BYTE(m_iMentalityIndex);
				WRITE_COORD(pev->origin.x);
				WRITE_COORD(pev->origin.y);
				WRITE_COORD(pev->origin.z);
				MESSAGE_END();
			}

		}
	}
}

LINK_ENTITY_TO_CLASS(morale_static, CMenSpawn);

void CMenSpawn::Spawn()
{
	return CPointEntity::Spawn();
}

void CMenSpawn::KeyValue(KeyValueData* pkvd)
{

}

BOOL CMenSpawn::IsTriggered(CBaseEntity* pEntity)
{
	BOOL master = UTIL_IsMasterTriggered(pev->netname, pEntity);

	return master;
}

void CMod_ZombieShelter_coop::CheckMentality()
{
	if (gpGlobals->time - tWorldTimeMen < 1.0f)
	{
		tDeltaMen += gpGlobals->time - tWorldTimeMen;
	}
	if (tNextAttackMen > 1.0f || (gpGlobals->time - tWorldTimeMen > 1.0f) || tDeltaMen > 1.0f)
	{
		tNextAttackMen = 0.0f;
		tDeltaMen = 0.0f;

		if (mentalityhealth == 10)
		{
			for (int iIndex = 1; iIndex <= gpGlobals->maxClients; ++iIndex)
			{
				CBaseEntity* entity = UTIL_PlayerByIndex(iIndex);
				if (!entity)
					continue;
				CLIENT_COMMAND(entity->edict(), "spk zsh/warning.wav\n");
			}
		}
		
		if (mentality == true)
		{
			if(mentalityhealth < 50)
			{ 
				mentalityhealth += 5;
			}
		}
		else
		{
			if (mentalityhealth > 1)
			{
				mentalityhealth--;
			}
			if (mentalityhealth < 2)
			{
				mentalitysecond--;
				if (mentalitysecond == 1)
				{
					for (CBasePlayer* m_pPlayer : moe::range::PlayersList())
					{
						m_pPlayer->TakeDamage(m_pPlayer->pev, m_pPlayer->pev, 5, DMG_BULLET);
					}
					mentalitysecond = 7;
				}
			}
		}
	}
	tWorldTimeMen = gpGlobals->time;
}