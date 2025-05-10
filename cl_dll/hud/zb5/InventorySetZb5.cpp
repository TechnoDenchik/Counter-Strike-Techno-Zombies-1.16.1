/* =================================================================================== *
		   * =================== TechnoSoftware =================== *
	  * ================= Weapon ID System Inventory ================== *
 * =================================================================================== */


#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"
#include "InventorySetZb5.h"

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

int CInventorySetZb5::VidInit(void)
{
	return 1;
}

int CInventorySetZb5::Draw(float time)
{
	return 1;
}

void CInventorySetZb5::SetWeaponKit()
{
	int Gun = CVAR_GET_FLOAT("wpn_getgun");
	int Pistol = CVAR_GET_FLOAT("wpn_getpistol");
	int Knife = CVAR_GET_FLOAT("wpn_getknife");
	int Grenade = CVAR_GET_FLOAT("wpn_getgrenade");

	if(Gun == 1)
	{
		ClientCmd("cstbuy weapon_arbalest");
	}
	else if(Gun == 2)
	{
		ClientCmd("cstbuy weapon_wondercannonex");
	}
	else if (Gun == 3)
	{
		ClientCmd("cstbuy weapon_wondercannon");
	}
	else if (Gun == 4)
	{
		ClientCmd("cstbuy weapon_quantum");
	}
	else if (Gun == 5)
	{
		ClientCmd("cstbuy weapon_gungnir");
	}
	else if (Gun == 6)
	{
		ClientCmd("cstbuy weapon_sgdrill");
	}
	else if (Gun == 7)
	{
		ClientCmd("cstbuy weapon_cannon");
	}
	else if (Gun == 8)
	{
		ClientCmd("cstbuy weapon_paladin");
	}
	else if (Gun == 9)
	{
		ClientCmd("cstbuy weapon_buffm4a1");
	}
	else if (Gun == 10)
	{
		ClientCmd("cstbuy weapon_chainsaw");
	}
	else if (Gun == 11)
	{
		ClientCmd("cstbuy weapon_starchaserar");
	}
	else if (Gun == 12)
	{
		ClientCmd("cstbuy weapon_bow");
	}
	else if (Gun == 13)
	{
		ClientCmd("cstbuy weapon_thompson");
	}
	else if (Gun == 14)
	{
		ClientCmd("cstbuy weapon_tar21");
	}
	else if (Gun == 15)
	{
		ClientCmd("cstbuy weapon_mp7a1c");
	}
	else if (Gun == 16)
	{
		ClientCmd("cstbuy weapon_mg3");
	}
	else if (Gun == 17)
	{
		ClientCmd("cstbuy weapon_m1887");
	}
	else if (Gun == 18)
	{
		ClientCmd("cstbuy weapon_m95");
	}
	else if (Gun == 19)
	{
		ClientCmd("cstbuy weapon_m14ebr");
	}
	else if (Gun == 20)
	{
		ClientCmd("cstbuy weapon_kriss");
	}
	else if (Gun == 21)
	{
		ClientCmd("cstbuy weapon_scarl");
	}
	else if (Gun == 22)
	{
		ClientCmd("cstbuy weapon_wa2000");
	}
	else if (Gun == 23)
	{
		ClientCmd("cstbuy weapon_xm8c");
	}
	else if (Gun == 24)
	{
		ClientCmd("cstbuy weapon_as50");
	}

	if (Pistol == 1)
	{
		ClientCmd("cstbuy weapon_voidpistolex");
	}
	else if (Pistol == 2)
	{
		ClientCmd("cstbuy weapon_voidpistol");
	}
	else if (Pistol == 3)
	{
		ClientCmd("cstbuy weapon_infinity");
	}
	else if (Pistol == 4)
	{
		ClientCmd("cstbuy weapon_infinityex1");
	}
	else if (Pistol == 5)
	{
		ClientCmd("cstbuy weapon_infinityex2");
	}
	else if (Pistol == 6)
	{
		ClientCmd("cstbuy weapon_infinitysb");
	}
	else if (Pistol == 7)
	{
		ClientCmd("cstbuy weapon_infinitysr");
	}
	else if (Pistol == 8)
	{
		ClientCmd("cstbuy weapon_infinityss");
	}

	if (Knife == 1)
	{
		ClientCmd("cstbuy weapon_twinaxesex");	
	}
	else if (Knife == 2)
	{
		ClientCmd("cstbuy weapon_twinaxes");
	}
	else if (Knife == 3)
	{
		ClientCmd("cstbuy knife_dualsword");
	}
	else if (Knife == 4)
	{
		ClientCmd("cstbuy knife_dragonsword");	
	}
	else if (Knife == 5)
	{
		ClientCmd("cstbuy knife_skullaxe");
	}

	if (Grenade == 1);
	{
		ClientCmd("cstbuy weapon_sandalphone");
	}	
}

int CHeroSetZb5::VidInit(void)
{
	if (!stringtext)
		stringtext = R_LoadTextureShared("resource/hud/zb3/zmrewalk_hero", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	return 1;
}

int CHeroSetZb5::Draw(float time)
{
	if (!m_pCurTexture)
		return 1;

	if (time > m_flDisplayTime + 5.0f)
	{
		m_pCurTexture = nullptr;
		return 1;
	}

	int x = ScreenWidth / 2;
	int y = ScreenHeight / 4;
	const float flScale = 0.0f;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255 * std::min(5.0f - (time - m_flDisplayTime), 1.0f));
	m_pCurTexture->Bind();
	DrawUtils::Draw2DQuadScaled(x - 881 / 3.0, y - 3.5, x + 881 / 3.0, y + 215);

	return 1;
}

void CHeroSetZb5::SetHero()
{
	m_pCurTexture = stringtext;
	m_flDisplayTime = gHUD.m_flTime;
}