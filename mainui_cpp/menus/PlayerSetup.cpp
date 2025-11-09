/*
Copyright (C) 1997-2001 Id Software & TechnoSoftware, Inc.

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
#include "CheckBox.h"
#include "Slider.h"
#include "Field.h"
#include "SpinControl.h"
#include "YesNoMessageBox.h"
#include "PlayerModelView.h"
#include "StringArrayModel.h"

#define ART_BANNER		"gfx/shell/head_customize"

#define ART_HIT0		"gfx/shell/Item/floatingdamage/option_none"
#define ART_HITORIG		"gfx/shell/Item/floatingdamage/option_default"
#define ART_HIT1		"gfx/shell/Item/floatingdamage/option_skin01"
#define ART_HIT2		"gfx/shell/Item/floatingdamage/option_skin02"
#define ART_HIT3		"gfx/shell/Item/floatingdamage/option_skin03"
#define ART_HIT4		"gfx/shell/Item/floatingdamage/option_skin04"
#define ART_HIT5		"gfx/shell/Item/floatingdamage/option_skin05"
#define ART_HIT6		"gfx/shell/Item/floatingdamage/option_skin06"
#define ART_HIT7		"gfx/shell/Item/floatingdamage/option_skin07"
#define ART_HIT8		"gfx/shell/Item/floatingdamage/option_skin08"
#define ART_HIT9		"gfx/shell/Item/floatingdamage/option_skin09"
#define ART_HIT10		"gfx/shell/Item/floatingdamage/option_skin10"
#define ART_HIT11		"gfx/shell/Item/floatingdamage/option_skin11"
#define ART_HIT12		"gfx/shell/Item/floatingdamage/option_skin12"
#define ART_HIT13		"gfx/shell/Item/floatingdamage/option_skin13"
#define ART_HIT14		"gfx/shell/Item/floatingdamage/option_skin14"
#define ART_HIT15		"gfx/shell/Item/floatingdamage/option_skin15"
#define ART_HIT16		"gfx/shell/Item/floatingdamage/option_skin16"
#define ART_HIT17		"gfx/shell/Item/floatingdamage/option_skin17"
#define ART_HIT18		"gfx/shell/Item/floatingdamage/option_skin18"
#define ART_HIT19		"gfx/shell/Item/floatingdamage/option_skin19"
#define ART_HIT20		"gfx/shell/Item/floatingdamage/option_skin20"
#define ART_HIT21		"gfx/shell/Item/floatingdamage/option_skin21"
#define ART_HIT22		"gfx/shell/Item/floatingdamage/option_skin22"
#define ART_HIT23		"gfx/shell/Item/floatingdamage/option_skin23"
#define ART_HIT24		"gfx/shell/Item/floatingdamage/option_skin24"
#define ART_HIT25		"gfx/shell/Item/floatingdamage/option_skin26"
#define ART_HIT26		"gfx/shell/Item/floatingdamage/option_skin27"
#define ART_HIT27		"gfx/shell/Item/floatingdamage/option_skin28"
#define ART_HIT28		"gfx/shell/Item/floatingdamage/option_skin29"
#define ART_HIT29		"gfx/shell/Item/floatingdamage/option_skin30"
#define ART_HIT30		"gfx/shell/Item/floatingdamage/option_skin31"
#define ART_HIT31		"gfx/shell/Item/floatingdamage/option_skin32"
#define ART_HIT32		"gfx/shell/Item/floatingdamage/option_skin33"
#define ART_HIT33		"gfx/shell/Item/floatingdamage/option_skin34"

#define ART_SYSTEM		"gfx/shell/Item/floatingdamage/option_system"
#define MAX_PLAYERMODELS	100

static struct
{
	const char *name;
	unsigned char r;
	unsigned char g;
	unsigned char b;
} g_LogoColors[] =
{
{ "orange", 255, 120, 24  },
{ "yellow",	225, 180, 24  },
{ "blue",   0,   60,  255 },
{ "ltblue", 0,   167, 255 },
{ "green",  0,   167, 0   },
{ "red",    255, 43,  0   },
{ "brown",  123, 73,  0   },
{ "ltgray", 100, 100, 100 },
{ "dkgray", 36,  36,  36  },
};


static byte g_iCrosshairAvailColors[6][3] =
{
	{ 0,   0,   0   },
	{ 50,  250, 50  },
	{ 250, 50,  50  },
	{ 50,  50,  250 },
	{ 250, 250, 50  },
	{ 50,  250, 250 },
};

/**/static const char* g_szCrosshairAvailSizes[] =
{
	"auto", "small", "medium", "large"
};

