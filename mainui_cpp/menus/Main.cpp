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
#include "SpinControl.h"

#define ART_MINIMIZE_N	"gfx/shell/min_n"
#define ART_MINIMIZE_F	"gfx/shell/min_f"
#define ART_MINIMIZE_D	"gfx/shell/min_d"
#define ART_CLOSEBTN_N	"gfx/shell/cls_n"
#define ART_CLOSEBTN_F	"gfx/shell/cls_f"
#define ART_CLOSEBTN_D	"gfx/shell/cls_d"

#define ART_DISCORD		"gfx/shell/icon_discord"
#define ART_CONSOLE		"gfx/shell/icon_console"
#define ART_PLAY		"gfx/shell/icon_play"
#define ART_RESUNEGAME	"gfx/shell/icon_returnright"
#define ART_DISCONNECT	"gfx/shell/icon_resune"
#define ART_MULTIPLAYER	"gfx/shell/icon_multi"
#define ART_SETTINGS	"gfx/shell/icon_settings"
#define ART_PREVIEW		"gfx/shell/icon_play"
#define ART_QUIT		"gfx/shell/icon_quit"

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

	class CMenuVidPreview : public CMenuBitmap {} 
	discord,
	iconplay,
	iconresune,
	icondiscon,
	iconconsole,
	iconsettings,
	iconmulti,
	iconpreview,
	iconquit;

	CMenuPicButton	resumeGame;
	CMenuPicButton	disconnect;
	CMenuPicButton	createGame;
	CMenuPicButton	configuration;
	CMenuPicButton	multiPlayer;
	CMenuPicButton	previews;
	CMenuPicButton	quit;
	CMenuSpinControl cvar;
	CMenuYesNoMessageBox dialog;

	int		outlineWidth;
	bool bTrainMap;
	bool bCustomGame;
};

static CMenuMain uiMain;

void CMenuMain::CMenuMainBanner::Draw()
{
	//if( !uiMain.background.ShouldDrawLogoMovie() )
	//	return; // no logos for steam background

	float	logoWidth, logoHeight, logoPosY;
	float	scaleX, scaleY;

	scaleX = ScreenWidth / 1920.0f;
	scaleY = ScreenHeight / 1080.0f;

	// a1ba: multiply by height scale to look better on widescreens
	logoWidth = EngFuncs::GetLogoWidth() * scaleX;
	logoHeight = EngFuncs::GetLogoHeight() * scaleY * uiStatic.scaleY;
	logoPosY = 70 * scaleY * uiStatic.scaleY;	// 70 it's empirically determined value (magic number)

	EngFuncs::DrawLogo( "technocorp.avi", 0, logoPosY, scaleX, scaleY);
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
			int musicset = (int)EngFuncs::GetCvarFloat("menu_musicpack");

			if (!CL_IsActive())
			{
				if (musicset == 0)
				{
					EngFuncs::PlayBackgroundTrack("Music/valve_01/mainmenu", "Music/valve_01/mainmenu");
				}
				else if (musicset == 1)
				{
					EngFuncs::PlayBackgroundTrack("Music/valve_cs2_01/mainmenu", "Music/valve_cs2_01/mainmenu");
				}
				else if (musicset == 2)
				{
					EngFuncs::PlayBackgroundTrack("Music/radcat_01/mainmenu", "Music/radcat_01/mainmenu");
				}
				else if (musicset == 3)
				{
					EngFuncs::PlayBackgroundTrack("Music/3kliksphilip_01/mainmenu", "Music/3kliksphilip_01/mainmenu");
				}
				else if (musicset == 4)
				{
					EngFuncs::PlayBackgroundTrack("Music/bbnos_01/mainmenu", "Music/bbnos_01/mainmenu");
				}
				else if (musicset == 5)
				{
					EngFuncs::PlayBackgroundTrack("Music/chipzel_01/mainmenu", "Music/chipzel_01/mainmenu");
				}
				else if (musicset == 6)
				{
					EngFuncs::PlayBackgroundTrack("Music/dryden_01/mainmenu", "Music/dryden_01/mainmenu");
				}
				else if (musicset == 7)
				{
					EngFuncs::PlayBackgroundTrack("Music/freakydna_01/mainmenu", "Music/freakydna_01/mainmenu");
				}
				else if (musicset == 8)
				{
					EngFuncs::PlayBackgroundTrack("Music/isoxo_01/mainmenu", "Music/isoxo_01/mainmenu");
				}
				else if (musicset == 9)
				{
					EngFuncs::PlayBackgroundTrack("Music/knock2_01/mainmenu", "Music/knock2_01/mainmenu");
				}
				else if (musicset == 10)
				{
					EngFuncs::PlayBackgroundTrack("Music/mattlevine_01/mainmenu", "Music/mattlevine_01/mainmenu");
				}
				else if (musicset == 11)
				{
					EngFuncs::PlayBackgroundTrack("Music/meechydarko_01/mainmenu", "Music/meechydarko_01/mainmenu");
				}
				else if (musicset == 12)
				{
					EngFuncs::PlayBackgroundTrack("Music/mordfustang_01/mainmenu", "Music/mordfustang_01/mainmenu");
				}
			}
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
		icondiscon.Show();
		iconresune.Show();
	}
	else
	{
		resumeGame.Hide();
		disconnect.Hide();
		icondiscon.Hide();
		iconresune.Hide();
	}

	if( gpGlobals->developer )
	{
		console.pos.y = CL_IsActive() ? 130 : 230;
		console.pos.y += 50;
	}

	int consoleset = (int)EngFuncs::GetCvarFloat("menu_getconsole");
	if (consoleset == 1)
	{
		gpGlobals->developer = 1;
		console.Show();
		iconconsole.Show();
	}
	else
	{
		gpGlobals->developer = 0;
		console.Hide();
		iconconsole.Hide();
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
	EngFuncs::CvarSetValue( "pausable", 1.0f );
	EngFuncs::CvarSetValue( "coop", 0.0f );
	EngFuncs::CvarSetValue( "maxplayers", 1.0f );

	EngFuncs::PlayBackgroundTrack( NULL, NULL );

	EngFuncs::ClientCmd( FALSE, "hazardcourse\n" );
}

