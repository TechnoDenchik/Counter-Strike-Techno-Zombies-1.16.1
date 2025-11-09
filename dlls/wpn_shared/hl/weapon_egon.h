#ifndef WPN_AK47_H
#define WPN_AK47_H
#ifdef _WIN32
#pragma once
#endif

#ifndef CLIENT_DLL
#include "effects.h"
#include "customentity.h"
#include "monsters.h"
#endif

#include <vector>
#include <array>

class CBeam;

class CEgon : public CBasePlayerWeapon
{
public:
#ifndef CLIENT_DLL
	static	TYPEDESCRIPTION m_SaveData[];
#endif

	void Spawn() override;
	void Precache() override;
	int iItemSlot() override { return 4; }
	int GetItemInfo(ItemInfo* p) override;
	void Reload() override;

	BOOL Deploy() override;
	void Holster(int skiplocal = 0) override;

	void UpdateEffect(const Vector& startPoint, const Vector& endPoint, float timeBlend);

	void CreateEffect(void);
	void DestroyEffect(void);

	void EndAttack(void);
	void Attack(void);
	void PrimaryAttack() override;
	void WeaponIdle() override;

	float m_flAmmoUseTime;// since we use < 1 point of ammo per update, we subtract ammo on a timer.

	float GetPulseInterval(void);
	float GetDischargeInterval(void);

	void Fire(const Vector& vecOrigSrc, const Vector& vecDir);

	BOOL HasAmmo(void);

	enum EGON_FIREMODE { FIRE_NARROW, FIRE_WIDE };

	CBeam* m_pBeam;
	CBeam* m_pNoise;
	CSprite* m_pSprite;

	virtual BOOL UseDecrement() override
	{
#if defined( CLIENT_WEAPONS )
		return TRUE;
#else
		return FALSE;
#endif
	}

	unsigned short m_usEgonStop;

private:
	float				m_shootTime;
	EGON_FIREMODE		m_fireMode;
	float				m_shakeTime;
	BOOL				m_deployed;

	unsigned short m_usEgonFire;
};

#endif