/* =================================================================================== *
		   * =================== TechnoSoftware =================== *
	  * ================= Weapon ID System Inventory ================== *
 * =================================================================================== */

#include "Framework.h"
#include "Slider.h"
#include "Bitmap.h"/*      ποπποπονπο*/
#include "PicButton.h"
#include "CheckBox.h"
#include "SpinControl.h"
#include "StringArrayModel.h"
#include "Table.h"

#define ART_BACK		"gfx/shell/WeaponGet/hud_weapon_off_bg"
#define ART_UPARROW		"gfx/shell/uparrowp"
#define ART_DOWNARROW	"gfx/shell/sm_dnarw"

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
#define ART_WONDERC		"gfx/shell/WeaponGet/wondercannon"

#define ART_WONDERCEX		"gfx/shell/WeaponGet/wondercannonex"

#define ART_BUFFM4A1		"gfx/shell/WeaponGet/buffm4"
#define ART_PALADIN		"gfx/shell/WeaponGet/buffak"

#define ART_DGAXEEX		"gfx/shell/WeaponGet/dgaxeex"
#define ART_VOIDPISTOLEX		"gfx/shell/WeaponGet/voidpistolex"

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
	
	int stringsprimary;
	int stringssecondary;
	int stringsmelee;

	int pagetypeweapon;

	CMenuMapListModel mapsListModel;
	class CMenuVidPreview : public CMenuBitmap{} 

		//gun
		background1, wpas50, wpas50tex,
		background2, wpbow, wpbowtex,
		background3, wpcannon, wpcannontex,
		background8,  wpgungnir, wpgungnirtex,
		background10, wphalogun, wphaloguntex,
		background17, wpkriss, wpkrisstex,
		background18, wpm14ebr, wpm14ebrtex,
		background19, wpm95, wpm95tex,
		background20, wpm1887, wpm1887tex,
		background21, wpmg3, wpmg3tex,
		background22, wpmp7a1, wpmp7a1tex,
		background23, wprevivegun, wpreviveguntex,
		background24, wpscar, wpscartex,
		background25, wpsgdrill, wpsgdrilltex,
		background27, wpstarchaserar, wpstarchaserartex,
		background28, wptar21, wptar21tex,
		background4,  wpchainsaw, wpchainsawtex,
		background29, wpthompson, wpthompsontex,
		background31, wpwa2000, wpwa2000tex,
		background32, wpwondercannon, wpwondercannontex,
		background33, wpxm8, wpxm8tex,
		background35, wpwondercannonex, wpwondercannonextex,
		background39, wpbuffm4a1, wpbuffm4a1tex,
		background40, wppaladin, wppaladintex,

		//pistol
		background36, wpvoidpistolex, wpvoidpistolextex,
		background30, wpvoidpistol, wpvoidpistoltex,		
		background11, wpinfinity, wpinfinitytex,
		background12, wpinfinityex1, wpinfinityex1tex,
		background13, wpinfinityex2, wpinfinityex2tex,
		background14, wpinfinitysb, wpinfinitysbtex,
		background15, wpinfinitysr, wpinfinitysrtex,
		background16, wpinfinityss, wpinfinitysstex,

		//knife
		background6,  wpdragonsword, wpdragonswordtex,
		background7,  wpdualsword, wpdualswordtex,
		background26, wpskullaxe, wpskullaxetex,
		background37, wptwinaxes, wptwinaxestex,
		background38, wptwinaxesex, wptwinaxesextex,
		wpknife, wpknifetex,

		//ui
		uparrow, downarrow;

		CMapSet
			 Setwpas50,
			Setwpbow,
			Setwpcannon,
			Setwpchainsaw,
			Setwpdragonsword,
			Setwpdualsword,
			Setwpgungnir,
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
			Setwpvoidpistolex,
			Setwpwa2000,
			Setwpwondercannon,
			Setwpwondercannonex,
			Setwptwinaxes,
			Setwptwinaxesex,
			Setwppaladin,
			Setwpbuffm4a1,
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
			SetBack16,
			SetBack17, 
			SetBack18;

		CMenuCheckBox
			as50, bow, cannon, chainsaw, dragonsword, dualsword, gungnir,
			halogun, infinity, infinityex1, infinityex2, infinityexsb, infinityexsr, infinityexss,
			kriss, m14ebr, m95, m1887, mg3, mp7a1, revivegun,
			scar, sgdrill, skullaxe, starchaserar, tar21, thompson, voidpistol,
			voidpistolex, wa2000, wondercannon, wondercannonex, twinaxes, twinaxesex, xm8, buffm4a1,
			paladin;
		
private:
	void _Init() override;
	void _VidInit() override;
	void GetWp(int primary, int secondary, int melee, int grenade);
	void GetPage(int page);
	void SaveAndPopMenu() override;
	void SetGun(int value);
	void SetPistol(int value);
	void SetKnife(int value);
	void SetGrenade(int value);
	CMenuCheckBox	set;
	CMenuPicButton done, apply, prim, secon, melee, equip, items;
	CMenuPicButton done2, exit, primtex, secontex, meleetex;
};

static CMenuInventory	uiInventory;

void CMenuInventory::SetGun(int value)
{
	set.LinkCvar("wpn_getgun");
	EngFuncs::CvarSetValue("wpn_getgun", value);
}

void CMenuInventory::SetPistol(int value)
{
	set.LinkCvar("wpn_getpistol");
	EngFuncs::CvarSetValue("wpn_getpistol", value);
}

void CMenuInventory::SetKnife(int value)
{
	set.LinkCvar("wpn_getknife");
	EngFuncs::CvarSetValue("wpn_getknife", value);
}

void CMenuInventory::SetGrenade(int value)
{
	set.LinkCvar("wpn_getgrenade");
	EngFuncs::CvarSetValue("wpn_getgrenade", value);
}

void CMenuInventory::GetWp(int primary, int secondary, int melee, int grenade)
{
	SetGun(primary);
	SetPistol(secondary);
	SetKnife(melee);
	SetGrenade(grenade);
}

void CMenuInventory::GetPage(int page)
{
	pagetypeweapon = page;

	if (pagetypeweapon == 1)
	{
		if (stringsprimary == 1)
		{
			wphaloguntex.Show();
			wpwondercannonextex.Show();
			wpwondercannontex.Show();
			wpreviveguntex.Show();
			wpgungnirtex.Show();
			wpsgdrilltex.Show();
			wpcannontex.Show();
			wppaladintex.Show();
			wpbuffm4a1tex.Show();
			wpchainsawtex.Show();
			wpstarchaserartex.Show();
			wpbowtex.Show();

			wpthompsontex.Hide();
			wptar21tex.Hide();
			wpmp7a1tex.Hide();
			wpmg3tex.Hide();
			wpm1887tex.Hide();
			wpm95tex.Hide();
			wpm14ebrtex.Hide();
			wpkrisstex.Hide();
			wpscartex.Hide();
			wpwa2000tex.Hide();
			wpxm8tex.Hide();
			wpas50tex.Hide();
		}
		else if (stringsprimary == 2)
		{
			wphaloguntex.Hide();
			wpwondercannonextex.Hide();
			wpwondercannontex.Hide();
			wpreviveguntex.Hide();
			wpgungnirtex.Hide();
			wpsgdrilltex.Hide();
			wpcannontex.Hide();
			wppaladintex.Hide();
			wpbuffm4a1tex.Hide();
			wpchainsawtex.Hide();
			wpstarchaserartex.Hide();
			wpbowtex.Hide();

			wpthompsontex.Show();
			wptar21tex.Show();
			wpmp7a1tex.Show();
			wpmg3tex.Show();
			wpm1887tex.Show();
			wpkrisstex.Show();
			wpm95tex.Show();
			wpm14ebrtex.Show();
			wpscartex.Show();
			wpwa2000tex.Show();
			wpxm8tex.Show();
			wpas50tex.Show();
		}
		wpvoidpistolextex.Hide();
		wpvoidpistoltex.Hide();
		wpinfinitytex.Hide();
		wpinfinityex1tex.Hide();
		wpinfinityex2tex.Hide();
		wpinfinitysbtex.Hide();
		wpinfinitysrtex.Hide();
		wpinfinitysstex.Hide();

		wptwinaxesextex.Hide();
		wptwinaxestex.Hide();
		wpdualswordtex.Hide();
		wpdragonswordtex.Hide();
		wpskullaxetex.Hide();

		downarrow.Show();
		uparrow.Show();
	}
	else if (pagetypeweapon == 2)
	{
		if (stringssecondary == 1)
		{
			wpvoidpistolextex.Show();
			wpvoidpistoltex.Show();
			wpinfinitytex.Show();
			wpinfinityex1tex.Show();
			wpinfinityex2tex.Show();
			wpinfinitysbtex.Show();
			wpinfinitysrtex.Show();
			wpinfinitysstex.Show();
		}

		wphaloguntex.Hide();
		wpwondercannonextex.Hide();
		wpwondercannontex.Hide();
		wpreviveguntex.Hide();
		wpgungnirtex.Hide();
		wpsgdrilltex.Hide();
		wpcannontex.Hide();
		wppaladintex.Hide();
		wpbuffm4a1tex.Hide();
		wpchainsawtex.Hide();
		wpstarchaserartex.Hide();
		wpbowtex.Hide();

		wpthompsontex.Hide();
		wptar21tex.Hide();
		wpmp7a1tex.Hide();
		wpmg3tex.Hide();
		wpm1887tex.Hide();
		wpm95tex.Hide();
		wpm14ebrtex.Hide();
		wpkrisstex.Hide();
		wpscartex.Hide();
		wpwa2000tex.Hide();
		wpxm8tex.Hide();
		wpas50tex.Hide();
		downarrow.Hide();
		uparrow.Hide();

		wptwinaxesextex.Hide();
		wptwinaxestex.Hide();
		wpdualswordtex.Hide();
		wpdragonswordtex.Hide();
		wpskullaxetex.Hide();
	}
	else if (pagetypeweapon == 3)
	{
		if (stringsmelee == 1)
		{
			wptwinaxesextex.Show();
			wptwinaxestex.Show();
			wpdualswordtex.Show();
			wpdragonswordtex.Show();
			wpskullaxetex.Show();
		}
		wphaloguntex.Hide();
		wpwondercannonextex.Hide();
		wpwondercannontex.Hide();
		wpreviveguntex.Hide();
		wpgungnirtex.Hide();
		wpsgdrilltex.Hide();
		wpcannontex.Hide();
		wppaladintex.Hide();
		wpbuffm4a1tex.Hide();
		wpchainsawtex.Hide();
		wpstarchaserartex.Hide();
		wpbowtex.Hide();

		wpthompsontex.Hide();
		wptar21tex.Hide();
		wpmp7a1tex.Hide();
		wpmg3tex.Hide();
		wpm1887tex.Hide();
		wpm95tex.Hide();
		wpm14ebrtex.Hide();
		wpkrisstex.Hide();
		wpscartex.Hide();
		wpwa2000tex.Hide();
		wpxm8tex.Hide();
		wpas50tex.Hide();

		wpvoidpistolextex.Hide();
		wpvoidpistoltex.Hide();
		wpinfinitytex.Hide();
		wpinfinityex1tex.Hide();
		wpinfinityex2tex.Hide();
		wpinfinitysbtex.Hide();
		wpinfinitysrtex.Hide();
		wpinfinitysstex.Hide();

		downarrow.Hide();
		uparrow.Hide();
	}
}

