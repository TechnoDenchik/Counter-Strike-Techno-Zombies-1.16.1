/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#ifndef WPN_PLASMAGUN_H
#define WPN_PLASMAGUN_H
#ifdef _WIN32
#pragma once
#endif


#define AUG_MAX_SPEED			240
#define AUG_DAMAGE			32
#define AUG_RANGE_MODIFER		0.96
#define AUG_RELOAD_TIME			3.3

class CPlasmaGun : public CBasePlayerWeapon
{
public:
	void Spawn() override;
	void Precache() override;
	int GetItemInfo(ItemInfo *p) override;
	BOOL Deploy() override;
	float GetMaxSpeed() override { return AUG_MAX_SPEED; }
	int iItemSlot() override { return PRIMARY_WEAPON_SLOT; }
	void PrimaryAttack() override;
	void SecondaryAttack() override;
	void ShootProjectile();
	void Reload() override;
	void WeaponIdle() override;
	const char* GetCSModelName() override { return "models/w_plasmagun.mdl"; }
	BOOL UseDecrement() override {
#ifdef CLIENT_WEAPONS
		return TRUE;
#else
		return FALSE;
#endif
	}
	KnockbackData GetKnockBackData() override { return { 350.0f,250.0f,300.0f,100.0f,0.6f }; }

public:
	void PLASMAGUNFire(float flSpread, float flCycleTime, BOOL fUseAutoAim);

	double damageA;
	double damageB;

	int m_iShell;
	int iShellOn;

private:
	unsigned short m_usFire;
	unsigned short m_usFireAug;
};

#ifndef CLIENT_DLL

#include "monsters.h"

class CPlasmaGunProjectile : public CBaseEntity
{
public:
	void Spawn() override;

	void Precache() override;

	KnockbackData GetKnockBackData()
	{
		return { 0.0, 0.0, 0.0, 0.0, 1.0 };
	}

	void EXPORT OnTouch(CBaseEntity* pOther)
	{
		if (pev->owner == pOther->edict())
			return;

		CBaseEntity* pAttacker = CBaseEntity::Instance(pev->owner);
		CBasePlayer* pAttackePlayer = nullptr;
		if (pAttacker && pAttacker->IsPlayer())
			pAttackePlayer = static_cast<CBasePlayer*>(pAttacker);

		if (pAttackePlayer &&
			pOther->pev->takedamage != DAMAGE_NO &&
			pOther->IsAlive()
			)
		{
			Vector vecDirection = (pOther->pev->origin - pev->origin).Normalize();

			TraceResult tr;
			UTIL_TraceLine(pev->origin, pOther->pev->origin, missile, ENT(pAttackePlayer->pev), &tr);

			tr.iHitgroup = HITGROUP_CHEST; // ...

			ClearMultiDamage();
			pOther->TraceAttack(pAttackePlayer->pev, m_flTouchDamage, vecDirection, &tr, DMG_BULLET);
			ApplyMultiDamage(pAttackePlayer->pev, pAttackePlayer->pev);
		}

		RadiusDamage();
	}

	void EXPORT OnThink()
	{
		if (gpGlobals->time <= m_flAnimEndTime)
		{
			this->pev->frame = (float)(this->pev->framerate * gpGlobals->frametime) + this->pev->frame;
			if (pev->frame > m_flMaxFrames)
			{
				pev->frame = fmod(pev->frame, m_flMaxFrames);
			}
			pev->nextthink = gpGlobals->time + 0.0099999998;
		}
		else
		{
			Remove();
		}
	}

