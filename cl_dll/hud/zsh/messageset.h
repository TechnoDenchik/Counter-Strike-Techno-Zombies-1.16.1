/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */
#pragma once

#include "hud_sub.h"
#include "r_texture.h"

class CHudTextZSH : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void Settext();

	void renaining(int iCountDown)
	{
		times = iCountDown;
	}
protected:
	int times;
	int names;
	SharedTexture m_pCurTexture;
	SharedTexture stringtext;
	float m_flDisplayTime;
};

class CHudTextDayZSH : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;
	void Settext();

	void nexting(int nextday)
	{
		nextdays = nextday;
	}
protected:
	int nextdays;
	SharedTexture m_pCurTexture;
	SharedTexture stringtext;
	float m_flDisplayTime;
};

class CHudWinhudZSH : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void SurvivalWin();
	void SurvivalLose();

protected:
	SharedTexture iconwinhm;
	SharedTexture iconwinzb;
	SharedTexture m_pCurTexture;
	float m_flDisplayTime;
};