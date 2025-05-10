
#pragma once

#include <vector>

#include "r_texture.h"
#include "hud_sub.h"

class CHudZBSKill : public IBaseHudSub
{
public:
	int VidInit(void) override;
	void Reset(void) override;
	int Draw(float time) override;
	void Think(void) override;

public:
	void OnKillMessage();
	void OnKillType(int buf)
	{
		szbufer = buf;
	}

protected:
	int szbufer;
	UniqueTexture m_iKillTexture;
	UniqueTexture m_iBossKillTexture;
	std::vector<float> m_killTimes;
};