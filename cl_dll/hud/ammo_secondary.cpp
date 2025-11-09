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
// ammo_secondary.cpp
//
// implementation of CHudAmmoSecondary class
//

#include "hud.h"
#include "cl_util.h"
#include <string.h>
#include <stdio.h>
#include "parsemsg.h"
#include "draw_util.h"

DECLARE_MESSAGE( m_AmmoSecondary, SecAmmoVal )
DECLARE_MESSAGE( m_AmmoSecondary, SecAmmoIcon )

inline void BuildNumberRC(wrect_t(&rgrc)[10], int w, int h)
{
	int nw = 0;

	for (int i = 0; i < 10; i++)
	{
		rgrc[i].left = nw;
		rgrc[i].top = 0;
		rgrc[i].right = rgrc[i].left + w;
		rgrc[i].bottom = h;

		nw += w;
	}
}

inline void BuildNumberRC(wrect_t(&rgrc)[10], int tex)
{
	int w = gRenderAPI.RenderGetParm(PARM_TEX_SRC_WIDTH, tex);
	int h = gRenderAPI.RenderGetParm(PARM_TEX_SRC_HEIGHT, tex);
	return BuildNumberRC(rgrc, w / 10, h);
}

inline void DrawTexturePart(const CTextureRef& tex, const wrect_t& rect, int x1, int y1, float scale = 1.0f)
{
	tex.Bind();

	float w = tex.w();
	float h = tex.h();

	x1 *= gHUD.m_flScale;
	y1 *= gHUD.m_flScale;
	scale *= gHUD.m_flScale;

	int x2 = x1 + (rect.right - rect.left) * scale;
	int y2 = y1 + (rect.bottom - rect.top) * scale;

	gEngfuncs.pTriAPI->Begin(TRI_QUADS);
	gEngfuncs.pTriAPI->TexCoord2f(rect.left / w, rect.top / h);
	gEngfuncs.pTriAPI->Vertex3f(x1, y1, 0);
	gEngfuncs.pTriAPI->TexCoord2f(rect.left / w, rect.bottom / h);
	gEngfuncs.pTriAPI->Vertex3f(x1, y2, 0);
	gEngfuncs.pTriAPI->TexCoord2f(rect.right / w, rect.bottom / h);
	gEngfuncs.pTriAPI->Vertex3f(x2, y2, 0);
	gEngfuncs.pTriAPI->TexCoord2f(rect.right / w, rect.top / h);
	gEngfuncs.pTriAPI->Vertex3f(x2, y1, 0);
	gEngfuncs.pTriAPI->End();
}

inline int DrawTexturedNumbersTopRightAligned(const CTextureRef& tex, const wrect_t(&rect)[10], int iNumber, int x, int y, float scale = 1.0f)
{

	do
	{
		int k = iNumber % 10;
		iNumber /= 10;
		DrawTexturePart(tex, rect[k], x, y, scale);
		x -= (rect[k].right - rect[k].left) * scale;
	} while (iNumber > 0);

	return x;
}

inline unsigned math_log10(unsigned v)
{
	return (v >= 1000000000) ? 9 : (v >= 100000000) ? 8 : (v >= 10000000) ? 7 :
		(v >= 1000000) ? 6 : (v >= 100000) ? 5 : (v >= 10000) ? 4 :
		(v >= 1000) ? 3 : (v >= 100) ? 2 : (v >= 10) ? 1 : 0;
}

inline int DrawTexturedNumbersTopCenterAligned(const CTextureRef& tex, const wrect_t(&rect)[10], int iNumber, int x, int y, float scale = 1.0f)
{
	int n = math_log10(iNumber);
	x += (rect[0].right - rect[0].left) * (n - 1) * scale * 0.5f;
	return DrawTexturedNumbersTopRightAligned(tex, rect, iNumber, x, y, scale);
}

int CHudAmmoSecondary :: Init( void )
{

	R_InitTexture(ammoclips, "resource/hud/zb3/hud_sb_num_big_white");
	R_InitTexture(weaponboard, "resource/hud/zb3/weapon_list_new");
	R_InitTexture(ammoboard, "resource/hud/zb3/hud_weapon_bg_bottom");
	R_InitTexture(ammoboard2, "resource/hud/zb3/hud_weapon_bg_top");
	BuildNumberRC(m_rcAmmoclip, 18, 22);
	BuildNumberRC(m_rcAmmofloat, 18, 22);

	HOOK_MESSAGE( SecAmmoVal );
	HOOK_MESSAGE( SecAmmoIcon );

	gHUD.AddHudElem(this);
	m_HUD_ammoicon = 0;

	for ( int i = 0; i < MAX_SEC_AMMO_VALUES; i++ )
		m_iAmmoAmounts[i] = -1;  // -1 means don't draw this value

	Reset();

	return 1;
}

void CHudAmmoSecondary :: Reset( void )
{
	m_fFade = 0;
}

int CHudAmmoSecondary :: VidInit( void )
{
	return 1;
}

