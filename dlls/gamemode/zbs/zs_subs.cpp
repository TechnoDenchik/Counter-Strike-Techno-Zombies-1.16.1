
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"

#include "zs_subs.h"
#include "../mod_zbs.h"


LINK_ENTITY_TO_CLASS(zombiespawn, CZombieSpawn);

void CZombieSpawn::Spawn()
{
	return CPointEntity::Spawn();
}

void CZombieSpawn::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "master"))
	{
		pev->netname = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "id"))
	{
		m_iId = Q_atoi(pkvd->szValue);
	}
	else
		CPointEntity::KeyValue(pkvd);
}

BOOL CZombieSpawn::IsTriggered(CBaseEntity *pEntity)
{
	BOOL master = UTIL_IsMasterTriggered(pev->netname, pEntity);

	return master;
}

LINK_ENTITY_TO_CLASS(zbsbreak, CZBSBreak);

void CZBSBreak::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "hmdmgratio"))
	{
		m_flHumanDamageRatio = Q_atof(pkvd->szValue);
	}
	else if (FStrEq(pkvd->szKeyName, "zbdmgratio"))
	{
		m_flZombiDamageRatio = Q_atof(pkvd->szValue);
	}

	return CBreakable::KeyValue(pkvd);
}

int CZBSBreak::TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType)
{
	CBaseEntity *entAttacker = CBaseEntity::Instance(pevAttacker);
	if (entAttacker->IsPlayer())
	{
		CBasePlayer *attacker = static_cast<CBasePlayer *>(entAttacker);
		if (attacker->m_bIsZombie)
			flDamage *= m_flZombiDamageRatio;
		else
			flDamage *= m_flHumanDamageRatio;
	}
	return CBreakable::TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
}

LINK_ENTITY_TO_CLASS(zbstrigger, CZBSTriggerWin);

void CZBSTriggerWin::Spawn()
{
	InitTrigger();
	SetTouch(&CZBSTriggerWin::TouchWin);
}

void CZBSTriggerWin::TouchWin(CBaseEntity* pOther)
{
	if (m_iId != 14)
		return;

	if (!pOther->IsPlayer())
		return;

	if (iswin == true)
		return;

	CMod_ZombieScenario zbs;
	zbs.HumanWin();
	iswin = true;
	SetTouch(NULL);
	
}

void CZBSTriggerWin::KeyValue(KeyValueData* pkvd)
{
	if (FStrEq(pkvd->szKeyName, "model"))
	{
		m_model = Q_atof(pkvd->szValue);
	}
	else if (FStrEq(pkvd->szKeyName, "id"))
	{
		m_iId = Q_atof(pkvd->szValue);
	}
	else
		CBaseTrigger::KeyValue(pkvd);
}