#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "client.h"
#include "../dlls/gamemode/mod_zbshelter_pve.h"

void CMod_ZombieShelter_coop::UpdateHUDBar(CBasePlayer* m_pPlayer)
{
	MESSAGE_BEGIN(MSG_ONE, gmsgZSHUpdateRes, NULL, m_pPlayer->pev);
	WRITE_BYTE(0);
	WRITE_BYTE(woods);
	WRITE_BYTE(metal);
	WRITE_BYTE(maxwoods);
	WRITE_BYTE(maxmetal);
	WRITE_BYTE(energy);
	WRITE_BYTE(maxenergy);
	MESSAGE_END();

}

void CMod_ZombieShelter_coop::UpdateHUDBarHome()
{
	MESSAGE_BEGIN(MSG_ALL, gmsgZSHUpdateResHome, NULL);
	WRITE_BYTE(0);
	WRITE_BYTE(woodshome);
	WRITE_BYTE(maxwoodshome);
	WRITE_BYTE(metalhome);
	WRITE_BYTE(maxmetalhome);
	MESSAGE_END();
}

void CMod_ZombieShelter_coop::UpdateWoods(CBasePlayer* m_pPlayer)
{
	if(woods < maxwoods)
	{ 
		++woods;
		UpdateHUDBar(m_pPlayer);
		CLIENT_COMMAND(m_pPlayer->edict(), "spk zsh/zsh_resouceget.wav\n");
	}
	else
	{
		UpdateHUDBar(m_pPlayer);
		CLIENT_COMMAND(m_pPlayer->edict(), "spk zsh/zsh_resoucenoget.wav\n");
	}
}

void CMod_ZombieShelter_coop::UpdateMetal(CBasePlayer* m_pPlayer)
{
	if(metal < maxmetal)
	{ 
		metal++;
		UpdateHUDBar(m_pPlayer);
		CLIENT_COMMAND(m_pPlayer->edict(), "spk zsh/zsh_resouceget.wav\n");
	}
	else
	{
		UpdateHUDBar(m_pPlayer);
		CLIENT_COMMAND(m_pPlayer->edict(), "spk zsh/zsh_resoucenoget.wav\n");
	}
}

void CMod_ZombieShelter_coop::UpdateWoodsHome()
{
	if (woodshome < maxwoodshome)
	{
		woodshome++;
		UpdateHUDBarHome();
	}
	else
	{
		UpdateHUDBarHome();
	}
}

void CMod_ZombieShelter_coop::UpdateMetalHome()
{
	if (metalhome < maxmetalhome)
	{
		metalhome++;
		UpdateHUDBarHome();
	}
	else
	{
		UpdateHUDBarHome();
	}
}

void CMod_ZombieShelter_coop::ResetRes()
{
	woods = 0;
	metal = 0;
	maxwoods = 15;
	maxmetal = 15;
	energy = 0;
	maxenergy = 0;
}

void CMod_ZombieShelter_coop::ResetResHome()
{
	woodshome = 0;
	metalhome = 0;
	maxwoodshome = 10;
	maxmetalhome = 10;
}