int CHudAmmoSecondary :: Draw(float flTime)
{
	if ( (gHUD.m_iHideHUDDisplay & ( HIDEHUD_WEAPONS | HIDEHUD_ALL )) )
		return 1;

	// draw secondary ammo icons above normal ammo readout
	int a, x, y, r, g, b, AmmoWidth;
	DrawUtils::UnpackRGB( r, g, b, RGB_YELLOWISH );
	a = max( MIN_ALPHA, (int)m_fFade );
	if (m_fFade > 0)
		m_fFade -= (gHUD.m_flTimeDelta * 20);  // slowly lower alpha to fade out icons
	DrawUtils::ScaleColors( r, g, b, a );

	AmmoWidth = gHUD.GetSpriteRect(gHUD.m_HUD_number_0).right - gHUD.GetSpriteRect(gHUD.m_HUD_number_0).left;

	y = ScreenHeight - (gHUD.m_iFontHeight*4);  // this is one font height higher than the weapon ammo values
	x = ScreenWidth - AmmoWidth;

	if ( m_HUD_ammoicon )
	{
		// Draw the ammo icon
		x -= (gHUD.GetSpriteRect(m_HUD_ammoicon).right - gHUD.GetSpriteRect(m_HUD_ammoicon).left);
		y -= (gHUD.GetSpriteRect(m_HUD_ammoicon).top - gHUD.GetSpriteRect(m_HUD_ammoicon).bottom);

		SPR_Set( gHUD.GetSprite(m_HUD_ammoicon), r, g, b );
		SPR_DrawAdditive( 0, x, y, &gHUD.GetSpriteRect(m_HUD_ammoicon) );
	}
	else
	{  // move the cursor by the '0' char instead, since we don't have an icon to work with
		x -= AmmoWidth;
		y -= (gHUD.GetSpriteRect(gHUD.m_HUD_number_0).top - gHUD.GetSpriteRect(gHUD.m_HUD_number_0).bottom);
	}

	// draw the ammo counts, in reverse order, from right to left
	for ( int i = MAX_SEC_AMMO_VALUES-1; i >= 0; i-- )
	{


		int ammos = gHUD.m_Ammo.m_pWeapon->iClip;

		int x3 = ScreenWidth / 1.1;
		int y3 = ScreenHeight / 1.0820;

		int x4 = ScreenWidth / 1.1;
		int y4 = ScreenHeight / 1.0820;

		int x7 = ScreenWidth / 1.1;
		int y7 = ScreenHeight / 1.0840;

		gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
		gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);

		ammoboard->Bind();
		DrawUtils::Draw2DQuadScaled(x7 - 420 / 3.0, y7 + 4.5, x7 + 522 / 3.0, y7 + 80);

		gEngfuncs.pTriAPI->Color4ub(r, g, b, 255);

		if (ammos < 10)
		{
			DrawTexturedNumbersTopRightAligned(*ammoclips, m_rcAmmoclip, m_iAmmoAmounts[i], x3 - 5, y3 + 45, 1.0f);
		}
		else if (ammos < 1000)
		{
			DrawTexturedNumbersTopRightAligned(*ammoclips, m_rcAmmoclip, m_iAmmoAmounts[i], x3 - 5, y3 + 45, 1.0f);
		}
		else if (ammos < 10000)
		{
			DrawTexturedNumbersTopRightAligned(*ammoclips, m_rcAmmoclip, m_iAmmoAmounts[i], x3 - 5, y3 + 45, 1.0f);
		}
		else
		{
			DrawTexturedNumbersTopRightAligned(*ammoclips, m_rcAmmoclip, m_iAmmoAmounts[i], x3 - 5, y3 + 45, 1.0f);
		}

		if ( m_iAmmoAmounts[i] < 0 )
			continue; // negative ammo amounts imply that they shouldn't be drawn

		// half a char gap between the ammo number and the previous pic
		x -= (AmmoWidth / 2);

		// draw the number, right-aligned
		x -= (DrawUtils::GetNumWidth( m_iAmmoAmounts[i], DHN_DRAWZERO ) * AmmoWidth);
		//DrawUtils::DrawHudNumber( x, y, DHN_DRAWZERO, m_iAmmoAmounts[i], r, g, b );

		if ( i != 0 )
		{
			// draw the divider bar
			x -= (AmmoWidth / 2);
			FillRGBA(x, y, (AmmoWidth/10), gHUD.m_iFontHeight, r, g, b, a);
		}
	}

	return 1;
}

// Message handler for Secondary Ammo Value
// accepts one value:
//		string:  sprite name
int CHudAmmoSecondary :: MsgFunc_SecAmmoIcon( const char *pszName, int iSize, void *pbuf )
{
	BufferReader reader( pszName, pbuf, iSize );
	m_HUD_ammoicon = gHUD.GetSpriteIndex( reader.ReadString() );

	return 1;
}

// Message handler for Secondary Ammo Icon
// Sets an ammo value
// takes two values:
//		byte:  ammo index
//		byte:  ammo value
int CHudAmmoSecondary :: MsgFunc_SecAmmoVal( const char *pszName, int iSize, void *pbuf )
{
	BufferReader reader( pszName, pbuf, iSize );

	int index = reader.ReadByte();
	if ( index < 0 || index >= MAX_SEC_AMMO_VALUES )
		return 1;

	m_iAmmoAmounts[index] = reader.ReadByte();
	m_iFlags |= HUD_DRAW;

	// check to see if there is anything left to draw
	int count = 0;
	for ( int i = 0; i < MAX_SEC_AMMO_VALUES; i++ )
	{
		count += max( 0, m_iAmmoAmounts[i] );
	}

	if ( count == 0 ) 
	{	// the ammo fields are all empty, so turn off this hud area
		m_iFlags &= ~HUD_DRAW;
		return 1;
	}

	// make the icons light up
	m_fFade = 200.0f;

	return 1;
}


