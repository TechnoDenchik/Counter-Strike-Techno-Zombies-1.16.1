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
	void LerpingCvarWrite();

	CMenuSlider	soundVolume;
	CMenuSlider	musicVolume;
	CMenuSpinControl lerping;
	CMenuCheckBox noDSP;
	CMenuCheckBox muteFocusLost;
	CMenuCheckBox reverseChannels;


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

	lerping.LinkCvar( "s_lerping", CMenuEditable::CVAR_VALUE );
	noDSP.LinkCvar( "dsp_off" );
	muteFocusLost.LinkCvar( "snd_mute_losefocus" );
	reverseChannels.LinkCvar( "s_reverse_channels" );
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
	lerping.WriteCvar();
	noDSP.WriteCvar();
	muteFocusLost.WriteCvar();
	reverseChannels.WriteCvar();

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
	soundVolume.Setup( 0.0, 1.0, 0.05f );
	soundVolume.onChanged = CMenuEditable::WriteCvarCb;
	soundVolume.SetCoord( 320, 280 );

	musicVolume.SetNameAndStatus(L("GameUI_MP3Volume"), "Set background music volume level" );
	musicVolume.Setup( 0.0, 1.0, 0.05f );
	musicVolume.onChanged = CMenuEditable::WriteCvarCb;
	musicVolume.SetCoord( 320, 340 );

	static CStringArrayModel model( lerpingStr, ARRAYSIZE( lerpingStr ));
	lerping.SetNameAndStatus(L("Disable DSP effects"), "Enable/disable interpolation on sound output" );
	lerping.Setup( &model );
	lerping.onChanged = CMenuEditable::WriteCvarCb;
	lerping.font = QM_SMALLFONT;
	lerping.SetRect( 320, 470, 300, 32 );

	noDSP.SetNameAndStatus(L("Use Alpha DSP effects"), "Disable sound processing (like echo, flanger, etc)" );
	noDSP.onChanged = CMenuEditable::WriteCvarCb;
	noDSP.SetCoord( 320, 520 );

	muteFocusLost.SetNameAndStatus(L("Mute when inactive"), "Disable sound when game goes into background" );
	muteFocusLost.onChanged = CMenuEditable::WriteCvarCb;
	muteFocusLost.SetCoord( 320, 570 );

	reverseChannels.SetNameAndStatus( "Reverse audio channels", "Use it when you can't swap your headphones' speakers" );
	reverseChannels.onChanged = CMenuEditable::WriteCvarCb;
	reverseChannels.SetCoord( 320, 620 );

	AddItem( background );
	AddItem( banner );
	AddButton(L("Done"), "Go back to the Configuration Menu", PC_DONE,
		VoidCb( &CMenuAudio::SaveAndPopMenu ) );
	AddItem( soundVolume );
	AddItem( musicVolume );
	AddItem( lerping );
	AddItem( noDSP );
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
