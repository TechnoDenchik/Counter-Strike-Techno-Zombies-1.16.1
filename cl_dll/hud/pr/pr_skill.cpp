#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"

#include "pr.h"
#include "pr_skill.h"

#include <algorithm>
#include <functional>

struct CHudPR_Skill::Config
{
	static const char * const ZOMBIE_SKILL_HUD_ICON_NEW[MAX_ZOMBIE_SKILL];
	static const char * const ZOMBIE_CLASS_HUD_ICON_NEW[MAX_ZOMBIE_CLASS];
	static const char * const ZOMBIE_ITEM_HUD_ICON[2][3];
};

const char *  const CHudPR_Skill::Config::ZOMBIE_SKILL_HUD_ICON_NEW[MAX_ZOMBIE_SKILL] =
{
		"", // ZOMBIE_SKILL_EMPTY
		"resource/zb3/humanskill_hm_spd", // ZOMBIE_SKILL_SPRINT
		"resource/zb3/humanskill_hm_hd", // ZOMBIE_SKILL_HEADSHOT
		"resource/zb3/humanskill_hm_2x", // ZOMBIE_SKILL_KNIFE2X
		"resource/zb3/zombieskill_zombicrazy", // ZOMBIE_SKILL_CRAZY,
		"resource/zb3/zombieskill_zombihiding", // ZOMBIE_SKILL_HIDE,
		"resource/zb3/zombieskill_zombitrap", // ZOMBIE_SKILL_TRAP,
		"resource/zb3/zombieskill_zombismoke", // ZOMBIE_SKILL_SMOKE,
		"resource/zb3/zombieskill_zombiheal", // ZOMBIE_SKILL_HEAL,
		"resource/zb3/zombieskill_zombideimos", // ZOMBIE_SKILL_SHOCK,
		"resource/zb3/zombieskill_zombicrazy2", // ZOMBIE_SKILL_CRAZY2,
		"resource/zb3/zombieskill_zombihook", // ZOMBIE_SKILL_RATS,
		"resource/zb3/zombieskill_zombipileexp", // ZOMBIE_SKILL_STAMP,
		"resource/zb5/zombieskill_dogshoot",
		"resource/zb5/zombieskill_rush",
		"resource/zb5/zombieskill_protect",
		"resource/zb5/zombieskill_chargeslash",
		"resource/zb5/zombieskill_webshooter", 
		"resource/zb5/zombieskill_webbomb",

		"resource/zb3/zombieskill_charge",
		"resource/zb3/zombieskill_discharge",
		"resource/zb3/zombieskill_strengthrecovery",

		"resource/zb3/zombieskill_zombiheal",
		"resource/zb3/zombieskill_zombiguard",

		"resource/zb3/zombieskill_booster",
		"resource/zb3/zombieskill_jumpupm",

		"resource/zb3/zombieskill_zombistiffen",
		"resource/zb3/zombieskill_strengthrecovery",

		"resource/zb3/zombieskill_glide",
		"resource/zb3/zombieskill_dive",
		"resource/zb3/zombieskill_zombicrazy",

		"resource/zb3/zombieskill_zombipenetration",
		"resource/zb3/zombieskill_jumpupm",
};

const char *  const CHudPR_Skill::Config::ZOMBIE_CLASS_HUD_ICON_NEW[MAX_ZOMBIE_CLASS] =
{
		"", // ZOMBIE_CLASS_HUMAN
		"resource/zb3/zombietype_defaultzb", // ZOMBIE_CLASS_TANK,
		"resource/zb3/zombietype_lightzb", // ZOMBIE_CLASS_SPEED,
		"resource/zb3/zombietype_heavyzb", // ZOMBIE_CLASS_HEAVY,
		"resource/zb3/zombietype_pczb", // ZOMBIE_CLASS_PC,
		"resource/zb3/zombietype_doctorzb", // ZOMBIE_CLASS_HEAL,
		"resource/zb3/zombietype_deimoszb", // ZOMBIE_CLASS_DEIMOS,
		"resource/zb3/zombietype_deimos2zb", // ZOMBIE_CLASS_DEIMOS2,
		"resource/zb3/zombietype_witchzb", // ZOMBIE_CLASS_BANCHEE,
		"resource/zb3/zombietype_undertakerzb", // ZOMBIE_CLASS_STAMPER,
		"resource/zb5/zombietype_meatwallzb", // ZOMBIE_CLASS_MEATWALL for mode zb5,
		"resource/zb5/ZombieType_deathknightzb", // ZOMBIE_CLASS_DEATHKNIGHT for mode zb5,
		"resource/zb5/zombietype_spiderzb", // ZOMBIE_CLASS_SPIDER for mode zb5,
		"resource/zb3/zombietype_akshazb", // ZOMBIE_CLASS_AKSHA,
		"resource/zb3/zombietype_boomerzb",// ZOMBIE_CLASS_BOOMER,
		"resource/zb3/zombietype_boosterzb", // ZOMBIE_CLASS_BOOSTER,
		"resource/zb3/zombietype_chinazb", // ZOMBIE_CLASS_CHINSEE,
		"resource/zb3/zombietype_flyingzb",// ZOMBIE_CLASS_FLYING,
		"resource/zb3/zombietype_residentzb",// ZOMBIE_CLASS_RESIDENT,
};

