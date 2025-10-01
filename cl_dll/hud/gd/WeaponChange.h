/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#pragma once

#include "hud_sub.h"
#include "r_texture.h"

class CHudGDWeapomChangeWEBM : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void SetWebm();

protected:
	bool m_pCurTexture;

	SharedTexture stringtext0;
	SharedTexture stringtext1;
	SharedTexture stringtext2;
	SharedTexture stringtext3;
	SharedTexture stringtext4;
	SharedTexture stringtext5;
	SharedTexture stringtext6;
	SharedTexture stringtext7;
	SharedTexture stringtext8;
	SharedTexture stringtext9;
	SharedTexture stringtext10;
	SharedTexture stringtext11;
	SharedTexture stringtext12;
	SharedTexture stringtext13;
	SharedTexture stringtext14;
	SharedTexture stringtext15;
	SharedTexture stringtext16;
	SharedTexture stringtext17;
	SharedTexture stringtext18;
	SharedTexture stringtext19;
	SharedTexture stringtext20;
	SharedTexture stringtext21;
	SharedTexture stringtext22;
	SharedTexture stringtext23;

	duration_t tNextsecond;
	time_point_t timesecond;
	duration_t tDeltasecond;

	int timetx;
	float m_flDisplayTime;
};