/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"
#include "WeaponChange.h"

#include "string.h"
#include "assert.h"
#include <numeric>
#include <tuple>

int CHudGDWeapomChangeWEBM::VidInit(void)
{
	if (!stringtext0)
		stringtext1 = R_LoadTextureShared("resource/hud/gd/weaponchange/videoframe_0", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext1)
		stringtext1 = R_LoadTextureShared("resource/hud/gd/weaponchange/videoframe_1", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext2)
		stringtext2 = R_LoadTextureShared("resource/hud/gd/weaponchange/videoframe_2", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext3)
		stringtext3 = R_LoadTextureShared("resource/hud/gd/weaponchange/videoframe_3", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext4)
		stringtext4 = R_LoadTextureShared("resource/hud/gd/weaponchange/videoframe_4", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext5)
		stringtext5 = R_LoadTextureShared("resource/hud/gd/weaponchange/videoframe_5", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext6)
		stringtext6 = R_LoadTextureShared("resource/hud/gd/weaponchange/videoframe_6", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext7)
		stringtext7 = R_LoadTextureShared("resource/hud/gd/weaponchange/videoframe_7", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext8)
		stringtext8 = R_LoadTextureShared("resource/hud/gd/weaponchange/videoframe_8", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext9)
		stringtext9 = R_LoadTextureShared("resource/hud/gd/weaponchange/videoframe_9", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext10)
		stringtext10 = R_LoadTextureShared("resource/hud/gd/weaponchange/videoframe_10", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext11)
		stringtext11 = R_LoadTextureShared("resource/hud/gd/weaponchange/videoframe_11", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext12)
		stringtext12 = R_LoadTextureShared("resource/hud/gd/weaponchange/videoframe_12", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext13)
		stringtext13 = R_LoadTextureShared("resource/hud/gd/weaponchange/videoframe_13", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext14)
		stringtext14 = R_LoadTextureShared("resource/hud/gd/weaponchange/videoframe_14", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext15)
		stringtext15 = R_LoadTextureShared("resource/hud/gd/weaponchange/videoframe_15", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext16)
		stringtext16 = R_LoadTextureShared("resource/hud/gd/weaponchange/videoframe_16", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext17)
		stringtext17 = R_LoadTextureShared("resource/hud/gd/weaponchange/videoframe_17", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext18)
		stringtext18 = R_LoadTextureShared("resource/hud/gd/weaponchange/videoframe_18", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext19)
		stringtext19 = R_LoadTextureShared("resource/hud/gd/weaponchange/videoframe_19", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext20)
		stringtext20 = R_LoadTextureShared("resource/hud/gd/weaponchange/videoframe_20", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext21)
		stringtext21 = R_LoadTextureShared("resource/hud/gd/weaponchange/videoframe_21", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext22)
		stringtext22 = R_LoadTextureShared("resource/hud/gd/weaponchange/videoframe_22", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!stringtext23)
		stringtext23 = R_LoadTextureShared("resource/hud/gd/weaponchange/videoframe_23", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	return 1;
}

int CHudGDWeapomChangeWEBM::Draw(float time)
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

	int w = 335;
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

		if (timetx < 23)
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

	return 1;
}

void CHudGDWeapomChangeWEBM::SetWebm()
{
	timetx = 0;
	m_pCurTexture = true;
	m_flDisplayTime = gHUD.m_flTime;
}