static const char *g_szCrosshairAvailColors[] =
{
	"Green", "Red", "Blue", "Yellow", "Ltblue"
};

static const char *g_szCrosshairTypes[5] =
{
	"cross", "cross + dot", "circle", "combined", "dot only"
};

static class CMenuPlayerSetup : public CMenuFramework
{
private:
	void _Init() override;
	void _VidInit() override;
public:
	CMenuPlayerSetup() : CMenuFramework( "CMenuPlayerSetup" ), msgBox( true ) { }

	void SetConfig();
	void UpdateLogo();
	void ApplyColorToImagePreview();
	void ApplyColorToLogoPreview();
	void WriteNewLogo();
	void SaveAndPopMenu() override;
	void SetSkinHitIndicator();
	class CMenuVidPreview : public CMenuBitmap {} hitindicatorid;

	class CLogosListModel : public CStringArrayModel
	{
	public:
		CLogosListModel() : CStringArrayModel( (const char *)logos, CS_SIZE, 0 ) {}
		void Update();

	private:
		char logos[MAX_PLAYERMODELS][CS_SIZE];
	} logosModel;

	CMenuField	name;

	class CMenuLogoPreview : public CMenuBaseItem
	{
	public:
		virtual void Draw();
		int r, g, b;
		HIMAGE hImage;
	} logoImage;

	class CMenuCrosshairPreview : public CMenuBaseItem
	{
	public:
		virtual void Draw();
		int r, g, b;
		HIMAGE hWhite;
	} crosshairView;

	CMenuSpinControl	logo;
	CMenuSpinControl		logoColor;
	CMenuSpinControl	crosshairSize;
	CMenuSpinControl	crosshairColor;
	CMenuSpinControl	crosshairType;
	CMenuSpinControl	playerinfo;
	CMenuSpinControl	hitindicator;
	CMenuCheckBox	crosshairTranslucent;
	CMenuCheckBox	uirenderworld;
	CMenuPicButton Exit;
	CMenuYesNoMessageBox msgBox;

} uiPlayerSetup;

void CMenuPlayerSetup::CMenuLogoPreview::Draw()
{
	if( !hImage )
	{
		// draw the background
		UI_FillRect( m_scPos, m_scSize, uiPromptBgColor );

		UI_DrawString( font, m_scPos, m_scSize, "No logo", colorBase, m_scChSize, QM_CENTER, ETF_SHADOW );
	}
	else
	{
		EngFuncs::PIC_Set( hImage, r, g, b, 255 );
		EngFuncs::PIC_Draw( m_scPos, m_scSize );
	}

	// draw the rectangle
	if( eFocusAnimation == QM_HIGHLIGHTIFFOCUS && IsCurrentSelected() )
		UI_DrawRectangle( m_scPos, m_scSize, uiInputTextColor );
	else
		UI_DrawRectangle( m_scPos, m_scSize, uiInputFgColor );

}

