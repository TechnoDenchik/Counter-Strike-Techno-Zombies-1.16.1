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
#include "keydefs.h"
#include "Bitmap.h"
#include "PicButton.h"
#include "SpinControl.h"
#include "Action.h"
#include "CheckBox.h"

#define ART_BANNER			"gfx/shell/head_advoptions"

class CMenuGameOptions : public CMenuFramework
{
public:
	CMenuGameOptions() : CMenuFramework("CMenuGameOptions") { }

	const char *Key(int key, int down) override;
	void SetNetworkMode( int maxpacket, int maxpayload, int cmdrate, int updaterate, int rate );
private:
	void _Init() override;
	void SaveCb( );
	void RestoreCb( );
	void Restore();
	void GetConfig();

	CMenuCheckBox allowDownload;
	CMenuCheckBox cl_predict;
	CMenuCheckBox cl_lw;
	CMenuCheckBox normal, dsl, slowest;
	CMenuCheckBox split, compress;

	CMenuPicButton allowConsole;
	CMenuPicButton Apply, Apply1;
	CMenuPicButton Exit, Exit1;

	CMenuSpinControl maxpacket, maxpayload, cmdrate, updaterate, rate;

	CMenuAction networkMode;
};

static CMenuGameOptions	uiGameOptions;

/*
=================
UI_GameOptions_KeyFunc
=================
*/
const char *CMenuGameOptions::Key( int key, int down )
{
	if( down && UI::Key::IsEscape( key ) )
		Restore();
	return CMenuFramework::Key( key, down );
}

void CMenuGameOptions::SetNetworkMode( int maxpacket1, int maxpayload1, int cmdrate1, int updaterate1, int rate1 )
{
	split.bChecked = true;
	compress.bChecked = false;
	normal.bChecked = dsl.bChecked = slowest.bChecked = false;
	maxpacket.SetCurrentValue( maxpacket1 );
	maxpayload.SetCurrentValue( maxpayload1 );
	if( !maxpayload1 )
	maxpayload.ForceDisplayString( "auto" );
	cmdrate.SetCurrentValue( cmdrate1 );
	updaterate.SetCurrentValue( updaterate1 );
	rate.SetCurrentValue( rate1 );
}

void CMenuGameOptions::SaveCb()
{
	allowDownload.WriteCvar();
	maxpacket.WriteCvar();
	maxpayload.WriteCvar();
	cmdrate.WriteCvar();
	updaterate.WriteCvar();
	rate.WriteCvar();
	split.WriteCvar();
	compress.WriteCvar();
	cl_lw.WriteCvar();

	SaveAndPopMenu();
}

void CMenuGameOptions::Restore()
{
	allowDownload.DiscardChanges();
	maxpacket.DiscardChanges();
	maxpayload.DiscardChanges();
	cmdrate.DiscardChanges();
	updaterate.DiscardChanges();
	rate.DiscardChanges();
	split.DiscardChanges();
	compress.DiscardChanges();
	cl_lw.DiscardChanges();
}

void CMenuGameOptions::RestoreCb()
{
	Restore();
	Hide();
}

