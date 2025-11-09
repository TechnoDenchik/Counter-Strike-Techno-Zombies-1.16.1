/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "game.h"

#include "mod_zb5.h"
#include "util/u_range.hpp"
#include "gamemode/interface/interface_const.h"
#include "zb5_evobox.h"

#include <vector>
#include <algorithm>
#include <functional>
#include <random>

constexpr auto MORALE_TYPE_GLOBAL = ZB5_MORALE_DEFAULT;

CPlayerModStrategy_ZB5::CPlayerModStrategy_ZB5(CBasePlayer* player, CMod_ZombieEvolution* mp)
	: CPlayerModStrategy_ZB2R(player, static_cast<CMod_ZombieMod2R*>(mp)),
	m_pModZB5(mp),
	m_eventRoundStartListener(mp->m_eventRoundStart.subscribe(&CPlayerModStrategy_ZB5::Event_OnRoundStart, this)),
	m_flRagePercent(0.0f)
{}

void CPlayerModStrategy_ZB5::OnSpawn()
{
	MESSAGE_BEGIN(MSG_ONE, gmsgZB5InventorySet, nullptr, m_pPlayer->edict());
	WRITE_BYTE(WPN_INVENTORY);
	MESSAGE_END();

	m_pPlayer->m_bIsVIP = false;
	return CPlayerModStrategy_ZB2R::OnSpawn();
}

bool CPlayerModStrategy_ZB5::ClientCommand2(const char* pcmd)
{
	if (!Q_stricmp(pcmd, "CST_GetEvo") && m_pPlayer->m_bIsZombie)
	{
		addevofrombox(m_pPlayer);
		return true;
	}
	if (!Q_stricmp(pcmd, "CST_SetEvo"))
	{
		if (m_pPlayer->m_bIsZombie)
		{
			if (m_pPlayer->m_bEvolutionProtected == true)
			{
				m_pPlayer->evolvl = 0;

				MESSAGE_BEGIN(MSG_ALL, gmsgZB5Evolution, nullptr);
				WRITE_BYTE(ZB5_MESSAGE_EVOLUTION);
				WRITE_BYTE(m_pPlayer->evolvl);
				MESSAGE_END();

				m_pPlayer->m_bEvolutionProtected = false;
			}
		}
		else if(!m_pPlayer->m_bIsZombie)
		{
			if (m_pPlayer->m_bEvolutionProtectedH == true)
			{
				m_pPlayer->m_iHumanMoraleLevel --;
				m_pPlayer->m_iHumanMoraleLevel --;
				m_pPlayer->m_iHumanMoraleLevel --;
				m_Morale.UpdateHUD(m_pPlayer);

				CLIENT_COMMAND(m_pPlayer->edict(), "spk zb5/human_ability.wav\n");

				switch (RANDOM_LONG(1, 2))
				{
				case 1:

					MESSAGE_BEGIN(MSG_ONE, gmsgZB5ExternEvo, NULL, m_pPlayer->pev);
					WRITE_BYTE(ZB5_USEEVO2);
					MESSAGE_END();

					m_pPlayer->m_bIsSkillHeadK2x = true;

					m_pCharacter_ZB5->InitHUD();

					break;
				case 2:

					MESSAGE_BEGIN(MSG_ONE, gmsgZB5ExternEvo, NULL, m_pPlayer->pev);
					WRITE_BYTE(ZB5_USEEVO);
					WRITE_BYTE(4);
					MESSAGE_END();

					m_pPlayer->m_bIsSkillHeal = true;

					m_pCharacter_ZB5->InitHUD();
					break;
				}
			}
		}
		return true;
	}
	if (!m_pPlayer->m_bIsZombie)
	{
		if (!Q_stricmp(pcmd, "MoE_HumanSkill1"))
		{
			m_pCharacter_ZB5->ActivateSkill(SKILL_SLOT_1);
			return true;
		}
		else if (!Q_stricmp(pcmd, "MoE_HumanSkill2"))
		{
			m_pCharacter_ZB5->ActivateSkill(SKILL_SLOT_2);
			return true;
		}
		else if (!Q_stricmp(pcmd, "MoE_HumanSkill3"))
		{
			m_pCharacter_ZB5->ActivateSkill(SKILL_SLOT_3);
			return true;
		}
		else if (!Q_stricmp(pcmd, "MoE_HumanSkill4"))
		{
			m_pCharacter_ZB5->ActivateSkill(SKILL_SLOT_4);
			return true;
		}
	}
	
	return false;
}

