#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"
#include "PRWinhud.h"
#include "string.h"
#include "assert.h"
#include <numeric>
#include <tuple>

int CHudWinhudPR::VidInit(void)
{
	if (!iconwinhm)
		iconwinhm = R_LoadTextureShared("resource/hud/zb3/humanwin", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!iconwinzb)
		iconwinzb = R_LoadTextureShared("resource/hud/zb3/zombiewin", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	return 1;
}

int CHudWinhudPR::Draw(float time)
{
	if (!m_pCurTexture)
		return 1;

	if (time > m_flDisplayTime + 5.0f)
	{
		m_pCurTexture = nullptr;
		return 1;
	}

	int x = ScreenWidth / 2;
	int y = ScreenHeight / 4;
	const float flScale = 0.0f;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255 * std::min(5.0f - (time - m_flDisplayTime), 1.0f));
	m_pCurTexture->Bind();
	DrawUtils::Draw2DQuadScaled(x - 881 / 3.0, y - 3.5, x + 881 / 3.0, y + 140);
	return 1;
}

void CHudWinhudPR::WinHuman()
{
	m_pCurTexture = iconwinhm;
	m_flDisplayTime = gHUD.m_flTime;
}

void CHudWinhudPR::WinZombie()
{
	m_pCurTexture = iconwinzb;
	m_flDisplayTime = gHUD.m_flTime;
}

int CHudInfectionPR::VidInit(void)
{
	if (!infect)
		infect = R_LoadTextureShared("resource/zb3/infection", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	return 1;
}

int CHudInfectionPR::Draw(float time)
{
	if (!m_pCurTexture)
		return 1;

	if (time > m_flDisplayTime + 1.0f)
	{
		m_pCurTexture = nullptr;
		return 1;
	}

	int x = ScreenWidth / 2;
	int y = ScreenHeight / 4;
	const float flScale = 0.0f;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255 * std::min(1.0f - (time - m_flDisplayTime), 1.0f));
	m_pCurTexture->Bind();
	DrawUtils::Draw2DQuadScaled(x - 881 , y - 15.5, x + 881, y + 587);
	return 1;
}

void CHudInfectionPR::infected()
{
	m_pCurTexture = infect;
	m_flDisplayTime = gHUD.m_flTime;
}

int CHudMakeZombiesPR::VidInit(void)
{
	if (!Zombieclassic)
		Zombieclassic = R_LoadTextureShared("resource/zombieenhance/zombie_nomal", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	return 1;
}

int CHudMakeZombiesPR::Draw(float time)
{
	if (!m_pCurTexture)
		return 1;

	if (time > m_flDisplayTime + 9999.0f)
	{
		m_pCurTexture = nullptr;
		return 1;
	}
	if (g_PlayerExtraInfo[gHUD.m_Scoreboard.m_iPlayerNum].teamnumber == TEAM_CT)
	{
		int x = ScreenWidth / 40.25;
		int y = ScreenHeight / 1.1380;
		const float flScale = 0.0f;

		gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
		gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
		Zombieclassic->Bind();
		DrawUtils::Draw2DQuadScaled(x - 60, y - 15.5, x + 60, y + 100);
	}
	return 1;
}

void CHudMakeZombiesPR::Make()
{
	m_pCurTexture = Zombieclassic;
	m_flDisplayTime = gHUD.m_flTime;
}

int CHudRespZombiePR::VidInit(void)
{
	if (!ZombieResp)
		ZombieResp = R_LoadTextureShared("resource/zombieenhance/zombie_nomal", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	return 1;
}

int CHudRespZombiePR::Draw(float time)
{
	if (!m_pCurTexture)
		return 1;

	if (time > m_flDisplayTime + 9999.0f)
	{
		m_pCurTexture = nullptr;
		return 1;
	}
	
	int x = ScreenWidth / 40.25;
	int y = ScreenHeight / 1.1380;
	const float flScale = 0.0f;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
	ZombieResp->Bind();
	DrawUtils::Draw2DQuadScaled(x - 60, y - 15.5, x + 60, y + 100);
	
	return 1;
}

void CHudRespZombiePR::RespZ()
{
	m_pCurTexture = ZombieResp;
	m_flDisplayTime = gHUD.m_flTime;
}