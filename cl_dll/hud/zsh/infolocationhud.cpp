/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "hud.h"
#include "infolocationhud.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"
#include "calcscreen.h"
#include <tuple>
#include "gamemode/mods_const.h"

int CHudInfoShelterIcon::Init(void)
{
	gHUD.AddHudElem(this);
	m_iFlags = 0;
	return 1;
}

int CHudInfoShelterIcon::VidInit(void)
{

	//zsht_buyzone
	//ingame_iron
	//zombie_s
	//ingame_wood
	if (!m_pTexture_woods)
		m_pTexture_woods = R_LoadTextureUnique("resource/shelterteam/ingame_wood");

	m_iFlags |= HUD_DRAW;
	return 1;
}

void CHudInfoShelterIcon::Shutdown(void)
{
	m_pTexture_woods = nullptr;
}

void CHudInfoShelterIcon::Reset(void) {}

int CHudInfoShelterIcon::Draw(float time)
{
	if (gHUD.m_iModRunning == MOD_ZSH)
	{
		if (g_PlayerExtraInfo[gHUD.m_Scoreboard.m_iPlayerNum].teamnumber == TEAM_CT)
		{
			for (int i = 0; i < MAX_HOSTAGES; i++)
			{
				if (g_ShelterInfo[i].dead)
					continue;

				float xyScreen[2];
				
				if (CalcScreen(g_ShelterInfo[i].origin, xyScreen))
				{
					gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
					gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
					m_pTexture_woods->Bind();
					DrawUtils::Draw2DQuadScaled(xyScreen[0] - 18, xyScreen[1] - 18, xyScreen[0] + 19, xyScreen[1] + 19);

					char szBuffer[16];
					sprintf(szBuffer, "[%im]", static_cast<int>((g_ShelterInfo[i].origin - gHUD.m_vecOrigin).Length() / 42.0f));
					
					if (((g_ShelterInfo[i].origin - gHUD.m_vecOrigin).Length() / 42.0f) < 30)
					{
						getcommand = false;
					}
					else
					{
						getcommand = true;
					}

					if (gHUD.m_flTime - tWorldTimeMen < 1.0f)
					{
						tDeltaMen += gHUD.m_flTime - tWorldTimeMen;
					}
					if (tNextAttackMen > 1.0f || (gHUD.m_flTime - tWorldTimeMen > 1.0f) || tDeltaMen > 1.0f)
					{
						tNextAttackMen = 0.0f;
						tDeltaMen = 0.0f;

						if (getcommand == true)
						{
							ClientCmd("zsh_mentality1");
						}
						else
						{
							ClientCmd("zsh_mentality0");
						}
					}
					tWorldTimeMen = gHUD.m_flTime;

					int textlen = DrawUtils::HudStringLen(szBuffer);
					int r, g, b;
					DrawUtils::UnpackRGB(r, g, b, RGB_YELLOWISH);
					DrawUtils::DrawHudString(xyScreen[0] - textlen * 0.5f, xyScreen[1] + 25, gHUD.m_scrinfo.iWidth, szBuffer, r, g, b);
				}
			}
		}
	}
	return 1;
}

int CHudInfoWoodIcon::Init(void)
{
	gHUD.AddHudElem(this);
	m_iFlags = 0;
	return 1;
}

int CHudInfoWoodIcon::VidInit(void)
{

	//zsht_buyzone
	//ingame_iron
	//zombie_s
	//ingame_wood
	if (!m_pTexture_woods)
		m_pTexture_woods = R_LoadTextureUnique("resource/shelterteam/ingame_wood");

	m_iFlags |= HUD_DRAW;
	return 1;
}

void CHudInfoWoodIcon::Shutdown(void)
{
	m_pTexture_woods = nullptr;
}

void CHudInfoWoodIcon::Reset(void) {}

bool CHudInfoWoodIcon::CheckForPlayer(cl_entity_s* pEnt)
{
	if (pEnt && pEnt->model)
		return true;

	return false;
}

int CHudInfoWoodIcon::Draw(float time)
{
	if (gHUD.m_iModRunning == MOD_ZSH)
	{
		/*if (g_PlayerExtraInfo[gHUD.m_Scoreboard.m_iPlayerNum].teamnumber == TEAM_CT)
		{
			for (int i = 0; i < MAX_HOSTAGES; i++)
			{
				if (g_WoodInfo[i].dead)
					continue;

				float xyScreen[2];
				if (CalcScreen(g_WoodInfo[i].origin, xyScreen))
				{
					//if (((g_ShelterInfo[i].origin - gHUD.m_vecOrigin).Length() / 42.0f) < 16)
					//{
					gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
					gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
					m_pTexture_woods->Bind();
					DrawUtils::Draw2DQuadScaled(xyScreen[0] - 18, xyScreen[1] - 18, xyScreen[0] + 19, xyScreen[1] + 19);

					char szBuffer[16];
					sprintf(szBuffer, "[%im]", static_cast<int>((g_WoodInfo[i].origin - gHUD.m_vecOrigin).Length() / 42.0f));


					int textlen = DrawUtils::HudStringLen(szBuffer);
					int r, g, b;
					DrawUtils::UnpackRGB(r, g, b, RGB_YELLOWISH);
					DrawUtils::DrawHudString(xyScreen[0] - textlen * 0.5f, xyScreen[1] + 25, gHUD.m_scrinfo.iWidth, szBuffer, r, g, b);

					//}
				}
			}
		}*/
		if ((gHUD.m_iHideHUDDisplay & HIDEHUD_ALL) || g_iUser1)
			return 1;

		for (int i = 1; i < 33; i++)
		{
			if (g_WoodInfo[i].dead)
				continue;

			if (i != gHUD.m_Scoreboard.m_iZombieNum)
			{
				cl_entity_t* ent = gEngfuncs.GetEntityByIndex(i);


				if (!CheckForPlayer(ent))
					continue;

				vec3_t origin = ent->origin;
				origin.z += 33.0;

				float xyScreen[2];
				if (CalcScreen(origin, xyScreen))
				{
					gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
					gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
					m_pTexture_woods->Bind();
					DrawUtils::Draw2DQuadScaled(xyScreen[0] - 13, xyScreen[1] - 8, xyScreen[0] + 13, xyScreen[1] + 8);
				}
			}
		}
	}
	return 1;
}

