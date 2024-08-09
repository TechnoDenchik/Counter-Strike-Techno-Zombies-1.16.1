/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"
#include "lvupdate.h"

#include "string.h"
#include "assert.h"
#include <numeric>
#include <tuple>

CHudGDLvUpdate m_hudzb1;

int CHudGDLvUpdate::VidInit(void)
{
	if (!weaponup)
		weaponup = R_LoadTextureShared("resource/hud/zb3/humanwin", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!weaponupmax)
		weaponupmax = R_LoadTextureShared("resource/hud/zb3/zombiewin", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	return 1;
}

int CHudGDLvUpdate::Draw(float time)
{
	if (!m_pCurTexture)
		return 1;

	if (time > m_flDisplayTime + 2.0f)
	{
		m_pCurTexture = nullptr;
		return 1;
	}

	int x = ScreenWidth / 2;
	int y = ScreenHeight / 4;
	const float flScale = 0.0f;
	const int r = 255, g = 255, b = 255;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255 * std::min(5.0f - (time - m_flDisplayTime), 2.0f));
	m_pCurTexture->Bind();

	DrawUtils::Draw2DQuadScaled(x - 881 / 3.0, y - 3.5, x + 881 / 3.0, y + 120);




	return 1;
}

void CHudGDLvUpdate::weaponups()
{
	m_pCurTexture = weaponup;
	m_flDisplayTime = gHUD.m_flTime;
}

void CHudGDLvUpdate::weaponupsmax()
{
	m_pCurTexture = weaponupmax;
	m_flDisplayTime = gHUD.m_flTime;
}