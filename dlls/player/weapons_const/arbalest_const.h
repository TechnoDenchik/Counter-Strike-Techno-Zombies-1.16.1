/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */
#ifndef ARB_UPDATE_CLIP
#define ARB_UPDATE_CLIP
#ifdef _WIN32
#pragma once
#endif

class CArbClip : public BasePlayerExtra
{
public:
	CArbClip(CBasePlayer *player);
	void UpdateDay();
	void spawn();
	void Reset();
	void UpdateHUD() const;

protected:
	int clipin;
};

#endif