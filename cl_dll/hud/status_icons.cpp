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
//
// status_icons.cpp
//
#include "hud.h"
#include "cl_util.h"
#include "const.h"
#include "entity_state.h"
#include "cl_entity.h"
#include <string.h>
#include <stdio.h>
#include "parsemsg.h"
#include "event_api.h"
#include "com_weapons.h"
#include "gamemode/mods_const.h"
#include "draw_util.h"
#include "triangleapi.h"

DECLARE_MESSAGE( m_StatusIcons, StatusIcon )
DECLARE_MESSAGE( m_StatusIcons, ShelterIcon)

int CHudStatusIcons::Init( void )
{
	HOOK_MESSAGE( StatusIcon );
	HOOK_MESSAGE( ShelterIcon);

	gHUD.AddHudElem( this );

	Reset();

	m_tgaC4[0] = m_tgaC4[1] = 0;

	return 1;
}

int CHudStatusIcons::VidInit( void )
{
	m_tgaC4[0] = gRenderAPI.GL_LoadTexture("resource/helperhud/c4_left_default", NULL, 0, TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	m_tgaC4[1] = gRenderAPI.GL_LoadTexture("resource/helperhud/c4_left_install", NULL, 0, TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	R_InitTexture(b_iconimage, "resource/shelterteam/ingame_shopkey");
	R_InitTexture(b_iconbuild, "resource/shelterteam/ingame_housingkey");
	R_InitTexture(b_iconskills, "resource/shelterteam/ingame_skillkey");
	return 1;
}

void CHudStatusIcons::Reset( void )
{
	memset( m_IconList, 0, sizeof m_IconList );
	if (gHUD.m_iModRunning == MOD_ZSH)
	{
		m_iFlags |= HUD_DRAW;
	}
	else
	{
		m_iFlags &= ~HUD_DRAW;
	}
}

void CHudStatusIcons::Shutdown(void)
{
	for (int iTexture : m_tgaC4)
		gRenderAPI.GL_FreeTexture(iTexture);
}

int CHudStatusIcons::Draw( float flTime )
{
	if (gEngfuncs.IsSpectateOnly())
		return 1;

	int x = 5;
	int y = ScreenHeight / 2;

	int x2 = ScreenWidth / 1.93;
	int y2 = ScreenHeight / 1.23;

	int x3 = ScreenWidth / 2.10;
	int y3 = ScreenHeight / 1.23;

	int x4 = ScreenWidth / 2;
	int y4 = ScreenHeight / 1.36;
	
	char szTitle[90];
	const float flScale = 0.0f;

	const int iStartX = (float)ScreenHeight / (float)ScreenWidth < 0.75 ? 100 * (ScreenHeight / 768.0) + 4 : 4;
	const int iStartY = 40 * (ScreenHeight / 768.0);
	const int iStartW = ScreenWidth - 2 * iStartX + 4;
	const int iStartH = ScreenHeight - 2 * iStartY + 25;
	const int iCharHeightOffset = 0;
	int iDraw = 0;
	int offsetY = 120 + 21 * iDraw + iCharHeightOffset - 6;

	if (gHUD.m_iModRunning == MOD_ZSH)
	{
		gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
		gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
		b_iconskills->Bind();
		DrawUtils::Draw2DQuadScaled(x2 - 25, y2 - 3.5, x2 + 25, y2 + 42);

		m_colors.r = 0;
		m_colors.g = 0;
		m_colors.b = 0;
		m_colors.a = 153;
		m_bDrawStroke = true;

		sprintf(szTitle, "Навыки");
		DrawUtils::DrawHudStringReverse(x2 + 25, y2 + 43.5, 0, szTitle, 140, 92, 3, 255, flScale);

		if (buyzones == false)
		{
			gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
			gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
			b_iconbuild->Bind();
			DrawUtils::Draw2DQuadScaled(x3 - 25, y3 - 3.5, x3 + 25, y3 + 42);
			sprintf(szTitle, "Стройка");
			DrawUtils::DrawHudStringReverse(x3 + 25, x3 + 7.0, 0, szTitle, 140, 92, 3, 255, flScale);
		}
	}
	else
	{
		for (int i = 0; i < MAX_ICONSPRITES; i++)
		{
			if (m_IconList[i].spr)
			{
				y -= (m_IconList[i].rc.bottom - m_IconList[i].rc.top) + 5;

				if (gHUD.m_iModRunning != MOD_ZB5)
				{
					gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
					gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
					b_iconimage->Bind();
					DrawUtils::Draw2DQuadScaled(x4 - 30, y4 - 3.5, x4 + 30, y4 + 42);
					sprintf(szTitle, "Магазин");
					DrawUtils::DrawHudStringReverse(x4 + 25, y4 + 40.0, 0, szTitle, 0, 200, 0, 255, flScale);
				}
			}
		}
	}
	
	return 1;
}

// Message handler for StatusIcon message
// accepts five values:
//		byte   : TRUE = ENABLE icon, FALSE = DISABLE icon
//		string : the sprite name to display
//		byte   : red
//		byte   : green
//		byte   : blue
int CHudStatusIcons::MsgFunc_StatusIcon( const char *pszName, int iSize, void *pbuf )
{
	BufferReader reader( pszName, pbuf, iSize );

	int ShouldEnable = reader.ReadByte();
	char *pszIconName = reader.ReadString();

	if ( ShouldEnable )
	{
		int r = reader.ReadByte();
		int g = reader.ReadByte();
		int b = reader.ReadByte();
		EnableIcon( pszIconName, r, g, b );	
		m_iFlags |= HUD_DRAW;
		buyzones = true;
	}
	else
	{
		buyzones = false;
		DisableIcon( pszIconName );
	}

	return 1;
}

int CHudStatusIcons::MsgFunc_ShelterIcon(const char* pszName, int iSize, void* pbuf)
{
	BufferReader reader(pszName, pbuf, iSize);

	int ShouldEnable = reader.ReadByte();
	char* pszIconName = reader.ReadString();

	if (ShouldEnable)
	{
		int r = reader.ReadByte();
		int g = reader.ReadByte();
		int b = reader.ReadByte();
		EnableIcon(pszIconName, r, g, b);
	
		//m_iFlags |= HUD_DRAW;
	}
	else
	{
	
		DisableIcon(pszIconName);
	}

	return 1;
}

// add the icon to the icon list, and set it's drawing color
void CHudStatusIcons::EnableIcon( const char *pszIconName, unsigned char red, unsigned char green, unsigned char blue )
{
	// check to see if the sprite is in the current list
	int i;
	for ( i = 0; i < MAX_ICONSPRITES; i++ )
	{
		if ( !stricmp( m_IconList[i].szSpriteName, pszIconName ) )
			break;
	}

	if ( i == MAX_ICONSPRITES )
	{
		// icon not in list, so find an empty slot to add to
		for ( i = 0; i < MAX_ICONSPRITES; i++ )
		{
			if ( !m_IconList[i].spr )
				break;
		}
	}

	// if we've run out of space in the list, overwrite the first icon
	if ( i == MAX_ICONSPRITES )
	{
		i = 0;
	}

	// Load the sprite and add it to the list
	// the sprite must be listed in hud.txt
	int spr_index = gHUD.GetSpriteIndex( pszIconName );
	m_IconList[i].spr = gHUD.GetSprite( spr_index );
	m_IconList[i].rc = gHUD.GetSpriteRect( spr_index );
	m_IconList[i].r = red;
	m_IconList[i].g = green;
	m_IconList[i].b = blue;
	strncpy( m_IconList[i].szSpriteName, pszIconName, MAX_ICONSPRITENAME_LENGTH );

	// Hack: Play Timer sound when a grenade icon is played (in 0.8 seconds)
	if ( strstr(m_IconList[i].szSpriteName, "grenade") )
	{
		cl_entity_t *pthisplayer = gEngfuncs.GetLocalPlayer();
		gEngfuncs.pEventAPI->EV_PlaySound( pthisplayer->index, pthisplayer->origin, CHAN_STATIC, "weapons/timer.wav", 1.0, ATTN_NORM, 0, PITCH_NORM );
	}
}

void CHudStatusIcons::DisableIcon( const char *pszIconName )
{
	// find the sprite is in the current list
	for ( int i = 0; i < MAX_ICONSPRITES; i++ )
	{
		if ( !stricmp( m_IconList[i].szSpriteName, pszIconName ) )
		{
			// clear the item from the list
			memset( &m_IconList[i], 0, sizeof( icon_sprite_t ) );
			return;
		}
	}
}
