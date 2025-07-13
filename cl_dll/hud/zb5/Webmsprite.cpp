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

	int x = ScreenWidth / 1.995;
	int y = ScreenHeight / 1.4;
	int y2 = ScreenHeight / 1.4;

	const float flScale = 0.0f;



	//w - горизонталь(ширина)
	//h - вертикаль(высота)
	//start -		//end - 1940.18
	int w = 1940.18;  int w2 = 335;
	int h = 668;	  int h2 = 38;

	int w3 = 335;  int w4 = 335;
	int h3 = 38;     int h4 = 38;

	int w5 = 335;  int w6 = 335;
	int h5 = 38;	 int h6 = 38;

	int w7 = 335;  int w8 = 335;
	int h7 = 38;	 int h8 = 38;

	int w9 = 335;  int w10 = 335;
	int h9 = 38;	 int h10 = 38;

	int w11 = 335;  int w12 = 335;
	int h11 = 38;	 int h12 = 38;

	int w13 = 335;  int w14 = 335;
	int h13 = 38;	 int h14 = 38;

	int w15 = 335;  int w16 = 335;
	int h15 = 38;	 int h16 = 38;

	int w17 = 335;  int w18 = 335;
	int h17 = 38;	 int h18 = 38;

	int w19 = 335;  int w20 = 335;
	int h19 = 38;	 int h20 = 38;

	int w21 = 335;  int w22 = 335;
	int h21 = 38;	 int h22 = 38;

	int w23 = 335;  int w24 = 335;
	int h23 = 38;	 int h24 = 38;

	int w25 = 335;  int w26 = 335;
	int h25 = 38;	 int h26 = 38; 

	int w27 = 335;  int w28 = 335;
	int h27 = 38;	 int h28 = 38;

	int w29 = 335;  int w30 = 335;
	int h29 = 38;	 int h30 = 38;

	char szbuffer[64];
	int id = gEngfuncs.GetLocalPlayer()->index;
	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);

	if (gHUD.m_flTime - timesecond1 < 1.0f)
	{
		tDeltasecond1 += gHUD.m_flTime - timesecond1;
	}
	if (tNextsecond1 > 0.024f || (gHUD.m_flTime - timesecond1 > 0.024f) || tDeltasecond1 > 0.024f)
	{
		tNextsecond1 = 0.0f;
		tDeltasecond1 = 0.0f;

		if (timetx1 < 30)
		{
			timetx1++;
		}
	}
	timesecond1 = gHUD.m_flTime;

	if (timetx1 > 1)
	{
		if (skin == 1)
		{
			
			stringtext->Bind();
			DrawUtils::Draw2DQuadScaled(x - 600 / 2, y - 38, x + 600 / 2, y - 8);

			stringtext2->Bind();
			DrawUtils::Draw2DQuadScaled(x - 600 / 2, y - 38, x + 600 / 2, y - 8);
			
		}
		else if (skin == 2)
		{
			
			stringtext->Bind();
			DrawUtils::Draw2DQuadScaled(x - 300 / 2, y - 38, x + 300 / 2, y + 58);

			stringtext3->Bind();
			DrawUtils::Draw2DQuadScaled(x - 100 / 2, y - 38, x + 100 / 2, y + 28);
			

			//sprintf(szbuffer, g_PlayerInfoList[id].name);
			//DrawUtils::DrawHudString(x - 190, y2 - 32, ScreenWidth, szbuffer, 255, 255, 255, flScale);
		}
		else if (skin == 3)
		{
			stringtext->Bind();
			DrawUtils::Draw2DQuadScaled(x - 600 / 2, y - 38, x + 600 / 2, y - 8);

			stringtext4->Bind();
			DrawUtils::Draw2DQuadScaled(x - 600 / 2, y - 38, x + 600 / 2, y - 8);
		}
	}
	else if (timetx1 == 2)
	{

	}
	else if (timetx1 == 2)
	{

	}
	else if (timetx1 == 2)
	{

	}
	else if (timetx1 == 2)
	{

	}
	else if (timetx1 == 2)
	{

	}
	else if (timetx1 == 2)
	{

	}
	else if (timetx1 == 2)
	{

	}
	else if (timetx1 == 2)
	{

	}
	else if (timetx1 == 2)
	{

	}
	else if (timetx1 == 2)
	{

	}
	else if (timetx1 == 2)
	{

	}
	else if (timetx1 == 2)
	{

	}
	else if (timetx1 == 2)
	{

	}
	else if (timetx1 == 2)
	{

	}
	else if (timetx1 == 2)
	{

	}
	else if (timetx1 == 2)
	{

	}
	else if (timetx1 == 2)
	{

	}
	else if (timetx1 == 2)
	{

	}
	else if (timetx1 == 2)
	{

	}
	else if (timetx1 == 2)
	{

	}
	else if (timetx1 == 2)
	{

	}
	else if (timetx1 == 2)
	{

	}
	else if (timetx1 == 2)
	{

	}
	else if (timetx1 == 2)
	{

	}
	else if (timetx1 == 2)
	{

	}
	else if (timetx1 == 2)
	{

	}
	else if (timetx1 == 2)
	{

	}
	else if (timetx1 == 2)
	{

	}
	else if (timetx1 == 2)
	{

	}

	return 1;
}

void CHudWebm::Settext()
{
	m_pCurTexture = stringtext;
	m_flDisplayTime = gHUD.m_flTime;
}

int CHudWebm2::VidInit(void)
{
	if (!stringtext)
		stringtext = R_LoadTextureShared("resource/hud/zb3/hud_string_bg", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	return 1;
}

int CHudWebm2::Draw(float time)
{
	if (!m_pCurTexture)
		return 1;

	if (time > m_flDisplayTime + 3.00f)
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
	DrawUtils::Draw2DQuadScaled(x - 600 / 2, y - 38, x + 600 / 2, y - 8);

	char szbuffer[64];
	sprintf(szbuffer, "Навык снова можно будет использовать в следующем раунде");

	DrawUtils::DrawHudString(x - 210, y2 - 32, ScreenWidth, szbuffer, r, g, b, flScale);

	return 1;
}

void CHudWebm2::Settext()
{
	m_pCurTexture = stringtext;
	m_flDisplayTime = gHUD.m_flTime;
}