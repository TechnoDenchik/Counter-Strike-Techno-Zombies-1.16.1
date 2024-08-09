/*
zb2_zclass_speed.cpp - CSMoE Gameplay server : Zombie Mod 2
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
#include "gamemode/zb2/zb2_zclass.h"
#include "gamemode/zb2/zb2_skill.h"

#include "zb2_zclass_speed.h"

CZombieClass_Speed::CZombieClass_Speed(CBasePlayer *player, ZombieLevel iEvolutionLevel) : CBaseZombieClass_ZB2(player, iEvolutionLevel)
{
	m_pZombieSkill.reset(new CZombieSkill_female(m_pPlayer));

	const char *szModel = iEvolutionLevel ? "speed_zombi_origin" : "speed_zombi_host";
	SET_CLIENT_KEY_VALUE(m_pPlayer->entindex(), GET_INFO_BUFFER(m_pPlayer->edict()), "model", szModel);

	static char szModelPath[64];
	Q_snprintf(szModelPath, sizeof(szModelPath), "models/player/%s/%s.mdl", szModel, szModel);
	m_pPlayer->SetNewPlayerModel(szModelPath);

	// set default property
	m_pPlayer->pev->health = m_pPlayer->pev->max_health = 8000;
	m_pPlayer->pev->armortype = ARMOR_TYPE_HELMET;
	m_pPlayer->pev->armorvalue = 2000;
	m_pPlayer->pev->gravity = 0.84f;
	m_pPlayer->ResetMaxSpeed();
	m_pPlayer->GiveNamedItem("Knife_Zombi_female");
	m_pPlayer->m_bIsZombieFemale = true;
}

void CZombieClass_Speed::InitHUD() const
{
	MESSAGE_BEGIN(MSG_ONE, gmsgZB2Msg, nullptr, m_pPlayer->pev);
	WRITE_BYTE(ZB2_MESSAGE_SKILL_INIT);
	WRITE_BYTE(ZOMBIE_CLASS_SPEED);
	WRITE_BYTE(ZOMBIE_SKILL_HIDE);
	MESSAGE_END();
}

void CZombieClass_Speed::ResetMaxSpeed() const
{
	m_pPlayer->pev->maxspeed = 310;
}

float CZombieClass_Speed::AdjustDamageTaken(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType) const
{
	flDamage *= 0.9;
	return CBaseZombieClass_ZB2::AdjustDamageTaken(pevInflictor, pevAttacker, flDamage, bitsDamageType);
}

bool CZombieClass_Speed::ApplyKnockback(CBasePlayer *attacker, const KnockbackData &kbd)
{
	// knife knockback
	if(attacker->m_pActiveItem && attacker->m_pActiveItem->iItemSlot() == KNIFE_SLOT)
		return CBaseZombieClass_ZB2::ApplyKnockback(attacker, { 550, 750, 400, 300, 0.85f });

	if(m_pPlayer->m_iZombieLevel == ZOMBIE_LEVEL_HOST)
	{
		return CBaseZombieClass_ZB2::ApplyKnockback(attacker, {
			kbd.flOnGround * 3.f,
			kbd.flNotOnGround * 2.f,
			kbd.flFlying * 2.f,
			kbd.flDucking * 3.f,
			kbd.flVelocityModifier * 0.9f,

		});
	}

	return CBaseZombieClass_ZB2::ApplyKnockback(attacker, {
			kbd.flOnGround * 2.4f,
			kbd.flNotOnGround * 1.7f,
			kbd.flFlying * 1.7f,
			kbd.flDucking * 2.4f,
			kbd.flVelocityModifier * 0.9f,
	});
}

void CZombieClass_Speed::Pain_Zombie(int m_LastHitGroup, bool HasArmour)
{
	switch (RANDOM_LONG(0, 1))
	{
		case 0: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "zb3/zombi_hurt_female_1.wav", VOL_NORM, ATTN_NORM); break;
		case 1: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "zb3/zombi_hurt_female_2.wav", VOL_NORM, ATTN_NORM); break;
		default:break;
	}
}

void CZombieClass_Speed::DeathSound_Zombie()
{
	switch (RANDOM_LONG(1, 2))
	{
		case 1: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "zb3/zombi_death_female_1.wav", VOL_NORM, ATTN_NORM); break;
		case 2: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "zb3/zombi_death_female_2.wav", VOL_NORM, ATTN_NORM); break;
		default:break;
	}
}

void CZombieClass_Speed::OnThink()
{
	Zombie_HealthRecoveryThink();
	return CZombieClass_Speed::OnThink();
}


void CZombieClass_Speed::Zombie_HealthRecoveryThink()
{
	if (!m_pPlayer->IsAlive() || !m_pPlayer->m_bIsZombie)
		return;

	if (m_pPlayer->pev->button & (IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT))
	{
		m_flTimeNextZombieHealthRecovery = gpGlobals->time + 3.0f;
		return;
	}

	// cannot recover during using zombie skill.
	const float flRecoverValue = m_pCharacter_ZB2->HealthRecoveryAmount();
	if (flRecoverValue <= 0.0f)
		return;

	if (gpGlobals->time > m_flTimeNextZombieHealthRecovery)
	{
		if (m_pPlayer->pev->max_health != m_pPlayer->pev->health)
		{

			m_flTimeNextZombieHealthRecovery = gpGlobals->time + 1.0f;
			m_pPlayer->pev->health = std::min(m_pPlayer->pev->max_health, m_pPlayer->pev->health + flRecoverValue);

			// effects
			CLIENT_COMMAND(m_pPlayer->edict(), "spk zb3/zombi_heal.wav\n");

			MESSAGE_BEGIN(MSG_ONE, gmsgZB2Msg, nullptr, m_pPlayer->pev);
			WRITE_BYTE(ZB2_MESSAGE_HEALTH_RECOVERY);
			MESSAGE_END();
		}
	}
}