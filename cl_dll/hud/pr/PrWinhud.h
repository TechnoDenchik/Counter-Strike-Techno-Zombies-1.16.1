#pragma once

#include "hud_sub.h"
#include "r_texture.h"

class CHudWinhudPR : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void WinHuman();
	void WinZombie();

protected:
	SharedTexture iconwinhm;
	SharedTexture iconwinzb;
	SharedTexture m_pCurTexture;
	float m_flDisplayTime;
};

class CHudRespZombiePR : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void RespZ();

protected:
	SharedTexture ZombieResp;
	SharedTexture m_pCurTexture;
	float m_flDisplayTime;
};

class CHudInfectionPR : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void infected();

protected:
	SharedTexture infect;
	SharedTexture m_pCurTexture;
	float m_flDisplayTime;
};

class CHudMakeZombiesPR : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void Make();

protected:
	SharedTexture Zombieclassic;
	SharedTexture m_pCurTexture;
	float m_flDisplayTime;
};

