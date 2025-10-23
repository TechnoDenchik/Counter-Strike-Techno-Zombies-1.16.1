/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"
#include "ZbGaugeBar.h"

#include "string.h"
#include "assert.h"
#include <numeric>
#include <tuple>

int CHudZB5GaugeBar::VidInit(void)
{
	if (!m_pBackground)
		m_pBackground = R_LoadTextureShared("resource/zb5/zb5_zombie_gaugebar_bg", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!m_pBackground2)
		m_pBackground2 = R_LoadTextureShared("resource/zb5/zb5_zombie_gaugebar", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!m_pFillTexture)
		m_pFillTexture = R_LoadTextureShared("resource/zb5/zb5_zombie_gaugebar_red", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	return 1;
}

int CHudZB5GaugeBar::Draw(float time)
{
	if (!m_bActive)
		return 1;

	float flProgress = 1.0f - ((m_flRespawnEndTime - time) / m_flRespawnDuration);

	if (flProgress < 0.0f) flProgress = 0.0f;
	if (flProgress > 1.0f) flProgress = 1.0f;

	int x = ScreenWidth / 2.130; // центрируем 100px ширину
	int y = ScreenHeight - 297;   // отступ от низа

	int iW = m_pBackground->w();
	int iH = m_pBackground->h();

	iW = m_pBackground->w();
	iH = m_pBackground->h();

	int iX = 0;
	int iY = ScreenHeight - 4;

	iX = ScreenWidth / 2.310;
	iY = ScreenHeight - 265;

	int iW2 = m_pBackground2->w();
	int iH2 = m_pBackground2->h();

	iW2 = m_pBackground2->w();
	iH2 = m_pBackground2->h();

	int iX2 = 0;
	int iY2 = ScreenHeight - 4;

	iX2 = ScreenWidth / 2.220;
	iY2 = ScreenHeight - 280;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);

	m_pBackground->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
	m_pBackground2->Draw2DQuadScaled(iX2, iY2 - iH2, iX2 + iW2, iY2 - iH2 + iH2);
	
	if (m_pFillTexture)
	{
		m_pFillTexture->Bind();

		// ¬ычисл€ем ширину заполнени€ based on progress
		int fillWidth = (int)(100.0f * flProgress);
		if (fillWidth > 0)
		{
			DrawUtils::Draw2DQuadScaled(x, y, x + fillWidth, y + 7);
		}
	}

	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 200);
	gEngfuncs.pTriAPI->RenderMode(kRenderTransAdd);

	char szText[32];
	float secondsLeft = m_flRespawnEndTime - time;
	sprintf(szText, "%.2f", secondsLeft);

	DrawUtils::DrawHudString(x + 110, y - 5, ScreenWidth, szText, 255, 0, 0, 255, 0.0f);
	DrawUtils::DrawHudString(x - 2, y - 18, ScreenWidth, "¬рем€ до возрождени€", 255, 0, 0, 255, 0.0f);

	if (flProgress >= 1.0f)
	{
		m_bActive = false;
	}

	return 1;
}

void CHudZB5GaugeBar::StartRespawn(float flDuration)
{
	m_bActive = true;
	m_flRespawnDuration = flDuration;
	m_flRespawnEndTime = gHUD.m_flTime + flDuration;
}

void CHudZB5GaugeBar::Reset()
{
	m_bActive = false;
	m_flRespawnEndTime = 0;
	m_flRespawnDuration = 0;
}

void CHudZB5GaugeBar::SetWebm()
{
	StartRespawn(5.0f);
}