const char *  const CHudPR_Skill::Config::ZOMBIE_ITEM_HUD_ICON[2][3] =
{
		{ "zombiACT", "zombiBCT", "zombiHCT" },
		{ "zombiATER", "zombiBTER", "zombiHTER" }
};

CHudPR_Skill::CHudPR_Skill(void) :  // 0-init
	m_HUD_zombirecovery(0),
	m_HUD_zombieGKey(0),
	m_HUD_SkillIcons{},
	m_HUD_ClassIcons{},
	m_flRecoveryBeginTime(0.0f),
	m_iCurrentClass(ZOMBIE_CLASS_HUMAN),
	m_ZombieSkillHudIcons{}{}

int CHudPR_Skill::VidInit(void)
{
	m_HUD_zombirecovery = gHUD.GetSpriteIndex("zombirecovery");
	m_HUD_zombieGKey = gHUD.GetSpriteIndex("zombiGkey");

	if(!m_pTexture_skillslotkeybg)
		m_pTexture_skillslotkeybg = R_LoadTextureUnique("resource/zb3/skillslotkeybg");
	if (!m_pTexture_skillslotbg)
		m_pTexture_skillslotbg = R_LoadTextureUnique("resource/zb3/skillslotbg");

	for (int i = 0; i < MAX_ZOMBIE_SKILL; ++i)
	{
		if (Config::ZOMBIE_SKILL_HUD_ICON_NEW[i][0] != '\0')
			if (!m_pTexture_NewSkillIcons[i])
				m_pTexture_NewSkillIcons[i] = R_LoadTextureUnique(Config::ZOMBIE_SKILL_HUD_ICON_NEW[i]);
	}

	for (int i = 0; i < MAX_ZOMBIE_CLASS; ++i)
	{
		if (Config::ZOMBIE_CLASS_HUD_ICON_NEW[i][0] != '\0')
			if (!m_pTexture_NewClassIcons[i])
				m_pTexture_NewClassIcons[i] = R_LoadTextureUnique(Config::ZOMBIE_CLASS_HUD_ICON_NEW[i]);
	}
	return 1;
}

void CHudPR_Skill::Reset(void)
{
	OnSkillInit();
}

int CHudPR_Skill::Draw(float time)
{
	int iHeight = gHUD.GetSpriteRect(m_HUD_zombirecovery).bottom - gHUD.GetSpriteRect(m_HUD_zombirecovery).top;
	int x = 0; // iWidth / 2;
	int y = ScreenHeight - gHUD.m_iFontHeight * 3 / 2 - iHeight;

	x = DrawHealthRecoveryIcon(time, x, y);
	x = DrawSkillBoard(time, x, y);
	DrawSkillTip(time);
	DrawSkillBoardNew(time);

	return 1;
}

void CHudPR_Skill::Think()
{
	for (auto &icon : m_ZombieSkillHudIcons)
	{
		if (icon.m_iCurrentSkillStatus == SKILL_STATUS_USING || icon.m_iCurrentSkillStatus == SKILL_STATUS_FREEZING)
		{
			if (gHUD.m_flTime > icon.m_flTimeSkillReady)
			{
				icon.m_iCurrentSkillStatus = SKILL_STATUS_READY;
				icon.m_flTimeSkillBlink = gHUD.m_flTime + 3.0f;
			}
		}
	}
}

void CHudPR_Skill::OnHealthRecovery()
{
	m_flRecoveryBeginTime = gHUD.m_flTime;
}

