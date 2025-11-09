#pragma once

#include "hud_sub.h"
#include "r_texture.h"

class CInventorySetZb5 : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void SetWeaponKit();
};

class CHeroSetZb5 : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void SetHero();

protected:
	SharedTexture m_pCurTexture;
	SharedTexture stringtext;
	float m_flDisplayTime;
};