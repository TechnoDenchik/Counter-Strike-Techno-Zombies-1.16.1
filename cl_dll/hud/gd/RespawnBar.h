/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#pragma once

#include "hud_sub.h"
#include "r_texture.h"

class CHudGDRespawnBar : public IBaseHudSub
{
public:
	int Init(void);
	int VidInit(void) override;
	int Draw(float time) override;

	void SetWebm();

	void StartRespawn(float flDuration);
	void Reset();
protected:
	bool m_pCurTexture;

	SharedTexture stringtext1;
	SharedTexture stringtext2;

	int timetx;
	signed barstring = -245;
	float m_flDisplayTime;
private:
	bool m_bActive;
	float m_flRespawnEndTime;
	float m_flRespawnDuration;

	SharedTexture m_pBackground;
	SharedTexture m_pFillTexture;
	SharedTexture m_pTextBg;
};