void CMenuInventory::SaveAndPopMenu()
{
	CMenuFramework::SaveAndPopMenu();
}

void CMenuInventory::_Init( void )
{
	stringsprimary = 1;
	stringssecondary = 1;
	stringsmelee = 1;
	
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

	SetBack9.iFlags = QMF_INACTIVE;
	SetBack9.SetCharSize(QM_SMALLFONT);
	SetBack9.SetModel(&mapsListModel);
	SetBack9.SetRect(610, 345, 180, 90);

	SetBack10.iFlags = QMF_INACTIVE;
	SetBack10.SetCharSize(QM_SMALLFONT);
	SetBack10.SetModel(&mapsListModel);
	SetBack10.SetRect(610, 345, 180, 90);

	SetBack11.iFlags = QMF_INACTIVE;
	SetBack11.SetCharSize(QM_SMALLFONT);
	SetBack11.SetModel(&mapsListModel);
	SetBack11.SetRect(610, 415, 180, 90);

	SetBack12.iFlags = QMF_INACTIVE;
	SetBack12.SetCharSize(QM_SMALLFONT);
	SetBack12.SetModel(&mapsListModel);
	SetBack12.SetRect(610, 485, 180, 90);

	SetBack13.iFlags = QMF_INACTIVE;
	SetBack13.SetCharSize(QM_SMALLFONT);
	SetBack13.SetModel(&mapsListModel);
	SetBack13.SetRect(610, 555, 180, 90);

	SetBack14.iFlags = QMF_INACTIVE;
	SetBack14.SetCharSize(QM_SMALLFONT);
	SetBack14.SetModel(&mapsListModel);
	SetBack14.SetRect(810, 345, 180, 90);

	SetBack15.iFlags = QMF_INACTIVE;
	SetBack15.SetCharSize(QM_SMALLFONT);
	SetBack15.SetModel(&mapsListModel);
	SetBack15.SetRect(810, 345, 180, 90);

	SetBack16.iFlags = QMF_INACTIVE;
	SetBack16.SetCharSize(QM_SMALLFONT);                                     
	SetBack16.SetModel(&mapsListModel);
	SetBack16.SetRect(810, 415, 180, 90);

	SetBack17.iFlags = QMF_INACTIVE;
	SetBack17.SetCharSize(QM_SMALLFONT);
	SetBack17.SetModel(&mapsListModel);
	SetBack17.SetRect(810, 485, 180, 90);

	SetBack18.iFlags = QMF_INACTIVE;
	SetBack18.SetCharSize(QM_SMALLFONT);
	SetBack18.SetModel(&mapsListModel);
	SetBack18.SetRect(810, 555, 180, 90);

	//primary
	wphalogun.iFlags = QMF_NOTIFY;
	wphalogun.SetRect(200, 225, 235, 81);
	wphalogun.SetPicture(ART_HALOGUN);
	wphalogun.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	
	wpwondercannonex.iFlags = QMF_NOTIFY;
	wpwondercannonex.SetRect(200, 225, 235, 81);
	wpwondercannonex.SetPicture(ART_WONDERCEX);
	wpwondercannonex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

	wpwondercannon.iFlags = QMF_NOTIFY;
	wpwondercannon.SetRect(200, 225, 235, 81);
	wpwondercannon.SetPicture(ART_WONDERC);
	wpwondercannon.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

	wprevivegun.iFlags = QMF_NOTIFY;
	wprevivegun.SetRect(200, 225, 235, 81);
	wprevivegun.SetPicture(ART_REVIVEGUN);
	wprevivegun.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

	wpgungnir.iFlags = QMF_NOTIFY;
	wpgungnir.SetRect(200, 225, 235, 81);
	wpgungnir.SetPicture(ART_GUNGNIR);
	wpgungnir.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

	wpsgdrill.iFlags = QMF_NOTIFY;
	wpsgdrill.SetRect(200, 225, 235, 81);
	wpsgdrill.SetPicture(ART_SGDRILL);
	wpsgdrill.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

	wpcannon.iFlags = QMF_NOTIFY;
	wpcannon.SetRect(200, 225, 235, 81);
	wpcannon.SetPicture(ART_CANNON);
	wpcannon.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

	wppaladin.iFlags = QMF_NOTIFY;
	wppaladin.SetRect(200, 225, 235, 81);
	wppaladin.SetPicture(ART_PALADIN);
	wppaladin.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

	wpbuffm4a1.iFlags = QMF_NOTIFY;
	wpbuffm4a1.SetRect(200, 225, 235, 81);
	wpbuffm4a1.SetPicture(ART_BUFFM4A1);
	wpbuffm4a1.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

	wpchainsaw.iFlags = QMF_NOTIFY;
	wpchainsaw.SetRect(200, 225, 235, 81);
	wpchainsaw.SetPicture(ART_CHAINSAW);
	wpchainsaw.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

	wpstarchaserar.iFlags = QMF_NOTIFY;
	wpstarchaserar.SetRect(200, 225, 235, 81);
	wpstarchaserar.SetPicture(ART_STARCHASER);
	wpstarchaserar.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

	wpbow.iFlags = QMF_NOTIFY;
	wpbow.SetRect(200, 225, 235, 81);
	wpbow.SetPicture(ART_BOW);
	wpbow.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

	wpthompson.iFlags = QMF_NOTIFY;
	wpthompson.SetRect(200, 225, 235, 81);
	wpthompson.SetPicture(ART_THOMPSON);
	wpthompson.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

	wptar21.iFlags = QMF_NOTIFY;
	wptar21.SetRect(200, 225, 235, 81);
	wptar21.SetPicture(ART_TAR21);
	wptar21.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

	wpmp7a1.iFlags = QMF_NOTIFY;
	wpmp7a1.SetRect(200, 225, 235, 81);
	wpmp7a1.SetPicture(ART_MP7A1);
	wpmp7a1.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

	wpmg3.iFlags = QMF_NOTIFY;
	wpmg3.SetRect(200, 225, 235, 81);
	wpmg3.SetPicture(ART_MG3);
	wpmg3.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

	wpm1887.iFlags = QMF_NOTIFY;
	wpm1887.SetRect(200, 225, 235, 81);
	wpm1887.SetPicture(ART_M1887);
	wpm1887.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

	wpm95.iFlags = QMF_NOTIFY;
	wpm95.SetRect(200, 225, 235, 81);
	wpm95.SetPicture(ART_M95);
	wpm95.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

	wpm14ebr.iFlags = QMF_NOTIFY;
	wpm14ebr.SetRect(200, 225, 235, 81);
	wpm14ebr.SetPicture(ART_M14EBR);
	wpm14ebr.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

	wpkriss.iFlags = QMF_NOTIFY;
	wpkriss.SetRect(200, 225, 235, 81);
	wpkriss.SetPicture(ART_KRISS);
	wpkriss.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

	wpscar.iFlags = QMF_NOTIFY;
	wpscar.SetRect(200, 225, 235, 81);
	wpscar.SetPicture(ART_SCAR);
	wpscar.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

	wpwa2000.iFlags = QMF_NOTIFY;
	wpwa2000.SetRect(200, 225, 235, 81);
	wpwa2000.SetPicture(ART_WA2000);
	wpwa2000.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

	wpxm8.iFlags = QMF_NOTIFY;
	wpxm8.SetRect(200, 225, 235, 81);
	wpxm8.SetPicture(ART_XM8);
	wpxm8.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

	wpas50.iFlags = QMF_NOTIFY;
	wpas50.SetRect(200, 225, 235, 81);
	wpas50.SetPicture(ART_AS50);
	wpas50.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

/*	
* 
	halogun,
 wondercannonex,
 wondercannon,
revivegun,
gungnir,
 sgdrill,
cannon,
paladin;
buffm4a1,
chainsaw,
starchaserar,
bow, 


thompson,
tar21,
mp7a1, 
mg3, 
m1887,
m95, 
m14ebr, 
kriss,  
scar,
wa2000,
xm8,
as50, 


twinaxes, twinaxesex,  
 dragonsword, dualsword, 
skullaxe, 

    voidpistol,
	voidpistolex,		
	infinity,
	infinityex1, infinityex2, infinitysb, infinitysr, infinityss,*/

	//secondary
	wpvoidpistolex.iFlags = QMF_NOTIFY;
	wpvoidpistolex.SetRect(200, 343, 235, 81);
	wpvoidpistolex.SetPicture(ART_VOIDPISTOLEX);
	wpvoidpistolex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	
	wpvoidpistol.iFlags = QMF_NOTIFY;
	wpvoidpistol.SetRect(200, 343, 235, 81);
	wpvoidpistol.SetPicture(ART_VOIDPISTOL);
	wpvoidpistol.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

	wpinfinity.iFlags = QMF_NOTIFY;
	wpinfinity.SetRect(200, 343, 235, 81);
	wpinfinity.SetPicture(ART_INFINITY);
	wpinfinity.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

	wpinfinityex1.iFlags = QMF_NOTIFY;
	wpinfinityex1.SetRect(200, 343, 235, 81);
	wpinfinityex1.SetPicture(ART_INFINITYEX1);
	wpinfinityex1.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

	wpinfinityex2.iFlags = QMF_NOTIFY;
	wpinfinityex2.SetRect(200, 343, 235, 81);
	wpinfinityex2.SetPicture(ART_INFINITYEX2);
	wpinfinityex2.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

	wpinfinitysb.iFlags = QMF_NOTIFY;
	wpinfinitysb.SetRect(200, 343, 235, 81);
	wpinfinitysb.SetPicture(ART_INFINITYSB);
	wpinfinitysb.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

	wpinfinitysr.iFlags = QMF_NOTIFY;
	wpinfinitysr.SetRect(200, 343, 235, 81);
	wpinfinitysr.SetPicture(ART_INFINITYSR);
	wpinfinitysr.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

	wpinfinityss.iFlags = QMF_NOTIFY;
	wpinfinityss.SetRect(200, 343, 235, 81);
	wpinfinityss.SetPicture(ART_INFINITYSS);
	wpinfinityss.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

	//knife
	wptwinaxesex.iFlags = QMF_NOTIFY;
	wptwinaxesex.SetRect(200, 463, 235, 81);
	wptwinaxesex.SetPicture(ART_DGAXEEX);
	wptwinaxesex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	
	wptwinaxes.iFlags = QMF_NOTIFY;
	wptwinaxes.SetRect(200, 463, 235, 81);
	wptwinaxes.SetPicture(ART_DGAXE);
	wptwinaxes.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

	wpdualsword.iFlags = QMF_NOTIFY;
	wpdualsword.SetRect(200, 463, 235, 81);
	wpdualsword.SetPicture(ART_DUALSWORD);
	wpdualsword.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

	wpdragonsword.iFlags = QMF_NOTIFY;
	wpdragonsword.SetRect(200, 463, 235, 81);
	wpdragonsword.SetPicture(ART_DRAGONSWORD);
	wpdragonsword.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

	wpskullaxe.iFlags = QMF_NOTIFY;
	wpskullaxe.SetRect(200, 463, 235, 81);
	wpskullaxe.SetPicture(ART_SKULLAXE);
	wpskullaxe.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

	wphaloguntex.iFlags = QMF_NOTIFY;
	wphaloguntex.SetRect(590, 237, 220, 55);
	wphaloguntex.SetPicture(ART_HALOGUN);
	wphaloguntex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wphaloguntex.onActivated,
		{
			uiInventory.wphalogun.Show();
			uiInventory.wpwondercannonex.Hide();
			uiInventory.wpwondercannon.Hide();
			uiInventory.wprevivegun.Hide();
			uiInventory.wpgungnir.Hide();
			uiInventory.wpsgdrill.Hide();
			uiInventory.wpcannon.Hide();
			uiInventory.wppaladin.Hide();
			uiInventory.wpbuffm4a1.Hide();
			uiInventory.wpchainsaw.Hide();
			uiInventory.wpstarchaserar.Hide();
			uiInventory.wpbow.Hide();

			uiInventory.wpthompson.Hide();
			uiInventory.wptar21.Hide();
			uiInventory.wpmp7a1.Hide();
			uiInventory.wpmg3.Hide();
			uiInventory.wpm1887.Hide();
			uiInventory.wpm95.Hide();
			uiInventory.wpm14ebr.Hide();
			uiInventory.wpkriss.Hide();
			uiInventory.wpscar.Hide();
			uiInventory.wpwa2000.Hide();
			uiInventory.wpxm8.Hide();
			uiInventory.wpas50.Hide();

			uiInventory.SetGun(1);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	wpwondercannonextex.iFlags = QMF_NOTIFY;
	wpwondercannonextex.SetRect(590, 307, 220, 55);
	wpwondercannonextex.SetPicture(ART_WONDERCEX);
	wpwondercannonextex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wpwondercannonextex.onActivated,
		{
			uiInventory.wphalogun.Hide();
			uiInventory.wpwondercannonex.Show();
			uiInventory.wpwondercannon.Hide();
			uiInventory.wprevivegun.Hide();
			uiInventory.wpgungnir.Hide();
			uiInventory.wpsgdrill.Hide();
			uiInventory.wpcannon.Hide();
			uiInventory.wppaladin.Hide();
			uiInventory.wpbuffm4a1.Hide();
			uiInventory.wpchainsaw.Hide();
			uiInventory.wpstarchaserar.Hide();
			uiInventory.wpbow.Hide();

			uiInventory.wpthompson.Hide();
			uiInventory.wptar21.Hide();
			uiInventory.wpmp7a1.Hide();
			uiInventory.wpmg3.Hide();
			uiInventory.wpm1887.Hide();
			uiInventory.wpm95.Hide();
			uiInventory.wpm14ebr.Hide();
			uiInventory.wpkriss.Hide();
			uiInventory.wpscar.Hide();
			uiInventory.wpwa2000.Hide();
			uiInventory.wpxm8.Hide();
			uiInventory.wpas50.Hide();

			uiInventory.SetGun(2);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	wpwondercannontex.iFlags = QMF_NOTIFY;
	wpwondercannontex.SetRect(590, 377, 220, 55);//y + 70
	wpwondercannontex.SetPicture(ART_WONDERC);
	wpwondercannontex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wpwondercannontex.onActivated,
		{
			uiInventory.wphalogun.Hide();
			uiInventory.wpwondercannonex.Hide();
			uiInventory.wpwondercannon.Show();
			uiInventory.wprevivegun.Hide();
			uiInventory.wpgungnir.Hide();
			uiInventory.wpsgdrill.Hide();
			uiInventory.wpcannon.Hide();
			uiInventory.wppaladin.Hide();
			uiInventory.wpbuffm4a1.Hide();
			uiInventory.wpchainsaw.Hide();
			uiInventory.wpstarchaserar.Hide();
			uiInventory.wpbow.Hide();

			uiInventory.wpthompson.Hide();
			uiInventory.wptar21.Hide();
			uiInventory.wpmp7a1.Hide();
			uiInventory.wpmg3.Hide();
			uiInventory.wpm1887.Hide();
			uiInventory.wpm95.Hide();
			uiInventory.wpm14ebr.Hide();
			uiInventory.wpkriss.Hide();
			uiInventory.wpscar.Hide();
			uiInventory.wpwa2000.Hide();
			uiInventory.wpxm8.Hide();
			uiInventory.wpas50.Hide();

			uiInventory.SetGun(3);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	wpreviveguntex.iFlags = QMF_NOTIFY;
	wpreviveguntex.SetRect(590, 447, 220, 55);//y + 70
	wpreviveguntex.SetPicture(ART_REVIVEGUN);
	wpreviveguntex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wpreviveguntex.onActivated,
		{
			uiInventory.wphalogun.Hide();
			uiInventory.wpwondercannonex.Hide();
			uiInventory.wpwondercannon.Hide();
			uiInventory.wprevivegun.Show();
			uiInventory.wpgungnir.Hide();
			uiInventory.wpsgdrill.Hide();
			uiInventory.wpcannon.Hide();
			uiInventory.wppaladin.Hide();
			uiInventory.wpbuffm4a1.Hide();
			uiInventory.wpchainsaw.Hide();
			uiInventory.wpstarchaserar.Hide();
			uiInventory.wpbow.Hide();

			uiInventory.wpthompson.Hide();
			uiInventory.wptar21.Hide();
			uiInventory.wpmp7a1.Hide();
			uiInventory.wpmg3.Hide();
			uiInventory.wpm1887.Hide();
			uiInventory.wpm95.Hide();
			uiInventory.wpm14ebr.Hide();
			uiInventory.wpkriss.Hide();
			uiInventory.wpscar.Hide();
			uiInventory.wpwa2000.Hide();
			uiInventory.wpxm8.Hide();
			uiInventory.wpas50.Hide();

			uiInventory.SetGun(4);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	wpgungnirtex.iFlags = QMF_NOTIFY;
	wpgungnirtex.SetRect(590, 517, 220, 55);//y + 70
	wpgungnirtex.SetPicture(ART_GUNGNIR);
	wpgungnirtex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wpgungnirtex.onActivated,
		{
			uiInventory.wphalogun.Hide();
			uiInventory.wpwondercannonex.Hide();
			uiInventory.wpwondercannon.Hide();
			uiInventory.wprevivegun.Hide();
			uiInventory.wpgungnir.Show();
			uiInventory.wpsgdrill.Hide();
			uiInventory.wpcannon.Hide();
			uiInventory.wppaladin.Hide();
			uiInventory.wpbuffm4a1.Hide();
			uiInventory.wpchainsaw.Hide();
			uiInventory.wpstarchaserar.Hide();
			uiInventory.wpbow.Hide();

			uiInventory.wpthompson.Hide();
			uiInventory.wptar21.Hide();
			uiInventory.wpmp7a1.Hide();
			uiInventory.wpmg3.Hide();
			uiInventory.wpm1887.Hide();
			uiInventory.wpm95.Hide();
			uiInventory.wpm14ebr.Hide();
			uiInventory.wpkriss.Hide();
			uiInventory.wpscar.Hide();
			uiInventory.wpwa2000.Hide();
			uiInventory.wpxm8.Hide();
			uiInventory.wpas50.Hide();

			uiInventory.SetGun(5);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	wpsgdrilltex.iFlags = QMF_NOTIFY;
	wpsgdrilltex.SetRect(590, 587, 220, 55);//y + 70
	wpsgdrilltex.SetPicture(ART_SGDRILL);
	wpsgdrilltex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wpsgdrilltex.onActivated,
		{
			uiInventory.wphalogun.Hide();
			uiInventory.wpwondercannonex.Hide();
			uiInventory.wpwondercannon.Hide();
			uiInventory.wprevivegun.Hide();
			uiInventory.wpgungnir.Hide();
			uiInventory.wpsgdrill.Show();
			uiInventory.wpcannon.Hide();
			uiInventory.wppaladin.Hide();
			uiInventory.wpbuffm4a1.Hide();
			uiInventory.wpchainsaw.Hide();
			uiInventory.wpstarchaserar.Hide();
			uiInventory.wpbow.Hide();

			uiInventory.wpthompson.Hide();
			uiInventory.wptar21.Hide();
			uiInventory.wpmp7a1.Hide();
			uiInventory.wpmg3.Hide();
			uiInventory.wpm1887.Hide();
			uiInventory.wpm95.Hide();
			uiInventory.wpm14ebr.Hide();
			uiInventory.wpkriss.Hide();
			uiInventory.wpscar.Hide();
			uiInventory.wpwa2000.Hide();
			uiInventory.wpxm8.Hide();
			uiInventory.wpas50.Hide();

			uiInventory.SetGun(6);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	wpcannontex.iFlags = QMF_NOTIFY;
	wpcannontex.SetRect(790, 237, 220, 55);
	wpcannontex.SetPicture(ART_CANNON);
	wpcannontex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wpcannontex.onActivated,
		{
			uiInventory.wphalogun.Hide();
			uiInventory.wpwondercannonex.Hide();
			uiInventory.wpwondercannon.Hide();
			uiInventory.wprevivegun.Hide();
			uiInventory.wpgungnir.Hide();
			uiInventory.wpsgdrill.Hide();
			uiInventory.wpcannon.Show();
			uiInventory.wppaladin.Hide();
			uiInventory.wpbuffm4a1.Hide();
			uiInventory.wpchainsaw.Hide();
			uiInventory.wpstarchaserar.Hide();
			uiInventory.wpbow.Hide();

			uiInventory.wpthompson.Hide();
			uiInventory.wptar21.Hide();
			uiInventory.wpmp7a1.Hide();
			uiInventory.wpmg3.Hide();
			uiInventory.wpm1887.Hide();
			uiInventory.wpm95.Hide();
			uiInventory.wpm14ebr.Hide();
			uiInventory.wpkriss.Hide();
			uiInventory.wpscar.Hide();
			uiInventory.wpwa2000.Hide();
			uiInventory.wpxm8.Hide();
			uiInventory.wpas50.Hide();

			uiInventory.SetGun(7);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	wppaladintex.iFlags = QMF_NOTIFY;
	wppaladintex.SetRect(790, 307, 220, 55);
	wppaladintex.SetPicture(ART_PALADIN);
	wppaladintex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wppaladintex.onActivated,
		{
			uiInventory.wphalogun.Hide();
			uiInventory.wpwondercannonex.Hide();
			uiInventory.wpwondercannon.Hide();
			uiInventory.wprevivegun.Hide();
			uiInventory.wpgungnir.Hide();
			uiInventory.wpsgdrill.Hide();
			uiInventory.wpcannon.Hide();
			uiInventory.wppaladin.Show();
			uiInventory.wpbuffm4a1.Hide();
			uiInventory.wpchainsaw.Hide();
			uiInventory.wpstarchaserar.Hide();
			uiInventory.wpbow.Hide();

			uiInventory.wpthompson.Hide();
			uiInventory.wptar21.Hide();
			uiInventory.wpmp7a1.Hide();
			uiInventory.wpmg3.Hide();
			uiInventory.wpm1887.Hide();
			uiInventory.wpm95.Hide();
			uiInventory.wpm14ebr.Hide();
			uiInventory.wpkriss.Hide();
			uiInventory.wpscar.Hide();
			uiInventory.wpwa2000.Hide();
			uiInventory.wpxm8.Hide();
			uiInventory.wpas50.Hide();

			uiInventory.SetGun(8);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	wpbuffm4a1tex.iFlags = QMF_NOTIFY;
	wpbuffm4a1tex.SetRect(790, 377, 220, 55);//y + 70
	wpbuffm4a1tex.SetPicture(ART_BUFFM4A1);
	wpbuffm4a1tex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wpbuffm4a1tex.onActivated,
		{
			uiInventory.wphalogun.Hide();
			uiInventory.wpwondercannonex.Hide();
			uiInventory.wpwondercannon.Hide();
			uiInventory.wprevivegun.Hide();
			uiInventory.wpgungnir.Hide();
			uiInventory.wpsgdrill.Hide();
			uiInventory.wpcannon.Hide();
			uiInventory.wppaladin.Hide();
			uiInventory.wpbuffm4a1.Show();
			uiInventory.wpchainsaw.Hide();
			uiInventory.wpstarchaserar.Hide();
			uiInventory.wpbow.Hide();

			uiInventory.wpthompson.Hide();
			uiInventory.wptar21.Hide();
			uiInventory.wpmp7a1.Hide();
			uiInventory.wpmg3.Hide();
			uiInventory.wpm1887.Hide();
			uiInventory.wpm95.Hide();
			uiInventory.wpm14ebr.Hide();
			uiInventory.wpkriss.Hide();
			uiInventory.wpscar.Hide();
			uiInventory.wpwa2000.Hide();
			uiInventory.wpxm8.Hide();
			uiInventory.wpas50.Hide();

			uiInventory.SetGun(9);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	wpchainsawtex.iFlags = QMF_NOTIFY;
	wpchainsawtex.SetRect(790, 447, 220, 55);//y + 70
	wpchainsawtex.SetPicture(ART_CHAINSAW);
	wpchainsawtex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wpchainsawtex.onActivated,
		{
			uiInventory.wphalogun.Hide();
			uiInventory.wpwondercannonex.Hide();
			uiInventory.wpwondercannon.Hide();
			uiInventory.wprevivegun.Hide();
			uiInventory.wpgungnir.Hide();
			uiInventory.wpsgdrill.Hide();
			uiInventory.wpcannon.Hide();
			uiInventory.wppaladin.Hide();
			uiInventory.wpbuffm4a1.Hide();
			uiInventory.wpchainsaw.Show();
			uiInventory.wpstarchaserar.Hide();
			uiInventory.wpbow.Hide();

			uiInventory.wpthompson.Hide();
			uiInventory.wptar21.Hide();
			uiInventory.wpmp7a1.Hide();
			uiInventory.wpmg3.Hide();
			uiInventory.wpm1887.Hide();
			uiInventory.wpm95.Hide();
			uiInventory.wpm14ebr.Hide();
			uiInventory.wpkriss.Hide();
			uiInventory.wpscar.Hide();
			uiInventory.wpwa2000.Hide();
			uiInventory.wpxm8.Hide();
			uiInventory.wpas50.Hide();

			uiInventory.SetGun(10);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	wpstarchaserartex.iFlags = QMF_NOTIFY;
	wpstarchaserartex.SetRect(790, 517, 220, 55);//y + 70
	wpstarchaserartex.SetPicture(ART_STARCHASER);
	wpstarchaserartex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wpstarchaserartex.onActivated,
		{
			uiInventory.wphalogun.Hide();
			uiInventory.wpwondercannonex.Hide();
			uiInventory.wpwondercannon.Hide();
			uiInventory.wprevivegun.Hide();
			uiInventory.wpgungnir.Hide();
			uiInventory.wpsgdrill.Hide();
			uiInventory.wpcannon.Hide();
			uiInventory.wppaladin.Hide();
			uiInventory.wpbuffm4a1.Hide();
			uiInventory.wpchainsaw.Hide();
			uiInventory.wpstarchaserar.Show();
			uiInventory.wpbow.Hide();

			uiInventory.wpthompson.Hide();
			uiInventory.wptar21.Hide();
			uiInventory.wpmp7a1.Hide();
			uiInventory.wpmg3.Hide();
			uiInventory.wpm1887.Hide();
			uiInventory.wpm95.Hide();
			uiInventory.wpm14ebr.Hide();
			uiInventory.wpkriss.Hide();
			uiInventory.wpscar.Hide();
			uiInventory.wpwa2000.Hide();
			uiInventory.wpxm8.Hide();
			uiInventory.wpas50.Hide();

			uiInventory.SetGun(11);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	wpbowtex.iFlags = QMF_NOTIFY;
	wpbowtex.SetRect(790, 587, 220, 55);//y + 70
	wpbowtex.SetPicture(ART_BOW);
	wpbowtex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wpbowtex.onActivated,
		{
			uiInventory.wphalogun.Hide();
			uiInventory.wpwondercannonex.Hide();
			uiInventory.wpwondercannon.Hide();
			uiInventory.wprevivegun.Hide();
			uiInventory.wpgungnir.Hide();
			uiInventory.wpsgdrill.Hide();
			uiInventory.wpcannon.Hide();
			uiInventory.wppaladin.Hide();
			uiInventory.wpbuffm4a1.Hide();
			uiInventory.wpchainsaw.Hide();
			uiInventory.wpstarchaserar.Hide();
			uiInventory.wpbow.Show();

			uiInventory.wpthompson.Hide();
			uiInventory.wptar21.Hide();
			uiInventory.wpmp7a1.Hide();
			uiInventory.wpmg3.Hide();
			uiInventory.wpm1887.Hide();
			uiInventory.wpm95.Hide();
			uiInventory.wpm14ebr.Hide();
			uiInventory.wpkriss.Hide();
			uiInventory.wpscar.Hide();
			uiInventory.wpwa2000.Hide();
			uiInventory.wpxm8.Hide();
			uiInventory.wpas50.Hide();

			uiInventory.SetGun(12);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});



	wpthompsontex.iFlags = QMF_NOTIFY;
	wpthompsontex.SetRect(590, 237, 220, 55);
	wpthompsontex.SetPicture(ART_THOMPSON);
	wpthompsontex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wpthompsontex.onActivated,
		{
			uiInventory.wphalogun.Hide();
			uiInventory.wpwondercannonex.Hide();
			uiInventory.wpwondercannon.Hide();
			uiInventory.wprevivegun.Hide();
			uiInventory.wpgungnir.Hide();
			uiInventory.wpsgdrill.Hide();
			uiInventory.wpcannon.Hide();
			uiInventory.wppaladin.Hide();
			uiInventory.wpbuffm4a1.Hide();
			uiInventory.wpchainsaw.Hide();
			uiInventory.wpstarchaserar.Hide();
			uiInventory.wpbow.Hide();

			uiInventory.wpthompson.Show();
			uiInventory.wptar21.Hide();
			uiInventory.wpmp7a1.Hide();
			uiInventory.wpmg3.Hide();
			uiInventory.wpm1887.Hide();
			uiInventory.wpm95.Hide();
			uiInventory.wpm14ebr.Hide();
			uiInventory.wpkriss.Hide();
			uiInventory.wpscar.Hide();
			uiInventory.wpwa2000.Hide();
			uiInventory.wpxm8.Hide();
			uiInventory.wpas50.Hide();

			uiInventory.SetGun(13);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	wptar21tex.iFlags = QMF_NOTIFY;
	wptar21tex.SetRect(590, 307, 220, 55);
	wptar21tex.SetPicture(ART_TAR21);
	wptar21tex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wptar21tex.onActivated,
		{
			uiInventory.wphalogun.Hide();
			uiInventory.wpwondercannonex.Hide();
			uiInventory.wpwondercannon.Hide();
			uiInventory.wprevivegun.Hide();
			uiInventory.wpgungnir.Hide();
			uiInventory.wpsgdrill.Hide();
			uiInventory.wpcannon.Hide();
			uiInventory.wppaladin.Hide();
			uiInventory.wpbuffm4a1.Hide();
			uiInventory.wpchainsaw.Hide();
			uiInventory.wpstarchaserar.Hide();
			uiInventory.wpbow.Hide();

			uiInventory.wpthompson.Hide();
			uiInventory.wptar21.Show();
			uiInventory.wpmp7a1.Hide();
			uiInventory.wpmg3.Hide();
			uiInventory.wpm1887.Hide();
			uiInventory.wpm95.Hide();
			uiInventory.wpm14ebr.Hide();
			uiInventory.wpkriss.Hide();
			uiInventory.wpscar.Hide();
			uiInventory.wpwa2000.Hide();
			uiInventory.wpxm8.Hide();
			uiInventory.wpas50.Hide();

			uiInventory.SetGun(14);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	wpmp7a1tex.iFlags = QMF_NOTIFY;
	wpmp7a1tex.SetRect(590, 377, 220, 55);//y + 70
	wpmp7a1tex.SetPicture(ART_MP7A1);
	wpmp7a1tex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wpmp7a1tex.onActivated,
		{
			uiInventory.wphalogun.Hide();
			uiInventory.wpwondercannonex.Hide();
			uiInventory.wpwondercannon.Hide();
			uiInventory.wprevivegun.Hide();
			uiInventory.wpgungnir.Hide();
			uiInventory.wpsgdrill.Hide();
			uiInventory.wpcannon.Hide();
			uiInventory.wppaladin.Hide();
			uiInventory.wpbuffm4a1.Hide();
			uiInventory.wpchainsaw.Hide();
			uiInventory.wpstarchaserar.Hide();
			uiInventory.wpbow.Hide();

			uiInventory.wpthompson.Hide();
			uiInventory.wptar21.Hide();
			uiInventory.wpmp7a1.Show();
			uiInventory.wpmg3.Hide();
			uiInventory.wpm1887.Hide();
			uiInventory.wpm95.Hide();
			uiInventory.wpm14ebr.Hide();
			uiInventory.wpkriss.Hide();
			uiInventory.wpscar.Hide();
			uiInventory.wpwa2000.Hide();
			uiInventory.wpxm8.Hide();
			uiInventory.wpas50.Hide();

			uiInventory.SetGun(15);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	wpmg3tex.iFlags = QMF_NOTIFY;
	wpmg3tex.SetRect(590, 447, 220, 55);//y + 70
	wpmg3tex.SetPicture(ART_MG3);
	wpmg3tex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wpmg3tex.onActivated,
		{
			uiInventory.wphalogun.Hide();
			uiInventory.wpwondercannonex.Hide();
			uiInventory.wpwondercannon.Hide();
			uiInventory.wprevivegun.Hide();
			uiInventory.wpgungnir.Hide();
			uiInventory.wpsgdrill.Hide();
			uiInventory.wpcannon.Hide();
			uiInventory.wppaladin.Hide();
			uiInventory.wpbuffm4a1.Hide();
			uiInventory.wpchainsaw.Hide();
			uiInventory.wpstarchaserar.Hide();
			uiInventory.wpbow.Hide();

			uiInventory.wpthompson.Hide();
			uiInventory.wptar21.Hide();
			uiInventory.wpmp7a1.Hide();
			uiInventory.wpmg3.Show();
			uiInventory.wpm1887.Hide();
			uiInventory.wpm95.Hide();
			uiInventory.wpm14ebr.Hide();
			uiInventory.wpkriss.Hide();
			uiInventory.wpscar.Hide();
			uiInventory.wpwa2000.Hide();
			uiInventory.wpxm8.Hide();
			uiInventory.wpas50.Hide();

			uiInventory.SetGun(16);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	wpm1887tex.iFlags = QMF_NOTIFY;
	wpm1887tex.SetRect(590, 517, 220, 55);//y + 70
	wpm1887tex.SetPicture(ART_M1887);
	wpm1887tex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wpm1887tex.onActivated,
		{
			uiInventory.wphalogun.Hide();
			uiInventory.wpwondercannonex.Hide();
			uiInventory.wpwondercannon.Hide();
			uiInventory.wprevivegun.Hide();
			uiInventory.wpgungnir.Hide();
			uiInventory.wpsgdrill.Hide();
			uiInventory.wpcannon.Hide();
			uiInventory.wppaladin.Hide();
			uiInventory.wpbuffm4a1.Hide();
			uiInventory.wpchainsaw.Hide();
			uiInventory.wpstarchaserar.Hide();
			uiInventory.wpbow.Hide();

			uiInventory.wpthompson.Hide();
			uiInventory.wptar21.Hide();
			uiInventory.wpmp7a1.Hide();
			uiInventory.wpmg3.Hide();
			uiInventory.wpm1887.Show();
			uiInventory.wpm95.Hide();
			uiInventory.wpm14ebr.Hide();
			uiInventory.wpkriss.Hide();
			uiInventory.wpscar.Hide();
			uiInventory.wpwa2000.Hide();
			uiInventory.wpxm8.Hide();
			uiInventory.wpas50.Hide();

			uiInventory.SetGun(17);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	wpm95tex.iFlags = QMF_NOTIFY;
	wpm95tex.SetRect(590, 587, 220, 55);//y + 70
	wpm95tex.SetPicture(ART_M95);
	wpm95tex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wpm95tex.onActivated,
		{
			uiInventory.wphalogun.Hide();
			uiInventory.wpwondercannonex.Hide();
			uiInventory.wpwondercannon.Hide();
			uiInventory.wprevivegun.Hide();
			uiInventory.wpgungnir.Hide();
			uiInventory.wpsgdrill.Hide();
			uiInventory.wpcannon.Hide();
			uiInventory.wppaladin.Hide();
			uiInventory.wpbuffm4a1.Hide();
			uiInventory.wpchainsaw.Hide();
			uiInventory.wpstarchaserar.Hide();
			uiInventory.wpbow.Hide();

			uiInventory.wpthompson.Hide();
			uiInventory.wptar21.Hide();
			uiInventory.wpmp7a1.Hide();
			uiInventory.wpmg3.Hide();
			uiInventory.wpm1887.Hide();
			uiInventory.wpm95.Show();
			uiInventory.wpm14ebr.Hide();
			uiInventory.wpkriss.Hide();
			uiInventory.wpscar.Hide();
			uiInventory.wpwa2000.Hide();
			uiInventory.wpxm8.Hide();
			uiInventory.wpas50.Hide();

			uiInventory.SetGun(18);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	wpm14ebrtex.iFlags = QMF_NOTIFY;
	wpm14ebrtex.SetRect(790, 237, 220, 55);
	wpm14ebrtex.SetPicture(ART_M14EBR);
	wpm14ebrtex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wpm14ebrtex.onActivated,
		{
			uiInventory.wphalogun.Hide();
			uiInventory.wpwondercannonex.Hide();
			uiInventory.wpwondercannon.Hide();
			uiInventory.wprevivegun.Hide();
			uiInventory.wpgungnir.Hide();
			uiInventory.wpsgdrill.Hide();
			uiInventory.wpcannon.Hide();
			uiInventory.wppaladin.Hide();
			uiInventory.wpbuffm4a1.Hide();
			uiInventory.wpchainsaw.Hide();
			uiInventory.wpstarchaserar.Hide();
			uiInventory.wpbow.Hide();

			uiInventory.wpthompson.Hide();
			uiInventory.wptar21.Hide();
			uiInventory.wpmp7a1.Hide();
			uiInventory.wpmg3.Hide();
			uiInventory.wpm1887.Hide();
			uiInventory.wpm95.Hide();
			uiInventory.wpm14ebr.Show();
			uiInventory.wpkriss.Hide();
			uiInventory.wpscar.Hide();
			uiInventory.wpwa2000.Hide();
			uiInventory.wpxm8.Hide();
			uiInventory.wpas50.Hide();

			uiInventory.SetGun(19);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	wpkrisstex.iFlags = QMF_NOTIFY;
	wpkrisstex.SetRect(790, 307, 220, 55);
	wpkrisstex.SetPicture(ART_KRISS);
	wpkrisstex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wpkrisstex.onActivated,
		{
			uiInventory.wphalogun.Hide();
			uiInventory.wpwondercannonex.Hide();
			uiInventory.wpwondercannon.Hide();
			uiInventory.wprevivegun.Hide();
			uiInventory.wpgungnir.Hide();
			uiInventory.wpsgdrill.Hide();
			uiInventory.wpcannon.Hide();
			uiInventory.wppaladin.Hide();
			uiInventory.wpbuffm4a1.Hide();
			uiInventory.wpchainsaw.Hide();
			uiInventory.wpstarchaserar.Hide();
			uiInventory.wpbow.Hide();

			uiInventory.wpthompson.Hide();
			uiInventory.wptar21.Hide();
			uiInventory.wpmp7a1.Hide();
			uiInventory.wpmg3.Hide();
			uiInventory.wpm1887.Hide();
			uiInventory.wpm95.Hide();
			uiInventory.wpm14ebr.Hide();
			uiInventory.wpkriss.Show();
			uiInventory.wpscar.Hide();
			uiInventory.wpwa2000.Hide();
			uiInventory.wpxm8.Hide();
			uiInventory.wpas50.Hide();

			uiInventory.SetGun(20);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	wpscartex.iFlags = QMF_NOTIFY;
	wpscartex.SetRect(790, 377, 220, 55);//y + 70
	wpscartex.SetPicture(ART_SCAR);
	wpscartex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wpscartex.onActivated,
		{
			uiInventory.wphalogun.Hide();
			uiInventory.wpwondercannonex.Hide();
			uiInventory.wpwondercannon.Hide();
			uiInventory.wprevivegun.Hide();
			uiInventory.wpgungnir.Hide();
			uiInventory.wpsgdrill.Hide();
			uiInventory.wpcannon.Hide();
			uiInventory.wppaladin.Hide();
			uiInventory.wpbuffm4a1.Hide();
			uiInventory.wpchainsaw.Hide();
			uiInventory.wpstarchaserar.Hide();
			uiInventory.wpbow.Hide();

			uiInventory.wpthompson.Hide();
			uiInventory.wptar21.Hide();
			uiInventory.wpmp7a1.Hide();
			uiInventory.wpmg3.Hide();
			uiInventory.wpm1887.Hide();
			uiInventory.wpm95.Hide();
			uiInventory.wpm14ebr.Hide();
			uiInventory.wpkriss.Hide();
			uiInventory.wpscar.Show();
			uiInventory.wpwa2000.Hide();
			uiInventory.wpxm8.Hide();
			uiInventory.wpas50.Hide();

			uiInventory.SetGun(21);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	wpwa2000tex.iFlags = QMF_NOTIFY;
	wpwa2000tex.SetRect(790, 447, 220, 55);//y + 70
	wpwa2000tex.SetPicture(ART_WA2000);
	wpwa2000tex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wpwa2000tex.onActivated,
		{
			uiInventory.wphalogun.Hide();
			uiInventory.wpwondercannonex.Hide();
			uiInventory.wpwondercannon.Hide();
			uiInventory.wprevivegun.Hide();
			uiInventory.wpgungnir.Hide();
			uiInventory.wpsgdrill.Hide();
			uiInventory.wpcannon.Hide();
			uiInventory.wppaladin.Hide();
			uiInventory.wpbuffm4a1.Hide();
			uiInventory.wpchainsaw.Hide();
			uiInventory.wpstarchaserar.Hide();
			uiInventory.wpbow.Hide();

			uiInventory.wpthompson.Hide();
			uiInventory.wptar21.Hide();
			uiInventory.wpmp7a1.Hide();
			uiInventory.wpmg3.Hide();
			uiInventory.wpm1887.Hide();
			uiInventory.wpm95.Hide();
			uiInventory.wpm14ebr.Hide();
			uiInventory.wpkriss.Hide();
			uiInventory.wpscar.Hide();
			uiInventory.wpwa2000.Show();
			uiInventory.wpxm8.Hide();
			uiInventory.wpas50.Hide();

			uiInventory.SetGun(22);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	wpxm8tex.iFlags = QMF_NOTIFY;
	wpxm8tex.SetRect(790, 517, 220, 55);//y + 70
	wpxm8tex.SetPicture(ART_XM8);
	wpxm8tex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wpxm8tex.onActivated,
		{
			uiInventory.wphalogun.Hide();
			uiInventory.wpwondercannonex.Hide();
			uiInventory.wpwondercannon.Hide();
			uiInventory.wprevivegun.Hide();
			uiInventory.wpgungnir.Hide();
			uiInventory.wpsgdrill.Hide();
			uiInventory.wpcannon.Hide();
			uiInventory.wppaladin.Hide();
			uiInventory.wpbuffm4a1.Hide();
			uiInventory.wpchainsaw.Hide();
			uiInventory.wpstarchaserar.Hide();
			uiInventory.wpbow.Hide();

			uiInventory.wpthompson.Hide();
			uiInventory.wptar21.Hide();
			uiInventory.wpmp7a1.Hide();
			uiInventory.wpmg3.Hide();
			uiInventory.wpm1887.Hide();
			uiInventory.wpm95.Hide();
			uiInventory.wpm14ebr.Hide();
			uiInventory.wpkriss.Hide();
			uiInventory.wpscar.Hide();
			uiInventory.wpwa2000.Hide();
			uiInventory.wpxm8.Show();
			uiInventory.wpas50.Hide();

			uiInventory.SetGun(23);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	wpas50tex.iFlags = QMF_NOTIFY;
	wpas50tex.SetRect(790, 587, 220, 55);//y + 70
	wpas50tex.SetPicture(ART_AS50);
	wpas50tex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wpas50tex.onActivated,
		{
			uiInventory.wphalogun.Hide();
			uiInventory.wpwondercannonex.Hide();
			uiInventory.wpwondercannon.Hide();
			uiInventory.wprevivegun.Hide();
			uiInventory.wpgungnir.Hide();
			uiInventory.wpsgdrill.Hide();
			uiInventory.wpcannon.Hide();
			uiInventory.wppaladin.Hide();
			uiInventory.wpbuffm4a1.Hide();
			uiInventory.wpchainsaw.Hide();
			uiInventory.wpstarchaserar.Hide();
			uiInventory.wpbow.Hide();

			uiInventory.wpthompson.Hide();
			uiInventory.wptar21.Hide();
			uiInventory.wpmp7a1.Hide();
			uiInventory.wpmg3.Hide();
			uiInventory.wpm1887.Hide();
			uiInventory.wpm95.Hide();
			uiInventory.wpm14ebr.Hide();
			uiInventory.wpkriss.Hide();
			uiInventory.wpscar.Hide();
			uiInventory.wpwa2000.Hide();
			uiInventory.wpxm8.Hide();
			uiInventory.wpas50.Show();

			uiInventory.SetGun(24);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});



	wpvoidpistolextex.iFlags = QMF_NOTIFY;
	wpvoidpistolextex.SetRect(590, 237, 220, 55);
	wpvoidpistolextex.SetPicture(ART_VOIDPISTOLEX);
	wpvoidpistolextex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wpvoidpistolextex.onActivated,
		{
			uiInventory.wpvoidpistolex.Show();
			uiInventory.wpvoidpistol.Hide();
			uiInventory.wpinfinity.Hide();
			uiInventory.wpinfinityex1.Hide();
			uiInventory.wpinfinityex2.Hide();
			uiInventory.wpinfinitysb.Hide();
			uiInventory.wpinfinitysr.Hide();
			uiInventory.wpinfinityss.Hide();

			uiInventory.SetPistol(1);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	wpvoidpistoltex.iFlags = QMF_NOTIFY;
	wpvoidpistoltex.SetRect(590, 307, 220, 55);
	wpvoidpistoltex.SetPicture(ART_VOIDPISTOL);
	wpvoidpistoltex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wpvoidpistoltex.onActivated,
		{
			uiInventory.wpvoidpistolex.Hide();
			uiInventory.wpvoidpistol.Show();
			uiInventory.wpinfinity.Hide();
			uiInventory.wpinfinityex1.Hide();
			uiInventory.wpinfinityex2.Hide();
			uiInventory.wpinfinitysb.Hide();
			uiInventory.wpinfinitysr.Hide();
			uiInventory.wpinfinityss.Hide();

			uiInventory.SetPistol(2);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	wpinfinitytex.iFlags = QMF_NOTIFY;
	wpinfinitytex.SetRect(590, 377, 220, 55);//y + 70
	wpinfinitytex.SetPicture(ART_INFINITY);
	wpinfinitytex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wpinfinitytex.onActivated,
		{
			uiInventory.wpvoidpistolex.Hide();
			uiInventory.wpvoidpistol.Hide();
			uiInventory.wpinfinity.Show();
			uiInventory.wpinfinityex1.Hide();
			uiInventory.wpinfinityex2.Hide();
			uiInventory.wpinfinitysb.Hide();
			uiInventory.wpinfinitysr.Hide();
			uiInventory.wpinfinityss.Hide();

			uiInventory.SetPistol(3);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	wpinfinityex1tex.iFlags = QMF_NOTIFY;
	wpinfinityex1tex.SetRect(590, 447, 220, 55);//y + 70
	wpinfinityex1tex.SetPicture(ART_INFINITYEX1);
	wpinfinityex1tex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wpinfinityex1tex.onActivated,
		{
			uiInventory.wpvoidpistolex.Hide();
			uiInventory.wpvoidpistol.Hide();
			uiInventory.wpinfinity.Hide();
			uiInventory.wpinfinityex1.Show();
			uiInventory.wpinfinityex2.Hide();
			uiInventory.wpinfinitysb.Hide();
			uiInventory.wpinfinitysr.Hide();
			uiInventory.wpinfinityss.Hide();

			uiInventory.SetPistol(4);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	wpinfinityex2tex.iFlags = QMF_NOTIFY;
	wpinfinityex2tex.SetRect(590, 517, 220, 55);//y + 70
	wpinfinityex2tex.SetPicture(ART_INFINITYEX2);
	wpinfinityex2tex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wpinfinityex2tex.onActivated,
		{
			uiInventory.wpvoidpistolex.Hide();
			uiInventory.wpvoidpistol.Hide();
			uiInventory.wpinfinity.Hide();
			uiInventory.wpinfinityex1.Hide();
			uiInventory.wpinfinityex2.Show();
			uiInventory.wpinfinitysb.Hide();
			uiInventory.wpinfinitysr.Hide();
			uiInventory.wpinfinityss.Hide();

			uiInventory.SetPistol(5);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	wpinfinitysbtex.iFlags = QMF_NOTIFY;
	wpinfinitysbtex.SetRect(590, 587, 220, 55);//y + 70
	wpinfinitysbtex.SetPicture(ART_INFINITYSB);
	wpinfinitysbtex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wpinfinitysbtex.onActivated,
		{
			uiInventory.wpvoidpistolex.Hide();
			uiInventory.wpvoidpistol.Hide();
			uiInventory.wpinfinity.Hide();
			uiInventory.wpinfinityex1.Hide();
			uiInventory.wpinfinityex2.Hide();
			uiInventory.wpinfinitysb.Show();
			uiInventory.wpinfinitysr.Hide();
			uiInventory.wpinfinityss.Hide();

			uiInventory.SetPistol(6);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	wpinfinitysrtex.iFlags = QMF_NOTIFY;
	wpinfinitysrtex.SetRect(790, 237, 220, 55);
	wpinfinitysrtex.SetPicture(ART_INFINITYSR);
	wpinfinitysrtex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wpinfinitysrtex.onActivated,
		{
			uiInventory.wpvoidpistolex.Hide();
			uiInventory.wpvoidpistol.Hide();
			uiInventory.wpinfinity.Hide();
			uiInventory.wpinfinityex1.Hide();
			uiInventory.wpinfinityex2.Hide();
			uiInventory.wpinfinitysb.Hide();
			uiInventory.wpinfinitysr.Show();
			uiInventory.wpinfinityss.Hide();

			uiInventory.SetPistol(7);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	wpinfinitysstex.iFlags = QMF_NOTIFY;
	wpinfinitysstex.SetRect(790, 307, 220, 55);
	wpinfinitysstex.SetPicture(ART_INFINITYSS);
	wpinfinitysstex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wpinfinitysstex.onActivated,
		{
			uiInventory.wpvoidpistolex.Hide();
			uiInventory.wpvoidpistol.Hide();
			uiInventory.wpinfinity.Hide();
			uiInventory.wpinfinityex1.Hide();
			uiInventory.wpinfinityex2.Hide();
			uiInventory.wpinfinitysb.Hide();
			uiInventory.wpinfinitysr.Hide();
			uiInventory.wpinfinityss.Show();

			uiInventory.SetPistol(8);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	wptwinaxesextex.iFlags = QMF_NOTIFY;
	wptwinaxesextex.SetRect(590, 237, 220, 55);
	wptwinaxesextex.SetPicture(ART_DGAXEEX);
	wptwinaxesextex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wptwinaxesextex.onActivated,
		{
			uiInventory.wptwinaxesex.Show();
			uiInventory.wptwinaxes.Hide();
			uiInventory.wpdragonsword.Hide();
			uiInventory.wpdualsword.Hide();
			uiInventory.wpskullaxe.Hide();

			uiInventory.SetKnife(1);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	wptwinaxestex.iFlags = QMF_NOTIFY;
	wptwinaxestex.SetRect(590, 307, 220, 55);
	wptwinaxestex.SetPicture(ART_DGAXE);
	wptwinaxestex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wptwinaxestex.onActivated,
		{
			uiInventory.wptwinaxesex.Hide();
			uiInventory.wptwinaxes.Show();
			uiInventory.wpdragonsword.Hide();
			uiInventory.wpdualsword.Hide();
			uiInventory.wpskullaxe.Hide();

			uiInventory.SetKnife(2);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	wpdualswordtex.iFlags = QMF_NOTIFY;
	wpdualswordtex.SetRect(590, 377, 220, 55);//y + 70
	wpdualswordtex.SetPicture(ART_DUALSWORD);
	wpdualswordtex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wpdualswordtex.onActivated,
		{
			uiInventory.wptwinaxesex.Hide();
			uiInventory.wptwinaxes.Hide();
			uiInventory.wpdragonsword.Hide();
			uiInventory.wpdualsword.Show();
			uiInventory.wpskullaxe.Hide();

			uiInventory.SetKnife(3);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	wpdragonswordtex.iFlags = QMF_NOTIFY;
	wpdragonswordtex.SetRect(590, 447, 220, 55);//y + 70
	wpdragonswordtex.SetPicture(ART_DRAGONSWORD);
	wpdragonswordtex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wpdragonswordtex.onActivated,
		{
			uiInventory.wptwinaxesex.Hide();
			uiInventory.wptwinaxes.Hide();
			uiInventory.wpdragonsword.Show();
			uiInventory.wpdualsword.Hide();
			uiInventory.wpskullaxe.Hide();

			uiInventory.SetKnife(4);
			((CMenuCheckBox*)pSelf)->bChecked = true;
		});

	wpskullaxetex.iFlags = QMF_NOTIFY;
	wpskullaxetex.SetRect(590, 517, 220, 55);//y + 70
	wpskullaxetex.SetPicture(ART_SKULLAXE);
	wpskullaxetex.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(wpskullaxetex.onActivated,
		{
			uiInventory.wptwinaxesex.Hide();
			uiInventory.wptwinaxes.Hide();
			uiInventory.wpdragonsword.Hide();
			uiInventory.wpdualsword.Hide();
			uiInventory.wpskullaxe.Show();

			uiInventory.SetKnife(5);
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

	uparrow.iFlags = QMF_NOTIFY;
	uparrow.SetRect(1025, 395, 25, 25);
	uparrow.SetPicture(ART_UPARROW);
	uparrow.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(uparrow.onActivated,
		{
			if (uiInventory.pagetypeweapon == 1)
			{
				if (uiInventory.stringsprimary != 1)
				{
					uiInventory.stringsprimary--;
					uiInventory.GetPage(1);
				}
				else
				{
					EngFuncs::PlayLocalSound("media/launch_deny2.wav");
				}
			}
			else if (uiInventory.pagetypeweapon == 2)
			{
				if (uiInventory.stringssecondary != 2)
				{
					uiInventory.stringssecondary++;
					uiInventory.GetPage(2);
				}
				else
				{
					EngFuncs::PlayLocalSound("media/launch_deny2.wav");
				}
			}
			else if (uiInventory.pagetypeweapon == 3)
			{
				if (uiInventory.stringsmelee != 2)
				{
					uiInventory.stringsmelee++;
					uiInventory.GetPage(3);
				}
				else
				{
					EngFuncs::PlayLocalSound("media/launch_deny2.wav");
				}
			}		
		});

	downarrow.iFlags = QMF_NOTIFY;
	downarrow.SetRect(1025, 445, 25, 25);
	downarrow.SetPicture(ART_DOWNARROW);
	downarrow.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	SET_EVENT_MULTI(downarrow.onActivated,
		{
			if (uiInventory.pagetypeweapon == 1)
			{
				if (uiInventory.stringsprimary != 2)
				{
					uiInventory.stringsprimary++;
					uiInventory.GetPage(1);
				}
				else
				{
					EngFuncs::PlayLocalSound("media/launch_deny2.wav");
				}
			}
			else if (uiInventory.pagetypeweapon == 2)
			{
				if (uiInventory.stringssecondary != 2)
				{
					uiInventory.stringssecondary++;
					uiInventory.GetPage(2);
				}
				else
				{
					EngFuncs::PlayLocalSound("media/launch_deny2.wav");
				}
			}
			else if (uiInventory.pagetypeweapon == 3)
			{
				if (uiInventory.stringsmelee != 2)
				{
					uiInventory.stringsmelee++;
					uiInventory.GetPage(3);
				}
				else
				{
					EngFuncs::PlayLocalSound("media/launch_deny2.wav");
				}
			}
		});

	prim.SetNameAndStatus(L("Gun"), L(""));
	prim.iFlags |= QMF_NOTIFY;
	prim.SetCoord(600, 190);
	SET_EVENT_MULTI(prim.onActivated,
		{
			uiInventory.GetPage(1);
			EngFuncs::PlayLocalSound("media/launch_deny2.wav");
		});

	secon.SetNameAndStatus(L("Pistol"), L(""));
	secon.iFlags |= QMF_NOTIFY;
	secon.SetCoord(680, 190);
	SET_EVENT_MULTI(secon.onActivated,
		{
			uiInventory.GetPage(2);
			EngFuncs::PlayLocalSound("media/launch_deny2.wav");
		});

	melee.SetNameAndStatus(L("Knife"), L(""));
	melee.onActivated = VoidCb(&CMenuInventory::exits);
	melee.iFlags |= QMF_NOTIFY;
	melee.SetCoord(780, 190);
	SET_EVENT_MULTI(melee.onActivated,
		{
			uiInventory.GetPage(3);
			EngFuncs::PlayLocalSound("media/launch_deny2.wav");
		});

	primtex.SetNameAndStatus(L("CstzUI_WP_Gun"), L(""));
	primtex.onActivated = VoidCb(&CMenuInventory::exits);
	primtex.iFlags |= QMF_INACTIVE;
	primtex.SetCoord(200, 195);

	secontex.SetNameAndStatus(L("CstzUI_WP_Pistol"), L(""));
	secontex.onActivated = VoidCb(&CMenuInventory::exits);
	secontex.iFlags |= QMF_INACTIVE;
	secontex.SetCoord(200, 315);

	meleetex.SetNameAndStatus(L("CstzUI_WP_Knife"), L(""));
	meleetex.onActivated = VoidCb(&CMenuInventory::exits);
	meleetex.iFlags |= QMF_INACTIVE;
	meleetex.SetCoord(200, 435);

	exit.SetNameAndStatus(L("GameUI_Close"), L(""));
	exit.onActivated = VoidCb(&CMenuInventory::Hide);
	exit.iFlags |= QMF_NOTIFY;
	exit.SetCoord(30, 600);
	
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

	AddItem(SetBack9);
	AddItem(SetBack10);
	AddItem(SetBack11);
	AddItem(SetBack12);
	AddItem(SetBack13);

	AddItem(SetBack14);
	AddItem(SetBack15);
	AddItem(SetBack16);
	AddItem(SetBack17);
	AddItem(SetBack18);

	/* Primary */
	AddItem(wphaloguntex);
	AddItem(wpwondercannonextex);
	AddItem(wpwondercannontex);
	AddItem(wpreviveguntex);
	AddItem(wpgungnirtex);
	AddItem(wpsgdrilltex);
	AddItem(wpcannontex);
	AddItem(wppaladintex);
	AddItem(wpbuffm4a1tex);
	AddItem(wpchainsawtex);
	AddItem(wpstarchaserartex);
	AddItem(wpbowtex);

	AddItem(wpthompsontex);
	AddItem(wptar21tex);
	AddItem(wpmp7a1tex);
	AddItem(wpmg3tex);
	AddItem(wpm1887tex);
	AddItem(wpm95tex);
	AddItem(wpm14ebrtex);
	AddItem(wpkrisstex);
	AddItem(wpscartex);
	AddItem(wpwa2000tex);
	AddItem(wpxm8tex);
	AddItem(wpas50tex);

	AddItem(wphalogun);
	AddItem(wpwondercannonex);
	AddItem(wpwondercannon);
	AddItem(wprevivegun);
	AddItem(wpgungnir);
	AddItem(wpsgdrill);
	AddItem(wpcannon);
	AddItem(wppaladin);
	AddItem(wpbuffm4a1);
	AddItem(wpchainsaw);
	AddItem(wpstarchaserar);
	AddItem(wpbow);

	AddItem(wpthompson);
	AddItem(wptar21);
	AddItem(wpmp7a1);
	AddItem(wpmg3);
	AddItem(wpm1887);
	AddItem(wpm95);
	AddItem(wpm14ebr);
	AddItem(wpkriss);
	AddItem(wpscar);
	AddItem(wpwa2000);
	AddItem(wpxm8);
	AddItem(wpas50);
	/* End Primary */

	/* Secondary */
	AddItem(wpvoidpistolextex);
	AddItem(wpvoidpistoltex);
	AddItem(wpinfinitytex);
	AddItem(wpinfinityex1tex);
	AddItem(wpinfinityex2tex);
	AddItem(wpinfinitysbtex);
	AddItem(wpinfinitysrtex);
	AddItem(wpinfinitysstex);

	AddItem(wpvoidpistolex);
	AddItem(wpvoidpistol);
	AddItem(wpinfinity);
	AddItem(wpinfinityex1);
	AddItem(wpinfinityex2);
	AddItem(wpinfinitysb);
	AddItem(wpinfinitysr);
	AddItem(wpinfinityss);
	/* End Secondary */

	/* Melee */
	AddItem(wptwinaxestex);
	AddItem(wptwinaxesextex);
	AddItem(wpdragonswordtex);
	AddItem(wpdualswordtex);
	AddItem(wpskullaxetex);

	AddItem(wptwinaxes);
	AddItem(wptwinaxesex);
	AddItem(wpdragonsword);
	AddItem(wpdualsword);
	AddItem(wpskullaxe);
	/* End Melee */

	AddItem(Setwpas50);
	AddItem(uparrow);
	AddItem(downarrow);
	AddItem(prim);
	AddItem(secon);
	AddItem(melee);
	AddItem(primtex);
	AddItem(secontex);
	AddItem(meleetex);
	AddItem( exit );
}

void CMenuInventory::_VidInit( )
{
	GetPage(1);

	uiInventory.wphalogun.Hide();
	uiInventory.wpwondercannonex.Hide();
	uiInventory.wpwondercannon.Hide();
	uiInventory.wprevivegun.Hide();
	uiInventory.wpgungnir.Hide();
	uiInventory.wpsgdrill.Hide();
	uiInventory.wpcannon.Hide();
	uiInventory.wppaladin.Hide();
	uiInventory.wpbuffm4a1.Hide();
	uiInventory.wpchainsaw.Hide();
	uiInventory.wpstarchaserar.Hide();
	uiInventory.wpbow.Hide();

	uiInventory.wpthompson.Hide();
	uiInventory.wptar21.Hide();
	uiInventory.wpmp7a1.Hide();
	uiInventory.wpmg3.Hide();
	uiInventory.wpm1887.Hide();
	uiInventory.wpm95.Hide();
	uiInventory.wpm14ebr.Hide();
	uiInventory.wpkriss.Hide();
	uiInventory.wpscar.Hide();
	uiInventory.wpwa2000.Hide();
	uiInventory.wpxm8.Hide();
	uiInventory.wpas50.Hide();

	uiInventory.wpvoidpistolex.Hide();
	uiInventory.wpvoidpistol.Hide();
	uiInventory.wpinfinity.Hide();
	uiInventory.wpinfinityex1.Hide();
	uiInventory.wpinfinityex2.Hide();
	uiInventory.wpinfinitysb.Hide();
	uiInventory.wpinfinitysr.Hide();
	uiInventory.wpinfinityss.Hide();

	uiInventory.wptwinaxesex.Hide();
	uiInventory.wptwinaxes.Hide();
	uiInventory.wpdragonsword.Hide();
	uiInventory.wpdualsword.Hide();
	uiInventory.wpskullaxe.Hide();
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
	EngFuncs::PIC_Load( ART_WONDERC	);
	EngFuncs::PIC_Load( ART_WONDERCEX );
	EngFuncs::PIC_Load( ART_XM8 );
}

void UI_Inventory_Menu( void )
{
	uiInventory.Show();
}
ADD_MENU( menu_inventory, UI_Inventory_Precache, UI_Inventory_Menu);