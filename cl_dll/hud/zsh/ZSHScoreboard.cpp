/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"
#include "gamemode/mods_const.h"
#include "ZSHScoreboard.h"
#include "zsh.h"
#include "eventscripts.h"


inline void BuildNumberRC(wrect_t(&rgrc)[10], int w, int h)
{
	int nw = 0;

	for (int i = 1; i < 10; i++)
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
	return BuildNumberRC(rgrc, w / 2, h);
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

inline void DrawTexturePart2(const CTextureRef& tex, const wrect_t& rect, int x1, int y1, float scale = 1.0f)
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

inline int DrawTexturedNumbersTopRightAligned2(const CTextureRef& tex, const wrect_t(&rect)[10], char* const iNumber, int x, int y, float scale = 1.0f)
{
	assert(*iNumber >= 0);

	do
	{
		char k = *iNumber % 10;
		*iNumber /= 10;
		DrawTexturePart2(tex, rect[k], x, y, scale);
		x -= (rect[k].right - rect[k].left) * scale;
	} while (*iNumber > 0);

	return x;
}

inline unsigned math_log10(unsigned v)
{
	return (v >= 1000000000) ? 9 : (v >= 100000000) ? 8 : (v >= 10000000) ? 7 :
		(v >= 1000000) ? 6 : (v >= 100000) ? 5 : (v >= 10000) ? 4 :
		(v >= 1000) ? 3 : (v >= 100) ? 2 : (v >= 10) ? 1 : 0;
}

inline unsigned math_log102(unsigned v)
{
	return (v >= 1000000000) ? 9 : (v >= 100000000) ? 8 : (v >= 10000000) ? 7 :
		(v >= 1000000) ? 6 : (v >= 100000) ? 5 : (v >= 10000) ? 4 :
		(v >= 1000) ? 3 : (v >= 100) ? 2 : (v >= 10) ? 1 : 0;
}

inline int DrawTexturedNumbersTopCenterAligned(const CTextureRef& tex, const wrect_t(&rect)[10], int iNumber, int x, int y, float scale = 1.0f)
{
	int n = math_log10(iNumber);
	x += (rect[0].right - rect[0].left) * (n)*scale * 0.5f;
	return DrawTexturedNumbersTopRightAligned(tex, rect, iNumber, x, y, scale);
}

inline int DrawTexturedNumbersTopCenterAligned2(const CTextureRef& tex, const wrect_t(&rect)[10], char* const iNumber, int x, int y, float scale = 1.0f)
{
	int n = math_log102(*iNumber);
	x += (rect[0].right - rect[0].left) * (n)*scale * 0.5f;
	return DrawTexturedNumbersTopRightAligned2(tex, rect, iNumber, x, y, scale);
}

int CHudZSHScoreboard::VidInit(void)
{
	R_InitTexture(newscoreboardzsh, "resource/hud/hud_scoreboard_bg_zsht_pvp01");
	R_InitTexture(zsht_gungergauge_bg, "resource/shelterteam/zsht_ingame_gungergauge_bg");

	R_InitTexture(zsht_moon_icon, "resource/shelterteam/zsht_ingame_moon_icon");
	R_InitTexture(zsht_sun_icon, "resource/shelterteam/zsht_ingame_sun_icon");
	R_InitTexture(noisegauge_bg, "resource/shelterteam/ingame_noisegauge_bg");
	R_InitTexture(noisegauge_bg_red, "resource/shelterteam/ingame_noisegauge_bg_red");
	R_InitTexture(survivalfail, "resource/shelterteam/zsh_survivalfail");
	R_InitTexture(survivalsuccess, "resource/shelterteam/zsh_survivalsuccess");
	R_InitTexture(count, "resource/hud/hud_sb_num_big_white");
	R_InitTexture(textstring, "resource/hud/zb3/hud_string_bg");	
	R_InitTexture(countplayer, "resource/hud/zb3/hud_sb_num_center");
	R_InitTexture(countplayer2, "resource/hud/hud_sb_num_human_small");
	
  /*R_InitTexture(, "resource/shelterteam/");
	R_InitTexture(, "resource/shelterteam/");
	R_InitTexture(, "resource/shelterteam/");
	R_InitTexture(, "resource/shelterteam/");
	R_InitTexture(, "resource/shelterteam/");*/

	BuildNumberRC(m_rcSelfnumber, 18, 22);
	BuildNumberRC(m_rcTeamnumber, 18, 22);
	BuildNumberRC(m_rcAmmocount, 18, 22);
	BuildNumberRC(m_rcToprecord, 11, 13);
	BuildNumberRC(m_rcToprecord2, 10, 14);

	return 1;
}

int CHudZSHScoreboard::Draw(float time)
{
	int x = ScreenWidth / 2; // Движение по вертикали
	int y = ScreenHeight / 166; // Движение по горизонтали

	int x2 = ScreenWidth / 2;
	int y2 = ScreenHeight / 1.050;

	double x3 = ScreenWidth / 1.940;
	int y3 = ScreenHeight / 26;

	double x4 = ScreenWidth / 2.1;
	double x5 = ScreenWidth / 2.3;
	double x6 = ScreenWidth / 2.6;

	int x7 = ScreenWidth / 1.905; 
	int x8 = ScreenWidth / 1.970; 
	int x9 = ScreenWidth / 1.980; 
	int x10 = ScreenWidth / 2.0; 
	int x11 = ScreenWidth / 2; 
	int x12 = ScreenWidth / 2;
	int x13 = ScreenWidth / 1.980; 
	int x14 = ScreenWidth / 1.980; 
	int x15 = ScreenWidth / 1.980; 
	int x16 = ScreenWidth / 1.980;
	int x17 = ScreenWidth / 1.980; 

	int y4 = ScreenHeight / 88; 
	int y8 = ScreenHeight / 30; 
	int y9 = ScreenHeight / 55; 
	int y10 = ScreenHeight / 1.027; 
	int y11 = ScreenHeight / 30; 
	int y12 = ScreenHeight / 30; 
	
	int best_player = gHUD.m_Scoreboard.FindBestPlayer();

	int countHM = gHUD.m_Scoreboard.m_iTeamAlive_CT;
	int countZB = gHUD.m_Scoreboard.m_iTeamAlive_T;

	int scoreCT = gHUD.m_Scoreboard.m_iTeamScore_CT;
	int scoreT = gHUD.m_Scoreboard.m_iTeamScore_T;

	int scoreMax = gHUD.m_Scoreboard.m_iNumTeams;
	int roundNumber = scoreMax ? scoreMax : scoreT + scoreCT + 1;

	const float flScale = 0.010f;
	const int r = 255, g = 255, b = 255;
	const int r2 = 89, g2 = 59, b2 = 2;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);

	newscoreboardzsh->Bind();
	DrawUtils::Draw2DQuadScaled(x - 540 / 3.0, y - 4.5, x + 540 / 3.0, y + 78);

	zsht_gungergauge_bg->Bind();
	DrawUtils::Draw2DQuadScaled(x2 - 420 / 3.0, y2 - 3.5, x2 + 420 / 3.0, y2 + 45);

	zsht_sun_icon->Bind();
	DrawUtils::Draw2DQuadScaled(x7 - 34, y4 - 3.5, x7 + 34, y4 + 23);

	//zsht_moon_icon->Bind();
	//DrawUtils::Draw2DQuadScaled(x7 - 34, y4 - 3.5, x7 + 34, y4 + 23);

	char szBuffer[64];
	
	sprintf(szBuffer, "Day %d", days);
	DrawUtils::DrawHudString2(x9 + 20, y8 + 5, ScreenWidth, szBuffer, r, g, b, flScale);

	DrawTexturedNumbersTopRightAligned(*count, m_rcSelfnumber, 0, x3 + 80, y3, 0.70f);
	DrawTexturedNumbersTopRightAligned(*count, m_rcSelfnumber, 0, x4 + 36, y3, 0.70f);
	DrawTexturedNumbersTopRightAligned(*count, m_rcSelfnumber, 0, x5 + 55, y3, 0.70f);
	DrawTexturedNumbersTopRightAligned(*count, m_rcSelfnumber, 0, x6 + 88, y3, 0.70f);

	DrawTexturedNumbersTopRightAligned(*countplayer2, m_rcToprecord2, 0, x10 - 130, y9, 1.0f);
	DrawTexturedNumbersTopRightAligned(*countplayer2, m_rcToprecord2, 0, x11 - 66, y9, 1.0f);
	DrawTexturedNumbersTopRightAligned(*countplayer2, m_rcToprecord2, 0, x12 - 7, y9, 1.0f);

	char szBuffer2[64];
	char szBuffer3[64];
	char szBuffer4[64];
	char szBuffer5[64];
	int maxwoods2 = 15;
	int maxmetal2 = 13;

	gEngfuncs.pTriAPI->Color4ub( 140, 92, 3, 200);

	sprintf(szBuffer2, "%d", maxwoods);
	DrawTexturedNumbersTopRightAligned(*countplayer2, m_rcToprecord2, maxwoods2, x13 - 20, y10, 1.0f);

	sprintf(szBuffer3, "%d", maxmetal);
	DrawTexturedNumbersTopRightAligned(*countplayer2, m_rcToprecord2, maxmetal2, x14 + 94, y10, 1.0f); //94

	sprintf(szBuffer4, "%d", wood);
	DrawTexturedNumbersTopRightAligned(*countplayer2, m_rcToprecord2, wood, x13 - 60, y10, 1.0f);

	sprintf(szBuffer5, "%d", meat);
	DrawTexturedNumbersTopRightAligned(*countplayer2, m_rcToprecord2, meat, x14 + 50, y10, 1.0f);
	
	//DrawTexturedNumbersTopRightAligned(*countplayer2, m_rcToprecord2, metal, x14 + 78, y10, 1.0f);
	//DrawTexturedNumbersTopRightAligned(*countplayer2, m_rcToprecord2, 0, x15 + 55, y10, 1.0f);
	//DrawTexturedNumbersTopRightAligned(*countplayer2, m_rcToprecord2, 0, x16 + 88, y10, 1.0f);
	//DrawTexturedNumbersTopRightAligned(*countplayer2, m_rcToprecord2, 0, x17 + 55, y10, 1.0f);

	return 1;
}