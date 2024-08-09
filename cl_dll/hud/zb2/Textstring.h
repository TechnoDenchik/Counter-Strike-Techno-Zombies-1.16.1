#pragma once

#include "hud_sub.h"
#include "r_texture.h"

class CHudTextZB1 : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void Settext();

	void renaining(int time)
	{
		times = time;
	}
protected:
	int times;
	int names;
	SharedTexture m_pCurTexture;
	SharedTexture stringtext;
	float m_flDisplayTime;
};

