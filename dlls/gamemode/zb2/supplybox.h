#pragma once

class CSupplyBox : public CBaseEntity
{
public:
	virtual void Spawn();
	virtual void Precache();

public:
	void EXPORT SupplyboxThink();
	void EXPORT SupplyboxTouch(CBaseEntity *pOther);

public:
	void SendPositionMsg();

public:
	float m_flNextRadarTime;
	int m_iSupplyboxIndex;
};

#include "func_break.h" // CBreakable

class CSupSpawn : public CPointEntity
{
public:
	void Spawn() override;
	void KeyValue(KeyValueData* pkvd) override;
	BOOL IsTriggered(CBaseEntity* pEntity) override;

public:
	int m_iId;

};