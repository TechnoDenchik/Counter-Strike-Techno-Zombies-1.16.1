#ifndef ZSH_UPDATE_RESOURCES
#define ZSH_UPDATE_RESOURCES
#ifdef _WIN32
#pragma once
#endif

class ZSHUpdateRes : public BasePlayerExtra
{
public:
	ZSHUpdateRes(CBasePlayer* player);

	void UpdateWoods();
	void UpdateMetal();

	float DamageWood() const;
	float DamageMeat() const;

	void touch();
	void Reset();
	void UpdateHUD() const;
	//void UpdateHUD2() const;

protected:

	int maxwoods = 15;
	int maxmetal = 13;
	int woods;
	int metal;
	int m_iAttack1;
	int m_iAttack2;
};

#endif