#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"
#include "OrigWinhud.h"

#include "string.h"
#include "assert.h"
#include <numeric>
#include <tuple>

int CHudOrigWins::VidInit(void)
{
	if (!winct)
		winct = R_LoadTextureShared("resource/hud/base/ctwin", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!wint)
		wint = R_LoadTextureShared("resource/hud/base/trwin", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	return 1;
}

int CHudOrigWins::Draw(float time)
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
	const int r = 255, g = 255, b = 255;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255 * std::min(5.0f - (time - m_flDisplayTime), 1.0f));
	m_pCurTexture->Bind();

	DrawUtils::Draw2DQuadScaled(x - 881 / 3.0, y - 3.5, x + 881 / 3.0, y + 120);
	return 1;
}

void CHudOrigWins::winsct()
{
	m_pCurTexture = winct;
	m_flDisplayTime = gHUD.m_flTime;
}

void CHudOrigWins::winst()
{
	m_pCurTexture = wint;
	m_flDisplayTime = gHUD.m_flTime;
}