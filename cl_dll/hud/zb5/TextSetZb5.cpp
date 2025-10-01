/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"
#include "TextSetZb5.h"
#include "gamemode/mods_const.h"

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

int CHudTextZB5::VidInit(void)
{
	if (!stringtext)
		stringtext = R_LoadTextureShared("resource/hud/zb3/hud_string_bg", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	return 1;
}

int CHudTextZB5::Draw(float time)
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

	if (times >= 5)
	{
		sprintf(szbuffer, "Навык снова можно будет использовать через: %d секунд", times);
	}
	else if (times == 4)
	{
		sprintf(szbuffer, "Навык снова можно будет использовать через: %d секунды", times);
	}
	else if (times == 3)
	{
		sprintf(szbuffer, "Навык снова можно будет использовать через: %d секунды", times);
	}
	else if (times == 2)
	{
		sprintf(szbuffer, "Навык снова можно будет использовать через: %d секунды", times);
	}
	else if (times == 1)
	{
		sprintf(szbuffer, "Навык снова можно будет использовать через: %d секунда", times);
	}
	else
	{
		sprintf(szbuffer, "Навык снова можно будет использовать через: %d секунд", times);
	}

	DrawUtils::DrawHudString(x - 190, y2 - 32, ScreenWidth, szbuffer, r, g, b, flScale);

	return 1;
}

void CHudTextZB5::Settext()
{
	m_pCurTexture = stringtext;
	m_flDisplayTime = gHUD.m_flTime;
}

