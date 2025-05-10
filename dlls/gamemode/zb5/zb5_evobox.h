#pragma once

#include "mod_zb5.h"

class CZb5EvoBox : public CBaseEntity
{
public:
	virtual void Spawn();
	virtual void Precache();
	void SendPositionMsg();

public:
	void EXPORT EvoboxThink();
	void EXPORT EvoboxTouch(CBaseEntity *pOther);

public:
	float m_flNextRadarTime;
	int m_iSupplyboxIndex;
};

#include "func_break.h" // CBreakable