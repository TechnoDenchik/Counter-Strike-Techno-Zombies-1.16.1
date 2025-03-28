/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "Framework.h"
#include "Slider.h"
#include "Bitmap.h"
#include "PicButton.h"
#include "CheckBox.h"
#include "SpinControl.h"
#include "StringArrayModel.h"
#include "Table.h"

#define ART_BACK	"gfx/shell/WeaponGet/hud_weapon_off_bg"

 /*=================== Weapon Picture ===================*/

#define ART_AS50		"gfx/shell/WeaponGet/as50"
#define ART_BOW			"gfx/shell/WeaponGet/bow"
#define ART_CANNON		"gfx/shell/WeaponGet/cannon"
#define ART_CHAINSAW	"gfx/shell/WeaponGet/chainsaw"

#define ART_DGAXE		"gfx/shell/WeaponGet/dgaxe"
#define ART_DRAGONSWORD	"gfx/shell/WeaponGet/dragonsword"
#define ART_DUALSWORD	"gfx/shell/WeaponGet/dualsword"
#define ART_GUNGNIR		"gfx/shell/WeaponGet/gungnir"

#define ART_GUNKATA		"gfx/shell/WeaponGet/gunkata"
#define ART_HALOGUN		"gfx/shell/WeaponGet/halogun"
#define ART_INFINITY	"gfx/shell/WeaponGet/infinity"
#define ART_INFINITYEX1	"gfx/shell/WeaponGet/infinityex1"

#define ART_INFINITYEX2	"gfx/shell/WeaponGet/infinityex2"
#define ART_INFINITYSB	"gfx/shell/WeaponGet/infinitysb"
#define ART_INFINITYSR	"gfx/shell/WeaponGet/infinitysr"
#define ART_INFINITYSS	"gfx/shell/WeaponGet/infinityss"

#define ART_KRISS		"gfx/shell/WeaponGet/kriss"
#define ART_M14EBR		"gfx/shell/WeaponGet/m14ebr"
#define ART_M95			"gfx/shell/WeaponGet/m95"
#define ART_M1887		"gfx/shell/WeaponGet/m1887"

#define ART_MG3			"gfx/shell/WeaponGet/mg3"
#define ART_MP7A1		"gfx/shell/WeaponGet/mp7a1"
#define ART_REVIVEGUN	"gfx/shell/WeaponGet/revivegun"
#define ART_SCAR		"gfx/shell/WeaponGet/scar"

#define ART_SGDRILL		"gfx/shell/WeaponGet/sgdrill"
#define ART_SKULLAXE	"gfx/shell/WeaponGet/skullaxe"
#define ART_STARCHASER	"gfx/shell/WeaponGet/starchaserar"
#define ART_TAR21		"gfx/shell/WeaponGet/tar21"

#define ART_THOMPSON	"gfx/shell/WeaponGet/thompson"
#define ART_VOIDPISTOL	"gfx/shell/WeaponGet/voidpistol"
#define ART_WA2000		"gfx/shell/WeaponGet/wa2000"
#define ART_WONDERK		"gfx/shell/WeaponGet/wondercannon"

#define ART_XM8			"gfx/shell/WeaponGet/xm8"

/*=================== End Weapon Picture ==================*/

