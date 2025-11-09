/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#pragma once

#include "hud_sub.h"

enum ZB5HumanMoraleType_e : byte;

class CHudZB5Morale : public IBaseHudSub
{
public:
	CHudZB5Morale();
	int VidInit(void) override;
	int Draw(float time) override;

	void UpdateLevel(ZB5HumanMoraleType_e type, int level);

protected:
	ZB5HumanMoraleType_e m_iMoraleType;
	int m_iMoraleLevel;

private:
	int m_iMoraleIconSPR;
	int m_iMoraleLevelSPR;
	int m_iMoraleEffectSPR;
};