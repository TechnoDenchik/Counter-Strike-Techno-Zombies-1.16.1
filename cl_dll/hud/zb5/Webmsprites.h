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

	void SetName(int names)
	{
		m_iPlayerNum = names;
	}

	duration_t tNextsecond1;
	time_point_t timesecond1;
	duration_t tDeltasecond1;

	signed barstring = -150;

protected:
	int skin;
	int m_iPlayerNum = 0;
	int m_iPlayerNum2;
	bool m_pCurTexture;
	SharedTexture stringtext;
	SharedTexture stringtext2;
	SharedTexture stringtext3;
	SharedTexture stringtext4;
	float m_flDisplayTime;
};