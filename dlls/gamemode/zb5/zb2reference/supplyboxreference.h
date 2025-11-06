#pragma once

class CSupplyBoxR : public CBaseEntity
{
public:
	virtual void Spawn();
	virtual void Precache();

public:
	void EXPORT SupplyboxThink();
	void EXPORT SupplyboxTouch(CBaseEntity *pOther);

public:
	void SendPositionMsg();
	void CreateSprite();
	void UpdateSpritePosition();
	void RemoveSprite();

public:
	CBaseEntity* m_pSprite;
	float m_flNextRadarTime;
	int m_iSupplyboxIndex;
};

#include "func_break.h"

class CSupSpawnR : public CPointEntity
{
public:
	void Spawn() override;
	void KeyValue(KeyValueData* pkvd) override;
	BOOL IsTriggered(CBaseEntity* pEntity) override;

public:
	int m_iId;

};