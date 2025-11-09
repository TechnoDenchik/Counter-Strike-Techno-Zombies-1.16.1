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

constexpr int MAX_LEVEL = 28;
constexpr int KILL_NUMS[MAX_LEVEL] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };

class PlayerExtraHumanLevel_GD : public BasePlayerExtra
{
public:
	explicit PlayerExtraHumanLevel_GD(CBasePlayer *player) : BasePlayerExtra(player), m_iLevel(1), m_iKillsRemaining(0)
	{
		m_iKillsRemaining = KillRemainingForCurrentLevel();
	}

	void OnKillEnemy()
	{
		--m_iKillsRemaining;

		if (m_iKillsRemaining == 1)
		{
			CLIENT_COMMAND(m_pPlayer->edict(), "spk gdm/finalattack.wav\n");
			MESSAGE_BEGIN(MSG_ONE, gmsgGDMsg, nullptr, m_pPlayer->pev);
			WRITE_BYTE(GD_FINAL_ATTACK);
			MESSAGE_END();
			return;
		}

		if (!m_iKillsRemaining)
		{
			if(m_iLevel < 28) 
			{ 
				CLIENT_COMMAND(m_pPlayer->edict(), "spk gdm/weaponchange.wav\n");

				MESSAGE_BEGIN(MSG_ONE, gmsgGDMsg, nullptr, m_pPlayer->pev);
				WRITE_BYTE(GD_WEAPON_CHANGE);
				MESSAGE_END();

				++m_iLevel;
				m_iKillsRemaining = KillRemainingForCurrentLevel();
				GiveWeaponsToPlayer(m_pPlayer);
			}
		}
		UpdateHUD();
	}

