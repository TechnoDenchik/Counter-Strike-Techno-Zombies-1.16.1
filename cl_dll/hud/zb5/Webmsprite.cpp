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
			DrawUtils::DrawHudString(x3 - 190, y3 - 32, ScreenWidth, szbuffer, 255, 255, 255, 255, flScale);
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
			DrawUtils::DrawHudString(x3 - 190, y3 - 32, ScreenWidth, szbuffer, 255, 255, 255, 255, flScale);
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
			DrawUtils::DrawHudString(x3 - 190, y3 - 32, ScreenWidth, szbuffer, 255, 255, 255, 255, flScale);
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

int CHudWebm2::VidInit(void)
{
	if (!stringtext)
		stringtext = R_LoadTextureShared("resource/zb5/zb5_skilll_effect", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!stringtext2)
		stringtext2 = R_LoadTextureShared("resource/zb5/zb5_skilll_effect", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	return 1;
}

int CHudWebm2::Draw(float time)
{
	if (!m_pCurTexture)
		return 1;

	if (time > m_flDisplayTime + 1.0f)
	{
		m_pCurTexture = false;
		return 1;
	}

	int x = ScreenWidth / 6.595;
	int x2 = ScreenWidth / 1.185;

	int y = ScreenHeight / 8.5;

	int x3 = ScreenWidth / 2.0;
	int y3 = ScreenHeight / 8.5;

	const float flScale = 0.0f;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	
	if (m_startadded == true)
	{
		if (barstring < 255)
		{
			barstring++;
			barstring++;
			barstring++;
			barstring++;
			barstring++;
		}

		if (barstring == 255)
		{
			m_startrenine = true;
			m_startadded = false;
		}

		gEngfuncs.pTriAPI->Color4ub(255, 255, 255, barstring);
	}
	if (m_startrenine == true)
	{
		barstring2++;
		barstring3--;
		gEngfuncs.pTriAPI->Color4ub(255, 255, 255, barstring * std::min(1.0f - (time - m_flDisplayTime), 1.0f));
	}
	
	stringtext->Bind();
	DrawUtils::Draw2DQuadScaled(x - 300, y - barstring3, x + 300, y + barstring2);

	stringtext2->Bind();
	DrawUtils::Draw2DQuadScaled(x2 + 300, y - barstring3, x2 - 300, y + barstring2);
	
	return 1;
}

void CHudWebm2::Settext()
{
	barstring3 = -38;
	barstring2 = 774;
	barstring = 0;
	m_startadded = true;
	m_startrenine == false;
	m_pCurTexture = true;
	m_flDisplayTime = gHUD.m_flTime;
}


int CHudWebm3::VidInit(void)
{
	if (!stringtext)
		stringtext = R_LoadTextureShared("resource/zb5/zb5_zombie_bg", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!zbtank)
		zbtank = R_LoadTextureShared("resource/hud/portrait/hud_zombi_host", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!zbspeed)
		zbspeed = R_LoadTextureShared("resource/hud/portrait/hud_speed_zombi_origin", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!zbheavy)
		zbheavy = R_LoadTextureShared("resource/hud/portrait/hud_heavy_zombi_host", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!zbheal)
		zbheal = R_LoadTextureShared("resource/hud/portrait/hud_heal_zombi_origin", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!zbpsycho)
		zbpsycho = R_LoadTextureShared("resource/hud/portrait/hud_pc_zombi_origin", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!zbdeimos)
		zbdeimos = R_LoadTextureShared("resource/hud/portrait/hud_deimos_zombi_host", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!zbganimed)
		zbganimed = R_LoadTextureShared("resource/hud/portrait/hud_deimos2_zombi_origin", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!zbbanchee)
		zbbanchee = R_LoadTextureShared("resource/hud/portrait/hud_witch_zombi_origin", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!zbstamper)
		zbstamper = R_LoadTextureShared("resource/hud/portrait/hud_stamper_zombi_origin", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!zbresident)
		zbresident = R_LoadTextureShared("resource/hud/portrait/hud_resident_zombi_origin", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!zbboomer)
		zbboomer = R_LoadTextureShared("resource/hud/portrait/hud_boomer_zombi_origin", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!zbbooster)
		zbbooster = R_LoadTextureShared("resource/hud/portrait/hud_booster_zombi_origin", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!zbflying)
		zbflying = R_LoadTextureShared("resource/hud/portrait/hud_flyingzombie_host", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!zbchina)
		zbchina = R_LoadTextureShared("resource/hud/portrait/hud_china_zombi_origin", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!zbaksha)
		zbaksha = R_LoadTextureShared("resource/hud/portrait/hud_akshazombie", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!zbpass)
		zbpass = R_LoadTextureShared("resource/hud/portrait/hud_akshazombie", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!zbrevival)
		zbrevival = R_LoadTextureShared("resource/hud/portrait/hud_akshazombie", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);


	if (!skilltank)
		skilltank = R_LoadTextureShared("resource/zb5/icon/zb5_ha_normal_skill", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!skillspeed)
		skillspeed = R_LoadTextureShared("resource/zb5/icon/zb5_ha_speed_skill", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!skillheavy)
		skillheavy = R_LoadTextureShared("resource/zb5/icon/zb5_ha_heavy_skill", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!skillheal)
		skillheal = R_LoadTextureShared("resource/zb5/icon/zb5_ha_heal_skill", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!skillpsycho)
		skillpsycho = R_LoadTextureShared("resource/zb5/icon/zb5_ha_psycho_skill", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!skilldeimos)
		skilldeimos = R_LoadTextureShared("resource/zb5/icon/zb5_ha_deimos_skill", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!skillganimed)
		skillganimed = R_LoadTextureShared("resource/zb5/icon/zb5_ha_ganymede_skill", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!skillbanchee)
		skillbanchee = R_LoadTextureShared("resource/zb5/icon/zb5_ha_heal_skill", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!skillstamper)
		skillstamper = R_LoadTextureShared("resource/zb5/icon/zb5_ha_undertaker_skill", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!skillresident)
		skillresident = R_LoadTextureShared("resource/zb5/icon/zb5_ha_resident_skill", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!skillboomer)
		skillboomer = R_LoadTextureShared("resource/zb5/icon/zb5_ha_boomer_skill", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!skillbooster)
		skillbooster = R_LoadTextureShared("resource/zb5/icon/zb5_ha_booster_skill", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!skillflyinng)
		skillflyinng = R_LoadTextureShared("resource/zb5/icon/zb5_ha_flying_skill", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!skillflyinng2)
		skillflyinng = R_LoadTextureShared("resource/zb5/icon/zb5_ha_flying_skill2", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!skillchina)
		skillchina = R_LoadTextureShared("resource/zb5/icon/zb5_ha_china_skill", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!skillaksha)
		skillaksha = R_LoadTextureShared("resource/zb5/icon/zb5_ha_aksha_skill", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!skillpass)
		skillpass = R_LoadTextureShared("resource/zb5/icon/zb5_ha_pass_skill", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	
	if (!skillrevival)
		skillrevival = R_LoadTextureShared("resource/zb5/icon/zb5_ha_revival_skill", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);


	if (!bufftank)
		bufftank = R_LoadTextureShared("resource/zb5/icon/zb5_ha_normal_buff", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!buffspeed)
		buffspeed = R_LoadTextureShared("resource/zb5/icon/zb5_ha_speed_buff", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!buffheavy)
		buffheavy = R_LoadTextureShared("resource/zb5/icon/zb5_ha_heavy_buff", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!buffheal)
		buffheal = R_LoadTextureShared("resource/zb5/icon/zb5_ha_heal_buff", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!buffpsycho)
		buffpsycho = R_LoadTextureShared("resource/zb5/icon/zb5_ha_psycho_buff", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!buffdeimos)
		buffdeimos = R_LoadTextureShared("resource/zb5/icon/zb5_ha_deimos_buff", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!buffganimed)
		buffganimed = R_LoadTextureShared("resource/zb5/icon/zb5_ha_ganymede_buff", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!buffbanchee)
		buffbanchee = R_LoadTextureShared("resource/zb5/icon/zb5_ha_heal_buff", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!buffstamper)
		buffstamper = R_LoadTextureShared("resource/zb5/icon/zb5_ha_undertaker_buff", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!buffresident)
		buffresident = R_LoadTextureShared("resource/zb5/icon/zb5_ha_resident_buff", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!buffboomer)
		buffboomer = R_LoadTextureShared("resource/zb5/icon/zb5_ha_boomer_buff", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!buffbooster)
		buffbooster = R_LoadTextureShared("resource/zb5/icon/zb5_ha_booster_buff", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!buffflying)
		buffflying = R_LoadTextureShared("resource/zb5/icon/zb5_ha_flying_buff", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!buffchina)
		buffchina = R_LoadTextureShared("resource/zb5/icon/zb5_ha_china_buff", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!buffaksha)
		buffaksha = R_LoadTextureShared("resource/zb5/icon/zb5_ha_aksha_buff", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!buffpass)
		buffpass = R_LoadTextureShared("resource/zb5/icon/zb5_ha_pass_buff", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!buffrevival)
		buffrevival = R_LoadTextureShared("resource/zb5/icon/zb5_ha_revival_buff", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	return 1;
}

int CHudWebm3::Draw(float time)
{
	if (!m_pCurTexture)
		return 1;

	if (time > m_flDisplayTime + 5.0f)
	{
		m_pCurTexture = false;
		return 1;
	}

	int x = ScreenWidth / 2.0;
	int y = ScreenHeight / 1.5;

	int x2 = ScreenWidth / 2.280;
	int y2 = ScreenHeight / 1.435;
	
	int x3 = ScreenWidth / 1.810;
	int y3 = ScreenHeight / 1.435;

	int x4 = ScreenWidth / 2.010;
	int y4 = ScreenHeight / 1.435;

	int x5 = ScreenWidth / 1.810;
	int y5 = ScreenHeight / 1.5;

	const float flScale = 0.0f;

	char szbuffer[64];

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);

	if (m_startadded == true)
	{
		if (barstring < 255)
		{
			barstring++;
			barstring++;
			barstring++;
			barstring++;
			barstring++;
		}

		if (barstring == 255)
		{
			m_startrenine = true;
			m_startadded = false;
		}

		gEngfuncs.pTriAPI->Color4ub(255, 255, 255, barstring);
	}
	if (m_startrenine == true)
	{
		barstring2++;
		barstring3--;
		gEngfuncs.pTriAPI->Color4ub(255, 255, 255, barstring * std::min(5.0f - (time - m_flDisplayTime), 1.0f));
	}

	if (evoskin == 1)
	{
		stringtext->Bind();
		DrawUtils::Draw2DQuadScaled(x - 180, y - 38, x + 180, y + 60);

		zbtank->Bind();
		DrawUtils::Draw2DQuadScaled(x2 - 50, y2 - 55, x2 + 36, y2 + 25);

		skilltank->Bind();
		DrawUtils::Draw2DQuadScaled(x4 - 30, y4 - 38, x4 + 30, y4 + 15);

		sprintf(szbuffer, "[Обычный Зомби]");
		
		if (m_startrenine == true)
		{
			DrawUtils::DrawHudString(x5 - 106, y5 - 32, ScreenWidth, szbuffer, 255, 0, 0, barstring * std::min(5.0f - (time - m_flDisplayTime), 1.0f), flScale);
			gEngfuncs.pTriAPI->Color4ub(255, 165, 0, barstring * std::min(5.0f - (time - m_flDisplayTime), 1.0f));
		}

		bufftank->Bind();
		DrawUtils::Draw2DQuadScaled(x3 - 30, y3 - 38, x3 + 30, y3 + 15);
	}
	else if (evoskin == 2)
	{
		stringtext->Bind();
		DrawUtils::Draw2DQuadScaled(x - 180, y - 38, x + 180, y + 60);

		zbspeed->Bind();
		DrawUtils::Draw2DQuadScaled(x2 - 50, y2 - 55, x2 + 36, y2 + 25);

		skillspeed->Bind();
		DrawUtils::Draw2DQuadScaled(x4 - 30, y4 - 38, x4 + 30, y4 + 15);

		sprintf(szbuffer, "[Лёгкий Зомби]");

		if (m_startrenine == true)
		{
			DrawUtils::DrawHudString(x5 - 106, y5 - 32, ScreenWidth, szbuffer, 255, 0, 0, barstring * std::min(5.0f - (time - m_flDisplayTime), 1.0f), flScale);
			gEngfuncs.pTriAPI->Color4ub(255, 165, 0, barstring * std::min(5.0f - (time - m_flDisplayTime), 1.0f));
		}

		buffspeed->Bind();
		DrawUtils::Draw2DQuadScaled(x3 - 30, y3 - 38, x3 + 30, y3 + 15);
	}
	else if (evoskin == 3)
	{
		stringtext->Bind();
		DrawUtils::Draw2DQuadScaled(x - 180, y - 38, x + 180, y + 60);

		zbheavy->Bind();
		DrawUtils::Draw2DQuadScaled(x2 - 50, y2 - 55, x2 + 36, y2 + 25);

		skillheavy->Bind();
		DrawUtils::Draw2DQuadScaled(x4 - 30, y4 - 38, x4 + 30, y4 + 15);

		sprintf(szbuffer, "[Тяжёлый Зомби]");

		if (m_startrenine == true)
		{
			DrawUtils::DrawHudString(x5 - 106, y5 - 32, ScreenWidth, szbuffer, 255, 0, 0, barstring * std::min(5.0f - (time - m_flDisplayTime), 1.0f), flScale);
			gEngfuncs.pTriAPI->Color4ub(255, 165, 0, barstring * std::min(5.0f - (time - m_flDisplayTime), 1.0f));
		}

		buffheavy->Bind();
		DrawUtils::Draw2DQuadScaled(x3 - 30, y3 - 38, x3 + 30, y3 + 15);
	}
	else if (evoskin == 4)
	{
		stringtext->Bind();
		DrawUtils::Draw2DQuadScaled(x - 180, y - 38, x + 180, y + 60);

		zbheal->Bind();
		DrawUtils::Draw2DQuadScaled(x2 - 50, y2 - 55, x2 + 36, y2 + 25);

		skillheal->Bind();
		DrawUtils::Draw2DQuadScaled(x4 - 30, y4 - 38, x4 + 30, y4 + 15);

		sprintf(szbuffer, "[Вуду Зомби]");

		if (m_startrenine == true)
		{
			DrawUtils::DrawHudString(x5 - 106, y5 - 32, ScreenWidth, szbuffer, 255, 0, 0, barstring * std::min(5.0f - (time - m_flDisplayTime), 1.0f), flScale);
			gEngfuncs.pTriAPI->Color4ub(255, 165, 0, barstring * std::min(5.0f - (time - m_flDisplayTime), 1.0f));
		}

		buffheal->Bind();
		DrawUtils::Draw2DQuadScaled(x3 - 30, y3 - 38, x3 + 30, y3 + 15);
	}
	else if (evoskin == 5)
	{
		stringtext->Bind();
		DrawUtils::Draw2DQuadScaled(x - 180, y - 38, x + 180, y + 60);

		zbpsycho->Bind();
		DrawUtils::Draw2DQuadScaled(x2 - 50, y2 - 55, x2 + 36, y2 + 25);

		skillpsycho->Bind();
		DrawUtils::Draw2DQuadScaled(x4 - 30, y4 - 38, x4 + 30, y4 + 15);

		sprintf(szbuffer, "[Психо Зомби]");

		if (m_startrenine == true)
		{
			DrawUtils::DrawHudString(x5 - 106, y5 - 32, ScreenWidth, szbuffer, 255, 0, 0, barstring * std::min(5.0f - (time - m_flDisplayTime), 1.0f), flScale);
			gEngfuncs.pTriAPI->Color4ub(255, 165, 0, barstring * std::min(5.0f - (time - m_flDisplayTime), 1.0f));
		}

		buffpsycho->Bind();
		DrawUtils::Draw2DQuadScaled(x3 - 30, y3 - 38, x3 + 30, y3 + 15);
	}
	else if (evoskin == 6)
	{
		stringtext->Bind();
		DrawUtils::Draw2DQuadScaled(x - 180, y - 38, x + 180, y + 60);

		zbdeimos->Bind();
		DrawUtils::Draw2DQuadScaled(x2 - 50, y2 - 55, x2 + 36, y2 + 25);

		skilldeimos->Bind();
		DrawUtils::Draw2DQuadScaled(x4 - 30, y4 - 38, x4 + 30, y4 + 15);

		sprintf(szbuffer, "[Дэймос]");

		if (m_startrenine == true)
		{
			DrawUtils::DrawHudString(x5 - 106, y5 - 32, ScreenWidth, szbuffer, 255, 0, 0, barstring * std::min(5.0f - (time - m_flDisplayTime), 1.0f), flScale);
			gEngfuncs.pTriAPI->Color4ub(255, 165, 0, barstring * std::min(5.0f - (time - m_flDisplayTime), 1.0f));
		}

		buffdeimos->Bind();
		DrawUtils::Draw2DQuadScaled(x3 - 30, y3 - 38, x3 + 30, y3 + 15);
	}
	else if (evoskin == 7)
	{
		stringtext->Bind();
		DrawUtils::Draw2DQuadScaled(x - 180, y - 38, x + 180, y + 60);

		zbganimed->Bind();
		DrawUtils::Draw2DQuadScaled(x2 - 50, y2 - 55, x2 + 36, y2 + 25);

		skillganimed->Bind();
		DrawUtils::Draw2DQuadScaled(x4 - 30, y4 - 38, x4 + 30, y4 + 15);

		sprintf(szbuffer, "[Ганимед]");

		if (m_startrenine == true)
		{
			DrawUtils::DrawHudString(x5 - 106, y5 - 32, ScreenWidth, szbuffer, 255, 0, 0, barstring * std::min(5.0f - (time - m_flDisplayTime), 1.0f), flScale);
			gEngfuncs.pTriAPI->Color4ub(255, 165, 0, barstring * std::min(5.0f - (time - m_flDisplayTime), 1.0f));
		}

		buffganimed->Bind();
		DrawUtils::Draw2DQuadScaled(x3 - 30, y3 - 38, x3 + 30, y3 + 15);
	}
	else if (evoskin == 8)
	{
		stringtext->Bind();
		DrawUtils::Draw2DQuadScaled(x - 180, y - 38, x + 180, y + 60);

		zbbanchee->Bind();
		DrawUtils::Draw2DQuadScaled(x2 - 50, y2 - 55, x2 + 36, y2 + 25);

		skillbanchee->Bind();
		DrawUtils::Draw2DQuadScaled(x4 - 30, y4 - 38, x4 + 30, y4 + 15);

		sprintf(szbuffer, "[Банши]");

		if (m_startrenine == true)
		{
			DrawUtils::DrawHudString(x5 - 106, y5 - 32, ScreenWidth, szbuffer, 255, 0, 0, barstring * std::min(5.0f - (time - m_flDisplayTime), 1.0f), flScale);
			gEngfuncs.pTriAPI->Color4ub(255, 165, 0, barstring * std::min(5.0f - (time - m_flDisplayTime), 1.0f));
		}

		buffbanchee->Bind();
		DrawUtils::Draw2DQuadScaled(x3 - 30, y3 - 38, x3 + 30, y3 + 15);
	}
	else if (evoskin == 9)
	{
		stringtext->Bind();
		DrawUtils::Draw2DQuadScaled(x - 180, y - 38, x + 180, y + 60);

		zbstamper->Bind();
		DrawUtils::Draw2DQuadScaled(x2 - 50, y2 - 55, x2 + 36, y2 + 25);

		skillstamper->Bind();
		DrawUtils::Draw2DQuadScaled(x4 - 30, y4 - 38, x4 + 30, y4 + 15);

		sprintf(szbuffer, "[Стампер]");

		if (m_startrenine == true)
		{
			DrawUtils::DrawHudString(x5 - 106, y5 - 32, ScreenWidth, szbuffer, 255, 0, 0, barstring * std::min(5.0f - (time - m_flDisplayTime), 1.0f), flScale);
			gEngfuncs.pTriAPI->Color4ub(255, 165, 0, barstring * std::min(5.0f - (time - m_flDisplayTime), 1.0f));
		}

		buffstamper->Bind();
		DrawUtils::Draw2DQuadScaled(x3 - 30, y3 - 38, x3 + 30, y3 + 15);
	}
	else if (evoskin == 10)
	{
		stringtext->Bind();
		DrawUtils::Draw2DQuadScaled(x - 180, y - 38, x + 180, y + 60);

		zbresident->Bind();
		DrawUtils::Draw2DQuadScaled(x2 - 50, y2 - 55, x2 + 36, y2 + 25);

		skillresident->Bind();
		DrawUtils::Draw2DQuadScaled(x4 - 30, y4 - 38, x4 + 30, y4 + 15);

		sprintf(szbuffer, "[Ядовитое Жало]");

		if (m_startrenine == true)
		{
			DrawUtils::DrawHudString(x5 - 106, y5 - 32, ScreenWidth, szbuffer, 255, 0, 0, barstring * std::min(5.0f - (time - m_flDisplayTime), 1.0f), flScale);
			gEngfuncs.pTriAPI->Color4ub(255, 165, 0, barstring * std::min(5.0f - (time - m_flDisplayTime), 1.0f));
		}

		buffresident->Bind();
		DrawUtils::Draw2DQuadScaled(x3 - 30, y3 - 38, x3 + 30, y3 + 15);
	}
	else if (evoskin == 11)
	{
		stringtext->Bind();
		DrawUtils::Draw2DQuadScaled(x - 180, y - 38, x + 180, y + 60);

		zbboomer->Bind();
		DrawUtils::Draw2DQuadScaled(x2 - 50, y2 - 55, x2 + 36, y2 + 25);

		skillboomer->Bind();
		DrawUtils::Draw2DQuadScaled(x4 - 30, y4 - 38, x4 + 30, y4 + 15);

		sprintf(szbuffer, "[Стражник Яда]");

		if (m_startrenine == true)
		{
			DrawUtils::DrawHudString(x5 - 106, y5 - 32, ScreenWidth, szbuffer, 255, 0, 0, barstring * std::min(5.0f - (time - m_flDisplayTime), 1.0f), flScale);
			gEngfuncs.pTriAPI->Color4ub(255, 165, 0, barstring * std::min(5.0f - (time - m_flDisplayTime), 1.0f));
		}

		buffboomer->Bind();
		DrawUtils::Draw2DQuadScaled(x3 - 30, y3 - 38, x3 + 30, y3 + 15);
	}
	else if (evoskin == 12)
	{
		stringtext->Bind();
		DrawUtils::Draw2DQuadScaled(x - 180, y - 38, x + 180, y + 60);

		zbbooster->Bind();
		DrawUtils::Draw2DQuadScaled(x2 - 50, y2 - 55, x2 + 36, y2 + 25);

		skillbooster->Bind();
		DrawUtils::Draw2DQuadScaled(x4 - 30, y4 - 38, x4 + 30, y4 + 15);

		sprintf(szbuffer, "[Ловец]");

		if (m_startrenine == true)
		{
			DrawUtils::DrawHudString(x5 - 106, y5 - 32, ScreenWidth, szbuffer, 255, 0, 0, barstring * std::min(5.0f - (time - m_flDisplayTime), 1.0f), flScale);
			gEngfuncs.pTriAPI->Color4ub(255, 165, 0, barstring * std::min(5.0f - (time - m_flDisplayTime), 1.0f));
		}

		buffbooster->Bind();
		DrawUtils::Draw2DQuadScaled(x3 - 30, y3 - 38, x3 + 30, y3 + 15);
	}
	else if (evoskin == 13)
	{
		stringtext->Bind();
		DrawUtils::Draw2DQuadScaled(x - 180, y - 38, x + 180, y + 60);

		zbflying->Bind();
		DrawUtils::Draw2DQuadScaled(x2 - 50, y2 - 55, x2 + 36, y2 + 25);

		skillflyinng->Bind();
		DrawUtils::Draw2DQuadScaled(x4 - 30, y4 - 38, x4 + 30, y4 + 15);

		sprintf(szbuffer, "[Ржавое Крыло]");

		if (m_startrenine == true)
		{
			DrawUtils::DrawHudString(x5 - 106, y5 - 32, ScreenWidth, szbuffer, 255, 0, 0, barstring * std::min(5.0f - (time - m_flDisplayTime), 1.0f), flScale);
			gEngfuncs.pTriAPI->Color4ub(255, 165, 0, barstring * std::min(5.0f - (time - m_flDisplayTime), 1.0f));
		}

		buffflying->Bind();
		DrawUtils::Draw2DQuadScaled(x3 - 30, y3 - 38, x3 + 30, y3 + 15);
	}
	else if (evoskin == 14)
	{
		stringtext->Bind();
		DrawUtils::Draw2DQuadScaled(x - 180, y - 38, x + 180, y + 60);

		zbchina->Bind();
		DrawUtils::Draw2DQuadScaled(x2 - 50, y2 - 55, x2 + 36, y2 + 25);

		skillchina->Bind();
		DrawUtils::Draw2DQuadScaled(x4 - 30, y4 - 38, x4 + 30, y4 + 15);

		sprintf(szbuffer, "[Вампир Зомби]");

		if (m_startrenine == true)
		{
			DrawUtils::DrawHudString(x5 - 106, y5 - 32, ScreenWidth, szbuffer, 255, 0, 0, barstring * std::min(5.0f - (time - m_flDisplayTime), 1.0f), flScale);
			gEngfuncs.pTriAPI->Color4ub(255, 165, 0, barstring * std::min(5.0f - (time - m_flDisplayTime), 1.0f));
		}

		buffchina->Bind();
		DrawUtils::Draw2DQuadScaled(x3 - 30, y3 - 38, x3 + 30, y3 + 15);
	}
	else if (evoskin == 15)
	{
		stringtext->Bind();
		DrawUtils::Draw2DQuadScaled(x - 180, y - 38, x + 180, y + 60);

		zbaksha->Bind();
		DrawUtils::Draw2DQuadScaled(x2 - 50, y2 - 55, x2 + 36, y2 + 25);

		skillaksha->Bind();
		DrawUtils::Draw2DQuadScaled(x4 - 30, y4 - 38, x4 + 30, y4 + 15);

		sprintf(szbuffer, "[Якша]");

		if (m_startrenine == true)
		{
			DrawUtils::DrawHudString(x5 - 106, y5 - 32, ScreenWidth, szbuffer, 255, 0, 0, barstring * std::min(5.0f - (time - m_flDisplayTime), 1.0f), flScale);
			gEngfuncs.pTriAPI->Color4ub(255, 165, 0, barstring * std::min(5.0f - (time - m_flDisplayTime), 1.0f));
		}

		buffaksha->Bind();
		DrawUtils::Draw2DQuadScaled(x3 - 30, y3 - 38, x3 + 30, y3 + 15);
	}
	else if (evoskin == 16)
	{
		stringtext->Bind();
		DrawUtils::Draw2DQuadScaled(x - 180, y - 38, x + 180, y + 60);

		zbpass->Bind();
		DrawUtils::Draw2DQuadScaled(x2 - 50, y2 - 55, x2 + 36, y2 + 25);

		skillpass->Bind();
		DrawUtils::Draw2DQuadScaled(x4 - 30, y4 - 38, x4 + 30, y4 + 15);

		sprintf(szbuffer, "[Крючкотвор]");

		if (m_startrenine == true)
		{
			DrawUtils::DrawHudString(x5 - 106, y5 - 32, ScreenWidth, szbuffer, 255, 0, 0, barstring * std::min(5.0f - (time - m_flDisplayTime), 1.0f), flScale);
			gEngfuncs.pTriAPI->Color4ub(255, 165, 0, barstring * std::min(5.0f - (time - m_flDisplayTime), 1.0f));
		}

		buffpass->Bind();
		DrawUtils::Draw2DQuadScaled(x3 - 30, y3 - 38, x3 + 30, y3 + 15);
	}
	else if (evoskin == 17)
	{
		stringtext->Bind();
		DrawUtils::Draw2DQuadScaled(x - 180, y - 38, x + 180, y + 60);

		zbrevival->Bind();
		DrawUtils::Draw2DQuadScaled(x2 - 50, y2 - 55, x2 + 36, y2 + 25);

		skillrevival->Bind();
		DrawUtils::Draw2DQuadScaled(x4 - 30, y4 - 38, x4 + 30, y4 + 15);

		sprintf(szbuffer, "[Метатроник]");

		if (m_startrenine == true)
		{
			DrawUtils::DrawHudString(x5 - 106, y5 - 32, ScreenWidth, szbuffer, 255, 0, 0, barstring * std::min(5.0f - (time - m_flDisplayTime), 1.0f), flScale);
			gEngfuncs.pTriAPI->Color4ub(255, 165, 0, barstring * std::min(5.0f - (time - m_flDisplayTime), 1.0f));
		}

		buffrevival->Bind();
		DrawUtils::Draw2DQuadScaled(x3 - 30, y3 - 38, x3 + 30, y3 + 15);
	}

	return 1;
}

void CHudWebm3::Settext()
{
	barstring3 = -38;
	barstring2 = 774;
	barstring = 0;
	m_startadded = true;
	m_startrenine == false;
	m_pCurTexture = true;
	m_flDisplayTime = gHUD.m_flTime;
}











int CHudWebm4::VidInit(void)
{
	if (!stringtext)
		stringtext = R_LoadTextureShared("resource/zb5/zb5_level_bg_human_ani", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	if (!stringtext2)
		stringtext2 = R_LoadTextureShared("resource/zb5/zb5_level_bg_zombie_ani", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);

	R_InitTexture(texnumsmallh, "resource/zb5/zb5_level_num_human");
	R_InitTexture(texnumsmallz, "resource/zb5/zb5_level_num_zombie");

	BuildNumberRC(m_rctexnumsmallh, 15, 23);
	BuildNumberRC(m_rctexnumsmallz, 15, 23);

	return 1;
}

void DrawSmoothCircularFill(float centerX, float centerY, float progress, SharedTexture circleTexture, int segments = 1024)
{
	if (!circleTexture || progress <= 0.0f)
		return;

	progress = std::min(progress, 1.0f);

	float radius = 50.0f; // 110/2
	float startAngle = -M_PI / 2.0f; // начинаем с верха (-90°)
	float currentAngle = startAngle + progress * 2.0f * M_PI;

	// Плавное заполнение с анти-алиасингом на краях
	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
	circleTexture->Bind();

	// Используем более высокое качество для плавности
	segments = std::max(segments, 1024);

	gEngfuncs.pTriAPI->Begin(TRI_TRIANGLE_FAN);

	// Центр круга
	gEngfuncs.pTriAPI->TexCoord2f(0.5f, 0.5f);
	gEngfuncs.pTriAPI->Vertex3f(centerX * gHUD.m_flScale, centerY * gHUD.m_flScale, 0);

	// Первая точка - вершина круга
	if (progress > 0.0f)
	{
		gEngfuncs.pTriAPI->TexCoord2f(0.5f, 0.0f);
		gEngfuncs.pTriAPI->Vertex3f(centerX * gHUD.m_flScale, (centerY - radius) * gHUD.m_flScale, 0);
	}

	// Заполняем по часовой стрелке с плавными шагами
	for (int i = 1; i <= segments; i++)
	{
		float segmentProgress = (float)i / (float)segments;
		float angle = startAngle + segmentProgress * (currentAngle - startAngle);

		// Плавная интерполяция для последнего сегмента
		if (angle > currentAngle)
			angle = currentAngle;

		float x = centerX + radius * cos(angle);
		float y = centerY + radius * sin(angle);

		// Текстурные координаты (маппинг на круглую текстуру)
		float texU = 0.5f + 0.5f * cos(angle);
		float texV = 0.5f + 0.5f * sin(angle);

		gEngfuncs.pTriAPI->TexCoord2f(texU, texV);
		gEngfuncs.pTriAPI->Vertex3f(x * gHUD.m_flScale, y * gHUD.m_flScale, 0);

		if (angle >= currentAngle)
			break;
	}

	gEngfuncs.pTriAPI->End();
}

int CHudWebm4::Draw(float time)
{
	if (m_pCurTexture)
		return 1;

	float x = ScreenWidth / 2.0;
	float y = ScreenHeight / 1.263;

	int id = gEngfuncs.GetLocalPlayer()->index;

	if (step < m_flStepEvo)
	{
		if (gHUD.m_flTime - timesecond1 < 1.0f)
		{
			tDeltasecond1 += gHUD.m_flTime - timesecond1;
		}
		if (tNextsecond1 > 0.01f || (gHUD.m_flTime - timesecond1 > 0.01f) || tDeltasecond1 > 0.01f)
		{
			tNextsecond1 = 0.0f;
			tDeltasecond1 = 0.0f;

			flprogress += 0.1f;

		}
		timesecond1 = gHUD.m_flTime;
	}

	if (flprogress >= 1.0f)
		flprogress = 0.0f;

	if (g_PlayerExtraInfo[id].zombie && flprogress < 1.0f && !g_PlayerExtraInfo[id].dead)
	{
		DrawSmoothCircularFill(x, y, flprogress, stringtext2);
		if ((int)(flprogress * 100.0f) < 10)
		{
			gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
			DrawTexturedNumbersTopRightAligned(*texnumsmallz, m_rctexnumsmallz, (int)(flprogress * 100.0f), x - 6, y + 6, 0.7f);
			DrawUtils::DrawHudString(x + 6, y + 6, ScreenWidth, "%", 255, 0, 0, 255, 1.0f);
		}
		else
		{
			gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
			DrawTexturedNumbersTopRightAligned(*texnumsmallz, m_rctexnumsmallz, (int)(flprogress * 100.0f), x - 2, y + 6, 0.7f);
			DrawUtils::DrawHudString(x + 8, y + 6, ScreenWidth, "%", 255, 0, 0, 255, 1.0f);
		}
	}	
	else if(!g_PlayerExtraInfo[id].dead)
	{
		DrawSmoothCircularFill(x, y, flprogress, stringtext);

		if (flprogress < 1.0f)
		{
			if ((int)(flprogress * 100.0f) < 10)
			{
				gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
				DrawTexturedNumbersTopRightAligned(*texnumsmallh, m_rctexnumsmallh, (int)(flprogress * 100.0f), x - 6, y + 6, 0.7f);
				DrawUtils::DrawHudString(x + 6, y + 6, ScreenWidth, "%", 66, 170, 255, 255, 1.0f);
			}
			else
			{
				gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
				DrawTexturedNumbersTopRightAligned(*texnumsmallh, m_rctexnumsmallh, (int)(flprogress * 100.0f), x - 2, y + 6, 0.7f);
				DrawUtils::DrawHudString(x + 8, y + 6 , ScreenWidth, "%", 66, 170, 255, 255, 1.0f);
			}		
		}	
	}
		
	if (gHUD.m_flTime - timesecond2 < 1.0f)
	{
		tDeltasecond2 += gHUD.m_flTime - timesecond2;
	}
	if (tNextsecond2 > 0.01f || (gHUD.m_flTime - timesecond2 > 0.01f) || tDeltasecond2 > 0.01f)
	{
		tNextsecond2 = 0.0f;
		tDeltasecond2 = 0.0f;

		if (step < m_flStepEvo)
		{
			step++;
		}
	}
	timesecond2 = gHUD.m_flTime;

	return 1;
}

void CHudWebm4::Settext(int flstep, float flstep2)
{
	step = 0;
	m_flStepEvo += flstep;
	flprogress2 += flstep2;
}