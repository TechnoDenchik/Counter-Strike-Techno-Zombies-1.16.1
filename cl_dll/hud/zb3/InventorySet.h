#pragma once

#include "hud_sub.h"
#include "r_texture.h"

class CInventorySet : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void SetWeaponKit();

	char szbuffer[64];
protected:
	SharedTexture m_pCurTexture;
	SharedTexture stringtext;
	float m_flDisplayTime;
};