int CHudText2ZB5::VidInit(void)
{
	if (!stringtext)
		stringtext = R_LoadTextureShared("resource/hud/zb3/hud_string_bg", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	return 1;
}

int CHudText2ZB5::Draw(float time)
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

void CHudText2ZB5::Settext()
{
	m_pCurTexture = stringtext;
	m_flDisplayTime = gHUD.m_flTime;
}

int CHudTextNum::VidInit(void)
{
	if (!stringtext)
		stringtext = R_LoadTextureShared("resource/hud/zb3/hud_string_bg", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	R_InitTexture(texnumsmallh, "resource/zb5/zb5_level_num_human");
	R_InitTexture(texnumsmallz, "resource/zb5/zb5_level_num_zombie");

	R_InitTexture(texhealthsmallh, "resource/zb5/zb5_health_text_num");
	R_InitTexture(levelhumanbg, "resource/zb5/zb5_level_bg_human");
	R_InitTexture(levelzombiebg, "resource/zb5/zb5_level_bg_zombie");

	R_InitTexture(levelhuman, "resource/zb5/zb5_level_bg_human_ani");
	R_InitTexture(levelzombie, "resource/zb5/zb5_level_bg_zombie_ani");

	R_InitTexture(leveliconh, "resource/zb5/zb5_level_icon_human_virus");
	R_InitTexture(leveliconz, "resource/zb5/zb5_level_icon_zombie_virus");

	R_InitTexture(iconlvh, "resource/zb5/zb5_level_text_human");
	R_InitTexture(iconlvz, "resource/zb5/zb5_level_text_zombie");

	R_InitTexture(buttonvh, "resource/zb5/zb5_level_text_icon_human");
	R_InitTexture(buttonvz, "resource/zb5/zb5_level_text_icon_zombie");

	R_InitTexture(iconmaxlvh, "resource/zb5/zb5_max_text_human");
	R_InitTexture(iconmaxlvz, "resource/zb5/zb5_max_text_zombie");

	R_InitTexture(iconxh, "resource/zb5/zb5_level_text_icon_human_x");
	R_InitTexture(iconxz, "resource/zb5/zb5_level_text_icon_zombie_x");

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

	BuildNumberRC(m_rctexnumsmallh, 15, 23);
	BuildNumberRC(m_rctexnumsmallz, 15, 23);

	BuildNumberRC(m_rctexhealthsmallh, 12, 16);

	return 1;
}

int CHudTextNum::Draw(float time)
{
	int x = ScreenWidth / 1.995;
	int y = ScreenHeight / 1.4;
	int y2 = ScreenHeight / 1.4;

	int x18 = ScreenWidth / 1.995;
	int y18 = ScreenHeight / 1.45;

	int iW2 = levelhumanbg->w();
	int iH2 = levelhumanbg->h();

	iW2 = levelhumanbg->w();
	iH2 = levelhumanbg->h();

	int iX2 = 0;
	int iY2 = ScreenHeight - 4;

	iX2 = ScreenWidth;
	iY2 = ScreenHeight - 4;

	int iW3 = levelzombiebg->w();
	int iH3 = levelzombiebg->h();

	iW3 = levelzombiebg->w();
	iH3 = levelzombiebg->h();

	int iX3 = 0;
	int iY3 = ScreenHeight - 4;

	iX3 = ScreenWidth;
	iY3 = ScreenHeight - 4;

	int w = 335;
	int h = 38;

	int h2 = 245;

	int iW4 = levelhumanbg->w();
	int iH4 = levelhumanbg->h();

	iW4 = levelhumanbg->w();
	iH4 = levelhumanbg->h();

	int iX4 = 0;
	int iY4 = ScreenHeight - 4;

	iX4 = ScreenWidth;
	iY4 = ScreenHeight - 4;

	int iW5 = buttonvh->w();
	int iH5 = buttonvh->h();

	iW5 = buttonvh->w();
	iH5 = buttonvh->h();

	int iX5 = 0;
	int iY5 = ScreenHeight - 4;

	iX5 = ScreenWidth;
	iY5 = ScreenHeight - 4;

	int iW6 = buttonvz->w();
	int iH6 = buttonvz->h();

	iW6 = buttonvz->w();
	iH6 = buttonvz->h();

	int iX6 = 0;
	int iY6 = ScreenHeight - 4;

	iX6 = ScreenWidth;
	iY6 = ScreenHeight - 4;

	int iW7 = iconxh->w();
	int iH7 = iconxh->h();

	iW7 = iconxh->w();
	iH7 = iconxh->h();

	int iX7 = 0;
	int iY7 = ScreenHeight - 4;

	iX7 = ScreenWidth;
	iY7 = ScreenHeight - 4;

	int iW8 = iconxz->w();
	int iH8 = iconxz->h();

	iW8 = iconxz->w();
	iH8 = iconxz->h();

	int iX8 = 0;
	int iY8 = ScreenHeight - 4;

	iX8 = ScreenWidth;
	iY8 = ScreenHeight - 4;

	int iW9 = leveliconh->w();
	int iH9 = leveliconh->h();

	iW9 = leveliconh->w();
	iH9 = leveliconh->h();

	int iX9 = 0;
	int iY9 = ScreenHeight - 4;

	iX9 = ScreenWidth;
	iY9 = ScreenHeight - 4;

	int iW10 = leveliconz->w();
	int iH10 = leveliconz->h();

	iW10 = leveliconz->w();
	iH10 = leveliconz->h();

	int iX10 = 0;
	int iY10 = ScreenHeight - 4;

	iX10 = ScreenWidth;
	iY10 = ScreenHeight - 4;

	int iW11 = iconmaxlvh->w();
	int iH11 = iconmaxlvh->h();

	iW11 = iconmaxlvh->w();
	iH11 = iconmaxlvh->h();

	int iX11 = 0;
	int iY11 = ScreenHeight - 4;

	iX11 = ScreenWidth;
	iY11 = ScreenHeight - 4;

	int iW12 = iconmaxlvz->w();
	int iH12 = iconmaxlvz->h();

	iW12 = iconmaxlvz->w();
	iH12 = iconmaxlvz->h();

	int iX12 = 0;
	int iY12 = ScreenHeight - 4;

	iX12 = ScreenWidth;
	iY12 = ScreenHeight - 4;

	const float flScale = 0.0f;
	const int r = 153, g = 97, b = 7;

	int idx = gEngfuncs.GetLocalPlayer()->index;

	if (gHUD.m_iModRunning != MOD_ZB5)
		return 0;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);

	if (gHUD.m_flTime - timesecond < 1.0f)
	{
		tDeltasecond += gHUD.m_flTime - timesecond;
	}
	if (tNextsecond > 0.024f || (gHUD.m_flTime - timesecond > 0.024f) || tDeltasecond > 0.024f)
	{
		tNextsecond = 0.0f;
		tDeltasecond = 0.0f;

		if (timetx < 62)
		{
			timetx++;
		}
	}
	timesecond = gHUD.m_flTime;

	if (g_PlayerExtraInfo[idx].zombie)
	{
		gEngfuncs.pTriAPI->Color4ub(255, 0, 0, 255);

		if (evo > 9)
		{
			if (timetx == 1)
			{
				stringtext1->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 2)
			{
				stringtext2->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);

			}
			else if (timetx == 3)
			{
				stringtext3->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 4)
			{
				stringtext4->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 5)
			{
				stringtext5->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 6)
			{
				stringtext6->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 7)
			{
				stringtext7->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 8)
			{
				stringtext8->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 9)
			{
				stringtext9->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 10)
			{
				stringtext10->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 11)
			{
				stringtext11->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 12)
			{
				stringtext12->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 13)
			{
				stringtext13->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 14)
			{
				stringtext14->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 15)
			{
				stringtext15->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 16)
			{
				stringtext16->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 17)
			{
				stringtext17->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 18)
			{
				stringtext18->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 19)
			{
				stringtext19->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 20)
			{
				stringtext20->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 21)
			{
				stringtext21->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 22)
			{
				stringtext22->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 23)
			{
				stringtext23->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 24)
			{
				stringtext24->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 25)
			{
				stringtext25->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 26)
			{
				stringtext26->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 27)
			{
				stringtext27->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 28)
			{
				stringtext28->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 29)
			{
				stringtext29->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 30)
			{
				stringtext30->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 31)
			{
				stringtext31->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 32)
			{
				stringtext32->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 33)
			{
				stringtext33->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 34)
			{
				stringtext34->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 35)
			{
				stringtext35->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 36)
			{
				stringtext36->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 37)
			{
				stringtext37->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 38)
			{
				stringtext38->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 39)
			{
				stringtext39->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 40)
			{
				stringtext40->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 41)
			{
				stringtext41->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 42)
			{
				stringtext42->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 43)
			{
				stringtext43->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 44)
			{
				stringtext44->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 45)
			{
				stringtext45->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 46)
			{
				stringtext46->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 47)
			{
				stringtext47->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 48)
			{
				stringtext48->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 49)
			{
				stringtext49->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 50)
			{
				stringtext50->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 51)
			{
				stringtext51->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 52)
			{
				stringtext52->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 53)
			{
				stringtext53->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 54)
			{
				stringtext54->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 55)
			{
				stringtext55->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 56)
			{
				stringtext56->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 57)
			{
				stringtext57->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 58)
			{
				stringtext58->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 59)
			{
				stringtext59->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 60)
			{
				stringtext60->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 61)
			{
				stringtext61->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 62)
			{
				stringtext62->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
				timetx = -0;
			}
			gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);

			iW6 = buttonvz->w();
			iH6 = buttonvz->h();
			iX6 = ScreenWidth / 2.063;
			iY6 = ScreenHeight - 135;

			buttonvz->Draw2DQuadScaled(iX6, iY6 - iH6, iX6 + iW6, iY6 - iH6 + iH6);
		}
		gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);

		iW3 = levelzombiebg->w();
		iH3 = levelzombiebg->h();
		iX3 = ScreenWidth / 2.120;
		iY3 = ScreenHeight - 170;

		levelzombiebg->Draw2DQuadScaled(iX3, iY3 - iH3, iX3 + iW3, iY3 - iH3 + iH3);
	}
	else
	{
		gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);

		if (evo > 2)
		{
			if (timetx == 1)
			{
				stringtext1->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 2)
			{
				stringtext2->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);

			}
			else if (timetx == 3)
			{
				stringtext3->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 4)
			{
				stringtext4->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 5)
			{
				stringtext5->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 6)
			{
				stringtext6->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 7)
			{
				stringtext7->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 8)
			{
				stringtext8->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 9)
			{
				stringtext9->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 10)
			{
				stringtext10->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 11)
			{
				stringtext11->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 12)
			{
				stringtext12->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 13)
			{
				stringtext13->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 14)
			{
				stringtext14->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 15)
			{
				stringtext15->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 16)
			{
				stringtext16->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 17)
			{
				stringtext17->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 18)
			{
				stringtext18->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 19)
			{
				stringtext19->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 20)
			{
				stringtext20->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 21)
			{
				stringtext21->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 22)
			{
				stringtext22->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 23)
			{
				stringtext23->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 24)
			{
				stringtext24->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 25)
			{
				stringtext25->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 26)
			{
				stringtext26->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 27)
			{
				stringtext27->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 28)
			{
				stringtext28->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 29)
			{
				stringtext29->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 30)
			{
				stringtext30->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 31)
			{
				stringtext31->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 32)
			{
				stringtext32->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 33)
			{
				stringtext33->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 34)
			{
				stringtext34->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 35)
			{
				stringtext35->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 36)
			{
				stringtext36->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 37)
			{
				stringtext37->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 38)
			{
				stringtext38->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 39)
			{
				stringtext39->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 40)
			{
				stringtext40->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 41)
			{
				stringtext41->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 42)
			{
				stringtext42->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 43)
			{
				stringtext43->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 44)
			{
				stringtext44->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 45)
			{
				stringtext45->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 46)
			{
				stringtext46->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 47)
			{
				stringtext47->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 48)
			{
				stringtext48->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 49)
			{
				stringtext49->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 50)
			{
				stringtext50->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 51)
			{
				stringtext51->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 52)
			{
				stringtext52->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 53)
			{
				stringtext53->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 54)
			{
				stringtext54->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 55)
			{
				stringtext55->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 56)
			{
				stringtext56->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 57)
			{
				stringtext57->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 58)
			{
				stringtext58->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 59)
			{
				stringtext59->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 60)
			{
				stringtext60->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 61)
			{
				stringtext61->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
			}
			else if (timetx == 62)
			{
				stringtext62->Bind();
				DrawUtils::Draw2DQuadScaled(x18 - w / static_cast<float>(2), y18 - h, x18 + w / static_cast<float>(2), y18 + h2);
				timetx = -0;
			}

			iW5 = buttonvh->w();
			iH5 = buttonvh->h();
			iX5 = ScreenWidth / 2.063;
			iY5 = ScreenHeight - 135;

			buttonvh->Draw2DQuadScaled(iX5, iY5 - iH5, iX5 + iW5, iY5 - iH5 + iH5);
		}

		iW2 = levelhumanbg->w();
		iH2 = levelhumanbg->h();
		iX2 = ScreenWidth / 2.120;
		iY2 = ScreenHeight - 170;

		levelhumanbg->Draw2DQuadScaled(iX2, iY2 - iH2, iX2 + iW2, iY2 - iH2 + iH2);
	}

	if (g_PlayerExtraInfo[idx].zombie)
	{
		if(evo < 10)
		{
			iW8 = iconxz->w();
			iH8 = iconxz->h();
			iX8 = ScreenWidth / 2.002;
			iY8 = ScreenHeight - 226;

			iconxz->Draw2DQuadScaled(iX8, iY8 - iH8, iX8 + iW8, iY8 - iH8 + iH8);

			iW10 = leveliconz->w();
			iH10 = leveliconz->h();
			iX10 = ScreenWidth / 2.056;
			iY10 = ScreenHeight - 218;

			leveliconz->Draw2DQuadScaled(iX10, iY10 - iH10, iX10 + iW10, iY10 - iH10 + iH10);


			DrawTexturedNumbersTopRightAligned(*texnumsmallz, m_rctexnumsmallz, evo, x + 8, y + 67, 1.0f);
		}
		else
		{
			iW12 = iconmaxlvz->w();
			iH12 = iconmaxlvz->h();
			iX12 = ScreenWidth / 2.067;
			iY12 = ScreenHeight - 216;

			iconmaxlvz->Draw2DQuadScaled(iX12, iY12 - iH12, iX12 + iW12, iY12 - iH12 + iH12);
		}
	}
	else
	{
		if (evo < 10)
		{
			iW7 = iconxh->w();
			iH7 = iconxh->h();
			iX7 = ScreenWidth / 2.002;
			iY7 = ScreenHeight - 226;

			iconxh->Draw2DQuadScaled(iX7, iY7 - iH7, iX7 + iW7, iY7 - iH7 + iH7);

			iW9 = leveliconh->w();
			iH9 = leveliconh->h();
			iX9 = ScreenWidth / 2.056;
			iY9 = ScreenHeight - 218;

			leveliconh->Draw2DQuadScaled(iX9, iY9 - iH9, iX9 + iW9, iY9 - iH9 + iH9);

			DrawTexturedNumbersTopRightAligned(*texnumsmallh, m_rctexnumsmallh, evo, x + 8, y + 67, 1.0f);
		}
		else
		{
			iW11 = iconmaxlvh->w();
			iH11 = iconmaxlvh->h();
			iX11 = ScreenWidth / 2.067;
			iY11 = ScreenHeight - 216;

			iconmaxlvh->Draw2DQuadScaled(iX11, iY11 - iH11, iX11 + iW11, iY11 - iH11 + iH11);
		}
	}

	return 1;
}

