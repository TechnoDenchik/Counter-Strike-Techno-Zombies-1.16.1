/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"

#include <string.h>
#include <studio.h>
#include "r_studioint.h"
#include "draw_util.h"
#include "calcscreen.h"
#include "com_model.h"
#include "events.h"
#include "r_efx.h"

#include "gamemode/mods_const.h"
#include "gamemode/interface/interface_const.h"

extern engine_studio_api_t IEngineStudio;

DECLARE_MESSAGE(m_HeadIcon, HeadIcon)
DECLARE_MESSAGE(m_HeadIcon, MPToCL)

#define MAXPLAYER 32

enum
{
	HUMAN_SKILL_KNIFE2X,
	HUMAN_SKILL_HEADSHOT,
	ZOMBIE_SKILL_HEAL,
	ZOMBIE_SKILL_HEAL_HEAD,
	CANNON_FLAME_BURN,
	HUNTBOW_DMGREITERATION,
	HUNTBOW_MARKZOMBIE,
	TELEPORT_MARKEF,
	HOLYBOMB_BURN,
	LANCE_HIT,
	ZOMBIE_SKILL_PILE,
	ZSHELTER_HOME,
	ZSHELTER_RESMEAT,
	ZSHELTER_RESWOOD,
	ZSHELTER_BUYZONE,
	ZSHELTER_ZOMBIE,
	WPN_VOID_SCANAIM,
	ZOMBIE_DEATH,
	ZOMBIE_DEBUFF,
	ZOMBIE_RESPAWN,
	ARBALEST_HIT,
	WONDERCANNON_HIT,
};

struct CHudHeadIconItem
{
	int iPlayer;
	int iType;
	int iuser1;
	int iId;
};

CHudHeadIconItem rgIconList[MAXPLAYER + 1];

int CHudHeadIcon:: Init( void )
{
	gHUD.AddHudElem( this );
	m_iFlags |= HUD_DRAW;
	HOOK_MESSAGE(HeadIcon);
	HOOK_MESSAGE(MPToCL);

	return 1;
}

void CHudHeadIcon::Reset(void)
{
	VidInit();
}

int CHudHeadIcon:: VidInit( void )
{
	char path[128]; 

	if (!m_pTexture_Zombie_s)
		R_InitTexture(m_pTexture_Zombie_s, "resource/helperhud/zombie_s");

	switch (frame)
	{
		case 1: i = 0; break;
		case 2: i = 1; break;
		case 3: i = 2; break;
		case 4: i = 3; break;
		case 5: i = 4; break;
		case 6: i = 5; break;
		case 7: i = 6; break;
		case 8: i = 7; break;
		case 9: i = 8; break;
		case 10: i = 9; break;
		case 11: i = 10; break;
		case 12: i = 11; break;
		case 13: i = 12; break;
		case 14: i = 13; break;
		case 15: i = 14; break;
		case 16: i = 15; break;
		case 17: i = 16; break;
		case 18: i = 17; break;
		case 19: i = 18; break;
		case 20: i = 19; break;
		case 21: i = 20; break;
		case 22: i = 21; break;
		case 23: i = 22; break;
		case 24: i = 23; break;
		case 25: i = 24; break;
	}
	sprintf(path, "sprites/web/wpn_voidpistol/videoframe_%d.tga", i);
		R_InitTexture(m_iTex[0], path);

	return 1;
}

void CHudHeadIcon::Shutdown(void)
{
	m_pTexture_Zombie_s = nullptr;
	std::fill(std::begin(m_iTex), std::end(m_iTex), nullptr);
}

