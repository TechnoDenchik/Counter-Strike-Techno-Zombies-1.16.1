#pragma once

#include "hostage/hostage.h"
#include <memory>
#include <map>
#include <string>
#include <mutex>

class CSpawnres : public CBaseEntity
{
public:
	
	virtual void Spawn();
	virtual void Precache();

public:
	void EXPORT SpawnresThink();
	void EXPORT SpawnresTouch(CBaseEntity* pOther);
	void getwood(CBaseEntity* pOther);

public:
	void SendPositionMsg();

public:
	int m_iModel;
	float m_flNextRadarTime;
	int m_iSupplyboxIndex;
};