void CPlayerModStrategy_ZB5::CheckEvolution()
{
	float flLastRagePercent = m_flRagePercent;
	auto iLastLevel = m_pPlayer->m_iZombieLevel;
	if (m_pPlayer->m_iZombieLevel == ZOMBIE_LEVEL_HOST && m_flRagePercent > 100.0f)
	{
		if (m_pPlayer->m_bIsZombieTank == true)
		{
			BecomeTank(ZOMBIE_LEVEL_ORIGIN);
			m_pPlayer->pev->health = m_pPlayer->pev->max_health = 15000.0f;
			m_pPlayer->pev->armorvalue = 4000.0f;
		}
		else if (m_pPlayer->m_bIsZombieFemale == true)
		{
			BecomeSpeed(ZOMBIE_LEVEL_ORIGIN);
			m_pPlayer->pev->health = m_pPlayer->pev->max_health = 12000.0f;
			m_pPlayer->pev->armorvalue = 3200.0f;
		}
		else if (m_pPlayer->m_bIsZombieHeavy == true)
		{
			BecomeHeavy(ZOMBIE_LEVEL_ORIGIN);
			m_pPlayer->pev->health = m_pPlayer->pev->max_health = 20000.0f;
			m_pPlayer->pev->armorvalue = 5500.0f;
		}
		else if (m_pPlayer->m_bIsZombieHeal == true)
		{
			BecomeHeal(ZOMBIE_LEVEL_ORIGIN);
			m_pPlayer->pev->health = m_pPlayer->pev->max_health = 25000.0f;
			m_pPlayer->pev->armorvalue = 2500.0f;
		}
		else if (m_pPlayer->m_bIsZombiePc == true)
		{
			BecomePsycho(ZOMBIE_LEVEL_ORIGIN);
			m_pPlayer->pev->health = m_pPlayer->pev->max_health = 22000.0f;
			m_pPlayer->pev->armorvalue = 3500.0f;
		}
		else if (m_pPlayer->m_bIsZombieDeimos == true)
		{
			BecomeDeimos(ZOMBIE_LEVEL_ORIGIN);
			m_pPlayer->pev->health = m_pPlayer->pev->max_health = 25000.0f;
			m_pPlayer->pev->armorvalue = 4000.0f;
		}
		else if (m_pPlayer->m_bIsZombieGanimed == true)
		{
			BecomeGanimed(ZOMBIE_LEVEL_ORIGIN);
			m_pPlayer->pev->health = m_pPlayer->pev->max_health = 25000.0f;
			m_pPlayer->pev->armorvalue = 4000.0f;
		}
		else if (m_pPlayer->m_bIsZombieBanchee == true)
		{
			BecomeBanchee(ZOMBIE_LEVEL_ORIGIN);
			m_pPlayer->pev->health = m_pPlayer->pev->max_health = 17500.0f;
			m_pPlayer->pev->armorvalue = 3300.0f;
		}
		else if (m_pPlayer->m_bIsZombieStamp == true)
		{
			BecomeStamper(ZOMBIE_LEVEL_ORIGIN);
			m_pPlayer->pev->health = m_pPlayer->pev->max_health = 18000.0f;
			m_pPlayer->pev->armorvalue = 4500.0f;
		}

		m_flRagePercent = (flLastRagePercent - 100.0f) * 0.5f;
	}

	if (m_pPlayer->m_iZombieLevel == ZOMBIE_LEVEL_ORIGIN && m_flRagePercent > 100.0f)
	{
		if (m_pPlayer->m_bIsZombieTank == true)
		{
			BecomeTank(ZOMBIE_LEVEL_ORIGIN_LV2);
			m_pPlayer->pev->health = m_pPlayer->pev->max_health = 25000.0f;
			m_pPlayer->pev->armorvalue = 5000.0f;
		}
		else if (m_pPlayer->m_bIsZombieFemale == true)
		{
			BecomeSpeed(ZOMBIE_LEVEL_ORIGIN_LV2);
			m_pPlayer->pev->health = m_pPlayer->pev->max_health = 20000.0f;
			m_pPlayer->pev->armorvalue = 3500.0f;
		}
		else if (m_pPlayer->m_bIsZombieHeavy == true)
		{
			BecomeHeavy(ZOMBIE_LEVEL_ORIGIN_LV2);
			m_pPlayer->pev->health = m_pPlayer->pev->max_health = 30000.0f;
			m_pPlayer->pev->armorvalue = 7500.0f;
		}
		else if (m_pPlayer->m_bIsZombieHeal == true)
		{
			BecomeHeal(ZOMBIE_LEVEL_ORIGIN_LV2);
			m_pPlayer->pev->health = m_pPlayer->pev->max_health = 27000.0f;
			m_pPlayer->pev->armorvalue = 3700.0f;
		}
		else if (m_pPlayer->m_bIsZombiePc == true)
		{
			BecomePsycho(ZOMBIE_LEVEL_ORIGIN_LV2);
			m_pPlayer->pev->health = m_pPlayer->pev->max_health = 25000.0f;
			m_pPlayer->pev->armorvalue = 4500.0f;
		}
		else if (m_pPlayer->m_bIsZombieDeimos == true)
		{
			BecomeDeimos(ZOMBIE_LEVEL_ORIGIN_LV2);
			m_pPlayer->pev->health = m_pPlayer->pev->max_health = 28000.0f;
			m_pPlayer->pev->armorvalue = 6500.0f;
		}
		else if (m_pPlayer->m_bIsZombieGanimed == true)
		{
			BecomeGanimed(ZOMBIE_LEVEL_ORIGIN_LV2);
			m_pPlayer->pev->health = m_pPlayer->pev->max_health = 28000.0f;
			m_pPlayer->pev->armorvalue = 6500.0f;
		}
		else if (m_pPlayer->m_bIsZombieBanchee == true)
		{
			BecomeBanchee(ZOMBIE_LEVEL_ORIGIN_LV2);
			m_pPlayer->pev->health = m_pPlayer->pev->max_health = 24000.0f;
			m_pPlayer->pev->armorvalue = 4000.0f;
		}
		else if (m_pPlayer->m_bIsZombieStamp == true)
		{
			BecomeStamper(ZOMBIE_LEVEL_ORIGIN_LV2);
			m_pPlayer->pev->health = m_pPlayer->pev->max_health = 22000.0f;
			m_pPlayer->pev->armorvalue = 5000.0f;
		}

		m_flRagePercent = (flLastRagePercent - 100.0f) * 0.5f;
	}

	if (m_pPlayer->m_iZombieLevel == ZOMBIE_LEVEL_ORIGIN_LV2)
	{
		m_flRagePercent = 100.0f;
	}

	UpdatePlayerEvolutionHUD();
}

