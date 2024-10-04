#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"
#include "Textset2.h"

#include "string.h"
#include "assert.h"
#include <numeric>
#include <tuple>

inline void DrawTexturePart(const CTextureRef& tex, const wrect_t& rect, int x1, int y1, float scale = 1.0f)
{
	tex.Bind();

	float w = tex.w();
	float h = tex.h();

	x1 *= gHUD.m_flScale;
	y1 *= gHUD.m_flScale;
	scale *= gHUD.m_flScale;

	int x2 = x1 + (rect.right - rect.left) * scale;
	int y2 = y1 + (rect.bottom - rect.top) * scale;

	gEngfuncs.pTriAPI->Begin(TRI_QUADS);
	gEngfuncs.pTriAPI->TexCoord2f(rect.left / w, rect.top / h);
	gEngfuncs.pTriAPI->Vertex3f(x1, y1, 0);
	gEngfuncs.pTriAPI->TexCoord2f(rect.left / w, rect.bottom / h);
	gEngfuncs.pTriAPI->Vertex3f(x1, y2, 0);
	gEngfuncs.pTriAPI->TexCoord2f(rect.right / w, rect.bottom / h);
	gEngfuncs.pTriAPI->Vertex3f(x2, y2, 0);
	gEngfuncs.pTriAPI->TexCoord2f(rect.right / w, rect.top / h);
	gEngfuncs.pTriAPI->Vertex3f(x2, y1, 0);
	gEngfuncs.pTriAPI->End();
}

int CHudTextZB1::VidInit(void)
{
	if (!stringtext)
		stringtext = R_LoadTextureShared("resource/hud/zb3/hud_string_bg", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	return 1;
}

int CHudTextZB1::Draw(float time)
{
	if (!m_pCurTexture)
		return 1;

	if (time > m_flDisplayTime + 1.02f)
	{
		m_pCurTexture = nullptr;
		return 1;
	}

	int x = ScreenWidth / 1.995;
	int y = ScreenHeight / 1.4;
	int y2 = ScreenHeight / 1.4;

	const float flScale = 0.0f;
	const int r = 153, g = 97, b = 7;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255 * std::min(5.0f - (time - m_flDisplayTime), 1.0f));

	stringtext->Bind();
	DrawUtils::Draw2DQuadScaled(x - 500 / 2, y - 38, x + 500 / 2, y - 8);

	char szbuffer[64];

	if (times >= 5)
	{
		sprintf(szbuffer, "Время до появления зомби: %d секунд", times);
	}
	else if (times == 4)
	{
		sprintf(szbuffer, "Время до появления зомби: %d секунды", times);
	}
	else if (times == 3)
	{
		sprintf(szbuffer, "Время до появления зомби: %d секунды", times);
	}
	else if (times == 2)
	{
		sprintf(szbuffer, "Время до появления зомби: %d секунды", times);
	}
	else
	{
		sprintf(szbuffer, "Время до появления зомби: %d секунда", times);
	}

	DrawUtils::DrawHudString(x - 110, y2 - 32, ScreenWidth, szbuffer, r, g, b, flScale);

	return 1;
}

void CHudTextZB1::Settext()
{
	m_pCurTexture = stringtext;
	m_flDisplayTime = gHUD.m_flTime;
}