
#pragma once

#include "hud_sub.h"
#include "r_texture.h"

class CHudZBSLevel : public IBaseHudSub
{
public:
	CHudZBSLevel();
	int VidInit(void) override;
	int Draw(float time) override;
	void UpdateLevel(int hp, int att, int wall) 
	{
		m_iLevel_HP = hp;
		m_iLevel_ATK = att;
		m_iLevel_Wall = wall;
	}

protected:
	UniqueTexture textbg;
	UniqueTexture m_pTexture_ZBSBoard_BG_Wall;

	int m_iLevel_HP;
	int m_iLevel_ATK;
	int m_iLevel_Wall;
};

class CHudZBSMsgLevel : public IBaseHudSub
{
public:

	int VidInit(void) override;
	int Draw(float time) override;
	void Setlv();
	void UpdateLevel(int lv)
	{
		lvl = lv;		
	}

protected:
	SharedTexture m_pCurTexture;
	SharedTexture stringtext;
	float m_flDisplayTime;

	int lvl;
};