void CPlayerModStrategy_ZB5::BecomeZombie(ZombieLevel iEvolutionLevel)
{
	m_pPlayer->m_bIsVIP = false;
	m_pPlayer->m_bIsHero = false;
	m_pPlayer->pev->renderfx = kRenderFxNone;
	m_pPlayer->pev->rendercolor = { 255,255,255 };
	m_pPlayer->pev->renderamt = 16;
	return CPlayerModStrategy_ZB2R::BecomeZombie(iEvolutionLevel);
}

void CPlayerModStrategy_ZB5::BecomeHuman()
{
	auto sp = std::make_shared<CHuman_ZB5>(m_pPlayer);

	m_pPlayer->m_bIsZombieTank = false;
	m_pPlayer->m_bIsZombieFemale = false;
	m_pPlayer->m_bIsZombieHeavy = false;
	m_pPlayer->m_bIsZombieHeal = false;
	m_pPlayer->m_bIsZombiePc = false;
	m_pPlayer->m_bIsZombieDeimos = false;
	m_pPlayer->m_bIsZombieGanimed = false;
	m_pPlayer->m_bIsZombieBanchee = false;
	m_pPlayer->m_bIsZombieStamp = false;

	m_pPlayer->m_bIsZombieMeatWall = false;
	m_pPlayer->m_bIsZombieDeathKnight = false;
	m_pPlayer->m_bIsZombieSpider = false;

	m_pCharacter_ZB5 = sp;
	m_pCharacter = sp;

	m_Morale.UpdateHUD(m_pPlayer);
	sp->InitHUD();
	sp->ResetMaxSpeed();
}

