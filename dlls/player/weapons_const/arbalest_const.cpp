/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "client.h"
#include "arbalest_const.h"
#include "gamemode/mods.h"

CArbClip::CArbClip(CBasePlayer *player) : BasePlayerExtra(player)
{
	clipin = 1;
}

void CArbClip::UpdateDay()
{
	clipin++;
	UpdateHUD();
}

void CArbClip::spawn()
{
	CLIENT_COMMAND(m_pPlayer->edict(), "spk zsh/skill_bonus.wav\n");
}

void CArbClip::Reset()
{
	clipin = 1;
}

void CArbClip::UpdateHUD() const
{
	MESSAGE_BEGIN(MSG_ONE, gmsgZSHUpdateDay, NULL, m_pPlayer->pev);
	WRITE_BYTE(0);
	WRITE_BYTE(clipin);
	MESSAGE_END();
}