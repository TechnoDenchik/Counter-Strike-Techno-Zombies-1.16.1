/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"
#include "Webmsprites.h"

#include "string.h"
#include "assert.h"
#include <numeric>
#include <tuple>

inline void BuildNumberRC(wrect_t(&rgrc)[10], int w, int h)
{
	int nw = 0;

	for (int i = 0; i < 10; i++)
	{
		rgrc[i].left = nw;
		rgrc[i].top = 0;
		rgrc[i].right = rgrc[i].left + w;
		rgrc[i].bottom = h;

		nw += w;
	}
}

inline void BuildNumberRC(wrect_t(&rgrc)[10], int tex)
{
	int w = gRenderAPI.RenderGetParm(PARM_TEX_SRC_WIDTH, tex);
	int h = gRenderAPI.RenderGetParm(PARM_TEX_SRC_HEIGHT, tex);
	return BuildNumberRC(rgrc, w / 10, h);
}

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

inline int DrawTexturedNumbersTopRightAligned(const CTextureRef& tex, const wrect_t(&rect)[10], int iNumber, int x, int y, float scale = 1.0f)
{
	assert(iNumber >= 0);

	do
	{
		int k = iNumber % 10;
		iNumber /= 10;
		DrawTexturePart(tex, rect[k], x, y, scale);
		x -= (rect[k].right - rect[k].left) * scale;
	} while (iNumber > 0);

	return x;
}

inline unsigned math_log10(unsigned v)
{
	return (v >= 1000000000) ? 9 : (v >= 100000000) ? 8 : (v >= 10000000) ? 7 :
		(v >= 1000000) ? 6 : (v >= 100000) ? 5 : (v >= 10000) ? 4 :
		(v >= 1000) ? 3 : (v >= 100) ? 2 : (v >= 10) ? 1 : 0;
}

inline int DrawTexturedNumbersTopCenterAligned(const CTextureRef& tex, const wrect_t(&rect)[10], int iNumber, int x, int y, float scale = 1.0f)
{
	int n = math_log10(iNumber);
	x += (rect[0].right - rect[0].left) * (n - 1) * scale * 0.5f;
	return DrawTexturedNumbersTopRightAligned(tex, rect, iNumber, x, y, scale);
}

int CHudWebm::VidInit(void)
{
	if (!stringtext)
		stringtext = R_LoadTextureShared("resource/zb5/zb5_evolution_bg", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!stringtext2)
		stringtext2 = R_LoadTextureShared("resource/zb5/meatwall", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!stringtext3)
		stringtext3 = R_LoadTextureShared("resource/zb5/deathknight", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!stringtext4)
		stringtext4 = R_LoadTextureShared("resource/zb5/spider", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	return 1;
}

int CHudWebm::Draw(float time)
{
	if (!m_pCurTexture)
		return 1;

	if (time > m_flDisplayTime + 8.00f)
	{
		m_pCurTexture = false;
		return 1;
	}

	int x = ScreenWidth / 12.995;
	int y = ScreenHeight / 3.5;

	int x2 = ScreenWidth / 15.295;
	int y2 = ScreenHeight / 3.0;

	int x3 = ScreenWidth / 8.395;
	int y3 = ScreenHeight / 3.220;

	const float flScale = 0.0f;

	char szbuffer[64];
	int id = gEngfuncs.GetLocalPlayer()->index;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);

	if (barstring < 150)
	{
		barstring++;
		barstring++;
		barstring++;
		barstring++;
		barstring++;
		barstring++;
	}

	if (skin == 1)
	{
		stringtext->Bind();
		DrawUtils::Draw2DQuadScaled(x - 300 / 2, y - 38, x + barstring, y + 74);

		if (barstring == 150)
		{
			stringtext2->Bind();
			DrawUtils::Draw2DQuadScaled(x2 - 200 / 2, y2 - 38, x2 + 200 / 2, y2 + 8);

			sprintf(szbuffer, g_PlayerInfoList[m_iPlayerNum].name);
			DrawUtils::DrawHudString(x3 - 190, y3 - 32, ScreenWidth, szbuffer, 255, 255, 255, flScale);
		}
	}
	else if (skin == 2)
	{
		stringtext->Bind();
		DrawUtils::Draw2DQuadScaled(x - 300 / 2, y - 38, x + barstring, y + 74);

		if (barstring == 150)
		{
			stringtext3->Bind();
			DrawUtils::Draw2DQuadScaled(x2 - 200 / 2, y2 - 38, x2 + 200 / 2, y2 + 8);

			sprintf(szbuffer, g_PlayerInfoList[m_iPlayerNum].name);
			DrawUtils::DrawHudString(x3 - 190, y3 - 32, ScreenWidth, szbuffer, 255, 255, 255, flScale);
		}
	}
	else if (skin == 3)
	{
		stringtext->Bind();
		DrawUtils::Draw2DQuadScaled(x - 300 / 2, y - 38, x + barstring, y + 74);

		if (barstring == 150)
		{
			stringtext4->Bind();
			DrawUtils::Draw2DQuadScaled(x2 - 200 / 2, y2 - 38, x2 + 200 / 2, y2 + 8);

			sprintf(szbuffer, g_PlayerInfoList[m_iPlayerNum].name);
			DrawUtils::DrawHudString(x3 - 190, y3 - 32, ScreenWidth, szbuffer, 255, 255, 255, flScale);
		}
	}

	return 1;
}

void CHudWebm::Settext()
{
	barstring = -150;
	m_pCurTexture = true;
	m_flDisplayTime = gHUD.m_flTime;
}