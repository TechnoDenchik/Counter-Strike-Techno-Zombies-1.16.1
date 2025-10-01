/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"
#include "parsemsg.h"
#include "original/mod_base.h"
#include "FinalAttack.h"
#include "hud_sub_impl.h"
#include "gamemode/zb2/zb2_const.h"

#include "string.h"
#include "assert.h"
#include <numeric>
#include <tuple>
#include <vector>

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

int CHudGDFinalAttackWEBM::VidInit(void)
{
	if (!stringtext0)
		stringtext1 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_0", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext1)
		stringtext1 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_1", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext2)
		stringtext2 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_2", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext3)
		stringtext3 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_3", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext4)
		stringtext4 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_4", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext5)
		stringtext5 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_5", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext6)
		stringtext6 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_6", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext7)
		stringtext7 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_7", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext8)
		stringtext8 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_8", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext9)
		stringtext9 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_9", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext10)
		stringtext10 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_10", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext11)
		stringtext11 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_11", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext12)
		stringtext12 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_12", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext13)
		stringtext13 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_13", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext14)
		stringtext14 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_14", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext15)
		stringtext15 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_15", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext16)
		stringtext16 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_16", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext17)
		stringtext17 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_17", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext18)
		stringtext18 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_18", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext19)
		stringtext19 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_19", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext20)
		stringtext20 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_20", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext21)
		stringtext21 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_21", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext22)
		stringtext22 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_22", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext23)
		stringtext23 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_23", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext24)
		stringtext24 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_24", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext25)
		stringtext25 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_25", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext26)
		stringtext26 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_26", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext27)
		stringtext27 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_27", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext28)
		stringtext28 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_28", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext29)
		stringtext29 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_29", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext30)
		stringtext30 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_30", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext31)
		stringtext31 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_31", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext32)
		stringtext32 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_32", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext33)
		stringtext33 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_33", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext34)
		stringtext34 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_34", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext35)
		stringtext35 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_35", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext36)
		stringtext36 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_36", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext37)
		stringtext37 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_37", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext38)
		stringtext38 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_38", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext39)
		stringtext39 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_39", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext40)
		stringtext40 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_40", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext41)
		stringtext41 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_41", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext42)
		stringtext42 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_42", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext43)
		stringtext43 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_43", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext44)
		stringtext44 = R_LoadTextureShared("resource/hud/gd/finalattack/videoframe_44", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	return 1;
}

int CHudGDFinalAttackWEBM::Draw(float time)
{
	if (!m_pCurTexture)
		return 1;

	if (time > m_flDisplayTime + 3.0f)
	{
		m_pCurTexture = false;
		return 1;
	}

	int x = ScreenWidth / 2;
	int y = ScreenHeight / 4;

	int x18 = ScreenWidth / 1.995;
	int y18 = ScreenHeight / 4.0;

	int w = 435;
	int h = 38;

	int h2 = 245;

	const float flScale = 0.0f;
	const int r = 255, g = 255, b = 255;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255 * std::min(5.0f - (time - m_flDisplayTime), 2.0f));

	if (gHUD.m_flTime - timesecond < 1.0f)
	{
		tDeltasecond += gHUD.m_flTime - timesecond;
	}
	if (tNextsecond > 0.024f || (gHUD.m_flTime - timesecond > 0.024f) || tDeltasecond > 0.024f)
	{
		tNextsecond = 0.0f;
		tDeltasecond = 0.0f;

		if (timetx < 44)
		{
			timetx++;
		}
	}
	timesecond = gHUD.m_flTime;

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

	return 1;
}

void CHudGDFinalAttackWEBM::SetWebm()
{
	timetx = 0;
	m_pCurTexture = true;
	m_flDisplayTime = gHUD.m_flTime;
}