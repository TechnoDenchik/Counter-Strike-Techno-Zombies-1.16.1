#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"

#include "zbs.h"
#include "zbs_kill.h"

#include <algorithm>
#include <functional>

const float ZBS_KILL_DISPLAY_TIME = 1.0f;

void CHudZBSKill::OnKillMessage()
{
	m_killTimes.push_back(gHUD.m_flTime);
}

int CHudZBSKill::VidInit(void)
{
	if (!m_iKillTexture)
		m_iKillTexture = R_LoadTextureUnique("resource/hud/zbs/zbskill");
	if (!m_iBossKillTexture)
		m_iBossKillTexture = R_LoadTextureUnique("resource/hud/zbs/zbsbosskill");

	return 1;
}

void CHudZBSKill::Reset(void)
{
	m_killTimes.clear();
}

int CHudZBSKill::Draw(float time)
{
	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);

	for (auto& flStartTime : m_killTimes)
	{
		float shownTime = time - flStartTime;
		if (shownTime > 1.0f)
			continue;

		int x = ScreenWidth / 2 - 234 / 2;
		int y = ScreenHeight / 4 - shownTime * 100.0f;

		float a = (flStartTime - gHUD.m_flTime) / ZBS_KILL_DISPLAY_TIME;
	
		if (szbufer == 2)
		{
			gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255 * a);
			m_iBossKillTexture->Bind();
			DrawUtils::Draw2DQuadScaled(x, y, x + 234, y + 85);
		}
		else
		{
			gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255 * a);
			m_iKillTexture->Bind();
			DrawUtils::Draw2DQuadScaled(x, y, x + 234, y + 55);			
		}	
	}

	return 1;
}

void CHudZBSKill::Think()
{
	// remove expired item
	m_killTimes.erase(std::remove_if(m_killTimes.begin(), m_killTimes.end(), std::bind(std::less<float>(), std::placeholders::_1, gHUD.m_flTime - ZBS_KILL_DISPLAY_TIME)), m_killTimes.end());
}