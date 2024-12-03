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
#include "Action.h"
#include "Bitmap.h"
#include "PicButton.h"
#include "YesNoMessageBox.h"
#include "keydefs.h"
#include "MenuStrings.h"
#include "PlayerIntroduceDialog.h"

#define ART_MINIMIZE_N	"gfx/shell/min_n"
#define ART_MINIMIZE_F	"gfx/shell/min_f"
#define ART_MINIMIZE_D	"gfx/shell/min_d"
#define ART_CLOSEBTN_N	"gfx/shell/cls_n"
#define ART_CLOSEBTN_F	"gfx/shell/cls_f"
#define ART_CLOSEBTN_D	"gfx/shell/cls_d"
#define ART_DISCORD		"gfx/shell/discord"

class CMenuMain: public CMenuFramework
{
public:
	CMenuMain() : CMenuFramework( "CMenuMain" ) { }

	const char *Key( int key, int down ) override;
	const char *Activate( ) override;

private:
	void _Init() override;
	void _VidInit( ) override;

	void QuitDialog( void *pExtra = NULL );
	void DisconnectDialogCb();
	void HazardCourseDialogCb();
	void HazardCourseCb();

	CMenuPicButton	console;
	class CMenuMainBanner : public CMenuBannerBitmap
	{
	public:
		virtual void Draw();
	} banner;

	class CMenuVidPreview : public CMenuBitmap
	{
		void Draw() override;
	} testImage;

	CMenuPicButton	resumeGame;
	CMenuPicButton	disconnect;
	CMenuPicButton	credits;
	CMenuPicButton	createGame;
	CMenuPicButton	configuration;
	CMenuPicButton	multiPlayer;
	CMenuPicButton	previews;
	CMenuPicButton	discord;
	CMenuPicButton	github;
	CMenuPicButton	quit;

	CMenuYesNoMessageBox dialog;

	int		outlineWidth;
	bool bTrainMap;
	bool bCustomGame;
};

static CMenuMain uiMain;

void CMenuMain::CMenuVidPreview::Draw()
{
	int		color = 0xFFFF0000; // 255, 0, 0, 255
	int		viewport[4];
	int		viewsize, size, sb_lines;

	viewsize = EngFuncs::GetCvarFloat("viewsize");

	if (viewsize >= 120)
		sb_lines = 0;	// no status bar at all
	else if (viewsize >= 110)
		sb_lines = 24;	// no inventory
	else sb_lines = 48;

	size = Q_min(viewsize, 100);

	viewport[2] = m_scSize.w * size / 100;
	viewport[3] = m_scSize.h * size / 100;

	if (viewport[3] > m_scSize.h - sb_lines)
		viewport[3] = m_scSize.h - sb_lines;
	if (viewport[3] > m_scSize.h)
		viewport[3] = m_scSize.h;

	viewport[2] &= ~7;
	viewport[3] &= ~1;

	viewport[0] = (m_scSize.w - viewport[2]) / 2;
	viewport[1] = (m_scSize.h - sb_lines - viewport[3]) / 2;

	UI_DrawPic(m_scPos.x + viewport[0], m_scPos.y + viewport[1], viewport[2], viewport[3], uiColorWhite, szPic);
	UI_DrawRectangleExt(m_scPos, m_scSize, color, ((CMenuMain*)Parent())->outlineWidth);
}

void CMenuMain::CMenuMainBanner::Draw()
{
	if( !uiMain.background.ShouldDrawLogoMovie() )
		return; // no logos for steam background

	if( EngFuncs::GetLogoLength() <= 0.05f || EngFuncs::GetLogoWidth() <= 32 )
		return;	// don't draw stub logo (GoldSrc rules)

	float	logoWidth, logoHeight, logoPosY;
	float	scaleX, scaleY;

	scaleX = ScreenWidth / 1920.0f;
	scaleY = ScreenHeight / 1080.0f;

	// a1ba: multiply by height scale to look better on widescreens
	logoWidth = EngFuncs::GetLogoWidth() * scaleX;
	logoHeight = EngFuncs::GetLogoHeight() * scaleY * uiStatic.scaleY;
	logoPosY = 70 * scaleY * uiStatic.scaleY;	// 70 it's empirically determined value (magic number)

	EngFuncs::DrawLogo( "technocorp.avi", 0, logoPosY, logoWidth, logoHeight );
}