int CHudKillZB5::VidInit(void)
{
	R_InitTexture(texnumbigh, "resource/zb5/zb5_kill_num_human");
	R_InitTexture(texnumbigz, "resource/zb5/zb5_kill_num_zombie");

	BuildNumberRC(m_rctexnumbigh, 44, 37);
	BuildNumberRC(m_rctexnumbigz, 44, 37);

	if (!stringtext1)
		stringtext1 = R_LoadTextureShared("resource/hud/zb5/kill/videoframe_1", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext2)
		stringtext2 = R_LoadTextureShared("resource/hud/zb5/kill/videoframe_2", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext3)
		stringtext3 = R_LoadTextureShared("resource/hud/zb5/kill/videoframe_3", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext4)
		stringtext4 = R_LoadTextureShared("resource/hud/zb5/kill/videoframe_4", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext5)
		stringtext5 = R_LoadTextureShared("resource/hud/zb5/kill/videoframe_5", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext6)
		stringtext6 = R_LoadTextureShared("resource/hud/zb5/kill/videoframe_6", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext7)
		stringtext7 = R_LoadTextureShared("resource/hud/zb5/kill/videoframe_7", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext8)
		stringtext8 = R_LoadTextureShared("resource/hud/zb5/kill/videoframe_8", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext9)
		stringtext9 = R_LoadTextureShared("resource/hud/zb5/kill/videoframe_9", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext10)
		stringtext10 = R_LoadTextureShared("resource/hud/zb5/kill/videoframe_10", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext11)
		stringtext11 = R_LoadTextureShared("resource/hud/zb5/kill/videoframe_11", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext12)
		stringtext12 = R_LoadTextureShared("resource/hud/zb5/kill/videoframe_12", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext13)
		stringtext13 = R_LoadTextureShared("resource/hud/zb5/kill/videoframe_13", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext14)
		stringtext14 = R_LoadTextureShared("resource/hud/zb5/kill/videoframe_14", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext15)
		stringtext15 = R_LoadTextureShared("resource/hud/zb5/kill/videoframe_15", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext16)
		stringtext16 = R_LoadTextureShared("resource/hud/zb5/kill/videoframe_16", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext17)
		stringtext17 = R_LoadTextureShared("resource/hud/zb5/kill/videoframe_17", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext18)
		stringtext18 = R_LoadTextureShared("resource/hud/zb5/kill/videoframe_18", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext19)
		stringtext19 = R_LoadTextureShared("resource/hud/zb5/kill/videoframe_19", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext20)
		stringtext20 = R_LoadTextureShared("resource/hud/zb5/kill/videoframe_20", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext21)
		stringtext21 = R_LoadTextureShared("resource/hud/zb5/kill/videoframe_21", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext22)
		stringtext22 = R_LoadTextureShared("resource/hud/zb5/kill/videoframe_22", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext23)
		stringtext23 = R_LoadTextureShared("resource/hud/zb5/kill/videoframe_23", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext24)
		stringtext24 = R_LoadTextureShared("resource/hud/zb5/kill/videoframe_24", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext25)
		stringtext25 = R_LoadTextureShared("resource/hud/zb5/kill/videoframe_25", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext26)
		stringtext26 = R_LoadTextureShared("resource/hud/zb5/kill/videoframe_26", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext27)
		stringtext27 = R_LoadTextureShared("resource/hud/zb5/kill/videoframe_27", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext28)
		stringtext28 = R_LoadTextureShared("resource/hud/zb5/kill/videoframe_28", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext29)
		stringtext29 = R_LoadTextureShared("resource/hud/zb5/kill/videoframe_29", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext30)
		stringtext30 = R_LoadTextureShared("resource/hud/zb5/kill/videoframe_30", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext31)
		stringtext31 = R_LoadTextureShared("resource/hud/zb5/kill/videoframe_31", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!stringtextz1)
		stringtextz1 = R_LoadTextureShared("resource/hud/zb5/killz/videoframe_1", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtextz2)
		stringtextz2 = R_LoadTextureShared("resource/hud/zb5/killz/videoframe_2", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtextz3)
		stringtextz3 = R_LoadTextureShared("resource/hud/zb5/killz/videoframe_3", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtextz4)
		stringtextz4 = R_LoadTextureShared("resource/hud/zb5/killz/videoframe_4", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtextz5)
		stringtextz5 = R_LoadTextureShared("resource/hud/zb5/killz/videoframe_5", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtextz6)
		stringtextz6 = R_LoadTextureShared("resource/hud/zb5/killz/videoframe_6", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtextz7)
		stringtextz7 = R_LoadTextureShared("resource/hud/zb5/killz/videoframe_7", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtextz8)
		stringtextz8 = R_LoadTextureShared("resource/hud/zb5/killz/videoframe_8", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtextz9)
		stringtextz9 = R_LoadTextureShared("resource/hud/zb5/killz/videoframe_9", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtextz10)
		stringtextz10 = R_LoadTextureShared("resource/hud/zb5/killz/videoframe_10", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtextz11)
		stringtextz11 = R_LoadTextureShared("resource/hud/zb5/killz/videoframe_11", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtextz12)
		stringtextz12 = R_LoadTextureShared("resource/hud/zb5/killz/videoframe_12", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtextz13)
		stringtextz13 = R_LoadTextureShared("resource/hud/zb5/killz/videoframe_13", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtextz14)
		stringtextz14 = R_LoadTextureShared("resource/hud/zb5/killz/videoframe_14", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtextz15)
		stringtextz15 = R_LoadTextureShared("resource/hud/zb5/killz/videoframe_15", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtextz16)
		stringtextz16 = R_LoadTextureShared("resource/hud/zb5/killz/videoframe_16", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtextz17)
		stringtextz17 = R_LoadTextureShared("resource/hud/zb5/killz/videoframe_17", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtextz18)
		stringtextz18 = R_LoadTextureShared("resource/hud/zb5/killz/videoframe_18", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtextz19)
		stringtextz19 = R_LoadTextureShared("resource/hud/zb5/killz/videoframe_19", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtextz20)
		stringtextz20 = R_LoadTextureShared("resource/hud/zb5/killz/videoframe_20", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtextz21)
		stringtextz21 = R_LoadTextureShared("resource/hud/zb5/killz/videoframe_21", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtextz22)
		stringtextz22 = R_LoadTextureShared("resource/hud/zb5/killz/videoframe_22", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtextz23)
		stringtextz23 = R_LoadTextureShared("resource/hud/zb5/killz/videoframe_23", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtextz24)
		stringtextz24 = R_LoadTextureShared("resource/hud/zb5/killz/videoframe_24", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtextz25)
		stringtextz25 = R_LoadTextureShared("resource/hud/zb5/killz/videoframe_25", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtextz26)
		stringtextz26 = R_LoadTextureShared("resource/hud/zb5/killz/videoframe_26", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtextz27)
		stringtextz27 = R_LoadTextureShared("resource/hud/zb5/killz/videoframe_27", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtextz28)
		stringtextz28 = R_LoadTextureShared("resource/hud/zb5/killz/videoframe_28", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtextz29)
		stringtextz29 = R_LoadTextureShared("resource/hud/zb5/killz/videoframe_29", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtextz30)
		stringtextz30 = R_LoadTextureShared("resource/hud/zb5/killz/videoframe_30", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtextz31)
		stringtextz31 = R_LoadTextureShared("resource/hud/zb5/killz/videoframe_31", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtextz32)
		stringtextz32 = R_LoadTextureShared("resource/hud/zb5/killz/videoframe_32", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	return 1;
}

int CHudKillZB5::Draw(float time)
{
	int x = ScreenWidth / 1.995;
	int y = ScreenHeight / 1.4;
	int y2 = ScreenHeight / 1.4;

	int x18 = ScreenWidth / 1.995;
	int y18 = ScreenHeight / 1.45;


	int iW = stringtext1->w();
	int iH = stringtext1->h();

	iW = stringtext1->w();
	iH = stringtext1->h();

	int iX = 0;
	int iY = ScreenHeight - 1.45;

	iX = ScreenWidth;
	iY = ScreenHeight - 1.45;

	double w = 4.5;

	int h = 212;

	//int w2 = 1;
	int h2 = 355;

	const float flScale = 0.0f;
	const int r = 153, g = 97, b = 7;
	int idx = gEngfuncs.GetLocalPlayer()->index;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);

	if (go == true)
	{
		if (gHUD.m_flTime - timesecond2 < 1.0f)
		{
			tDeltasecond2 += gHUD.m_flTime - timesecond2;
		}
		if (tNextsecond2 > 0.0264f || (gHUD.m_flTime - timesecond2 > 0.0264f) || tDeltasecond2 > 0.0264f)
		{
			tNextsecond2 = 0.0f;
			tDeltasecond2 = 0.0f;

			if (timetx2 < 32)
			{
				timetx2++;
			}
		}
		timesecond2 = gHUD.m_flTime;

		if (g_PlayerExtraInfo[idx].zombie)
		{

			if (timetx2 == 1)
			{
				iW = stringtextz1->w();
				iH = stringtextz1->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;
				gEngfuncs.pTriAPI->Color4ub(255, 0, 0, 255);
				stringtextz1->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
			}
			else if (timetx2 == 2)
			{
				iW = stringtextz2->w();
				iH = stringtextz2->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;
				gEngfuncs.pTriAPI->Color4ub(255, 0, 0, 255);
				stringtextz2->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);

			}
			else if (timetx2 == 3)
			{
				iW = stringtextz3->w();
				iH = stringtextz3->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;
				gEngfuncs.pTriAPI->Color4ub(255, 0, 0, 255);
				stringtextz3->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
			}
			else if (timetx2 == 4)
			{
				iW = stringtextz4->w();
				iH = stringtextz4->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;
				gEngfuncs.pTriAPI->Color4ub(255, 0, 0, 255);
				stringtextz4->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
			}
			else if (timetx2 == 5)
			{
				iW = stringtextz5->w();
				iH = stringtextz5->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;
				gEngfuncs.pTriAPI->Color4ub(255, 0, 0, 255);
				stringtextz5->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
			}
			else if (timetx2 == 6)
			{
				iW = stringtextz6->w();
				iH = stringtextz6->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;
				gEngfuncs.pTriAPI->Color4ub(255, 0, 0, 255);
				stringtextz6->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
			}
			else if (timetx2 == 7)
			{
				iW = stringtextz7->w();
				iH = stringtextz7->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;
				gEngfuncs.pTriAPI->Color4ub(255, 0, 0, 255);
				stringtextz7->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigz, m_rctexnumbigz, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 8)
			{
				iW = stringtextz8->w();
				iH = stringtextz8->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;
				gEngfuncs.pTriAPI->Color4ub(255, 0, 0, 255);
				stringtextz8->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigz, m_rctexnumbigz, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 9)
			{
				iW = stringtextz9->w();
				iH = stringtextz9->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;
				gEngfuncs.pTriAPI->Color4ub(255, 0, 0, 255);
				stringtextz9->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigz, m_rctexnumbigz, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 10)
			{
				iW = stringtextz10->w();
				iH = stringtextz10->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;
				gEngfuncs.pTriAPI->Color4ub(255, 0, 0, 255);
				stringtextz10->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigz, m_rctexnumbigz, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 11)
			{
				iW = stringtextz11->w();
				iH = stringtextz11->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;
				gEngfuncs.pTriAPI->Color4ub(255, 0, 0, 255);
				stringtextz11->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigz, m_rctexnumbigz, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 12)
			{
				iW = stringtextz12->w();
				iH = stringtextz12->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;
				gEngfuncs.pTriAPI->Color4ub(255, 0, 0, 255);
				stringtextz12->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigz, m_rctexnumbigz, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 13)
			{
				iW = stringtextz13->w();
				iH = stringtextz13->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;
				gEngfuncs.pTriAPI->Color4ub(255, 0, 0, 255);
				stringtextz13->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigz, m_rctexnumbigz, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 14)
			{
				iW = stringtextz14->w();
				iH = stringtextz14->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;
				gEngfuncs.pTriAPI->Color4ub(255, 0, 0, 255);
				stringtextz14->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigz, m_rctexnumbigz, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 15)
			{
				iW = stringtextz15->w();
				iH = stringtextz15->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;
				gEngfuncs.pTriAPI->Color4ub(255, 0, 0, 255);
				stringtextz15->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigz, m_rctexnumbigz, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 16)
			{
				iW = stringtextz16->w();
				iH = stringtextz16->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;
				gEngfuncs.pTriAPI->Color4ub(255, 0, 0, 255);
				stringtextz16->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigz, m_rctexnumbigz, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 17)
			{
				iW = stringtextz17->w();
				iH = stringtextz17->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;
				gEngfuncs.pTriAPI->Color4ub(255, 0, 0, 255);
				stringtextz17->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigz, m_rctexnumbigz, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 18)
			{
				iW = stringtextz18->w();
				iH = stringtextz18->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;
				gEngfuncs.pTriAPI->Color4ub(255, 0, 0, 255);
				stringtextz18->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigz, m_rctexnumbigz, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 19)
			{
				iW = stringtextz19->w();
				iH = stringtextz19->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;
				gEngfuncs.pTriAPI->Color4ub(255, 0, 0, 255);
				stringtextz19->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigz, m_rctexnumbigz, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 20)
			{
				iW = stringtextz20->w();
				iH = stringtextz20->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;
				gEngfuncs.pTriAPI->Color4ub(255, 0, 0, 255);
				stringtextz20->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigz, m_rctexnumbigz, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 21)
			{
				iW = stringtextz21->w();
				iH = stringtextz21->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;
				gEngfuncs.pTriAPI->Color4ub(255, 0, 0, 255);
				stringtextz21->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigz, m_rctexnumbigz, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 22)
			{
				iW = stringtextz22->w();
				iH = stringtextz22->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;
				gEngfuncs.pTriAPI->Color4ub(255, 0, 0, 255);
				stringtextz22->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigz, m_rctexnumbigz, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 23)
			{
				iW = stringtextz23->w();
				iH = stringtextz23->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;
				gEngfuncs.pTriAPI->Color4ub(255, 0, 0, 255);
				stringtextz23->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigz, m_rctexnumbigz, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 24)
			{
				iW = stringtextz24->w();
				iH = stringtextz24->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;
				gEngfuncs.pTriAPI->Color4ub(255, 0, 0, 255);
				stringtextz24->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigz, m_rctexnumbigz, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 25)
			{
				iW = stringtextz25->w();
				iH = stringtextz25->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;
				gEngfuncs.pTriAPI->Color4ub(255, 0, 0, 255);
				stringtextz25->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigz, m_rctexnumbigz, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 26)
			{
				iW = stringtextz26->w();
				iH = stringtextz26->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;
				gEngfuncs.pTriAPI->Color4ub(255, 0, 0, 255);
				stringtextz26->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
			}
			else if (timetx2 == 27)
			{
				iW = stringtextz27->w();
				iH = stringtextz27->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;
				gEngfuncs.pTriAPI->Color4ub(255, 0, 0, 255);
				stringtextz27->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
			}
			else if (timetx2 == 28)
			{
				iW = stringtextz28->w();
				iH = stringtextz28->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;
				gEngfuncs.pTriAPI->Color4ub(255, 0, 0, 255);
				stringtextz28->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
			}
			else if (timetx2 == 29)
			{
				iW = stringtextz29->w();
				iH = stringtextz29->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;
				gEngfuncs.pTriAPI->Color4ub(255, 0, 0, 255);
				stringtextz29->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
			}
			else if (timetx2 == 30)
			{
				iW = stringtextz30->w();
				iH = stringtextz30->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;
				gEngfuncs.pTriAPI->Color4ub(255, 0, 0, 255);
				stringtextz30->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
			}
			else if (timetx2 == 31)
			{
				iW = stringtextz31->w();
				iH = stringtextz31->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;
				gEngfuncs.pTriAPI->Color4ub(255, 0, 0, 255);
				stringtextz31->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
			}
			else if (timetx2 == 32)
			{
				iW = stringtextz32->w();
				iH = stringtextz32->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;
				gEngfuncs.pTriAPI->Color4ub(255, 0, 0, 255);
				stringtextz32->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);

				go = false;
				timetx2 = -0;
			}
		}
		else
		{
			gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);

			if (timetx2 == 1)
			{
				iW = stringtext1->w();
				iH = stringtext1->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;

				stringtext1->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
			}
			else if (timetx2 == 2)
			{
				iW = stringtext2->w();
				iH = stringtext2->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;

				stringtext2->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);

			}
			else if (timetx2 == 3)
			{
				iW = stringtext3->w();
				iH = stringtext3->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;

				stringtext3->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
			}
			else if (timetx2 == 4)
			{
				iW = stringtext4->w();
				iH = stringtext4->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;

				stringtext4->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
			}
			else if (timetx2 == 5)
			{
				iW = stringtext5->w();
				iH = stringtext5->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;

				stringtext5->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
			}
			else if (timetx2 == 6)
			{
				iW = stringtext6->w();
				iH = stringtext6->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;

				stringtext6->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
			}
			else if (timetx2 == 7)
			{
				iW = stringtext7->w();
				iH = stringtext7->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;

				stringtext7->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigh, m_rctexnumbigh, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 8)
			{
				iW = stringtext8->w();
				iH = stringtext8->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;

				stringtext8->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigh, m_rctexnumbigh, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 9)
			{
				iW = stringtext9->w();
				iH = stringtext9->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;

				stringtext9->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigh, m_rctexnumbigh, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 10)
			{
				iW = stringtext10->w();
				iH = stringtext10->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;

				stringtext10->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigh, m_rctexnumbigh, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 11)
			{
				iW = stringtext11->w();
				iH = stringtext11->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;

				stringtext11->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigh, m_rctexnumbigh, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 12)
			{
				iW = stringtext12->w();
				iH = stringtext12->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;

				stringtext12->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);

				DrawTexturedNumbersTopRightAligned(*texnumbigh, m_rctexnumbigh, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 13)
			{
				iW = stringtext13->w();
				iH = stringtext13->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;

				stringtext13->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigh, m_rctexnumbigh, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 14)
			{
				iW = stringtext14->w();
				iH = stringtext14->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;

				stringtext14->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigh, m_rctexnumbigh, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 15)
			{
				iW = stringtext15->w();
				iH = stringtext15->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;

				stringtext15->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigh, m_rctexnumbigh, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 16)
			{
				iW = stringtext16->w();
				iH = stringtext16->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;

				stringtext16->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigh, m_rctexnumbigh, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 17)
			{
				iW = stringtext17->w();
				iH = stringtext17->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;

				stringtext17->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigh, m_rctexnumbigh, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 18)
			{
				iW = stringtext18->w();
				iH = stringtext18->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;

				stringtext18->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigh, m_rctexnumbigh, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 19)
			{
				iW = stringtext19->w();
				iH = stringtext19->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;

				stringtext19->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigh, m_rctexnumbigh, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 20)
			{
				iW = stringtext20->w();
				iH = stringtext20->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;

				stringtext20->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigh, m_rctexnumbigh, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 21)
			{
				iW = stringtext21->w();
				iH = stringtext21->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;

				stringtext21->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigh, m_rctexnumbigh, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 22)
			{
				iW = stringtext22->w();
				iH = stringtext22->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;

				stringtext22->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigh, m_rctexnumbigh, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 23)
			{
				iW = stringtext23->w();
				iH = stringtext23->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;

				stringtext23->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigh, m_rctexnumbigh, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 24)
			{
				iW = stringtext24->w();
				iH = stringtext24->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;

				stringtext24->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigh, m_rctexnumbigh, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 25)
			{
				iW = stringtext25->w();
				iH = stringtext25->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;

				stringtext25->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
				DrawTexturedNumbersTopRightAligned(*texnumbigh, m_rctexnumbigh, kill, x - 75, y - 55, 1.0f);
			}
			else if (timetx2 == 26)
			{
				iW = stringtext26->w();
				iH = stringtext26->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;

				stringtext26->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
			}
			else if (timetx2 == 27)
			{
				iW = stringtext27->w();
				iH = stringtext27->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;

				stringtext27->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
			}
			else if (timetx2 == 28)
			{
				iW = stringtext28->w();
				iH = stringtext28->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;

				stringtext28->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
			}
			else if (timetx2 == 29)
			{
				iW = stringtext29->w();
				iH = stringtext29->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;

				stringtext29->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
			}
			else if (timetx2 == 30)
			{
				iW = stringtext30->w();
				iH = stringtext30->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;

				stringtext30->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
			}
			else if (timetx2 == 31)
			{
				iW = stringtext31->w();
				iH = stringtext31->h();
				iX = ScreenWidth / w;
				iY = ScreenHeight - h;

				stringtext31->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
			}
			else if (timetx2 == 32)
			{
				go = false;
				timetx2 = -0;
			}
		}
	}
	return 1;
}

void CHudKillZB5::getgo()
{
	go = true;
}