void CMenuMain::_Init( void )
{
	
	bTrainMap = false;
	bCustomGame = false;

	iconconsole.iFlags = QMF_NOTIFY;
	iconconsole.SetRect(7, 400, 32, 32);
	iconconsole.SetPicture(ART_CONSOLE);
	iconconsole.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

	console.SetNameAndStatus(L("GameUI_Console"), L(""));
	console.onActivated = UI_CloseMenu;
	console.iFlags |= QMF_NOTIFY;
	SET_EVENT_MULTI( console.onActivated,
	{
		UI_SetActiveMenu( FALSE );
		EngFuncs::KEY_SetDest( KEY_CONSOLE );
	});

	

	discord.iFlags = QMF_NOTIFY;
	discord.SetPicture(ART_DISCORD);
	discord.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	discord.SetRect(32, 40, 32, 32);
	SET_EVENT(discord.onActivated, EngFuncs::ShellExecute("https://discord.gg/U9sdYbZrRU", NULL, false));

	iconresune.iFlags = QMF_NOTIFY;
	iconresune.SetRect(7, 440, 32, 32);
	iconresune.SetPicture(ART_RESUNEGAME);
	iconresune.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	iconresune.onActivated = UI_CloseMenu;

	resumeGame.SetNameAndStatus(L("GameUI_GameMenu_ResumeGame"), L(""));
	resumeGame.onActivated = UI_CloseMenu;
	resumeGame.iFlags |= QMF_NOTIFY;

	icondiscon.iFlags = QMF_NOTIFY;
	icondiscon.SetRect(7, 480, 32, 32);
	icondiscon.SetPicture(ART_DISCONNECT);
	icondiscon.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	icondiscon.onActivated = VoidCb(&CMenuMain::DisconnectDialogCb);

	disconnect.SetNameAndStatus(L("GameUI_GameMenu_Disconnect"), L(""));
	disconnect.onActivated = VoidCb(&CMenuMain::DisconnectDialogCb);
	disconnect.iFlags |= QMF_NOTIFY;

	iconplay.iFlags = QMF_NOTIFY;
	iconplay.SetRect(7, 520, 32, 32);
	iconplay.SetPicture(ART_PLAY);
	iconplay.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	iconplay.onActivated = UI_CreateGame_Menu;

	createGame.SetNameAndStatus(LL("GameUI_GameMenu_CreateServer"), L(""));
	createGame.onActivated = UI_CreateGame_Menu;
	createGame.iFlags |= QMF_NOTIFY;

	iconmulti.iFlags = QMF_NOTIFY;
	iconmulti.SetRect(7, 560, 32, 32);
	iconmulti.SetPicture(ART_MULTIPLAYER);
	iconmulti.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	iconmulti.onActivated = UI_InternetGames_Menu;

	multiPlayer.SetNameAndStatus(L("GameUI_GameMenu_FindServers"), L(""));
	multiPlayer.onActivated = UI_InternetGames_Menu;
	multiPlayer.iFlags |= QMF_NOTIFY;

	iconsettings.iFlags = QMF_NOTIFY;
	iconsettings.SetRect(7, 640, 32, 32);
	iconsettings.SetPicture(ART_SETTINGS);
	iconsettings.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	iconsettings.onActivated = UI_Options_Menu;

	configuration.SetNameAndStatus(L("GameUI_GameMenu_Options"), L(""));
	configuration.onActivated = UI_Options_Menu;
	configuration.iFlags |= QMF_NOTIFY;

	iconpreview.iFlags = QMF_NOTIFY;
	iconpreview.SetRect(7, 600, 32, 32);
	iconpreview.SetPicture(ART_PREVIEW);
	iconpreview.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	iconpreview.onActivated = UI_Options_Menu;
	SET_EVENT(iconpreview.onActivated, EngFuncs::ShellExecute("https://github.com/TechnoDenchik/Counter-Strike-Techno-Zombies-1.16.1/tree/cstz1161", NULL, false));

	previews.SetNameAndStatus(L("GameUI_Previews"), L(""));
	previews.onActivated = UI_Options_Menu;
	previews.iFlags |= QMF_NOTIFY;
	SET_EVENT(previews.onActivated, EngFuncs::ShellExecute("https://github.com/TechnoDenchik/Counter-Strike-Techno-Zombies-1.16.1/tree/cstz1161", NULL, false));

	iconquit.iFlags = QMF_NOTIFY;
	iconquit.SetRect(10, 685, 26, 26);
	iconquit.SetPicture(ART_QUIT);
	iconquit.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	iconquit.onActivated = MenuCb(&CMenuMain::QuitDialog);

	quit.SetNameAndStatus(L("GameUI_GameMenu_Quit"), L(""));
	quit.onActivated = MenuCb(&CMenuMain::QuitDialog);
	quit.iFlags |= QMF_NOTIFY;

	if ( gMenu.m_gameinfo.gamemode == GAME_SINGLEPLAYER_ONLY )
		multiPlayer.SetGrayed( true );

	dialog.Link( this );

	AddItem( background );
	AddItem( banner );

	
	AddItem( console );
	AddItem( iconconsole );
	
	

	AddItem( disconnect );
	AddItem( iconplay );
	AddItem( iconresune );
	AddItem( icondiscon );
	AddItem(iconmulti);
	AddItem(iconsettings);
	AddItem(iconpreview);
	AddItem(iconquit);
	
	
	AddItem( discord );
	AddItem( resumeGame );
	AddItem( createGame );
	AddItem( configuration );
	AddItem( multiPlayer );
	AddItem( previews );
	AddItem( quit );
}

