#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "gamerules.h"
#include "client.h"

void CBasePlayer::SpawnProtection_Check()
{
	if (!m_bSpawnProtection)
		return;
	if (gpGlobals->time > m_flTimeSpawnProctionExpires)
	{
		SpawnProtection_End();
	}
}

void CBasePlayer::SetMoraleEffect(int lv)
{
	switch (lv)
	{
	case 0:
		pev->renderfx = kRenderFxNone;
		break;
	case 1:
		pev->renderfx = kRenderFxGlowShell;
		pev->rendercolor = { 0,255,10 };
		pev->renderamt = 1;
		break;
	case 2:
		pev->renderfx = kRenderFxGlowShell;
		pev->rendercolor = { 0,250,15 };
		pev->renderamt = 1;
		break;
	case 3:
		pev->renderfx = kRenderFxGlowShell;
		pev->rendercolor = { 0,245,20 };
		pev->renderamt = 1;
		break;
	case 4:
		pev->renderfx = kRenderFxGlowShell;
		pev->rendercolor = { 0,240,25 };
		pev->renderamt = 1;
		break;
	case 5:
		pev->renderfx = kRenderFxGlowShell;
		pev->rendercolor = { 0,230,30 };
		pev->renderamt = 1;
		break;
	case 6:
		pev->renderfx = kRenderFxGlowShell;
		pev->rendercolor = { 0,220,40 };
		pev->renderamt = 1;
		break;
	case 7:
		pev->renderfx = kRenderFxGlowShell;
		pev->rendercolor = { 220,40,0 };
		pev->renderamt = 1;
		break;
	case 8:
		pev->renderfx = kRenderFxGlowShell;
		pev->rendercolor = { 240,20,0 };
		pev->renderamt = 1;
		break;
	case 9:
		pev->renderfx = kRenderFxGlowShell;
		pev->rendercolor = { 245,10,0 };
		pev->renderamt = 1;
		break;
	case 10:
		pev->renderfx = kRenderFxGlowShell;
		pev->rendercolor = { 255,0,0 };
		pev->renderamt = 1;
		break;
	}
}

void CBasePlayer::SpawnProtection_Start(float flTime)
{
	m_bSpawnProtection = true;
	m_flTimeSpawnProctionExpires = gpGlobals->time + flTime;


	pev->renderfx = kRenderFxGlowShell;
	pev->rendercolor = { 255,255,255 };
	pev->renderamt = 1;
	pev->takedamage = DAMAGE_NO;
}

void CBasePlayer::SpawnProtection_End()
{
	m_bSpawnProtection = false;
	pev->renderfx = kRenderFxNone;
	pev->takedamage = DAMAGE_AIM;
}