void CMenuMain::QuitDialog(void *pExtra)
{
	if( CL_IsActive() && EngFuncs::GetCvarFloat( "host_serverstate" ) && EngFuncs::GetCvarFloat( "maxplayers" ) == 1.0f )
		dialog.SetMessage( MenuStrings[IDS_MAIN_QUITPROMPTINGAME] );
	else
		dialog.SetMessage(L("CstzUI_Exit"));

	dialog.onPositive.SetCommand( FALSE, "quit\n" );
	dialog.Show();
}

void CMenuMain::DisconnectDialogCb()
{
	dialog.onPositive.SetCommand( FALSE, "cmd disconnect;endgame disconnect;wait;wait;wait;menu_options;menu_main\n" );
	dialog.SetMessage(L("CstzUI_MainDiscon"));
	dialog.Show();
}

void CMenuMain::HazardCourseDialogCb()
{
	dialog.onPositive = VoidCb( &CMenuMain::HazardCourseCb );;
	dialog.SetMessage( L("CstzUI_Hazard") );
	dialog.Show();
}

/*
=================
CMenuMain::Key
=================
*/
const char *CMenuMain::Key( int key, int down )
{
	if( down && UI::Key::IsEscape( key ) )
	{
		if ( CL_IsActive( ))
		{
			if( !dialog.IsVisible() )
				UI_CloseMenu();
		}
		else
		{
			QuitDialog( );
		}
		return uiSoundNull;
	}
	return CMenuFramework::Key( key, down );
}

/*
=================
UI_Main_ActivateFunc
=================
*/
const char *CMenuMain::Activate( void )
{
	if ( CL_IsActive( ))
	{
		resumeGame.Show();
		disconnect.Show();
	}
	else
	{
		resumeGame.Hide();
		disconnect.Hide();
	}

	if( gpGlobals->developer )
	{
		console.pos.y = CL_IsActive() ? 130 : 230;
		console.pos.y += 50;
	}

	CMenuPicButton::ClearButtonStack();

	return 0;
}

/*
=================
UI_Main_HazardCourse
=================
*/
void CMenuMain::HazardCourseCb()
{
	if( EngFuncs::GetCvarFloat( "host_serverstate" ) && EngFuncs::GetCvarFloat( "maxplayers" ) > 1 )
		EngFuncs::HostEndGame( "end of the game" );

	EngFuncs::CvarSetValue( "skill", 1.0f );
	EngFuncs::CvarSetValue( "deathmatch", 0.0f );
	EngFuncs::CvarSetValue( "teamplay", 0.0f );
	EngFuncs::CvarSetValue( "pausable", 1.0f ); // singleplayer is always allowing pause
	EngFuncs::CvarSetValue( "coop", 0.0f );
	EngFuncs::CvarSetValue( "maxplayers", 1.0f ); // singleplayer

	EngFuncs::PlayBackgroundTrack( NULL, NULL );

	EngFuncs::ClientCmd( FALSE, "hazardcourse\n" );
}

