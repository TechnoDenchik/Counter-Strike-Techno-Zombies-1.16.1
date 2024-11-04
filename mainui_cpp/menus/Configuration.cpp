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
#include "Bitmap.h"
#include "PicButton.h"
#include "YesNoMessageBox.h"
#include "keydefs.h"
#include "TabView.h"
#include "PlayerIntroduceDialog.h"


#define ART_BANNER	     	"gfx/shell/head_config"

class CMenuOptions: public CMenuFramework
{
private:
	
	void _Init( void ) override;
	CMenuYesNoMessageBox msgBox;
public:	

	void AskPredictEnable() { msgBox.Show(); }
	typedef CMenuFramework BaseClass;
	CMenuOptions() : CMenuFramework("CMenuOptions") { }

	// update dialog
	
};

static CMenuOptions	uiOptions;

/*
=================
CMenuOptions::Init
=================
*/
void CMenuOptions::_Init( void )
{
	banner.SetPicture( ART_BANNER );

	//msgBox.SetMessage( "Check the Internet for updates?" );
	//SET_EVENT( msgBox.onPositive, UI_OpenUpdatePage( false, true ) );

	AddItem( background );
	AddItem( banner );
	AddButton(L("Controls"), L("Change keyboard and mouse settings"), PC_CONTROLS, UI_Controls_Menu, QMF_NOTIFY );
	//AddButton("Customize", "Choose your player name, and select visual options for your character", PC_CUSTOMIZE, UI_PlayerSetup_Menu, QMF_NOTIFY);
	AddButton("Customize", L("Change sound volume and quality"), PC_CUSTOMIZE, UI_PlayerSetup_Menu, QMF_NOTIFY);
	AddButton(L("GameUI_Video"), L("Change screen size, video mode and gamma"), PC_AUDIO, UI_Audio_Menu, QMF_NOTIFY );
	AddButton( "Video",    "Change screen size, video mode and gamma", PC_VIDEO, UI_Video_Menu, QMF_NOTIFY );
//	AddButton( "Gamepad",  "Change gamepad axis and button settings", PC_GAMEPAD, UI_GamePad_Menu, QMF_NOTIFY );
	//AddButton(L("Update"), L("Check for updates"), PC_UPDATE, msgBox.MakeOpenEvent(), QMF_NOTIFY );
	AddButton(L("Done"), L("Go back to the Main menu"), PC_DONE, VoidCb( &CMenuOptions::Hide ), QMF_NOTIFY );

	msgBox.SetMessage("It is recomended to enable client movement prediction.\nPress OK to enable it now or enable it later in ^5(Multiplayer/Customize)");
	msgBox.SetPositiveButton("Ok", PC_OK);
	msgBox.SetNegativeButton("Cancel", PC_CANCEL);
	msgBox.HighlightChoice(CMenuYesNoMessageBox::HIGHLIGHT_YES);
	SET_EVENT_MULTI
	(
		msgBox.onPositive,
		{
			EngFuncs::CvarSetValue("cl_predict", 1.0f); EngFuncs::CvarSetValue("menu_mp_firsttime", 0.0f);
			UI_PlayerIntroduceDialog_Show(&uiOptions);
		}
	);
	SET_EVENT_MULTI
	(
		msgBox.onNegative,
		{
			EngFuncs::CvarSetValue("menu_mp_firsttime", 0.0f);

			UI_PlayerIntroduceDialog_Show(&uiOptions);
		}
	);
	msgBox.Link(this);
}

/*
=================
CMenuOptions::Precache
=================
*/
void UI_Options_Precache( void )
{
	EngFuncs::PIC_Load( ART_BANNER );
}

/*
=================
CMenuOptions::Menu
=================
*/
void UI_Options_Menu( void )
{
	
	if (gMenu.m_gameinfo.gamemode == GAME_SINGLEPLAYER_ONLY)
		return;

	uiOptions.Show();

	if (EngFuncs::GetCvarFloat("menu_mp_firsttime") && !EngFuncs::GetCvarFloat("cl_predict"))
	{
		uiOptions.AskPredictEnable();
	}
	else if (!UI::Names::CheckIsNameValid(EngFuncs::GetCvarString("name")))
	{
		UI_PlayerIntroduceDialog_Show(&uiOptions);
	}
}

ADD_MENU( menu_options, UI_Options_Precache, UI_Options_Menu );