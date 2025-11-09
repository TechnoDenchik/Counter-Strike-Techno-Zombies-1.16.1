/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
			* =================== Mod Background UI =================== *
 * =================================================================================== */

#ifndef MOD_BACKUI_H
#define MOD_BACKUI_H
#ifdef _WIN32
#pragma once
#endif

#include "../mod_base.h"

#include <vector>
#include <utility>

class CMod_BackUI : public IBaseMod
{
public:
	CMod_BackUI();
public:
	void Think() override;
	BOOL IsTeamplay(void) override { return FALSE; }
	void CheckMapConditions() override;
	void UpdateGameMode(CBasePlayer *pPlayer) override;
	DamageTrack_e DamageTrack() override { return DT_BACK; }
};

#endif