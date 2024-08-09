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

float ZSHUpdateRes::DamageWood() const
{
	if (m_iAttack1 >= 40)
		return 5.0f;

	return 1.0 + (m_iAttack1 - 1) * 0.1f;
}

float ZSHUpdateRes::DamageMeat() const
{
	if (m_iAttack1 >= 40)
		return 5.0f;
	

	return 1.0 + (m_iAttack1 - 1) * 0.1f;
}

void ZSHUpdateRes::UpdateWoods()
{
	if(woods < maxwoods)
	{ 
		++woods;
		MESSAGE_BEGIN(MSG_ONE, gmsgZSHUpdateRes, NULL, m_pPlayer->pev);
		WRITE_BYTE(0);
		WRITE_BYTE(woods);
		WRITE_BYTE(metal);
		MESSAGE_END();
		CLIENT_COMMAND(m_pPlayer->edict(), "spk zsh/zsh_resouceget.wav\n");
	}
	else
	{
		CLIENT_COMMAND(m_pPlayer->edict(), "spk zsh/zsh_resoucenoget.wav\n");
	}
}

void ZSHUpdateRes::UpdateMetal()
{
	if(metal < maxmetal)
	{ 
		metal++;
		MESSAGE_BEGIN(MSG_ONE, gmsgZSHUpdateRes, NULL, m_pPlayer->pev);
		WRITE_BYTE(0);
		WRITE_BYTE(woods);
		WRITE_BYTE(metal);
		MESSAGE_END();
		CLIENT_COMMAND(m_pPlayer->edict(), "spk zsh/zsh_resouceget.wav\n");
	}
	else
	{
		CLIENT_COMMAND(m_pPlayer->edict(), "spk zsh/zsh_resoucenoget.wav\n");
	}
}

void ZSHUpdateRes::Reset()
{
	woods = 0;
	metal = 0;
}

