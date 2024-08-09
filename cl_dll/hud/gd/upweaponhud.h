/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#pragma once

#include "hud_sub.h"
#include "r_texture.h"

class CHudGDUpdateWeapon : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void weaponups();
	void weaponupsmax();

protected:
	SharedTexture m_pCurTexture;
	SharedTexture weaponup;
	SharedTexture weaponupmax;
	float m_flDisplayTime;
};