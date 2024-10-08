/*
mod_zb2.cpp - CSMoE Gameplay server : Zombie Mod 2
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

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "game.h"
#include "client.h"
#include "bmodels.h"

#include "mod_zb2.h"
#include "zb2/supplybox.h"
#include "player/csdm_randomspawn.h"

#include <algorithm>
#include <vector>
#include <dlls/gamemode/zb2/zb2_zclass.h>
#include <dlls/gamemode/zb3/zb3_hero.h>
#include <dlls/gamemode/zb3/zb3_class_hero.h>
#include <dlls/util/u_range.hpp>


CMod_ZombieMod2::CMod_ZombieMod2() // precache
{
	UTIL_PrecacheOther("supplybox");

	PRECACHE_SOUND("zb3/zombi_box.wav");
	PRECACHE_SOUND("zb3/zombi_evolution.wav");
	PRECACHE_SOUND("zb3/zombi_evolution_female.wav");

	ZombieSkill_Precache();
	HumanSkill_Precache();
}

void CMod_ZombieMod2::UpdateGameMode(CBasePlayer *pPlayer)
{
	MESSAGE_BEGIN(MSG_ONE, gmsgGameMode, nullptr, pPlayer->edict());
	WRITE_BYTE(MOD_ZB2);
	WRITE_BYTE(0); // Reserved. (weapon restriction? )
	WRITE_BYTE(static_cast<int>(maxrounds.value)); // MaxRound (mp_roundlimit)
	WRITE_BYTE(0); // Reserved. (MaxTime?)

	MESSAGE_END();
}

void CMod_ZombieMod2::Think()
{
	MakeSupplyboxThink();

	return CMod_Zombi::Think();
}

void CMod_ZombieMod2::PlayerSpawn(CBasePlayer *pPlayer)
{
	CMod_Zombi::PlayerSpawn(pPlayer);
}

void CMod_ZombieMod2::PlayerThink(CBasePlayer *pPlayer)
{
	return CMod_Zombi::PlayerThink(pPlayer);
}

void CMod_ZombieMod2::RestartRound()
{
	RemoveAllSupplybox();
	m_flTimeNextMakeSupplybox = gpGlobals->time + RANDOM_FLOAT(30.0f, 60.0f);
	return CMod_Zombi::RestartRound();
}

BOOL CMod_ZombieMod2::ClientCommand(CBasePlayer *pPlayer, const char *pcmd)
{
	return CMod_Zombi::ClientCommand(pPlayer, pcmd);
}

float CMod_ZombieMod2::GetAdjustedEntityDamage(CBaseEntity *victim, entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType)
{
	flDamage = Base::GetAdjustedEntityDamage(victim, pevInflictor, pevAttacker, flDamage, bitsDamageType);

	CBasePlayer *pAttacker = dynamic_ent_cast<CBasePlayer *>(pevAttacker);
	if (pAttacker)
	{
		m_eventAdjustDamage.dispatch(pAttacker, flDamage);
	}

	return flDamage;
}

HitBoxGroup CMod_ZombieMod2::GetAdjustedTraceAttackHitgroup(CBaseEntity *victim, entvars_t * pevAttacker, float flDamage, const Vector & vecDir, TraceResult * ptr, int bitsDamageType)
{
	HitBoxGroup hbg = CMod_Zombi::GetAdjustedTraceAttackHitgroup(victim, pevAttacker, flDamage, vecDir, ptr, bitsDamageType);

	CBasePlayer *pAttacker = dynamic_ent_cast<CBasePlayer *>(pevAttacker);
	if (pAttacker)
	{
		m_eventAdjustHitgroup.dispatch(pAttacker, hbg);
	}

	return hbg;
}

void CMod_ZombieMod2::MakeSupplyboxThink()
{
	if (!FInfectionStarted())
		return;
	if (gpGlobals->time < m_flTimeNextMakeSupplybox)
		return;
	m_flTimeNextMakeSupplybox = gpGlobals->time + RANDOM_FLOAT(20.0f, 30.0f);

	RemoveAllSupplybox();

	int iSupplyboxCount = SupplyboxCount();
	for (int i = 0; i < iSupplyboxCount; ++i)
	{
		CSupplyBox *sb = CreateSupplybox();
		if (!sb) 
			continue;
		sb->m_iSupplyboxIndex = i + 1;

		for(CBasePlayer * player : moe::range::PlayersList())
		{
			if(player->m_bIsZombie)
				continue;

			MESSAGE_BEGIN(MSG_ALL, gmsgHostagePos, nullptr, player->pev);
			WRITE_BYTE(1);
			WRITE_BYTE(sb->m_iSupplyboxIndex);
			WRITE_COORD(sb->pev->origin.x);
			WRITE_COORD(sb->pev->origin.y);
			WRITE_COORD(sb->pev->origin.z);
			MESSAGE_END();
		}
	}

	//UTIL_ClientPrintAll(HUD_PRINTCENTER, "A Supply Box has arrived!"); // #CSO_SupportItemSpawned

	for (CBasePlayer* player : moe::range::PlayersList())
	{
		if (player->m_bIsZombie)
			continue;

		MESSAGE_BEGIN(MSG_ONE, gmsgSupplyText, NULL, player->pev);
		WRITE_BYTE(ZB2_SUPPLY_GET);
		MESSAGE_END();
	}

	for (int iIndex = 1; iIndex <= gpGlobals->maxClients; ++iIndex)
	{
		CBaseEntity *entity = UTIL_PlayerByIndex(iIndex);
		if (!entity)
			continue;
		CLIENT_COMMAND(entity->edict(), "spk zb3/zombi_box.wav\n");

	}

}

int CMod_ZombieMod2::SupplyboxCount()
{
	int NumDeadCT, NumDeadTerrorist, NumAliveTerrorist, NumAliveCT;
	InitializePlayerCounts(NumAliveTerrorist, NumAliveCT, NumDeadTerrorist, NumDeadCT);
	int iSupplyboxCount = (NumAliveTerrorist + NumAliveCT + NumDeadTerrorist) / 10 + 1;
	return iSupplyboxCount;
}

void CMod_ZombieMod2::RemoveAllSupplybox()
{
	CBaseEntity *ent = nullptr;
	while ((ent = UTIL_FindEntityByClassname(ent, "supplybox")) != nullptr)
	{
		CSupplyBox *sb = dynamic_ent_cast<CSupplyBox *>(ent);
		sb->pev->effects |= EF_NODRAW;
		sb->pev->flags |= FL_KILLME;
		sb->SendPositionMsg();
		sb->SetThink(&CBaseEntity::SUB_Remove);
	}
}

CSupplyBox *CMod_ZombieMod2::CreateSupplybox()
{
	auto supplybox = CreateClassPtr<CSupplyBox>();

	Vector backup_v_angle = supplybox->pev->v_angle;
	CSDM_DoRandomSpawn(supplybox);
	supplybox->pev->v_angle = backup_v_angle;

	supplybox->pev->spawnflags |= SF_NORESPAWN;

	DispatchSpawn(supplybox->edict());
	return supplybox;
}

void CMod_ZombieMod2::HumanInfectionByZombie(CBasePlayer *player, CBasePlayer *attacker)
{
	CMod_Zombi::HumanInfectionByZombie(player, attacker);
	m_eventInfection.dispatch(player, attacker);
}

void CMod_ZombieMod2::InstallPlayerModStrategy(CBasePlayer *player)
{
	player->m_pModStrategy.reset(new CPlayerModStrategy_ZB2(player, this));
}

CPlayerModStrategy_ZB2::CPlayerModStrategy_ZB2(CBasePlayer *player, CMod_ZombieMod2 *mp)
	:	CPlayerModStrategy_ZB1(player, mp),
		m_eventListeners {
			mp->m_eventInfection.subscribe(&CPlayerModStrategy_ZB2::Event_OnInfection, this),
			mp->m_eventAdjustDamage.subscribe([this](CBasePlayer *attacker, float &out){ return this->Event_AdjustHumanDamage(attacker, out); }),
			mp->m_eventAdjustHitgroup.subscribe([this](CBasePlayer *attacker, HitBoxGroup &out) { return this->Event_AdjustHumanHitgroup(attacker, out); })
		} ,
		m_pModZB2(mp)
{}

bool CPlayerModStrategy_ZB2::ClientCommand(const char *pcmd)
{
	if (!Q_stricmp(pcmd, "BTE_ZombieSkill1") && m_pPlayer->m_bIsZombie)
	{
		if(CanUseZombieSkill())
			m_pCharacter_ZB2->ActivateSkill(SKILL_SLOT_1);
		return true;
	}

	if (!m_pPlayer->m_bIsZombie)
	{
		if (!Q_stricmp(pcmd, "MoE_HumanSkill1"))
		{
			m_pCharacter_ZB2->ActivateSkill(SKILL_SLOT_1);
			return true;
		}
		else if (!Q_stricmp(pcmd, "MoE_HumanSkill2"))
		{
			m_pCharacter_ZB2->ActivateSkill(SKILL_SLOT_2);
			return true;
		}
		else if (!Q_stricmp(pcmd, "MoE_HumanSkill3"))
		{
			m_pCharacter_ZB2->ActivateSkill(SKILL_SLOT_3);
			return true;
		}
		else if (!Q_stricmp(pcmd, "MoE_HumanSkill4"))
		{
			m_pCharacter_ZB2->ActivateSkill(SKILL_SLOT_4);
			return true;
		}
	}

	return false;
}

void CPlayerModStrategy_ZB2::OnSpawn()
{
	UpdatePlayerEvolutionHUD();

	return CPlayerModStrategy_ZB1::OnSpawn();
}

void CPlayerModStrategy_ZB2::OnThink()
{
	Zombie_HealthRecoveryThink();
	return CPlayerModStrategy_ZB1::OnThink();
}

bool CPlayerModStrategy_ZB2::CanUseZombieSkill()
{
	return m_pPlayer->m_bIsZombie && m_pPlayer->m_iZombieLevel != ZOMBIE_LEVEL_HOST;
}

void CPlayerModStrategy_ZB2::Zombie_HealthRecoveryThink()
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
			if(m_pPlayer->m_bIsZombieTank == true)
			{ 
				CLIENT_COMMAND(m_pPlayer->edict(), "spk zb3/zombi_heal.wav\n");
			}
			else if (m_pPlayer->m_bIsZombieFemale == true)
			{
				CLIENT_COMMAND(m_pPlayer->edict(), "spk zb3/zombi_heal_female.wav\n");
			}
			MESSAGE_BEGIN(MSG_ONE, gmsgZB2Msg, nullptr, m_pPlayer->pev);
			WRITE_BYTE(ZB2_MESSAGE_HEALTH_RECOVERY);
			MESSAGE_END();
		}
	}
}

void CPlayerModStrategy_ZB2::BecomeZombie(ZombieLevel iEvolutionLevel)
{

	auto sp2 = HeroClassFactory(m_pPlayer, iEvolutionLevel, "random");
	auto sp = ZombieClassFactory(m_pPlayer, iEvolutionLevel, "random");
	
	m_pCharacter_ZB3 = sp2;
	

	m_pCharacter_ZB2 = sp;
	m_pCharacter = sp;

	sp->InitHUD();
	sp->ResetMaxSpeed();
	sp2->ResetMaxSpeed();

	m_iZombieInfections = 0;
	UpdatePlayerEvolutionHUD();
}

void CPlayerModStrategy_ZB2::BecomeHuman()
{
	

	auto sp = std::make_shared<CHuman_ZB2>(m_pPlayer);



	m_pCharacter_ZB2 = sp;
	m_pCharacter = sp;

	sp->InitHUD();
	sp->ResetMaxSpeed();
}

void CPlayerModStrategy_ZB2::Event_OnInfection(CBasePlayer *victim, CBasePlayer *attacker)
{
	if (m_pPlayer != attacker)
		return;

	if (victim == attacker)
		return;

	m_iZombieInfections++;

	CheckEvolution();
}

void CPlayerModStrategy_ZB2::Event_AdjustHumanDamage(CBasePlayer * attacker, float &flDamage)
{
	if (attacker != m_pPlayer)
		return;
	if (!m_pPlayer->m_bIsZombie && m_pCharacter_ZB2->GetSkillStatus(SKILL_SLOT_3) == SKILL_STATUS_USING)
	{
		// using knife 2x?
		if (m_pPlayer->m_pActiveItem->iItemSlot() == KNIFE_SLOT)
			flDamage *= 2.0f;
	}
}

void CPlayerModStrategy_ZB2::Event_AdjustHumanHitgroup(CBasePlayer * attacker, HitBoxGroup & iHitgroup)
{
	if (attacker != m_pPlayer)
		return;

	if (!m_pPlayer->m_bIsZombie && m_pCharacter_ZB2->GetSkillStatus(SKILL_SLOT_2) == SKILL_STATUS_USING)
	{
		if (m_pPlayer->m_pActiveItem->iItemSlot() != KNIFE_SLOT)
			iHitgroup = HITGROUP_HEAD;
	}
}

void CPlayerModStrategy_ZB2::Pain(int m_LastHitGroup, bool HasArmour)
{
	m_flTimeNextZombieHealthRecovery = gpGlobals->time + 3.0f;
	return CPlayerModStrategy_ZB1::Pain(m_LastHitGroup, HasArmour);
}

void CPlayerModStrategy_ZB2::UpdatePlayerEvolutionHUD()
{
	int iCount = 0;
	if (m_pPlayer->m_bIsZombie && m_pPlayer->m_iZombieLevel != ZOMBIE_LEVEL_ORIGIN_LV2)
	{
		int iInfectionRequired = m_pPlayer->m_iZombieLevel == ZOMBIE_LEVEL_HOST ? 3 : 5;
		iCount = iInfectionRequired - m_iZombieInfections;
	}

	char buf[16];
	Q_snprintf(buf, ARRAYSIZE(buf), "hostage%d", iCount);

	if (iCount)
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgScenarioIcon, nullptr, m_pPlayer->pev);
		WRITE_BYTE(1);
		WRITE_STRING(buf);
		WRITE_BYTE(0);
		MESSAGE_END();
	}
	else
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgScenarioIcon, nullptr, m_pPlayer->pev);
		WRITE_BYTE(0);
		MESSAGE_END();
	}
}

void CPlayerModStrategy_ZB2::CheckEvolution()
{
	if (m_pPlayer->m_iZombieLevel == ZOMBIE_LEVEL_HOST && m_iZombieInfections >= 3)
	{
		BecomeZombie(ZOMBIE_LEVEL_ORIGIN);

		m_pPlayer->pev->health = m_pPlayer->pev->max_health = 10000.0f;
		m_pPlayer->pev->armorvalue = 1500.0f;

		EvolutionSound();
	}

	if (m_pPlayer->m_iZombieLevel == ZOMBIE_LEVEL_ORIGIN && m_iZombieInfections >= 5)
	{
		BecomeZombie(ZOMBIE_LEVEL_ORIGIN_LV2);

		m_pPlayer->pev->health = m_pPlayer->pev->max_health = 14000.0f;
		m_pPlayer->pev->armorvalue = 2500.0f;

		EvolutionSound();
	}

	UpdatePlayerEvolutionHUD();
}

void CPlayerModStrategy_ZB2::EvolutionSound() const
{
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_BODY, "zb3/zombi_evolution.wav", VOL_NORM, ATTN_NORM);
}
