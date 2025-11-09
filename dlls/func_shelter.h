/*
*
*   This program is free software; you can redistribute it and/or modify it
*   under the terms of the GNU General Public License as published by the
*   Free Software Foundation; either version 2 of the License, or (at
*   your option) any later version.
*
*   This program is distributed in the hope that it will be useful, but
*   WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software Foundation,
*   Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*   In addition, as a special exception, the author gives permission to
*   link the code of this program with the Half-Life Game Engine ("HL
*   Engine") and Modified Game Libraries ("MODs") developed by Valve,
*   L.L.C ("Valve").  You must obey the GNU General Public License in all
*   respects for all of the code used other than the HL Engine and MODs
*   from Valve.  If you modify this file, you may extend this exception
*   to your version of the file, but you are not obligated to do so.  If
*   you do not wish to do so, delete this exception statement from your
*   version.
*
*/

#ifndef FUNC_SHELTER_H
#define FUNC_SHELTER_H
#ifdef _WIN32
#pragma once
#endif

// this many shards spawned when breakable objects break;
#define NUM_SHARDS 6

// func breakable
#define SF_BREAK_TRIGGER_ONLY		1	// may only be broken by trigger
#define	SF_BREAK_TOUCH			2	// can be 'crashed through' by running player (plate glass)
#define SF_BREAK_PRESSURE		4	// can be broken by a player standing on it
#define SF_BREAK_CROWBAR		256	// instant break if hit with crowbar

// func_pushable (it's also func_breakable, so don't collide with those flags)
#define SF_PUSH_BREAKABLE		128

#include "func_break.h"

class CShelter: public CBreakable
{
public:
	// basic functions
	void Spawn() override;
	void Precache() override;
	void Restart() override;
	void KeyValue(KeyValueData *pkvd) override;
	int Save(CSave &save) override;
	int Restore(CRestore &restore) override;
	int ObjectCaps() override { return (CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION); } 

	// To spark when hit
	void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType) override;
	// breakables use an overridden takedamage
	int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType) override;

	int DamageDecal(int bitsDamageType);
	void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value) override;

public:
	void EXPORT BreakTouch(CBaseEntity *pOther) ;
	void DamageSound();

	BOOL IsBreakable();
	NOXREF BOOL SparkWhenHit();

	void EXPORT Die();

	BOOL Explodable() const		{ return ExplosionMagnitude() > 0; }
	int ExplosionMagnitude() const	{ return pev->impulse; }

	void ExplosionSetMagnitude(int magnitude) { pev->impulse = magnitude; }

	static void MaterialSoundPrecache(Materials precacheMaterial);
	static void MaterialSoundRandom(edict_t *pEdict, Materials soundMaterial, float volume);
	static const char **MaterialSoundList(Materials precacheMaterial, int &soundCount);

	static const char *pSoundsWood[3];
	static const char *pSoundsFlesh[6];
	static const char *pSoundsGlass[3];
	static const char *pSoundsMetal[3];
	static const char *pSoundsConcrete[3];
	static const char *pSpawnObjects[32];

	static TYPEDESCRIPTION m_SaveData[5];
	float m_flHumanDamageRatio;
	float m_flZombiDamageRatio;
	float m_flNextSpawnNPC;
	bool shelterattack(bool result)
	{
		return result;
	}
public:
	friend class CBaseTurret;
	Materials m_Material;
	Explosions m_Explosion;
	int m_idShard;
	float m_angle;
	int m_iszGibModel;
	int m_iszSpawnObject;
	float m_flHealth;

	EHANDLE m_hShelterEnt;

	duration_t tNextAttack9;
	time_point_t tWorldTime9;
	duration_t tDelta9;

	duration_t tNextAttack10;
	time_point_t tWorldTime10;
	duration_t tDelta10;

};

class CPushable2: public CShelter
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual void KeyValue(KeyValueData *pkvd);
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	virtual int ObjectCaps() { return (CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION) | FCAP_CONTINUOUS_USE; }
	virtual int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);
	virtual void Touch(CBaseEntity *pOther);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);

public:
	void Move(CBaseEntity *pMover, int push);
	void EXPORT StopSound()
	{
#if 0
		Vector dist = pev->oldorigin - pev->origin;
		if (dist.Length() <= 0)
		{
			STOP_SOUND(ENT(pev), CHAN_WEAPON, m_soundNames[m_lastSound]);
		}
#endif
	}
	float MaxSpeed() const { return m_maxSpeed; }

public:
	static TYPEDESCRIPTION m_SaveData[2];
	static const char *m_soundNames[3];

	int m_lastSound;
	float m_maxSpeed;
	float m_soundTime;
};

#endif // FUNC_BREAK_H
