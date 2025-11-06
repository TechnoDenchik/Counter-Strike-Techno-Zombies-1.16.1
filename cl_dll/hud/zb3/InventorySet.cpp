/* =================================================================================== *
		   * =================== TechnoSoftware =================== *
	  * ================= Weapon ID System Inventory ================== *
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
	return 1;
}

int CInventorySet::Draw(float time)
{
	return 1;
}

void CInventorySet::SetWeaponKit()
{
	int Gun = CVAR_GET_FLOAT("wpn_getgun");
	int Pistol = CVAR_GET_FLOAT("wpn_getpistol");
	int Knife = CVAR_GET_FLOAT("wpn_getknife");
	int Grenade = CVAR_GET_FLOAT("wpn_getgrenade");


	const char* PrimaryCommands[] = {
	"", // WEAPON_NULL0

	/*EPIC WEAPON*/
	"cstbuy weapon_arbalest",           // WEAPON_HALOGUN (1)
	"cstbuy weapon_wondercannonex",    // WEAPON_WONDERCANNONEX (2)
	"cstbuy weapon_wondercannon",      // WEAPON_WONDERCANNON (3)
	"cstbuy weapon_quantum",         // WEAPON_REVIVEGUN (4)
	"cstbuy weapon_gungnir",           // WEAPON_GUNGNIR (5)

	/*OVERCOMING WEAPON*/
	"cstbuy weapon_huntbow",           // WEAPON_HUNTBOW (6)
	"cstbuy weapon_sgdrill",           // WEAPON_SGDRILL (7)
	"cstbuy weapon_cannon",            // WEAPON_CANNON (8)
	"cstbuy weapon_paladin",           // WEAPON_PALADIN (9)
	"cstbuy weapon_buffm4a1",          // WEAPON_BUFFM4A1 (10)
	"cstbuy weapon_chainsaw",          // WEAPON_CHAINSAW (11)
	"cstbuy weapon_gatling",           // WEAPON_GATLING (12)
	"cstbuy weapon_gatlingex",         // WEAPON_GATLINGEX (13)
	"cstbuy weapon_buffaug",           // WEAPON_BUFFAUG (14)

	/*RARE WEAPON*/
	"cstbuy weapon_starchaserar",      // WEAPON_STARCHASERAR (15)
	"cstbuy weapon_balrog3",           // WEAPON_BALROG3 (16)
	"cstbuy weapon_balrog5",           // WEAPON_BALROG5 (17)
	"cstbuy weapon_balrog7",           // WEAPON_BALROG7 (18)
	"cstbuy weapon_balrog11",          // WEAPON_BALROG11 (19)
	"cstbuy weapon_ethereal",          // WEAPON_ETHEREAL (20)
	"cstbuy weapon_stunrifle",         // WEAPON_STUNRIFLE (21)
	"cstbuy weapon_bow",               // WEAPON_BOW (22)
	"cstbuy weapon_plasmagun",         // WEAPON_PLASMAGUN (23)
	"cstbuy weapon_poisongun",         // WEAPON_POISONGUN (24)

	"cstbuy weapon_coilgun",           // WEAPON_COILGUN (25)
	"cstbuy weapon_broad",             // WEAPON_BROAD (26)
	"cstbuy weapon_svdex",             // WEAPON_SVDEX (27)
	"cstbuy weapon_tbarrel",           // WEAPON_TBARREL (28)

	"cstbuy weapon_kriss",             // WEAPON_KRISS (29)
	"cstbuy weapon_scar",              // WEAPON_SCAR (30)
	"cstbuy weapon_thompson",          // WEAPON_THOMPSON (31)
	"cstbuy weapon_tar21",             // WEAPON_TAR21 (32)
	"cstbuy weapon_mp7a1",             // WEAPON_MP7A1 (33)
	"cstbuy weapon_m14ebr",            // WEAPON_M14EBR (34)
	"cstbuy weapon_guitar",            // WEAPON_GUITAR (35)
	"cstbuy weapon_janus7xmas",        // WEAPON_JANUS7XMAS (36)
	"cstbuy weapon_k1a",               // WEAPON_K1A (37)
	"cstbuy weapon_as50",              // WEAPON_AS50 (38)
	"cstbuy weapon_wa2000",            // WEAPON_WA2000 (39)
	"cstbuy weapon_m95xmas",           // WEAPON_M95XMAS (40)
	"cstbuy weapon_m95",               // WEAPON_M95 (41)
	"cstbuy weapon_m134ex",            // WEAPON_M134EX (42)
	"cstbuy weapon_m134hero",          // WEAPON_M134HERO (43)
	"cstbuy weapon_m134xmas",          // WEAPON_M134XMAS (44)
	"cstbuy weapon_m1887",             // WEAPON_M1887 (45)
	"cstbuy weapon_scarh",             // WEAPON_SCARH (46)
	"cstbuy weapon_scarl",             // WEAPON_SCARL (47)
	"cstbuy weapon_spas12ex",          // WEAPON_SPAS12EX (48)
	"cstbuy weapon_stg44",             // WEAPON_STG44 (49)
	"cstbuy weapon_violingun",         // WEAPON_VIOLINGUN (50)
	"cstbuy weapon_hk23",              // WEAPON_HK23 (51)
	"cstbuy weapon_k3",                // WEAPON_K3 (52)
	"cstbuy weapon_m60e4",             // WEAPON_M60E4 (53)
	"cstbuy weapon_mp5tiger",          // WEAPON_MP5TIGER (54)
	"cstbuy weapon_pkm",               // WEAPON_PKM (55)

	/*CLASSIC WEAPON*/
	"cstbuy weapon_ak47dragon",        // WEAPON_AK47DRAGON (56)
	"cstbuy weapon_m4a1dragon",        // WEAPON_M4A1DRAGON (57)
	"cstbuy weapon_tmpdragon",         // WEAPON_TMPDRAGON (58)
	"cstbuy weapon_p90lapin",          // WEAPON_P90LAPIN (59)
	"cstbuy weapon_mg3",               // WEAPON_MG3 (60)
	"cstbuy weapon_xm8",               // WEAPON_XM8 (61)
	"cstbuy weapon_m2",                // WEAPON_M2 (62)
	"cstbuy weapon_ak47",              // WEAPON_AK47 (63)
	"cstbuy weapon_aug",               // WEAPON_AUG (64)
	"cstbuy weapon_awp",               // WEAPON_AWP (65)
	"cstbuy weapon_famas",             // WEAPON_FAMAS (66)
	"cstbuy weapon_g3sg1",             // WEAPON_G3SG1 (67)
	"cstbuy weapon_galil",             // WEAPON_GALIL (68)
	"cstbuy weapon_m3",                // WEAPON_M3 (69)
	"cstbuy weapon_m4a1",              // WEAPON_M4A1 (70)
	"cstbuy weapon_m249",              // WEAPON_M249 (71)
	"cstbuy weapon_mac10",             // WEAPON_MAC10 (72)
	"cstbuy weapon_mp5",               // WEAPON_MP5 (73)
	"cstbuy weapon_p90",               // WEAPON_P90 (74)
	"cstbuy weapon_scout",             // WEAPON_SCOUT (75)
	"cstbuy weapon_sg550",             // WEAPON_SG550 (76)
	"cstbuy weapon_sg552",             // WEAPON_SG552 (77)
	"cstbuy weapon_tmp",               // WEAPON_TMP (78)
	"cstbuy weapon_ump45",             // WEAPON_UMP45 (79)
	"cstbuy weapon_xm1014",            // WEAPON_XM1014 (80)
	};

	const char* SecondaryCommands[] = { // ÈÑÏÐÀÂËÅÍÎ: SecondaryCommands âìåñòî SecondaruCommands
		"",
		"cstbuy weapon_voidpistol",
		"cstbuy weapon_voidpistolex",
		"cstbuy weapon_desperado",
		"cstbuy weapon_gunkata",
		"cstbuy weapon_balrog1",
		"cstbuy weapon_deagled",
		"cstbuy weapon_infinity",
		"cstbuy weapon_infinityex1",
		"cstbuy weapon_infinityex2",
		"cstbuy weapon_infinitysb",
		"cstbuy weapon_infinitysr",
		"cstbuy weapon_infinityss",
		"cstbuy weapon_deagle",
		"cstbuy weapon_elite",
		"cstbuy weapon_fiveseven",
		"cstbuy weapon_glock18",
		"cstbuy weapon_usp",
	};

	const char* MeleeCommands[] = { // ÈÑÏÐÀÂËÅÍÎ: MeleeCommands âìåñòî MeeleCommands
		"",
		"cstbuy weapon_twinaxesex",
		"cstbuy weapon_twinaxes",
		"cstbuy knife_dualsword",
		"cstbuy knife_stormgiant",
		"cstbuy knife_balrog9",
		"cstbuy knife_zsh_clawhammer",
		"cstbuy knife_knifedragon",
		"cstbuy knife_katanad",
		"cstbuy knife_nataknifed",
		"cstbuy knife_snakesword",
		"cstbuy knife_thanatos9",
		"cstbuy knife_dragonsword",
		"cstbuy knife_katana",
		"cstbuy knife_skullaxe",
		"cstbuy weapon_knife",
	};

	const char* GrenadeCommands[] = {
		"",
		"cstbuy weapon_sbmine",
		"cstbuy weapon_hegrenade",
		"cstbuy weapon_flashbang",
		"cstbuy weapon_smokegrenade",
	};

	// Âûäàåì òîëüêî âûáðàííîå îðóæèå
	if (Gun >= 1 && Gun <= 79) {
		ClientCmd(PrimaryCommands[Gun]);
	}

	if (Pistol >= 1 && Pistol <= 17) {
		ClientCmd(SecondaryCommands[Pistol]); // ÈÑÏÐÀÂËÅÍÎ: SecondaryCommands âìåñòî PrimaryCommands
	}

	if (Knife >= 1 && Knife <= 16) {
		ClientCmd(MeleeCommands[Knife]); // ÈÑÏÐÀÂËÅÍÎ: MeleeCommands âìåñòî MeeleCommands
	}

	if (Grenade >= 1 && Grenade <= 4) {
		ClientCmd(GrenadeCommands[Grenade]);
	}
}

int CHeroSet::VidInit(void)
{
	if (!stringtext)
		stringtext = R_LoadTextureShared("resource/hud/zb3/zmrewalk_hero", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	return 1;
}

int CHeroSet::Draw(float time)
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

void CHeroSet::SetHero()
{
	m_pCurTexture = stringtext;
	m_flDisplayTime = gHUD.m_flTime;
}