int CHudHeadIcon:: Draw( float flTime )
{
	if ((gHUD.m_iHideHUDDisplay & HIDEHUD_ALL)  )
		return 1;

	if (gHUD.m_iModRunning == MOD_ZB2 || gHUD.m_iModRunning == MOD_ZB3 || gHUD.m_iModRunning == MOD_ZB5)
	{
		for (int i = 0; i < MAXPLAYER; i++)
		{
			if (rgIconList[i].iType == HUNTBOW_MARKZOMBIE)
			{
				if (rgIconList[i].iuser1 == 0)
				{
					if (rgIconList[i].iId == 0)
						break;
					cl_entity_t* ent = gEngfuncs.GetEntityByIndex(rgIconList[i].iPlayer);
					float xyScreen[2];
					if (CalcScreen(ent->origin, xyScreen))
					{
						m_pTexture_Zombie_s->Draw2DQuadScaled(xyScreen[0] - 18, xyScreen[1] - 18, xyScreen[0] + 19, xyScreen[1] + 19);

						char szBuffer[16];
						sprintf(szBuffer, "[%im]", static_cast<int>((ent->origin - gHUD.m_vecOrigin).Length() / 42.0f));

						int textlen = DrawUtils::HudStringLen(szBuffer);
						int r, g, b;
						DrawUtils::UnpackRGB(r, g, b, RGB_YELLOWISH);
						DrawUtils::DrawHudString(xyScreen[0] - textlen * 0.5f, xyScreen[1] + 25, gHUD.m_scrinfo.iWidth, szBuffer, r, g, b, 255);
					}
				}
				else
				{
					rgIconList[i].iId = 0;
					rgIconList[i].iPlayer = 0;
				}
			}
			else if (rgIconList[i].iType == WPN_VOID_SCANAIM)
			{
				if (rgIconList[i].iuser1 == 0)
				{
					if (rgIconList[i].iId == 0)
						break;
					cl_entity_t* ent = gEngfuncs.GetEntityByIndex(rgIconList[i].iPlayer);
					float xyScreen[2];
					if (CalcScreen(ent->origin, xyScreen))
					{
						
						if (gHUD.m_flTime - timesecond1 < 1.0f)
						{
							tDeltasecond1 += gHUD.m_flTime - timesecond1;
						}
						if (tNextsecond1 > 0.024f || (gHUD.m_flTime - timesecond1 > 0.024f) || tDeltasecond1 > 0.024f)
						{
							tNextsecond1 = 0.0f;
							tDeltasecond1 = 0.0f;

							if (frame < 25)
							{
								frame++;
							}
						}
						timesecond1 = gHUD.m_flTime;

						m_iTex[0]->Draw2DQuadScaled(xyScreen[0] - 18, xyScreen[1] - 18, xyScreen[0] + 19, xyScreen[1] + 19);

						if (frame == 25)
						{
							frame =- 0;
						}

						char szBuffer[16];
						sprintf(szBuffer, "[%im]", static_cast<int>((ent->origin - gHUD.m_vecOrigin).Length() / 42.0f));

						int textlen = DrawUtils::HudStringLen(szBuffer);
						int r, g, b;
						DrawUtils::UnpackRGB(r, g, b, RGB_YELLOWISH);
						DrawUtils::DrawHudString(xyScreen[0] - textlen * 0.5f, xyScreen[1] + 25, gHUD.m_scrinfo.iWidth, szBuffer, r, g, b, 255);
					}
				}
				else
				{
					rgIconList[i].iId = 0;
					rgIconList[i].iPlayer = 0;
				}
			}
		}
	}
	else if (gHUD.m_iModRunning == MOD_ZSH)
	{
		for (int i = 0; i < MAXPLAYER; i++)
		{
			if (rgIconList[i].iType == ZSHELTER_HOME)
			{
				if (rgIconList[i].iuser1 == 1)
				{
					if (rgIconList[i].iId == 0)
						break;
					cl_entity_t* ent = gEngfuncs.GetEntityByIndex(rgIconList[i].iPlayer);
					float xyScreen[2];
					if (CalcScreen(ent->origin, xyScreen))
					{
						m_pTexture_Zombie_s->Draw2DQuadScaled(xyScreen[0] - 18, xyScreen[1] - 18, xyScreen[0] + 19, xyScreen[1] + 19);

						char szBuffer[16];
						sprintf(szBuffer, "[%im]", static_cast<int>((ent->origin - gHUD.m_vecOrigin).Length() / 42.0f));

						int textlen = DrawUtils::HudStringLen(szBuffer);
						int r, g, b;
						DrawUtils::UnpackRGB(r, g, b, RGB_YELLOWISH);
						DrawUtils::DrawHudString(xyScreen[0] - textlen * 0.5f, xyScreen[1] + 25, gHUD.m_scrinfo.iWidth, szBuffer, r, g, b, 255);
					}
				}
				else
				{
					rgIconList[i].iId = 0;
					rgIconList[i].iPlayer = 0;
				}
			}
			else if (rgIconList[i].iType == ZSHELTER_RESMEAT)
			{
				if (rgIconList[i].iuser1 == 1)
				{
					if (rgIconList[i].iId == 0)
						break;
					cl_entity_t* ent = gEngfuncs.GetEntityByIndex(rgIconList[i].iPlayer);
					float xyScreen[2];
					if (CalcScreen(ent->origin, xyScreen))
					{
						m_pTexture_Zombie_s->Draw2DQuadScaled(xyScreen[0] - 18, xyScreen[1] - 18, xyScreen[0] + 19, xyScreen[1] + 19);

						char szBuffer[16];
						sprintf(szBuffer, "[%im]", static_cast<int>((ent->origin - gHUD.m_vecOrigin).Length() / 42.0f));

						int textlen = DrawUtils::HudStringLen(szBuffer);
						int r, g, b;
						DrawUtils::UnpackRGB(r, g, b, RGB_YELLOWISH);
						DrawUtils::DrawHudString(xyScreen[0] - textlen * 0.5f, xyScreen[1] + 25, gHUD.m_scrinfo.iWidth, szBuffer, r, g, b, 255);
					}
				}
				else
				{
					rgIconList[i].iId = 0;
					rgIconList[i].iPlayer = 0;
				}
			}
			else if (rgIconList[i].iType == ZSHELTER_RESWOOD)
			{
				if (rgIconList[i].iuser1 == 1)
				{
					if (rgIconList[i].iId == 0)
						break;
					cl_entity_t* ent = gEngfuncs.GetEntityByIndex(rgIconList[i].iPlayer);
					float xyScreen[2];
					if (CalcScreen(ent->origin, xyScreen))
					{
						m_pTexture_Zombie_s->Draw2DQuadScaled(xyScreen[0] - 18, xyScreen[1] - 18, xyScreen[0] + 19, xyScreen[1] + 19);

						char szBuffer[16];
						sprintf(szBuffer, "[%im]", static_cast<int>((ent->origin - gHUD.m_vecOrigin).Length() / 42.0f));

						int textlen = DrawUtils::HudStringLen(szBuffer);
						int r, g, b;
						DrawUtils::UnpackRGB(r, g, b, RGB_YELLOWISH);
						DrawUtils::DrawHudString(xyScreen[0] - textlen * 0.5f, xyScreen[1] + 25, gHUD.m_scrinfo.iWidth, szBuffer, r, g, b, 255);
					}
				}
				else
				{
					rgIconList[i].iId = 0;
					rgIconList[i].iPlayer = 0;
				}
			}
			else if (rgIconList[i].iType == ZSHELTER_BUYZONE)
			{

			}
			else if (rgIconList[i].iType == ZSHELTER_ZOMBIE)
			{

			}
		}
	}

	return 1;
}

