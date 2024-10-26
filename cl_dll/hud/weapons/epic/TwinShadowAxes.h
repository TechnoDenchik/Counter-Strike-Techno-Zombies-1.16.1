#pragma once

#include "hud_sub.h"
#include "r_texture.h"

class CHudTwinAxes : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void Setint();

protected:
	int smoke;
	CClientSprite ef_dgaxe_smoke;
	HSPRITE ef_dgaxe_smoke2;
	SharedTexture m_pCurTexture;
	SharedTexture winct;
	SharedTexture wint;
	float m_flDisplayTime;
};