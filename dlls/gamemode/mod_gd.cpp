/*
mod_gd.cpp - CSMoE Gameplay server : GunDeath
Copyright (C) 2019 Moemod Hyakuya

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
#include "globals.h"
#include "gd/gd_const.h"
#include "mod_gd.h"


#include <random>
#include <string>

constexpr int MAX_LEVEL = 18;
constexpr int KILL_NUMS[MAX_LEVEL] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 2, 3, 3, 3, 3 };

constexpr const char *DEFAULT_WEAPONS[MAX_LEVEL][2] = 
{
		{"weapon_m4a1","weapon_ak47"},
		{"weapon_sg550","weapon_g3sg1"},
		{"weapon_tar21","weapon_tar21"},
		{"weapon_aug","weapon_sg552"},
		{"weapon_an94","weapon_m16a4"},
		{"weapon_awp","weapon_svd"},
		{"weapon_scar","weapon_xm8"},
		{"weapon_famas","weapon_galil"},
		{"weapon_k1a","weapon_usas"},
		{"weapon_mp5","weapon_mp7a1"},
		{"weapon_m3","weapon_xm1014"},
		{"weapon_m249","weapon_qbb95"},
		{"weapon_scout","weapon_scout"},
		{"weapon_p90","weapon_p90"},
		{"weapon_deagle","weapon_elite"},
		{"weapon_tmp","weapon_mac10"},
		{"weapon_p228","weapon_fiveseven"},
		{"weapon_glock18","weapon_glock18"}
};

class PlayerExtraHumanLevel_GD : public BasePlayerExtra
{
public:
	explicit PlayerExtraHumanLevel_GD(CBasePlayer *player) : BasePlayerExtra(player), m_iLevel(0), m_iKillsRemaining(0)
	{
		m_iKillsRemaining = KillRemainingForCurrentLevel();
	}

	void OnKillEnemy()
	{
		if (m_iLevel > 19)
		{
			CLIENT_COMMAND(m_pPlayer->edict(), "spk gdm/finalattack.wav\n");
			MESSAGE_BEGIN(MSG_ONE, gmsgGDMsg, nullptr, m_pPlayer->pev);
			WRITE_BYTE(GD_FINAL_ATTACK); // Level
			MESSAGE_END();
			return;
		}

		--m_iKillsRemaining;
		if (!m_iKillsRemaining)
		{
			if(m_iLevel < 19) 
			{ 
				CLIENT_COMMAND(m_pPlayer->edict(), "spk gdm/weaponchange.wav\n");

				MESSAGE_BEGIN(MSG_ONE, gmsgGDMsg, nullptr, m_pPlayer->pev);
				WRITE_BYTE(GD_UPDATE_LEVEL); // Level
				MESSAGE_END();

				++m_iLevel;
				m_iKillsRemaining = KillRemainingForCurrentLevel();
				GiveWeaponsToPlayer();
			}
		}

		UpdateHUD();
	}

	void GiveWeaponsToPlayer() const
	{
		m_pPlayer->RemoveAllItems(false);
		m_pPlayer->m_bHasPrimary = true;
		std::random_device rd;
		//m_pPlayer->GiveNamedItem(DEFAULT_WEAPONS[m_iLevel][rd() & 1]);
		int iAmount2 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("50ae", MAX_AMMO_50AE);
		int iAmount3 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("50ae", MAX_AMMO_45ACP);
		int iAmount4 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("50ae", MAX_AMMO_50AE);
		int iAmount5 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("50ae", MAX_AMMO_50AE);
		int iAmount6 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("50ae", MAX_AMMO_50AE);
		int iAmount7 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("50ae", MAX_AMMO_50AE);
		int iAmount8 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("50ae", MAX_AMMO_50AE);
		int iAmount9 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("50ae", MAX_AMMO_50AE);
		int iAmount10 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("50ae", MAX_AMMO_50AE);
		int iAmount11 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("50ae", MAX_AMMO_50AE);
		int iAmount12 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("50ae", MAX_AMMO_50AE);
		int iAmount13 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("50ae", MAX_AMMO_50AE);
		int iAmount14 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("50ae", MAX_AMMO_50AE);
		int iAmount15 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("50ae", MAX_AMMO_50AE);
		int iAmount16 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("50ae", MAX_AMMO_50AE);
		int iAmount17 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("50ae", MAX_AMMO_50AE);
		int iAmount18 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("50ae", MAX_AMMO_50AE);
		int iAmount19 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("50ae", MAX_AMMO_50AE);

		switch (m_iLevel)
		{
		case 1:
			
			m_pPlayer->GiveNamedItem("knife_katana");
			m_pPlayer->GiveNamedItem("weapon_ak47");
			m_pPlayer->GiveAmmo(iAmount2, "762Nato", iAmount2);
			break;
		case 2:
			m_pPlayer->GiveNamedItem("knife_dragonsword");
			m_pPlayer->GiveNamedItem("weapon_as50");			
			m_pPlayer->GiveAmmo(iAmount3, "50BMG", iAmount3);
			break;
		case 3:
			m_pPlayer->GiveNamedItem("knife_katana");
			m_pPlayer->GiveNamedItem("weapon_deagled");			
			m_pPlayer->GiveAmmo(iAmount4, "50AE", iAmount4);
			break;
		case 4:
			m_pPlayer->GiveNamedItem("knife_dragonsword");
			m_pPlayer->GiveNamedItem("weapon_k1a");			
			m_pPlayer->GiveAmmo(iAmount5, "762Nato", iAmount5);
			break;
		case 5:
			m_pPlayer->GiveNamedItem("knife_katana");
			m_pPlayer->GiveNamedItem("weapon_mp7a1p");		
			m_pPlayer->GiveAmmo(iAmount6, "762Nato", iAmount6);
			break;
		case 6:
			m_pPlayer->GiveNamedItem("knife_dragonsword");
			m_pPlayer->GiveNamedItem("weapon_sg552");			
			m_pPlayer->GiveAmmo(iAmount7, "762Nato", iAmount7);
			break;
		case 7:
			m_pPlayer->GiveNamedItem("knife_katana");
			m_pPlayer->GiveNamedItem("weapon_thompson");			
			m_pPlayer->GiveAmmo(iAmount8, "762Nato", iAmount8);
			break;
		case 8:
			m_pPlayer->GiveNamedItem("knife_dragonsword");
			m_pPlayer->GiveNamedItem("weapon_xm8s");		
			m_pPlayer->GiveAmmo(iAmount9, "762Nato", iAmount9);
			break;
		case 9:
			m_pPlayer->GiveNamedItem("knife_katana");
			m_pPlayer->GiveNamedItem("weapon_m1887");			
			m_pPlayer->GiveAmmo(iAmount10, "762Nato", iAmount10);
			break;
		case 10:
			m_pPlayer->GiveNamedItem("knife_dragonsword");
			m_pPlayer->GiveNamedItem("weapon_mp7a1d");			
			m_pPlayer->GiveAmmo(iAmount11, "762Nato", iAmount11);
			break;
		case 11:
			m_pPlayer->GiveNamedItem("knife_katana");
			m_pPlayer->GiveNamedItem("weapon_scarh");		
			m_pPlayer->GiveAmmo(iAmount12, "762Nato", iAmount12);
			break;
		case 12:
			m_pPlayer->GiveNamedItem("knife_dragonsword");
			m_pPlayer->GiveNamedItem("weapon_scarl");			
			m_pPlayer->GiveAmmo(iAmount13, "762Nato", iAmount13);
			break;
		case 13:
			m_pPlayer->GiveNamedItem("knife_katana");
			m_pPlayer->GiveNamedItem("weapon_kriss");
			m_pPlayer->GiveAmmo(iAmount14, "762Nato", iAmount14);
			break;
		case 14:
			m_pPlayer->GiveNamedItem("knife_dragonsword");
			m_pPlayer->GiveNamedItem("weapon_infinityex2");
			m_pPlayer->GiveAmmo(iAmount15, "762Nato", iAmount15);
			break;
		case 15:
			m_pPlayer->GiveNamedItem("knife_katana");
			m_pPlayer->GiveNamedItem("weapon_infinityex1");
			m_pPlayer->GiveAmmo(iAmount16, "762Nato", iAmount16);
			break;
		case 16:
			m_pPlayer->GiveNamedItem("knife_dragonsword");
			m_pPlayer->GiveNamedItem("weapon_infinity");
			m_pPlayer->GiveAmmo(iAmount17, "762Nato", iAmount17);
			break;
		case 17:
			m_pPlayer->GiveNamedItem("knife_dragonsword");
			m_pPlayer->GiveNamedItem("weapon_infinityss");
			m_pPlayer->GiveAmmo(iAmount18, "762Nato", iAmount18);
			break;
		case 18:
			m_pPlayer->GiveNamedItem("knife_dragonsword");
			m_pPlayer->GiveNamedItem("weapon_tar21");
			m_pPlayer->GiveAmmo(iAmount19, "762Nato", iAmount19);
			break;
		}
	}

	void UpdateHUD() const
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgGunDeath, nullptr, m_pPlayer->pev);
		WRITE_BYTE(1); // type, reserved.
		WRITE_BYTE(m_iLevel); // Level
		MESSAGE_END();

		MESSAGE_BEGIN(MSG_ONE, gmsgGDUpdateLV, nullptr, m_pPlayer->pev);
		WRITE_BYTE(1); // type, reserved.
		WRITE_BYTE(m_iLevel); // Level
		MESSAGE_END();

		if (m_iKillsRemaining)
		{
			char buffer[64];
			sprintf(buffer, "hostage%d", m_iKillsRemaining);
			MESSAGE_BEGIN(MSG_ONE, gmsgScenarioIcon, nullptr, m_pPlayer->pev);
			WRITE_BYTE(1);
			WRITE_STRING(buffer);
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

protected:
	int KillRemainingForCurrentLevel() const { return KILL_NUMS[m_iLevel]; }

private:
	int m_iLevel;
	int m_iKillsRemaining;
};

class CMod_GunDeath::PlayerModStrategy : public CPlayerModStrategy_Default
{
public:

	explicit PlayerModStrategy(CBasePlayer *player, CMod_GunDeath *pGD)
		: CPlayerModStrategy_Default(player),
		mp(pGD),
		lv(player),
		m_eventPlayerKilledListener(pGD->m_eventPlayerKilled.subscribe(&PlayerModStrategy::Event_OnPlayerKilled, this))
	{}

	bool CanPlayerBuy(bool display) override { return false; }

	bool CanDropWeapon(const char *pszItemName) override { return false; }

	void CheckBuyZone() override { /* */ }

	void Event_OnPlayerKilled(CBasePlayer *pVictim, CBasePlayer *pKiller, entvars_t *pInflictor)
	{
		if (pKiller != m_pPlayer)
			return;
		lv.OnKillEnemy();
	}

