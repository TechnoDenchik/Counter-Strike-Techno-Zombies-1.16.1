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

#define ART_BANNER		"gfx/shell/head_audio"

#define ART_CSGO		"gfx/shell/MusicPack/CSGO"
#define ART_CS2			"gfx/shell/MusicPack/CS2"
#define ART_RADCAT		"gfx/shell/MusicPack/RadCat"
#define ART_KLIKSPHILIP	"gfx/shell/MusicPack/3kliksphilip"

#define ART_BBNO		"gfx/shell/MusicPack/bbnos_01"
#define ART_Chipzel		"gfx/shell/MusicPack/chipzel_01"
#define ART_DRYDEN		"gfx/shell/MusicPack/dryden_01"
#define ART_Freaky		"gfx/shell/MusicPack/freakydna_01"
#define ART_ISOxo		"gfx/shell/MusicPack/isoxo_01"
#define ART_Knock2		"gfx/shell/MusicPack/knock2_01"
#define ART_Matt		"gfx/shell/MusicPack/mattlevine_01"
#define ART_Meechy		"gfx/shell/MusicPack/meechydarko_01"
#define ART_Mord		"gfx/shell/MusicPack/mordfustang_01"

class CMenuMusic : public CMenuFramework
{
public:
	typedef CMenuFramework BaseClass;

	CMenuMusic() : CMenuFramework("CMenuMusic") { }

	class CMenuVidPreview : public CMenuBitmap{} testImage;
private:
	void _Init() override;
	void _VidInit() override;
	void GetConfig();
	void SaveAndPopMenu() override;
	void exits() override;

	CMenuSpinControl musicpack;
	CMenuPicButton done, apply;
	CMenuPicButton done2, exit;
	HIMAGE		hTestImage;
};

static CMenuMusic	uiMusic;

void CMenuMusic::GetConfig( void )
{
	musicpack.LinkCvar("menu_musicpack", CMenuEditable::CVAR_VALUE);
}

void CMenuMusic::SaveAndPopMenu()
{
	musicpack.WriteCvar();
	CMenuFramework::SaveAndPopMenu();
}

