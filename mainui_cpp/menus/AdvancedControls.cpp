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

class CAdvancedControls : public CMenuFramework
{
public:
	typedef CMenuFramework BaseClass;
	CAdvancedControls() : CMenuFramework("CAdvancedControls") { }

	void ToggleLookCheckboxes( bool write );

private:
	void _Init( void ) override;
	void _VidInit( void ) override;
	void SaveAndPopMenu() override;

	void GetConfig( void );
	void PitchInvert( void );

	CMenuPicButton Apply, inputDev;

	CMenuCheckBox	crosshair;
	CMenuCheckBox	invertMouse;
	CMenuCheckBox	lookFilter;
	CMenuCheckBox	autoaim;
	CMenuSlider	sensitivity;
};

static CAdvancedControls	uiAdvControls;

/*
=================
UI_AdvControls_GetConfig
=================
*/
void CAdvancedControls::GetConfig( )
{
	kbutton_t	*mlook;

	if( EngFuncs::GetCvarFloat( "m_pitch" ) < 0 )
		invertMouse.bChecked = true;

	crosshair.LinkCvar( "crosshair" );
	lookFilter.LinkCvar( "look_filter" );
	sensitivity.LinkCvar( "sensitivity" );

	ToggleLookCheckboxes( false );
}

void CAdvancedControls::PitchInvert()
{
	bool invert = invertMouse.bChecked;
	float m_pitch = EngFuncs::GetCvarFloat( "m_pitch" );
	if( ( invert && (m_pitch > 0) ) ||
		( !invert && (m_pitch < 0) ) )
	{
		EngFuncs::CvarSetValue( "m_pitch", -m_pitch );
	}
}

void CAdvancedControls::ToggleLookCheckboxes( bool write )
{
}

void CAdvancedControls::SaveAndPopMenu()
{
	crosshair.WriteCvar();
	lookFilter.WriteCvar();
	if( EngFuncs::GetCvarString("m_filter")[0] )
		EngFuncs::CvarSetValue( "m_filter", lookFilter.bChecked );
	sensitivity.WriteCvar();

	ToggleLookCheckboxes( true );

	CMenuFramework::SaveAndPopMenu();
}

/*
=================
UI_AdvControls_Init
=================
*/
void CAdvancedControls::_Init( void )
{
	banner.SetPicture( ART_BANNER );

	crosshair.SetNameAndStatus(L("CstzUI_Crosshair"), L(""));
	crosshair.iFlags |= QMF_NOTIFY;
	crosshair.SetCoord( 72, 280 );

	invertMouse.SetNameAndStatus(L("GameUI_MouseLook"), L(""));
	invertMouse.iFlags |= QMF_NOTIFY;
	invertMouse.onChanged = VoidCb( &CAdvancedControls::PitchInvert );
	invertMouse.SetCoord( 72, 330 );

	lookFilter.SetNameAndStatus(L("GameUI_MouseFilter"), L(""));
	lookFilter.iFlags |= QMF_NOTIFY;
	lookFilter.SetCoord( 72, 530 );

	sensitivity.SetNameAndStatus(L("GameUI_MouseSensitivity"), L(""));
	sensitivity.Setup( 0.0, 20.0f, 0.1 );
	sensitivity.SetCoord( 72, 660 );

	Apply.SetNameAndStatus(L("GameUI_Apply"), L(""));
	Apply.onActivated = VoidCb(&CAdvancedControls::SaveAndPopMenu);
	Apply.iFlags |= QMF_NOTIFY;
	if (CL_IsActive() && !EngFuncs::GetCvarFloat("host_serverstate"))
		Apply.SetGrayed(true);
	Apply.SetCoord(72, 680);

	AddItem( background );
	AddItem( banner );
	AddItem( Apply );
	AddItem( crosshair );
	AddItem( invertMouse );
	AddItem( lookFilter );
	AddItem( sensitivity );
}


void CAdvancedControls::_VidInit()
{
	GetConfig();
}

/*
=================
UI_AdvControls_Precache
=================
*/
void UI_AdvControls_Precache( void )
{
	EngFuncs::PIC_Load( ART_BANNER );
}

/*
=================
UI_AdvControls_Menu
=================
*/
void UI_AdvControls_Menu( void )
{
	uiAdvControls.Show();
}
ADD_MENU( menu_advcontrols, UI_AdvControls_Precache, UI_AdvControls_Menu );