private:
	CMod_GunDeath * const mp;
	PlayerExtraHumanLevel_GD lv;
	const EventListener m_eventPlayerKilledListener;
};

void CMod_GunDeath::UpdateGameMode(CBasePlayer *pPlayer)
{
	MESSAGE_BEGIN(MSG_ONE, gmsgGameMode, nullptr, pPlayer->edict());
	WRITE_BYTE(MOD_GD);
	WRITE_BYTE(0); // Reserved. (weapon restriction? )
	WRITE_BYTE(static_cast<int>(maxkills.value)); // MaxRound (mp_roundlimit)
	WRITE_BYTE(0); // Reserved. (MaxTime?)
	MESSAGE_END();
}

void CMod_GunDeath::InstallPlayerModStrategy(CBasePlayer *player)
{
	std::unique_ptr<PlayerModStrategy> up(new PlayerModStrategy(player, this));
	player->m_pModStrategy = std::move(up);
}
void CMod_GunDeath::PlayerKilled(CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor)
{
	CBasePlayer *pKillerPlayer = dynamic_ent_cast<CBasePlayer *>(pKiller);
	if (pKillerPlayer)
		m_eventPlayerKilled.dispatch(pVictim, pKillerPlayer, pInflictor);
	CMod_TeamDeathMatch::PlayerKilled(pVictim, pKiller, pInflictor);
}