void CPlayerModStrategy_ZB5::Event_OnInfection(CBasePlayer * victim, CBasePlayer * attacker)
{
	if (m_pPlayer != attacker)
		return;

	if (victim->m_bIsZombie)
		m_flRagePercent += m_pPlayer->m_iZombieLevel == ZOMBIE_LEVEL_HOST ? 100 : 50;

	if (victim->m_bIsVIP && victim->m_bIsHero)
	{
		victim->m_iHumanMoraleLevel = victim->evolvl;

		if (m_pPlayer->evolvl < 10)
		{
			if (m_pPlayer->evolvprogress < 1.0f)
			{
				m_pPlayer->evolvprogress += 0.5f;

				MESSAGE_BEGIN(MSG_ONE, gmsgZB5UpdateEvoLv, nullptr, m_pPlayer->edict());
				WRITE_BYTE(ZB5_UPDATE_STATUS_BAR);
				WRITE_BYTE(0);
				WRITE_BYTE(6);
				WRITE_COORD(0.1f);
				MESSAGE_END();
			}

			if (m_pPlayer->evolvprogress == 1.0f)
			{
				if (m_pPlayer->evolvl < 3)
				{
					m_pPlayer->evolvl += 1;
				}
				else if (m_pPlayer->evolvl >= 3)
				{
					m_pPlayer->evolvl += 3;
				}

				m_pPlayer->evolvprogress = 0.0f;
			}

			MESSAGE_BEGIN(MSG_ONE, gmsgZB5Evolution, nullptr, m_pPlayer->edict());
			WRITE_BYTE(ZB5_MESSAGE_EVOLUTION);
			WRITE_BYTE(m_pPlayer->evolvl);
			MESSAGE_END();

			CLIENT_COMMAND(m_pPlayer->edict(), "spk zb5/level_up.wav\n");
		}

		m_flRagePercent += m_pPlayer->m_iZombieLevel == ZOMBIE_LEVEL_HOST ? 100 : 50;

		m_pModZB5->kills++;

		MESSAGE_BEGIN(MSG_ONE, gmsgZB5Msg, nullptr, m_pPlayer->edict());
		WRITE_BYTE(ZB5_MESSAGE_KILL);
		WRITE_BYTE(m_pModZB5->kills);
		MESSAGE_END();
	}
	else
	{
		victim->evolvl = victim->m_iHumanMoraleLevel;

		if (m_pPlayer->evolvl < 10)
		{
			if (m_pPlayer->evolvprogress < 1.0f)
			{
				m_pPlayer->evolvprogress += 0.5f;

				MESSAGE_BEGIN(MSG_ONE, gmsgZB5UpdateEvoLv, nullptr, m_pPlayer->edict());
				WRITE_BYTE(ZB5_UPDATE_STATUS_BAR);
				WRITE_BYTE(0);
				WRITE_BYTE(6);
				WRITE_COORD(0.1f);
				MESSAGE_END();
			}

			if (m_pPlayer->evolvprogress == 1.0f)
			{
				if (m_pPlayer->evolvl < 3)
				{
					m_pPlayer->evolvl += 1;
					m_pPlayer->m_bEvolutionProtected = false;
				}
				else if (m_pPlayer->evolvl > 3)
				{
					m_pPlayer->evolvl += 3;
				}

				m_pPlayer->evolvprogress = 0.0f;
			}

			if (m_pPlayer->evolvl > 6)
			{
				m_pPlayer->m_bEvolutionProtected = true;
			}

			MESSAGE_BEGIN(MSG_ONE, gmsgZB5Evolution, nullptr, m_pPlayer->edict());
			WRITE_BYTE(ZB5_MESSAGE_EVOLUTION);
			WRITE_BYTE(m_pPlayer->evolvl);
			MESSAGE_END();

			CLIENT_COMMAND(m_pPlayer->edict(), "spk zb5/level_up.wav\n");
		}
		else
		{
			m_pPlayer->m_bEvolutionProtected = true;
		}

		UpdatePlayerEvolutionHUD();

		m_flRagePercent += m_pPlayer->m_iZombieLevel == ZOMBIE_LEVEL_HOST ? 100 : 50;

		m_pModZB5->kills++;

		MESSAGE_BEGIN(MSG_ONE, gmsgZB5Msg, nullptr, m_pPlayer->edict());
		WRITE_BYTE(ZB5_MESSAGE_KILL);
		WRITE_BYTE(m_pModZB5->kills);
		MESSAGE_END();
	}
	return CPlayerModStrategy_ZB2R::Event_OnInfection(victim, attacker);
}