void CMenuMusic::exits()
{
	musicpack.WriteCvar();
	int musicset = (int)EngFuncs::GetCvarFloat("menu_musicpack");

		if (musicset == 0)
		{
			testImage.iFlags = QMF_INACTIVE;
			testImage.SetRect(540, 425, 240, 240);
			testImage.SetPicture(ART_CSGO);
			testImage.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
			if (!CL_IsActive())
			EngFuncs::PlayBackgroundTrack("Music/valve_01/mainmenu", "Music/valve_01/mainmenu");
		}
		else if (musicset == 1)
		{
			testImage.iFlags = QMF_INACTIVE;
			testImage.SetRect(540, 425, 240, 240);
			testImage.SetPicture(ART_CS2);
			testImage.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
			if (!CL_IsActive())
			EngFuncs::PlayBackgroundTrack("Music/valve_cs2_01/mainmenu", "Music/valve_cs2_01/mainmenu");
		}
		else if (musicset == 2)
		{
			testImage.iFlags = QMF_INACTIVE;
			testImage.SetRect(540, 425, 240, 240);
			testImage.SetPicture(ART_RADCAT);
			testImage.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
			if (!CL_IsActive())
			EngFuncs::PlayBackgroundTrack("Music/radcat_01/mainmenu", "Music/radcat_01/mainmenu");
		}
		else if (musicset == 3)
		{
			testImage.iFlags = QMF_INACTIVE;
			testImage.SetRect(540, 425, 240, 240);
			testImage.SetPicture(ART_KLIKSPHILIP);
			testImage.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
			if (!CL_IsActive())
			EngFuncs::PlayBackgroundTrack("Music/3kliksphilip_01/mainmenu", "Music/3kliksphilip_01/mainmenu");
		}
		else if (musicset == 4)
		{
			testImage.iFlags = QMF_INACTIVE;
			testImage.SetRect(540, 425, 240, 240);
			testImage.SetPicture(ART_BBNO);
			testImage.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
			if (!CL_IsActive())
			EngFuncs::PlayBackgroundTrack("Music/bbnos_01/mainmenu", "Music/bbnos_01/mainmenu");
		}
		else if (musicset == 5)
		{
			testImage.iFlags = QMF_INACTIVE;
			testImage.SetRect(540, 425, 240, 240);
			testImage.SetPicture(ART_Chipzel);
			testImage.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
			if (!CL_IsActive())
			EngFuncs::PlayBackgroundTrack("Music/chipzel_01/mainmenu", "Music/chipzel_01/mainmenu");
		}
		else if (musicset == 6)
		{
			testImage.iFlags = QMF_INACTIVE;
			testImage.SetRect(540, 425, 240, 240);
			testImage.SetPicture(ART_DRYDEN);
			testImage.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
			if (!CL_IsActive())
			EngFuncs::PlayBackgroundTrack("Music/dryden_01/mainmenu", "Music/dryden_01/mainmenu");
		}
		else if (musicset == 7)
		{
			testImage.iFlags = QMF_INACTIVE;
			testImage.SetRect(540, 425, 240, 240);
			testImage.SetPicture(ART_Freaky);
			testImage.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
			if (!CL_IsActive())
			EngFuncs::PlayBackgroundTrack("Music/freakydna_01/mainmenu", "Music/freakydna_01/mainmenu");
		}
		else if (musicset == 8)
		{
			testImage.iFlags = QMF_INACTIVE;
			testImage.SetRect(540, 425, 240, 240);
			testImage.SetPicture(ART_ISOxo);
			testImage.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
			if (!CL_IsActive())
			EngFuncs::PlayBackgroundTrack("Music/isoxo_01/mainmenu", "Music/isoxo_01/mainmenu");
		}
		else if (musicset == 9)
		{
			testImage.iFlags = QMF_INACTIVE;
			testImage.SetRect(540, 425, 240, 240);
			testImage.SetPicture(ART_Knock2);
			testImage.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
			if (!CL_IsActive())
			EngFuncs::PlayBackgroundTrack("Music/knock2_01/mainmenu", "Music/knock2_01/mainmenu");
		}
		else if (musicset == 10)
		{
			testImage.iFlags = QMF_INACTIVE;
			testImage.SetRect(540, 425, 240, 240);
			testImage.SetPicture(ART_Matt);
			testImage.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
			if (!CL_IsActive())
			EngFuncs::PlayBackgroundTrack("Music/mattlevine_01/mainmenu", "Music/mattlevine_01/mainmenu");
		}
		else if (musicset == 11)
		{
			testImage.iFlags = QMF_INACTIVE;
			testImage.SetRect(540, 425, 240, 240);
			testImage.SetPicture(ART_Meechy);
			testImage.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
			if (!CL_IsActive())
			EngFuncs::PlayBackgroundTrack("Music/meechydarko_01/mainmenu", "Music/meechydarko_01/mainmenu");
		}
		else if (musicset == 12)
		{
			testImage.iFlags = QMF_INACTIVE;
			testImage.SetRect(540, 425, 240, 240);
			testImage.SetPicture(ART_Mord);
			testImage.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
			if (!CL_IsActive())
			EngFuncs::PlayBackgroundTrack("Music/mordfustang_01/mainmenu", "Music/mordfustang_01/mainmenu");
		}
	CMenuFramework::exits();
}