	void GiveWeaponsToPlayer(CBasePlayer* m_pPlayer) const
	{
		UpdateHUD();

		m_pPlayer->RemoveAllItems(false);
		m_pPlayer->m_bHasPrimary = true;

		if (m_pPlayer->m_rgpPlayerItems[PRIMARY_WEAPON_SLOT])
		{
			m_pPlayer->RemovePlayerItem(m_pPlayer->m_rgpPlayerItems[PRIMARY_WEAPON_SLOT]);
		}

		if (m_pPlayer->m_rgpPlayerItems[PISTOL_SLOT])
		{
			m_pPlayer->RemovePlayerItem(m_pPlayer->m_rgpPlayerItems[PISTOL_SLOT]);
		}

		if (m_pPlayer->m_rgpPlayerItems[KNIFE_SLOT])
		{
			m_pPlayer->RemovePlayerItem(m_pPlayer->m_rgpPlayerItems[KNIFE_SLOT]);
		}

		int iAmount2 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("762Nato", MAX_AMMO_762NATO);
		int iAmount3 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("50BMG", MAX_AMMO_50BMG);
		int iAmount4 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("50AE", MAX_AMMO_50AE);
		int iAmount5 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("556Nato", MAX_AMMO_556NATO);
		int iAmount6 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("46MM", MAX_AMMO_46MM);
		int iAmount7 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("556Nato", MAX_AMMO_556NATO);
		int iAmount8 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("45acp", MAX_AMMO_45ACP);
		int iAmount9 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("556Nato", MAX_AMMO_556NATO);
		int iAmount10 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("buckshot", MAX_AMMO_BUCKSHOT);
		int iAmount11 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("46MM", MAX_AMMO_46MM);
		int iAmount12 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("762Nato", MAX_AMMO_762NATO);
		int iAmount13 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("556Nato", MAX_AMMO_556NATO);
		int iAmount14 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("45acp", MAX_AMMO_45ACP);
		int iAmount15 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("45acp", MAX_AMMO_45ACP);
		int iAmount16 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("45acp", MAX_AMMO_45ACP);
		int iAmount17 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("45acp", MAX_AMMO_45ACP);
		int iAmount18 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("45acp", MAX_AMMO_45ACP);
		int iAmount19 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("556Nato", MAX_AMMO_556NATO);
		int iAmount20 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("57mm", MAX_AMMO_57MM);

		int iAmount21 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("9mm", MAX_AMMO_9MM);
		int iAmount22 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("556NatoBox", MAX_AMMO_556NATOBOX);
		int iAmount23 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("buckshot", MAX_AMMO_BUCKSHOT);
		int iAmount24 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("556Nato", MAX_AMMO_556NATO);
		int iAmount25 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("762Nato", MAX_AMMO_762NATO);
		int iAmount26 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("556Nato", MAX_AMMO_556NATO);
		int iAmount27 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("556Nato", MAX_AMMO_556NATO);
		int iAmount28 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("556Nato", MAX_AMMO_556NATO);
		int iAmount29 = m_pPlayer->m_pModStrategy->ComputeMaxAmmo("762Nato", MAX_AMMO_762NATO);

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
			m_pPlayer->GiveAmmo(iAmount5, "556Nato", iAmount5);
			break;
		case 5:
			m_pPlayer->GiveNamedItem("knife_katana");
			m_pPlayer->GiveNamedItem("weapon_mp7a1p");
			m_pPlayer->GiveAmmo(iAmount6, "46MM", iAmount6);
			break;
		case 6:
			m_pPlayer->GiveNamedItem("knife_dragonsword");
			m_pPlayer->GiveNamedItem("weapon_sg552");
			m_pPlayer->GiveAmmo(iAmount7, "556Nato", iAmount7);
			break;
		case 7:
			m_pPlayer->GiveNamedItem("knife_katana");
			m_pPlayer->GiveNamedItem("weapon_thompson");
			m_pPlayer->GiveAmmo(iAmount8, "45acp", iAmount8);
			break;
		case 8:
			m_pPlayer->GiveNamedItem("knife_dragonsword");
			m_pPlayer->GiveNamedItem("weapon_xm8s");
			m_pPlayer->GiveAmmo(iAmount9, "556Nato", iAmount9);
			break;
		case 9:
			m_pPlayer->GiveNamedItem("knife_katana");
			m_pPlayer->GiveNamedItem("weapon_m1887");
			m_pPlayer->GiveAmmo(iAmount10, "buckshot", iAmount10);
			break;
		case 10:
			m_pPlayer->GiveNamedItem("knife_dragonsword");
			m_pPlayer->GiveNamedItem("weapon_mp7a1d");
			m_pPlayer->GiveAmmo(iAmount11, "46MM", iAmount11);
			break;
		case 11:
			m_pPlayer->GiveNamedItem("knife_katana");
			m_pPlayer->GiveNamedItem("weapon_scarh");
			m_pPlayer->GiveAmmo(iAmount12, "762Nato", iAmount12);
			break;
		case 12:
			m_pPlayer->GiveNamedItem("knife_dragonsword");
			m_pPlayer->GiveNamedItem("weapon_scarl");
			m_pPlayer->GiveAmmo(iAmount13, "556Nato", iAmount13);
			break;
		case 13:
			m_pPlayer->GiveNamedItem("knife_katana");
			m_pPlayer->GiveNamedItem("weapon_kriss");
			m_pPlayer->GiveAmmo(iAmount14, "45acp", iAmount14);
			break;
		case 14:
			m_pPlayer->GiveNamedItem("knife_dragonsword");
			m_pPlayer->GiveNamedItem("weapon_infinityex2");
			m_pPlayer->GiveAmmo(iAmount15, "45acp", iAmount15);
			break;
		case 15:
			m_pPlayer->GiveNamedItem("knife_katana");
			m_pPlayer->GiveNamedItem("weapon_infinityex1");
			m_pPlayer->GiveAmmo(iAmount16, "45acp", iAmount16);
			break;
		case 16:
			m_pPlayer->GiveNamedItem("knife_dragonsword");
			m_pPlayer->GiveNamedItem("weapon_infinity");
			m_pPlayer->GiveAmmo(iAmount17, "45acp", iAmount17);
			break;
		case 17:
			m_pPlayer->GiveNamedItem("knife_dragonsword");
			m_pPlayer->GiveNamedItem("weapon_infinityss");
			m_pPlayer->GiveAmmo(iAmount18, "45acp", iAmount18);
			break;
		case 18:
			m_pPlayer->GiveNamedItem("knife_dragonsword");
			m_pPlayer->GiveNamedItem("weapon_tar21");
			m_pPlayer->GiveAmmo(iAmount19, "556Nato", iAmount19);
			break;
		case 19:
			m_pPlayer->GiveNamedItem("knife_dragonsword");
			m_pPlayer->GiveNamedItem("weapon_balrog3");
			m_pPlayer->GiveAmmo(iAmount20, "57mm", iAmount20);
			break;


		case 20:
			m_pPlayer->GiveNamedItem("knife_katana");
			m_pPlayer->GiveNamedItem("weapon_balrog5");
			m_pPlayer->GiveAmmo(iAmount21, "9mm", iAmount21);
			break;
		case 21:
			m_pPlayer->GiveNamedItem("knife_dragonsword");
			m_pPlayer->GiveNamedItem("weapon_balrog7");
			m_pPlayer->GiveAmmo(iAmount22, "556NatoBox", iAmount22);
			break;
		case 22:
			m_pPlayer->GiveNamedItem("knife_katana");
			m_pPlayer->GiveNamedItem("weapon_balrog11");
			m_pPlayer->GiveAmmo(iAmount23, "buckshot", iAmount23);
			break;
		case 23:
			m_pPlayer->GiveNamedItem("knife_dragonsword");
			m_pPlayer->GiveNamedItem("weapon_buffaug");
			m_pPlayer->GiveAmmo(iAmount24, "556Nato", iAmount24);
			break;
		case 24:
			m_pPlayer->GiveNamedItem("knife_katana");
			m_pPlayer->GiveNamedItem("weapon_ethereal");
			m_pPlayer->GiveAmmo(iAmount25, "762Nato", iAmount25);
			break;
		case 25:
			m_pPlayer->GiveNamedItem("knife_dragonsword");
			m_pPlayer->GiveNamedItem("weapon_guitar");
			m_pPlayer->GiveAmmo(iAmount26, "556Nato", iAmount26);
			break;
		case 26:
			m_pPlayer->GiveNamedItem("knife_dragonsword");
			m_pPlayer->GiveNamedItem("weapon_plasmagun");
			m_pPlayer->GiveAmmo(iAmount27, "556Nato", iAmount27);
			break;
		case 27:
			m_pPlayer->GiveNamedItem("knife_dragonsword");
			m_pPlayer->GiveNamedItem("weapon_violingun");
			m_pPlayer->GiveAmmo(iAmount28, "556Nato", iAmount28);
			break;
		case 28:
			m_pPlayer->GiveNamedItem("knife_dragonsword");
			m_pPlayer->GiveNamedItem("weapon_paladin");
			m_pPlayer->GiveAmmo(iAmount29, "762Nato", iAmount29);
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

	void GiveDefaultItems() override
	{
		lv.GiveWeaponsToPlayer(m_pPlayer);
	}

	void Event_OnPlayerKilled(CBasePlayer *pVictim, CBasePlayer *pKiller, entvars_t *pInflictor)
	{
		if (pKiller != m_pPlayer)
			return;
		lv.OnKillEnemy();
	}

	void OnThink() override
	{
		
		if (!m_pPlayer->IsAlive())
		{
			if (gpGlobals->time > m_flDeadTime + 3.0f)
			{
				m_pPlayer->RoundRespawn();

				m_pPlayer->pev->max_health = 100;
				m_pPlayer->pev->armorvalue = 100;
			}
		}

		return CPlayerModStrategy_Default::OnThink();
	}
	
	void OnKilled(entvars_t* pKiller, entvars_t* pInflictor) override
	{
		m_flDeadTime = gpGlobals->time;

		MESSAGE_BEGIN(MSG_ONE, gmsgGDMsg, nullptr, m_pPlayer->pev);
		WRITE_BYTE(GD_RESPAWN_BAR);
		WRITE_BYTE(3);
		MESSAGE_END();

		return CPlayerModStrategy_Default::OnKilled(pKiller, pInflictor);
	}

private:
	float m_flDeadTime;
	CMod_GunDeath * const mp;
	PlayerExtraHumanLevel_GD lv;
	const EventListener m_eventPlayerKilledListener;
	const EventListener m_eventPlayerSpawnListener;
};

void CMod_GunDeath::UpdateGameMode(CBasePlayer *pPlayer)
{
	MESSAGE_BEGIN(MSG_ONE, gmsgGameMode, nullptr, pPlayer->edict());
	WRITE_BYTE(MOD_GD);
	WRITE_BYTE(0);
	WRITE_BYTE(static_cast<int>(maxkills.value));
	WRITE_BYTE(0);
	MESSAGE_END();
}

void CMod_GunDeath::RemoveGuns()
{
	CBaseEntity* toremove = NULL;

	while ((toremove = UTIL_FindEntityByClassname(toremove, "weaponbox")) != NULL)
		((CWeaponBox*)toremove)->Kill();

	toremove = NULL;

	while ((toremove = UTIL_FindEntityByClassname(toremove, "weapon_shield")) != NULL)
	{
		toremove->SetThink(&CBaseEntity::SUB_Remove);
		toremove->pev->nextthink = gpGlobals->time + 0.1;
	}
}

void CMod_GunDeath::Think(void)
{
	RemoveGuns();

	CMod_TeamDeathMatch::Think();
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
	RemoveGuns();
}