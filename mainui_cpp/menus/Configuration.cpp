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
	void AskPredictEnable() { msgBox.Show(); }
	void _Init( void ) override;

public:
	typedef CMenuFramework BaseClass;
	CMenuOptions() : CMenuFramework("CMenuOptions") { }

	// update dialog
	CMenuYesNoMessageBox msgBox;
};

static CMenuOptions	uiOptions;

class CMenuMultiplayer : public CMenuFramework
{
public:
	CMenuMultiplayer() : CMenuFramework("CMenuMultiplayer") { }

	void AskPredictEnable() { msgBox.Show(); }

private:
	void _Init() override;

	// prompt dialog
	CMenuYesNoMessageBox msgBox;
};

static CMenuMultiplayer	uiMultiPlayer;

/*
=================
CMenuOptions::Init
=================
*/
void CMenuOptions::_Init( void )
{
	banner.SetPicture( ART_BANNER );

	msgBox.SetMessage( "Check the Internet for updates?" );
	SET_EVENT( msgBox.onPositive, UI_OpenUpdatePage( false, true ) );

	msgBox.Link( this );

	AddItem( background );
	AddItem( banner );
	AddButton(L("Controls"), L("Change keyboard and mouse settings"), PC_CONTROLS, UI_Controls_Menu, QMF_NOTIFY );
	AddButton(L("Customize"), L("Change sound volume and quality"), PC_CUSTOMIZE, UI_PlayerSetup_Menu, QMF_NOTIFY);
	AddButton(L("GameUI_Video"), L("Change screen size, video mode and gamma"), PC_AUDIO, UI_Audio_Menu, QMF_NOTIFY );
	AddButton( "Video",    "Change screen size, video mode and gamma", PC_VIDEO, UI_Video_Menu, QMF_NOTIFY );
//	AddButton( "Gamepad",  "Change gamepad axis and button settings", PC_GAMEPAD, UI_GamePad_Menu, QMF_NOTIFY );
	AddButton(L("Update"), L("Check for updates"), PC_UPDATE, msgBox.MakeOpenEvent(), QMF_NOTIFY );
	AddButton(L("Done"), L("Go back to the Main menu"), PC_DONE, VoidCb( &CMenuOptions::Hide ), QMF_NOTIFY );

	msgBox.SetMessage("It is recomended to enable client movement prediction.\nPress OK to enable it now or enable it later in ^5(Multiplayer/Customize)");
	msgBox.SetPositiveButton("Ok", PC_OK);
	msgBox.SetNegativeButton("Cancel", PC_CANCEL);
	msgBox.HighlightChoice(CMenuYesNoMessageBox::HIGHLIGHT_YES);
	SET_EVENT_MULTI(msgBox.onPositive,
		{
			EngFuncs::CvarSetValue("cl_predict", 1.0f);
			EngFuncs::CvarSetValue("menu_mp_firsttime", 0.0f);

			UI_PlayerIntroduceDialog_Show(&uiMultiPlayer);
		});
	SET_EVENT_MULTI(msgBox.onNegative,
		{
			EngFuncs::CvarSetValue("menu_mp_firsttime", 0.0f);

			UI_PlayerIntroduceDialog_Show(&uiMultiPlayer);
		});
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
	uiOptions.Show();
}

void UI_MultiPlayer_Menu(void)
{
	if (gMenu.m_gameinfo.gamemode == GAME_SINGLEPLAYER_ONLY)
		return;

	uiMultiPlayer.Show();

	if (EngFuncs::GetCvarFloat("menu_mp_firsttime") && !EngFuncs::GetCvarFloat("cl_predict"))
	{
		uiMultiPlayer.AskPredictEnable();
	}
	else if (!UI::Names::CheckIsNameValid(EngFuncs::GetCvarString("name")))
	{
		UI_PlayerIntroduceDialog_Show(&uiMultiPlayer);
	}
}
ADD_MENU(menu_multiplayer, UI_Options_Precache, UI_MultiPlayer_Menu);
ADD_MENU( menu_options, UI_Options_Precache, UI_Options_Menu );
ADD_MENU(menu_playersetup, UI_Options_Precache, UI_PlayerSetup_Menu);