/*
=================
UI_Main_Init
=================
*/
void CMenuMain::_VidInit( void )
{
	Activate();
	cvar.LinkCvar("menu_getconsole", CMenuEditable::CVAR_VALUE);
	console.SetCoord(42, 400);
	resumeGame.SetCoord( 42, 440);
	disconnect.SetCoord( 42, 480);
	createGame.SetCoord( 42, 520 );
	multiPlayer.SetCoord( 42, 560 );
	previews.SetCoord( 42,  600);
	configuration.SetCoord( 42, 640 );

	quit.SetCoord( 42, 680 );

	outlineWidth = 2;
	UI_ScaleCoords(NULL, NULL, &outlineWidth, NULL);
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
	EngFuncs::PIC_Load( ART_DISCORD );
	EngFuncs::PIC_Load(ART_PLAY);
	EngFuncs::PrecacheLogo( "technocorp.avi" );

	//EngFuncs::PrecacheGeneric("Music/valve_01/mainmenu.mp3");
	//EngFuncs::PrecacheGeneric("Music/valve_cs2_01/mainmenu.mp3");
	//EngFuncs::PrecacheGeneric("Music/radcat_01/mainmenu.mp3");
	//EngFuncs::PrecacheGeneric("Music/3kliksphilip_01/mainmenu.mp3");
	//EngFuncs::PrecacheGeneric("Music/bbnos_01/mainmenu.mp3");
	//EngFuncs::PrecacheGeneric("Music/chipzel_01/mainmenu.mp3");
	//EngFuncs::PrecacheGeneric("Music/dryden_01/mainmenu.mp3");
	//EngFuncs::PrecacheGeneric("Music/freakydna_01/mainmenu.mp3");
	//EngFuncs::PrecacheGeneric("Music/isoxo_01/mainmenu.mp3");
	//EngFuncs::PrecacheGeneric("Music/knock2_01/mainmenu.mp3");
	//EngFuncs::PrecacheGeneric("Music/mattlevine_01/mainmenu.mp3");
	//EngFuncs::PrecacheGeneric("Music/meechydarko_01/mainmenu.mp3");
	//EngFuncs::PrecacheGeneric("Music/mordfustang_01/mainmenu.mp3");
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
