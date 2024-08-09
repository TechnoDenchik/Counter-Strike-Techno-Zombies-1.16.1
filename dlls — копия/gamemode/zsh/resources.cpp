#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "client.h"
#include "resources.h"

ZSHUpdateRes::ZSHUpdateRes(CBasePlayer* player) : BasePlayerExtra(player)
{
	woods = 0;
	metal = 0;
}

void ZSHUpdateRes::UpdateWoods()
{
	if(woods < maxwoods)
	{ 
		woods++;
		UpdateWoods();
		MESSAGE_BEGIN(MSG_ONE, gmsgZSHUpdateDay, NULL, m_pPlayer->pev);
		WRITE_BYTE(0);
		WRITE_BYTE(woods);
		MESSAGE_END();
		CLIENT_COMMAND(m_pPlayer->edict(), "spk shelterteam/zsh_resouceget.wav\n");
	}
	else
	{
		CLIENT_COMMAND(m_pPlayer->edict(), "spk shelterteam/skill_bonus.wav\n");
	}
}

void ZSHUpdateRes::UpdateMetal()
{
	if(metal < maxmetal)
	{ 
		metal ++;
		UpdateWoods();
		MESSAGE_BEGIN(MSG_ONE, gmsgZSHUpdateDay, NULL, m_pPlayer->pev);
		WRITE_BYTE(0);
		WRITE_BYTE(metal);
		MESSAGE_END();
		CLIENT_COMMAND(m_pPlayer->edict(), "spk shelterteam/zsh_resouceget.wav\n");
	}
	else
	{
		CLIENT_COMMAND(m_pPlayer->edict(), "spk shelterteam/skill_bonus.wav\n");
	}
}

void ZSHUpdateRes::Reset()
{
	woods = 0;
	metal = 0;
}