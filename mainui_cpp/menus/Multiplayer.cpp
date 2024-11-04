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
#include "YesNoMessageBox.h"
#include "keydefs.h"
#include "PlayerIntroduceDialog.h"

#define ART_BANNER			"gfx/shell/head_multi"

class CMenuMultiplayer2 : public CMenuFramework
{
public:
	CMenuMultiplayer2() : CMenuFramework( "CMenuMultiplayer" ) { }

	void AskPredictEnable() { msgBox.Show(); }

private:
	void _Init() override;

	// prompt dialog
	CMenuYesNoMessageBox msgBox;
};

static CMenuMultiplayer2	uiMultiPlayer;


/*
=================
CMenuMultiplayer::Init
=================
*/
void CMenuMultiplayer2::_Init( void )
{

	AddItem( background );

	banner.SetPicture( ART_BANNER );
	AddItem( banner );

	AddButton( "Internet game", "View list of a game internet servers and join the one of your choice", PC_INET_GAME, UI_InternetGames_Menu, QMF_NOTIFY );

	AddButton( "LAN game", "Set up the game on the local area network", PC_LAN_GAME, UI_LanGame_Menu, QMF_NOTIFY );
//	AddButton( "Customize", "Choose your player name, and select visual options for your character", PC_CUSTOMIZE, UI_PlayerSetup_Menu, QMF_NOTIFY );
	AddButton( "Controls", "Change keyboard and mouse settings", PC_CONTROLS, UI_Controls_Menu, QMF_NOTIFY );
	AddButton( "Done", "Go back to the Main menu", PC_DONE, VoidCb( &CMenuMultiplayer2::Hide ), QMF_NOTIFY );

	msgBox.SetMessage( "It is recomended to enable client movement prediction.\nPress OK to enable it now or enable it later in ^5(Multiplayer/Customize)");
	msgBox.SetPositiveButton( "Ok", PC_OK );
	msgBox.SetNegativeButton( "Cancel", PC_CANCEL );
	msgBox.HighlightChoice( CMenuYesNoMessageBox::HIGHLIGHT_YES );
	msgBox.Link( this );

}

/*
=================
CMenuMultiplayer::Precache
=================
*/
void UI_MultiPlayer_Precache( void )
{
	EngFuncs::PIC_Load( ART_BANNER );
}

/*
=================
UI_MultiPlayer_Menu
=================
*/
void UI_MultiPlayer_Menu2( void )
{

	uiMultiPlayer.Show();

}
ADD_MENU( menu_multiplayer, UI_MultiPlayer_Precache, UI_MultiPlayer_Menu2 );