void CPlayerModStrategy_ZB5::UpdatePlayerEvolutionHUD()
{
	if (m_pPlayer->evolvl <= 6)
	{
		m_pPlayer->m_bEvolutionProtected = false;
	}
	else
	{
		m_pPlayer->m_bEvolutionProtected = true;
	}

	MESSAGE_BEGIN(MSG_ONE, gmsgZB5Evolution, nullptr, m_pPlayer->edict());
	WRITE_BYTE(ZB5_MESSAGE_EVOLUTION); // type, reserved.
	WRITE_BYTE(m_pPlayer->evolvl);
	MESSAGE_END();
}

float CPlayerModStrategy_ZB5::AdjustDamageTaken(entvars_t * pevInflictor, entvars_t * pevAttacker, float flDamage, int bitsDamageType)
{
	flDamage = CPlayerModStrategy_ZB2R::AdjustDamageTaken(pevInflictor, pevAttacker, flDamage, bitsDamageType);

	m_flRagePercent += flDamage * (m_pPlayer->m_iZombieLevel == ZOMBIE_LEVEL_HOST ? 0.01f : 0.005f);
	
	if (m_pPlayer->m_bIsZombie)
	{
		CheckEvolution();
	}
	else
	{
		m_Morale.UpdateHUD(m_pPlayer);
	}

	return flDamage;
}

void CPlayerModStrategy_ZB5::Event_AdjustHumanHitgroup(CBasePlayer* attacker, HitBoxGroup& iHitgroup)
{
	if (attacker != m_pPlayer)
		return;

	if (!m_pPlayer->m_bIsZombie && m_pCharacter_ZB5->GetSkillStatus(SKILL_SLOT_2) == SKILL_STATUS_USING)
	{
		if (m_pPlayer->m_pActiveItem->iItemSlot() != KNIFE_SLOT)
			iHitgroup = HITGROUP_HEAD;
	}
}

void CPlayerModStrategy_ZB5::Event_AdjustHumanDamage(CBasePlayer* attacker, float& flDamage)
{
	if (attacker != m_pPlayer)
		return;
	if (!m_pPlayer->m_bIsZombie && m_pCharacter_ZB5->GetSkillStatus(SKILL_SLOT_3) == SKILL_STATUS_USING)
	{
		// using knife 2x?
		if (m_pPlayer->m_pActiveItem->iItemSlot() == KNIFE_SLOT)
			flDamage *= 2.0f;
	}
}

