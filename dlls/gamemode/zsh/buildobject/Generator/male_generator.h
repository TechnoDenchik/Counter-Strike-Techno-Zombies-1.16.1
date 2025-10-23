#pragma once

#include "hostage/hostage.h"
#include <memory>
#include <map>
#include <string>
#include <mutex>

class CGeneratorMale : public CBaseEntity
{
public:

	static CGeneratorMale* Create(const Vector& vecOrigin, const Vector& vecAngles, edict_t* pentOwner);
	void Init(CBasePlayer* pOwner, Vector vecVelocity);
	void Remove();
	virtual void Spawn();
	virtual void Precache();
public:
	void EXPORT GeneratorThink();
	void EXPORT GeneratorTouch(CBaseEntity* pOther);
	int TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType) override;
	float GetModifiedDamage(float flDamage, int nHitGroup) const;
	void PlayDeadSound();

public:
	CBasePlayer* m_pOwner;
	time_point_t m_flNextAnim;

	int m_iTeam;
	int m_iState;
	int m_iModel;
	float m_flNextRadarTime;
	int m_iGeneratorIndex;
};