#pragma once

class CZbsSupplyBox : public CBaseEntity
{
public:
	virtual void Spawn();
	virtual void Precache();

public:
	void EXPORT SupplyboxThink();
	void EXPORT SupplyboxTouch(CBaseEntity *pOther);

public:
	float m_flNextRadarTime;
	int m_iSupplyboxIndex;
};

#include "func_break.h" // CBreakable