void CPlayerModStrategy_ZB5::OnThink()
{
	if (!m_pPlayer->IsAlive() && m_pPlayer->m_bIsZombie)
	{
		if (!m_pPlayer->m_bHeadshotKilled && gpGlobals->time > m_flDeadTime + 5.0f)
		{
			const float health = max(m_flBackupMaxHealth / 2, 1000);
			const float armor = m_flBackupArmor;

			m_pPlayer->RoundRespawn();
			BecomeZombie(m_pPlayer->m_iZombieLevel);

			m_pPlayer->pev->max_health = m_pPlayer->pev->health = health;
			m_pPlayer->pev->armorvalue = armor;

			if (m_pPlayer->evolvl < 7)
			{
				m_pPlayer->m_bEvolutionProtected = false;

				MESSAGE_BEGIN(MSG_ONE, gmsgZB5Evolution, nullptr, m_pPlayer->edict());
				WRITE_BYTE(ZB5_MESSAGE_EVOLUTION); // type, reserved.
				WRITE_BYTE(m_pPlayer->evolvl);
				MESSAGE_END();
			}
			else if (m_pPlayer->evolvl > 8)
			{
				MESSAGE_BEGIN(MSG_ONE, gmsgZB5Evolution, nullptr, m_pPlayer->edict());
				WRITE_BYTE(ZB5_MESSAGE_EVOLUTION); // type, reserved.
				WRITE_BYTE(m_pPlayer->evolvl);
				MESSAGE_END();

				m_pPlayer->m_bEvolutionProtected = true;
			}

			for (CBasePlayer *dest : moe::range::PlayersList())
			{
				SetScoreAttrib(dest, m_pPlayer);
			}
			for (int iIndex = 1; iIndex <= gpGlobals->maxClients; ++iIndex)
			{
				CBaseEntity* entity = UTIL_PlayerByIndex(iIndex);
				if (!entity)
					continue;
				CLIENT_COMMAND(entity->edict(), "spk zb3/zombi_comeback\n");
			}
		}
	}

	return CPlayerModStrategy_ZB2R::OnThink();
}

void CPlayerModStrategy_ZB5::OnKilled(entvars_t * pKiller, entvars_t * pInflictor)
{
	m_flDeadTime = gpGlobals->time;
	m_flBackupMaxHealth = m_pPlayer->pev->max_health;
	m_flBackupArmor = m_pPlayer->pev->armorvalue;
	
	MESSAGE_BEGIN(MSG_ONE, gmsgZB5RespawnBar, NULL, m_pPlayer->edict());
	WRITE_BYTE(ZB5_RESPAWN_BAR);
	WRITE_BYTE(5);
	MESSAGE_END();

	return CPlayerModStrategy_ZB2R::OnKilled(pKiller, pInflictor);
}

void CPlayerModStrategy_ZB5::Event_OnRoundStart()
{
	m_flRagePercent = 0;
	m_Morale.UpdateHUD(m_pPlayer);
	m_pPlayer->m_bEvolutionProtected = false;
	m_pModZB5->kills = 0;

	if (m_pPlayer->m_iHumanMoraleLevel >= 3)
	{
		m_pPlayer->m_bEvolutionProtectedH = true;
	}
	else
	{
		m_pPlayer->m_bEvolutionProtectedH = false;
	}
}

void CZB3HumanMoraleR::UpdateHUD(CBasePlayer *player) const
{
	if (player->m_iHumanMoraleLevel < 3)
	{
		player->m_bEvolutionProtectedH = false;
	}
	else
	{
		player->m_bEvolutionProtectedH = true;
	}

	MESSAGE_BEGIN(MSG_ONE, gmsgZB5Evolution, nullptr, player->pev);
	WRITE_BYTE(ZB5_MESSAGE_EVOLUTION);
	WRITE_BYTE(player->m_iHumanMoraleLevel);
	MESSAGE_END();
}

CMod_ZombieEvolution::CMod_ZombieEvolution(){}

void CMod_ZombieEvolution::InstallPlayerModStrategy(CBasePlayer *player)
{
	player->m_pModStrategy.reset(new CPlayerModStrategy_ZB5(player, this));
}

void CMod_ZombieEvolution::UpdateGameMode(CBasePlayer * pPlayer)
{
	MESSAGE_BEGIN(MSG_ONE, gmsgGameMode, NULL, pPlayer->edict());
	WRITE_BYTE(MOD_ZB5);
	WRITE_BYTE(0);
	WRITE_BYTE(maxrounds.value);
	WRITE_BYTE(0);
	MESSAGE_END();

	pPlayer->m_bIsZombieMod1 = false;
	iszombiemod1 = false;
}

