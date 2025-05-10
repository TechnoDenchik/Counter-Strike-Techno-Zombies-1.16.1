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
	if (!stringtext1)
		stringtext1 = R_LoadTextureShared("resource/hud/zb5/videoframe_0", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext2)
		stringtext2 = R_LoadTextureShared("resource/hud/zb5/videoframe_2", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext3)
		stringtext3 = R_LoadTextureShared("resource/hud/zb5/videoframe_3", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext4)
		stringtext4 = R_LoadTextureShared("resource/hud/zb5/videoframe_4", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext5)
		stringtext5 = R_LoadTextureShared("resource/hud/zb5/videoframe_5", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext6)
		stringtext6 = R_LoadTextureShared("resource/hud/zb5/videoframe_6", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext7)
		stringtext7 = R_LoadTextureShared("resource/hud/zb5/videoframe_7", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext8)
		stringtext8 = R_LoadTextureShared("resource/hud/zb5/videoframe_8", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext9)
		stringtext9 = R_LoadTextureShared("resource/hud/zb5/videoframe_9", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext10)
		stringtext10 = R_LoadTextureShared("resource/hud/zb5/videoframe_10", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext11)
		stringtext11 = R_LoadTextureShared("resource/hud/zb5/videoframe_11", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext12)
		stringtext12 = R_LoadTextureShared("resource/hud/zb5/videoframe_12", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext13)
		stringtext13 = R_LoadTextureShared("resource/hud/zb5/videoframe_13", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext14)
		stringtext14 = R_LoadTextureShared("resource/hud/zb5/videoframe_14", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext15)
		stringtext15 = R_LoadTextureShared("resource/hud/zb5/videoframe_15", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext16)
		stringtext16 = R_LoadTextureShared("resource/hud/zb5/videoframe_16", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext17)
		stringtext17 = R_LoadTextureShared("resource/hud/zb5/videoframe_17", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext18)
		stringtext18 = R_LoadTextureShared("resource/hud/zb5/videoframe_18", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext19)
		stringtext19 = R_LoadTextureShared("resource/hud/zb5/videoframe_19", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext20)
		stringtext20 = R_LoadTextureShared("resource/hud/zb5/videoframe_20", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext21)
		stringtext21 = R_LoadTextureShared("resource/hud/zb5/videoframe_21", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext22)
		stringtext22 = R_LoadTextureShared("resource/hud/zb5/videoframe_22", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext23)
		stringtext23 = R_LoadTextureShared("resource/hud/zb5/videoframe_23", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext24)
		stringtext24 = R_LoadTextureShared("resource/hud/zb5/videoframe_24", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext25)
		stringtext25 = R_LoadTextureShared("resource/hud/zb5/videoframe_25", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext26)
		stringtext26 = R_LoadTextureShared("resource/hud/zb5/videoframe_26", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext27)
		stringtext27 = R_LoadTextureShared("resource/hud/zb5/videoframe_27", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext28)
		stringtext28 = R_LoadTextureShared("resource/hud/zb5/videoframe_28", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext29)
		stringtext29 = R_LoadTextureShared("resource/hud/zb5/videoframe_29", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext30)
		stringtext30 = R_LoadTextureShared("resource/hud/zb5/videoframe_30", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext31)
		stringtext31 = R_LoadTextureShared("resource/hud/zb5/videoframe_31", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext32)
		stringtext32 = R_LoadTextureShared("resource/hud/zb5/videoframe_32", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext33)
		stringtext33 = R_LoadTextureShared("resource/hud/zb5/videoframe_33", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext34)
		stringtext34 = R_LoadTextureShared("resource/hud/zb5/videoframe_34", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext35)
		stringtext35 = R_LoadTextureShared("resource/hud/zb5/videoframe_35", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext36)
		stringtext36 = R_LoadTextureShared("resource/hud/zb5/videoframe_36", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext37)
		stringtext37 = R_LoadTextureShared("resource/hud/zb5/videoframe_37", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext38)
		stringtext38 = R_LoadTextureShared("resource/hud/zb5/videoframe_38", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext39)
		stringtext39 = R_LoadTextureShared("resource/hud/zb5/videoframe_39", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext40)
		stringtext40 = R_LoadTextureShared("resource/hud/zb5/videoframe_40", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext41)
		stringtext41 = R_LoadTextureShared("resource/hud/zb5/videoframe_41", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext42)
		stringtext42 = R_LoadTextureShared("resource/hud/zb5/videoframe_42", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext43)
		stringtext43 = R_LoadTextureShared("resource/hud/zb5/videoframe_43", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext44)
		stringtext44 = R_LoadTextureShared("resource/hud/zb5/videoframe_44", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext45)
		stringtext45 = R_LoadTextureShared("resource/hud/zb5/videoframe_45", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext46)
		stringtext46 = R_LoadTextureShared("resource/hud/zb5/videoframe_46", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext47)
		stringtext47 = R_LoadTextureShared("resource/hud/zb5/videoframe_47", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext48)
		stringtext48 = R_LoadTextureShared("resource/hud/zb5/videoframe_48", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext49)
		stringtext49 = R_LoadTextureShared("resource/hud/zb5/videoframe_49", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext50)
		stringtext50 = R_LoadTextureShared("resource/hud/zb5/videoframe_50", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext51)
		stringtext51 = R_LoadTextureShared("resource/hud/zb5/videoframe_51", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext52)
		stringtext52 = R_LoadTextureShared("resource/hud/zb5/videoframe_52", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext53)
		stringtext53 = R_LoadTextureShared("resource/hud/zb5/videoframe_53", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext54)
		stringtext54 = R_LoadTextureShared("resource/hud/zb5/videoframe_54", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext55)
		stringtext55 = R_LoadTextureShared("resource/hud/zb5/videoframe_55", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext56)
		stringtext56 = R_LoadTextureShared("resource/hud/zb5/videoframe_56", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext57)
		stringtext57 = R_LoadTextureShared("resource/hud/zb5/videoframe_57", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext58)
		stringtext58 = R_LoadTextureShared("resource/hud/zb5/videoframe_58", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext59)
		stringtext59 = R_LoadTextureShared("resource/hud/zb5/videoframe_59", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext60)
		stringtext60 = R_LoadTextureShared("resource/hud/zb5/videoframe_60", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext61)
		stringtext61 = R_LoadTextureShared("resource/hud/zb5/videoframe_61", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext62)
		stringtext62 = R_LoadTextureShared("resource/hud/zb5/videoframe_62", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext63)
		stringtext63 = R_LoadTextureShared("resource/hud/zb5/videoframe_63", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	
	return 1;
}

int CHudWebm::Draw(float time)
{
	if (!m_pCurTexture1)
		return 1;

	int x = ScreenWidth / 1.995;
	int y = ScreenHeight / 1.4;
	int y2 = ScreenHeight / 1.4;

	const float flScale = 0.0f;
	const int r = 153, g = 97, b = 7;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);

	char szbuffer[64];

	DrawUtils::DrawHudString(x - 190, y2 - 32, ScreenWidth, szbuffer, r, g, b, flScale);

	return 1;
}

void CHudWebm::Settext()
{
	m_pCurTexture1 = stringtext1;
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