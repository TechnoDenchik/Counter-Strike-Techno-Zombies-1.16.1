/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#pragma once

#include "hud_sub.h"
#include "r_texture.h"

class CHudWebm : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void Settext();

	void renaining(int evoskin)
	{
		skin = evoskin;
	}

	void SetName(char* names)
	{
		named = names;
	}

	duration_t tNextsecond1;
	time_point_t timesecond1;
	duration_t tDeltasecond1;

	time_point_t timetx1;

protected:
	int skin;
	char* named;

	SharedTexture m_pCurTexture;
	SharedTexture stringtext;
	SharedTexture stringtext2;
	SharedTexture stringtext3;
	SharedTexture stringtext4;
	float m_flDisplayTime;
};

class CHudWebm2 : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void Settext();

protected:
	SharedTexture m_pCurTexture;
	SharedTexture stringtext;
	float m_flDisplayTime;
};