void CMenuPlayerSetup::CMenuCrosshairPreview::Draw()
{

	UI_DrawPic(m_scPos, m_scSize, 0x00FFFFFF, "gfx/vgui/crosshair" );


	int l;
	switch( (int)uiPlayerSetup.crosshairSize.GetCurrentValue() )
	{
	case 1:
		l = 10;
		break;
	case 2:
		l = 20;
		break;
	case 3:
		l = 30;
		break;
	case 0:
		if( ScreenWidth < 640 )
			l = 30;
		else if( ScreenWidth < 1024 )
			l = 20;
		else l = 10;
	}

	bool bDrawPoint = false;
	bool bDrawCircle = false;
	bool bDrawCross = false;

	switch ((int)uiPlayerSetup.crosshairType.GetCurrentValue())
	{
	case 1:
	{
		bDrawPoint = true;
		bDrawCross = true;
		break;
	}

	case 2:
	{
		bDrawPoint = true;
		bDrawCircle = true;
		break;
	}

	case 3:
	{
		bDrawPoint = true;
		bDrawCircle = true;
		bDrawCross = true;
		break;
	}

	case 4:
	{
		bDrawPoint = true;
		break;
	}

	default:
	{
		bDrawCross = true;
		break;
	}
	}

	l *= ScreenHeight / 768.0f;

	int x = m_scPos.x, // xpos
		y = m_scPos.y, // ypos
		w = m_scSize.w, // width
		h = m_scSize.h, // height
		// delta distance
		d = (m_scSize.w / 2 - l) * 0.5,
		// alpha
		a = 180,
		// red
		r = g_iCrosshairAvailColors[(int)uiPlayerSetup.crosshairColor.GetCurrentValue()+1][0],
		// green
		g = g_iCrosshairAvailColors[(int)uiPlayerSetup.crosshairColor.GetCurrentValue()+1][1],
		// blue
		b = g_iCrosshairAvailColors[(int)uiPlayerSetup.crosshairColor.GetCurrentValue()+1][2];

	bool additive = uiPlayerSetup.crosshairTranslucent.bChecked;
	bool additive2 = uiPlayerSetup.uirenderworld.bChecked;

	if (bDrawCircle)
	{
		int radius = d + (l / 2);
		int count = radius * 6;

		if (additive)
		{
			EngFuncs::PIC_Set(hWhite, r, g, b, a);
			for (int i = 0; i < count; i++)
				EngFuncs::PIC_DrawTrans(x + w / 2 + radius * cos(2 * M_PI / count * i), y + h / 2 + radius * sin(2 * M_PI / count * i), 1, 1);
		}
		else
		{
			EngFuncs::PIC_Set(hWhite, r, g, b, a);
			for (int i = 0; i < count; i++)
				EngFuncs::PIC_DrawAdditive(x + w / 2 + radius * cos(2 * M_PI / count * i), y + h / 2 + radius * sin(2 * M_PI / count * i), 1, 1);
		}
		if (additive2)
		{
			EngFuncs::PIC_Set(hWhite, r, g, b, a);
			for (int i = 0; i < count; i++)
				EngFuncs::PIC_DrawTrans(x + w / 2 + radius * cos(2 * M_PI / count * i), y + h / 2 + radius * sin(2 * M_PI / count * i), 1, 1);
		}
		else
		{
			EngFuncs::PIC_Set(hWhite, r, g, b, a);
			for (int i = 0; i < count; i++)
				EngFuncs::PIC_DrawAdditive(x + w / 2 + radius * cos(2 * M_PI / count * i), y + h / 2 + radius * sin(2 * M_PI / count * i), 1, 1);
		}
	}

	if (bDrawPoint)
	{
		if (additive)
		{
			EngFuncs::PIC_Set(hWhite, r, g, b, a);
			EngFuncs::PIC_DrawTrans(x + w / 2 - 1, y + h / 2 - 1, 3, 3);
		}
		else
		{
			EngFuncs::PIC_Set(hWhite, r, g, b, a);
			EngFuncs::PIC_DrawAdditive(x + w / 2 - 1, y + h / 2 - 1, 3, 3);
		}
		if (additive2)
		{
			EngFuncs::PIC_Set(hWhite, r, g, b, a);
			EngFuncs::PIC_DrawTrans(x + w / 2 - 1, y + h / 2 - 1, 3, 3);
		}
		else
		{
			EngFuncs::PIC_Set(hWhite, r, g, b, a);
			EngFuncs::PIC_DrawAdditive(x + w / 2 - 1, y + h / 2 - 1, 3, 3);
		}
	}

	if (bDrawCross)
	{
		if (additive)
		{
			// verical
			EngFuncs::PIC_Set(hWhite, r, g, b, a);
			EngFuncs::PIC_DrawTrans(x + w / 2, y + d, 1, l);

			EngFuncs::PIC_Set(hWhite, r, g, b, a);
			EngFuncs::PIC_DrawTrans(x + w / 2, y + h / 2 + d, 1, l);

			// horizontal
			EngFuncs::PIC_Set(hWhite, r, g, b, a);
			EngFuncs::PIC_DrawTrans(x + d, y + h / 2, l, 1);

			EngFuncs::PIC_Set(hWhite, r, g, b, a);
			EngFuncs::PIC_DrawTrans(x + w / 2 + d, y + h / 2, l, 1);
		}
		else
		{
			// verical
			EngFuncs::PIC_Set(hWhite, r, g, b, a);
			EngFuncs::PIC_DrawAdditive(x + w / 2, y + d, 1, l);

			EngFuncs::PIC_Set(hWhite, r, g, b, a);
			EngFuncs::PIC_DrawAdditive(x + w / 2, y + h / 2 + d, 1, l);

			// horizontal
			EngFuncs::PIC_Set(hWhite, r, g, b, a);
			EngFuncs::PIC_DrawAdditive(x + d, y + h / 2, l, 1);

			EngFuncs::PIC_Set(hWhite, r, g, b, a);
			EngFuncs::PIC_DrawAdditive(x + w / 2 + d, y + h / 2, l, 1);
		}
		if (additive2)
		{
			// verical
			EngFuncs::PIC_Set(hWhite, r, g, b, a);
			EngFuncs::PIC_DrawTrans(x + w / 2, y + d, 1, l);

			EngFuncs::PIC_Set(hWhite, r, g, b, a);
			EngFuncs::PIC_DrawTrans(x + w / 2, y + h / 2 + d, 1, l);

			// horizontal
			EngFuncs::PIC_Set(hWhite, r, g, b, a);
			EngFuncs::PIC_DrawTrans(x + d, y + h / 2, l, 1);

			EngFuncs::PIC_Set(hWhite, r, g, b, a);
			EngFuncs::PIC_DrawTrans(x + w / 2 + d, y + h / 2, l, 1);
		}
		else
		{
			// verical
			EngFuncs::PIC_Set(hWhite, r, g, b, a);
			EngFuncs::PIC_DrawAdditive(x + w / 2, y + d, 1, l);

			EngFuncs::PIC_Set(hWhite, r, g, b, a);
			EngFuncs::PIC_DrawAdditive(x + w / 2, y + h / 2 + d, 1, l);

			// horizontal
			EngFuncs::PIC_Set(hWhite, r, g, b, a);
			EngFuncs::PIC_DrawAdditive(x + d, y + h / 2, l, 1);

			EngFuncs::PIC_Set(hWhite, r, g, b, a);
			EngFuncs::PIC_DrawAdditive(x + w / 2 + d, y + h / 2, l, 1);
		}
	}
}