class CMenuMapListModel : public CMenuBaseModel
{
public:
	void Update() override;
	int GetColumns() const override { return 2; }
	int GetRows() const override { return m_iNumItems; }
	const char* GetCellText(int line, int column) override
	{
		switch (column)
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

class CMenuInventory : public CMenuFramework
{
public:
	typedef CMenuFramework BaseClass;

	CMenuInventory() : CMenuFramework("CMenuInventory") { }

	CMenuMapListModel mapsListModel;
	class CMenuVidPreview : public CMenuBitmap{} 
		background1, wpas50,
		background2, wpbow,
		background3, wpcannon,
		background4, wpchainsaw,
		background5, wpdgaxe,
		background6, wpdragonsword,
		background7, wpdualsword,
		background8, wpgungnir,
		background9, wpgunkata,
		background10, wphalogun,
		background11, wpinfinity,
		background12, wpinfinityex1,
		background13, wpinfinityex2,
		background14, wpinfinitysb,
		background15, wpinfinitysr,
		background16, wpinfinityss,
		background17, wpkriss,
		background18, wpm14ebr,
		background19, wpm95,
		background20, wpm1887,
		background21, wpmg3,
		background22, wpmp7a1,
		background23, wprevivegun,
		background24, wpscar,
		background25, wpsgdrill,
		background26, wpskullaxe,
		background27, wpstarchaserar,
		background28, wptar21,
		background29, wpthompson,
		background30, wpvoidpistol,
		background31, wpwa2000,
		background32, wpwondercannon,
		background33, wpxm8,
		background34;

		CMapSet
			 Setwpas50,
			Setwpbow,
			Setwpcannon,
			Setwpchainsaw,
			Setwpdgaxe,
			Setwpdragonsword,
			Setwpdualsword,
			Setwpgungnir,
			Setwpgunkata,
			Setwphalogun,
			Setwpinfinity,
			Setwpinfinityex1,
			Setwpinfinityex2,
			Setwpinfinitysb,
			Setwpinfinitysr,
			Setwpinfinityss,
			Setwpkriss,
			Setwpm14ebr,
			Setwpm95,
			Setwpm1887,
			Setwpmg3,
			Setwpmp7a1,
			Setwprevivegun,
			Setwpscar,
			Setwpsgdrill,
			Setwpskullaxe,
			Setwpstarchaserar,
			Setwptar21,
			Setwpthompson,
			Setwpvoidpistol,
			Setwpwa2000,
			Setwpwondercannon,
			Setwpxm8, 
			SetBack1,
			SetBack2,
			SetBack3,
			SetBack4,
			SetBack5,
			SetBack6,
			SetBack7,
			SetBack8,
			SetBack9,
			SetBack10,
			SetBack11,
			SetBack12,
			SetBack13,
			SetBack14,
			SetBack15,
			SetBack16;
		
private:
	void _Init() override;
	void _VidInit() override;
	void GetConfig();
	void SaveAndPopMenu() override;
	void exits() override;
	void SetGun(char* value);
	void SetPistol(char* value);
	void SetKnife(char* value);
	void SetGrenade(char* value);

	CMenuPicButton done, apply;
	CMenuPicButton done2, exit;
};

static CMenuInventory	uiInventory;

void CMenuInventory::SetGun(char* value)
{
	EngFuncs::CvarSetString("wpn_getgun", value);
}

void CMenuInventory::SetPistol(char* value)
{
	EngFuncs::CvarSetString("wpn_getpistol", value);
}

void CMenuInventory::SetKnife(char* value)
{
	EngFuncs::CvarSetString("wpn_getknife", value);
}

void CMenuInventory::SetGrenade(char* value)
{
	EngFuncs::CvarSetString("wpn_getgrenade", value);
}

void CMenuInventory::GetConfig( void )
{
	SetGun("weapon_wondercannoex");
	SetPistol("weapon_voidpistolex");
	SetKnife("weapon_twinaxes");
}

void CMenuInventory::SaveAndPopMenu()
{
	CMenuFramework::SaveAndPopMenu();
}

void CMenuInventory::exits()
{
	char musicset = (char)EngFuncs::GetCvarString("");
		
	CMenuFramework::exits();
}

void CMenuInventory::_Init( void )
{
	char gunset = (char)EngFuncs::GetCvarString("wpn_getgun");
	char pistolset = (char)EngFuncs::GetCvarString("wpn_pistol");
	char knifeset = (char)EngFuncs::GetCvarString("wpn_knife");
	char grenadeset = (char)EngFuncs::GetCvarString("wpn_grenade");

	background1.iFlags = QMF_INACTIVE;
	background1.SetRect(200, 225, 235, 81);
	background1.SetPicture(ART_BACK);
	background1.SetRenderMode(QM_DRAWTRANS, QM_DRAWTRANS, QM_DRAWTRANS);

	background2.iFlags = QMF_INACTIVE;
	background2.SetRect(600, 225, 400, 480);
	background2.SetPicture(ART_BACK);
	background2.SetRenderMode(QM_DRAWTRANS, QM_DRAWTRANS, QM_DRAWTRANS);

	SetBack4.iFlags = QMF_INACTIVE;
	SetBack4.SetCharSize(QM_SMALLFONT);
	SetBack4.SetModel(&mapsListModel);
	SetBack4.SetRect(600, 190, 400, 510);

	Setwpas50.iFlags = QMF_INACTIVE;
	Setwpas50.SetCharSize(QM_SMALLFONT);
	Setwpas50.SetModel(&mapsListModel);
	Setwpas50.SetRect(200, 193, 230, 111);

	SetBack5.iFlags = QMF_INACTIVE;
	SetBack5.SetCharSize(QM_SMALLFONT);
	SetBack5.SetModel(&mapsListModel);
	SetBack5.SetRect(610, 205, 180, 90);

	SetBack6.iFlags = QMF_INACTIVE;
	SetBack6.SetCharSize(QM_SMALLFONT);
	SetBack6.SetModel(&mapsListModel);
	SetBack6.SetRect(810, 205, 180, 90);

	SetBack7.iFlags = QMF_INACTIVE;
	SetBack7.SetCharSize(QM_SMALLFONT);
	SetBack7.SetModel(&mapsListModel);
	SetBack7.SetRect(610, 275, 180, 90);

	SetBack8.iFlags = QMF_INACTIVE;
	SetBack8.SetCharSize(QM_SMALLFONT);
	SetBack8.SetModel(&mapsListModel);
	SetBack8.SetRect(810, 275, 180, 90);

	wpas50.iFlags = QMF_NOTIFY;
	wpas50.SetRect(200, 225, 235, 81);
	wpas50.SetPicture(ART_HALOGUN);
	wpas50.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wpas50.onActivated,
		{
			uiInventory.SetGun("as50");
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});


	SetBack1.iFlags = QMF_INACTIVE;
	SetBack1.SetCharSize(QM_SMALLFONT);
	SetBack1.SetModel(&mapsListModel);
	SetBack1.SetRect(200, 193, 230, 111);

	background3.iFlags = QMF_INACTIVE;
	background3.SetRect(200, 346, 235, 81);
	background3.SetPicture(ART_BACK);
	background3.SetRenderMode(QM_DRAWTRANS, QM_DRAWTRANS, QM_DRAWTRANS);

	SetBack2.iFlags = QMF_INACTIVE;
	SetBack2.SetCharSize(QM_SMALLFONT);
	SetBack2.SetModel(&mapsListModel);
	SetBack2.SetRect(200, 313, 230, 111);

	background4.iFlags = QMF_INACTIVE;
	background4.SetRect(200, 463, 235, 81);
	background4.SetPicture(ART_BACK);
	background4.SetRenderMode(QM_DRAWTRANS, QM_DRAWTRANS, QM_DRAWTRANS);

	SetBack3.iFlags = QMF_INACTIVE;
	SetBack3.SetCharSize(QM_SMALLFONT);
	SetBack3.SetModel(&mapsListModel);
	SetBack3.SetRect(200, 433, 230, 111);

	apply.SetNameAndStatus(L("GameUI_Apply"), L(""));
	apply.onActivated = VoidCb(&CMenuInventory::exits);
	apply.iFlags |= QMF_NOTIFY;
	apply.SetCoord(30, 450);

	exit.SetNameAndStatus(L("GameUI_Close"), L(""));
	exit.onActivated = VoidCb(&CMenuInventory::Hide);
	exit.iFlags |= QMF_NOTIFY;
	exit.SetCoord(30, 400);
	
	AddItem( background );
	AddItem(background1);
	AddItem(background2);
	AddItem(background3);
	AddItem(background4);

	AddItem(SetBack1);
	AddItem(SetBack2);
	AddItem(SetBack3);
	AddItem(SetBack4);
	AddItem(SetBack5);
	AddItem(SetBack6);

	AddItem(SetBack7);
	AddItem(SetBack8);

	AddItem(wpas50);
	AddItem(Setwpas50);
	AddItem( apply );
	AddItem( exit );
	//AddItem( testImage );
	//AddItem(inventory);
}

void CMenuInventory::_VidInit( )
{
	GetConfig();
}

void UI_Inventory_Precache(void)
{
	EngFuncs::PIC_Load( ART_BACK );
	EngFuncs::PIC_Load( ART_AS50 );
	EngFuncs::PIC_Load( ART_BOW );
	EngFuncs::PIC_Load( ART_CANNON );
	EngFuncs::PIC_Load( ART_CHAINSAW );
	EngFuncs::PIC_Load( ART_DGAXE );
	EngFuncs::PIC_Load( ART_DRAGONSWORD	);
	EngFuncs::PIC_Load( ART_DUALSWORD );
	EngFuncs::PIC_Load( ART_GUNGNIR	);
	EngFuncs::PIC_Load( ART_GUNKATA	);
	EngFuncs::PIC_Load( ART_HALOGUN );
	EngFuncs::PIC_Load( ART_INFINITY );
	EngFuncs::PIC_Load( ART_INFINITYEX1	);
	EngFuncs::PIC_Load( ART_INFINITYEX2	);
	EngFuncs::PIC_Load( ART_INFINITYSB );
	EngFuncs::PIC_Load( ART_INFINITYSR );
	EngFuncs::PIC_Load( ART_INFINITYSS );
	EngFuncs::PIC_Load( ART_KRISS );
	EngFuncs::PIC_Load( ART_M14EBR );
	EngFuncs::PIC_Load( ART_M95	);
	EngFuncs::PIC_Load( ART_M1887 );
	EngFuncs::PIC_Load( ART_MG3	);
	EngFuncs::PIC_Load( ART_MP7A1 );
	EngFuncs::PIC_Load( ART_REVIVEGUN );
	EngFuncs::PIC_Load( ART_SCAR );
	EngFuncs::PIC_Load( ART_SGDRILL );
	EngFuncs::PIC_Load( ART_SKULLAXE );
	EngFuncs::PIC_Load( ART_STARCHASER );
	EngFuncs::PIC_Load( ART_TAR21 );
	EngFuncs::PIC_Load( ART_THOMPSON );
	EngFuncs::PIC_Load( ART_VOIDPISTOL );
	EngFuncs::PIC_Load( ART_WA2000 );
	EngFuncs::PIC_Load( ART_WONDERK	);
	EngFuncs::PIC_Load( ART_XM8 );
}

void UI_Inventory_Menu( void )
{
	uiInventory.Show();
}
ADD_MENU( menu_inventory, UI_Inventory_Precache, UI_Inventory_Menu);