int CHudInfoMetalIcon::Init(void)
{
	gHUD.AddHudElem(this);
	m_iFlags = 0;
	return 1;
}

int CHudInfoMetalIcon::VidInit(void)
{

	//zsht_buyzone
	//ingame_iron
	//zombie_s
	//ingame_wood
	if (!m_pTexture_woods)
		m_pTexture_woods = R_LoadTextureUnique("resource/shelterteam/ingame_wood");

	m_iFlags |= HUD_DRAW;
	return 1;
}

void CHudInfoMetalIcon::Shutdown(void)
{
	m_pTexture_woods = nullptr;
}

void CHudInfoMetalIcon::Reset(void) {}

int CHudInfoMetalIcon::Draw(float time)
{
	if (gHUD.m_iModRunning == MOD_ZSH)
	{
		if (g_PlayerExtraInfo[gHUD.m_Scoreboard.m_iPlayerNum].teamnumber == TEAM_CT)
		{
			for (int i = 0; i < MAX_HOSTAGES; i++)
			{
				if (g_MetalInfo[i].dead)
					continue;

				float xyScreen[2];
				if (CalcScreen(g_MetalInfo[i].origin, xyScreen))
				{
					//if (((g_ShelterInfo[i].origin - gHUD.m_vecOrigin).Length() / 42.0f) < 16)
					//{
					gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
					gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
					m_pTexture_woods->Bind();
					DrawUtils::Draw2DQuadScaled(xyScreen[0] - 18, xyScreen[1] - 18, xyScreen[0] + 19, xyScreen[1] + 19);

					char szBuffer[16];
					sprintf(szBuffer, "[%im]", static_cast<int>((g_MetalInfo[i].origin - gHUD.m_vecOrigin).Length() / 42.0f));

					int textlen = DrawUtils::HudStringLen(szBuffer);
					int r, g, b;
					DrawUtils::UnpackRGB(r, g, b, RGB_YELLOWISH);
					DrawUtils::DrawHudString(xyScreen[0] - textlen * 0.5f, xyScreen[1] + 25, gHUD.m_scrinfo.iWidth, szBuffer, r, g, b);

					//}
				}
			}
		}
	}
	return 1;
}

int CHudInfoZombieIcon::Init(void)
{
	gHUD.AddHudElem(this);
	m_iFlags = 0;
	return 1;
}

int CHudInfoZombieIcon::VidInit(void)
{

	//zsht_buyzone
	//ingame_iron
	//zombie_s
	//ingame_wood
	if (!m_pTexture_woods)
		m_pTexture_woods = R_LoadTextureUnique("resource/shelterteam/ingame_wood");

	m_iFlags |= HUD_DRAW;
	return 1;
}

void CHudInfoZombieIcon::Shutdown(void)
{
	m_pTexture_woods = nullptr;
}

void CHudInfoZombieIcon::Reset(void) {}

int CHudInfoZombieIcon::Draw(float time)
{
	if (gHUD.m_iModRunning == MOD_ZSH)
	{
		if (g_PlayerExtraInfo[gHUD.m_Scoreboard.m_iPlayerNum].teamnumber == TEAM_CT)
		{
			for (int i = 0; i < MAX_HOSTAGES; i++)
			{
				if (g_ZombieInfo[i].dead)
					continue;

				float xyScreen[2];
				if (CalcScreen(g_ZombieInfo[i].origin, xyScreen))
				{
					//if (((g_ShelterInfo[i].origin - gHUD.m_vecOrigin).Length() / 42.0f) < 16)
					//{
					gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
					gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
					m_pTexture_woods->Bind();
					DrawUtils::Draw2DQuadScaled(xyScreen[0] - 18, xyScreen[1] - 18, xyScreen[0] + 19, xyScreen[1] + 19);

					char szBuffer[16];
					sprintf(szBuffer, "[%im]", static_cast<int>((g_ZombieInfo[i].origin - gHUD.m_vecOrigin).Length() / 42.0f));

					int textlen = DrawUtils::HudStringLen(szBuffer);
					int r, g, b;
					DrawUtils::UnpackRGB(r, g, b, RGB_YELLOWISH);
					DrawUtils::DrawHudString(xyScreen[0] - textlen * 0.5f, xyScreen[1] + 25, gHUD.m_scrinfo.iWidth, szBuffer, r, g, b);

					//}
				}
			}
		}
	}
	return 1;
}