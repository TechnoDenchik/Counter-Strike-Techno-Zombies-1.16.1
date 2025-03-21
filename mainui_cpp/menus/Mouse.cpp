/*
Copyright (C) 1997-2001 Id Software & TechnoSoftware, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "Framework.h"
#include "kbutton.h"
#include "MenuStrings.h"
#include "Bitmap.h"
#include "PicButton.h"
#include "CheckBox.h"
#include "Slider.h"

#define ART_BANNER			"gfx/shell/head_advanced"

class CMouseControls : public CMenuFramework
{
public:
	typedef CMenuFramework BaseClass;
	CMouseControls() : CMenuFramework("CAdvancedControls") { }

	void ToggleLookCheckboxes( bool write );

private:
	void _Init( void ) override;
	void _VidInit( void ) override;
	void SaveAndPopMenu() override;

	void GetConfig( void );
	void PitchInvert( void );

	CMenuPicButton Apply, inputDev;
	CMenuCheckBox	invertMouse;
	CMenuCheckBox	lookFilter;
	CMenuCheckBox	console;
	CMenuCheckBox	autoaim;
	CMenuSlider	sensitivity;
};

static CMouseControls	uimouseControls;

/*
=================
UI_AdvControls_GetConfig
=================
*/
void CMouseControls::GetConfig( )
{
	kbutton_t	*mlook;

	if( EngFuncs::GetCvarFloat( "m_pitch" ) < 0 )
		invertMouse.bChecked = true;

	lookFilter.LinkCvar( "look_filter" );
	console.LinkCvar("menu_getconsole");
	sensitivity.LinkCvar( "sensitivity" );

	ToggleLookCheckboxes( false );
}

void CMouseControls::PitchInvert()
{
	bool invert = invertMouse.bChecked;
	float m_pitch = EngFuncs::GetCvarFloat( "m_pitch" );
	if( ( invert && (m_pitch > 0) ) ||
		( !invert && (m_pitch < 0) ) )
	{
		EngFuncs::CvarSetValue( "m_pitch", -m_pitch );
	}
}

void CMouseControls::ToggleLookCheckboxes( bool write )
{
}

void CMouseControls::SaveAndPopMenu()
{
	lookFilter.WriteCvar();
	if( EngFuncs::GetCvarString("m_filter")[0] )
		EngFuncs::CvarSetValue( "m_filter", lookFilter.bChecked );
	sensitivity.WriteCvar();
	console.WriteCvar();

	if (EngFuncs::GetCvarString("menu_getconsole")[0])
		EngFuncs::CvarSetValue("menu_getconsole", console.bChecked);

	
	ToggleLookCheckboxes( true );

	CMenuFramework::SaveAndPopMenu();
}

/*
=================
UI_AdvControls_Init
=================
*/
void CMouseControls::_Init( void )
{
	banner.SetPicture( ART_BANNER );

	invertMouse.SetNameAndStatus(L("GameUI_MouseLook"), L(""));
	invertMouse.iFlags |= QMF_NOTIFY;
	invertMouse.onChanged = VoidCb( &CMouseControls::PitchInvert );
	invertMouse.SetCoord( 72, 300 );

	lookFilter.SetNameAndStatus(L("GameUI_MouseFilter"), L(""));
	lookFilter.iFlags |= QMF_NOTIFY;
	lookFilter.SetCoord( 72, 350 );

	console.SetNameAndStatus(L("GameUI_Console"), L(""));
	console.iFlags |= QMF_NOTIFY;
	console.SetCoord(72, 400);

	sensitivity.SetNameAndStatus(L("GameUI_MouseSensitivity"), L(""));
	sensitivity.Setup( 0.0, 20.0f, 0.1 );
	sensitivity.SetCoord( 72, 500 );

	Apply.SetNameAndStatus(L("GameUI_Apply"), L(""));
	Apply.onActivated = VoidCb(&CMouseControls::SaveAndPopMenu);
	Apply.iFlags |= QMF_NOTIFY;
	Apply.SetCoord(72, 550);

	AddItem( background );
	AddItem( banner );
	AddItem( Apply );
	AddItem( invertMouse );
	AddItem( console );
	AddItem( lookFilter );
	AddItem( sensitivity );
}


void CMouseControls::_VidInit()
{
	GetConfig();
}

/*
=================
UI_AdvControls_Precache
=================
*/
void UI_MouseControls_Precache( void )
{
	EngFuncs::PIC_Load( ART_BANNER );
}

/*
=================
UI_AdvControls_Menu
=================
*/
void UI_MouseControls_Menu( void )
{
	uimouseControls.Show();
}
ADD_MENU( menu_mousecontrols, UI_MouseControls_Precache, UI_MouseControls_Menu );
