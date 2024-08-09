#ifndef ZSH_UPDATE_DAY
#define ZSH_UPDATE_DAY
#ifdef _WIN32
#pragma once
#endif

class ZSHUpdateDay : public BasePlayerExtra
{
public:
	ZSHUpdateDay(CBasePlayer *player);
	void UpdateDay();
	void spawn();
	void Reset();
	void UpdateHUD() const;

protected:
	int dayses;
};

#endif