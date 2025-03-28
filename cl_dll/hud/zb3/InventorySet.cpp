/* =================================================================================== *
	  * =================== TechnoSoftware & Valve Developing =================== *
 * =================================================================================== */


#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"
#include "InventorySet.h"

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

int CInventorySet::VidInit(void)
{
	if (!stringtext)
		stringtext = R_LoadTextureShared("resource/hud/zb3/hud_string_bg", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	return 1;
}

int CInventorySet::Draw(float time)
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

	DrawUtils::DrawHudString(x - 190, y2 - 32, ScreenWidth, szbuffer, r, g, b, flScale);

	return 1;
}

void CInventorySet::SetWeaponKit()
{
	m_pCurTexture = stringtext;
	m_flDisplayTime = gHUD.m_flTime;

	//char* Gun = (char*)CVAR_GET_STRING("wpn_getgun");
	//char* Pistol = (char*)CVAR_GET_STRING("wpn_getpistol");
	//char* Knife = (char*)CVAR_GET_STRING("wpn_getknife");
	//char* Grenade = (char*)CVAR_GET_STRING("wpn_getgrenade");

	if(Gun == "weapon_arbalest")
	{
		ClientCmd("cstbuy weapon_arbalest");
	}
	else if(Gun == "weapon_mp7a1c")
	{
		ClientCmd("cstbuy weapon_mp7a1c");
	}
	else if (Gun == "weapon_kriss")
	{
		ClientCmd("cstbuy weapon_kriss");
	}
	else if (Gun == "weapon_thompson")
	{
		ClientCmd("cstbuy weapon_thompson");
	}
	else if (Gun == "weapon_m14ebr")
	{
		ClientCmd("cstbuy weapon_m14ebr");
	}
	else if (Gun == "weapon_k1a")
	{
		ClientCmd("cstbuy weapon_k1a");
	}
	else if (Gun == "weapon_m1887")
	{
		ClientCmd("cstbuy weapon_m1887");
	}
	else if (Gun == "weapon_xm8c")
	{
		ClientCmd("cstbuy weapon_xm8c");
	}
	else if (Gun == "weapon_scarl")
	{
		ClientCmd("cstbuy weapon_scarl");
	}
	else if (Gun == "weapon_tar21")
	{
		ClientCmd("cstbuy weapon_tar21");
	}
	else if (Gun == "weapon_m95")
	{
		ClientCmd("cstbuy weapon_m95");
	}
	else if (Gun == "weapon_wa2000")
	{
		ClientCmd("cstbuy weapon_wa2000");
	}
	else if (Gun == "weapon_as50")
	{
		ClientCmd("cstbuy weapon_as50");
	}
	else if (Gun == "weapon_mg3")
	{
		ClientCmd("cstbuy weapon_mg3");
	}
	else if (Gun == "weapon_cannon")
	{
		ClientCmd("cstbuy weapon_cannon");
	}
	else if (Gun == "weapon_gungnir")
	{
		ClientCmd("cstbuy weapon_gungnir");
	}
	else if (Gun == "weapon_quantum")
	{
		ClientCmd("cstbuy weapon_quantum");
	}
	else if (Gun == "weapon_wondercannon")
	{
		ClientCmd("cstbuy weapon_wondercannon");
	}
	else if (Gun == "weapon_bow")
	{
		ClientCmd("cstbuy weapon_bow");
	}
	else if (Gun == "weapon_chainsaw")
	{
		ClientCmd("cstbuy weapon_chainsaw");
	}
	else if (Gun == "weapon_starchaserar")
	{
		ClientCmd("cstbuy weapon_starchaserar");
	}
	else if (Gun == "weapon_sgdrill")
	{
		ClientCmd("cstbuy weapon_sgdrill");
	}

	if (Pistol == "weapon_voidpistol")
	{
		ClientCmd("cstbuy weapon_voidpistol");
	}
	else if (Pistol == "weapon_infinity")
	{
		ClientCmd("cstbuy weapon_infinity");
	}
	else if (Pistol == "weapon_infinityex1")
	{
		ClientCmd("cstbuy weapon_infinityex1");
	}
	else if (Pistol == "weapon_infinityex2")
	{
		ClientCmd("cstbuy weapon_infinityex2");
	}
	else if (Pistol == "weapon_infinityss")
	{
		ClientCmd("cstbuy weapon_infinityss");
	}
	else if (Pistol == "weapon_infinitysr")
	{
		ClientCmd("cstbuy weapon_infinitysr");
	}
	else if (Pistol == "weapon_infinitysb")
	{
		ClientCmd("cstbuy weapon_infinitysb");
	}
	else if (Pistol == "weapon_gunkata")
	{
		ClientCmd("cstbuy weapon_gunkata");
	}

	if (Knife == "weapon_twinaxes")
	{
		ClientCmd("cstbuy weapon_twinaxes");
	}
	else if (Knife == "weapon_knife")
	{
		ClientCmd("cstbuy weapon_knife");
	}
	else if (Knife == "knife_skullaxe")
	{
		ClientCmd("cstbuy knife_skullaxe");
	}
	else if (Knife == "knife_dragonsword")
	{
		ClientCmd("cstbuy knife_dragonsword");
	}
	else if (Knife == "knife_dualsword")
	{
		ClientCmd("cstbuy knife_dualsword");
	}

	if (Grenade == "Sandalphone");
	{
		ClientCmd("cstbuy weapon_sandalphone");
	}
}