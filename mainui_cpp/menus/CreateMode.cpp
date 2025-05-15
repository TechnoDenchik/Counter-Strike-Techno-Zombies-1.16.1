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
#include "Field.h"
#include "CheckBox.h"
#include "PicButton.h"
#include "Table.h"
#include "Action.h"
#include "YesNoMessageBox.h"
#include "MessageBox.h"
#include "Table.h"
#include "SpinControl.h"
#include "StringArrayModel.h"

#define ART_BANNER		"gfx/shell/head_creategame"
#define ART_PLAY		"gfx/shell/playbutton"
#define ART_UPARROW		"gfx/shell/uparrowp"
#define ART_DOWNARROW	"gfx/shell/sm_dnarw"

/*=================== Gamemodes Map ===================*/

#define ART_ASSAULT		"gfx/maps/cs_assault_cso"
#define ART_ITALY		"gfx/maps/cs_italy_cso"
#define ART_VERTIGO		"gfx/maps/de_vertigo_cso"
#define ART_INFERNO		"gfx/maps/de_inferno_cso"

#define ART_NUKE		"gfx/maps/de_nuke_cso"
#define ART_DUST		"gfx/maps/de_dust2_cso"
#define ART_MIRAGE		"gfx/maps/de_mirage_cso"
#define ART_GREESIA		"gfx/maps/awp_greesia2_cso"

#define ART_RUIN		"gfx/maps/awp_ruin2_cso"
#define ART_BIGTREE		"gfx/maps/bzm_bigtree_cso"
#define ART_DUSTMINI	"gfx/maps/bzm_dust_mini_cso"
#define ART_747			"gfx/maps/cs_747_cso"

#define ART_ESTATE		"gfx/maps/cs_estate_cso"
#define ART_HAVANA		"gfx/maps/cs_havana_cso"
#define ART_MILITIA		"gfx/maps/cs_militia_cso"
#define ART_OFFICE		"gfx/maps/cs_office_cso"

#define ART_SIEGE		"gfx/maps/cs_siege_cso"
#define ART_ANGELCITY	"gfx/maps/de_angelcity_cso"
#define ART_AZTEC		"gfx/maps/de_aztec_cso"
#define ART_CBBLE		"gfx/maps/de_cbble_cso"

#define ART_CHATEAU		"gfx/maps/de_chateau_cso"
#define ART_PRODIGY		"gfx/maps/de_prodigy_cso"
#define ART_RATS		"gfx/maps/de_rats_cso"
#define ART_SANTORINI	"gfx/maps/de_santorini_cso"

#define ART_SKYSCRAPER	"gfx/maps/dm_skyscraper_cso"
#define ART_TORN		"gfx/maps/de_torn_cso"
#define ART_DARKSNOW	"gfx/maps/dm_darksnow_cso"
#define ART_GALERY		"gfx/maps/dm_gallery_cso"

#define ART_INDUSTRY	"gfx/maps/dm_industry_cso"
#define ART_INDUSTRY2	"gfx/maps/dm_industry2_cso"
#define ART_MOONLIGHT	"gfx/maps/dm_moonlight2_cso"
#define ART_PORT		"gfx/maps/dm_port_cso"

#define ART_ABYSS		"gfx/maps/zm_abyss_cso"
#define ART_ABYSS2		"gfx/maps/zm_abyss2_cso"
#define ART_ABYSS3		"gfx/maps/zm_abyss3_cso"

#define ART_ORIGIN		"gfx/maps/zm_origin_cso"
#define ART_NIGHTMARE	"gfx/maps/zs_nightmare_cso"

#define ART_NIGHTMARE2	"gfx/maps/zs_nightmare2_cso"
#define ART_NIGHTMARE3	"gfx/maps/zs_nightmare3_cso"
#define ART_LOSTCITY	"gfx/maps/zs_lostcity_cso"
#define ART_LASTCLUE	"gfx/maps/zs_lastclue_cso"
#define ART_PANIC	"gfx/maps/zs_panic_cso"
#define ART_TRAP	"gfx/maps/zs_trap_cso"

#define ART_SNAILCITY	"gfx/maps/awp_greesia2_cso"

/*=================== End Gamemodes Map ===================*/

class CMenuMapListModel : public CMenuBaseModel
{
public:
	void Update() override;
	int GetColumns() const override { return 2; }
	int GetRows() const override { return m_iNumItems; }
	const char *GetCellText( int line, int column ) override
	{
		switch( column )
		{
		case 0: return mapName[line];
		case 1: return mapsDescription[line];
		}

		return NULL;
	}

	char		mapName[UI_MAXGAMES][64];
	char		mapsDescription[UI_MAXGAMES][64];
	int	m_iNumItems;
};

#define MAX_GAMEMODES 8
static const char* g_szGameModeCodes[MAX_GAMEMODES] = {"none", "dm", "tdm", "gd", "zb1", "zb3", "zbs", "zsh_pve"};

class CMenuCreateGame : public CMenuFramework
{
public:
	CMenuCreateGame() : CMenuFramework("CMenuCreateGame"), nomap(true), nomode(true) { }
	void Begin();

	void Reload(void) override;
	void ResetMap(int value);
	void ResetMode(const char* value);

	char* mapsDescriptionPtr[UI_MAXGAMES];

	bool ischecksetmap;
	bool ischecksetmode;

	bool modenone;
	bool modedm;
	bool modetdm;
	bool modegd;
	bool modezb1;
	bool modezb3;
	bool modezbs;
	bool modezsh;

	int stringmaplist;
	int stringmaplist2;
	int stringmaplist3;

	CMenuField	maxClients;
	CMenuField	hostName;
	CMenuField	password;
	CMenuField  botNum;
	CMenuCheckBox   nat;
	CMenuCheckBox	hltv;
	
/*=================== Gamemodes ===================*/
	/*
	===================
	 GameMode: Classic
	===================
	*/
		CMenuCheckBox	classic;
	/*
	=======================
	 GameMode: Death Match
	=======================
	*/
		CMenuCheckBox	dm;
	/*
	============================
	 GameMode: Team Death Match
	============================
	*/
		CMenuCheckBox	tdm;
	/*
	===========================
	 GameMode: Gun Death Match
	===========================
	*/
		CMenuCheckBox	gd;
	/*
	==========================
	 GameMode: Zombie Classic
	==========================
	*/
		CMenuCheckBox	zc;
	/*
	=======================
	 GameMode: Zombie Hero
	=======================
	*/
		CMenuCheckBox	zh;
	/*
	===========================
	 GameMode: Scenario Zombie
	===========================
	*/
		CMenuCheckBox	sz;
	/*
	================================
	 GameMode: Zombie Shelter Co-Op
	================================
	*/
		CMenuCheckBox	zsh;

/*=================== End Gamemodes ===================*/

class CMenuVidPreview : public 

	CMenuBitmap {} 
			uparrow, downarrow, playbutton, mapassault, mapitaly, mapvertigo, mapinferno, mapnuke, mapdust, mapmirage,
					mapgressia, mapruin,mapbigtree, mapdustmini, mapcs747, mapestate, maphavana, mapmilitia,
				 mapoffice, mapsiege, mapangelcity, mapaztec, mapcbble, mapchateau, mapprodigy, maprats, mapsantorini,
					 mapskyscraper, maptorn, mapdarksnow, mapgalery, mapindustry, mapindustry2, mapmoonlight, mapport, mapabyss,
						 mapabyss2, mapabyss3, maporigin, mapnightmare, mapnightmare2, mapnightmare3, maplastclue, mappanic, maplostcity, maptrap, mapsnailcity;
		
	
	CMenuCheckBox	
					assault, italy, vertigo, inferno, nuke, dust2, mirage,
				gressia, ruin, bigtree, dustmini, cs747, estate, havana,
			militia, office, siege, angelcity, aztec, cbble, chateau,
		prodigy, rats, santorini, skyscraper, torn, darksnow, galery,
			industry, industry2, moonlight, port, abyss, abyss2, abyss3,
				origin, nightmare, nightmare2, nightmare3, lastclue, panic, lostcity, trap, snailcity;

	CMapSet
				MapSetAssault, MapSetItaly, MapSetVertigo, MapSetInferno,MapSetNuke,MapSetDust,MapSetMirage,
			MapSetGressia, MapSetRuin, MapSetBigTree, MapSetDustMini, MapSet747, MapSetEstate, MapSetHavana, 
		MapSetMilitia, MapSetOffice, MapSetSiege, MapSetAngelCity, MapSetAztec, MapSetCbble, MapSetChateau, 
			MapSetProdigy, MapSetRats, MapSetSantorini, MapSetSkyScraper, MapSetTorn, MapSetDarkSnow, MapSetGalery, 
				MapSetIndustry, MapSetIndustry2, MapSetMoonLight, MapSetPort, MapSetAbyss, MapSetAbyss2, MapSetAbyss3, 
					MapSetOrigin, MapSetNightMare, MapSetNightMare2, MapSetNightMare3, MapSetLastClue, MapSetPanic, MapSetLostCity, MapSetTrap, MapSetSnailCity;

	CMenuYesNoMessageBox msgBox;
	CMenuYesNoMessageBox nomap;
	CMenuYesNoMessageBox nomode;

	CMenuMessageBox message;

	CMenuTable        mapsList;
	CMenuMapListModel mapsListModel;

	CMenuPicButton Inventory;
	CMenuPicButton* done;
	CMenuPicButton 
		Adv, 
		textmap, 
		textmap2, 
		textmap3, 
		textmap4, 
		textmap5, 
		textmap6, 
		textmap7, 
		textmap8,
		textmap9,
		textmap10,
		textmap11,
		textmap12,
		textmap13,
		textmap14,
		textmap15,
		textmap16,
		textmap17,
		textmap18,
		textmap19,
		textmap20,
		textmap21,
		textmap22,
		textmap23,
		textmap24,
		textmap25,
		textmap26,
		textmap27,
		textmap28,
		textmap29, 
		textmap30, 
		textmap31, 
		textmap32, 
		textmap33, 
		textmap34;
	CMenuPicButton Exit, Exit1;
private:
	void _Init() override;
	void _VidInit() override;
};

static CMenuCreateGame	uiCreateGame;

