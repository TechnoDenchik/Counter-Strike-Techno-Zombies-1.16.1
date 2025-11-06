/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"
#include "RespawnBarH.h"

#include "string.h"
#include "assert.h"
#include <numeric>
#include <tuple>

int CHudHidRespawnBar::VidInit(void)
{
	if (!m_pBackground)
		m_pBackground = R_LoadTextureShared("resource/hud/gd/respawnbar/respawn_bg", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!m_pFillTexture)
		m_pFillTexture = R_LoadTextureShared("resource/hud/gd/respawnbar/respawn_string", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!m_pTextBg)
		m_pTextBg = R_LoadTextureShared("resource/shelterteam/zsht_ingame_timertext_bg", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	return 1;
}

int CHudHidRespawnBar::Draw(float time)
{
	if (!m_bActive)
		return 1;

	float flProgress = 1.0f - ((m_flRespawnEndTime - time) / m_flRespawnDuration);
	flProgress = std::max(0.0f, std::min(1.0f, flProgress));

	int x = ScreenWidth / 2;
	int y = ScreenHeight - 293; // Отступ от низа

	int x2 = ScreenWidth / 1.995;
	int y2 = ScreenHeight - 243;

	int iW = m_pBackground->w();
	int iH = m_pBackground->h();

	iW = m_pBackground->w();
	iH = m_pBackground->h();

	int iX = 0;
	int iY = ScreenHeight - 4;

	iX = ScreenWidth / 2.580;
	iY = ScreenHeight - 264;

	const int PROGRESS_WIDTH = 419;
	const int PROGRESS_HEIGHT = 8;
	const int BG_WIDTH = PROGRESS_WIDTH + 20;
	const int BG_HEIGHT = PROGRESS_HEIGHT + 12;

	int bg1_x = x - BG_WIDTH / 2;
	int bg1_y = y - BG_HEIGHT / 2;

	int bg2_x = x - PROGRESS_WIDTH / 2;
	int bg2_y = y - PROGRESS_HEIGHT / 2;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);

	m_pBackground->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);

	m_pTextBg->Bind();
	DrawUtils::Draw2DQuadScaled(x2 - 450 / 2, y2 - 38, x2 + 450 / 2, y2 - 9 );

	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);

	if (m_pFillTexture)
	{
		m_pFillTexture->Bind();

		int fillWidth = (int)(PROGRESS_WIDTH * flProgress);
		if (fillWidth > 0)
		{
			DrawUtils::Draw2DQuadScaled(bg2_x, bg2_y, bg2_x + fillWidth, bg2_y + PROGRESS_HEIGHT);
		}
	}

	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 200);
	DrawUtils::DrawHudString(x2 - 180 , y2 - 33 , ScreenWidth, "Вы неуязвимы в течении 3-х секунд после возрождения", 255, 255, 255, 200, 0.0f);

	if (flProgress >= 1.0f)
	{
		m_bActive = false;
	}

	return 1;
}

void CHudHidRespawnBar::StartRespawn(float flDuration)
{
	m_bActive = true;
	m_flRespawnDuration = flDuration;
	m_flRespawnEndTime = gHUD.m_flTime + flDuration;
}

void CHudHidRespawnBar::Reset()
{
	m_bActive = false;
	m_flRespawnEndTime = 0;
	m_flRespawnDuration = 0;
}

void CHudHidRespawnBar::SetWebm()
{
	StartRespawn(5.0f);
}