/*
=================
UI_GameOptions_Init
=================
*/
void CMenuGameOptions::_Init( void )
{
	banner.SetPicture( ART_BANNER );

	allowDownload.SetNameAndStatus(L("Allow download"), "Allow download of files from servers" );
	allowDownload.SetCharSize(QM_BOLDFONT);
	allowDownload.LinkCvar( "sv_allow_download" );
	allowDownload.SetCoord(320, 315 );

	cl_lw.SetNameAndStatus( "Local weapons", "Enable local weapons" );
	cl_lw.SetCharSize(QM_BOLDFONT);
	cl_lw.LinkCvar( "cl_lw" );
	cl_lw.SetCoord(320, 415 );

	maxpacket.SetRect( 650, 270, 200, 32 );
	maxpacket.Setup( 150, 1550, 50 );
	maxpacket.LinkCvar( "cl_maxpacket", CMenuEditable::CVAR_VALUE );
	maxpacket.SetNameAndStatus( "Network packet size limit (cl_maxpacket)", "Split packet size and minimum size to compress");
	maxpacket.SetCharSize(QM_BOLDFONT);
	SET_EVENT_MULTI( maxpacket.onChanged,
	{
		CMenuSpinControl *self = (CMenuSpinControl *)pSelf;
		if( self->GetCurrentValue() == 1550 || self->GetCurrentValue() == 150 )
		{
			self->SetCurrentValue( 40000 );
			self->ForceDisplayString( "auto" );
		}
		else if( self->GetCurrentValue() > 1550 )
		{
			self->SetCurrentValue( 1500 );
		}
	});
	if( maxpacket.GetCurrentValue() == 40000 )
		maxpacket.ForceDisplayString( "auto" );

	maxpayload.SetRect( 650, 370, 200, 32 );
	maxpayload.Setup( 150, 1550, 50 );
	maxpayload.LinkCvar( "cl_maxpayload", CMenuEditable::CVAR_VALUE );
	maxpayload.SetNameAndStatus( "Singon size (cl_maxpayload)", "Singon cnain split decrease if cl_maxpacket does not help");
	maxpayload.SetCharSize(QM_BOLDFONT);
	SET_EVENT_MULTI( maxpayload.onChanged,
	{
		CMenuSpinControl *self = (CMenuSpinControl *)pSelf;
		if( self->GetCurrentValue() == 250 || self->GetCurrentValue() == 40050 )
		{
			self->SetCurrentValue( 0.0f );
			self->ForceDisplayString( "auto" );
		}
		else if( self->GetCurrentValue() > 0 && self->GetCurrentValue() < 250 )
		{
			self->SetCurrentValue( 300 );
		}
	});
	if( maxpayload.GetCurrentValue() == 0 )
		maxpayload.ForceDisplayString( "auto" );

	cmdrate.SetRect( 650, 470, 200, 32 );
	cmdrate.Setup( 20, 60, 5 );
	cmdrate.LinkCvar( "cl_cmdrate", CMenuEditable::CVAR_VALUE );
	cmdrate.SetNameAndStatus( "Command rate (cl_cmdrate)", "How many commands sent to server in second");
	cmdrate.SetCharSize(QM_BOLDFONT);

	updaterate.SetRect( 650, 570, 200, 32 );
	updaterate.Setup( 20, 100, 5 );
	updaterate.LinkCvar( "cl_updaterate", CMenuEditable::CVAR_VALUE );
	updaterate.SetNameAndStatus( "Update rate (cl_updaterate)", "How many uodates sent from server per second");
	updaterate.SetCharSize(QM_BOLDFONT);

	rate.SetRect( 650, 670, 200, 32 );
	rate.Setup( 2500, 90000, 500 );
	rate.LinkCvar( "rate", CMenuEditable::CVAR_VALUE );
	rate.SetNameAndStatus( "Network speed (rate)", "Limit traffic (bytes per second)");
	rate.SetCharSize(QM_BOLDFONT);

	networkMode.iFlags = QMF_INACTIVE|QMF_DROPSHADOW;
	networkMode.szName = L("CstzUI_SetConnection");
	networkMode.colorBase = uiColorHelp;
	networkMode.SetCharSize( QM_BOLDFONT );
	networkMode.SetRect(320, 450, 400, 32 );

	normal.SetRect(320, 510, 24, 24 );
	normal.szName = "Normal internet connection";
	normal.SetCharSize(QM_BOLDFONT);
	SET_EVENT_MULTI( normal.onChanged,
	{
		uiGameOptions.SetNetworkMode( 1400, 0, 30, 60, 25000 );
		((CMenuCheckBox*)pSelf)->bChecked = true;
	});

	dsl.SetRect(320, 560, 24, 24 );
	dsl.szName = "DSL or PPTP with limited packet size";
	dsl.SetCharSize(QM_BOLDFONT);
	SET_EVENT_MULTI( dsl.onChanged,
	{
		uiGameOptions.SetNetworkMode( 1200, 1000, 30, 60, 25000 );
		((CMenuCheckBox*)pSelf)->bChecked = true;
	});

	slowest.SetRect(320, 610, 24, 24 );
	slowest.szName = "Slow connection mode (64kbps)";
	slowest.SetCharSize(QM_BOLDFONT);
	SET_EVENT_MULTI( slowest.onChanged,
	{
		uiGameOptions.SetNetworkMode( 900, 700, 25, 30, 7500 );
		((CMenuCheckBox*)pSelf)->bChecked = true;
	});
	compress.SetNameAndStatus( "Compress", "Compress splitted packets (need split to work)" );
	compress.SetCharSize(QM_BOLDFONT);
	compress.LinkCvar("cl_enable_splitcompress" );
	compress.SetCoord( 440, 680 );

	split.SetCoord( 320, 680 );
	split.SetNameAndStatus( "Split", "Split network packets" );
	split.SetCharSize(QM_BOLDFONT);
	split.LinkCvar("cl_enable_split" );

	Apply.SetNameAndStatus(L("GameUI_Apply"), L(""));
	Apply.SetCharSize(QM_BOLDFONT);
	Apply.onActivated = VoidCb(&CMenuGameOptions::SaveCb);
	Apply.iFlags |= QMF_NOTIFY;
	Apply.SetCoord(80, 250);

	Exit.SetNameAndStatus(L("GameUI_Cancel"), L(""));
	Exit.SetCharSize(QM_BOLDFONT);
	Exit.onActivated = VoidCb(&CMenuGameOptions::RestoreCb);
	Exit.iFlags |= QMF_NOTIFY;
	Exit.SetCoord(80, 300);

#ifdef NEW_ENGINE_INTERFACE
	if( EngFuncs::GetCvarFloat( "developer" ) < 1 )
#else
	if( EngFuncs::GetCvarFloat( "developer" ) < 3 )
#endif
	{
		maxpacket.Hide();
		rate.Hide();
	}

#ifdef NEW_ENGINE_INTERFACE
	if( EngFuncs::GetCvarFloat( "developer" ) < 2 )
#else
	if( EngFuncs::GetCvarFloat( "developer" ) < 4 )
#endif
	{
		maxpayload.Hide();
		cmdrate.Hide();
		updaterate.Hide();
		rate.SetCoord( 650, 370 );
	}

	AddItem( background );
	AddItem( banner );
	AddItem( allowDownload );
	AddItem( cl_lw );
	AddItem( maxpacket );
	AddItem( maxpayload );
	AddItem( cmdrate );
	AddItem( updaterate );
	AddItem( rate );
	AddItem( networkMode );
	AddItem( normal );
	AddItem( dsl );
	AddItem( slowest );
	AddItem( split );
	AddItem( compress );
	AddItem( allowConsole);
	AddItem( Apply );
	AddItem( Exit );
}

/*
=================
UI_GameOptions_Precache
=================
*/
void UI_GameOptions_Precache( void )
{
	EngFuncs::PIC_Load( ART_BANNER );
}

/*
=================
UI_GameOptions_Menu
=================
*/
void UI_GameOptions_Menu( void )
{
	uiGameOptions.Show();
}
ADD_MENU( menu_gameoptions, UI_GameOptions_Precache, UI_GameOptions_Menu );