/*
=================
CMenuPlayerSetup::FindLogos

=================
*/
void CMenuPlayerSetup::CLogosListModel::Update( void )
{
	char	**filenames;
	int numFiles, i;

	m_iCount = 0;

	// Get file list
	filenames = EngFuncs::GetFilesList( "logos/*.bmp", &numFiles, FALSE );

	if( !filenames || !numFiles )
	{
		m_iCount = 0;
		return;
	}

	// build the model list
	for( i = 0; i < numFiles; i++ )
	{
		char logoFileName[CS_SIZE];

		Q_strncpy( logoFileName, filenames[i], sizeof( logos[0] ) );
		COM_FileBase( logoFileName, logos[m_iCount] );

		// ignore remapped.bmp
		if( !stricmp( logos[m_iCount], "remapped" ) )
			continue;

		m_iCount++;
	}
}

/*
=================
UI_PlayerSetup_SetConfig
=================
*/
void CMenuPlayerSetup::SetConfig( void )
{
	name.WriteCvar();
	char curColor[CS_SIZE];
	int i = uiPlayerSetup.crosshairColor.GetCurrentValue() + 1;
	snprintf( curColor, CS_SIZE, "%i %i %i",
			  g_iCrosshairAvailColors[i][0],
			  g_iCrosshairAvailColors[i][1],
			  g_iCrosshairAvailColors[i][2]);
	EngFuncs::CvarSetString( "cl_crosshair_color", curColor );
	crosshairSize.WriteCvar();
	crosshairType.WriteCvar();
	crosshairTranslucent.WriteCvar();
	uirenderworld.WriteCvar();
	playerinfo.WriteCvar();
	hitindicator.LinkCvar("hud_hitstyle", CMenuEditable::CVAR_VALUE);
}