int CHudPR_Skill::DrawHealthRecoveryIcon(float time, int x, int y) const
{
	int iWidth = gHUD.GetSpriteRect(m_HUD_zombirecovery).right - gHUD.GetSpriteRect(m_HUD_zombirecovery).left;
	if (time > m_flRecoveryBeginTime + 1.0f)
		return x + iWidth;

	int a = 255.0f - (time - m_flRecoveryBeginTime) * 255.0f;

	int r, g, b;
	DrawUtils::UnpackRGB(r, g, b, RGB_WHITE);
	DrawUtils::ScaleColors(r, g, b, a);

	SPR_Set(gHUD.GetSprite(m_HUD_zombirecovery), r, g, b);
	SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(m_HUD_zombirecovery));

	
	return x + iWidth;
}

int CHudPR_Skill::DrawSkillBoard(float time, int x, int y) const
{
	for (auto &icon : m_ZombieSkillHudIcons)
	{
		x = DrawSkillIcon(time, x, y, icon);
	}
	return x;
}

int CHudPR_Skill::DrawSkillIcon(float time, int x, int y, const ZombieSkillHudIcon &icon) const
{
	if (icon.m_iCurrentSkill == ZOMBIE_SKILL_EMPTY)
		return x;

	int r, g, b;
	DrawUtils::UnpackRGB(r, g, b, RGB_WHITE);
	
	float flPercent = (time - icon.m_flTimeSkillStart) / (icon.m_flTimeSkillReady - icon.m_flTimeSkillStart);
		
	DrawUtils::UnpackRGB(r, g, b, RGB_WHITE);

	return x;
}

void CHudPR_Skill::DrawSkillTip(float time) const
{
	int x = ScreenWidth / 2;
	int y = ScreenHeight * 2 / 3;
}

void CHudPR_Skill::DrawSkillBoardNew(float time) const
{
	int x = ScreenWidth / 2;
	int y = 80;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);

	x -= m_pTexture_skillslotbg->w() / 2;
	if (m_iCurrentClass >= 0 && m_iCurrentClass < MAX_ZOMBIE_CLASS)
	{
		const auto &classicon = m_pTexture_NewClassIcons[m_iCurrentClass];
		if (classicon != nullptr)
		{
			x -= (classicon->w() + 4) / 2;

			gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
			classicon->Bind();
			DrawUtils::Draw2DQuadScaled(x, y, x + classicon->w(), y + classicon->h());

			x += classicon->w() + 4;
		}
	}

	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
	m_pTexture_skillslotbg->Bind();
	DrawUtils::Draw2DQuadScaled(x, y, x + m_pTexture_skillslotbg->w(), y + m_pTexture_skillslotbg->h());

	char SkillKey = 'G';
	if (m_iCurrentClass == ZOMBIE_CLASS_HUMAN)
		SkillKey = '5';

	for (auto &icon : m_ZombieSkillHudIcons)
	{
		const auto &skillicon = m_pTexture_NewSkillIcons[icon.m_iCurrentSkill];
		if (skillicon && icon.m_iCurrentSkill >= 0 && icon.m_iCurrentSkill < MAX_ZOMBIE_SKILL)
		{
			if (time < icon.m_flTimeSkillReady)
			{
				// waiting for freezing
				float flPercent = (time - icon.m_flTimeSkillStart) / (icon.m_flTimeSkillReady - icon.m_flTimeSkillStart);

				float center_y = y + skillicon->h() * (1.0f - flPercent);

				// top half
				gEngfuncs.pTriAPI->Color4ub(255, 255, 191, 50);
				skillicon->Bind();

				gEngfuncs.pTriAPI->Begin(TRI_QUADS);
				// �I (x1, y1)
				gEngfuncs.pTriAPI->TexCoord2f(0, 0);
				gEngfuncs.pTriAPI->Vertex3f(x * gHUD.m_flScale, y * gHUD.m_flScale, 0);
				// �L (x1, y2)
				gEngfuncs.pTriAPI->TexCoord2f(0, (1.0f - flPercent));
				gEngfuncs.pTriAPI->Vertex3f(x * gHUD.m_flScale, center_y * gHUD.m_flScale, 0);
				// �K (x2, y2)
				gEngfuncs.pTriAPI->TexCoord2f(1, (1.0f - flPercent));
				gEngfuncs.pTriAPI->Vertex3f((x + skillicon->w()) * gHUD.m_flScale, center_y * gHUD.m_flScale, 0);
				// �J (x2, y1)
				gEngfuncs.pTriAPI->TexCoord2f(1, 0);
				gEngfuncs.pTriAPI->Vertex3f((x + skillicon->w()) * gHUD.m_flScale, y * gHUD.m_flScale, 0);
				gEngfuncs.pTriAPI->End();

				// bottom half
				gEngfuncs.pTriAPI->Color4ub(230, 150, 150, 255);
				skillicon->Bind();

				gEngfuncs.pTriAPI->Begin(TRI_QUADS);
				// �I (x1, y1)
				gEngfuncs.pTriAPI->TexCoord2f(0, (1.0f - flPercent));
				gEngfuncs.pTriAPI->Vertex3f(x * gHUD.m_flScale, center_y * gHUD.m_flScale, 0);
				// �L (x1, y2)
				gEngfuncs.pTriAPI->TexCoord2f(0, 1);
				gEngfuncs.pTriAPI->Vertex3f(x * gHUD.m_flScale, (y + skillicon->h()) * gHUD.m_flScale, 0);
				// �K (x2, y2)
				gEngfuncs.pTriAPI->TexCoord2f(1, 1);
				gEngfuncs.pTriAPI->Vertex3f((x + skillicon->w()) * gHUD.m_flScale, (y + skillicon->h()) * gHUD.m_flScale, 0);
				// �J (x2, y1)
				gEngfuncs.pTriAPI->TexCoord2f(1, (1.0f - flPercent));
				gEngfuncs.pTriAPI->Vertex3f((x + skillicon->w()) * gHUD.m_flScale, center_y * gHUD.m_flScale, 0);
				gEngfuncs.pTriAPI->End();
			}
			else if (icon.m_iCurrentSkillStatus == SKILL_STATUS_USED)
			{
				// used
				gEngfuncs.pTriAPI->Color4ub(255, 255, 191, 50);
				skillicon->Bind();
				DrawUtils::Draw2DQuadScaled(x, y, x + skillicon->w(), y + skillicon->h());
			}
			else if (time < icon.m_flTimeSkillBlink)
			{
				// blinking
				float timeDelta = icon.m_flTimeSkillBlink - time;
				float modDelta = timeDelta - static_cast<float>(static_cast<int>(timeDelta));
				float a = modDelta < 0.5f ? modDelta * 2.0f : 2.0f - modDelta * 2.0f;
				a = 1.0 - a; // reverse ?

				gEngfuncs.pTriAPI->Color4ub(255, 255, 191, 255 * a);
				skillicon->Bind();
				DrawUtils::Draw2DQuadScaled(x, y, x + skillicon->w(), y + skillicon->h());
			}
			else
			{
				// normal
				gEngfuncs.pTriAPI->Color4ub(255, 255, 191, 255);
				skillicon->Bind();
				DrawUtils::Draw2DQuadScaled(x, y, x + skillicon->w(), y + skillicon->h());
			}
		}
		
		gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
		gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
		m_pTexture_skillslotkeybg->Bind();
		DrawUtils::Draw2DQuadScaled(x - 3, y - 3, x + m_pTexture_skillslotkeybg->w(), y + m_pTexture_skillslotkeybg->h());

		DrawUtils::TextMessageDrawChar(x + 7, y, SkillKey, 100, 100, 100);

		SkillKey = SkillKey == 'G' ? '5' : SkillKey + 1;
		x += 58;
	}
}

