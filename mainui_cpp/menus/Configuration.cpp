/*
Copyright (C) 1997-2025 Id Software & TechnoSoftware, Inc.

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

	CMenuPicButton KeyBoard, Keyboard1;
	CMenuPicButton Mouse, Mouse1;
	CMenuPicButton Profile1, Profile;
	CMenuPicButton Audio1, Audio;
	CMenuPicButton Video1, Video;
	CMenuPicButton Exit1, Exit;
	
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

	AddItem( background );
	AddItem( banner );

	KeyBoard.SetNameAndStatus(L("GameUI_Keyboard"), L(""));
	KeyBoard.onActivated = UI_Controls_Menu;
	KeyBoard.iFlags |= QMF_NOTIFY;
	KeyBoard.SetCoord(80, 300);

	Mouse.SetNameAndStatus(L("GameUI_Mouse"), L(""));
	Mouse.onActivated = UI_MouseControls_Menu;
	Mouse.iFlags |= QMF_NOTIFY;
	Mouse.SetCoord(80, 350);

	Profile.SetNameAndStatus(L("GameUI_Profile"), L(""));
	Profile.onActivated = UI_PlayerSetup_Menu;
	Profile.iFlags |= QMF_NOTIFY;
	Profile.SetCoord(80, 400);
	
	Audio.SetNameAndStatus(L("GameUI_Audio"), L(""));
	Audio.onActivated = UI_Audio_Menu;
	Audio.iFlags |= QMF_NOTIFY;
	Audio.SetCoord(80, 450);

	Video.SetNameAndStatus(L("GameUI_Video"), L(""));
	Video.onActivated = UI_Video_Menu;
	Video.iFlags |= QMF_NOTIFY;
	Video.SetCoord(80, 500);

	Exit.SetNameAndStatus(L("GameUI_Close"), L(""));
	Exit.onActivated = VoidCb(&CMenuOptions::Hide);
	Exit.iFlags |= QMF_NOTIFY;
	Exit.SetCoord(80, 550);

	
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

	AddItem(KeyBoard);
	AddItem(Mouse);
	AddItem(Profile);
	AddItem(Audio);
	AddItem(Video);
	AddItem(Exit);
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