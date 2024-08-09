/*
zclass.h - CSMoE Gameplay server : zombie class for zb2
Copyright (C) 2018 Moemod Hyakuya

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#ifndef PROJECT_CLASS_H
#define PROJECT_CLASS_H

#include "gamemode/zb3/zb3_human.h"
#include "gamemode/zb2/zb2_const.h"

class IHeroCharacter_ZB2_Extra
{
public:
	
};

class IZombieSkill;

class CBaseHeroClass_ZB3 : public CHero_ZB1, public IHeroCharacter_ZB2_Extra
{
public:
	explicit CBaseHeroClass_ZB3(CBasePlayer *player, ZombieLevel lv) : CHero_ZB1(player, lv) {}
	void InitHUD() const;
	void Think() override;
	void ResetMaxSpeed() const override;
	bool ApplyKnockback(CBasePlayer *attacker, const KnockbackData & kbd) override;
	float AdjustDamageTaken(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType) const override;
	
protected:

};

std::shared_ptr<CBaseHeroClass_ZB3> HeroClassFactory(CBasePlayer *player, ZombieLevel lv, const char *name = nullptr);


#endif