void CHudPR_Skill::OnSkillInit(ZombieClassType zclass, ZombieSkillType skill1, ZombieSkillType skill2, ZombieSkillType skill3, ZombieSkillType skill4)
{
	assert(zclass >= 0 && zclass < MAX_ZOMBIE_CLASS);
	float flSkillBlinkTime = gHUD.m_flTime + 3.0f;
	m_iCurrentClass = zclass;
	m_ZombieSkillHudIcons[0] = { skill1, SKILL_STATUS_READY, 0.0f, 0.0f, flSkillBlinkTime };
	m_ZombieSkillHudIcons[1] = { skill2, SKILL_STATUS_READY, 0.0f, 0.0f, flSkillBlinkTime };
	m_ZombieSkillHudIcons[2] = { skill3, SKILL_STATUS_READY, 0.0f, 0.0f, flSkillBlinkTime };
	m_ZombieSkillHudIcons[3] = { skill4, SKILL_STATUS_READY, 0.0f, 0.0f, flSkillBlinkTime };
}

void CHudPR_Skill::OnSkillActivate(ZombieSkillType skill, float flHoldTime, float flFreezeTime)
{
	for (auto &icon : m_ZombieSkillHudIcons)
	{
		if (icon.m_iCurrentSkill == skill)
		{
			icon = { skill, flFreezeTime > 0.0f ? SKILL_STATUS_FREEZING : SKILL_STATUS_USED, gHUD.m_flTime, gHUD.m_flTime + flFreezeTime, 0.0f };
		}
	}
}