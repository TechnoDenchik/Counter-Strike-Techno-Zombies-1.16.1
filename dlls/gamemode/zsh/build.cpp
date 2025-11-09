/*#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "func_break.h"
#include "build.h"

//LINK_ENTITY_TO_CLASS(func_illusionary, CGenSpawn);

void CGenSpawn::Spawn()
{
	return CPointEntity::Spawn();
}

void CGenSpawn::KeyValue(KeyValueData* pkvd)
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

BOOL CGenSpawn::IsTriggered(CBaseEntity* pEntity)
{
	BOOL master = UTIL_IsMasterTriggered(pev->netname, pEntity);

	return master;
}*/