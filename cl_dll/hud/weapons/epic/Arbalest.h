#pragma once

#include "hud_sub.h"
#include "r_texture.h"

class CHudArbalest : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void Settext();

	void renaining(int m_iClip_c)
	{
		times = m_iClip_c;
	}

protected:
	int times;
	int names;
	SharedTexture m_pCurTexture;
	SharedTexture stringtext;
	wrect_t m_rcAmmoclip[10];
	float m_flDisplayTime;
};