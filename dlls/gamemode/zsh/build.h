/*#ifndef ZSH_GEN_H
#define ZSH_GEN_H
#ifdef _WIN32
#pragma once
#endif

#include "func_break.h" // CBreakable

class CGenSpawn : public CPointEntity
{
public:
	void Spawn() override;
	void KeyValue(KeyValueData* pkvd) override;
	BOOL IsTriggered(CBaseEntity* pEntity) override;

public:
	int m_iId;

};
#endif*/