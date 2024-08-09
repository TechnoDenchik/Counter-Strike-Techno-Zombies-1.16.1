#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"
#include "gamemode/mods_const.h"
#include "menuspecifed.h"
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
	x += (rect[0].right - rect[0].left) * (n)*scale * 0.5f;
	return DrawTexturedNumbersTopRightAligned(tex, rect, iNumber, x, y, scale);
}

int CHudZSHEnginnermenu::VidInit(void)
{
	R_InitTexture(newscoreboard, "resource/hud/zb3/hud_scoreboard_bg_zsht_pvp02");
	R_InitTexture(weaponboard, "resource/hud/zb3/weapon_list_new");
	R_InitTexture(healthboard, "resource/hud/zb3/hud_character_bg_bottom");
	R_InitTexture(healthboard2, "resource/hud/zb3/hud_character_bg_top");
	R_InitTexture(ammoboard, "resource/hud/zb3/hud_weapon_bg");
	R_InitTexture(textstring, "resource/hud/zb3/hud_string_bg");

	R_InitTexture(winhm, "resource/hud/zb3/hud_sb_num_big_blue");
	R_InitTexture(winzb, "resource/hud/zb3/hud_sb_num_big_red");
	R_InitTexture(countplayer, "resource/hud/zb3/hud_sb_num_center");
	R_InitTexture(iconround, "resource/hud/zb3/hud_text_icon_round");
	R_InitTexture(iconhm, "resource/hud/zb3/hud_text_icon_hm_left");
	R_InitTexture(iconzb, "resource/hud/zb3/hud_text_icon_zb_right");
	R_InitTexture(iconwinhm, "resource/hud/zb3/humanwin");
	R_InitTexture(iconwinzb, "resource/hud/zb3/zombiewin");
	R_InitTexture(ammocounts, "resource/hud/zb3/hud_character_num");
	R_InitTexture(iconclassmilitia, "resource/hud/zb3/militia");

	BuildNumberRC(m_rcSelfnumber, 18, 22);
	BuildNumberRC(m_rcTeamnumber, 18, 22);
	BuildNumberRC(m_rcAmmocount, 18, 22);
	BuildNumberRC(m_rcToprecord, 11, 13);
	return 1;
}

int CHudZSHEnginnermenu::Draw(float time)
{
	int x = ScreenWidth / 2; // Движение по вертикали
	int y = ScreenHeight / 5; // Движение по горизонтали

	int best_player = gHUD.m_Scoreboard.FindBestPlayer();

	int countHM = gHUD.m_Scoreboard.m_iTeamAlive_CT;
	int countZB = gHUD.m_Scoreboard.m_iTeamAlive_T;

	int scoreCT = gHUD.m_Scoreboard.m_iTeamScore_CT;
	int scoreT = gHUD.m_Scoreboard.m_iTeamScore_T;

	int scoreMax = gHUD.m_Scoreboard.m_iNumTeams;
	int roundNumber = scoreMax ? scoreMax : scoreT + scoreCT + 1;
	int countammo = gHUD.m_Ammo.m_iAmmoLastCheck;

	const float flScale = 0.0f;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);

	newscoreboard->Bind();
	DrawUtils::Draw2DQuadScaled(x - 481 / 3.0, y - 4.5, x + 481 / 3.0, y + 77);

	DrawTexturedNumbersTopCenterAligned(*winhm, m_rcTeamnumber, scoreCT, x + 70, y + 14, 1.20f);
	return 1;
}