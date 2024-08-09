/*
zb2_zclass_tank.cpp - CSMoE Gameplay server : Zombie Mod 2
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
#include "gamemode/zb3/zb3_hero.h"
#include "gamemode/zb2/zb2_skill.h"

#include "zb3_class_hero.h"

CHeroClass_Default::CHeroClass_Default(CBasePlayer *player, ZombieLevel iEvolutionLevel) : CBaseHeroClass_ZB3(player, iEvolutionLevel)
{
	m_pZombieSkill.reset(new CHeroSkill(m_pPlayer));

	const char* szModel = iEvolutionLevel ? "hero" : "hero";
	
	SET_CLIENT_KEY_VALUE(m_pPlayer->entindex(), GET_INFO_BUFFER(m_pPlayer->edict()), "model", szModel);

	static char szModelPath[64];
	Q_snprintf(szModelPath, sizeof(szModelPath), "models/player/%s/%s.mdl", szModel, szModel);
	m_pPlayer->SetNewPlayerModel(szModelPath);

	// set default property
	m_pPlayer->pev->health = m_pPlayer->pev->max_health = 2000;
	m_pPlayer->pev->armortype = ARMOR_TYPE_HELMET;
	m_pPlayer->pev->armorvalue = 1000;
	m_pPlayer->pev->gravity = 0.83f;
	m_pPlayer->ResetMaxSpeed();
	m_pPlayer->GiveNamedItem("knife");
	
}


void CHeroClass_Default::ResetMaxSpeed() const
{
	m_pPlayer->pev->maxspeed = 290;
}

float CHeroClass_Default::AdjustDamageTaken(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType) const
{
	flDamage *= 0.9;
	return CHeroClass_Default::AdjustDamageTaken(pevInflictor, pevAttacker, flDamage, bitsDamageType);
}

bool CHeroClass_Default::ApplyKnockback(CBasePlayer *attacker, const KnockbackData &kbd)
{
	// knife knockback
	if(attacker->m_pActiveItem && attacker->m_pActiveItem->iItemSlot() == KNIFE_SLOT)
		return CBaseHeroClass_ZB3::ApplyKnockback(attacker, { 700, 1600, 1300, 400, 1.0f });

	if(m_pPlayer->m_iZombieLevel == ZOMBIE_LEVEL_HOST)
		return CBaseHeroClass_ZB3::ApplyKnockback(attacker, kbd);

	return CBaseHeroClass_ZB3::ApplyKnockback(attacker, {
			kbd.flOnGround * 0.7f,
			kbd.flNotOnGround * 0.75f,
			kbd.flFlying * 0.75f,
			kbd.flDucking * 0.7f,
			kbd.flVelocityModifier,
	});
}

void CHeroClass_Default::Pain_Hero(int m_LastHitGroup, bool HasArmour)
{
	switch (RANDOM_LONG(0, 1))
	{
		case 0: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "zombi/zombi_hurt_01.wav", VOL_NORM, ATTN_NORM); break;
		case 1: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "zombi/zombi_hurt_02.wav", VOL_NORM, ATTN_NORM); break;
		default:break;
	}
}

void CHeroClass_Default::DeathSound_Hero()
{
	switch (RANDOM_LONG(1, 2))
	{
		case 1: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "zombi/zombi_death_1.wav", VOL_NORM, ATTN_NORM); break;
		case 2: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "zombi/zombi_death_2.wav", VOL_NORM, ATTN_NORM); break;
		default:break;
	}
}