void CMenuPlayerSetup::SetSkinHitIndicator()
{
	hitindicator.WriteCvar();
	int hitset = (int)EngFuncs::GetCvarFloat("hud_hitstyle");


	if (hitset == 0)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT0);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 1)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_SYSTEM);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 2)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT1);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 3)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT2);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 4)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT3);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 5)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT4);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 6)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT5);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 7)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT6);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 8)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT7);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 9)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT8);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 10)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT9);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 11)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT10);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 12)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT11);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 13)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT12);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 14)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT13);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 15)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT14);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 16)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT15);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 17)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT16);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 18)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT17);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 19)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT18);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 20)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT19);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 21)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT20);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 22)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT21);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 23)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT22);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 24)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT23);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 25)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT24);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 26)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT25);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 27)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT26);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 28)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT27);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 29)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT28);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 30)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT29);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 31)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT30);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 32)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT31);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 33)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT32);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 34)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT33);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
}

void CMenuPlayerSetup::SaveAndPopMenu()
{
	if( !UI::Names::CheckIsNameValid( name.GetBuffer() ) )
	{
		msgBox.Show();
		return;
	}

	SetConfig();
	CMenuFramework::SaveAndPopMenu();
}

void CMenuPlayerSetup::UpdateLogo()
{
	char image[256];
	const char *mdl = logo.GetCurrentString();

	if( !mdl || !mdl[0] )
	{
		return;
	}

	snprintf( image, 256, "logos/%s.bmp", mdl );
	logoImage.hImage = EngFuncs::PIC_Load( image, 0 );
	ApplyColorToLogoPreview();

	EngFuncs::CvarSetString( "cl_logofile", mdl );
}

void CMenuPlayerSetup::ApplyColorToLogoPreview()
{
	const char *logoColorStr = logoColor.GetCurrentString();

	for( size_t i = 0; i < ARRAYSIZE( g_LogoColors ); i++ )
	{
		if( !stricmp( logoColorStr, g_LogoColors[i].name ))
		{
			logoImage.r = g_LogoColors[i].r;
			logoImage.g = g_LogoColors[i].g;
			logoImage.b = g_LogoColors[i].b;
			return;
		}
	}

	logoImage.r = 255;
	logoImage.g = 255;
	logoImage.b = 255;
}

void CMenuPlayerSetup::WriteNewLogo( void )
{

	char filename[1024];
	CBMP *bmpFile;

	snprintf( filename, sizeof( filename ), "logos/%s.bmp", logo.GetCurrentString() );
	bmpFile = CBMP::LoadFile( filename );

	// not valid logo BMP file
	if( !bmpFile )
		return;

	// remap logo if needed
	bmpFile->RemapLogo( logoImage.r, logoImage.g, logoImage.b );

	EngFuncs::DeleteFile( "custom.hpk" );
	EngFuncs::DeleteFile( "logos/remapped.bmp" );
	EngFuncs::COM_SaveFile( "logos/remapped.bmp", bmpFile->GetBitmap(), bmpFile->GetBitmapHdr()->fileSize );

	delete bmpFile;
}