/*
=================
CMenuCreateGame::Begin
=================
*/
void CMenuCreateGame::Begin( )
{
	if (ischecksetmode == true)
	{
		if (ischecksetmap == true)
		{
			if (uiStatic.enterSound > 0.0f && uiStatic.enterSound <= gpGlobals->time)
			{
				EngFuncs::PlayLocalSound(uiStartGame);
				uiStatic.enterSound = -1;
			}

			if (EngFuncs::GetCvarFloat("host_serverstate"))
			{
				if (EngFuncs::GetCvarFloat("maxplayers") == 1.0f)
					EngFuncs::HostEndGame("end of the game");
				else
					EngFuncs::HostEndGame("starting new server");
			}

			EngFuncs::CvarSetValue("deathmatch", 1.0f);	// start deathmatch as default
			//EngFuncs::CvarSetValue("public", 1.0f);
			EngFuncs::CvarSetValue("cl_nat", 1);


			password.WriteCvar();
			hostName.WriteCvar();
			hltv.WriteCvar();
			maxClients.WriteCvar();
			botNum.WriteCvar();

			EngFuncs::PlayBackgroundTrack(NULL, NULL);
			EngFuncs::WriteServerConfig(EngFuncs::GetCvarString("lservercfgfile"));

			char cmd[128];
			sprintf(cmd, "exec %s\n", EngFuncs::GetCvarString("lservercfgfile"));

			EngFuncs::ClientCmd(TRUE, cmd);
			EngFuncs::CvarSetValue("maxplayers", atoi(maxClients.GetBuffer()));

			if (assault.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map cs_assault", atoi(maxClients.GetBuffer()));
			else if (italy.bChecked == true)	
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map cs_italy", atoi(maxClients.GetBuffer()));		
			else if (vertigo.bChecked == true)	
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map de_vertigo", atoi(maxClients.GetBuffer()));			
			else if (inferno.bChecked == true)	
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map de_inferno", atoi(maxClients.GetBuffer()));			
			else if (nuke.bChecked == true)		
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map de_nuke", atoi(maxClients.GetBuffer()));			
			else if (dust2.bChecked == true)		
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map de_dust2", atoi(maxClients.GetBuffer()));			
			else if (mirage.bChecked == true)	
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map de_mirage", atoi(maxClients.GetBuffer()));	
			else if (gressia.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map awp_greesia", atoi(maxClients.GetBuffer()));
			else if (ruin.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map awp_ruin", atoi(maxClients.GetBuffer()));
			else if (bigtree.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map bzm_bigtree", atoi(maxClients.GetBuffer()));
			else if (dustmini.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map bzm_dust_mini", atoi(maxClients.GetBuffer()));
			else if (cs747.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map cs_747", atoi(maxClients.GetBuffer()));
			else if (estate.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map cs_estate", atoi(maxClients.GetBuffer()));
			else if (havana.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map cs_havana", atoi(maxClients.GetBuffer()));
			else if (militia.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map cs_militia", atoi(maxClients.GetBuffer()));
			else if (office.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map cs_office", atoi(maxClients.GetBuffer()));
			else if (siege.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map cs_siege", atoi(maxClients.GetBuffer()));
			else if (angelcity.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map de_angelcity", atoi(maxClients.GetBuffer()));
			else if (aztec.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map de_aztec", atoi(maxClients.GetBuffer()));
			else if (cbble.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map de_cbble", atoi(maxClients.GetBuffer()));
			else if (chateau.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map de_chateau", atoi(maxClients.GetBuffer()));
			else if (prodigy.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map de_prodigy", atoi(maxClients.GetBuffer()));
			else if (rats.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map de_rats", atoi(maxClients.GetBuffer()));
			else if (santorini.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map de_santorini", atoi(maxClients.GetBuffer()));
			else if (skyscraper.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map de_skyscraper", atoi(maxClients.GetBuffer()));
			else if (torn.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map de_torn", atoi(maxClients.GetBuffer()));
			else if (darksnow.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map dm_darksnow", atoi(maxClients.GetBuffer()));
			else if (galery.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map dm_gallery", atoi(maxClients.GetBuffer()));
			else if (industry.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map dm_industry", atoi(maxClients.GetBuffer()));
			else if (industry2.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map dm_industry2", atoi(maxClients.GetBuffer()));
			else if (moonlight.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map dm_moonlight", atoi(maxClients.GetBuffer()));
			else if (port.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map dm_port", atoi(maxClients.GetBuffer()));
			else if (abyss.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map zm_abyss", atoi(maxClients.GetBuffer()));
			else if (abyss2.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map zm_abyss2", atoi(maxClients.GetBuffer()));
			else if (abyss3.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map zm_abyss3", atoi(maxClients.GetBuffer()));
			else if (origin.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map zm_origin", atoi(maxClients.GetBuffer()));
			else if (nightmare.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map zs_nightmare", atoi(maxClients.GetBuffer()));
			else if (nightmare2.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map zs_nightmare2", atoi(maxClients.GetBuffer()));
			else if (nightmare3.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map zs_nightmare3", atoi(maxClients.GetBuffer()));
			else if (lastclue.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map zs_lastclue", atoi(maxClients.GetBuffer()));
			else if (lostcity.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map zs_lostcity", atoi(maxClients.GetBuffer()));
			else if (panic.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map zs_panic", atoi(maxClients.GetBuffer()));
			else if (trap.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map zs_trap", atoi(maxClients.GetBuffer()));
			else if (snailcity.bChecked == true)
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map zsh_snailcity", atoi(maxClients.GetBuffer()));
			else
				sprintf(cmd, "menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map %s\n", atoi(maxClients.GetBuffer()));		

			EngFuncs::ClientCmd(FALSE, cmd);
		}
		else
		{
			nomap.Show();
		}
	}
	else
	{
		nomode.Show();
	}
}

/*
=================
CMenuMapListModel::Update
=================
*/
void CMenuMapListModel::Update( void )
{
	char *afile;

	if( !uiStatic.needMapListUpdate )
		return;

	if( !EngFuncs::CreateMapsList( TRUE ) || (afile = (char *)EngFuncs::COM_LoadFile( "maps.lst", NULL )) == NULL )
	{
		uiCreateGame.done->SetGrayed( true );
		m_iNumItems = 0;
		Con_Printf( "Cmd_GetMapsList: can't open maps.lst\n" );
		return;
	}

	char *pfile = afile;
	char token[1024];
	int numMaps = 1;

	strcpy( mapName[0], L("CstzUI_GameRandom"));
	mapsDescription[0][0] = 0;
	
	while(( pfile = EngFuncs::COM_ParseFile( pfile, token )) != NULL )
	{
		if( numMaps >= UI_MAXGAMES ) break;

		Q_strncpy( mapName[numMaps], token, 64 );
		if(( pfile = EngFuncs::COM_ParseFile( pfile, token )) == NULL )
		{
			Q_strncpy( mapsDescription[numMaps], mapName[numMaps], 64 );
			break; // unexpected end of file
		}
		Q_strncpy( mapsDescription[numMaps], token, 64 );
		numMaps++;
	}

	if( !( numMaps - 1) ) uiCreateGame.done->SetGrayed( true );
	m_iNumItems = numMaps;
	EngFuncs::COM_FreeFile( afile );
	uiStatic.needMapListUpdate = false;
}

void CMenuCreateGame::ResetMode(const char *value)
{
	if (value == "none")
	{
		classic.LinkCvar("mp_gamemode");
		EngFuncs::CvarSetString("mp_gamemode", value);
		ischecksetmode = true;

		modenone = true;
		modedm = false;
		modetdm = false;
		modegd = false;
		modezb1 = false;
		modezb3 = false;
		modezbs = false;
		modezsh = false;

		classic.bChecked = true;
		dm.bChecked = false;
		tdm.bChecked = false;
		gd.bChecked = false;
		zc.bChecked = false;
		zh.bChecked = false;
		sz.bChecked = false;
		zsh.bChecked = false;

		italy.bChecked = false;
		assault.bChecked = false;
		vertigo.bChecked = false;
		nuke.bChecked = false;
		dust2.bChecked = false;
		inferno.bChecked = false;
		mirage.bChecked = false;
		gressia.bChecked = false;
		ruin.bChecked = false;
		bigtree.bChecked = false;
		dustmini.bChecked = false;
		cs747.bChecked = false;
		estate.bChecked = false;
		havana.bChecked = false;
		militia.bChecked = false;
		office.bChecked = false;
		siege.bChecked = false;
		angelcity.bChecked = false;

		aztec.bChecked = false;
		cbble.bChecked = false;
		chateau.bChecked = false;
		prodigy.bChecked = false;
		rats.bChecked = false;
		santorini.bChecked = false;
		skyscraper.bChecked = false;
		torn.bChecked = false;
		darksnow.bChecked = false;

		MapSetAztec.Hide();
		MapSetCbble.Hide();
		MapSetChateau.Hide();
		MapSetProdigy.Hide();
		MapSetRats.Hide();
		MapSetSantorini.Hide();
		MapSetSkyScraper.Hide();
		MapSetTorn.Hide();
		MapSetDarkSnow.Hide();

		uparrow.Show();
		downarrow.Show();

		mapnightmare.Hide();
		MapSetNightMare.Hide();

		nightmare.Hide();
		textmap28.Hide();

		mapnightmare2.Hide();
		MapSetNightMare2.Hide();
		mapnightmare3.Hide();
		MapSetNightMare3.Hide();
		maplastclue.Hide();
		MapSetLastClue.Hide();
		maplostcity.Hide();
		MapSetLostCity.Hide();
		mappanic.Hide();
		MapSetPanic.Hide();
		maptrap.Hide();
		MapSetTrap.Hide();

		

		nightmare2.Hide();
		textmap29.Hide();

		nightmare3.Hide();
		textmap30.Hide();

		lastclue.Hide();
		textmap31.Hide();

		lostcity.Hide();
		textmap32.Hide();

		panic.Hide();
		textmap33.Hide();

		trap.Hide();
		textmap34.Hide();

		nightmare2.bChecked = false;
		nightmare3.bChecked = false;
		lastclue.bChecked = false;
		lostcity.bChecked = false;
		panic.bChecked = false;
		trap.bChecked = false;

		MapSetBigTree.Hide();
		MapSetDustMini.Hide();
		MapSet747.Hide();
		MapSetEstate.Hide();
		MapSetHavana.Hide();
		MapSetMilitia.Hide();
		MapSetOffice.Hide();
		MapSetSiege.Hide();
		MapSetAngelCity.Hide();
		MapSetAssault.Hide();
		MapSetItaly.Hide();
		MapSetVertigo.Hide();
		MapSetInferno.Hide();
		MapSetNuke.Hide();
		MapSetDust.Hide();
		MapSetMirage.Hide();
		MapSetGressia.Hide();
		MapSetRuin.Hide();

		if (stringmaplist == 1)
		{
			textmap.SetNameAndStatus(L("Assault"), L(""));
			textmap2.SetNameAndStatus(L("Italy"), L(""));
			textmap3.SetNameAndStatus(L("Vertigo"), L(""));

			textmap4.SetNameAndStatus(L("Inferno"), L(""));
			textmap5.SetNameAndStatus(L("Nuke"), L(""));
			textmap6.SetNameAndStatus(L("Dust II"), L(""));

			textmap7.SetNameAndStatus(L("Mirage"), L(""));
			textmap8.SetNameAndStatus(L("Greesia"), L(""));
			textmap9.SetNameAndStatus(L("Ruin"), L(""));

			textmap.Show();
			textmap2.Show();
			textmap3.Show();
			textmap4.Show();
			textmap5.Show();
			textmap6.Show();
			textmap7.Show();
			textmap8.Show();
			textmap9.Show();

			textmap10.Hide();
			textmap11.Hide();
			textmap12.Hide();
			textmap13.Hide();
			textmap14.Hide();
			textmap15.Hide();
			textmap16.Hide();
			textmap17.Hide();
			textmap18.Hide();

			textmap19.Hide();
			textmap20.Hide();
			textmap21.Hide();
			textmap22.Hide();
			textmap23.Hide();
			textmap24.Hide();
			textmap25.Hide();
			textmap26.Hide();
			textmap27.Hide();

			mapassault.Show();
			mapitaly.Show();
			mapvertigo.Show();
			mapinferno.Show();
			mapnuke.Show();
			mapdust.Show();
			mapmirage.Show();
			mapgressia.Show();
			mapruin.Show();

			assault.Show();
			vertigo.Show();
			italy.Show();
			nuke.Show();
			dust2.Show();
			inferno.Show();
			mirage.Show();
			gressia.Show();
			ruin.Show();

			bigtree.Hide();
			dustmini.Hide();
			cs747.Hide();
			estate.Hide();
			havana.Hide();
			militia.Hide();
			office.Hide();
			siege.Hide();
			angelcity.Hide();

			mapbigtree.Hide();
			mapdustmini.Hide();
			mapcs747.Hide();
			mapestate.Hide();
			maphavana.Hide();
			mapmilitia.Hide();
			mapoffice.Hide();
			mapsiege.Hide();
			mapangelcity.Hide();

			aztec.Hide();
			cbble.Hide();
			chateau.Hide();
			prodigy.Hide();
			rats.Hide();
			santorini.Hide();
			skyscraper.Hide();
			torn.Hide();
			darksnow.Hide();

			mapaztec.Hide();
			mapcbble.Hide();
			mapchateau.Hide();
			mapprodigy.Hide();
			maprats.Hide();
			mapsantorini.Hide();
			mapskyscraper.Hide();
			maptorn.Hide();
			mapdarksnow.Hide();
		}
		else if (stringmaplist == 2)
		{
			textmap10.SetNameAndStatus(L("BigTree"), L(""));
			textmap11.SetNameAndStatus(L("Dust EX"), L(""));
			textmap12.SetNameAndStatus(L("747"), L(""));

			textmap13.SetNameAndStatus(L("Estate"), L(""));
			textmap14.SetNameAndStatus(L("Havana"), L(""));
			textmap15.SetNameAndStatus(L("Militia"), L(""));

			textmap16.SetNameAndStatus(L("Office"), L(""));
			textmap17.SetNameAndStatus(L("Siege"), L(""));
			textmap18.SetNameAndStatus(L("Angel City"), L(""));

			textmap.Hide();
			textmap2.Hide();
			textmap3.Hide();
			textmap4.Hide();
			textmap5.Hide();
			textmap6.Hide();
			textmap7.Hide();
			textmap8.Hide();
			textmap9.Hide();

			textmap10.Show();
			textmap11.Show();
			textmap12.Show();
			textmap13.Show();
			textmap14.Show();
			textmap15.Show();
			textmap16.Show();
			textmap17.Show();
			textmap18.Show();

			textmap19.Hide();
			textmap20.Hide();
			textmap21.Hide();
			textmap22.Hide();
			textmap23.Hide();
			textmap24.Hide();
			textmap25.Hide();
			textmap26.Hide();
			textmap27.Hide();

			mapassault.Hide();
			mapitaly.Hide();
			mapvertigo.Hide();
			mapinferno.Hide();
			mapnuke.Hide();
			mapdust.Hide();
			mapmirage.Hide();
			mapgressia.Hide();
			mapruin.Hide();

			assault.Hide();
			vertigo.Hide();
			italy.Hide();
			nuke.Hide();
			dust2.Hide();
			inferno.Hide();
			mirage.Hide();
			gressia.Hide();
			ruin.Hide();

			bigtree.Show();
			dustmini.Show();
			cs747.Show();
			estate.Show();
			havana.Show();
			militia.Show();
			office.Show();
			siege.Show();
			angelcity.Show();

			mapbigtree.Show();
			mapdustmini.Show();
			mapcs747.Show();
			mapestate.Show();
			maphavana.Show();
			mapmilitia.Show();
			mapoffice.Show();
			mapsiege.Show();
			mapangelcity.Show();

			aztec.Hide();
			cbble.Hide();
			chateau.Hide();
			prodigy.Hide();
			rats.Hide();
			santorini.Hide();
			skyscraper.Hide();
			torn.Hide();
			darksnow.Hide();

			mapaztec.Hide();
			mapcbble.Hide();
			mapchateau.Hide();
			mapprodigy.Hide();
			maprats.Hide();
			mapsantorini.Hide();
			mapskyscraper.Hide();
			maptorn.Hide();
			mapdarksnow.Hide();
		}
		else if (stringmaplist == 3)
		{
			textmap19.SetNameAndStatus(L("Aztec"), L(""));
			textmap20.SetNameAndStatus(L("CobbleStone"), L(""));
			textmap21.SetNameAndStatus(L("ChateAu"), L(""));

			textmap22.SetNameAndStatus(L("Prodigy"), L(""));
			textmap23.SetNameAndStatus(L("Rats"), L(""));
			textmap24.SetNameAndStatus(L("Santorini"), L(""));

			textmap25.SetNameAndStatus(L("SkyScraper"), L(""));
			textmap26.SetNameAndStatus(L("Torn"), L(""));
			textmap27.SetNameAndStatus(L("DarkSnow"), L(""));

			textmap.Hide();
			textmap2.Hide();
			textmap3.Hide();
			textmap4.Hide();
			textmap5.Hide();
			textmap6.Hide();
			textmap7.Hide();
			textmap8.Hide();
			textmap9.Hide();

			textmap10.Hide();
			textmap11.Hide();
			textmap12.Hide();
			textmap13.Hide();
			textmap14.Hide();
			textmap15.Hide();
			textmap16.Hide();
			textmap17.Hide();
			textmap18.Hide();

			textmap19.Show();
			textmap20.Show();
			textmap21.Show();
			textmap22.Show();
			textmap23.Show();
			textmap24.Show();
			textmap25.Show();
			textmap26.Show();
			textmap27.Show();

			mapassault.Hide();
			mapitaly.Hide();
			mapvertigo.Hide();
			mapinferno.Hide();
			mapnuke.Hide();
			mapdust.Hide();
			mapmirage.Hide();
			mapgressia.Hide();
			mapruin.Hide();

			assault.Hide();
			vertigo.Hide();
			italy.Hide();
			nuke.Hide();
			dust2.Hide();
			inferno.Hide();
			mirage.Hide();
			gressia.Hide();
			ruin.Hide();



			bigtree.Hide();
			dustmini.Hide();
			cs747.Hide();
			estate.Hide();
			havana.Hide();
			militia.Hide();
			office.Hide();
			siege.Hide();
			angelcity.Hide();

			mapbigtree.Hide();
			mapdustmini.Hide();
			mapcs747.Hide();
			mapestate.Hide();
			maphavana.Hide();
			mapmilitia.Hide();
			mapoffice.Hide();
			mapsiege.Hide();
			mapangelcity.Hide();

			aztec.Show();
			cbble.Show();
			chateau.Show();
			prodigy.Show();
			rats.Show();
			santorini.Show();
			skyscraper.Show();
			torn.Show();
			darksnow.Show();

			mapaztec.Show();
			mapcbble.Show();
			mapchateau.Show();
			mapprodigy.Show();
			maprats.Show();
			mapsantorini.Show();
			mapskyscraper.Show();
			maptorn.Show();
			mapdarksnow.Show();
		}
	}
	else if (value == "dm")
	{
		dm.LinkCvar("mp_gamemode");
		EngFuncs::CvarSetString("mp_gamemode", value);
		ischecksetmode = true;
		
		modenone = false;
		modedm = true;
		modetdm = false;
		modegd = false;
		modezb1 = false;
		modezb3 = false;
		modezbs = false;
		modezsh = false;

		classic.bChecked = false;
		dm.bChecked = true;
		tdm.bChecked = false;
		gd.bChecked = false;
		zc.bChecked = false;
		zh.bChecked = false;
		sz.bChecked = false;
		zsh.bChecked = false;
		
		italy.bChecked = false;
		assault.bChecked = false;
		vertigo.bChecked = false;
		nuke.bChecked = false;
		dust2.bChecked = false;
		mirage.bChecked = false;
		inferno.bChecked = false;
		gressia.bChecked = false;
		ruin.bChecked = false;
		bigtree.bChecked = false;
		dustmini.bChecked = false;
		cs747.bChecked = false;
		estate.bChecked = false;
		havana.bChecked = false;
		militia.bChecked = false;
		office.bChecked = false;
		siege.bChecked = false;
		angelcity.bChecked = false;

		aztec.bChecked = false;
		cbble.bChecked = false;
		chateau.bChecked = false;
		prodigy.bChecked = false;
		rats.bChecked = false;
		santorini.bChecked = false;
		skyscraper.bChecked = false;
		torn.bChecked = false;
		darksnow.bChecked = false;

		MapSetAztec.Hide();
		MapSetCbble.Hide();
		MapSetChateau.Hide();
		MapSetProdigy.Hide();
		MapSetRats.Hide();
		MapSetSantorini.Hide();
		MapSetSkyScraper.Hide();
		MapSetTorn.Hide();
		MapSetDarkSnow.Hide();

		uparrow.Show();
		downarrow.Show();

		mapnightmare.Hide();
		MapSetNightMare.Hide();
		nightmare.Hide();
		textmap28.Hide();
		nightmare.bChecked = false;

		mapnightmare2.Hide();
		MapSetNightMare2.Hide();
		mapnightmare3.Hide();
		MapSetNightMare3.Hide();
		maplastclue.Hide();
		MapSetLastClue.Hide();
		maplostcity.Hide();
		MapSetLostCity.Hide();
		mappanic.Hide();
		MapSetPanic.Hide();
		maptrap.Hide();
		MapSetTrap.Hide();



		nightmare2.Hide();
		textmap29.Hide();

		nightmare3.Hide();
		textmap30.Hide();

		lastclue.Hide();
		textmap31.Hide();

		lostcity.Hide();
		textmap32.Hide();

		panic.Hide();
		textmap33.Hide();

		trap.Hide();
		textmap34.Hide();

		nightmare2.bChecked = false;
		nightmare3.bChecked = false;
		lastclue.bChecked = false;
		lostcity.bChecked = false;
		panic.bChecked = false;
		trap.bChecked = false;

		MapSetBigTree.Hide();
		MapSetDustMini.Hide();
		MapSet747.Hide();
		MapSetEstate.Hide();
		MapSetHavana.Hide();
		MapSetMilitia.Hide();
		MapSetOffice.Hide();
		MapSetSiege.Hide();
		MapSetAngelCity.Hide();
		MapSetAssault.Hide();
		MapSetItaly.Hide();
		MapSetVertigo.Hide();
		MapSetInferno.Hide();
		MapSetNuke.Hide();
		MapSetDust.Hide();
		MapSetMirage.Hide();
		MapSetGressia.Hide();
		MapSetRuin.Hide();

		if (stringmaplist == 1)
		{
			textmap.SetNameAndStatus(L("Assault"), L(""));
			textmap2.SetNameAndStatus(L("Italy"), L(""));
			textmap3.SetNameAndStatus(L("Vertigo"), L(""));

			textmap4.SetNameAndStatus(L("Inferno"), L(""));
			textmap5.SetNameAndStatus(L("Nuke"), L(""));
			textmap6.SetNameAndStatus(L("Dust II"), L(""));

			textmap7.SetNameAndStatus(L("Mirage"), L(""));
			textmap8.SetNameAndStatus(L("Greesia"), L(""));
			textmap9.SetNameAndStatus(L("Ruin"), L(""));

			textmap.Show();
			textmap2.Show();
			textmap3.Show();
			textmap4.Show();
			textmap5.Show();
			textmap6.Show();
			textmap7.Show();
			textmap8.Show();
			textmap9.Show();

			textmap10.Hide();
			textmap11.Hide();
			textmap12.Hide();
			textmap13.Hide();
			textmap14.Hide();
			textmap15.Hide();
			textmap16.Hide();
			textmap17.Hide();
			textmap18.Hide();

			textmap19.Hide();
			textmap20.Hide();
			textmap21.Hide();
			textmap22.Hide();
			textmap23.Hide();
			textmap24.Hide();
			textmap25.Hide();
			textmap26.Hide();
			textmap27.Hide();

			mapassault.Show();
			mapitaly.Show();
			mapvertigo.Show();
			mapinferno.Show();
			mapnuke.Show();
			mapdust.Show();
			mapmirage.Show();
			mapgressia.Show();
			mapruin.Show();

			assault.Show();
			vertigo.Show();
			italy.Show();
			nuke.Show();
			dust2.Show();
			inferno.Show();
			mirage.Show();
			gressia.Show();
			ruin.Show();

			bigtree.Hide();
			dustmini.Hide();
			cs747.Hide();
			estate.Hide();
			havana.Hide();
			militia.Hide();
			office.Hide();
			siege.Hide();
			angelcity.Hide();

			mapbigtree.Hide();
			mapdustmini.Hide();
			mapcs747.Hide();
			mapestate.Hide();
			maphavana.Hide();
			mapmilitia.Hide();
			mapoffice.Hide();
			mapsiege.Hide();
			mapangelcity.Hide();

			aztec.Hide();
			cbble.Hide();
			chateau.Hide();
			prodigy.Hide();
			rats.Hide();
			santorini.Hide();
			skyscraper.Hide();
			torn.Hide();
			darksnow.Hide();

			mapaztec.Hide();
			mapcbble.Hide();
			mapchateau.Hide();
			mapprodigy.Hide();
			maprats.Hide();
			mapsantorini.Hide();
			mapskyscraper.Hide();
			maptorn.Hide();
			mapdarksnow.Hide();
		}
		else if (stringmaplist == 2)
		{
			textmap10.SetNameAndStatus(L("BigTree"), L(""));
			textmap11.SetNameAndStatus(L("Dust EX"), L(""));
			textmap12.SetNameAndStatus(L("747"), L(""));

			textmap13.SetNameAndStatus(L("Estate"), L(""));
			textmap14.SetNameAndStatus(L("Havana"), L(""));
			textmap15.SetNameAndStatus(L("Militia"), L(""));

			textmap16.SetNameAndStatus(L("Office"), L(""));
			textmap17.SetNameAndStatus(L("Siege"), L(""));
			textmap18.SetNameAndStatus(L("Angel City"), L(""));

			textmap.Hide();
			textmap2.Hide();
			textmap3.Hide();
			textmap4.Hide();
			textmap5.Hide();
			textmap6.Hide();
			textmap7.Hide();
			textmap8.Hide();
			textmap9.Hide();

			textmap10.Show();
			textmap11.Show();
			textmap12.Show();
			textmap13.Show();
			textmap14.Show();
			textmap15.Show();
			textmap16.Show();
			textmap17.Show();
			textmap18.Show();

			textmap19.Hide();
			textmap20.Hide();
			textmap21.Hide();
			textmap22.Hide();
			textmap23.Hide();
			textmap24.Hide();
			textmap25.Hide();
			textmap26.Hide();
			textmap27.Hide();

			mapassault.Hide();
			mapitaly.Hide();
			mapvertigo.Hide();
			mapinferno.Hide();
			mapnuke.Hide();
			mapdust.Hide();
			mapmirage.Hide();
			mapgressia.Hide();
			mapruin.Hide();

			assault.Hide();
			vertigo.Hide();
			italy.Hide();
			nuke.Hide();
			dust2.Hide();
			inferno.Hide();
			mirage.Hide();
			gressia.Hide();
			ruin.Hide();

			bigtree.Show();
			dustmini.Show();
			cs747.Show();
			estate.Show();
			havana.Show();
			militia.Show();
			office.Show();
			siege.Show();
			angelcity.Show();

			mapbigtree.Show();
			mapdustmini.Show();
			mapcs747.Show();
			mapestate.Show();
			maphavana.Show();
			mapmilitia.Show();
			mapoffice.Show();
			mapsiege.Show();
			mapangelcity.Show();

			aztec.Hide();
			cbble.Hide();
			chateau.Hide();
			prodigy.Hide();
			rats.Hide();
			santorini.Hide();
			skyscraper.Hide();
			torn.Hide();
			darksnow.Hide();

			mapaztec.Hide();
			mapcbble.Hide();
			mapchateau.Hide();
			mapprodigy.Hide();
			maprats.Hide();
			mapsantorini.Hide();
			mapskyscraper.Hide();
			maptorn.Hide();
			mapdarksnow.Hide();
		}
		else if (stringmaplist == 3)
		{
			textmap19.SetNameAndStatus(L("Aztec"), L(""));
			textmap20.SetNameAndStatus(L("CobbleStone"), L(""));
			textmap21.SetNameAndStatus(L("ChateAu"), L(""));

			textmap22.SetNameAndStatus(L("Prodigy"), L(""));
			textmap23.SetNameAndStatus(L("Rats"), L(""));
			textmap24.SetNameAndStatus(L("Santorini"), L(""));

			textmap25.SetNameAndStatus(L("SkyScraper"), L(""));
			textmap26.SetNameAndStatus(L("Torn"), L(""));
			textmap27.SetNameAndStatus(L("DarkSnow"), L(""));

			textmap.Hide();
			textmap2.Hide();
			textmap3.Hide();
			textmap4.Hide();
			textmap5.Hide();
			textmap6.Hide();
			textmap7.Hide();
			textmap8.Hide();
			textmap9.Hide();

			textmap10.Hide();
			textmap11.Hide();
			textmap12.Hide();
			textmap13.Hide();
			textmap14.Hide();
			textmap15.Hide();
			textmap16.Hide();
			textmap17.Hide();
			textmap18.Hide();

			textmap19.Show();
			textmap20.Show();
			textmap21.Show();
			textmap22.Show();
			textmap23.Show();
			textmap24.Show();
			textmap25.Show();
			textmap26.Show();
			textmap27.Show();

			mapassault.Hide();
			mapitaly.Hide();
			mapvertigo.Hide();
			mapinferno.Hide();
			mapnuke.Hide();
			mapdust.Hide();
			mapmirage.Hide();
			mapgressia.Hide();
			mapruin.Hide();

			assault.Hide();
			vertigo.Hide();
			italy.Hide();
			nuke.Hide();
			dust2.Hide();
			inferno.Hide();
			mirage.Hide();
			gressia.Hide();
			ruin.Hide();



			bigtree.Hide();
			dustmini.Hide();
			cs747.Hide();
			estate.Hide();
			havana.Hide();
			militia.Hide();
			office.Hide();
			siege.Hide();
			angelcity.Hide();

			mapbigtree.Hide();
			mapdustmini.Hide();
			mapcs747.Hide();
			mapestate.Hide();
			maphavana.Hide();
			mapmilitia.Hide();
			mapoffice.Hide();
			mapsiege.Hide();
			mapangelcity.Hide();

			aztec.Show();
			cbble.Show();
			chateau.Show();
			prodigy.Show();
			rats.Show();
			santorini.Show();
			skyscraper.Show();
			torn.Show();
			darksnow.Show();

			mapaztec.Show();
			mapcbble.Show();
			mapchateau.Show();
			mapprodigy.Show();
			maprats.Show();
			mapsantorini.Show();
			mapskyscraper.Show();
			maptorn.Show();
			mapdarksnow.Show();
		}
	}
	else if (value == "tdm")
	{
		tdm.LinkCvar("mp_gamemode");
		EngFuncs::CvarSetString("mp_gamemode", value);
		ischecksetmode = true;

		modenone = false;
		modedm = false;
		modetdm = true;
		modegd = false;
		modezb1 = false;
		modezb3 = false;
		modezbs = false;
		modezsh = false;

		classic.bChecked = false;
		dm.bChecked = false;
		tdm.bChecked = true;
		gd.bChecked = false;
		zc.bChecked = false;
		zh.bChecked = false;
		sz.bChecked = false;
		zsh.bChecked = false;

		vertigo.bChecked = false;
		assault.bChecked = false;
		nuke.bChecked = false;
		inferno.bChecked = false;
		dust2.bChecked = false;
		italy.bChecked = false;
		mirage.bChecked = false;
		gressia.bChecked = false;
		ruin.bChecked = false;
		bigtree.bChecked = false;
		dustmini.bChecked = false;
		cs747.bChecked = false;
		estate.bChecked = false;
		havana.bChecked = false;
		militia.bChecked = false;
		office.bChecked = false;
		siege.bChecked = false;
		angelcity.bChecked = false;

		aztec.bChecked = false;
		cbble.bChecked = false;
		chateau.bChecked = false;
		prodigy.bChecked = false;
		rats.bChecked = false;
		santorini.bChecked = false;
		skyscraper.bChecked = false;
		torn.bChecked = false;
		darksnow.bChecked = false;

		MapSetAztec.Hide();
		MapSetCbble.Hide();
		MapSetChateau.Hide();
		MapSetProdigy.Hide();
		MapSetRats.Hide();
		MapSetSantorini.Hide();
		MapSetSkyScraper.Hide();
		MapSetTorn.Hide();
		MapSetDarkSnow.Hide();

		uparrow.Show();
		downarrow.Show();

		mapnightmare.Hide();
		MapSetNightMare.Hide();
		nightmare.Hide();
		textmap28.Hide();
		nightmare.bChecked = false;

		mapnightmare2.Hide();
		MapSetNightMare2.Hide();
		mapnightmare3.Hide();
		MapSetNightMare3.Hide();
		maplastclue.Hide();
		MapSetLastClue.Hide();
		maplostcity.Hide();
		MapSetLostCity.Hide();
		mappanic.Hide();
		MapSetPanic.Hide();
		maptrap.Hide();
		MapSetTrap.Hide();



		nightmare2.Hide();
		textmap29.Hide();

		nightmare3.Hide();
		textmap30.Hide();

		lastclue.Hide();
		textmap31.Hide();

		lostcity.Hide();
		textmap32.Hide();

		panic.Hide();
		textmap33.Hide();

		trap.Hide();
		textmap34.Hide();

		nightmare2.bChecked = false;
		nightmare3.bChecked = false;
		lastclue.bChecked = false;
		lostcity.bChecked = false;
		panic.bChecked = false;
		trap.bChecked = false;

		MapSetBigTree.Hide();
		MapSetDustMini.Hide();
		MapSet747.Hide();
		MapSetEstate.Hide();
		MapSetHavana.Hide();
		MapSetMilitia.Hide();
		MapSetOffice.Hide();
		MapSetSiege.Hide();
		MapSetAngelCity.Hide();
		MapSetAssault.Hide();
		MapSetItaly.Hide();
		MapSetVertigo.Hide();
		MapSetInferno.Hide();
		MapSetNuke.Hide();
		MapSetDust.Hide();
		MapSetMirage.Hide();
		MapSetGressia.Hide();
		MapSetRuin.Hide();

		if (stringmaplist == 1)
		{
			textmap.SetNameAndStatus(L("Assault"), L(""));
			textmap2.SetNameAndStatus(L("Italy"), L(""));
			textmap3.SetNameAndStatus(L("Vertigo"), L(""));

			textmap4.SetNameAndStatus(L("Inferno"), L(""));
			textmap5.SetNameAndStatus(L("Nuke"), L(""));
			textmap6.SetNameAndStatus(L("Dust II"), L(""));

			textmap7.SetNameAndStatus(L("Mirage"), L(""));
			textmap8.SetNameAndStatus(L("Greesia"), L(""));
			textmap9.SetNameAndStatus(L("Ruin"), L(""));

			textmap.Show();
			textmap2.Show();
			textmap3.Show();
			textmap4.Show();
			textmap5.Show();
			textmap6.Show();
			textmap7.Show();
			textmap8.Show();
			textmap9.Show();

			textmap10.Hide();
			textmap11.Hide();
			textmap12.Hide();
			textmap13.Hide();
			textmap14.Hide();
			textmap15.Hide();
			textmap16.Hide();
			textmap17.Hide();
			textmap18.Hide();

			textmap19.Hide();
			textmap20.Hide();
			textmap21.Hide();
			textmap22.Hide();
			textmap23.Hide();
			textmap24.Hide();
			textmap25.Hide();
			textmap26.Hide();
			textmap27.Hide();

			mapassault.Show();
			mapitaly.Show();
			mapvertigo.Show();
			mapinferno.Show();
			mapnuke.Show();
			mapdust.Show();
			mapmirage.Show();
			mapgressia.Show();
			mapruin.Show();

			assault.Show();
			vertigo.Show();
			italy.Show();
			nuke.Show();
			dust2.Show();
			inferno.Show();
			mirage.Show();
			gressia.Show();
			ruin.Show();

			bigtree.Hide();
			dustmini.Hide();
			cs747.Hide();
			estate.Hide();
			havana.Hide();
			militia.Hide();
			office.Hide();
			siege.Hide();
			angelcity.Hide();

			mapbigtree.Hide();
			mapdustmini.Hide();
			mapcs747.Hide();
			mapestate.Hide();
			maphavana.Hide();
			mapmilitia.Hide();
			mapoffice.Hide();
			mapsiege.Hide();
			mapangelcity.Hide();

			aztec.Hide();
			cbble.Hide();
			chateau.Hide();
			prodigy.Hide();
			rats.Hide();
			santorini.Hide();
			skyscraper.Hide();
			torn.Hide();
			darksnow.Hide();

			mapaztec.Hide();
			mapcbble.Hide();
			mapchateau.Hide();
			mapprodigy.Hide();
			maprats.Hide();
			mapsantorini.Hide();
			mapskyscraper.Hide();
			maptorn.Hide();
			mapdarksnow.Hide();
		}
		else if (stringmaplist == 2)
		{
			textmap10.SetNameAndStatus(L("BigTree"), L(""));
			textmap11.SetNameAndStatus(L("Dust EX"), L(""));
			textmap12.SetNameAndStatus(L("747"), L(""));

			textmap13.SetNameAndStatus(L("Estate"), L(""));
			textmap14.SetNameAndStatus(L("Havana"), L(""));
			textmap15.SetNameAndStatus(L("Militia"), L(""));

			textmap16.SetNameAndStatus(L("Office"), L(""));
			textmap17.SetNameAndStatus(L("Siege"), L(""));
			textmap18.SetNameAndStatus(L("Angel City"), L(""));

			textmap.Hide();
			textmap2.Hide();
			textmap3.Hide();
			textmap4.Hide();
			textmap5.Hide();
			textmap6.Hide();
			textmap7.Hide();
			textmap8.Hide();
			textmap9.Hide();

			textmap10.Show();
			textmap11.Show();
			textmap12.Show();
			textmap13.Show();
			textmap14.Show();
			textmap15.Show();
			textmap16.Show();
			textmap17.Show();
			textmap18.Show();

			textmap19.Hide();
			textmap20.Hide();
			textmap21.Hide();
			textmap22.Hide();
			textmap23.Hide();
			textmap24.Hide();
			textmap25.Hide();
			textmap26.Hide();
			textmap27.Hide();

			mapassault.Hide();
			mapitaly.Hide();
			mapvertigo.Hide();
			mapinferno.Hide();
			mapnuke.Hide();
			mapdust.Hide();
			mapmirage.Hide();
			mapgressia.Hide();
			mapruin.Hide();

			assault.Hide();
			vertigo.Hide();
			italy.Hide();
			nuke.Hide();
			dust2.Hide();
			inferno.Hide();
			mirage.Hide();
			gressia.Hide();
			ruin.Hide();

			bigtree.Show();
			dustmini.Show();
			cs747.Show();
			estate.Show();
			havana.Show();
			militia.Show();
			office.Show();
			siege.Show();
			angelcity.Show();

			mapbigtree.Show();
			mapdustmini.Show();
			mapcs747.Show();
			mapestate.Show();
			maphavana.Show();
			mapmilitia.Show();
			mapoffice.Show();
			mapsiege.Show();
			mapangelcity.Show();

			aztec.Hide();
			cbble.Hide();
			chateau.Hide();
			prodigy.Hide();
			rats.Hide();
			santorini.Hide();
			skyscraper.Hide();
			torn.Hide();
			darksnow.Hide();

			mapaztec.Hide();
			mapcbble.Hide();
			mapchateau.Hide();
			mapprodigy.Hide();
			maprats.Hide();
			mapsantorini.Hide();
			mapskyscraper.Hide();
			maptorn.Hide();
			mapdarksnow.Hide();
		}
		else if (stringmaplist == 3)
		{
			textmap19.SetNameAndStatus(L("Aztec"), L(""));
			textmap20.SetNameAndStatus(L("CobbleStone"), L(""));
			textmap21.SetNameAndStatus(L("ChateAu"), L(""));

			textmap22.SetNameAndStatus(L("Prodigy"), L(""));
			textmap23.SetNameAndStatus(L("Rats"), L(""));
			textmap24.SetNameAndStatus(L("Santorini"), L(""));

			textmap25.SetNameAndStatus(L("SkyScraper"), L(""));
			textmap26.SetNameAndStatus(L("Torn"), L(""));
			textmap27.SetNameAndStatus(L("DarkSnow"), L(""));

			textmap.Hide();
			textmap2.Hide();
			textmap3.Hide();
			textmap4.Hide();
			textmap5.Hide();
			textmap6.Hide();
			textmap7.Hide();
			textmap8.Hide();
			textmap9.Hide();

			textmap10.Hide();
			textmap11.Hide();
			textmap12.Hide();
			textmap13.Hide();
			textmap14.Hide();
			textmap15.Hide();
			textmap16.Hide();
			textmap17.Hide();
			textmap18.Hide();

			textmap19.Show();
			textmap20.Show();
			textmap21.Show();
			textmap22.Show();
			textmap23.Show();
			textmap24.Show();
			textmap25.Show();
			textmap26.Show();
			textmap27.Show();

			mapassault.Hide();
			mapitaly.Hide();
			mapvertigo.Hide();
			mapinferno.Hide();
			mapnuke.Hide();
			mapdust.Hide();
			mapmirage.Hide();
			mapgressia.Hide();
			mapruin.Hide();

			assault.Hide();
			vertigo.Hide();
			italy.Hide();
			nuke.Hide();
			dust2.Hide();
			inferno.Hide();
			mirage.Hide();
			gressia.Hide();
			ruin.Hide();



			bigtree.Hide();
			dustmini.Hide();
			cs747.Hide();
			estate.Hide();
			havana.Hide();
			militia.Hide();
			office.Hide();
			siege.Hide();
			angelcity.Hide();

			mapbigtree.Hide();
			mapdustmini.Hide();
			mapcs747.Hide();
			mapestate.Hide();
			maphavana.Hide();
			mapmilitia.Hide();
			mapoffice.Hide();
			mapsiege.Hide();
			mapangelcity.Hide();

			aztec.Show();
			cbble.Show();
			chateau.Show();
			prodigy.Show();
			rats.Show();
			santorini.Show();
			skyscraper.Show();
			torn.Show();
			darksnow.Show();

			mapaztec.Show();
			mapcbble.Show();
			mapchateau.Show();
			mapprodigy.Show();
			maprats.Show();
			mapsantorini.Show();
			mapskyscraper.Show();
			maptorn.Show();
			mapdarksnow.Show();
		}
	}
	else if (value == "gd")
	{
		gd.LinkCvar("mp_gamemode");
		EngFuncs::CvarSetString("mp_gamemode", value);
		ischecksetmode = true;

		modenone = false;
		modedm = false;
		modetdm = false;
		modegd = true;
		modezb1 = false;
		modezb3 = false;
		modezbs = false;
		modezsh = false;

		classic.bChecked = false;
		dm.bChecked = false;
		tdm.bChecked = false;
		gd.bChecked = true;
		zc.bChecked = false;
		zh.bChecked = false;
		sz.bChecked = false;
		zsh.bChecked = false;

		assault.bChecked = false;
		nuke.bChecked = false;
		vertigo.bChecked = false;
		dust2.bChecked = false;
		inferno.bChecked = false;
		italy.bChecked = false;
		mirage.bChecked = false;
		gressia.bChecked = false;
		ruin.bChecked = false;
		bigtree.bChecked = false;
		dustmini.bChecked = false;
		cs747.bChecked = false;
		estate.bChecked = false;
		havana.bChecked = false;
		militia.bChecked = false;
		office.bChecked = false;
		siege.bChecked = false;
		angelcity.bChecked = false;

		aztec.bChecked = false;
		cbble.bChecked = false;
		chateau.bChecked = false;
		prodigy.bChecked = false;
		rats.bChecked = false;
		santorini.bChecked = false;
		skyscraper.bChecked = false;
		torn.bChecked = false;
		darksnow.bChecked = false;

		MapSetAztec.Hide();
		MapSetCbble.Hide();
		MapSetChateau.Hide();
		MapSetProdigy.Hide();
		MapSetRats.Hide();
		MapSetSantorini.Hide();
		MapSetSkyScraper.Hide();
		MapSetTorn.Hide();
		MapSetDarkSnow.Hide();

		uparrow.Show();
		downarrow.Show();

		mapnightmare.Hide();
		MapSetNightMare.Hide();
		nightmare.Hide();
		textmap28.Hide();
		nightmare.bChecked = false;

		mapnightmare2.Hide();
		MapSetNightMare2.Hide();
		mapnightmare3.Hide();
		MapSetNightMare3.Hide();
		maplastclue.Hide();
		MapSetLastClue.Hide();
		maplostcity.Hide();
		MapSetLostCity.Hide();
		mappanic.Hide();
		MapSetPanic.Hide();
		maptrap.Hide();
		MapSetTrap.Hide();



		nightmare2.Hide();
		textmap29.Hide();

		nightmare3.Hide();
		textmap30.Hide();

		lastclue.Hide();
		textmap31.Hide();

		lostcity.Hide();
		textmap32.Hide();

		panic.Hide();
		textmap33.Hide();

		trap.Hide();
		textmap34.Hide();

		nightmare2.bChecked = false;
		nightmare3.bChecked = false;
		lastclue.bChecked = false;
		lostcity.bChecked = false;
		panic.bChecked = false;
		trap.bChecked = false;

		MapSetBigTree.Hide();
		MapSetDustMini.Hide();
		MapSet747.Hide();
		MapSetEstate.Hide();
		MapSetHavana.Hide();
		MapSetMilitia.Hide();
		MapSetOffice.Hide();
		MapSetSiege.Hide();
		MapSetAngelCity.Hide();
		MapSetAssault.Hide();
		MapSetItaly.Hide();
		MapSetVertigo.Hide();
		MapSetInferno.Hide();
		MapSetNuke.Hide();
		MapSetDust.Hide();
		MapSetMirage.Hide();
		MapSetGressia.Hide();
		MapSetRuin.Hide();
	}
	else if (value == "zb1")
	{
		zc.LinkCvar("mp_gamemode");
		EngFuncs::CvarSetString("mp_gamemode", value);
		ischecksetmode = true;

		modenone = false;
		modedm = false;
		modetdm = false;
		modegd = false;
		modezb1 = true;
		modezb3 = false;
		modezbs = false;
		modezsh = false;

		classic.bChecked = false;
		dm.bChecked = false;
		tdm.bChecked = false;
		gd.bChecked = false;
		zc.bChecked = true;
		zh.bChecked = false;
		sz.bChecked = false;
		zsh.bChecked = false;

		assault.bChecked = false;
		vertigo.bChecked = false;
		nuke.bChecked = false;
		dust2.bChecked = false;
		italy.bChecked = false;
		mirage.bChecked = false;
		inferno.bChecked = false;
		gressia.bChecked = false;
		ruin.bChecked = false;
		bigtree.bChecked = false;
		dustmini.bChecked = false;
		cs747.bChecked = false;
		estate.bChecked = false;
		havana.bChecked = false;
		militia.bChecked = false;
		office.bChecked = false;
		siege.bChecked = false;
		angelcity.bChecked = false;

		aztec.bChecked = false;
		cbble.bChecked = false;
		chateau.bChecked = false;
		prodigy.bChecked = false;
		rats.bChecked = false;
		santorini.bChecked = false;
		skyscraper.bChecked = false;
		torn.bChecked = false;
		darksnow.bChecked = false;

		MapSetAztec.Hide();
		MapSetCbble.Hide();
		MapSetChateau.Hide();
		MapSetProdigy.Hide();
		MapSetRats.Hide();
		MapSetSantorini.Hide();
		MapSetSkyScraper.Hide();
		MapSetTorn.Hide();
		MapSetDarkSnow.Hide();

		uparrow.Show();
		downarrow.Show();

		mapnightmare.Hide();
		MapSetNightMare.Hide();
		nightmare.Hide();
		textmap28.Hide();
		nightmare.bChecked = false;

		mapnightmare2.Hide();
		MapSetNightMare2.Hide();
		mapnightmare3.Hide();
		MapSetNightMare3.Hide();
		maplastclue.Hide();
		MapSetLastClue.Hide();
		maplostcity.Hide();
		MapSetLostCity.Hide();
		mappanic.Hide();
		MapSetPanic.Hide();
		maptrap.Hide();
		MapSetTrap.Hide();



		nightmare2.Hide();
		textmap29.Hide();

		nightmare3.Hide();
		textmap30.Hide();

		lastclue.Hide();
		textmap31.Hide();

		lostcity.Hide();
		textmap32.Hide();

		panic.Hide();
		textmap33.Hide();

		trap.Hide();
		textmap34.Hide();

		nightmare2.bChecked = false;
		nightmare3.bChecked = false;
		lastclue.bChecked = false;
		lostcity.bChecked = false;
		panic.bChecked = false;
		trap.bChecked = false;

		MapSetBigTree.Hide();
		MapSetDustMini.Hide();
		MapSet747.Hide();
		MapSetEstate.Hide();
		MapSetHavana.Hide();
		MapSetMilitia.Hide();
		MapSetOffice.Hide();
		MapSetSiege.Hide();
		MapSetAngelCity.Hide();
		MapSetAssault.Hide();
		MapSetItaly.Hide();
		MapSetVertigo.Hide();
		MapSetInferno.Hide();
		MapSetNuke.Hide();
		MapSetDust.Hide();
		MapSetMirage.Hide();
		MapSetGressia.Hide();
		MapSetRuin.Hide();

		if (stringmaplist == 1)
		{
			textmap.SetNameAndStatus(L("Assault"), L(""));
			textmap2.SetNameAndStatus(L("Italy"), L(""));
			textmap3.SetNameAndStatus(L("Vertigo"), L(""));

			textmap4.SetNameAndStatus(L("Inferno"), L(""));
			textmap5.SetNameAndStatus(L("Nuke"), L(""));
			textmap6.SetNameAndStatus(L("Dust II"), L(""));

			textmap7.SetNameAndStatus(L("Mirage"), L(""));
			textmap8.SetNameAndStatus(L("Greesia"), L(""));
			textmap9.SetNameAndStatus(L("Ruin"), L(""));

			textmap.Show();
			textmap2.Show();
			textmap3.Show();
			textmap4.Show();
			textmap5.Show();
			textmap6.Show();
			textmap7.Show();
			textmap8.Show();
			textmap9.Show();

			textmap10.Hide();
			textmap11.Hide();
			textmap12.Hide();
			textmap13.Hide();
			textmap14.Hide();
			textmap15.Hide();
			textmap16.Hide();
			textmap17.Hide();
			textmap18.Hide();

			textmap19.Hide();
			textmap20.Hide();
			textmap21.Hide();
			textmap22.Hide();
			textmap23.Hide();
			textmap24.Hide();
			textmap25.Hide();
			textmap26.Hide();
			textmap27.Hide();

			mapassault.Show();
			mapitaly.Show();
			mapvertigo.Show();
			mapinferno.Show();
			mapnuke.Show();
			mapdust.Show();
			mapmirage.Show();
			mapgressia.Show();
			mapruin.Show();

			assault.Show();
			vertigo.Show();
			italy.Show();
			nuke.Show();
			dust2.Show();
			inferno.Show();
			mirage.Show();
			gressia.Show();
			ruin.Show();

			bigtree.Hide();
			dustmini.Hide();
			cs747.Hide();
			estate.Hide();
			havana.Hide();
			militia.Hide();
			office.Hide();
			siege.Hide();
			angelcity.Hide();

			mapbigtree.Hide();
			mapdustmini.Hide();
			mapcs747.Hide();
			mapestate.Hide();
			maphavana.Hide();
			mapmilitia.Hide();
			mapoffice.Hide();
			mapsiege.Hide();
			mapangelcity.Hide();

			aztec.Hide();
			cbble.Hide();
			chateau.Hide();
			prodigy.Hide();
			rats.Hide();
			santorini.Hide();
			skyscraper.Hide();
			torn.Hide();
			darksnow.Hide();

			mapaztec.Hide();
			mapcbble.Hide();
			mapchateau.Hide();
			mapprodigy.Hide();
			maprats.Hide();
			mapsantorini.Hide();
			mapskyscraper.Hide();
			maptorn.Hide();
			mapdarksnow.Hide();
		}
		else if (stringmaplist == 2)
		{
			textmap10.SetNameAndStatus(L("BigTree"), L(""));
			textmap11.SetNameAndStatus(L("Dust EX"), L(""));
			textmap12.SetNameAndStatus(L("747"), L(""));

			textmap13.SetNameAndStatus(L("Estate"), L(""));
			textmap14.SetNameAndStatus(L("Havana"), L(""));
			textmap15.SetNameAndStatus(L("Militia"), L(""));

			textmap16.SetNameAndStatus(L("Office"), L(""));
			textmap17.SetNameAndStatus(L("Siege"), L(""));
			textmap18.SetNameAndStatus(L("Angel City"), L(""));

			textmap.Hide();
			textmap2.Hide();
			textmap3.Hide();
			textmap4.Hide();
			textmap5.Hide();
			textmap6.Hide();
			textmap7.Hide();
			textmap8.Hide();
			textmap9.Hide();

			textmap10.Show();
			textmap11.Show();
			textmap12.Show();
			textmap13.Show();
			textmap14.Show();
			textmap15.Show();
			textmap16.Show();
			textmap17.Show();
			textmap18.Show();

			textmap19.Hide();
			textmap20.Hide();
			textmap21.Hide();
			textmap22.Hide();
			textmap23.Hide();
			textmap24.Hide();
			textmap25.Hide();
			textmap26.Hide();
			textmap27.Hide();

			mapassault.Hide();
			mapitaly.Hide();
			mapvertigo.Hide();
			mapinferno.Hide();
			mapnuke.Hide();
			mapdust.Hide();
			mapmirage.Hide();
			mapgressia.Hide();
			mapruin.Hide();

			assault.Hide();
			vertigo.Hide();
			italy.Hide();
			nuke.Hide();
			dust2.Hide();
			inferno.Hide();
			mirage.Hide();
			gressia.Hide();
			ruin.Hide();

			bigtree.Show();
			dustmini.Show();
			cs747.Show();
			estate.Show();
			havana.Show();
			militia.Show();
			office.Show();
			siege.Show();
			angelcity.Show();

			mapbigtree.Show();
			mapdustmini.Show();
			mapcs747.Show();
			mapestate.Show();
			maphavana.Show();
			mapmilitia.Show();
			mapoffice.Show();
			mapsiege.Show();
			mapangelcity.Show();

			aztec.Hide();
			cbble.Hide();
			chateau.Hide();
			prodigy.Hide();
			rats.Hide();
			santorini.Hide();
			skyscraper.Hide();
			torn.Hide();
			darksnow.Hide();

			mapaztec.Hide();
			mapcbble.Hide();
			mapchateau.Hide();
			mapprodigy.Hide();
			maprats.Hide();
			mapsantorini.Hide();
			mapskyscraper.Hide();
			maptorn.Hide();
			mapdarksnow.Hide();
		}
		else if (stringmaplist == 3)
		{
			textmap19.SetNameAndStatus(L("Aztec"), L(""));
			textmap20.SetNameAndStatus(L("CobbleStone"), L(""));
			textmap21.SetNameAndStatus(L("ChateAu"), L(""));

			textmap22.SetNameAndStatus(L("Prodigy"), L(""));
			textmap23.SetNameAndStatus(L("Rats"), L(""));
			textmap24.SetNameAndStatus(L("Santorini"), L(""));

			textmap25.SetNameAndStatus(L("SkyScraper"), L(""));
			textmap26.SetNameAndStatus(L("Torn"), L(""));
			textmap27.SetNameAndStatus(L("DarkSnow"), L(""));

			textmap.Hide();
			textmap2.Hide();
			textmap3.Hide();
			textmap4.Hide();
			textmap5.Hide();
			textmap6.Hide();
			textmap7.Hide();
			textmap8.Hide();
			textmap9.Hide();

			textmap10.Hide();
			textmap11.Hide();
			textmap12.Hide();
			textmap13.Hide();
			textmap14.Hide();
			textmap15.Hide();
			textmap16.Hide();
			textmap17.Hide();
			textmap18.Hide();

			textmap19.Show();
			textmap20.Show();
			textmap21.Show();
			textmap22.Show();
			textmap23.Show();
			textmap24.Show();
			textmap25.Show();
			textmap26.Show();
			textmap27.Show();

			mapassault.Hide();
			mapitaly.Hide();
			mapvertigo.Hide();
			mapinferno.Hide();
			mapnuke.Hide();
			mapdust.Hide();
			mapmirage.Hide();
			mapgressia.Hide();
			mapruin.Hide();

			assault.Hide();
			vertigo.Hide();
			italy.Hide();
			nuke.Hide();
			dust2.Hide();
			inferno.Hide();
			mirage.Hide();
			gressia.Hide();
			ruin.Hide();

			mapnightmare.Hide();
			MapSetNightMare.Hide();
			nightmare.Hide();

			bigtree.Hide();
			dustmini.Hide();
			cs747.Hide();
			estate.Hide();
			havana.Hide();
			militia.Hide();
			office.Hide();
			siege.Hide();
			angelcity.Hide();

			mapbigtree.Hide();
			mapdustmini.Hide();
			mapcs747.Hide();
			mapestate.Hide();
			maphavana.Hide();
			mapmilitia.Hide();
			mapoffice.Hide();
			mapsiege.Hide();
			mapangelcity.Hide();

			aztec.Show();
			cbble.Show();
			chateau.Show();
			prodigy.Show();
			rats.Show();
			santorini.Show();
			skyscraper.Show();
			torn.Show();
			darksnow.Show();

			mapaztec.Show();
			mapcbble.Show();
			mapchateau.Show();
			mapprodigy.Show();
			maprats.Show();
			mapsantorini.Show();
			mapskyscraper.Show();
			maptorn.Show();
			mapdarksnow.Show();
		}
	}
	else if (value == "zb5")
	{
		zh.LinkCvar("mp_gamemode");
		EngFuncs::CvarSetString("mp_gamemode", value);
		ischecksetmode = true;

		modenone = false;
		modedm = false;
		modetdm = false;
		modegd = false;
		modezb1 = false;
		modezb3 = true;
		modezbs = false;
		modezsh = false;

		classic.bChecked = false;
		dm.bChecked = false;
		tdm.bChecked = false;
		gd.bChecked = false;
		zc.bChecked = false;
		zh.bChecked = true;
		sz.bChecked = false;
		zsh.bChecked = false;

		assault.bChecked = false;
		vertigo.bChecked = false;
		nuke.bChecked = false;
		dust2.bChecked = false;
		italy.bChecked = false;
		mirage.bChecked = false;
		inferno.bChecked = false;
		gressia.bChecked = false;
		ruin.bChecked = false;
		bigtree.bChecked = false;
		dustmini.bChecked = false;
		cs747.bChecked = false;
		estate.bChecked = false;
		havana.bChecked = false;
		militia.bChecked = false;
		office.bChecked = false;
		siege.bChecked = false;
		angelcity.bChecked = false;

		aztec.bChecked = false;
		cbble.bChecked = false;
		chateau.bChecked = false;
		prodigy.bChecked = false;
		rats.bChecked = false;
		santorini.bChecked = false;
		skyscraper.bChecked = false;
		torn.bChecked = false;
		darksnow.bChecked = false;

		uparrow.Show();
		downarrow.Show();

		mapnightmare.Hide();
		MapSetNightMare.Hide();
		nightmare.Hide();
		nightmare.bChecked = false;

		mapnightmare2.Hide();
		MapSetNightMare2.Hide();
		mapnightmare3.Hide();
		MapSetNightMare3.Hide();
		maplastclue.Hide();
		MapSetLastClue.Hide();
		maplostcity.Hide();
		MapSetLostCity.Hide();
		mappanic.Hide();
		MapSetPanic.Hide();
		maptrap.Hide();
		MapSetTrap.Hide();



		nightmare2.Hide();
		textmap29.Hide();

		nightmare3.Hide();
		textmap30.Hide();

		lastclue.Hide();
		textmap31.Hide();

		lostcity.Hide();
		textmap32.Hide();

		panic.Hide();
		textmap33.Hide();

		trap.Hide();
		textmap34.Hide();

		nightmare2.bChecked = false;
		nightmare3.bChecked = false;
		lastclue.bChecked = false;
		lostcity.bChecked = false;
		panic.bChecked = false;
		trap.bChecked = false;

		MapSetAztec.Hide();
		MapSetCbble.Hide();
		MapSetChateau.Hide();
		MapSetProdigy.Hide();
		MapSetRats.Hide();
		MapSetSantorini.Hide();
		MapSetSkyScraper.Hide();
		MapSetTorn.Hide();
		MapSetDarkSnow.Hide();

		MapSetBigTree.Hide();
		MapSetDustMini.Hide();
		MapSet747.Hide();
		MapSetEstate.Hide();
		MapSetHavana.Hide();
		MapSetMilitia.Hide();
		MapSetOffice.Hide();
		MapSetSiege.Hide();
		MapSetAngelCity.Hide();
		MapSetAssault.Hide();
		MapSetItaly.Hide();
		MapSetVertigo.Hide();
		MapSetInferno.Hide();
		MapSetNuke.Hide();
		MapSetDust.Hide();
		MapSetMirage.Hide();
		MapSetGressia.Hide();
		MapSetRuin.Hide();

		if (stringmaplist == 1)
		{
			textmap.SetNameAndStatus(L("Assault"), L(""));
			textmap2.SetNameAndStatus(L("Italy"), L(""));
			textmap3.SetNameAndStatus(L("Vertigo"), L(""));

			textmap4.SetNameAndStatus(L("Inferno"), L(""));
			textmap5.SetNameAndStatus(L("Nuke"), L(""));
			textmap6.SetNameAndStatus(L("Dust II"), L(""));

			textmap7.SetNameAndStatus(L("Mirage"), L(""));
			textmap8.SetNameAndStatus(L("Greesia"), L(""));
			textmap9.SetNameAndStatus(L("Ruin"), L(""));

			textmap.Show();
			textmap2.Show();
			textmap3.Show();
			textmap4.Show();
			textmap5.Show();
			textmap6.Show();
			textmap7.Show();
			textmap8.Show();
			textmap9.Show();

			textmap10.Hide();
			textmap11.Hide();
			textmap12.Hide();
			textmap13.Hide();
			textmap14.Hide();
			textmap15.Hide();
			textmap16.Hide();
			textmap17.Hide();
			textmap18.Hide();

			textmap19.Hide();
			textmap20.Hide();
			textmap21.Hide();
			textmap22.Hide();
			textmap23.Hide();
			textmap24.Hide();
			textmap25.Hide();
			textmap26.Hide();
			textmap27.Hide();

			mapassault.Show();
			mapitaly.Show();
			mapvertigo.Show();
			mapinferno.Show();
			mapnuke.Show();
			mapdust.Show();
			mapmirage.Show();
			mapgressia.Show();
			mapruin.Show();

			mapnightmare.Hide();
			MapSetNightMare.Hide();
			nightmare.Hide();
			textmap28.Hide();

			assault.Show();
			vertigo.Show();
			italy.Show();
			nuke.Show();
			dust2.Show();
			inferno.Show();
			mirage.Show();
			gressia.Show();
			ruin.Show();

			bigtree.Hide();
			dustmini.Hide();
			cs747.Hide();
			estate.Hide();
			havana.Hide();
			militia.Hide();
			office.Hide();
			siege.Hide();
			angelcity.Hide();

			mapbigtree.Hide();
			mapdustmini.Hide();
			mapcs747.Hide();
			mapestate.Hide();
			maphavana.Hide();
			mapmilitia.Hide();
			mapoffice.Hide();
			mapsiege.Hide();
			mapangelcity.Hide();

			aztec.Hide();
			cbble.Hide();
			chateau.Hide();
			prodigy.Hide();
			rats.Hide();
			santorini.Hide();
			skyscraper.Hide();
			torn.Hide();
			darksnow.Hide();

			mapaztec.Hide();
			mapcbble.Hide();
			mapchateau.Hide();
			mapprodigy.Hide();
			maprats.Hide();
			mapsantorini.Hide();
			mapskyscraper.Hide();
			maptorn.Hide();
			mapdarksnow.Hide();
		}
		else if (stringmaplist == 2)
		{
			textmap10.SetNameAndStatus(L("BigTree"), L(""));
			textmap11.SetNameAndStatus(L("Dust EX"), L(""));
			textmap12.SetNameAndStatus(L("747"), L(""));

			textmap13.SetNameAndStatus(L("Estate"), L(""));
			textmap14.SetNameAndStatus(L("Havana"), L(""));
			textmap15.SetNameAndStatus(L("Militia"), L(""));

			textmap16.SetNameAndStatus(L("Office"), L(""));
			textmap17.SetNameAndStatus(L("Siege"), L(""));
			textmap18.SetNameAndStatus(L("Angel City"), L(""));

			textmap.Hide();
			textmap2.Hide();
			textmap3.Hide();
			textmap4.Hide();
			textmap5.Hide();
			textmap6.Hide();
			textmap7.Hide();
			textmap8.Hide();
			textmap9.Hide();

			textmap10.Show();
			textmap11.Show();
			textmap12.Show();
			textmap13.Show();
			textmap14.Show();
			textmap15.Show();
			textmap16.Show();
			textmap17.Show();
			textmap18.Show();

			textmap19.Hide();
			textmap20.Hide();
			textmap21.Hide();
			textmap22.Hide();
			textmap23.Hide();
			textmap24.Hide();
			textmap25.Hide();
			textmap26.Hide();
			textmap27.Hide();

			mapassault.Hide();
			mapitaly.Hide();
			mapvertigo.Hide();
			mapinferno.Hide();
			mapnuke.Hide();
			mapdust.Hide();
			mapmirage.Hide();
			mapgressia.Hide();
			mapruin.Hide();

			mapnightmare.Hide();
			MapSetNightMare.Hide();
			nightmare.Hide();

			assault.Hide();
			vertigo.Hide();
			italy.Hide();
			nuke.Hide();
			dust2.Hide();
			inferno.Hide();
			mirage.Hide();
			gressia.Hide();
			ruin.Hide();

			bigtree.Show();
			dustmini.Show();
			cs747.Show();
			estate.Show();
			havana.Show();
			militia.Show();
			office.Show();
			siege.Show();
			angelcity.Show();

			mapbigtree.Show();
			mapdustmini.Show();
			mapcs747.Show();
			mapestate.Show();
			maphavana.Show();
			mapmilitia.Show();
			mapoffice.Show();
			mapsiege.Show();
			mapangelcity.Show();

			aztec.Hide();
			cbble.Hide();
			chateau.Hide();
			prodigy.Hide();
			rats.Hide();
			santorini.Hide();
			skyscraper.Hide();
			torn.Hide();
			darksnow.Hide();

			mapaztec.Hide();
			mapcbble.Hide();
			mapchateau.Hide();
			mapprodigy.Hide();
			maprats.Hide();
			mapsantorini.Hide();
			mapskyscraper.Hide();
			maptorn.Hide();
			mapdarksnow.Hide();
		}
		else if (stringmaplist == 3)
		{
			textmap19.SetNameAndStatus(L("Aztec"), L(""));
			textmap20.SetNameAndStatus(L("CobbleStone"), L(""));
			textmap21.SetNameAndStatus(L("ChateAu"), L(""));

			textmap22.SetNameAndStatus(L("Prodigy"), L(""));
			textmap23.SetNameAndStatus(L("Rats"), L(""));
			textmap24.SetNameAndStatus(L("Santorini"), L(""));

			textmap25.SetNameAndStatus(L("SkyScraper"), L(""));
			textmap26.SetNameAndStatus(L("Torn"), L(""));
			textmap27.SetNameAndStatus(L("DarkSnow"), L(""));

			textmap.Hide();
			textmap2.Hide();
			textmap3.Hide();
			textmap4.Hide();
			textmap5.Hide();
			textmap6.Hide();
			textmap7.Hide();
			textmap8.Hide();
			textmap9.Hide();

			textmap10.Hide();
			textmap11.Hide();
			textmap12.Hide();
			textmap13.Hide();
			textmap14.Hide();
			textmap15.Hide();
			textmap16.Hide();
			textmap17.Hide();
			textmap18.Hide();

			textmap19.Show();
			textmap20.Show();
			textmap21.Show();
			textmap22.Show();
			textmap23.Show();
			textmap24.Show();
			textmap25.Show();
			textmap26.Show();
			textmap27.Show();

			mapassault.Hide();
			mapitaly.Hide();
			mapvertigo.Hide();
			mapinferno.Hide();
			mapnuke.Hide();
			mapdust.Hide();
			mapmirage.Hide();
			mapgressia.Hide();
			mapruin.Hide();

			assault.Hide();
			vertigo.Hide();
			italy.Hide();
			nuke.Hide();
			dust2.Hide();
			inferno.Hide();
			mirage.Hide();
			gressia.Hide();
			ruin.Hide();

			mapnightmare.Hide();
			MapSetNightMare.Hide();
			nightmare.Hide();

			bigtree.Hide();
			dustmini.Hide();
			cs747.Hide();
			estate.Hide();
			havana.Hide();
			militia.Hide();
			office.Hide();
			siege.Hide();
			angelcity.Hide();

			mapbigtree.Hide();
			mapdustmini.Hide();
			mapcs747.Hide();
			mapestate.Hide();
			maphavana.Hide();
			mapmilitia.Hide();
			mapoffice.Hide();
			mapsiege.Hide();
			mapangelcity.Hide();

			aztec.Show();
			cbble.Show();
			chateau.Show();
			prodigy.Show();
			rats.Show();
			santorini.Show();
			skyscraper.Show();
			torn.Show();
			darksnow.Show();

			mapaztec.Show();
			mapcbble.Show();
			mapchateau.Show();
			mapprodigy.Show();
			maprats.Show();
			mapsantorini.Show();
			mapskyscraper.Show();
			maptorn.Show();
			mapdarksnow.Show();
		}
	}
	else if (value == "zbs")
	{
		sz.LinkCvar("mp_gamemode");
		EngFuncs::CvarSetString("mp_gamemode", value);
		ischecksetmode = true;

		modenone = false;
		modedm = false;
		modetdm = false;
		modegd = false;
		modezb1 = false;
		modezb3 = false;
		modezbs = true;
		modezsh = false;

		classic.bChecked = false;
		dm.bChecked = false;
		tdm.bChecked = false;
		gd.bChecked = false;
		zc.bChecked = false;
		zh.bChecked = false;
		sz.bChecked = true;
		zsh.bChecked = false;

		mapnightmare.Show();
		//MapSetNightMare.Show();
		nightmare.Show();
		textmap28.Show();

		textmap28.SetNameAndStatus(L("NightMare"), L(""));
		textmap29.SetNameAndStatus(L("NightMare2"), L(""));
		textmap30.SetNameAndStatus(L("NightMare3"), L(""));
		textmap31.SetNameAndStatus(L("Last Clue"), L(""));
		textmap32.SetNameAndStatus(L("Lost City"), L(""));
		textmap33.SetNameAndStatus(L("Panic"), L(""));
		textmap34.SetNameAndStatus(L("Trap"), L(""));

		mapnightmare2.Show();
		//MapSetNightMare2.Show();
		mapnightmare3.Show();
		//MapSetNightMare3.Show();
		maplastclue.Show();
		//MapSetLastClue.Show();
		maplostcity.Show();
		//MapSetLostCity.Show();
		mappanic.Show();
		//MapSetPanic.Show();
		maptrap.Show();
		//MapSetTrap.Show();



		nightmare2.Show();
		textmap29.Show();

		nightmare3.Show();
		textmap30.Show();

		lastclue.Show();
		textmap31.Show();

		lostcity.Show();
		textmap32.Show();

		panic.Show();
		textmap33.Show();

		trap.Show();
		textmap34.Show();

		nightmare2.bChecked = false;
		nightmare3.bChecked = false;
		lastclue.bChecked = false;
		lostcity.bChecked = false;
		panic.bChecked = false;
		trap.bChecked = false;

		assault.bChecked = false;
		vertigo.bChecked = false;
		nuke.bChecked = false;
		dust2.bChecked = false;
		italy.bChecked = false;
		mirage.bChecked = false;
		inferno.bChecked = false;
		gressia.bChecked = false;
		ruin.bChecked = false;
		bigtree.bChecked = false;
		dustmini.bChecked = false;
		cs747.bChecked = false;
		estate.bChecked = false;
		havana.bChecked = false;
		militia.bChecked = false;
		office.bChecked = false;
		siege.bChecked = false;
		angelcity.bChecked = false;

		aztec.bChecked = false;
		cbble.bChecked = false;
		chateau.bChecked = false;
		prodigy.bChecked = false;
		rats.bChecked = false;
		santorini.bChecked = false;
		skyscraper.bChecked = false;
		torn.bChecked = false;
		darksnow.bChecked = false;

		MapSetAztec.Hide();
		MapSetCbble.Hide();
		MapSetChateau.Hide();
		MapSetProdigy.Hide();
		MapSetRats.Hide();
		MapSetSantorini.Hide();
		MapSetSkyScraper.Hide();
		MapSetTorn.Hide();
		MapSetDarkSnow.Hide();

		MapSetBigTree.Hide();
		MapSetDustMini.Hide();
		MapSet747.Hide();
		MapSetEstate.Hide();
		MapSetHavana.Hide();
		MapSetMilitia.Hide();
		MapSetOffice.Hide();
		MapSetSiege.Hide();
		MapSetAngelCity.Hide();
		MapSetAssault.Hide();
		MapSetItaly.Hide();
		MapSetVertigo.Hide();
		MapSetInferno.Hide();
		MapSetNuke.Hide();
		MapSetDust.Hide();
		MapSetMirage.Hide();
		MapSetGressia.Hide();
		MapSetRuin.Hide();

		textmap.Hide();
		textmap2.Hide();
		textmap3.Hide();
		textmap4.Hide();
		textmap5.Hide();
		textmap6.Hide();
		textmap7.Hide();
		textmap8.Hide();
		textmap9.Hide();

		textmap10.Hide();
		textmap11.Hide();
		textmap12.Hide();
		textmap13.Hide();
		textmap14.Hide();
		textmap15.Hide();
		textmap16.Hide();
		textmap17.Hide();
		textmap18.Hide();

		textmap19.Hide();
		textmap20.Hide();
		textmap21.Hide();
		textmap22.Hide();
		textmap23.Hide();
		textmap24.Hide();
		textmap25.Hide();
		textmap26.Hide();
		textmap27.Hide();

		mapassault.Hide();
		mapitaly.Hide();
		mapvertigo.Hide();
		mapinferno.Hide();
		mapnuke.Hide();
		mapdust.Hide();
		mapmirage.Hide();
		mapgressia.Hide();
		mapruin.Hide();

		assault.Hide();
		vertigo.Hide();
		italy.Hide();
		nuke.Hide();
		dust2.Hide();
		inferno.Hide();
		mirage.Hide();
		gressia.Hide();
		ruin.Hide();

		bigtree.Hide();
		dustmini.Hide();
		cs747.Hide();
		estate.Hide();
		havana.Hide();
		militia.Hide();
		office.Hide();
		siege.Hide();
		angelcity.Hide();

		mapbigtree.Hide();
		mapdustmini.Hide();
		mapcs747.Hide();
		mapestate.Hide();
		maphavana.Hide();
		mapmilitia.Hide();
		mapoffice.Hide();
		mapsiege.Hide();
		mapangelcity.Hide();

		aztec.Hide();
		cbble.Hide();
		chateau.Hide();
		prodigy.Hide();
		rats.Hide();
		santorini.Hide();
		skyscraper.Hide();
		torn.Hide();
		darksnow.Hide();

		uparrow.Hide();
		downarrow.Hide();

		mapaztec.Hide();
		mapcbble.Hide();
		mapchateau.Hide();
		mapprodigy.Hide();
		maprats.Hide();
		mapsantorini.Hide();
		mapskyscraper.Hide();
		maptorn.Hide();
		mapdarksnow.Hide();
	}
	else if (value == "zsh_pve")
	{
		zsh.LinkCvar("mp_gamemode");
		EngFuncs::CvarSetString("mp_gamemode", value);
		ischecksetmode = true;

		modenone = false;
		modedm = false;
		modetdm = false;
		modegd = false;
		modezb1 = false;
		modezb3 = false;
		modezbs = false;
		modezsh = true;

		classic.bChecked = false;
		dm.bChecked = false;
		tdm.bChecked = false;
		gd.bChecked = false;
		zc.bChecked = false;
		zh.bChecked = false;
		sz.bChecked = false;
		zsh.bChecked = true;

		assault.bChecked = false;
		vertigo.bChecked = false;
		nuke.bChecked = false;
		dust2.bChecked = false;
		italy.bChecked = false;
		mirage.bChecked = false;
		inferno.bChecked = false;
		gressia.bChecked = false;
		ruin.bChecked = false;
		bigtree.bChecked = false;
		dustmini.bChecked = false;
		cs747.bChecked = false;
		estate.bChecked = false;
		havana.bChecked = false;
		militia.bChecked = false;
		office.bChecked = false;
		siege.bChecked = false;
		angelcity.bChecked = false;

		aztec.bChecked = false;
		cbble.bChecked = false;
		chateau.bChecked = false;
		prodigy.bChecked = false;
		rats.bChecked = false;
		santorini.bChecked = false;
		skyscraper.bChecked = false;
		torn.bChecked = false;
		darksnow.bChecked = false;

		MapSetAztec.Hide();
		MapSetCbble.Hide();
		MapSetChateau.Hide();
		MapSetProdigy.Hide();
		MapSetRats.Hide();
		MapSetSantorini.Hide();
		MapSetSkyScraper.Hide();
		MapSetTorn.Hide();
		MapSetDarkSnow.Hide();

		MapSetBigTree.Hide();
		MapSetDustMini.Hide();
		MapSet747.Hide();
		MapSetEstate.Hide();
		MapSetHavana.Hide();
		MapSetMilitia.Hide();
		MapSetOffice.Hide();
		MapSetSiege.Hide();
		MapSetAngelCity.Hide();
		MapSetAssault.Hide();
		MapSetItaly.Hide();
		MapSetVertigo.Hide();
		MapSetInferno.Hide();
		MapSetNuke.Hide();
		MapSetDust.Hide();
		MapSetMirage.Hide();
		MapSetGressia.Hide();
		MapSetRuin.Hide();

		textmap.Hide();
		textmap2.Hide();
		textmap3.Hide();
		textmap4.Hide();
		textmap5.Hide();
		textmap6.Hide();
		textmap7.Hide();
		textmap8.Hide();
		textmap9.Hide();

		textmap10.Hide();
		textmap11.Hide();
		textmap12.Hide();
		textmap13.Hide();
		textmap14.Hide();
		textmap15.Hide();
		textmap16.Hide();
		textmap17.Hide();
		textmap18.Hide();

		textmap19.Hide();
		textmap20.Hide();
		textmap21.Hide();
		textmap22.Hide();
		textmap23.Hide();
		textmap24.Hide();
		textmap25.Hide();
		textmap26.Hide();
		textmap27.Hide();

		mapassault.Hide();
		mapitaly.Hide();
		mapvertigo.Hide();
		mapinferno.Hide();
		mapnuke.Hide();
		mapdust.Hide();
		mapmirage.Hide();
		mapgressia.Hide();
		mapruin.Hide();

		assault.Hide();
		vertigo.Hide();
		italy.Hide();
		nuke.Hide();
		dust2.Hide();
		inferno.Hide();
		mirage.Hide();
		gressia.Hide();
		ruin.Hide();

		bigtree.Hide();
		dustmini.Hide();
		cs747.Hide();
		estate.Hide();
		havana.Hide();
		militia.Hide();
		office.Hide();
		siege.Hide();
		angelcity.Hide();

		mapbigtree.Hide();
		mapdustmini.Hide();
		mapcs747.Hide();
		mapestate.Hide();
		maphavana.Hide();
		mapmilitia.Hide();
		mapoffice.Hide();
		mapsiege.Hide();
		mapangelcity.Hide();

		aztec.Hide();
		cbble.Hide();
		chateau.Hide();
		prodigy.Hide();
		rats.Hide();
		santorini.Hide();
		skyscraper.Hide();
		torn.Hide();
		darksnow.Hide();

		uparrow.Hide();
		downarrow.Hide();

		mapnightmare.Hide();
		MapSetNightMare.Hide();
		nightmare.Hide();
		textmap28.Hide();
		nightmare.bChecked = false;

		mapnightmare2.Hide();
		MapSetNightMare2.Hide();
		mapnightmare3.Hide();
		MapSetNightMare3.Hide();
		maplastclue.Hide();
		MapSetLastClue.Hide();
		maplostcity.Hide();
		MapSetLostCity.Hide();
		mappanic.Hide();
		MapSetPanic.Hide();
		maptrap.Hide();
		MapSetTrap.Hide();

		nightmare2.Hide();
		textmap29.Hide();

		nightmare3.Hide();
		textmap30.Hide();

		lastclue.Hide();
		textmap31.Hide();

		lostcity.Hide();
		textmap32.Hide();

		panic.Hide();
		textmap33.Hide();

		trap.Hide();
		textmap34.Hide();

		nightmare2.bChecked = false;
		nightmare3.bChecked = false;
		lastclue.bChecked = false;
		lostcity.bChecked = false;
		panic.bChecked = false;
		trap.bChecked = false;

		mapaztec.Hide();
		mapcbble.Hide();
		mapchateau.Hide();
		mapprodigy.Hide();
		maprats.Hide();
		mapsantorini.Hide();
		mapskyscraper.Hide();
		maptorn.Hide();
		mapdarksnow.Hide();
	}
	else
	{
		if (stringmaplist == 1)
		{
			textmap.SetNameAndStatus(L("Assault"), L(""));
			textmap2.SetNameAndStatus(L("Italy"), L(""));
			textmap3.SetNameAndStatus(L("Vertigo"), L(""));

			textmap4.SetNameAndStatus(L("Inferno"), L(""));
			textmap5.SetNameAndStatus(L("Nuke"), L(""));
			textmap6.SetNameAndStatus(L("Dust II"), L(""));

			textmap7.SetNameAndStatus(L("Mirage"), L(""));
			textmap8.SetNameAndStatus(L("Greesia"), L(""));
			textmap9.SetNameAndStatus(L("Ruin"), L(""));

			textmap.Show();
			textmap2.Show();
			textmap3.Show();
			textmap4.Show();
			textmap5.Show();
			textmap6.Show();
			textmap7.Show();
			textmap8.Show();
			textmap9.Show();

			textmap10.Hide();
			textmap11.Hide();
			textmap12.Hide();
			textmap13.Hide();
			textmap14.Hide();
			textmap15.Hide();
			textmap16.Hide();
			textmap17.Hide();
			textmap18.Hide();

			textmap19.Hide();
			textmap20.Hide();
			textmap21.Hide();
			textmap22.Hide();
			textmap23.Hide();
			textmap24.Hide();
			textmap25.Hide();
			textmap26.Hide();
			textmap27.Hide();

			mapassault.Show();
			mapitaly.Show();
			mapvertigo.Show();
			mapinferno.Show();
			mapnuke.Show();
			mapdust.Show();
			mapmirage.Show();
			mapgressia.Show();
			mapruin.Show();

			assault.Show();
			vertigo.Show();
			italy.Show();
			nuke.Show();
			dust2.Show();
			inferno.Show();
			mirage.Show();
			gressia.Show();
			ruin.Show();

			bigtree.Hide();
			dustmini.Hide();
			cs747.Hide();
			estate.Hide();
			havana.Hide();
			militia.Hide();
			office.Hide();
			siege.Hide();
			angelcity.Hide();

			mapbigtree.Hide();
			mapdustmini.Hide();
			mapcs747.Hide();
			mapestate.Hide();
			maphavana.Hide();
			mapmilitia.Hide();
			mapoffice.Hide();
			mapsiege.Hide();
			mapangelcity.Hide();

			aztec.Hide();
			cbble.Hide();
			chateau.Hide();
			prodigy.Hide();
			rats.Hide();
			santorini.Hide();
			skyscraper.Hide();
			torn.Hide();
			darksnow.Hide();

			mapaztec.Hide();
			mapcbble.Hide();
			mapchateau.Hide();
			mapprodigy.Hide();
			maprats.Hide();
			mapsantorini.Hide();
			mapskyscraper.Hide();
			maptorn.Hide();
			mapdarksnow.Hide();
		}
		else if (stringmaplist == 2)
		{
			textmap10.SetNameAndStatus(L("BigTree"), L(""));
			textmap11.SetNameAndStatus(L("Dust EX"), L(""));
			textmap12.SetNameAndStatus(L("747"), L(""));

			textmap13.SetNameAndStatus(L("Estate"), L(""));
			textmap14.SetNameAndStatus(L("Havana"), L(""));
			textmap15.SetNameAndStatus(L("Militia"), L(""));

			textmap16.SetNameAndStatus(L("Office"), L(""));
			textmap17.SetNameAndStatus(L("Siege"), L(""));
			textmap18.SetNameAndStatus(L("Angel City"), L(""));

			textmap.Hide();
			textmap2.Hide();
			textmap3.Hide();
			textmap4.Hide();
			textmap5.Hide();
			textmap6.Hide();
			textmap7.Hide();
			textmap8.Hide();
			textmap9.Hide();

			textmap10.Show();
			textmap11.Show();
			textmap12.Show();
			textmap13.Show();
			textmap14.Show();
			textmap15.Show();
			textmap16.Show();
			textmap17.Show();
			textmap18.Show();

			textmap19.Hide();
			textmap20.Hide();
			textmap21.Hide();
			textmap22.Hide();
			textmap23.Hide();
			textmap24.Hide();
			textmap25.Hide();
			textmap26.Hide();
			textmap27.Hide();

			mapassault.Hide();
			mapitaly.Hide();
			mapvertigo.Hide();
			mapinferno.Hide();
			mapnuke.Hide();
			mapdust.Hide();
			mapmirage.Hide();
			mapgressia.Hide();
			mapruin.Hide();

			assault.Hide();
			vertigo.Hide();
			italy.Hide();
			nuke.Hide();
			dust2.Hide();
			inferno.Hide();
			mirage.Hide();
			gressia.Hide();
			ruin.Hide();

			bigtree.Show();
			dustmini.Show();
			cs747.Show();
			estate.Show();
			havana.Show();
			militia.Show();
			office.Show();
			siege.Show();
			angelcity.Show();

			mapbigtree.Show();
			mapdustmini.Show();
			mapcs747.Show();
			mapestate.Show();
			maphavana.Show();
			mapmilitia.Show();
			mapoffice.Show();
			mapsiege.Show();
			mapangelcity.Show();

			aztec.Hide();
			cbble.Hide();
			chateau.Hide();
			prodigy.Hide();
			rats.Hide();
			santorini.Hide();
			skyscraper.Hide();
			torn.Hide();
			darksnow.Hide();

			mapaztec.Hide();
			mapcbble.Hide();
			mapchateau.Hide();
			mapprodigy.Hide();
			maprats.Hide();
			mapsantorini.Hide();
			mapskyscraper.Hide();
			maptorn.Hide();
			mapdarksnow.Hide();
		}
		else if (stringmaplist == 3)
		{
			textmap19.SetNameAndStatus(L("Aztec"), L(""));
			textmap20.SetNameAndStatus(L("CobbleStone"), L(""));
			textmap21.SetNameAndStatus(L("ChateAu"), L(""));

			textmap22.SetNameAndStatus(L("Prodigy"), L(""));
			textmap23.SetNameAndStatus(L("Rats"), L(""));
			textmap24.SetNameAndStatus(L("Santorini"), L(""));

			textmap25.SetNameAndStatus(L("SkyScraper"), L(""));
			textmap26.SetNameAndStatus(L("Torn"), L(""));
			textmap27.SetNameAndStatus(L("DarkSnow"), L(""));

			textmap.Hide();
			textmap2.Hide();
			textmap3.Hide();
			textmap4.Hide();
			textmap5.Hide();
			textmap6.Hide();
			textmap7.Hide();
			textmap8.Hide();
			textmap9.Hide();

			textmap10.Hide();
			textmap11.Hide();
			textmap12.Hide();
			textmap13.Hide();
			textmap14.Hide();
			textmap15.Hide();
			textmap16.Hide();
			textmap17.Hide();
			textmap18.Hide();

			textmap19.Show();
			textmap20.Show();
			textmap21.Show();
			textmap22.Show();
			textmap23.Show();
			textmap24.Show();
			textmap25.Show();
			textmap26.Show();
			textmap27.Show();

			mapassault.Hide();
			mapitaly.Hide();
			mapvertigo.Hide();
			mapinferno.Hide();
			mapnuke.Hide();
			mapdust.Hide();
			mapmirage.Hide();
			mapgressia.Hide();
			mapruin.Hide();

			assault.Hide();
			vertigo.Hide();
			italy.Hide();
			nuke.Hide();
			dust2.Hide();
			inferno.Hide();
			mirage.Hide();
			gressia.Hide();
			ruin.Hide();



			bigtree.Hide();
			dustmini.Hide();
			cs747.Hide();
			estate.Hide();
			havana.Hide();
			militia.Hide();
			office.Hide();
			siege.Hide();
			angelcity.Hide();

			mapbigtree.Hide();
			mapdustmini.Hide();
			mapcs747.Hide();
			mapestate.Hide();
			maphavana.Hide();
			mapmilitia.Hide();
			mapoffice.Hide();
			mapsiege.Hide();
			mapangelcity.Hide();

			aztec.Show();
			cbble.Show();
			chateau.Show();
			prodigy.Show();
			rats.Show();
			santorini.Show();
			skyscraper.Show();
			torn.Show();
			darksnow.Show();

			mapaztec.Show();
			mapcbble.Show();
			mapchateau.Show();
			mapprodigy.Show();
			maprats.Show();
			mapsantorini.Show();
			mapskyscraper.Show();
			maptorn.Show();
			mapdarksnow.Show();
		}

		ischecksetmode = false;

		modenone = false;
		modedm = false;
		modetdm = false;
		modegd = false;
		modezb1 = false;
		modezb3 = false;
		modezbs = false;
		modezsh = false;

		classic.bChecked = false;
		dm.bChecked = false;
		tdm.bChecked = false;
		gd.bChecked = false;
		zc.bChecked = false;
		zh.bChecked = false;
		sz.bChecked = false;
		zsh.bChecked = false;

		assault.bChecked = false;
		vertigo.bChecked = false;
		nuke.bChecked = false;
		dust2.bChecked = false;
		italy.bChecked = false;
		mirage.bChecked = false;
		inferno.bChecked = false;
		gressia.bChecked = false;
		ruin.bChecked = false;

		bigtree.bChecked = false;
		dustmini.bChecked = false;
		cs747.bChecked = false;
		estate.bChecked = false;
		havana.bChecked = false;
		militia.bChecked = false;
		office.bChecked = false;
		siege.bChecked = false;
		angelcity.bChecked = false;

		aztec.bChecked = false;
		cbble.bChecked = false;
		chateau.bChecked = false;
		prodigy.bChecked = false;
		rats.bChecked = false;
		santorini.bChecked = false;
		skyscraper.bChecked = false;
		torn.bChecked = false;
		darksnow.bChecked = false;

		MapSetAztec.Hide();
		MapSetCbble.Hide();
		MapSetChateau.Hide();
		MapSetProdigy.Hide();
		MapSetRats.Hide();
		MapSetSantorini.Hide();
		MapSetSkyScraper.Hide();
		MapSetTorn.Hide();
		MapSetDarkSnow.Hide();

		MapSetBigTree.Hide();
		MapSetDustMini.Hide();
		MapSet747.Hide();
		MapSetEstate.Hide();
		MapSetHavana.Hide();
		MapSetMilitia.Hide();
		MapSetOffice.Hide();
		MapSetSiege.Hide();
		MapSetAngelCity.Hide();

		MapSetAssault.Hide();
		MapSetItaly.Hide();
		MapSetVertigo.Hide();
		MapSetInferno.Hide();
		MapSetNuke.Hide();
		MapSetDust.Hide();
		MapSetMirage.Hide();
		MapSetGressia.Hide();
		MapSetRuin.Hide();
	}
}

void CMenuCreateGame::ResetMap( int value)
{
	if (value == 1)//assault
	{
		ischecksetmap = true;
		italy.bChecked = false;
		assault.bChecked = true;
		vertigo.bChecked = false;
		nuke.bChecked = false;
		dust2.bChecked = false;
		inferno.bChecked = false;
		mirage.bChecked = false;
		gressia.bChecked = false;
		ruin.bChecked = false;

		MapSetAssault.Show();
		MapSetItaly.Hide();
		MapSetVertigo.Hide();
		MapSetInferno.Hide();
		MapSetNuke.Hide();
		MapSetDust.Hide();
		MapSetMirage.Hide();
		MapSetGressia.Hide();
		MapSetRuin.Hide();
	}
	else if(value == 2)//italy
	{
		ischecksetmap = true;
		italy.bChecked = true;
		assault.bChecked = false;
		vertigo.bChecked = false;
		nuke.bChecked = false;
		dust2.bChecked = false;
		mirage.bChecked = false;
		inferno.bChecked = false;
		gressia.bChecked = false;
		ruin.bChecked = false;

		MapSetAssault.Hide();
		MapSetItaly.Show();
		MapSetVertigo.Hide();
		MapSetNuke.Hide();
		MapSetDust.Hide();
		MapSetInferno.Hide();
		MapSetMirage.Hide();
		MapSetGressia.Hide();
		MapSetRuin.Hide();
	}
	else if (value == 3)//vertigo
	{
		ischecksetmap = true;
		vertigo.bChecked = true;
		assault.bChecked = false;
		nuke.bChecked = false;
		inferno.bChecked = false;
		dust2.bChecked = false;
		italy.bChecked = false;
		mirage.bChecked = false;
		gressia.bChecked = false;
		ruin.bChecked = false;

		MapSetAssault.Hide();
		MapSetItaly.Hide();
		MapSetVertigo.Show();
		MapSetInferno.Hide();
		MapSetNuke.Hide();
		MapSetDust.Hide();
		MapSetMirage.Hide();
		MapSetGressia.Hide();
		MapSetRuin.Hide();
	}
	else if (value == 4)//inferno
	{
		ischecksetmap = true;
		assault.bChecked = false;
		nuke.bChecked = false;
		vertigo.bChecked = false;
		dust2.bChecked = false;
		italy.bChecked = false;
		mirage.bChecked = false;
		inferno.bChecked = true;
		gressia.bChecked = false;
		ruin.bChecked = false;

		MapSetAssault.Hide();
		MapSetItaly.Hide();
		MapSetVertigo.Hide();
		MapSetAssault.Hide();
		MapSetInferno.Show();
		MapSetNuke.Hide();
		MapSetDust.Hide();
		MapSetMirage.Hide();
		MapSetGressia.Hide();
		MapSetRuin.Hide();
	}
	else if (value == 5)//nuke
	{
		ischecksetmap = true;
		assault.bChecked = false;
		vertigo.bChecked = false;
		dust2.bChecked = false;
		italy.bChecked = false;
		mirage.bChecked = false;
		inferno.bChecked = false;
		nuke.bChecked = true;
		gressia.bChecked = false;
		ruin.bChecked = false;

		MapSetAssault.Hide();
		MapSetItaly.Hide();
		MapSetVertigo.Hide();
		MapSetAssault.Hide();
		MapSetInferno.Hide();
		MapSetDust.Hide();
		MapSetNuke.Show();
		MapSetMirage.Hide();
		MapSetGressia.Hide();
		MapSetRuin.Hide();
	}
	else if (value == 6)//dust
	{
		ischecksetmap = true;
		assault.bChecked = false;
		vertigo.bChecked = false;
		italy.bChecked = false;
		mirage.bChecked = false;
		nuke.bChecked = false;
		inferno.bChecked = false;
		dust2.bChecked = true;
		gressia.bChecked = false;
		ruin.bChecked = false;

		MapSetAssault.Hide();
		MapSetItaly.Hide();
		MapSetVertigo.Hide();
		MapSetAssault.Hide();
		MapSetInferno.Hide();
		MapSetNuke.Hide();
		MapSetDust.Show();
		MapSetMirage.Hide();
		MapSetGressia.Hide();
		MapSetRuin.Hide();
	}
	else if (value == 7)//mirage
	{
		ischecksetmap = true;
		assault.bChecked = false;
		vertigo.bChecked = false;
		italy.bChecked = false;
		mirage.bChecked = true;
		nuke.bChecked = false;
		inferno.bChecked = false;
		dust2.bChecked = false;
		gressia.bChecked = false;
		ruin.bChecked = false;

		MapSetAssault.Hide();
		MapSetItaly.Hide();
		MapSetVertigo.Hide();
		MapSetAssault.Hide();
		MapSetInferno.Hide();
		MapSetNuke.Hide();
		MapSetDust.Hide();
		MapSetMirage.Show();
		MapSetGressia.Hide();
		MapSetRuin.Hide();
	}
	else if (value == 8)//gressia
	{
		ischecksetmap = true;
		assault.bChecked = false;
		vertigo.bChecked = false;
		italy.bChecked = false;
		mirage.bChecked = false;
		nuke.bChecked = false;
		inferno.bChecked = false;
		dust2.bChecked = false;
		gressia.bChecked = true;
		ruin.bChecked = false;

		MapSetAssault.Hide();
		MapSetItaly.Hide();
		MapSetVertigo.Hide();
		MapSetAssault.Hide();
		MapSetInferno.Hide();
		MapSetNuke.Hide();
		MapSetDust.Hide();
		MapSetMirage.Hide();
		MapSetGressia.Show();
		MapSetRuin.Hide();
	}
	else if (value == 9)//ruin
	{
		ischecksetmap = true;
		assault.bChecked = false;
		vertigo.bChecked = false;
		italy.bChecked = false;
		mirage.bChecked = false;
		nuke.bChecked = false;
		inferno.bChecked = false;
		dust2.bChecked = false;
		gressia.bChecked = false;
		ruin.bChecked = true;

		MapSetAssault.Hide();
		MapSetItaly.Hide();
		MapSetVertigo.Hide();
		MapSetAssault.Hide();
		MapSetInferno.Hide();
		MapSetNuke.Hide();
		MapSetDust.Hide();
		MapSetMirage.Hide();
		MapSetGressia.Hide();
		MapSetRuin.Show();
	}
	else if (value == 10)//bigtree
	{
		ischecksetmap = true;

		bigtree.bChecked = true;
		dustmini.bChecked = false;
		cs747.bChecked = false;
		estate.bChecked = false;
		havana.bChecked = false;
		militia.bChecked = false;
		office.bChecked = false;
		siege.bChecked = false;
		angelcity.bChecked = false;

		MapSetBigTree.Show();
		MapSetDustMini.Hide();
		MapSet747.Hide();
		MapSetEstate.Hide();
		MapSetHavana.Hide();
		MapSetMilitia.Hide();
		MapSetOffice.Hide();
		MapSetSiege.Hide();
		MapSetAngelCity.Hide();
	}
	else if (value == 11)//dustmini
	{
		ischecksetmap = true;
		bigtree.bChecked = false;
		dustmini.bChecked = true;
		cs747.bChecked = false;
		estate.bChecked = false;
		havana.bChecked = false;
		militia.bChecked = false;
		office.bChecked = false;
		siege.bChecked = false;
		angelcity.bChecked = false;

		MapSetBigTree.Hide();
		MapSetDustMini.Show();
		MapSet747.Hide();
		MapSetEstate.Hide();
		MapSetHavana.Hide();
		MapSetMilitia.Hide();
		MapSetOffice.Hide();
		MapSetSiege.Hide();
		MapSetAngelCity.Hide();
	}
	else if (value == 12)//747
	{
		ischecksetmap = true;
		bigtree.bChecked = false;
		dustmini.bChecked = false;
		cs747.bChecked = true;
		estate.bChecked = false;
		havana.bChecked = false;
		militia.bChecked = false;
		office.bChecked = false;
		siege.bChecked = false;
		angelcity.bChecked = false;

		MapSetBigTree.Hide();
		MapSetDustMini.Hide();
		MapSet747.Show();
		MapSetEstate.Hide();
		MapSetHavana.Hide();
		MapSetMilitia.Hide();
		MapSetOffice.Hide();
		MapSetSiege.Hide();
		MapSetAngelCity.Hide();
	}
	else if (value == 13)//estate
	{
		ischecksetmap = true;
		bigtree.bChecked = false;
		dustmini.bChecked = false;
		cs747.bChecked = false;
		estate.bChecked = true;
		havana.bChecked = false;
		militia.bChecked = false;
		office.bChecked = false;
		siege.bChecked = false;
		angelcity.bChecked = false;

		MapSetBigTree.Hide();
		MapSetDustMini.Hide();
		MapSet747.Hide();
		MapSetEstate.Show();
		MapSetHavana.Hide();
		MapSetMilitia.Hide();
		MapSetOffice.Hide();
		MapSetSiege.Hide();
		MapSetAngelCity.Hide();
	}
	else if (value == 14)//havana
	{
		ischecksetmap = true;
		bigtree.bChecked = false;
		dustmini.bChecked = false;
		cs747.bChecked = false;
		estate.bChecked = false;
		havana.bChecked = true;
		militia.bChecked = false;
		office.bChecked = false;
		siege.bChecked = false;
		angelcity.bChecked = false;

		MapSetBigTree.Hide();
		MapSetDustMini.Hide();
		MapSet747.Hide();
		MapSetEstate.Hide();
		MapSetHavana.Show();
		MapSetMilitia.Hide();
		MapSetOffice.Hide();
		MapSetSiege.Hide();
		MapSetAngelCity.Hide();
	}
	else if (value == 15)//militia
	{
		ischecksetmap = true;
		bigtree.bChecked = false;
		dustmini.bChecked = false;
		cs747.bChecked = false;
		estate.bChecked = false;
		havana.bChecked = false;
		militia.bChecked = true;
		office.bChecked = false;
		siege.bChecked = false;
		angelcity.bChecked = false;

		MapSetBigTree.Hide();
		MapSetDustMini.Hide();
		MapSet747.Hide();
		MapSetEstate.Hide();
		MapSetHavana.Hide();
		MapSetMilitia.Show();
		MapSetOffice.Hide();
		MapSetSiege.Hide();
		MapSetAngelCity.Hide();
	}
	else if (value == 16)//office
	{
		ischecksetmap = true;
		bigtree.bChecked = false;
		dustmini.bChecked = false;
		cs747.bChecked = false;
		estate.bChecked = false;
		havana.bChecked = false;
		militia.bChecked = false;
		office.bChecked = true;
		siege.bChecked = false;
		angelcity.bChecked = false;

		MapSetBigTree.Hide();
		MapSetDustMini.Hide();
		MapSet747.Hide();
		MapSetEstate.Hide();
		MapSetHavana.Hide();
		MapSetMilitia.Hide();
		MapSetOffice.Show();
		MapSetSiege.Hide();
		MapSetAngelCity.Hide();
	}
	else if (value == 17)//siege
	{
		ischecksetmap = true;
		bigtree.bChecked = false;
		dustmini.bChecked = false;
		cs747.bChecked = false;
		estate.bChecked = false;
		havana.bChecked = false;
		militia.bChecked = false;
		office.bChecked = false;
		siege.bChecked = true;
		angelcity.bChecked = false;

		MapSetBigTree.Hide();
		MapSetDustMini.Hide();
		MapSet747.Hide();
		MapSetEstate.Hide();
		MapSetHavana.Hide();
		MapSetMilitia.Hide();
		MapSetOffice.Hide();
		MapSetSiege.Show();
		MapSetAngelCity.Hide();
	}
	else if (value == 18)//angelcity
	{
		ischecksetmap = true;
		bigtree.bChecked = false;
		dustmini.bChecked = false;
		cs747.bChecked = false;
		estate.bChecked = false;
		havana.bChecked = false;
		militia.bChecked = false;
		office.bChecked = false;
		siege.bChecked = false;
		angelcity.bChecked = true;

		MapSetBigTree.Hide();
		MapSetDustMini.Hide();
		MapSet747.Hide();
		MapSetEstate.Hide();
		MapSetHavana.Hide();
		MapSetMilitia.Hide();
		MapSetOffice.Hide();
		MapSetSiege.Hide();
		MapSetAngelCity.Show();
	}
	else if (value == 19)//aztec
	{
		ischecksetmap = true;

		aztec.bChecked = true;
		cbble.bChecked = false;
		chateau.bChecked = false;
		prodigy.bChecked = false;
		rats.bChecked = false;
		santorini.bChecked = false;
		skyscraper.bChecked = false;
		torn.bChecked = false;
		darksnow.bChecked = false;

		MapSetAztec.Show();
		MapSetCbble.Hide();
		MapSetChateau.Hide();
		MapSetProdigy.Hide();
		MapSetRats.Hide();
		MapSetSantorini.Hide();
		MapSetSkyScraper.Hide();
		MapSetTorn.Hide();
		MapSetDarkSnow.Hide();	
	}
	else if (value == 20)//cbble
	{
		ischecksetmap = true;

		aztec.bChecked = false;
		cbble.bChecked = true;
		chateau.bChecked = false;
		prodigy.bChecked = false;
		rats.bChecked = false;
		santorini.bChecked = false;
		skyscraper.bChecked = false;
		torn.bChecked = false;
		darksnow.bChecked = false;

		MapSetAztec.Hide();
		MapSetCbble.Show();
		MapSetChateau.Hide();
		MapSetProdigy.Hide();
		MapSetRats.Hide();
		MapSetSantorini.Hide();
		MapSetSkyScraper.Hide();
		MapSetTorn.Hide();
		MapSetDarkSnow.Hide();	
	}
	else if (value == 21)//chateau
	{
		ischecksetmap = true;

		aztec.bChecked = false;
		cbble.bChecked = false;
		chateau.bChecked = true;
		prodigy.bChecked = false;
		rats.bChecked = false;
		santorini.bChecked = false;
		skyscraper.bChecked = false;
		torn.bChecked = false;
		darksnow.bChecked = false;

		MapSetAztec.Hide();
		MapSetCbble.Hide();
		MapSetChateau.Show();
		MapSetProdigy.Hide();
		MapSetRats.Hide();
		MapSetSantorini.Hide();
		MapSetSkyScraper.Hide();
		MapSetTorn.Hide();
		MapSetDarkSnow.Hide();	
		MapSetNightMare.Hide();
	}
	else if (value == 22)//prodigy
	{
		ischecksetmap = true;

		aztec.bChecked = false;
		cbble.bChecked = false;
		chateau.bChecked = false;
		prodigy.bChecked = true;
		rats.bChecked = false;
		santorini.bChecked = false;
		skyscraper.bChecked = false;
		torn.bChecked = false;
		darksnow.bChecked = false;

		MapSetAztec.Hide();
		MapSetCbble.Hide();
		MapSetChateau.Hide();
		MapSetProdigy.Show();
		MapSetRats.Hide();
		MapSetSantorini.Hide();
		MapSetSkyScraper.Hide();
		MapSetTorn.Hide();
		MapSetDarkSnow.Hide();	
		MapSetNightMare.Hide();
	}
	else if (value == 23)//rats
	{
		ischecksetmap = true;

		aztec.bChecked = false;
		cbble.bChecked = false;
		chateau.bChecked = false;
		prodigy.bChecked = false;
		rats.bChecked = true;
		santorini.bChecked = false;
		skyscraper.bChecked = false;
		torn.bChecked = false;
		darksnow.bChecked = false;

		MapSetAztec.Hide();
		MapSetCbble.Hide();
		MapSetChateau.Hide();
		MapSetProdigy.Hide();
		MapSetRats.Show();
		MapSetSantorini.Hide();
		MapSetSkyScraper.Hide();
		MapSetTorn.Hide();
		MapSetDarkSnow.Hide();
		MapSetNightMare.Hide();
	}
	else if (value == 24)//santorini
	{
		ischecksetmap = true;

		aztec.bChecked = false;
		cbble.bChecked = false;
		chateau.bChecked = false;
		prodigy.bChecked = false;
		rats.bChecked = false;
		santorini.bChecked = true;
		skyscraper.bChecked = false;
		torn.bChecked = false;
		darksnow.bChecked = false;

		MapSetAztec.Hide();
		MapSetCbble.Hide();
		MapSetChateau.Hide();
		MapSetProdigy.Hide();
		MapSetRats.Hide();
		MapSetSantorini.Show();
		MapSetSkyScraper.Hide();
		MapSetTorn.Hide();
		MapSetDarkSnow.Hide();	
		MapSetNightMare.Hide();
	}
	else if (value == 25)//skyscraper
	{
		ischecksetmap = true;

		aztec.bChecked = false;
		cbble.bChecked = false;
		chateau.bChecked = false;
		prodigy.bChecked = false;
		rats.bChecked = false;
		santorini.bChecked = false;
		skyscraper.bChecked = true;
		torn.bChecked = false;
		darksnow.bChecked = false;

		MapSetAztec.Hide();
		MapSetCbble.Hide();
		MapSetChateau.Hide();
		MapSetProdigy.Hide();
		MapSetRats.Hide();
		MapSetSantorini.Hide();
		MapSetSkyScraper.Show();
		MapSetTorn.Hide();
		MapSetDarkSnow.Hide();	
		MapSetNightMare.Hide();
	}
	else if (value == 26)//torn
	{
		ischecksetmap = true;

		aztec.bChecked = false;
		cbble.bChecked = false;
		chateau.bChecked = false;
		prodigy.bChecked = false;
		rats.bChecked = false;
		santorini.bChecked = false;
		skyscraper.bChecked = false;
		torn.bChecked = true;
		darksnow.bChecked = false;

		MapSetAztec.Hide();
		MapSetCbble.Hide();
		MapSetChateau.Hide();
		MapSetProdigy.Hide();
		MapSetRats.Hide();
		MapSetSantorini.Hide();
		MapSetSkyScraper.Hide();
		MapSetTorn.Show();
		MapSetDarkSnow.Hide();		
		MapSetNightMare.Hide();
	}
	else if (value == 27)//darksnow
	{
		ischecksetmap = true;

		aztec.bChecked = false;
		cbble.bChecked = false;
		chateau.bChecked = false;
		prodigy.bChecked = false;
		rats.bChecked = false;
		santorini.bChecked = false;
		skyscraper.bChecked = false;
		torn.bChecked = false;
		darksnow.bChecked = true;

		MapSetAztec.Hide();
		MapSetCbble.Hide();
		MapSetChateau.Hide();
		MapSetProdigy.Hide();
		MapSetRats.Hide();
		MapSetSantorini.Hide();
		MapSetSkyScraper.Hide();
		MapSetTorn.Hide();
		MapSetDarkSnow.Show();
		MapSetNightMare.Hide();
	}
	else if (value == 28)//nightmare
	{
		ischecksetmap = true;

		aztec.bChecked = false;
		cbble.bChecked = false;
		chateau.bChecked = false;
		prodigy.bChecked = false;
		rats.bChecked = false;
		santorini.bChecked = false;
		skyscraper.bChecked = false;
		torn.bChecked = false;
		darksnow.bChecked = false;
		nightmare.bChecked = true;

		nightmare2.bChecked = false;
		nightmare3.bChecked = false;
		lastclue.bChecked = false;
		lostcity.bChecked = false;
		panic.bChecked = false;
		trap.bChecked = false;

		MapSetAztec.Hide();
		MapSetCbble.Hide();
		MapSetChateau.Hide();
		MapSetProdigy.Hide();
		MapSetRats.Hide();
		MapSetSantorini.Hide();
		MapSetSkyScraper.Hide();
		MapSetTorn.Hide();
		MapSetDarkSnow.Hide();
		MapSetNightMare.Show();

		MapSetNightMare2.Hide();
		MapSetNightMare3.Hide();
		MapSetLastClue.Hide();
		MapSetLostCity.Hide();
		MapSetPanic.Hide();
		MapSetTrap.Hide();
	}

	else if (value == 29)//nightmare2
	{
		ischecksetmap = true;

		aztec.bChecked = false;
		cbble.bChecked = false;
		chateau.bChecked = false;
		prodigy.bChecked = false;
		rats.bChecked = false;
		santorini.bChecked = false;
		skyscraper.bChecked = false;
		torn.bChecked = false;
		darksnow.bChecked = false;
		nightmare.bChecked = false;

		nightmare2.bChecked = true;
		nightmare3.bChecked = false;
		lastclue.bChecked = false;
		lostcity.bChecked = false;
		panic.bChecked = false;
		trap.bChecked = false;

		MapSetAztec.Hide();
		MapSetCbble.Hide();
		MapSetChateau.Hide();
		MapSetProdigy.Hide();
		MapSetRats.Hide();
		MapSetSantorini.Hide();
		MapSetSkyScraper.Hide();
		MapSetTorn.Hide();
		MapSetDarkSnow.Hide();
		MapSetNightMare.Hide();

		MapSetNightMare2.Show();
		MapSetNightMare3.Hide();
		MapSetLastClue.Hide();
		MapSetLostCity.Hide();
		MapSetPanic.Hide();
		MapSetTrap.Hide();

	}
	else if (value == 30)//nightmare3
	{
		ischecksetmap = true;

		aztec.bChecked = false;
		cbble.bChecked = false;
		chateau.bChecked = false;
		prodigy.bChecked = false;
		rats.bChecked = false;
		santorini.bChecked = false;
		skyscraper.bChecked = false;
		torn.bChecked = false;
		darksnow.bChecked = false;
		nightmare.bChecked = false;

		nightmare2.bChecked = false;
		nightmare3.bChecked = true;
		lastclue.bChecked = false;
		lostcity.bChecked = false;
		panic.bChecked = false;
		trap.bChecked = false;

		MapSetAztec.Hide();
		MapSetCbble.Hide();
		MapSetChateau.Hide();
		MapSetProdigy.Hide();
		MapSetRats.Hide();
		MapSetSantorini.Hide();
		MapSetSkyScraper.Hide();
		MapSetTorn.Hide();
		MapSetDarkSnow.Hide();
		MapSetNightMare.Hide();

		MapSetNightMare2.Hide();
		MapSetNightMare3.Show();
		MapSetLastClue.Hide();
		MapSetLostCity.Hide();
		MapSetPanic.Hide();
		MapSetTrap.Hide();

	}
	else if (value == 31)//lastclue
	{
		ischecksetmap = true;

		aztec.bChecked = false;
		cbble.bChecked = false;
		chateau.bChecked = false;
		prodigy.bChecked = false;
		rats.bChecked = false;
		santorini.bChecked = false;
		skyscraper.bChecked = false;
		torn.bChecked = false;
		darksnow.bChecked = false;
		nightmare.bChecked = false;

		nightmare2.bChecked = false;
		nightmare3.bChecked = false;
		lastclue.bChecked = true;
		lostcity.bChecked = false;
		panic.bChecked = false;
		trap.bChecked = false;

		MapSetAztec.Hide();
		MapSetCbble.Hide();
		MapSetChateau.Hide();
		MapSetProdigy.Hide();
		MapSetRats.Hide();
		MapSetSantorini.Hide();
		MapSetSkyScraper.Hide();
		MapSetTorn.Hide();
		MapSetDarkSnow.Hide();
		MapSetNightMare.Hide();

		MapSetNightMare2.Hide();
		MapSetNightMare3.Hide();
		MapSetLastClue.Show();
		MapSetLostCity.Hide();
		MapSetPanic.Hide();
		MapSetTrap.Hide();
	}
	else if (value == 32)//lostcity
	{
		ischecksetmap = true;

		aztec.bChecked = false;
		cbble.bChecked = false;
		chateau.bChecked = false;
		prodigy.bChecked = false;
		rats.bChecked = false;
		santorini.bChecked = false;
		skyscraper.bChecked = false;
		torn.bChecked = false;
		darksnow.bChecked = false;
		nightmare.bChecked = false;

		nightmare2.bChecked = false;
		nightmare3.bChecked = false;
		lastclue.bChecked = false;
		lostcity.bChecked = true;
		panic.bChecked = false;
		trap.bChecked = false;

		MapSetAztec.Hide();
		MapSetCbble.Hide();
		MapSetChateau.Hide();
		MapSetProdigy.Hide();
		MapSetRats.Hide();
		MapSetSantorini.Hide();
		MapSetSkyScraper.Hide();
		MapSetTorn.Hide();
		MapSetDarkSnow.Hide();
		MapSetNightMare.Hide();

		MapSetNightMare2.Hide();
		MapSetNightMare3.Hide();
		MapSetLastClue.Hide();
		MapSetLostCity.Show();
		MapSetPanic.Hide();
		MapSetTrap.Hide();
	}
	else if (value == 33)//panic
	{
		ischecksetmap = true;

		aztec.bChecked = false;
		cbble.bChecked = false;
		chateau.bChecked = false;
		prodigy.bChecked = false;
		rats.bChecked = false;
		santorini.bChecked = false;
		skyscraper.bChecked = false;
		torn.bChecked = false;
		darksnow.bChecked = false;
		nightmare.bChecked = false;

		nightmare2.bChecked = false;
		nightmare3.bChecked = false;
		lastclue.bChecked = false;
		lostcity.bChecked = false;
		panic.bChecked = true;
		trap.bChecked = false;

		MapSetAztec.Hide();
		MapSetCbble.Hide();
		MapSetChateau.Hide();
		MapSetProdigy.Hide();
		MapSetRats.Hide();
		MapSetSantorini.Hide();
		MapSetSkyScraper.Hide();
		MapSetTorn.Hide();
		MapSetDarkSnow.Hide();
		MapSetNightMare.Hide();

		MapSetNightMare2.Hide();
		MapSetNightMare3.Hide();
		MapSetLastClue.Hide();
		MapSetLostCity.Hide();
		MapSetPanic.Show();
		MapSetTrap.Hide();
	}
	else if (value == 34)//trap
	{
		ischecksetmap = true;

		aztec.bChecked = false;
		cbble.bChecked = false;
		chateau.bChecked = false;
		prodigy.bChecked = false;
		rats.bChecked = false;
		santorini.bChecked = false;
		skyscraper.bChecked = false;
		torn.bChecked = false;
		darksnow.bChecked = false;
		nightmare.bChecked = false;

		nightmare2.bChecked = false;
		nightmare3.bChecked = false;
		lastclue.bChecked = false;
		lostcity.bChecked = false;
		panic.bChecked = false;
		trap.bChecked = true;

		MapSetAztec.Hide();
		MapSetCbble.Hide();
		MapSetChateau.Hide();
		MapSetProdigy.Hide();
		MapSetRats.Hide();
		MapSetSantorini.Hide();
		MapSetSkyScraper.Hide();
		MapSetTorn.Hide();
		MapSetDarkSnow.Hide();
		MapSetNightMare.Hide();

		MapSetNightMare2.Hide();
		MapSetNightMare3.Hide();
		MapSetLastClue.Hide();
		MapSetLostCity.Hide();
		MapSetPanic.Hide();
		MapSetTrap.Show();
	}

	else
	{
		if (stringmaplist == 1)
		{
			textmap.SetNameAndStatus(L("Assault"), L(""));
			textmap2.SetNameAndStatus(L("Italy"), L(""));
			textmap3.SetNameAndStatus(L("Vertigo"), L(""));

			textmap4.SetNameAndStatus(L("Inferno"), L(""));
			textmap5.SetNameAndStatus(L("Nuke"), L(""));
			textmap6.SetNameAndStatus(L("Dust II"), L(""));

			textmap7.SetNameAndStatus(L("Mirage"), L(""));
			textmap8.SetNameAndStatus(L("Greesia"), L(""));
			textmap9.SetNameAndStatus(L("Ruin"), L(""));

			textmap.Show();
			textmap2.Show();
			textmap3.Show();
			textmap4.Show();
			textmap5.Show();
			textmap6.Show();
			textmap7.Show();
			textmap8.Show();
			textmap9.Show();

			textmap10.Hide();
			textmap11.Hide();
			textmap12.Hide();
			textmap13.Hide();
			textmap14.Hide();
			textmap15.Hide();
			textmap16.Hide();
			textmap17.Hide();
			textmap18.Hide();

			textmap19.Hide();
			textmap20.Hide();
			textmap21.Hide();
			textmap22.Hide();
			textmap23.Hide();
			textmap24.Hide();
			textmap25.Hide();
			textmap26.Hide();
			textmap27.Hide();

			mapassault.Show();
			mapitaly.Show();
			mapvertigo.Show();
			mapinferno.Show();
			mapnuke.Show();
			mapdust.Show();
			mapmirage.Show();
			mapgressia.Show();
			mapruin.Show();

			assault.Show();
			vertigo.Show();
			italy.Show();
			nuke.Show();
			dust2.Show();
			inferno.Show();
			mirage.Show();
			gressia.Show();
			ruin.Show();

			bigtree.Hide();
			dustmini.Hide();
			cs747.Hide();
			estate.Hide();
			havana.Hide();
			militia.Hide();
			office.Hide();
			siege.Hide();
			angelcity.Hide();

			mapbigtree.Hide();
			mapdustmini.Hide();
			mapcs747.Hide();
			mapestate.Hide();
			maphavana.Hide();
			mapmilitia.Hide();
			mapoffice.Hide();
			mapsiege.Hide();
			mapangelcity.Hide();

			aztec.Hide();
			cbble.Hide();
			chateau.Hide();
			prodigy.Hide();
			rats.Hide();
			santorini.Hide();
			skyscraper.Hide();
			torn.Hide();
			darksnow.Hide();

			mapaztec.Hide();
			mapcbble.Hide();
			mapchateau.Hide();
			mapprodigy.Hide();
			maprats.Hide();
			mapsantorini.Hide();
			mapskyscraper.Hide();
			maptorn.Hide();
			mapdarksnow.Hide();

			mapnightmare.Hide();
			MapSetNightMare.Hide();
			nightmare.Hide();
		}
		else if (stringmaplist == 2)
		{
			textmap10.SetNameAndStatus(L("BigTree"), L(""));
			textmap11.SetNameAndStatus(L("Dust EX"), L(""));
			textmap12.SetNameAndStatus(L("747"), L(""));

			textmap13.SetNameAndStatus(L("Estate"), L(""));
			textmap14.SetNameAndStatus(L("Havana"), L(""));
			textmap15.SetNameAndStatus(L("Militia"), L(""));

			textmap16.SetNameAndStatus(L("Office"), L(""));
			textmap17.SetNameAndStatus(L("Siege"), L(""));
			textmap18.SetNameAndStatus(L("Angel City"), L(""));

			textmap.Hide();
			textmap2.Hide();
			textmap3.Hide();
			textmap4.Hide();
			textmap5.Hide();
			textmap6.Hide();
			textmap7.Hide();
			textmap8.Hide();
			textmap9.Hide();

			textmap10.Show();
			textmap11.Show();
			textmap12.Show();
			textmap13.Show();
			textmap14.Show();
			textmap15.Show();
			textmap16.Show();
			textmap17.Show();
			textmap18.Show();

			textmap19.Hide();
			textmap20.Hide();
			textmap21.Hide();
			textmap22.Hide();
			textmap23.Hide();
			textmap24.Hide();
			textmap25.Hide();
			textmap26.Hide();
			textmap27.Hide();

			mapassault.Hide();
			mapitaly.Hide();
			mapvertigo.Hide();
			mapinferno.Hide();
			mapnuke.Hide();
			mapdust.Hide();
			mapmirage.Hide();
			mapgressia.Hide();
			mapruin.Hide();

			assault.Hide();
			vertigo.Hide();
			italy.Hide();
			nuke.Hide();
			dust2.Hide();
			inferno.Hide();
			mirage.Hide();
			gressia.Hide();
			ruin.Hide();

			bigtree.Show();
			dustmini.Show();
			cs747.Show();
			estate.Show();
			havana.Show();
			militia.Show();
			office.Show();
			siege.Show();
			angelcity.Show();

			mapbigtree.Show();
			mapdustmini.Show();
			mapcs747.Show();
			mapestate.Show();
			maphavana.Show();
			mapmilitia.Show();
			mapoffice.Show();
			mapsiege.Show();
			mapangelcity.Show();

			aztec.Hide();
			cbble.Hide();
			chateau.Hide();
			prodigy.Hide();
			rats.Hide();
			santorini.Hide();
			skyscraper.Hide();
			torn.Hide();
			darksnow.Hide();

			mapaztec.Hide();
			mapcbble.Hide();
			mapchateau.Hide();
			mapprodigy.Hide();
			maprats.Hide();
			mapsantorini.Hide();
			mapskyscraper.Hide();
			maptorn.Hide();
			mapdarksnow.Hide();

			mapnightmare.Hide();
			MapSetNightMare.Hide();
			nightmare.Hide();
		}
		else if (stringmaplist == 3)
		{
			textmap19.SetNameAndStatus(L("Aztec"), L(""));
			textmap20.SetNameAndStatus(L("CobbleStone"), L(""));
			textmap21.SetNameAndStatus(L("ChateAu"), L(""));

			textmap22.SetNameAndStatus(L("Prodigy"), L(""));
			textmap23.SetNameAndStatus(L("Rats"), L(""));
			textmap24.SetNameAndStatus(L("Santorini"), L(""));

			textmap25.SetNameAndStatus(L("SkyScraper"), L(""));
			textmap26.SetNameAndStatus(L("Torn"), L(""));
			textmap27.SetNameAndStatus(L("DarkSnow"), L(""));

			textmap.Hide();
			textmap2.Hide();
			textmap3.Hide();
			textmap4.Hide();
			textmap5.Hide();
			textmap6.Hide();
			textmap7.Hide();
			textmap8.Hide();
			textmap9.Hide();

			textmap10.Hide();
			textmap11.Hide();
			textmap12.Hide();
			textmap13.Hide();
			textmap14.Hide();
			textmap15.Hide();
			textmap16.Hide();
			textmap17.Hide();
			textmap18.Hide();

			textmap19.Show();
			textmap20.Show();
			textmap21.Show();
			textmap22.Show();
			textmap23.Show();
			textmap24.Show();
			textmap25.Show();
			textmap26.Show();
			textmap27.Show();

			mapassault.Hide();
			mapitaly.Hide();
			mapvertigo.Hide();
			mapinferno.Hide();
			mapnuke.Hide();
			mapdust.Hide();
			mapmirage.Hide();
			mapgressia.Hide();
			mapruin.Hide();

			assault.Hide();
			vertigo.Hide();
			italy.Hide();
			nuke.Hide();
			dust2.Hide();
			inferno.Hide();
			mirage.Hide();
			gressia.Hide();
			ruin.Hide();



			bigtree.Hide();
			dustmini.Hide();
			cs747.Hide();
			estate.Hide();
			havana.Hide();
			militia.Hide();
			office.Hide();
			siege.Hide();
			angelcity.Hide();

			mapbigtree.Hide();
			mapdustmini.Hide();
			mapcs747.Hide();
			mapestate.Hide();
			maphavana.Hide();
			mapmilitia.Hide();
			mapoffice.Hide();
			mapsiege.Hide();
			mapangelcity.Hide();

			aztec.Show();
			cbble.Show();
			chateau.Show();
			prodigy.Show();
			rats.Show();
			santorini.Show();
			skyscraper.Show();
			torn.Show();
			darksnow.Show();

			mapaztec.Show();
			mapcbble.Show();
			mapchateau.Show();
			mapprodigy.Show();
			maprats.Show();
			mapsantorini.Show();
			mapskyscraper.Show();
			maptorn.Show();
			mapdarksnow.Show();

			mapnightmare.Hide();
			MapSetNightMare.Hide();
			nightmare.Hide();

			mapnightmare2.Hide();
			MapSetNightMare2.Hide();
			nightmare2.Hide();

			mapnightmare3.Hide();
			MapSetNightMare3.Hide();
			nightmare3.Hide();

			maplastclue.Hide();
			MapSetLastClue.Hide();
			lastclue.Hide();

			maplostcity.Hide();
			MapSetLostCity.Hide();
			lostcity.Hide();

			mappanic.Hide();
			MapSetPanic.Hide();
			panic.Hide();

			maptrap.Hide();
			MapSetTrap.Hide();
			trap.Hide();
		}

		botNum.Show();

		ischecksetmap = false;
		assault.bChecked = false;
		vertigo.bChecked = false;
		italy.bChecked = false;
		nuke.bChecked = false;
		dust2.bChecked = false;
		inferno.bChecked = false;
		mirage.bChecked = false;
		gressia.bChecked = false;
		ruin.bChecked = false;

		bigtree.bChecked = false;
		dustmini.bChecked = false;
		cs747.bChecked = false;
		estate.bChecked = false;
		havana.bChecked = false;
		militia.bChecked = false;
		office.bChecked = false;
		siege.bChecked = false;
		angelcity.bChecked = false;

		aztec.bChecked = false;
		cbble.bChecked = false;
		chateau.bChecked = false;
		prodigy.bChecked = false;
		rats.bChecked = false;
		santorini.bChecked = false;
		skyscraper.bChecked = false;
		torn.bChecked = false;
		darksnow.bChecked = false;

		MapSetAztec.Hide();
		MapSetCbble.Hide();
		MapSetChateau.Hide();
		MapSetProdigy.Hide();
		MapSetRats.Hide();
		MapSetSantorini.Hide();
		MapSetSkyScraper.Hide();
		MapSetTorn.Hide();
		MapSetDarkSnow.Hide();

		MapSetBigTree.Hide();
		MapSetDustMini.Hide();
		MapSet747.Hide();
		MapSetEstate.Hide();
		MapSetHavana.Hide();
		MapSetMilitia.Hide();
		MapSetOffice.Hide();
		MapSetSiege.Hide();
		MapSetAngelCity.Hide();
		MapSetAssault.Hide();
		MapSetItaly.Hide();
		MapSetVertigo.Hide();
		MapSetInferno.Hide();
		MapSetNuke.Hide();
		MapSetDust.Hide();
		MapSetMirage.Hide();
		MapSetGressia.Hide();
		MapSetRuin.Hide();
	}
}

/*
=================
CMenuCreateGame::Init
=================
*/
void CMenuCreateGame::_Init( void )
{
	ResetMap(0);
	ResetMode(0);
	stringmaplist = 1;

	static const char* g_szGameModeNames[MAX_GAMEMODES] =
	{
		L("CstzUI_Mod_classic"),
		L("CstzUI_Mod_dm"),
		L("CstzUI_Mod_tdm"),
		L("CstzUI_Mod_gdm"),
		L("CstzUI_Mod_zbm"),
		L("CstzUI_Mod_zbh"),
		L("CstzUI_Mod_scen"),
		L("CstzUI_Mod_zsh")
	};

	uiStatic.needMapListUpdate = true;
	banner.SetPicture( ART_BANNER );

	nat.SetNameAndStatus( "NAT", "Use NAT Bypass instead of direct mode" );
	nat.bChecked = true;

	hltv.SetNameAndStatus( "HLTV", "Enable HLTV mode in Multiplayer" );
	hltv.LinkCvar( "hltv" );

	nomap.SetMessage(L("CstzUI_NoMap"));
	nomap.Link(this);

	nomode.SetMessage(L("CstzUI_NoMode"));
	nomode.Link(this);

	AddItem( background );

	mapassault.iFlags = QMF_NOTIFY;
	mapassault.SetRect(160, 225, 256, 124);
	mapassault.SetPicture(ART_ASSAULT);
	mapassault.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(mapassault.onActivated,
		{
			uiCreateGame.ResetMap(1);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	MapSetAssault.SetCharSize(QM_SMALLFONT);
	MapSetAssault.SetModel(&mapsListModel);
	MapSetAssault.SetRect(160, 193, 252, 154);

	assault.SetNameAndStatus(L(""), L(""));
	assault.iFlags |= QMF_NOTIFY;
	assault.SetCoord(380, 230);
	SET_EVENT_MULTI(assault.onChanged,
		{
			uiCreateGame.ResetMap(1);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	mapitaly.iFlags = QMF_NOTIFY;
	mapitaly.SetRect(460, 225, 256, 124);
	mapitaly.SetPicture(ART_ITALY);
	mapitaly.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(mapitaly.onActivated,
		{
			uiCreateGame.ResetMap(2);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	MapSetItaly.SetCharSize(QM_SMALLFONT);
	MapSetItaly.SetModel(&mapsListModel);
	MapSetItaly.SetRect(460, 193, 252, 154);

	italy.SetNameAndStatus(L(""), L(""));
	italy.iFlags |= QMF_NOTIFY;
	italy.SetCoord(680, 230);
	SET_EVENT_MULTI(italy.onChanged,
		{
			uiCreateGame.ResetMap(2);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});
	
	mapvertigo.iFlags = QMF_NOTIFY;
	mapvertigo.SetRect(760, 225, 256, 124);
	mapvertigo.SetPicture(ART_VERTIGO);
	mapvertigo.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(mapvertigo.onActivated,
		{
			uiCreateGame.ResetMap(3);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	MapSetVertigo.SetCharSize(QM_SMALLFONT);
	MapSetVertigo.SetModel(&mapsListModel);
	MapSetVertigo.SetRect(760, 193, 252, 154);

	vertigo.SetNameAndStatus(L(""), L(""));
	vertigo.iFlags |= QMF_NOTIFY;
	vertigo.SetCoord(980, 230);
	SET_EVENT_MULTI(vertigo.onChanged,
		{
			uiCreateGame.ResetMap(3);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	mapinferno.iFlags = QMF_NOTIFY;
	mapinferno.SetRect(160, 365, 256, 124);
	mapinferno.SetPicture(ART_INFERNO);
	mapinferno.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(mapinferno.onActivated,
		{
			uiCreateGame.ResetMap(4);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	MapSetInferno.SetCharSize(QM_SMALLFONT);
	MapSetInferno.SetModel(&mapsListModel);
	MapSetInferno.SetRect(160, 333, 252, 154);

	inferno.SetNameAndStatus(L(""), L(""));
	inferno.iFlags |= QMF_NOTIFY;
	inferno.SetCoord(380, 370);
	SET_EVENT_MULTI(inferno.onChanged,
		{
			uiCreateGame.ResetMap(4);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	mapnuke.iFlags = QMF_NOTIFY;
	mapnuke.SetRect(460, 365, 256, 124);
	mapnuke.SetPicture(ART_NUKE);
	mapnuke.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(mapnuke.onActivated,
		{
			uiCreateGame.ResetMap(5);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	MapSetNuke.SetCharSize(QM_SMALLFONT);
	MapSetNuke.SetModel(&mapsListModel);
	MapSetNuke.SetRect(460, 333, 252, 154);

	nuke.SetNameAndStatus(L(""), L(""));
	nuke.iFlags |= QMF_NOTIFY;
	nuke.SetCoord(680, 370);
	SET_EVENT_MULTI(nuke.onChanged,
		{
			uiCreateGame.ResetMap(5);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	mapdust.iFlags = QMF_NOTIFY;
	mapdust.SetRect(760, 365, 256, 124);
	mapdust.SetPicture(ART_DUST);
	mapdust.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(mapdust.onActivated,
		{
			uiCreateGame.ResetMap(6);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	MapSetDust.SetCharSize(QM_SMALLFONT);
	MapSetDust.SetModel(&mapsListModel);
	MapSetDust.SetRect(760, 333, 252, 154);

	dust2.SetNameAndStatus(L(""), L(""));
	dust2.iFlags |= QMF_NOTIFY;
	dust2.SetCoord(980, 370);
	SET_EVENT_MULTI(dust2.onChanged,
		{
			uiCreateGame.ResetMap(6);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	mapmirage.iFlags = QMF_NOTIFY;
	mapmirage.SetRect(156, 504, 266, 128);//defoult 160,505
	mapmirage.SetPicture(ART_MIRAGE);
	mapmirage.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(mapmirage.onActivated,
		{
			uiCreateGame.ResetMap(7);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	MapSetMirage.SetCharSize(QM_SMALLFONT);
	MapSetMirage.SetModel(&mapsListModel);
	MapSetMirage.SetRect(160, 473, 252, 154);

	mirage.SetNameAndStatus(L(""), L(""));
	mirage.iFlags |= QMF_NOTIFY;
	mirage.SetCoord(380, 510);
	SET_EVENT_MULTI(mirage.onChanged,
		{
			uiCreateGame.ResetMap(7);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	mapgressia.iFlags = QMF_NOTIFY;
	mapgressia.SetRect(460, 505, 256, 124);//defoult 160,505
	mapgressia.SetPicture(ART_GREESIA);
	mapgressia.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(mapgressia.onActivated,
		{
			uiCreateGame.ResetMap(8);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	MapSetGressia.SetCharSize(QM_SMALLFONT);
	MapSetGressia.SetModel(&mapsListModel);
	MapSetGressia.SetRect(460, 473, 252, 154);

	gressia.SetNameAndStatus(L(""), L(""));
	gressia.iFlags |= QMF_NOTIFY;
	gressia.SetCoord(680, 510);
	SET_EVENT_MULTI(gressia.onChanged,
		{
			uiCreateGame.ResetMap(8);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});


	mapruin.iFlags = QMF_NOTIFY;
	mapruin.SetRect(760, 505, 256, 124);//defoult 160,505
	mapruin.SetPicture(ART_RUIN);
	mapruin.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(mapruin.onActivated,
		{
			uiCreateGame.ResetMap(9);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	MapSetRuin.SetCharSize(QM_SMALLFONT);
	MapSetRuin.SetModel(&mapsListModel);
	MapSetRuin.SetRect(760, 473, 252, 154);

	ruin.SetNameAndStatus(L(""), L(""));
	ruin.iFlags |= QMF_NOTIFY;
	ruin.SetCoord(980, 510);
	SET_EVENT_MULTI(ruin.onChanged,
		{
			uiCreateGame.ResetMap(9);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	mapbigtree.iFlags = QMF_NOTIFY;
	mapbigtree.SetRect(160, 225, 256, 124);
	mapbigtree.SetPicture(ART_BIGTREE);
	mapbigtree.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(mapbigtree.onActivated,
		{
			uiCreateGame.ResetMap(10);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	MapSetBigTree.SetCharSize(QM_SMALLFONT);
	MapSetBigTree.SetModel(&mapsListModel);
	MapSetBigTree.SetRect(160, 193, 252, 154);

	bigtree.SetNameAndStatus(L(""), L(""));
	bigtree.iFlags |= QMF_NOTIFY;
	bigtree.SetCoord(380, 230);
	SET_EVENT_MULTI(bigtree.onChanged,
		{
			uiCreateGame.ResetMap(10);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	mapdustmini.iFlags = QMF_NOTIFY;
	mapdustmini.SetRect(460, 225, 256, 124);
	mapdustmini.SetPicture(ART_DUSTMINI);
	mapdustmini.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(mapdustmini.onActivated,
		{
			uiCreateGame.ResetMap(11);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	MapSetDustMini.SetCharSize(QM_SMALLFONT);
	MapSetDustMini.SetModel(&mapsListModel);
	MapSetDustMini.SetRect(460, 193, 252, 154);

	dustmini.SetNameAndStatus(L(""), L(""));
	dustmini.iFlags |= QMF_NOTIFY;
	dustmini.SetCoord(680, 230);
	SET_EVENT_MULTI(dustmini.onChanged,
		{
			uiCreateGame.ResetMap(11);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	mapcs747.iFlags = QMF_NOTIFY;
	mapcs747.SetRect(760, 225, 256, 124);
	mapcs747.SetPicture(ART_747);
	mapcs747.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(mapcs747.onActivated,
		{
			uiCreateGame.ResetMap(12);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	MapSet747.SetCharSize(QM_SMALLFONT);
	MapSet747.SetModel(&mapsListModel);
	MapSet747.SetRect(760, 193, 252, 154);

	cs747.SetNameAndStatus(L(""), L(""));
	cs747.iFlags |= QMF_NOTIFY;
	cs747.SetCoord(980, 230);
	SET_EVENT_MULTI(cs747.onChanged,
		{
			uiCreateGame.ResetMap(12);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	mapestate.iFlags = QMF_NOTIFY;
	mapestate.SetRect(160, 365, 256, 124);
	mapestate.SetPicture(ART_ESTATE);
	mapestate.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(mapestate.onActivated,
		{
			uiCreateGame.ResetMap(13);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	MapSetEstate.SetCharSize(QM_SMALLFONT);
	MapSetEstate.SetModel(&mapsListModel);
	MapSetEstate.SetRect(160, 333, 252, 154);

	estate.SetNameAndStatus(L(""), L(""));
	estate.iFlags |= QMF_NOTIFY;
	estate.SetCoord(380, 370);
	SET_EVENT_MULTI(estate.onChanged,
		{
			uiCreateGame.ResetMap(13);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	maphavana.iFlags = QMF_NOTIFY;
	maphavana.SetRect(460, 365, 256, 124);
	maphavana.SetPicture(ART_HAVANA);
	maphavana.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(maphavana.onActivated,
		{
			uiCreateGame.ResetMap(14);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	MapSetHavana.SetCharSize(QM_SMALLFONT);
	MapSetHavana.SetModel(&mapsListModel);
	MapSetHavana.SetRect(460, 333, 252, 154);

	havana.SetNameAndStatus(L(""), L(""));
	havana.iFlags |= QMF_NOTIFY;
	havana.SetCoord(680, 370);
	SET_EVENT_MULTI(havana.onChanged,
		{
			uiCreateGame.ResetMap(14);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	mapmilitia.iFlags = QMF_NOTIFY;
	mapmilitia.SetRect(760, 365, 256, 124);
	mapmilitia.SetPicture(ART_MILITIA);
	mapmilitia.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(mapmilitia.onActivated,
		{
			uiCreateGame.ResetMap(15);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	MapSetMilitia.SetCharSize(QM_SMALLFONT);
	MapSetMilitia.SetModel(&mapsListModel);
	MapSetMilitia.SetRect(760, 333, 252, 154);

	militia.SetNameAndStatus(L(""), L(""));
	militia.iFlags |= QMF_NOTIFY;
	militia.SetCoord(980, 370);
	SET_EVENT_MULTI(militia.onChanged,
		{
			uiCreateGame.ResetMap(15);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	mapoffice.iFlags = QMF_NOTIFY;
	mapoffice.SetRect(156, 504, 266, 128);//defoult 160,505
	mapoffice.SetPicture(ART_OFFICE);
	mapoffice.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(mapoffice.onActivated,
		{
			uiCreateGame.ResetMap(16);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	MapSetOffice.SetCharSize(QM_SMALLFONT);
	MapSetOffice.SetModel(&mapsListModel);
	MapSetOffice.SetRect(160, 473, 252, 154);

	office.SetNameAndStatus(L(""), L(""));
	office.iFlags |= QMF_NOTIFY;
	office.SetCoord(380, 510);
	SET_EVENT_MULTI(office.onChanged,
		{
			uiCreateGame.ResetMap(16);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	mapsiege.iFlags = QMF_NOTIFY;
	mapsiege.SetRect(460, 505, 256, 124);//defoult 160,505
	mapsiege.SetPicture(ART_SIEGE);
	mapsiege.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(mapsiege.onActivated,
		{
			uiCreateGame.ResetMap(17);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	MapSetSiege.SetCharSize(QM_SMALLFONT);
	MapSetSiege.SetModel(&mapsListModel);
	MapSetSiege.SetRect(460, 473, 252, 154);

	siege.SetNameAndStatus(L(""), L(""));
	siege.iFlags |= QMF_NOTIFY;
	siege.SetCoord(680, 510);
	SET_EVENT_MULTI(siege.onChanged,
		{
			uiCreateGame.ResetMap(17);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	mapangelcity.iFlags = QMF_NOTIFY;
	mapangelcity.SetRect(760, 505, 256, 124);//defoult 160,505
	mapangelcity.SetPicture(ART_ANGELCITY);
	mapangelcity.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(mapangelcity.onActivated,
		{
			uiCreateGame.ResetMap(18);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	MapSetAngelCity.SetCharSize(QM_SMALLFONT);
	MapSetAngelCity.SetModel(&mapsListModel);
	MapSetAngelCity.SetRect(760, 473, 252, 154);

	angelcity.SetNameAndStatus(L(""), L(""));
	angelcity.iFlags |= QMF_NOTIFY;
	angelcity.SetCoord(980, 510);
	SET_EVENT_MULTI(angelcity.onChanged,
		{
			uiCreateGame.ResetMap(18);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	mapaztec.iFlags = QMF_NOTIFY;
	mapaztec.SetRect(160, 225, 256, 124);
	mapaztec.SetPicture(ART_AZTEC);
	mapaztec.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(mapaztec.onActivated,
		{
			uiCreateGame.ResetMap(19);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	MapSetAztec.SetCharSize(QM_SMALLFONT);
	MapSetAztec.SetModel(&mapsListModel);
	MapSetAztec.SetRect(160, 193, 252, 154);

	aztec.SetNameAndStatus(L(""), L(""));
	aztec.iFlags |= QMF_NOTIFY;
	aztec.SetCoord(380, 230);
	SET_EVENT_MULTI(aztec.onChanged,
		{
			uiCreateGame.ResetMap(19);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	mapcbble.iFlags = QMF_NOTIFY;
	mapcbble.SetRect(460, 225, 256, 124);
	mapcbble.SetPicture(ART_CBBLE);
	mapcbble.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(mapcbble.onActivated,
		{
			uiCreateGame.ResetMap(20);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	MapSetCbble.SetCharSize(QM_SMALLFONT);
	MapSetCbble.SetModel(&mapsListModel);
	MapSetCbble.SetRect(460, 193, 252, 154);

	cbble.SetNameAndStatus(L(""), L(""));
	cbble.iFlags |= QMF_NOTIFY;
	cbble.SetCoord(680, 230);
	SET_EVENT_MULTI(cbble.onChanged,
		{
			uiCreateGame.ResetMap(20);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	mapchateau.iFlags = QMF_NOTIFY;
	mapchateau.SetRect(760, 225, 256, 124);
	mapchateau.SetPicture(ART_CHATEAU);
	mapchateau.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(mapchateau.onActivated,
		{
			uiCreateGame.ResetMap(21);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	MapSetChateau.SetCharSize(QM_SMALLFONT);
	MapSetChateau.SetModel(&mapsListModel);
	MapSetChateau.SetRect(760, 193, 252, 154);

	chateau.SetNameAndStatus(L(""), L(""));
	chateau.iFlags |= QMF_NOTIFY;
	chateau.SetCoord(980, 230);
	SET_EVENT_MULTI(chateau.onChanged,
		{
			uiCreateGame.ResetMap(21);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	mapprodigy.iFlags = QMF_NOTIFY;
	mapprodigy.SetRect(160, 365, 256, 124);
	mapprodigy.SetPicture(ART_PRODIGY);
	mapprodigy.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(mapprodigy.onActivated,
		{
			uiCreateGame.ResetMap(22);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	MapSetProdigy.SetCharSize(QM_SMALLFONT);
	MapSetProdigy.SetModel(&mapsListModel);
	MapSetProdigy.SetRect(160, 333, 252, 154);

	prodigy.SetNameAndStatus(L(""), L(""));
	prodigy.iFlags |= QMF_NOTIFY;
	prodigy.SetCoord(380, 370);
	SET_EVENT_MULTI(prodigy.onChanged,
		{
			uiCreateGame.ResetMap(22);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	maprats.iFlags = QMF_NOTIFY;
	maprats.SetRect(460, 365, 256, 124);
	maprats.SetPicture(ART_RATS);
	maprats.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(maprats.onActivated,
		{
			uiCreateGame.ResetMap(23);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	MapSetRats.SetCharSize(QM_SMALLFONT);
	MapSetRats.SetModel(&mapsListModel);
	MapSetRats.SetRect(460, 333, 252, 154);

	rats.SetNameAndStatus(L(""), L(""));
	rats.iFlags |= QMF_NOTIFY;
	rats.SetCoord(680, 370);
	SET_EVENT_MULTI(rats.onChanged,
		{
			uiCreateGame.ResetMap(23);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	mapsantorini.iFlags = QMF_NOTIFY;
	mapsantorini.SetRect(760, 365, 256, 124);
	mapsantorini.SetPicture(ART_SANTORINI);
	mapsantorini.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(mapsantorini.onActivated,
		{
			uiCreateGame.ResetMap(24);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	MapSetSantorini.SetCharSize(QM_SMALLFONT);
	MapSetSantorini.SetModel(&mapsListModel);
	MapSetSantorini.SetRect(760, 333, 252, 154);

	santorini.SetNameAndStatus(L(""), L(""));
	santorini.iFlags |= QMF_NOTIFY;
	santorini.SetCoord(980, 370);
	SET_EVENT_MULTI(santorini.onChanged,
		{
			uiCreateGame.ResetMap(24);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	mapskyscraper.iFlags = QMF_NOTIFY;
	mapskyscraper.SetRect(156, 504, 266, 128);//defoult 160,505
	mapskyscraper.SetPicture(ART_SKYSCRAPER);
	mapskyscraper.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(mapskyscraper.onActivated,
		{
			uiCreateGame.ResetMap(25);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	MapSetSkyScraper.SetCharSize(QM_SMALLFONT);
	MapSetSkyScraper.SetModel(&mapsListModel);
	MapSetSkyScraper.SetRect(160, 473, 252, 154);

	skyscraper.SetNameAndStatus(L(""), L(""));
	skyscraper.iFlags |= QMF_NOTIFY;
	skyscraper.SetCoord(380, 510);
	SET_EVENT_MULTI(skyscraper.onChanged,
		{
			uiCreateGame.ResetMap(25);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	maptorn.iFlags = QMF_NOTIFY;
	maptorn.SetRect(460, 505, 256, 124);//defoult 160,505
	maptorn.SetPicture(ART_TORN);
	maptorn.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(maptorn.onActivated,
		{
			uiCreateGame.ResetMap(26);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	MapSetTorn.SetCharSize(QM_SMALLFONT);
	MapSetTorn.SetModel(&mapsListModel);
	MapSetTorn.SetRect(460, 473, 252, 154);

	torn.SetNameAndStatus(L(""), L(""));
	torn.iFlags |= QMF_NOTIFY;
	torn.SetCoord(680, 510);
	SET_EVENT_MULTI(torn.onChanged,
		{
			uiCreateGame.ResetMap(26);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	mapdarksnow.iFlags = QMF_NOTIFY;
	mapdarksnow.SetRect(760, 505, 256, 124);//defoult 160,505
	mapdarksnow.SetPicture(ART_DARKSNOW);
	mapdarksnow.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(mapdarksnow.onActivated,
		{
			uiCreateGame.ResetMap(27);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	MapSetDarkSnow.SetCharSize(QM_SMALLFONT);
	MapSetDarkSnow.SetModel(&mapsListModel);
	MapSetDarkSnow.SetRect(760, 473, 252, 154);

	darksnow.SetNameAndStatus(L(""), L(""));
	darksnow.iFlags |= QMF_NOTIFY;
	darksnow.SetCoord(980, 510);
	SET_EVENT_MULTI(darksnow.onChanged,
		{
			uiCreateGame.ResetMap(27);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	mapnightmare.iFlags |= QMF_NOTIFY;
	mapnightmare.SetRect(160, 225, 256, 124);
	mapnightmare.SetPicture(ART_NIGHTMARE);
	mapnightmare.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(mapnightmare.onActivated,
		{
			uiCreateGame.ResetMap(28);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	MapSetNightMare.iFlags |= QMF_INACTIVE;
	MapSetNightMare.SetCharSize(QM_SMALLFONT);
	MapSetNightMare.SetModel(&mapsListModel);
	MapSetNightMare.SetRect(160, 193, 252, 154);

	nightmare.SetNameAndStatus(L(""), L(""));
	nightmare.iFlags |= QMF_INACTIVE;
	nightmare.SetCoord(380, 230);
	SET_EVENT_MULTI(nightmare.onChanged,
		{
			uiCreateGame.ResetMap(28);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	mapnightmare2.iFlags = QMF_NOTIFY;
	mapnightmare2.SetRect(460, 225, 256, 124);
	mapnightmare2.SetPicture(ART_NIGHTMARE2);
	mapnightmare2.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(mapnightmare2.onActivated,
		{
			uiCreateGame.ResetMap(29);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	MapSetNightMare2.SetCharSize(QM_SMALLFONT);
	MapSetNightMare2.SetModel(&mapsListModel);
	MapSetNightMare2.SetRect(460, 193, 252, 154);

	nightmare2.SetNameAndStatus(L(""), L(""));
	nightmare2.iFlags |= QMF_NOTIFY;
	nightmare2.SetCoord(680, 230);
	SET_EVENT_MULTI(nightmare3.onChanged,
		{
			uiCreateGame.ResetMap(29);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	mapnightmare3.iFlags = QMF_NOTIFY;
	mapnightmare3.SetRect(760, 225, 256, 124);
	mapnightmare3.SetPicture(ART_NIGHTMARE3);
	mapnightmare3.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(mapnightmare3.onActivated,
		{
			uiCreateGame.ResetMap(30);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	MapSetNightMare3.SetCharSize(QM_SMALLFONT);
	MapSetNightMare3.SetModel(&mapsListModel);
	MapSetNightMare3.SetRect(760, 193, 252, 154);

	nightmare3.SetNameAndStatus(L(""), L(""));
	nightmare3.iFlags |= QMF_NOTIFY;
	nightmare3.SetCoord(980, 230);
	SET_EVENT_MULTI(nightmare3.onChanged,
		{
			uiCreateGame.ResetMap(30);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	maplastclue.iFlags = QMF_NOTIFY;
	maplastclue.SetRect(160, 365, 256, 124);
	maplastclue.SetPicture(ART_LASTCLUE);
	maplastclue.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(maplastclue.onActivated,
		{
			uiCreateGame.ResetMap(31);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	MapSetLastClue.SetCharSize(QM_SMALLFONT);
	MapSetLastClue.SetModel(&mapsListModel);
	MapSetLastClue.SetRect(160, 333, 252, 154);
	
	lastclue.SetNameAndStatus(L(""), L(""));
	lastclue.iFlags |= QMF_NOTIFY;
	lastclue.SetCoord(380, 370);
	SET_EVENT_MULTI(lastclue.onChanged,
		{
			uiCreateGame.ResetMap(31);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	maplostcity.iFlags = QMF_NOTIFY;
	maplostcity.SetRect(460, 365, 256, 124);
	maplostcity.SetPicture(ART_LOSTCITY);
	maplostcity.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(maplostcity.onActivated,
		{
			uiCreateGame.ResetMap(32);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	MapSetLostCity.SetCharSize(QM_SMALLFONT);
	MapSetLostCity.SetModel(&mapsListModel);
	MapSetLostCity.SetRect(460, 333, 252, 154);

	lostcity.SetNameAndStatus(L(""), L(""));
	lostcity.iFlags |= QMF_NOTIFY;
	lostcity.SetCoord(680, 370);
	SET_EVENT_MULTI(lostcity.onChanged,
		{
			uiCreateGame.ResetMap(32);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	mappanic.iFlags = QMF_NOTIFY;
	mappanic.SetRect(760, 365, 256, 124);
	mappanic.SetPicture(ART_PANIC);
	mappanic.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(mappanic.onActivated,
		{
			uiCreateGame.ResetMap(33);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	MapSetPanic.SetCharSize(QM_SMALLFONT);
	MapSetPanic.SetModel(&mapsListModel);
	MapSetPanic.SetRect(760, 333, 252, 154);

	panic.SetNameAndStatus(L(""), L(""));
	panic.iFlags |= QMF_NOTIFY;
	panic.SetCoord(980, 370);
	SET_EVENT_MULTI(panic.onChanged,
		{
			uiCreateGame.ResetMap(33);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	maptrap.iFlags = QMF_NOTIFY;
	maptrap.SetRect(156, 504, 266, 128);//defoult 160,505
	maptrap.SetPicture(ART_TRAP);
	maptrap.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(maptrap.onActivated,
		{
			uiCreateGame.ResetMap(34);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	MapSetTrap.SetCharSize(QM_SMALLFONT);
	MapSetTrap.SetModel(&mapsListModel);
	MapSetTrap.SetRect(160, 473, 252, 154);

	trap.SetNameAndStatus(L(""), L(""));
	trap.iFlags |= QMF_NOTIFY;
	trap.SetCoord(380, 510);
	SET_EVENT_MULTI(trap.onChanged,
		{
			uiCreateGame.ResetMap(34);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});



	classic.SetNameAndStatus(L("CstzUI_Mod_classic"), L(""));
	classic.iFlags |= QMF_NOTIFY;
	classic.SetCoord(1025, 230);
	SET_EVENT_MULTI(classic.onChanged,
		{
			uiCreateGame.botNum.Show();
			uiCreateGame.ResetMode("none");
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	dm.SetNameAndStatus(L("CstzUI_Mod_dm"), L(""));
	dm.iFlags |= QMF_NOTIFY;
	dm.SetCoord(1025, 270);
	SET_EVENT_MULTI(dm.onChanged,
		{
			uiCreateGame.botNum.Show();
			uiCreateGame.ResetMode("dm");
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	tdm.SetNameAndStatus(L("CstzUI_Mod_tdm"), L(""));
	tdm.iFlags |= QMF_NOTIFY;
	tdm.SetCoord(1025, 310);
	SET_EVENT_MULTI(tdm.onChanged,
		{
			uiCreateGame.botNum.Show();
			uiCreateGame.ResetMode("tdm");
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	gd.SetNameAndStatus(L("CstzUI_Mod_gdm"), L(""));
	gd.iFlags |= QMF_NOTIFY;
	gd.SetCoord(1025, 350);
	SET_EVENT_MULTI(gd.onChanged,
		{
			uiCreateGame.botNum.Show();
			uiCreateGame.ResetMode("gd");
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	zc.SetNameAndStatus(L("CstzUI_Mod_zbm"), L(""));
	zc.iFlags |= QMF_NOTIFY;
	zc.SetCoord(1025, 400);
	SET_EVENT_MULTI(zc.onChanged,
		{
			uiCreateGame.botNum.Show();
			uiCreateGame.ResetMode("zb1");
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	zh.SetNameAndStatus(L("CstzUI_Mod_zbh"), L(""));
	zh.iFlags |= QMF_NOTIFY;
	zh.SetCoord(1025, 440);
	SET_EVENT_MULTI(zh.onChanged,
		{
			uiCreateGame.botNum.Show();
			uiCreateGame.ResetMode("zb5");
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	sz.SetNameAndStatus(L("CstzUI_Mod_scen"), L(""));
	sz.iFlags |= QMF_NOTIFY;
	sz.SetCoord(1025, 480);
	SET_EVENT_MULTI(sz.onChanged,
		{
			uiCreateGame.botNum.Hide();
			uiCreateGame.ResetMode("zbs");
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	zsh.SetNameAndStatus(L("CstzUI_Mod_zsh"), L(""));
	zsh.iFlags |= QMF_NOTIFY;
	zsh.SetCoord(1025, 520);
	SET_EVENT_MULTI(zsh.onChanged,
		{
			uiCreateGame.ResetMap(0);
			uiCreateGame.botNum.Hide();
			uiCreateGame.ResetMode("zsh_pve");
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});
	
	Adv.SetNameAndStatus(L("GameUI_ServerSettings"), L(""));
	Adv.onActivated = UI_AdvServerOptions_Menu;
	Adv.iFlags |= QMF_NOTIFY;
	Adv.SetCoord(160, 660);

	Inventory.SetNameAndStatus(L("CstzUI_Inventory"), L(""));
	Inventory.onActivated = UI_Inventory_Menu;
	Inventory.iFlags |= QMF_NOTIFY;
	Inventory.SetCoord(460, 660);

	playbutton.iFlags = QMF_NOTIFY;
	playbutton.SetRect(1025, 555, 256, 124);
	playbutton.SetPicture(ART_PLAY);
	playbutton.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(playbutton.onActivated,
		{
			uiCreateGame.Begin();
			uiCreateGame.msgBox.MakeOpenEvent();
		});

	uparrow.iFlags = QMF_NOTIFY;
	uparrow.SetRect(125, 395, 25, 25);
	uparrow.SetPicture(ART_UPARROW);
	uparrow.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(uparrow.onActivated,
		{
			if (uiCreateGame.stringmaplist != 1)
			{
				uiCreateGame.stringmaplist--;
				uiCreateGame.ResetMap(0);
			}
			else
			{
				EngFuncs::PlayLocalSound("media/launch_deny2.wav");
			}
		});

	downarrow.iFlags = QMF_NOTIFY;
	downarrow.SetRect(125, 445, 25, 25);
	downarrow.SetPicture(ART_DOWNARROW);
	downarrow.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(downarrow.onActivated,
		{
			if (uiCreateGame.stringmaplist != 3)
			{
				uiCreateGame.stringmaplist++;
				uiCreateGame.ResetMap(0);
			}
			else
			{
				EngFuncs::PlayLocalSound("media/launch_deny2.wav");
			}
		});

	mapsList.SetCharSize( QM_SMALLFONT );
	mapsList.SetupColumn( 0, L("GameUI_Map"), 0.5f ); // Map
	mapsList.SetupColumn( 1, L("Title"), 0.5f ); // Title
	mapsList.SetModel( &mapsListModel );

	hostName.szName = L("GameUI_ServerName");
	hostName.iMaxLength = 28;
	hostName.LinkCvar( "hostname" );

	maxClients.iMaxLength = 3;
	maxClients.bNumbersOnly = true;
	maxClients.szName = L("GameUI_MaxPlayers");
	maxClients.LinkCvar("maxplayers");
	maxClients.UpdateCvar();
	SET_EVENT_MULTI( maxClients.onChanged,
	{
		CMenuField *self = (CMenuField*)pSelf;
		const char *buf = self->GetBuffer();
		int players = atoi( buf );
		if( players <= 1 )
			self->SetBuffer( "2" );
		else if( players > 32 )
			self->SetBuffer( "32" );
	});
	maxClients.onCvarGet = maxClients.onChanged;

	password.szName = L("GameUI_Password");
	password.iMaxLength = 16;
	password.eTextAlignment = QM_CENTER;
	password.bHideInput = true;
	password.LinkCvar( "sv_password" );

	botNum.iMaxLength = 3;
	botNum.bNumbersOnly = true;
	botNum.SetNameAndStatus(L("GameUI_Bots"), L(""));
	botNum.LinkCvar("bot_quota");
	botNum.UpdateCvar();
	botNum.onCvarGet = botNum.onChanged;
	SET_EVENT_MULTI(botNum.onChanged,
		{
			CMenuField *self = (CMenuField*)pSelf;
			const char *buf = self->GetBuffer();
			int players = atoi(buf);
			if (players <= 0)
				self->SetBuffer("");
			else if (players > 32)
				self->SetBuffer("32");
		});
	botNum.onCvarGet = botNum.onChanged;

	msgBox.onPositive = VoidCb(&CMenuCreateGame::Begin);
	msgBox.SetMessage(L("Starting a new game will exit any current game, OK to exit?") );
	msgBox.Link( this );

	Exit.SetNameAndStatus(L("GameUI_Cancel"), L(""));
	Exit.onActivated = VoidCb(&CMenuCreateGame::Hide);
	Exit.iFlags |= QMF_NOTIFY;
	Exit.SetCoord(1098, 685);

	AddItem( mapassault );
	AddItem( mapitaly );
	AddItem( mapvertigo );
	AddItem( mapinferno );
	AddItem( mapnuke );
	AddItem( mapdust );
	AddItem( mapmirage );
	AddItem( mapgressia );
	AddItem( mapruin );

	AddItem( mapbigtree );
	AddItem( mapdustmini );
	AddItem( mapcs747 );
	AddItem( mapestate );
	AddItem( maphavana );
	AddItem( mapmilitia );
	AddItem( mapoffice );
	AddItem( mapsiege );
	AddItem( mapangelcity );

	AddItem( mapaztec );
	AddItem( mapcbble );
	AddItem( mapchateau );
	AddItem( mapprodigy );
	AddItem( maprats );
	AddItem( mapsantorini );
	AddItem( mapskyscraper );
	AddItem( maptorn );
	AddItem( mapdarksnow );
	AddItem( mapnightmare );

	AddItem(mapnightmare2);
	AddItem(mapnightmare3);
	AddItem(maplastclue);
	AddItem(maplostcity);
	AddItem(mappanic);
	AddItem(maptrap);

	AddItem(MapSetAztec);
	AddItem(MapSetCbble);
	AddItem(MapSetChateau);
	AddItem(MapSetProdigy);
	AddItem(MapSetRats);
	AddItem(MapSetSantorini);
	AddItem(MapSetSkyScraper);
	AddItem(MapSetTorn);
	AddItem(MapSetDarkSnow);

	AddItem(MapSetNightMare);
	AddItem(MapSetNightMare2);
	AddItem(MapSetNightMare3);
	AddItem(MapSetLastClue);
	AddItem(MapSetLostCity);
	AddItem(MapSetPanic);
	AddItem(MapSetTrap);

	AddItem( MapSetVertigo );
	AddItem( MapSetAssault );
	AddItem( MapSetItaly );
	AddItem( MapSetInferno );
	AddItem( MapSetNuke );
	AddItem( MapSetDust );
	AddItem( MapSetMirage );
	AddItem( MapSetGressia );
	AddItem( MapSetRuin );

	AddItem( MapSetBigTree );
	AddItem( MapSetDustMini );
	AddItem( MapSet747 );
	AddItem( MapSetEstate );
	AddItem( MapSetHavana );
	AddItem( MapSetMilitia );
	AddItem( MapSetOffice );
	AddItem( MapSetSiege );
	AddItem( MapSetAngelCity );

	AddItem( assault );
	AddItem( italy );
	AddItem( vertigo );
	AddItem( inferno );
	AddItem( nuke );
	AddItem( dust2 );
	AddItem( mirage );
	AddItem( gressia );
	AddItem( ruin );

	AddItem( aztec );
	AddItem( cbble );
	AddItem( chateau );
	AddItem( prodigy );
	AddItem( rats );
	AddItem( santorini );
	AddItem( skyscraper );
	AddItem( torn );
	AddItem( darksnow );

	AddItem( nightmare );
	AddItem(nightmare2);
	AddItem(nightmare3);
	AddItem(lastclue);
	AddItem(lostcity);
	AddItem(panic);
	AddItem(trap);

	AddItem( bigtree );
	AddItem( dustmini );
	AddItem( cs747 );
	AddItem( estate );
	AddItem( havana );
	AddItem( militia );
	AddItem( office );
	AddItem( siege );
	AddItem( angelcity );

	AddItem( classic );
	AddItem( dm );
	AddItem( tdm );
	AddItem( gd );
	AddItem( zc );
	AddItem( zh );
	AddItem( sz );
	AddItem( zsh );

	AddItem( textmap );
	AddItem(textmap2);
	AddItem(textmap3);
	AddItem(textmap4);
	AddItem(textmap5);
	AddItem(textmap6);
	AddItem(textmap7);
	AddItem(textmap8);
	AddItem(textmap9);

	AddItem(textmap10);
	AddItem(textmap11);
	AddItem(textmap12);
	AddItem(textmap13);
	AddItem(textmap14);
	AddItem(textmap15);
	AddItem(textmap16);
	AddItem(textmap17);
	AddItem(textmap18);

	AddItem(textmap19);
	AddItem(textmap20);
	AddItem(textmap21);
	AddItem(textmap22);
	AddItem(textmap23);
	AddItem(textmap24);
	AddItem(textmap25);
	AddItem(textmap26);
	AddItem(textmap27);

	AddItem(textmap28);

	AddItem(textmap29);
	AddItem(textmap30);
	AddItem(textmap31);
	AddItem(textmap32);
	AddItem(textmap33);
	AddItem(textmap34);

	AddItem(Inventory);

	AddItem( playbutton );
	AddItem( uparrow );
	AddItem( downarrow );
	AddItem( maxClients );
	AddItem( hostName );
	AddItem( botNum );
	AddItem( password );
	AddItem( Adv );
	AddItem( Exit );

	//AddItem( nat );
}

void CMenuCreateGame::_VidInit()
{
	nat.SetCoord( 72, 585 );
	if( !EngFuncs::GetCvarFloat("public") )
		nat.Hide();
	else nat.Show();

	hltv.SetCoord( 72, 635 );

	mapsList.SetRect( 590, 230, -200, 465 );
	
	hostName.SetRect( 160, 160, 205, 32 );
	maxClients.SetRect( 390, 160, 205, 32 );
	password.SetRect( 620, 160, 205, 32 );
	botNum.SetRect( 850, 160, 165, 32);

	if (stringmaplist == 1)
	{
		textmap.SetNameAndStatus(L("Assault"), L(""));
		textmap2.SetNameAndStatus(L("Italy"), L(""));
		textmap3.SetNameAndStatus(L("Vertigo"), L(""));

		textmap4.SetNameAndStatus(L("Inferno"), L(""));
		textmap5.SetNameAndStatus(L("Nuke"), L(""));
		textmap6.SetNameAndStatus(L("Dust II"), L(""));

		textmap7.SetNameAndStatus(L("Mirage"), L(""));
		textmap8.SetNameAndStatus(L("Greesia"), L(""));
		textmap9.SetNameAndStatus(L("Ruin"), L(""));

		uiCreateGame.textmap.iFlags |= QMF_INACTIVE;
		uiCreateGame.textmap.SetCoord(248, 310);
		uiCreateGame.textmap2.iFlags |= QMF_INACTIVE;
		uiCreateGame.textmap2.SetCoord(565, 310);
		uiCreateGame.textmap3.iFlags |= QMF_INACTIVE;
		uiCreateGame.textmap3.SetCoord(845, 310);
		uiCreateGame.textmap4.iFlags |= QMF_INACTIVE;
		uiCreateGame.textmap4.SetCoord(248, 450);
		uiCreateGame.textmap5.iFlags |= QMF_INACTIVE;
		uiCreateGame.textmap5.SetCoord(560, 450);
		uiCreateGame.textmap6.iFlags |= QMF_INACTIVE;
		uiCreateGame.textmap6.SetCoord(855, 450);
		uiCreateGame.textmap7.iFlags |= QMF_INACTIVE;
		uiCreateGame.textmap7.SetCoord(248, 592);
		uiCreateGame.textmap8.iFlags |= QMF_INACTIVE;
		uiCreateGame.textmap8.SetCoord(550, 592);
		uiCreateGame.textmap9.iFlags |= QMF_INACTIVE;
		uiCreateGame.textmap9.SetCoord(865, 592);
		uiCreateGame.textmap10.iFlags |= QMF_INACTIVE;
		uiCreateGame.textmap10.SetCoord(248, 310);
		uiCreateGame.textmap11.iFlags |= QMF_INACTIVE;
		uiCreateGame.textmap11.SetCoord(552, 310);
		uiCreateGame.textmap12.iFlags |= QMF_INACTIVE;
		uiCreateGame.textmap12.SetCoord(865, 310);
		uiCreateGame.textmap13.iFlags |= QMF_INACTIVE;
		uiCreateGame.textmap13.SetCoord(248, 450);
		uiCreateGame.textmap14.iFlags |= QMF_INACTIVE;
		uiCreateGame.textmap14.SetCoord(550, 450);
		uiCreateGame.textmap15.iFlags |= QMF_INACTIVE;
		uiCreateGame.textmap15.SetCoord(855, 450);
		uiCreateGame.textmap16.iFlags |= QMF_INACTIVE;
		uiCreateGame.textmap16.SetCoord(248, 592);
		uiCreateGame.textmap17.iFlags |= QMF_INACTIVE;
		uiCreateGame.textmap17.SetCoord(560, 592);
		uiCreateGame.textmap18.iFlags |= QMF_INACTIVE;
		uiCreateGame.textmap18.SetCoord(835, 592);
		uiCreateGame.textmap19.iFlags |= QMF_INACTIVE;
		uiCreateGame.textmap19.SetCoord(251, 310);
		uiCreateGame.textmap20.iFlags |= QMF_INACTIVE;
		uiCreateGame.textmap20.SetCoord(525, 310);
		uiCreateGame.textmap21.iFlags |= QMF_INACTIVE;
		uiCreateGame.textmap21.SetCoord(842, 310);
		uiCreateGame.textmap22.iFlags |= QMF_INACTIVE;
		uiCreateGame.textmap22.SetCoord(248, 450);
		uiCreateGame.textmap23.iFlags |= QMF_INACTIVE;
		uiCreateGame.textmap23.SetCoord(565, 450);
		uiCreateGame.textmap24.iFlags |= QMF_INACTIVE;
		uiCreateGame.textmap24.SetCoord(845, 450);
		uiCreateGame.textmap25.iFlags |= QMF_INACTIVE;
		uiCreateGame.textmap25.SetCoord(228, 592);
		uiCreateGame.textmap26.iFlags |= QMF_INACTIVE;
		uiCreateGame.textmap26.SetCoord(565, 592);
		uiCreateGame.textmap27.iFlags |= QMF_INACTIVE;
		uiCreateGame.textmap27.SetCoord(840, 592);
		uiCreateGame.textmap28.iFlags |= QMF_INACTIVE;
		uiCreateGame.textmap28.SetCoord(241, 310);


		uiCreateGame.textmap29.iFlags |= QMF_INACTIVE;
		uiCreateGame.textmap29.SetCoord(525, 310);
		uiCreateGame.textmap30.iFlags |= QMF_INACTIVE;
		uiCreateGame.textmap30.SetCoord(842, 310);
		uiCreateGame.textmap31.iFlags |= QMF_INACTIVE;
		uiCreateGame.textmap31.SetCoord(248, 450);
		uiCreateGame.textmap32.iFlags |= QMF_INACTIVE;
		uiCreateGame.textmap32.SetCoord(565, 450);
		uiCreateGame.textmap33.iFlags |= QMF_INACTIVE;
		uiCreateGame.textmap33.SetCoord(845, 450);
		uiCreateGame.textmap34.iFlags |= QMF_INACTIVE;
		uiCreateGame.textmap34.SetCoord(228, 592);

		textmap.Show();
		textmap2.Show();
		textmap3.Show();
		textmap4.Show();
		textmap5.Show();
		textmap6.Show();
		textmap7.Show();
		textmap8.Show();
		textmap9.Show();

		textmap10.Hide();
		textmap11.Hide();
		textmap12.Hide();
		textmap13.Hide();
		textmap14.Hide();
		textmap15.Hide();
		textmap16.Hide();
		textmap17.Hide();
		textmap18.Hide();

		textmap19.Hide();
		textmap20.Hide();
		textmap21.Hide();
		textmap22.Hide();
		textmap23.Hide();
		textmap24.Hide();
		textmap25.Hide();
		textmap26.Hide();
		textmap27.Hide();
		textmap28.Hide();

		textmap29.Hide();
		textmap30.Hide();
		textmap31.Hide();
		textmap32.Hide();
		textmap33.Hide();
		textmap34.Hide();

		mapassault.Show();
		mapitaly.Show();
		mapvertigo.Show();
		mapinferno.Show();
		mapnuke.Show();
		mapdust.Show();
		mapmirage.Show();
		mapgressia.Show();
		mapruin.Show();

		assault.Show();
		vertigo.Show();
		italy.Show();
		nuke.Show();
		dust2.Show();
		inferno.Show();
		mirage.Show();
		gressia.Show();
		ruin.Show();

		bigtree.Hide();
		dustmini.Hide();
		cs747.Hide();
		estate.Hide();
		havana.Hide();
		militia.Hide();
		office.Hide();
		siege.Hide();
		angelcity.Hide();

		mapbigtree.Hide();
		mapdustmini.Hide();
		mapcs747.Hide();
		mapestate.Hide();
		maphavana.Hide();
		mapmilitia.Hide();
		mapoffice.Hide();
		mapsiege.Hide();
		mapangelcity.Hide();

		aztec.Hide();
		cbble.Hide();
		chateau.Hide();
		prodigy.Hide();
		rats.Hide();
		santorini.Hide();
		skyscraper.Hide();
		torn.Hide();
		darksnow.Hide();

		mapaztec.Hide();
		mapcbble.Hide();
		mapchateau.Hide();
		mapprodigy.Hide();
		maprats.Hide();
		mapsantorini.Hide();
		mapskyscraper.Hide();
		maptorn.Hide();
		mapdarksnow.Hide();
		mapnightmare.Hide();
		MapSetNightMare.Hide();
		nightmare.Hide();

		mapnightmare2.Hide();
		MapSetNightMare2.Hide();
		nightmare2.Hide();

		mapnightmare3.Hide();
		MapSetNightMare3.Hide();
		nightmare3.Hide();

		maplastclue.Hide();
		MapSetLastClue.Hide();
		lastclue.Hide();

		maplostcity.Hide();
		MapSetLostCity.Hide();
		lostcity.Hide();

		mappanic.Hide();
		MapSetPanic.Hide();
		panic.Hide();

		maptrap.Hide();
		MapSetTrap.Hide();
		trap.Hide();
	}
	else if (stringmaplist == 2)
	{
		textmap.SetNameAndStatus(L("BigTree"), L(""));

		

		mapassault.Hide();
		mapitaly.Hide();
		mapvertigo.Hide();
		mapinferno.Hide();
		mapnuke.Hide();
		mapdust.Hide();
		mapmirage.Hide();
		mapgressia.Hide();
		mapruin.Hide();

		assault.Hide();
		vertigo.Hide();
		italy.Hide();
		nuke.Hide();
		dust2.Hide();
		inferno.Hide();
		mirage.Hide();
		gressia.Hide();
		ruin.Hide();

		bigtree.Show();
		dustmini.Show();
		cs747.Show();
		estate.Show();
		havana.Show();
		militia.Show();
		office.Show();
		siege.Show();
		angelcity.Show();

		mapbigtree.Show();
		mapdustmini.Show();
		mapcs747.Show();
		mapestate.Show();
		maphavana.Show();
		mapmilitia.Show();
		mapoffice.Show();
		mapsiege.Show();
		mapangelcity.Show();

		aztec.Hide();
		cbble.Hide();
		chateau.Hide();
		prodigy.Hide();
		rats.Hide();
		santorini.Hide();
		skyscraper.Hide();
		torn.Hide();
		darksnow.Hide();

		mapaztec.Hide();
		mapcbble.Hide();
		mapchateau.Hide();
		mapprodigy.Hide();
		maprats.Hide();
		mapsantorini.Hide();
		mapskyscraper.Hide();
		maptorn.Hide();
		mapdarksnow.Hide();
		mapnightmare.Hide();
		MapSetNightMare.Hide();
		nightmare.Hide();

		mapnightmare2.Hide();
		MapSetNightMare2.Hide();
		nightmare2.Hide();

		mapnightmare3.Hide();
		MapSetNightMare3.Hide();
		nightmare3.Hide();

		maplastclue.Hide();
		MapSetLastClue.Hide();
		lastclue.Hide();

		maplostcity.Hide();
		MapSetLostCity.Hide();
		lostcity.Hide();

		mappanic.Hide();
		MapSetPanic.Hide();
		panic.Hide();

		maptrap.Hide();
		MapSetTrap.Hide();
		trap.Hide();
	}
	else if (stringmaplist == 3)
	{
	

		mapassault.Hide();
		mapitaly.Hide();
		mapvertigo.Hide();
		mapinferno.Hide();
		mapnuke.Hide();
		mapdust.Hide();
		mapmirage.Hide();
		mapgressia.Hide();
		mapruin.Hide();

		assault.Hide();
		vertigo.Hide();
		italy.Hide();
		nuke.Hide();
		dust2.Hide();
		inferno.Hide();
		mirage.Hide();
		gressia.Hide();
		ruin.Hide();



		bigtree.Hide();
		dustmini.Hide();
		cs747.Hide();
		estate.Hide();
		havana.Hide();
		militia.Hide();
		office.Hide();
		siege.Hide();
		angelcity.Hide();

		mapbigtree.Hide();
		mapdustmini.Hide();
		mapcs747.Hide();
		mapestate.Hide();
		maphavana.Hide();
		mapmilitia.Hide();
		mapoffice.Hide();
		mapsiege.Hide();
		mapangelcity.Hide();

		aztec.Show();
		cbble.Show();
		chateau.Show();
		prodigy.Show();
		rats.Show();
		santorini.Show();
		skyscraper.Show();
		torn.Show();
		darksnow.Show();

		mapaztec.Show();
		mapcbble.Show();
		mapchateau.Show();
		mapprodigy.Show();
		maprats.Show();
		mapsantorini.Show();
		mapskyscraper.Show();
		maptorn.Show();
		mapdarksnow.Show();
		mapnightmare.Hide();
		MapSetNightMare.Hide();
		nightmare.Hide();

		mapnightmare2.Hide();
		MapSetNightMare2.Hide();
		nightmare2.Hide();

		mapnightmare3.Hide();
		MapSetNightMare3.Hide();
		nightmare3.Hide();

		maplastclue.Hide();
		MapSetLastClue.Hide();
		lastclue.Hide();

		maplostcity.Hide();
		MapSetLostCity.Hide();
		lostcity.Hide();

		mappanic.Hide();
		MapSetPanic.Hide();
		panic.Hide();

		maptrap.Hide();
		MapSetTrap.Hide();
		trap.Hide();
	}

}

void CMenuCreateGame::Reload( void )
{
	mapsListModel.Update();
}

/*
=================
CMenuCreateGame::Precache
=================
*/
void UI_CreateGame_Precache( void )
{
	EngFuncs::PIC_Load( ART_BANNER );
	EngFuncs::PIC_Load( ART_ASSAULT );
	EngFuncs::PIC_Load( ART_ITALY );
	EngFuncs::PIC_Load( ART_VERTIGO );
	EngFuncs::PIC_Load( ART_INFERNO );
	EngFuncs::PIC_Load( ART_NUKE );
	EngFuncs::PIC_Load( ART_DUST );
	EngFuncs::PIC_Load( ART_MIRAGE );
	EngFuncs::PIC_Load( ART_BIGTREE );
	EngFuncs::PIC_Load( ART_DUSTMINI );
	EngFuncs::PIC_Load( ART_747 );
	EngFuncs::PIC_Load( ART_ESTATE );
	EngFuncs::PIC_Load( ART_HAVANA );
	EngFuncs::PIC_Load( ART_MILITIA );
	EngFuncs::PIC_Load( ART_OFFICE );
	EngFuncs::PIC_Load( ART_SIEGE );
	EngFuncs::PIC_Load( ART_ANGELCITY );
	EngFuncs::PIC_Load( ART_AZTEC );
	EngFuncs::PIC_Load( ART_CBBLE );
	EngFuncs::PIC_Load( ART_CHATEAU );
	EngFuncs::PIC_Load( ART_PRODIGY );
	EngFuncs::PIC_Load( ART_RATS );
	EngFuncs::PIC_Load( ART_SANTORINI );
	EngFuncs::PIC_Load( ART_SKYSCRAPER );
	EngFuncs::PIC_Load( ART_TORN );
	EngFuncs::PIC_Load( ART_DARKSNOW );
}

/*
=================
CMenuCreateGame::Menu
=================
*/
void UI_CreateGame_Menu( void )
{
	if ( gMenu.m_gameinfo.gamemode == GAME_SINGLEPLAYER_ONLY )
		return;

	uiCreateGame.Show();

	uiCreateGame.ResetMode(0);
}
ADD_MENU( menu_creategame, UI_CreateGame_Precache, UI_CreateGame_Menu );
