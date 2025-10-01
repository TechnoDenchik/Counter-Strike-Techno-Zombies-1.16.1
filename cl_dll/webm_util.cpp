/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "hud.h"
#include "cl_util.h"
#include "webm_util.h"
#include "triangleapi.h"
#include <string.h>
#include <studio.h>
#include "r_studioint.h"
#include "draw_util.h"
#include "calcscreen.h"
#include "com_model.h"

#include "events.h"
#include "r_efx.h"

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

int WebmUtils::Init(void)
{
	gHUD.AddHudElem(this);
	m_iFlags |= HUD_DRAW;

	return 1;
}

void WebmUtils::Reset(void)
{
	VidInit();
}

int WebmUtils::VidInit(void)
{
	char path[128];
	static int numberframe;
	

	switch (frame)
	{
	case 1: numberframe = 0; break;
	case 2: numberframe = 1; break;
	case 3: numberframe = 2; break;
	case 4: numberframe = 3; break;
	case 5: numberframe = 4; break;
	case 6: numberframe = 5; break;
	case 7: numberframe = 6; break;
	case 8: numberframe = 7; break;
	case 9: numberframe = 8; break;
	case 10: numberframe = 9; break;
	case 11: numberframe = 10; break;
	case 12: numberframe = 11; break;
	case 13: numberframe = 12; break;
	case 14: numberframe = 13; break;
	case 15: numberframe = 14; break;
	case 16: numberframe = 15; break;
	case 17: numberframe = 16; break;
	case 18: numberframe = 17; break;
	case 19: numberframe = 18; break;
	case 20: numberframe = 19; break;
	case 21: numberframe = 20; break;
	case 22: numberframe = 21; break;
	case 23: numberframe = 22; break;
	case 24: numberframe = 23; break;
	case 25: numberframe = 24; break;
	case 26: numberframe = 25; break;
	case 27: numberframe = 26; break;
	case 28: numberframe = 27; break;
	case 29: numberframe = 28; break;
	case 30: numberframe = 29; break;
	case 31: numberframe = 30; break;
	case 32: numberframe = 31; break;
	case 33: numberframe = 32; break;
	case 34: numberframe = 33; break;
	case 35: numberframe = 34; break;
	case 36: numberframe = 35; break;
	case 37: numberframe = 36; break;
	case 38: numberframe = 37; break;
	case 39: numberframe = 38; break;
	case 40: numberframe = 39; break;
	case 41: numberframe = 40; break;
	case 42: numberframe = 41; break;
	case 43: numberframe = 42; break;
	case 44: numberframe = 43; break;
	case 45: numberframe = 44; break;
	case 46: numberframe = 45; break;
	case 47: numberframe = 46; break;
	case 48: numberframe = 47; break;
	case 49: numberframe = 48; break;
	case 50: numberframe = 49; break;
	case 51: numberframe = 50; break;
	case 52: numberframe = 51; break;
	case 53: numberframe = 52; break;
	case 54: numberframe = 53; break;
	case 55: numberframe = 54; break;
	case 56: numberframe = 55; break;
	case 57: numberframe = 56; break;
	case 58: numberframe = 57; break;
	case 59: numberframe = 58; break;
	case 60: numberframe = 59; break;
	case 61: numberframe = 60; break;
	case 62: numberframe = 61; break;
	case 63: numberframe = 62; break;
	case 64: numberframe = 63; break;
	case 65: numberframe = 64; break;
	case 66: numberframe = 65; break;
	case 67: numberframe = 66; break;
	case 68: numberframe = 67; break;
	case 69: numberframe = 68; break;
	case 70: numberframe = 69; break;
	case 71: numberframe = 70; break;
	case 72: numberframe = 71; break;
	case 73: numberframe = 72; break;
	case 74: numberframe = 73; break;
	case 75: numberframe = 74; break;
	}

	sprintf(path, "resource/hud/zb5/videoframe_%d.tga", numberframe);
	R_InitTexture(m_iTex2[0], path);

	return 1;
}

int WebmUtils::Draw(float time)
{
	return 1;
}

void WebmUtils::Shutdown(void)
{
	std::fill(std::begin(m_iTex2), std::end(m_iTex2), nullptr);
}

void WebmUtils::DrawWebm(int countframe, float nexttime, char folder1[64], char folder2[64], float w, float h, int h2, float x1, float y1, float x2, float y2, int r, int g, int b, int brightness)
{
	static WebmUtils util;

	util.numfolder1[0] = folder1[0];
	util.numfolder2[0] = folder2[0];

	if (gHUD.m_flTime - util.timeseconds < 1.0f)
	{
		util.tDeltaseconds += gHUD.m_flTime - util.timeseconds;
	}
	if (util.tNextwebm > nexttime || (gHUD.m_flTime - util.timeseconds > nexttime) || util.tDeltaseconds > nexttime)
	{
		util.tNextwebm = 0.0f;
		util.tDeltaseconds = 0.0f;

		if (util.frame < countframe)
		{
			util.frame++;
		}
	}
	util.timeseconds = gHUD.m_flTime;

	if (util.frame > countframe)
	{
		util.frame = 0;
	}

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->Color4ub(r, g, b, brightness);

	
	util.m_iTex2[0]->Draw2DQuadScaled(x1 - w / static_cast<float>(2), y1 - h, x2 + w / static_cast<float>(2), y2 + h2);

}