/*
=================
UI_PlayerSetup_Init
=================
*/
void CMenuPlayerSetup::_Init( void )
{
	SetSkinHitIndicator();
	bool hideModels = false;
	bool hideLogos = false;
	int addFlags = 0;

	static const char* playerinfor[] =
	{
		L("CstzUI_PlayerInfOff"), L("CstzUI_PlayerInfText"), L("CstzUI_PlayerInfTga")
	};

	static const char* hitstyle[] =
	{
		L("CstzUI_PlayerInfOff"),L("CSTUI_System"), 
		L("CSTUI_Hit1"), L("CSTUI_Hit2"), L("CSTUI_Hit3"), L("CSTUI_Hit4"),
		L("CSTUI_Hit5"), L("CSTUI_Hit6"), L("CSTUI_Hit7"), L("CSTUI_Hit8"),
		L("CSTUI_Hit9"), L("CSTUI_Hit10"), L("CSTUI_Hit11"), L("CSTUI_Hit12"),
		L("CSTUI_Hit13"), L("CSTUI_Hit14"), L("CSTUI_Hit15"), L("CSTUI_Hit16"),
		L("CSTUI_Hit17"), L("CSTUI_Hit18"), L("CSTUI_Hit19"), L("CSTUI_Hit20"), 
		L("CSTUI_Hit21"), L("CSTUI_Hit22"), L("CSTUI_Hit23"), L("CSTUI_Hit24"), 
		L("CSTUI_Hit25"), L("CSTUI_Hit26"), L("CSTUI_Hit27"), L("CSTUI_Hit28"),
		L("CSTUI_Hit29"), L("CSTUI_Hit30"), L("CSTUI_Hit31"), L("CSTUI_Hit32"),
		L("CSTUI_Hit33")
	};

	// disable playermodel preview for HLRally to prevent crash
	if( !stricmp( gMenu.m_gameinfo.gamefolder, "hlrally" ))
		hideModels = true;

	// old engine cannot support logo customization, just don't add them
#ifndef NEW_ENGINE_INTERFACE
	hideLogos = true;
#endif

	if( gMenu.m_gameinfo.flags & GFL_NOMODELS )
		addFlags |= QMF_INACTIVE;

	banner.SetPicture(ART_BANNER);

	name.SetNameAndStatus(L("CstzUI_NickName"), L("CstzUI_NickName"));
	name.SetCharSize(QM_BOLDFONT);
	name.iMaxLength = 32;
	name.LinkCvar( "name" );
	name.SetRect( 320, 260, 256, 36 );

	crosshairView.SetRect( 320, 370, 96, 96 );
	crosshairView.SetNameAndStatus(L("CstzUI_Crosshair_preview"), L("CstzUI_Crosshair_preview"));
	crosshairView.SetCharSize(QM_BOLDFONT);
	crosshairView.hWhite = EngFuncs::PIC_Load("*white");

	static CStringArrayModel modelSizes( g_szCrosshairAvailSizes, ARRAYSIZE(g_szCrosshairAvailSizes));

	crosshairSize.SetRect( 480, 345, 256, 26 );
	crosshairSize.SetNameAndStatus(L("CstzUI_Crosshair_size"), L("CstzUI_Crosshair_size"));
	crosshairSize.SetCharSize(QM_BOLDFONT);
	crosshairSize.Setup(&modelSizes);
	crosshairSize.onChanged = CMenuEditable::WriteCvarCb;
	crosshairSize.LinkCvar("cl_crosshair_size", CMenuEditable::CVAR_STRING);
	crosshairSize.font = QM_SMALLFONT;

	static CStringArrayModel modelColors( g_szCrosshairAvailColors, ARRAYSIZE( g_szCrosshairAvailColors ));
	crosshairColor.SetRect( 480, 415, 256, 26 );
	crosshairColor.SetNameAndStatus(L("CstzUI_Crosshair_color"), L("CstzUI_Crosshair_color"));
	crosshairColor.SetCharSize(QM_BOLDFONT);
	crosshairColor.Setup(&modelColors);
	crosshairColor.onChanged = CMenuEditable::WriteCvarCb;
	crosshairColor.font = QM_SMALLFONT;

	static CStringArrayModel modelTypes(g_szCrosshairTypes, ARRAYSIZE(g_szCrosshairTypes));
	crosshairType.SetRect(480, 485, 256, 26);
	crosshairType.SetNameAndStatus(L("CstzUI_Crosshair_type"), L("CstzUI_Crosshair_type"));
	crosshairType.SetCharSize(QM_BOLDFONT);
	crosshairType.Setup(&modelTypes);
	
	crosshairType.LinkCvar("cl_crosshair_type", CMenuEditable::CVAR_VALUE);
	crosshairType.font = QM_SMALLFONT;

	crosshairTranslucent.SetCoord( 320, 540 );
	crosshairTranslucent.SetNameAndStatus(L("CstzUI_Translucent"), L("CstzUI_Translucent"));
	crosshairTranslucent.SetCharSize(QM_BOLDFONT);
	crosshairTranslucent.LinkCvar( "cl_crosshair_translucent" );

	uirenderworld.SetCoord(320, 580);
	uirenderworld.SetNameAndStatus(L("CstzUI_uirender"), L("CstzUI_uirender2"));
	uirenderworld.SetCharSize(QM_BOLDFONT);
	uirenderworld.LinkCvar("ui_renderworld");

	msgBox.SetMessage(L("CstzUI_SetMessage"));
	msgBox.SetCharSize(QM_BOLDFONT);
	msgBox.Link( this );

	static CStringArrayModel model(playerinfor, ARRAYSIZE(playerinfor));
	playerinfo.SetNameAndStatus(L("CstzUI_PlayerInfo"), L("CstzUI_PlayerInfo2"));
	playerinfo.SetCharSize(QM_BOLDFONT);
	playerinfo.Setup(&model);
	playerinfo.onChanged = CMenuEditable::WriteCvarCb;
	playerinfo.font = QM_SMALLFONT;
	playerinfo.LinkCvar("cl_headname", CMenuEditable::CVAR_VALUE);
	playerinfo.SetRect(780, 345, 256, 26);

	static CStringArrayModel modelhit(hitstyle, ARRAYSIZE(hitstyle));
	hitindicator.SetNameAndStatus(L("CstzUI_HitIndicator"), L("CstzUI_HitIndicator2"));
	hitindicator.SetCharSize(QM_BOLDFONT);
	hitindicator.Setup(&modelhit);
	hitindicator.font = QM_SMALLFONT;
	hitindicator.LinkCvar("hud_hitstyle", CMenuEditable::CVAR_VALUE);
	hitindicator.SetRect(780, 485, 412, 26);
	SET_EVENT_MULTI(hitindicator.onChanged,
		{
			CMenuEditable::WriteCvarCb;
			uiPlayerSetup.SetSkinHitIndicator();
		});

	AddItem( background );
	
	Exit.SetNameAndStatus(L("GameUI_Apply"), L(""));
	Exit.SetCharSize(QM_BOLDFONT);
	Exit.onActivated = VoidCb(&CMenuPlayerSetup::SaveAndPopMenu);
	Exit.iFlags |= QMF_NOTIFY;
	Exit.SetCoord(80, 250);

	if( !hideLogos )
	{
		logosModel.Update();
		if( !logosModel.GetRows() )
		{
			// don't add to framework
			hideLogos = true;
		}
		else
		{
			static const char *itemlist[ARRAYSIZE( g_LogoColors )];
			static CStringArrayModel colors( itemlist, ARRAYSIZE( g_LogoColors ) );
			for( size_t i = 0; i < ARRAYSIZE( g_LogoColors ); i++ )
				itemlist[i] = g_LogoColors[i].name;

			logoImage.SetRect( 72, 230 + m_iBtnsNum * 50 + 10, 200, 200 );

			logo.Setup( &logosModel );
			logo.LinkCvar( "cl_logofile", CMenuEditable::CVAR_STRING );
			logo.onChanged = VoidCb( &CMenuPlayerSetup::UpdateLogo );
			logo.SetRect( 72, logoImage.pos.y + logoImage.size.h + UI_OUTLINE_WIDTH, 200, 32 );

			logoColor.Setup( &colors );
			logoColor.LinkCvar( "cl_logocolor", CMenuEditable::CVAR_STRING );
			logoColor.onChanged = VoidCb( &CMenuPlayerSetup::ApplyColorToLogoPreview );;
			logoColor.SetRect( 72, logo.pos.y + logo.size.h + UI_OUTLINE_WIDTH, 200, 32 );
		}
	}

	AddItem( name );
	AddItem( crosshairSize );
	AddItem( crosshairColor );
	AddItem( crosshairType );
	AddItem( crosshairTranslucent );
	AddItem( crosshairView );
	AddItem( uirenderworld );
	AddItem( playerinfo );
	AddItem(hitindicator);
	AddItem(hitindicatorid);
	AddItem(Exit);
	if( !hideLogos )
	{
		AddItem( logo );
		AddItem( logoColor );
		AddItem( logoImage );
		UpdateLogo();
	}
}

