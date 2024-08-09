/*
zb2_zclass.cpp - CSMoE Gameplay server : Zombie Mod 2
Copyright (C) 2019 Moemod Yanase

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"

#include "gamemode/zb2/zb2_const.h"
#include "gamemode/zb3/zb3_human.h"
#include "gamemode/zb3/zb3_hero.h"
#include "zb3_hero.h"
#include "zb3_class_hero.h"
#include <random>

template<class T>
std::shared_ptr<CBaseHeroClass_ZB3> MakeHeroClass(CBasePlayer *player, ZombieLevel lv)
{
	return std::make_shared<T>(player, lv);
}

const std::pair<const char *, std::shared_ptr<CBaseHeroClass_ZB3>(*)(CBasePlayer *, ZombieLevel)> g_FindList[] = {
		{"Hero", MakeHeroClass<CHeroClass_Default> }
};

constexpr auto NUM_ZCLASSES = std::extent<decltype(g_FindList)>::value;

std::shared_ptr<CBaseHeroClass_ZB3> HeroClassFactory(CBasePlayer *player, ZombieLevel lv, const char *name)
{
	if(name == nullptr)
		return g_FindList[0].second(player, lv);

	if (!stricmp(name, "random"))
	{
		std::random_device rd;
		const std::size_t N = std::uniform_int_distribution<size_t>(0, NUM_ZCLASSES - 1)(rd);
		return g_FindList[N].second(player, lv);
	}

	return g_FindList[0].second(player, lv);
}

void CBaseHeroClass_ZB3::Think()
{
	return CBaseHeroClass_ZB3::Think();
}

void CBaseHeroClass_ZB3::ResetMaxSpeed() const
{
	CBaseHeroClass_ZB3::ResetMaxSpeed();
}

bool CBaseHeroClass_ZB3::ApplyKnockback(CBasePlayer *attacker, const KnockbackData &kbd)
{
	return CBaseHeroClass_ZB3::ApplyKnockback(attacker, kbd);
}

float CBaseHeroClass_ZB3::AdjustDamageTaken(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType) const
{
	return CBaseHeroClass_ZB3::AdjustDamageTaken(pevInflictor, pevAttacker, flDamage, bitsDamageType);
}