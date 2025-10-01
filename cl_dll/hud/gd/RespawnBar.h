/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#pragma once

#include "hud_sub.h"
#include "r_texture.h"

class CHudGDRespawnBar : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void SetWebm();

protected:
	bool m_pCurTexture;

	SharedTexture stringtext1;
	SharedTexture stringtext2;

	int timetx;
	signed barstring = -245;
	float m_flDisplayTime;
};