void CMod_ZombieEvolution::RestartRound()
{
	RemoveAllEvobox();
	m_eventRoundStart.dispatch();
	return CMod_ZombieMod2R::RestartRound();
}

void CPlayerModStrategy_ZB5::addevofrombox(CBaseEntity* pOther)
{
	CBasePlayer* m_pPlayer = static_cast<CBasePlayer*>(pOther);

	if (m_pPlayer->evolvl < 9)
	{
		m_pPlayer->evolvl++;
		m_pPlayer->evolvl++;
		m_pPlayer->evolvl++;		
	}

	UpdatePlayerEvolutionHUD();
}

void CMod_ZombieEvolution::PlayerKilled(CBasePlayer * pVictim, entvars_t * pKiller, entvars_t * pInflictor)
{
	if (pVictim->m_bIsZombie)
	{
		CBaseEntity* pKiller2 = CBaseEntity::Instance(pKiller);
		CBasePlayer* pAttacker = dynamic_cast<CBasePlayer*>(pKiller2);

		if (!pKiller2->IsAlive())
			return;

		if (!pKiller2->IsPlayer())
			return;

		if (!pAttacker->IsAlive())
			return;

		if (!pAttacker->IsPlayer())
			return;

		if (pAttacker->m_iHumanMoraleLevel < 10)
		{
			if (pAttacker->m_iHumanMoraleLevelProgress < 1.0f)
			{
				pAttacker->m_iHumanMoraleLevelProgress += 0.5f;

				MESSAGE_BEGIN(MSG_ONE, gmsgZB5UpdateEvoLv, nullptr, pAttacker->edict());
				WRITE_BYTE(ZB5_UPDATE_STATUS_BAR);
				WRITE_BYTE(0);
				WRITE_BYTE(6);
				WRITE_ANGLE(0.1f);
				MESSAGE_END();
			}

			if (pAttacker->m_iHumanMoraleLevelProgress == 1.0f)
			{
				if (pAttacker->m_iHumanMoraleLevel < 3)
				{
					pAttacker->m_iHumanMoraleLevel += 1;
				}
				else if (pAttacker->m_iHumanMoraleLevel >= 3)
				{
					pAttacker->m_iHumanMoraleLevel += 3;
				}

				pAttacker->m_iHumanMoraleLevelProgress = 0;
			}

			MESSAGE_BEGIN(MSG_ONE, gmsgZB5Evolution, nullptr, pAttacker->edict());
			WRITE_BYTE(ZB5_MESSAGE_EVOLUTION);
			WRITE_BYTE(pAttacker->m_iHumanMoraleLevel);
			MESSAGE_END();

			CLIENT_COMMAND(pAttacker->edict(), "spk zb5/level_up.wav\n");
		}

		m_Morale.UpdateHUD(pAttacker);

		MakeEvoboxThink(pVictim->pev->origin, pVictim->pev->angles);

		kills++;

		MESSAGE_BEGIN(MSG_ONE, gmsgZB5Msg, nullptr, pAttacker->edict());
		WRITE_BYTE(ZB5_MESSAGE_KILL);
		WRITE_BYTE(kills);
		MESSAGE_END();

		if (!pVictim->m_bHeadshotKilled)
		{
			MESSAGE_BEGIN(MSG_ALL, gmsgHeadIcon);
			WRITE_BYTE(19);
			WRITE_SHORT(ENTINDEX(pVictim->edict()));
			MESSAGE_END();
		}
		
		MESSAGE_BEGIN(MSG_ALL, gmsgHeadIcon);
		WRITE_BYTE(17);
		WRITE_SHORT(ENTINDEX(pVictim->edict()));
		MESSAGE_END();

		CLIENT_COMMAND(pAttacker->edict(), "spk zb5/level_up.wav\n");
	}
	return CMod_ZombieMod2R::PlayerKilled(pVictim, pKiller, pInflictor);
}

