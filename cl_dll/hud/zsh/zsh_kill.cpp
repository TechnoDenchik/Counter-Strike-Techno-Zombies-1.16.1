#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"

#include "zsh.h"
#include "zsh_kill.h"

#include <algorithm>
#include <functional>

const float ZSH_KILL_DISPLAY_TIME = 1.0f;

void CHudZSHKill::OnKillMessage()
{
	m_killTimes.push_back(gHUD.m_flTime);
}

int CHudZSHKill::VidInit(void)
{
	if (!m_iKillTexture)
		m_iKillTexture = R_LoadTextureUnique("resource/hud/zbs/zbskill");

	return 1;
}

void CHudZSHKill::Reset(void)
{
	m_killTimes.clear();
}

int CHudZSHKill::Draw(float time)
{
	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);

	for (auto& flStartTime : m_killTimes)
	{
		float shownTime = time - flStartTime;
		if (shownTime > 1.0f)
			continue;

		int x = ScreenWidth / 2 - 234 / 2;
		int y = ScreenHeight / 4 - shownTime * 100.0f;

		m_iKillTexture->Bind();

		float a = (flStartTime - gHUD.m_flTime) / ZSH_KILL_DISPLAY_TIME;
		gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255 * a);

		DrawUtils::Draw2DQuadScaled(x, y, x + 234, y + 55);
	}

	return 1;
}

void CHudZSHKill::Think()
{
	// remove expired item
	m_killTimes.erase(std::remove_if(m_killTimes.begin(), m_killTimes.end(), std::bind(std::less<float>(), std::placeholders::_1, gHUD.m_flTime - ZSH_KILL_DISPLAY_TIME)), m_killTimes.end());
}