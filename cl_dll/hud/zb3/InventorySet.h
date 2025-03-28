#pragma once

#include "hud_sub.h"
#include "r_texture.h"

class CInventorySet : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void SetWeaponKit();

	void GetWeapon(char* gun, char* pistol, char* knife, char* grenade)
	{
		Gun = gun;
		Pistol = gun;
		Knife = gun;
		Grenade = gun;
	}
	char szbuffer[64];
protected:
	char* Gun;
	char* Pistol;
	char* Knife;
	char* Grenade;
	SharedTexture m_pCurTexture;
	SharedTexture stringtext;
	float m_flDisplayTime;
};