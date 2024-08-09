#pragma once

//hegrenade
#define HEGRENADE_MAX_SPEED		250
#define HEGRENADE_MAX_SPEED_SHIELD	180

class CZombieBomb : public CBasePlayerWeapon
{
	enum zombiebomb_e
    {
        HEGRENADE_IDLE,
        HEGRENADE_PULLPIN,
        HEGRENADE_THROW,
        HEGRENADE_DRAW
    };
public:
	virtual void Spawn();
	virtual void Precache();
	virtual int GetItemInfo(ItemInfo *p);
	virtual BOOL CanDeploy();
	virtual BOOL CanDrop() { return FALSE; }
	virtual BOOL CanHolster();
	virtual BOOL Deploy();
	const char* GetCSModelName() override { return "models/w_zombibomb.mdl"; }
	virtual void Holster(int skiplocal);
	virtual float GetMaxSpeed() { return m_fMaxSpeed; }
	virtual int iItemSlot() { return GRENADE_SLOT; }
	virtual void PrimaryAttack();
	virtual void SecondaryAttack();
	virtual void WeaponIdle();
	virtual BOOL UseDecrement()
	{
#ifdef CLIENT_WEAPONS
		return TRUE;
#else
		return FALSE;
#endif
	}

public:
	bool ShieldSecondaryFire(int iUpAnim, int iDownAnim);
	void SetPlayerShieldAnim();
	void ResetPlayerShieldAnim();

public:
	unsigned short m_usCreateExplosion;
};