void CMenuMain::_Init( void )
{
	bTrainMap = false;
	bCustomGame = false;

	// console
	console.SetNameAndStatus(L("GameUI_Console"), "Show console" );
	console.iFlags |= QMF_NOTIFY;
	console.SetPicture( PC_CONSOLE );
	SET_EVENT_MULTI( console.onActivated,
	{
		UI_SetActiveMenu( FALSE );
		EngFuncs::KEY_SetDest( KEY_CONSOLE );
	});

	testImage.iFlags = QMF_INACTIVE;
	testImage.SetRect(490, 225, 480, 450);
	testImage.SetPicture(ART_DISCORD);

	resumeGame.SetNameAndStatus(L("GameUI_GameMenu_ResumeGame"), 0);
	resumeGame.SetPicture( PC_RESUME_GAME );
	resumeGame.iFlags |= QMF_NOTIFY;
	resumeGame.onActivated = UI_CloseMenu;

	disconnect.SetNameAndStatus(L("GameUI_GameMenu_Disconnect"), 0);
	disconnect.SetPicture( PC_DISCONNECT );
	disconnect.iFlags |= QMF_NOTIFY;
	disconnect.onActivated = VoidCb( &CMenuMain::DisconnectDialogCb );

	credits.SetNameAndStatus( "Credits", 0);
	credits.SetPicture( PC_VIEW_README );
	credits.iFlags |= QMF_NOTIFY;
	credits.onActivated = UI_Credits_Menu;

	createGame.SetNameAndStatus( "Create Game", 0);
	createGame.SetPicture(PC_CREATE_GAME);
	createGame.iFlags |= QMF_NOTIFY;
	createGame.onActivated = UI_CreateGame_Menu;

	multiPlayer.SetNameAndStatus(L("GameUI_Multiplayer"), 0);
	multiPlayer.SetPicture( PC_MULTIPLAYER );
	multiPlayer.iFlags |= QMF_NOTIFY;
	multiPlayer.onActivated = UI_InternetGames_Menu;

	configuration.SetNameAndStatus(L("GameUI_Options"), 0);
	configuration.SetPicture( PC_CONFIG );
	configuration.iFlags |= QMF_NOTIFY;
	configuration.onActivated = UI_Options_Menu;

	previews.SetNameAndStatus(L("Previews"), 0);
	previews.SetPicture( PC_PREVIEWS );
	previews.iFlags |= QMF_NOTIFY;
	SET_EVENT( previews.onActivated, EngFuncs::ShellExecute( MenuStrings[IDS_MEDIA_PREVIEWURL], NULL, false ) );


	discord.SetNameAndStatus(0, L("CstzUI_Discord"));
	discord.SetPicture(ART_DISCORD);
	discord.iFlags |= QMF_MOUSEONLY;
	discord.eFocusAnimation = QM_HIGHLIGHTIFFOCUS;
	SET_EVENT(discord.onActivated, EngFuncs::ShellExecute("https://github.com/TechnoDenchik/Counter-Strike-Techno-Zombies-1.16.1", NULL, false));

	quit.SetNameAndStatus(L("GameUI_GameMenu_Quit"), 0);
	quit.SetPicture( PC_QUIT );
	quit.iFlags |= QMF_NOTIFY;
	quit.onActivated = MenuCb( &CMenuMain::QuitDialog );


	if ( gMenu.m_gameinfo.gamemode == GAME_MULTIPLAYER_ONLY || gMenu.m_gameinfo.startmap[0] == 0 )
		credits.SetGrayed( true );

	if ( gMenu.m_gameinfo.gamemode == GAME_SINGLEPLAYER_ONLY )
		multiPlayer.SetGrayed( true );

	// server.dll needs for reading savefiles or startup newgame
	if( !EngFuncs::CheckGameDll( ))
	{
		credits.SetGrayed( true );
	}

	dialog.Link( this );

	AddItem( background );
	AddItem( banner );

	if ( gpGlobals->developer )
		AddItem( console );

	AddItem( disconnect );
	AddItem( discord );
	AddItem( resumeGame );
	//AddItem( credits );

	AddItem( createGame );
	AddItem( configuration );
	AddItem( multiPlayer );

	AddItem( previews );
	AddItem( quit );
	//AddItem( minimizeBtn );
	//AddItem( quitButton );
}

/*
=================
UI_Main_Init
=================
*/
void CMenuMain::_VidInit( void )
{
	Activate();

	console.pos.x = 32;
	resumeGame.SetCoord( 32, 440);
	disconnect.SetCoord( 32, 480);
	//credits.SetCoord( 72, 280 );
	createGame.SetCoord( 32, 520 );

	configuration.SetCoord( 32, 560 );
	multiPlayer.SetCoord( 32, 600 );

	previews.SetCoord( 32,  640);
	discord.SetRect(uiStatic.width - 102, 220, 32, 32);

	// too short execute string - not a real command
	if( strlen("https://github.com/TechnoDenchik/Counter-Strike-Techno-Zombies-1.16.0") <= 3 )
		previews.SetGrayed( true );

	if (strlen("https://discord.gg/U9sdYbZrRU") <= 3)
		discord.SetGrayed(true);

	quit.SetCoord( 32, 680 );

	outlineWidth = 2;
	UI_ScaleCoords(NULL, NULL, &outlineWidth, NULL);

	//minimizeBtn.SetRect( uiStatic.width - 72, 13, 32, 32 );

	//quitButton.SetRect( uiStatic.width - 36, 13, 32, 32 );
}

/*
=================
UI_Main_Precache
=================
*/
void UI_Main_Precache( void )
{
	EngFuncs::PIC_Load( ART_MINIMIZE_N );
	EngFuncs::PIC_Load( ART_MINIMIZE_F );
	EngFuncs::PIC_Load( ART_MINIMIZE_D );
	EngFuncs::PIC_Load( ART_CLOSEBTN_N );
	EngFuncs::PIC_Load( ART_CLOSEBTN_F );
	EngFuncs::PIC_Load( ART_CLOSEBTN_D );
	EngFuncs::PIC_Load(ART_DISCORD);
	// precache .avi file and get logo width and height
	EngFuncs::PrecacheLogo( "technocorp.avi" );
}

/*
=================
UI_Main_Menu
=================
*/
void UI_Main_Menu( void )
{
	uiMain.Show();
}
ADD_MENU( menu_main, UI_Main_Precache, UI_Main_Menu );