void CMenuPlayerSetup::_VidInit(void)
{
	int hitset = (int)EngFuncs::GetCvarFloat("hud_hitstyle");
	hitindicatorid.SetRect(1000, 415, 64, 64);

	if (hitset == 0)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT0);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 1)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_SYSTEM);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 2)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT1);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 3)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT2);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 4)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT3);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 5)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT4);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 6)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT5);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 7)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT6);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 8)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT7);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 9)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT8);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 10)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT9);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 11)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT10);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 12)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT11);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 13)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT12);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 14)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT13);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 15)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT14);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 16)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT15);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 17)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT16);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 18)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT17);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 19)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT18);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 20)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT19);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 21)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT20);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 22)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT21);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 23)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT22);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 24)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT23);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 25)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT24);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 26)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT25);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 27)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT26);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 28)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT27);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 29)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT28);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 30)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT29);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 31)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT30);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 32)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT31);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 33)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT32);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
	else if (hitset == 34)
	{
		hitindicatorid.iFlags = QMF_INACTIVE;
		hitindicatorid.SetPicture(ART_HIT33);
		hitindicatorid.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
	}
}

/*
=================
UI_PlayerSetup_Precache
=================
*/
void UI_PlayerSetup_Precache( void )
{
	EngFuncs::PIC_Load( ART_BANNER );
}