// This message handler may be better off elsewhere
int CHudHeadIcon::MsgFunc_HeadIcon( const char *pszName, int iSize, void *pbuf )
{
	if ((gHUD.m_iHideHUDDisplay & HIDEHUD_ALL))
		return 1;

	m_iFlags |= HUD_DRAW;

	int i;
	for (i = 0; i < MAXPLAYER; i++)
	{
		if (rgIconList[i].iId == 0)
			break;
	}
	if (i == MAXPLAYER)
	{ // move the rest of the list forward to make room for this item
		memmove(rgIconList, rgIconList + 1, sizeof(CHudHeadIconItem) * MAXPLAYER);
		i = MAXPLAYER - 1;
	}

	BufferReader reader(pszName, pbuf, iSize);

	rgIconList[i].iType = reader.ReadByte();
	rgIconList[i].iPlayer = reader.ReadShort();
	rgIconList[i].iuser1 = reader.ReadByte();
	rgIconList[i].iId = 1;

	int iModelIndex = 0;
	int flags = FTENT_PERSIST | FTENT_FADEOUT | FTENT_SPRANIMATELOOP;
	switch (rgIconList[i].iType)
	{
	case HUMAN_SKILL_KNIFE2X:
	{
		R_AttachTentToPlayer(rgIconList[i].iPlayer, gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/zb_meleeup.spr"),
			Vector(0.0, 0.0, 30.0), 10.0, TRUE, flags, 1.0);
		break;
	}
	case HUMAN_SKILL_HEADSHOT:
	{
		R_AttachTentToPlayer(rgIconList[i].iPlayer, gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/zb_skill_headshot.spr"),
			Vector(0.0, 0.0, 30.0), rgIconList[i].iuser1 ? 5.5 : 4.5, TRUE, flags, 1.0);
		break;
	}
	case ZOMBIE_SKILL_HEAL:
	{
		R_AttachTentToPlayer(rgIconList[i].iPlayer, gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/zombihealer.spr"),
			Vector(0.0, 0.0, 25.0), 1.0, TRUE, flags, 1.0, kRenderTransAdd, 10.0);
		break;
	}
	case ZOMBIE_SKILL_HEAL_HEAD:
	{
		R_AttachTentToPlayer(rgIconList[i].iPlayer, gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/zombiheal_head.spr"),
			Vector(0.0, 0.0, 25.0), 1.0, TRUE, flags, 1.0, kRenderTransAdd, 10.0);
		break;
	}
	case ZOMBIE_DEATH:
	{
		R_AttachTentToPlayer(rgIconList[i].iPlayer, gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/zb5_death_effect.spr"),
			Vector(0.0, 0.0, 25.0), 0.1, TRUE, flags, 0.5, kRenderTransAdd, 15.0);
		break;
	}
	case CANNON_FLAME_BURN:
	{
		R_AttachTentToPlayer(rgIconList[i].iPlayer, gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/flame_burn01.spr"),
			Vector(-5.0, 5.0, 0.0), 3, TRUE, flags, 0.3, kRenderTransAdd, 10.0);
		break;
	}
	case HUNTBOW_DMGREITERATION:
	{
		switch (rgIconList[i].iuser1)
		{
		case 1: {iModelIndex = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/dmgreiteration01.spr"); break; }
		case 2: {iModelIndex = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/dmgreiteration02.spr"); break; }
		case 3: {iModelIndex = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/dmgreiteration03.spr"); break; }
		case 4: {iModelIndex = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/dmgreiteration04.spr"); break; }
		case 5: {iModelIndex = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/dmgreiteration05.spr"); break; }
		}
		R_AttachTentToPlayer(rgIconList[i].iPlayer, iModelIndex,
			Vector(0.0, 0.0, 40.0), 1.0, TRUE, flags, 1.0);
		break;
	}
	case HUNTBOW_MARKZOMBIE:
	{
		break;
	}
	case TELEPORT_MARKEF:
	{
		flags = FTENT_PERSIST;
		if (rgIconList[i].iuser1)
			R_AttachTentToEntity(rgIconList[i].iPlayer, gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/ef_teleportzombieout.spr"),
				Vector(0.0, 0.0, 0.0), 2.0, TRUE, flags, 0.8, kRenderTransAdd, 7.5);
		else
			R_AttachTentToPlayer(rgIconList[i].iPlayer, gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/ef_teleportzombie.spr"),
				Vector(0.0, 0.0, 0.0), 2.0, TRUE, flags, 0.8, kRenderTransAdd, 7.5);
		break;
	}
	case HOLYBOMB_BURN:
	{
		float fLife = rgIconList[i].iuser1;
		R_AttachTentToPlayer(rgIconList[i].iPlayer, gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/holybomb_burn.spr"),
			Vector(0.0, 0.0, 30.0), fLife, TRUE, flags, 0.9, kRenderTransAdd, 10.0);
		break;
	}
	case LANCE_HIT:
	{
		flags = FTENT_PERSIST;
		R_AttachTentToPlayer(rgIconList[i].iPlayer, gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/ef_lance_hit.spr"),
			Vector(0.0, 0.0, 0.0), 0.18, TRUE, flags, 1.0, kRenderTransAdd, 10.0);
		break;
	}
	case ZOMBIE_SKILL_PILE:
	{
		R_AttachTentToPlayer(rgIconList[i].iPlayer, gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/zbt_slow.spr"),
			Vector(0.0, 0.0, 40.0), 3.0, TRUE, flags, 1.0, kRenderTransAdd, 1.0);
		break;
	}

	case ZOMBIE_DEBUFF:
	{
		R_AttachTentToPlayer(rgIconList[i].iPlayer, gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/ef_sbmine_debuff.spr"),
			Vector(0.0, 0.0, 40.0), 0.5, TRUE, flags, 0.5, kRenderTransAdd, 2.0);
		break;
	}
	case ZOMBIE_RESPAWN:
	{
		R_AttachTentToPlayer(rgIconList[i].iPlayer, gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/deathres_zombie.spr"),
			Vector(0.0, 0.0, 0.0), 1.5, TRUE, flags, 0.7, kRenderTransAdd, 10.0);
		break;
	}
	case ARBALEST_HIT:
	{
		R_AttachTentToPlayer(rgIconList[i].iPlayer, gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/ef_halogun_shootB_hit.spr"),
			Vector(0.0, 0.0, 0.0), 0.2, TRUE, flags, 0.3, kRenderTransAdd, 25.0);
		break;
	}
	}

	return 1;
}






int CHudHeadIcon::MsgFunc_MPToCL(const char* pszName, int iSize, void* pbuf)
{
	if ((gHUD.m_iHideHUDDisplay & HIDEHUD_ALL))
		return 1;

	m_iFlags |= HUD_DRAW;

	int iType;
	BufferReader reader(pszName, pbuf, iSize);
	int arg1;
	int arg2;
	Vector pos;
	iType = reader.ReadByte();

	switch (iType)
	{
	case 0:
	{
		int iBanType = reader.ReadByte();
		int iSlot = iBanType / 10; // 1-wpn,2-knife,3-grenad
		int iDataType = iBanType % 10; // 0-all, 1-add, 2-remove
		int length = reader.ReadByte();
		if (iSlot == 1) {
			if (iDataType == 0) {
				gHUD.m_setBanWeapon.clear();
			}
			for (int i = 0; i < length; i++)
			{
				int id = reader.ReadShort();
				if (iDataType <= 1)
					gHUD.m_setBanWeapon.insert(id);
				else if (iDataType == 2)
					gHUD.m_setBanWeapon.erase(id);
			}
		}
		else if (iSlot == 2) {
			if (iDataType == 0) {
				gHUD.m_setBanKnife.clear();
			}
			for (int i = 0; i < length; i++)
			{
				int id = reader.ReadByte();
				if (iDataType <= 1)
					gHUD.m_setBanKnife.insert(id);
				else if (iDataType == 2)
					gHUD.m_setBanKnife.erase(id);
			}
		}
		else if (iSlot == 3) {
			if (iDataType == 0) {
				gHUD.m_setBanGrenade.clear();
			}
			for (int i = 0; i < length; i++)
			{
				int id = reader.ReadByte();
				if (iDataType <= 1)
					gHUD.m_setBanGrenade.insert(id);
				else if (iDataType == 2)
					gHUD.m_setBanGrenade.erase(id);
			}
		}
		break;
	}
	case 1:
	{
		//CBasePlayerWeapon* pActiveBTEWeapon = BTEClientWeapons().GetActiveWeaponEntity();
		//if (pActiveBTEWeapon)
		//{
			//switch (pActiveBTEWeapon->m_iId)
			//{
				//case WEAPON_M95TIGER:
				//case WEAPON_M3DRAGON:
				//case WEAPON_M3DRAGONM:
				//case WEAPON_KRONOS12:
				//	pActiveBTEWeapon->pev->iuser1 = reader.ReadByte();
			//default:
			//	break;
			//}
		//}

		break;
	}
	case 2:
	{
		pos.x = reader.ReadCoord();
		pos.y = reader.ReadCoord();
		pos.z = reader.ReadCoord();
		arg1 = reader.ReadShort();
		arg2 = reader.ReadByte();
		//CreateBalrog11CannonSingleProjectile(NULL, pos, arg1, arg2);
		break;
	}
	case 3:
	{
		pos.x = reader.ReadCoord();
		pos.y = reader.ReadCoord();
		pos.z = reader.ReadCoord();
		arg1 = reader.ReadShort();
		arg2 = reader.ReadByte();
		//CreateFollowEnt(NULL, pos, arg1, arg2);
		break;
	}
	case 4:
	{
		arg1 = reader.ReadShort();
		arg2 = reader.ReadByte();

		//CreateAttachedEntitiesToPlayer(arg1, arg2);

		break;

	}
	case 7:
	{
		pos.x = reader.ReadCoord();
		pos.y = reader.ReadCoord();
		pos.z = reader.ReadCoord();

		bool bEnabled = reader.ReadByte() != 0;
		float flLife = reader.ReadByte() * 0.1;
		int iType = reader.ReadByte();

		//gHUD.m_FollowItem.SetIconItem(iType, pos, bEnabled, flLife);

		break;
	}
	case 15:
		switch (reader.ReadByte())
		{
		case 1:
			gEngfuncs.pfnPlaySoundByNameAtPitch("zombi/TD_Buff.wav", 1.0, 100);
			break;
		case 2:
			gEngfuncs.pfnPlaySoundByNameAtPitch("zombi/siren_scream.wav", 1.0, 100);
			break;
		case 44:
			gEngfuncs.pfnPlaySoundByNameAtLocation("weapons/sgmissile_reload.wav", VOL_NORM, gHUD.m_vecOrigin);
			break;
		case 80:
			gEngfuncs.pfnPlaySoundByNameAtLocation("weapons/divinetitan_charge.wav", VOL_NORM, gHUD.m_vecOrigin);
			break;
		case 81:
			gEngfuncs.pfnPlaySoundByNameAtLocation("weapons/bunkerbuster_gauge.wav", VOL_NORM, gHUD.m_vecOrigin);
			break;
		}

		break;
	case 16:
		arg1 = reader.ReadShort();
		arg2 = reader.ReadByte();

		switch (arg2)
		{
			/*case WINGGUN_WING:
			{
				int bOn = reader.ReadByte();

				if (bOn)
				{
					if (!iWingGunEffect[arg1])
						iWingGunEffect[arg1] = AttachTentToEntity(arg1, gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/ef_winggun_idle.spr"),
							Vector(0.0, 0.0, 50.0), 15.0, TRUE, FTENT_PERSIST | FTENT_SPRANIMATELOOP, 0.5, kRenderTransAdd, 30.0);
				}
				else
				{
					if (iWingGunEffect[arg1])
					{
						iWingGunEffect[arg1]->die = gHUD.m_flTime;
						iWingGunEffect[arg1] = nullptr;
					}
				}

				break;
			}
			case HOLYFIST_GLITCH_RING:
			{
				if (iHolyFistRingEffect[arg1])
				{
					iHolyFistRingEffect[arg1]->die = gHUD.m_flTime;
					iHolyFistRingEffect[arg1] = nullptr;
					break;
				}

				if (!iHolyFistRingEffect[arg1])
					iHolyFistRingEffect[arg1] = AttachTentToEntity(arg1, gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/terminator_ring.spr"),
						Vector(0.0, 0.0, 0.0), 5.0, TRUE, FTENT_PERSIST | FTENT_SPRANIMATELOOP, 0.7, kRenderTransAdd, 1.0);
				break;
			}*/
		default:
			break;
		}
		break;
	case 17:
	{
		arg1 = reader.ReadByte();
		arg2 = reader.ReadByte();


		cl_entity_t* pPlayer = gEngfuncs.GetEntityByIndex(arg1);


		break;
	}
	case 18:
	{
		arg1 = reader.ReadByte();
		arg2 = reader.ReadShort();

		switch (arg1)
		{
		case 0:
		{
			TEMPENTITY* pEnt = gEngfuncs.pEfxAPI->R_TempCustomModel({ 0, 0, 10 }, { 0, 0, 0 }, { 0, 0, 0 }, 999, gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/ef_wondercannon_bomb_set.spr"), 0, 15, false, 255, kRenderTransAdd, arg2, 0, false, 0, 0.2, 21, FTENT_PERSIST | FTENT_SPRANIMATELOOP | FTENT_PLYRATTACHMENT);
			break;
		}
		case 1:
		{
			TEMPENTITY* pEnt = gEngfuncs.pEfxAPI->R_TempCustomModel({ 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 999, gEngfuncs.pEventAPI->EV_FindModelIndex("models/ef_wondercannon_area.mdl"), 0, 1.0, false, 255, kRenderTransAdd, arg2, 0, false, 0, 1.0, 200, FTENT_PERSIST | FTENT_PLYRATTACHMENT);
			break;
		}
		case 2:
		{
			TEMPENTITY* pEnt = gEngfuncs.pEfxAPI->R_TempCustomModel({ 0, 0, 10 }, { 0, 0, 0 }, { 0, 0, 0 }, 999, gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/ef_wondercannonex_bomb_set.spr"), 0, 15, false, 255, kRenderTransAdd, arg2, 0, false, 0, 0.2, 21, FTENT_PERSIST | FTENT_SPRANIMATELOOP | FTENT_PLYRATTACHMENT);
			break;
		}
		case 3:
		{
			TEMPENTITY* pEnt = gEngfuncs.pEfxAPI->R_TempCustomModel({ 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 999, gEngfuncs.pEventAPI->EV_FindModelIndex("models/ef_wondercannonex_area.mdl"), 0, 1.0, false, 255, kRenderTransAdd, arg2, 0, false, 0, 1.0, 200, FTENT_PERSIST | FTENT_PLYRATTACHMENT);
			break;
		}
		default:
			break;
		}

		break;
	}
	case 19:
	{
		arg1 = reader.ReadShort();
		arg2 = reader.ReadShort();
		int arg3 = reader.ReadByte();

		cl_entity_t* pEnt = gEngfuncs.GetEntityByIndex(arg1);
		cl_entity_t* pLinkEnt = gEngfuncs.GetEntityByIndex(arg2);

		vec3_t vecBeamStart, vecBeamEnd;
		vecBeamStart = pEnt->origin;
		vecBeamEnd = pLinkEnt->origin;

		int iBeamIndex;

		if (arg3)
			iBeamIndex = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/ef_wondercannonex_chain.spr");
		else
			iBeamIndex = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/ef_wondercannon_chain.spr");

		BEAM* pBeam = gEngfuncs.pEfxAPI->R_BeamPoints_Stretch(vecBeamStart, vecBeamEnd, iBeamIndex, 1.0, 30.0, 255, 0, 30.0, 255, 255, 255);

		if (pBeam)
		{
			pBeam->startEntity = arg1;
			pBeam->endEntity = arg2;
			pBeam->flags = FBEAM_STARTENTITY | FBEAM_ENDENTITY;
		}

		break;
	}
	case 20:
	{
		arg1 = reader.ReadShort();

		//m_SniperScope.SetKronosTime(arg1);
		break;
	}
	case 21:
	{
		arg1 = reader.ReadByte();
		arg2 = reader.ReadByte();
		int arg3 = reader.ReadByte();
		//m_SniperScope.SetLockOnData(arg1, arg2, arg3);
		break;
	}
	case 22:
	{
		//m_SniperScope.ClearAllLockOnData();
		break;
	}
	case 23:
	{
		int length = reader.ReadByte();

		for (int i = 0; i < length; i++)
		{
			arg1 = reader.ReadShort();
			arg2 = reader.ReadByte();
			//	m_SniperScope.InsertPatrolDroneData(i, arg1, arg2);
		}

		//	m_SniperScope.SetPatrolDroneDeployTime();
		break;
	}
	case 24:
	{
		int Slot = reader.ReadByte();
		arg1 = reader.ReadShort();
		arg2 = reader.ReadByte();
		//	m_SniperScope.InsertPatrolDroneData(Slot, arg1, arg2);
		break;
	}
	case 25:
	{
		//bloodhunter
		arg1 = reader.ReadByte();
		arg2 = reader.ReadByte();
		//CBasePlayerWeapon* pActiveBTEWeapon = BTEClientWeapons().GetActiveWeaponEntity();
		//if (pActiveBTEWeapon)
		//{
			//if (pActiveBTEWeapon->m_iId == WEAPON_BLOODHUNTER)
			//{
				//if (arg1 < 3)	//iAnim
				//{
					//if (!g_flBloodhunterAnimTime)
					//	g_flBloodhunterAnimTime = gHUD.m_flTime;
					//g_iBloodhunterSecAnim = 19 + arg2;
				//}
				//else
				//{
					//g_flBloodhunterAnimTime = 0.0;
					//g_iBloodhunterSecAnim = 0;
			//	}
		//	}
		//	else
		//	{
			//	g_flBloodhunterAnimTime = 0.0;
			//	g_iBloodhunterSecAnim = 0;
		//	}
		//}
			break;
		//}
	case 26:
	{
		//bloodhunter
		arg1 = reader.ReadByte();
		//g_iBloodhunterState = arg1;
		break;
	}
	case 27:
	{
		//mgsm
		float arg3 = reader.ReadCoord();
		float arg4 = reader.ReadShort();
		//m_SniperScope.SetMGSMAmmo(arg3, arg4);
		break;
	}
	case 28:
	{
		arg1 = reader.ReadByte();
		arg2 = reader.ReadByte();
		//CBasePlayerWeapon* pActiveBTEWeapon = BTEClientWeapons().GetActiveWeaponEntity();
		//if (pActiveBTEWeapon)
		//{
			/*if (pActiveBTEWeapon->m_iId == WEAPON_MGSM)
			{
				if (arg1)	//iWpnState
				{
					//if (!g_flMGSMAnimTime)
					g_flMGSMAnimTime = gHUD.m_flTime;
					g_iMGSMSecAnim = arg2;
				}
				else
				{
					g_flMGSMAnimTime = 0.0;
					g_iMGSMSecAnim = 0;
				}
			}
			else
			{
				g_flMGSMAnimTime = 0.0;
				g_iMGSMSecAnim = 0;
			}*/
		//}
		break;
	}
	case 29:
	{
		//bunkerbuster
		arg2 = reader.ReadByte();
		float arg3 = reader.ReadCoord();
		float arg4 = reader.ReadCoord();

		//m_SniperScope.InsertBunkerBusterData(arg2, arg3, arg4);
		break;
	}
	case 30:
	{
		float arg3 = reader.ReadCoord();

		//m_SniperScope.InsertBunkerBusterData2(arg3);
		break;
	}
	case 31:
	{
		//g_iM1887xmasAnim = -1;
		//g_flM1887xmasAnimTime = 0.0;
		break;
	}
	case 32:
	{
		arg1 = reader.ReadByte();
		switch (arg1)
		{
		case 0:
		{
			//buffng7
			arg2 = reader.ReadByte();
			//g_iBUFFNG7State = arg2;
			break;
		}
		case 1:
		{
			//m95tiger
			arg2 = reader.ReadByte();
			//g_iM95TigerState = arg2;
			break;
		}
		default:
			break;
		}
		break;
	}
	case 33:
	{
		int iDidHit = reader.ReadByte();
		int iType = reader.ReadByte();

		//EV_DragonTailFX(iDidHit, iType);

		break;
	}
	case 34:
	{
		pos.x = reader.ReadCoord();
		pos.y = reader.ReadCoord();
		pos.z = reader.ReadCoord();
		//	EV_Crow9FX(pos);

		break;
	}
	case 35:
	{
		arg1 = reader.ReadByte();

		pos.x = reader.ReadCoord();
		pos.y = reader.ReadCoord();
		pos.z = reader.ReadCoord();

		//	EV_Explosion(arg1, pos);
		break;
	}
	case 36:
	{
		pos.x = reader.ReadCoord();
		pos.y = reader.ReadCoord();
		pos.z = reader.ReadCoord();

		gEngfuncs.pEfxAPI->R_SparkEffect(pos, 8, -200, 200);
		break;
	}
	case 37:
	{
		arg1 = reader.ReadByte();
		arg2 = reader.ReadByte();
		int arg3 = reader.ReadByte();
		//	m_SniperScope.SetHaloGunAmmo(arg1, arg2, arg3);
		break;
	}
	case 38:
	{
		arg1 = reader.ReadByte();
		arg2 = reader.ReadByte();

		switch (arg1)
		{
			//case 3: bReviveGunRetinaOn = arg2 ? true : false; break;
			//case 2: bReviveGunLoopRetinaOn = arg2 ? true : false; break;
			//case 1: bHaloGunHitRetinaOn = arg2 ? true : false; break;
			//case 0: bHaloGunLoopRetinaOn = arg2 ? true : false; break;
		default:
			break;
		}


		break;
	}
	case 44:
	{
		arg1 = reader.ReadByte();

		//	g_iSPKnifeAmmo = arg1;

		break;
	}
	case 42:
	{
		arg1 = reader.ReadShort();
		pos.x = reader.ReadCoord();
		pos.y = reader.ReadCoord();
		pos.z = reader.ReadCoord();

		R_AttachTentToEntity(arg1, gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/flame_burn01.spr"),
			pos, reader.ReadShort() * 0.1, TRUE, FTENT_FADEOUT | FTENT_SPRANIMATE | FTENT_PERSIST | FTENT_PLYRATTACHMENT | FTENT_SPRANIMATELOOP, 0.3, kRenderTransAdd, 1.0);

		break;
	}
	case 79:	// stop sound
		arg1 = reader.ReadShort();
		arg2 = reader.ReadShort();

		gEngfuncs.pEventAPI->EV_StopSound(arg1, arg2, reader.ReadString());

		break;

	}
	}
	return 1;
}

void CHudHeadIcon::R_AttachTentToEntity(int entity, int modelIndex, vec3_t offset, float life, int additive, int flags, float scale, int rendermode, float framerate)
{
	if (!modelIndex)
	{
		gEngfuncs.Con_Printf("No model %d!\n", modelIndex);
		return;
	}

	cl_entity_t* pEnity = gEngfuncs.GetEntityByIndex(entity);
	if (!pEnity)
	{
		gEngfuncs.Con_Printf("Couldn't get ClientEntity for %i\n", entity);
		return;
	}

	TEMPENTITY* pTemp = gEngfuncs.pEfxAPI->R_DefaultSprite(pEnity->origin + offset, modelIndex, 1.0);
	if (!pTemp)
	{
		gEngfuncs.Con_Printf("No temp ent.\n");

		return;
	}

	pTemp->entity.curstate.framerate = framerate;
	pTemp->entity.baseline.renderamt = pTemp->entity.curstate.renderamt = 255;
	pTemp->tentOffset = offset;
	pTemp->flags = FTENT_PLYRATTACHMENT | FTENT_SPRANIMATE | flags;
	pTemp->clientIndex = entity;
	pTemp->entity.curstate.renderfx = kRenderFxNoDissipation;
	pTemp->entity.curstate.scale = scale;
	pTemp->entity.curstate.rendermode = rendermode;
	if (additive)
		pTemp->entity.curstate.rendermode = kRenderTransAdd;
	pTemp->die = gHUD.m_flTime + life;

}


void CHudHeadIcon::R_AttachTentToPlayer(int client, int modelIndex, vec3_t offset, float life, int additive, int flags, float scale, int rendermode, float framerate)
{
	if (!modelIndex)
	{
		gEngfuncs.Con_Printf("No model %d!\n", modelIndex);
		return;
	}

	if (client <= 0 || client > gEngfuncs.GetMaxClients())
	{
		gEngfuncs.Con_Printf("Bad client %i in R_AttachTentToEntity()!\n", client);
		return;
	}

	cl_entity_t* pClient = gEngfuncs.GetEntityByIndex(client);
	if (!pClient)
	{
		gEngfuncs.Con_Printf("Couldn't get ClientEntity for %i\n", client);
		return;
	}

	TEMPENTITY* pTemp = gEngfuncs.pEfxAPI->R_DefaultSprite(pClient->origin + offset, modelIndex, 1.0);
	if (!pTemp)
	{
		gEngfuncs.Con_Printf("No temp ent.\n");

		return;
	}

	pTemp->entity.curstate.framerate = framerate;
	pTemp->entity.baseline.renderamt = pTemp->entity.curstate.renderamt = 255;
	pTemp->tentOffset = offset;
	pTemp->flags = FTENT_PLYRATTACHMENT | FTENT_SPRANIMATE | flags;
	pTemp->clientIndex = client;
	pTemp->entity.curstate.renderfx = kRenderFxNoDissipation;
	pTemp->entity.curstate.scale = scale;
	pTemp->entity.curstate.rendermode = rendermode;
	if (additive)
		pTemp->entity.curstate.rendermode = kRenderTransAdd;
	pTemp->die = gHUD.m_flTime + life;

}