void CMenuMusic::_Init( void )
{
	static const char* MusicStr[] =
	{
		"CS:GO", 
		"CS:2", 
		"RadCat", 
		"3kliksphilip", 
		"bbno$—u mad!", 
		"Chipzel—Yellow Magic", 
		"DRYDEN","Freaky DNA—Vici",
		"ISOxo—inhuman",
		"Knock2 — dashstar",
		"Matt Levine—Drifter",
		"Meechy Darko",
		"Mord Fustang"
	};

	banner.SetPicture(ART_BANNER);

	static CStringArrayModel music(MusicStr, ARRAYSIZE(MusicStr));
	musicpack.SetNameAndStatus(L("CstzUI_musicpack"), L("CstzUI_musicpacktext"));
	musicpack.Setup(&music);
	musicpack.onChanged = CMenuEditable::WriteCvarCb;
	musicpack.font = QM_SMALLFONT;
	musicpack.SetRect(510, 340, 300, 32);

	int musicset = (int)EngFuncs::GetCvarFloat("menu_musicpack");

	switch (musicset)
	{
	case 0:
		testImage.iFlags = QMF_INACTIVE;
		testImage.SetRect(540, 425, 240, 240);
		testImage.SetPicture(ART_CSGO);
		testImage.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
		break;
	case 1:
		testImage.iFlags = QMF_INACTIVE;
		testImage.SetRect(540, 425, 240, 240);
		testImage.SetPicture(ART_CS2);
		testImage.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
		break;
	case 2:
		testImage.iFlags = QMF_INACTIVE;
		testImage.SetRect(540, 425, 240, 240);
		testImage.SetPicture(ART_RADCAT);
		testImage.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
		break;
	case 3:
		testImage.iFlags = QMF_INACTIVE;
		testImage.SetRect(540, 425, 240, 240);
		testImage.SetPicture(ART_KLIKSPHILIP);
		testImage.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
		break;
	case 4:
		testImage.iFlags = QMF_INACTIVE;
		testImage.SetRect(540, 425, 240, 240);
		testImage.SetPicture(ART_BBNO);
		testImage.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
		break;
	case 5:
		testImage.iFlags = QMF_INACTIVE;
		testImage.SetRect(540, 425, 240, 240);
		testImage.SetPicture(ART_Chipzel);
		testImage.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
		break;
	case 6:
		testImage.iFlags = QMF_INACTIVE;
		testImage.SetRect(540, 425, 240, 240);
		testImage.SetPicture(ART_DRYDEN);
		testImage.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
		break;
	case 7:
		testImage.iFlags = QMF_INACTIVE;
		testImage.SetRect(540, 425, 240, 240);
		testImage.SetPicture(ART_Freaky);
		testImage.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
		break;
	case 8:
		testImage.iFlags = QMF_INACTIVE;
		testImage.SetRect(540, 425, 240, 240);
		testImage.SetPicture(ART_ISOxo);
		testImage.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
		break;
	case 9:
		testImage.iFlags = QMF_INACTIVE;
		testImage.SetRect(540, 425, 240, 240);
		testImage.SetPicture(ART_Knock2);
		testImage.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
		break;
	case 10:
		testImage.iFlags = QMF_INACTIVE;
		testImage.SetRect(540, 425, 240, 240);
		testImage.SetPicture(ART_Matt);
		testImage.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
		break;
	case 11:
		testImage.iFlags = QMF_INACTIVE;
		testImage.SetRect(540, 425, 240, 240);
		testImage.SetPicture(ART_Meechy);
		testImage.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
		break;
	case 12:
		testImage.iFlags = QMF_INACTIVE;
		testImage.SetRect(540, 425, 240, 240);
		testImage.SetPicture(ART_Mord);
		testImage.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
		break;
	}

	apply.SetNameAndStatus(L("GameUI_Apply"), L(""));
	apply.onActivated = VoidCb(&CMenuMusic::exits);
	apply.iFlags |= QMF_NOTIFY;
	apply.SetCoord(80, 250);

	exit.SetNameAndStatus(L("GameUI_Close"), L(""));
	exit.onActivated = VoidCb(&CMenuMusic::Hide);
	exit.iFlags |= QMF_NOTIFY;
	exit.SetCoord(80, 300);
	
	AddItem( background );
	AddItem( banner );
	AddItem( apply );
	AddItem( exit );
	AddItem( testImage );
	AddItem( musicpack );
}

void CMenuMusic::_VidInit( )
{
	GetConfig();
}

void UI_Music_Precache( void )
{
	EngFuncs::PIC_Load( ART_BANNER );
	EngFuncs::PIC_Load( ART_CSGO);
	EngFuncs::PIC_Load( ART_CS2);
	EngFuncs::PIC_Load( ART_RADCAT);
	EngFuncs::PIC_Load( ART_KLIKSPHILIP);
}

void UI_Music_Menu( void )
{
	uiMusic.Show();
}
ADD_MENU( menu_music, UI_Music_Precache, UI_Music_Menu);
