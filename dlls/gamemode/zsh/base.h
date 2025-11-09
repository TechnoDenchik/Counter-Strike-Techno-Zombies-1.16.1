#pragma once

#include "hostage/hostage.h"
#include <memory>
#include <map>
#include <string>
#include <mutex>
#include "func_break.h"

class CMentalityHealth : public CBaseEntity
{
public:
	virtual void Spawn();
	virtual void Precache();

public:
	void EXPORT MentalityThink();
	void EXPORT MentalityTouch(CBaseEntity* pOther);
	void SendPositionMsg();

public:
	int m_iModel;
	float m_flNextRadarTime;
	int m_iMentalityIndex;
};

class CMenSpawn : public CPointEntity
{
public:
	void Spawn() override;
	void KeyValue(KeyValueData* pkvd) override;
	BOOL IsTriggered(CBaseEntity* pEntity) override;

public:
	int m_iId;

};