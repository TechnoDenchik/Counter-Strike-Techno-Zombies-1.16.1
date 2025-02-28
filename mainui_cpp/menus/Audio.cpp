/*
Copyright (C) 1997-2001 Id Software, Inc.

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
#include "Slider.h"
#include "Bitmap.h"
#include "PicButton.h"
#include "CheckBox.h"
#include "SpinControl.h"
#include "StringArrayModel.h"

#define ART_BANNER			"gfx/shell/head_audio"

class CMenuAudio : public CMenuFramework
{
public:
	typedef CMenuFramework BaseClass;

	CMenuAudio() : CMenuFramework("CMenuAudio") { }

private:
	void _Init() override;
	void _VidInit() override;
	void GetConfig();
	void SaveAndPopMenu() override;

	CMenuPicButton done, getmusicmenu;
	CMenuPicButton exit2, exit;
	CMenuSlider	soundVolume;
	CMenuSlider	musicVolume;
	CMenuCheckBox muteFocusLost;
};

static CMenuAudio		uiAudio;

/*
=================
CMenuAudio::GetConfig
=================
*/
void CMenuAudio::GetConfig( void )
{
	soundVolume.LinkCvar( "volume" );
	musicVolume.LinkCvar( "MP3Volume" );
	muteFocusLost.LinkCvar( "snd_mute_losefocus" );
}

/*
=================
CMenuAudio::SetConfig
=================
*/
void CMenuAudio::SaveAndPopMenu()
{
	soundVolume.WriteCvar();
	musicVolume.WriteCvar();
	muteFocusLost.WriteCvar();

	CMenuFramework::SaveAndPopMenu();
}

/*
=================
CMenuAudio::Init
=================
*/
void CMenuAudio::_Init( void )
{
	static const char *lerpingStr[] =
	{
		"Disabled", "Balance", "Quality"
	};

	banner.SetPicture(ART_BANNER);

	soundVolume.SetNameAndStatus(L("GameUI_SoundEffectVolume"), "Set master volume level" );
	soundVolume.Setup( 0.00, 2.0, 0.001 );
	soundVolume.onChanged = CMenuEditable::WriteCvarCb;
	soundVolume.SetCoord( 320, 280 );

	musicVolume.SetNameAndStatus(L("GameUI_MP3Volume"), "Set background music volume level" );
	musicVolume.Setup( 0.00, 2.0, 0.001 );
	musicVolume.onChanged = CMenuEditable::WriteCvarCb;
	musicVolume.SetCoord( 320, 340 );

	getmusicmenu.SetNameAndStatus(L("CstzUI_musicpackb"), L(""));
	getmusicmenu.onActivated = UI_Music_Menu;
	getmusicmenu.iFlags |= QMF_NOTIFY;
	getmusicmenu.SetCoord(320, 380);

	muteFocusLost.SetNameAndStatus(L("CstzUI_MuteGame"), L("CstzUI_MuteGame2") );
	muteFocusLost.onChanged = CMenuEditable::WriteCvarCb;
	muteFocusLost.SetCoord( 320, 470 );

	AddItem( background );
	AddItem( getmusicmenu );
	AddItem( banner );

	exit.SetNameAndStatus(L("GameUI_Close"), L(""));
	exit.onActivated = VoidCb(( & CMenuAudio::SaveAndPopMenu) );
	exit.iFlags |= QMF_NOTIFY;
	exit.SetCoord(80, 250);

	AddItem( soundVolume );
	AddItem( musicVolume );
	AddItem( exit );
	AddItem( muteFocusLost );
}

void CMenuAudio::_VidInit( )
{
	GetConfig();
}

/*
=================
UI_Audio_Precache
=================
*/
void UI_Audio_Precache( void )
{
	EngFuncs::PIC_Load( ART_BANNER );
}

/*
=================
UI_Audio_Menu
=================
*/
void UI_Audio_Menu( void )
{
	uiAudio.Show();
}
ADD_MENU( menu_audio, UI_Audio_Precache, UI_Audio_Menu );