	void RadiusDamage()
	{
		const float flRadius = m_flExplodeRadius;
		const float flDamage = m_flExplodeDamage;
		const Vector vecSrc = pev->origin;
		entvars_t* const pevAttacker = VARS(pev->owner);
		entvars_t* const pevInflictor = this->pev;
		int bitsDamageType = DMG_BULLET;

		TraceResult tr;
		const float falloff = flRadius ? flDamage / flRadius : 1;
		const int bInWater = (UTIL_PointContents(vecSrc) == CONTENTS_WATER);

		CBaseEntity* pEntity = NULL;
		while ((pEntity = UTIL_FindEntityInSphere(pEntity, vecSrc, flRadius)) != NULL)
		{
			if (pEntity->pev->takedamage != DAMAGE_NO)
			{
				if (bInWater && !pEntity->pev->waterlevel)
					continue;

				if (!bInWater && pEntity->pev->waterlevel == 3)
					continue;

				if (pEntity->IsBSPModel())
					continue;

				if (pEntity->pev == pevAttacker)
					continue;

				Vector vecSpot = pEntity->BodyTarget(vecSrc);
				UTIL_TraceLine(vecSrc, vecSpot, missile, ENT(pevInflictor), &tr);

				if (tr.flFraction == 1.0f || tr.pHit == pEntity->edict())
				{
					if (tr.fStartSolid)
					{
						tr.vecEndPos = vecSrc;
						tr.flFraction = 0;
					}
					float flAdjustedDamage = flDamage - (vecSrc - pEntity->pev->origin).Length() * falloff;
					flAdjustedDamage = Q_max(0, flAdjustedDamage);

					if (tr.flFraction == 1.0f)
					{
						pEntity->TakeDamage(pevInflictor, pevAttacker, flAdjustedDamage, bitsDamageType);
					}
					else
					{

						tr.iHitgroup = HITGROUP_CHEST;

						ClearMultiDamage();
						pEntity->TraceAttack(pevInflictor, flAdjustedDamage, (tr.vecEndPos - vecSrc).Normalize(), &tr, bitsDamageType);
						ApplyMultiDamage(pevInflictor, pevAttacker);
					}
				}
			}
		}


		switch (RANDOM_LONG(1, 3))
		{
		case 1:
			MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
			WRITE_BYTE(TE_EXPLOSION);
			WRITE_COORD(pev->origin.x);
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z);
			WRITE_SHORT(MODEL_INDEX("sprites/plasmabomb.spr"));
			WRITE_BYTE(3);
			WRITE_BYTE(20);
			WRITE_BYTE(TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NOSOUND);
			MESSAGE_END();

			MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
			WRITE_BYTE(TE_EXPLOSION);
			WRITE_COORD(pev->origin.x);
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z);
			WRITE_SHORT(MODEL_INDEX("sprites/plasmabomb.spr"));
			WRITE_BYTE(3);
			WRITE_BYTE(20);
			WRITE_BYTE(TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NOSOUND);
			MESSAGE_END();
			break;
		case 2:
			MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
			WRITE_BYTE(TE_EXPLOSION);
			WRITE_COORD(pev->origin.x);
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z);
			WRITE_SHORT(MODEL_INDEX("sprites/plasmabomb.spr"));
			WRITE_BYTE(4);
			WRITE_BYTE(20);
			WRITE_BYTE(TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NOSOUND);
			MESSAGE_END();

			MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
			WRITE_BYTE(TE_EXPLOSION);
			WRITE_COORD(pev->origin.x);
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z);
			WRITE_SHORT(MODEL_INDEX("sprites/plasmabomb.spr"));
			WRITE_BYTE(4);
			WRITE_BYTE(20);
			WRITE_BYTE(TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NOSOUND);
			MESSAGE_END();
			break;
		case 3:
			MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
			WRITE_BYTE(TE_EXPLOSION);
			WRITE_COORD(pev->origin.x);
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z);
			WRITE_SHORT(MODEL_INDEX("sprites/plasmabomb.spr"));
			WRITE_BYTE(5);
			WRITE_BYTE(20);
			WRITE_BYTE(TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NOSOUND);
			MESSAGE_END();

			MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
			WRITE_BYTE(TE_EXPLOSION);
			WRITE_COORD(pev->origin.x);
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z);
			WRITE_SHORT(MODEL_INDEX("sprites/plasmabomb.spr"));
			WRITE_BYTE(5);
			WRITE_BYTE(20);
			WRITE_BYTE(TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NOSOUND);
			MESSAGE_END();
			break;
		}

		EMIT_SOUND_DYN(this->edict(), CHAN_WEAPON, "weapons/plasmagun_exp.wav", VOL_NORM, ATTN_NORM, 0, RANDOM_FLOAT(95, 100));

		return Remove();
	}

	void Init(Vector vecVelocity, float flTouchDamage, float flExplodeDamage, float flExplodeRadius, TeamName iTeam)
	{
		std::tie(m_flTouchDamage, m_flExplodeDamage, m_flExplodeRadius, m_iTeam) = std::make_tuple(flTouchDamage, flExplodeDamage, flExplodeRadius, iTeam);
		pev->velocity = std::move(vecVelocity);
	}

	int m_fSequenceLoops;
	float m_flAnimEndTime;
	float m_flMaxFrames;
	float m_flTouchDamage;
	float m_flExplodeDamage;
	float m_flExplodeRadius;
	TeamName m_iTeam;

protected:
	void Remove()
	{
		SetThink(nullptr);
		SetTouch(nullptr);
		pev->effects |= EF_NODRAW; // 0x80u
		return UTIL_Remove(this);
	}
};
#endif
#endif