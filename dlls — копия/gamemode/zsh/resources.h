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

	void touch();
	void Reset();
	void UpdateHUD() const;
	//void UpdateHUD2() const;

protected:

	int maxwoods;
	int maxmetal;
	int woods;
	int metal;
};

#endif