/*
=================
UI_PlayerSetup_Menu
=================
*/
void UI_PlayerSetup_Menu( void )
{
	if ( gMenu.m_gameinfo.gamemode == GAME_SINGLEPLAYER_ONLY )
		return;


	uiPlayerSetup.Show();

	char curColor[CS_SIZE];
	int rgb[3];
	strncpy( curColor, EngFuncs::GetCvarString("cl_crosshair_color"), CS_SIZE);
	sscanf( curColor, "%d %d %d", rgb, rgb + 1, rgb + 2 );

	// check for custom colors
	int i;
	for( i = 1; i < 6; i++)
	{
		if( rgb[0] == g_iCrosshairAvailColors[i][0] &&
			rgb[1] == g_iCrosshairAvailColors[i][1] &&
			rgb[2] == g_iCrosshairAvailColors[i][2] )
		{
			break;
		}
	}

	if( i == 6 )
	{
		g_iCrosshairAvailColors[0][0] = rgb[0];
		g_iCrosshairAvailColors[0][1] = rgb[1];
		g_iCrosshairAvailColors[0][2] = rgb[2];
		uiPlayerSetup.crosshairColor.SetCurrentValue(curColor);
	}
	else
		uiPlayerSetup.crosshairColor.SetCurrentValue(i-1);


}
ADD_MENU( menu_playersetup, UI_PlayerSetup_Precache, UI_PlayerSetup_Menu );
