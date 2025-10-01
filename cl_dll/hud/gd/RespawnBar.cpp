/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"
#include "RespawnBar.h"

#include "string.h"
#include "assert.h"
#include <numeric>
#include <tuple>

int CHudGDRespawnBar::VidInit(void)
{
	if (!stringtext1)
		stringtext1 = R_LoadTextureShared("resource/hud/gd/respawnbar/respawn_bg", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext2)
		stringtext2 = R_LoadTextureShared("resource/hud/gd/respawnbar/respawn_string", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	return 1;
}

int CHudGDRespawnBar::Draw(float time)
{
	if (!m_pCurTexture)
		return 1;

	if (barstring == 245)
	{
		m_pCurTexture = false;
		return 1;
	}

	int x = ScreenWidth / 1.995;
	int y = ScreenHeight / 1.40;

	int x18 = ScreenWidth / 1.995;
	int y18 = ScreenHeight / 1.45;

	int w = 335;
	int h = 38;

	int h2 = 245;

	const float flScale = 0.0f;
	const int r = 255, g = 255, b = 255;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);

	if (barstring < 245)
	{
		barstring++;
	}

	stringtext1->Bind();
	DrawUtils::Draw2DQuadScaled(x18 - 500 / 2, y18 - 38, x18 + 500 / 2, y18 + 28);

	stringtext2->Bind();
	DrawUtils::Draw2DQuadScaled(x - 490 / 2, y - 38, x + barstring, y - 26);

	return 1;
}

void CHudGDRespawnBar::SetWebm()
{
	timetx = 0;

	barstring = -245;

	m_pCurTexture = true;
	m_flDisplayTime = gHUD.m_flTime;
}