void CMod_ZombieEvolution::CheckWinConditions()
{
	if (m_bFirstConnected && m_iRoundWinStatus != WINNER_NONE)
		return;

	if (!FInfectionStarted())
		return;

	moe::range::PlayersList list;

	// Считаем живых людей (CT + не зомби + живые)
	auto iAliveHuman = std::count_if(list.begin(), list.end(),
		[](CBasePlayer* player) {
			return player &&
				player->IsAlive() &&
				player->m_iTeam == TEAM_CT &&
				!player->m_bIsZombie;
		});

	// Считаем живых зомби (T + зомби + живые)
	auto iAliveZombie = std::count_if(list.begin(), list.end(),
		[](CBasePlayer* player) {
			return player &&
				player->IsAlive() &&
				player->m_iTeam == TEAM_TERRORIST &&
				player->m_bIsZombie;
		});

	// Проверяем условия победы
	if (iAliveHuman == 0 && iAliveZombie > 0)
	{
		ZombieWin();
	}
	else if (iAliveZombie == 0 && iAliveHuman > 0)
	{
		HumanWin();
	}
}

int CMod_ZombieEvolution::EvoboxCount()
{
	int NumDeadCT, NumDeadTerrorist, NumAliveTerrorist, NumAliveCT;
	InitializePlayerCounts(NumAliveTerrorist, NumAliveCT, NumDeadTerrorist, NumDeadCT);
	int iSupplyboxCount = (NumAliveTerrorist + NumAliveCT + NumDeadTerrorist) / 10 + 1;
	return iSupplyboxCount;
}

void CMod_ZombieEvolution::MakeEvoboxThink(Vector x, Vector y)
{
	if (evobox > 30)
	{
		RemoveAllEvobox();
	}
	int iSupplyboxCount = EvoboxCount();
	for (int i = 0; i < iSupplyboxCount; ++i)
	{
		CZb5EvoBox* sb = CreateEvobox(x, y);
		if (!sb)
			continue;
		sb->m_iSupplyboxIndex = i + 1;

		for (CBasePlayer* player : moe::range::PlayersList())
		{
			if (!player->m_bIsZombie)
				continue;
			MESSAGE_BEGIN(MSG_ALL, gmsgZB5GetLocationBox, nullptr, player->pev);
			WRITE_BYTE(1);
			WRITE_BYTE(sb->m_iSupplyboxIndex);
			WRITE_COORD(sb->pev->origin.x);
			WRITE_COORD(sb->pev->origin.y);
			WRITE_COORD(sb->pev->origin.z);
			MESSAGE_END();
		}
	}
}

void CMod_ZombieEvolution::RemoveAllEvobox()
{
	CBaseEntity* ent = nullptr;
	while ((ent = UTIL_FindEntityByClassname(ent, "zb5evobox")) != nullptr)
	{
		CZb5EvoBox* sb = dynamic_ent_cast<CZb5EvoBox*>(ent);
		sb->pev->effects |= EF_NODRAW;
		sb->pev->flags |= FL_KILLME;
		sb->SetThink(&CBaseEntity::SUB_Remove);
	}
}

CZb5EvoBox* CMod_ZombieEvolution::CreateEvobox(Vector x, Vector y)
{
	auto supplybox = CreateClassPtr<CZb5EvoBox>();

	supplybox->pev->origin = x;
	supplybox->pev->angles = y;

	supplybox->pev->spawnflags |= SF_NORESPAWN;

	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
	WRITE_BYTE(TE_EXPLOSION);
	WRITE_COORD(supplybox->pev->origin.x);
	WRITE_COORD(supplybox->pev->origin.y);
	WRITE_COORD(supplybox->pev->origin.z);
	WRITE_SHORT(MODEL_INDEX("sprites/zb5_itemdrop_effect.spr"));
	WRITE_BYTE(4);
	WRITE_BYTE(30);
	WRITE_BYTE(TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOSOUND);
	MESSAGE_END();

	DispatchSpawn(supplybox->edict());
	return supplybox;
}