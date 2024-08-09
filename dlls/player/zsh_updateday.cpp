/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "client.h"
#include "zsh_updateday.h"
#include "gamemode/mods.h"

ZSHUpdateDay::ZSHUpdateDay(CBasePlayer *player) : BasePlayerExtra(player)
{
	dayses = 1;
}

/* void ZSHUpdateDay::LevelUpHealth()
{

	if (m_iHealth >= 40)
		return;

	if (m_pPlayer->m_iAccount < 3000)
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "#Not_Enough_Money");
		BlinkAccount(m_pPlayer, 2);
		return;
	}

	m_pPlayer->AddAccount(-3000);
	m_iHealth++;
	CLIENT_COMMAND(m_pPlayer->edict(), "spk zombi/td_heal.wav\n");
	UpdateHUD();

	if (!m_pPlayer->IsAlive())
		return;

	m_pPlayer->pev->health += 20.0f;
}*/

void ZSHUpdateDay::UpdateDay()
{
	dayses++;
	UpdateHUD();
}

void ZSHUpdateDay::spawn()
{
	CLIENT_COMMAND(m_pPlayer->edict(), "spk zsh/skill_bonus.wav\n");
}

void ZSHUpdateDay::Reset()
{
	dayses = 1;
}

void ZSHUpdateDay::UpdateHUD() const
{
	MESSAGE_BEGIN(MSG_ONE, gmsgZSHUpdateDay, NULL, m_pPlayer->pev);
	WRITE_BYTE(0);
	WRITE_BYTE(dayses);
	MESSAGE_END();
}