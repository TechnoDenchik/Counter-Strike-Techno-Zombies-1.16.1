/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "hud.h"
#include "cl_util.h"
#include "draw_util.h"
#include "eventscripts.h"
#include "triangleapi.h"
#include "player/player_const.h"
#include "gd/GDScoreboard.h"

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

int CHudGDScoreboard::VidInit(void)
{
	R_InitTexture(newscoreboard, "resource/hud/hud_scoreboard_bg_gundeath");
	R_InitTexture(winhm, "resource/hud/zb3/hud_sb_num_big_blue");
	R_InitTexture(winzb, "resource/hud/zb3/hud_sb_num_big_red");
	R_InitTexture(slash, "resource/hud/hud_sb_num_center_slash");
	R_InitTexture(iconlv, "resource/hud/hud_text_icon_level");
	R_InitTexture(iconlvmax, "resource/hud/hud_text_icon_maxlv");
	R_InitTexture(countround, "resource/hud/zb3/hud_sb_num_center");
	R_InitTexture(countkill, "resource/hud/zb3/hud_sb_num_center");
	R_InitTexture(countplayer, "resource/hud/hud_sb_num_small_blue");
	R_InitTexture(countplayer2, "resource/hud/hud_sb_num_small_red");
	R_InitTexture(iconct, "resource/hud/zb3/hud_text_icon_ct_left");
	R_InitTexture(icont, "resource/hud/zb3/hud_text_icon_tr_right");

	BuildNumberRC(m_rcSelfnumber, 18, 22);
	BuildNumberRC(m_rcTeamnumber, 18, 22);
	BuildNumberRC(m_rcToprecord, 11, 13);
	BuildNumberRC(m_rcToprecord2, 8, 11);
	BuildNumberRC(m_rcToprecord3, 18, 22);
	BuildNumberRC(m_rcroundmax, 11, 13);
	BuildNumberRC(m_rcroundnumber, 11, 13);

	return 1;
}

int CHudGDScoreboard::Draw(float time)
{
	int x = ScreenWidth / 2; // �������� �� �����������
	int y = 5; // �������� �� ���������

	int x3 = ScreenWidth / 2;
	int y3 = 10;

	int x4 = ScreenWidth / 2;
	int y4 = 10;

	int x5 = ScreenWidth / 2;
	int y5 = 60;

	int x6 = ScreenWidth / 2;

	int x9 = ScreenWidth / 2;
	int y9 = 20;

	int x10 = ScreenWidth / 1.85;

	int x11 = ScreenWidth / 2.17;
	int y11 = 10;

	int x15 = ScreenWidth / 2;
	int y15 = 43;

	int x16 = ScreenWidth / 2;
	int y16 = 20.5;

	int x17 = ScreenWidth / 2.0144;
	int y17 = ScreenHeight / 52;
	
	const int r = 255, g = 255, b = 255;
	const float flScale = 0.0f;

	int best_player = gHUD.m_Scoreboard.FindBestPlayer();
	int idx = gEngfuncs.GetLocalPlayer()->index;

	int countHM = gHUD.m_Scoreboard.m_iTeamAlive_CT;
	int countZB = gHUD.m_Scoreboard.m_iTeamAlive_T;

	int scoreCT = gHUD.m_Scoreboard.m_iTeamScore_CT;
	int scoreT = gHUD.m_Scoreboard.m_iTeamScore_T;

	int scoreMax = gHUD.m_Scoreboard.m_iNumTeams;
	int roundNumber = scoreMax ? scoreMax : scoreT + scoreCT - 1;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);

	newscoreboard->Bind();
	DrawUtils::Draw2DQuadScaled(x - 450 / 3.0, y - 4.6, x + 450 / 3.0, y + 78);

	char szBuffer[64];
	sprintf(szBuffer, "%d", lv);

	if (lv < 18)
	{
		iconlv->Bind();
		DrawUtils::Draw2DQuadScaled(x9 - 39.7, y9 - 1.7, x9 + 39.7, y9 + 9.63);

		slash->Bind();
		DrawUtils::Draw2DQuadScaled(x15 - 4.0, y15 - 8.7, x15 + 4.0, y15 + 5.0);

		DrawTexturedNumbersTopRightAligned(*countround, m_rcToprecord, lv, x16 - 24, y16 + 14, 1.0f);
		DrawTexturedNumbersTopRightAligned(*countround, m_rcToprecord, 18, x16 + 19, y16 + 14, 1.0f);
	}
	else
	{
		iconlvmax->Bind();
		DrawUtils::Draw2DQuadScaled(x17 - 38.7, y17 - 1.7, x17 + 38.7, y17 + 9.63);
	}

	iconct->Bind();
	DrawUtils::Draw2DQuadScaled(x10 - 34.7, y11 - 1.7, x10 + 34.7, y11 + 10.7);

	icont->Bind();
	DrawUtils::Draw2DQuadScaled(x11 - 34.7, y11 - 1.7, x11 + 34.7, y11 + 10.7);

	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
	if (gHUD.m_Scoreboard.m_iTeamScore_CT < 10)
	{
		DrawTexturedNumbersTopRightAligned(*winhm, m_rcTeamnumber, gHUD.m_Scoreboard.m_iTeamScore_CT, x4 + 70, y4 + 14, 1.0f);
		DrawTexturedNumbersTopRightAligned(*winhm, m_rcTeamnumber, 0, x4 + 50, y4 + 14, 1.0f);
	}
	else if (gHUD.m_Scoreboard.m_iTeamScore_CT < 100)
	{
		 DrawTexturedNumbersTopRightAligned(*winhm, m_rcTeamnumber, gHUD.m_Scoreboard.m_iTeamScore_CT, x4 + 75, y4 + 14, 1.0f);
	}
	else
	{
		DrawTexturedNumbersTopRightAligned(*winhm, m_rcTeamnumber, gHUD.m_Scoreboard.m_iTeamScore_CT, x4 + 87, y4 + 14, 1.0f);
	}

	if (gHUD.m_Scoreboard.m_iTeamScore_T < 10)
	{
		DrawTexturedNumbersTopRightAligned(*winzb, m_rcSelfnumber, gHUD.m_Scoreboard.m_iTeamScore_T, x3 - 68, y3 + 14, 1.0f);
		DrawTexturedNumbersTopRightAligned(*winzb, m_rcSelfnumber, 0, x3 - 88, y3 + 14, 1.0f);
	}
	else if (gHUD.m_Scoreboard.m_iTeamScore_T < 100)
	{
		DrawTexturedNumbersTopRightAligned(*winzb, m_rcSelfnumber, gHUD.m_Scoreboard.m_iTeamScore_T, x3 - 77, y3 + 14, 1.0f);
	}
	else
	{
		DrawTexturedNumbersTopRightAligned(*winzb, m_rcSelfnumber, gHUD.m_Scoreboard.m_iTeamScore_T, x3 - 78, y3 + 14, 1.0f);
	}

	DrawTexturedNumbersTopRightAligned(*countplayer, m_rcToprecord2, countHM, x5 + 77, y5, 1.0f);
	DrawTexturedNumbersTopRightAligned(*countplayer2, m_rcToprecord2, countZB, x6 - 87, y5, 1.0f);

	return 1;
}