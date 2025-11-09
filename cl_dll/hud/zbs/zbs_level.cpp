#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"

#include "zbs.h"
#include "zbs_level.h"

#include "string.h"

CHudZBSLevel::CHudZBSLevel(void)
{
	m_iLevel_HP = m_iLevel_ATK = m_iLevel_Wall = 0;
}

int CHudZBSLevel::VidInit(void)
{
	
	//m_iFlags |= HUD_DRAW;

	m_iLevel_HP = m_iLevel_ATK = m_iLevel_Wall = 1;
	return 1;
}

int CHudZBSLevel::Draw(float time)
{
	int x = ScreenWidth - 2012;
	int y = ScreenHeight - 210;
	int x2 = ScreenWidth - 1950;
	int y2 = ScreenHeight - 210;
	const float flScale = 0.0f;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransAlpha);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);

	const int r = 255, g = 255, b = 255;

	char szBuffer[64];


	if (m_iLevel_HP >= 100)
		sprintf(szBuffer, "Lv. MAX");
	else
		sprintf(szBuffer, "Lv. %d", m_iLevel_HP);
	DrawUtils::DrawHudString(x + 100, y + 5, ScreenWidth, szBuffer, r, g, b, 255, flScale);

	// Wall Level
	if (m_iLevel_Wall)
	{
		y -= 32;

		DrawUtils::DrawHudString(x + 20, y + 5, ScreenWidth, "Durability", r, g, b, 255, flScale);
		if (m_iLevel_Wall >= 100)
			sprintf(szBuffer, "Lv. MAX");
		else
			sprintf(szBuffer, "Lv. %d", m_iLevel_Wall);
		DrawUtils::DrawHudString(x + 100, y + 5, ScreenWidth, szBuffer, r, g, b, 255, flScale);
	}


	return 1;
}

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

int CHudZBSMsgLevel::VidInit(void)
{
	if (!stringtext)
		stringtext = R_LoadTextureShared("resource/shelterteam/zsht_ingame_timertext_bg", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	return 1;
}

int CHudZBSMsgLevel::Draw(float time)
{
	if (!m_pCurTexture)
		return 1;

	if (time > m_flDisplayTime + 2.0f)
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
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255 - std::min(5.0f - (time - m_flDisplayTime), 1.0f));

	m_pCurTexture->Bind();
	DrawUtils::Draw2DQuadScaled(x - 600 / 2, y - 38, x + 600 / 2, y - 8);

	char szbuffer[64];

	sprintf(szbuffer, "Уровень: %d", lvl);

	DrawUtils::DrawHudString(x - 35, y2 - 32, ScreenWidth, szbuffer, r, g, b, 255, flScale);

	return 1;
}

void CHudZBSMsgLevel::Setlv()
{
  	m_pCurTexture = stringtext;
	m_flDisplayTime = gHUD.m_flTime;
}