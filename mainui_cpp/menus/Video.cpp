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

#define ART_BANNER		"gfx/shell/head_video"

/*
=================
UI_Video_Precache
=================
*/
void UI_Video_Precache( void )
{
	EngFuncs::PIC_Load( ART_BANNER );
}


/*
=================
UI_Video_Menu
=================
*/
void UI_Video_Menu( void )
{
	static CMenuFramework video("CMenuVideo");

	static CMenuPicButton Graphic1, Graphic;
	static CMenuPicButton VidoeMode1, VidoeMode;
	static CMenuPicButton Exit1, Exit;

	if( !video.WasInit() )
	{
		video.banner.SetPicture(ART_BANNER);

		Graphic.SetNameAndStatus(L("CstzUI_VideoGraphic"), L(""));
		Graphic.onActivated = UI_VidOptions_Menu;
		Graphic.iFlags |= QMF_NOTIFY;
		Graphic.SetCoord(80, 300);

		VidoeMode.SetNameAndStatus(L("CstzUI_VideoModes"), L(""));
		VidoeMode.onActivated = UI_VidModes_Menu;
		VidoeMode.iFlags |= QMF_NOTIFY;
		VidoeMode.SetCoord(80, 350);

		Exit.SetNameAndStatus(L("GameUI_Close"), L(""));
		Exit.onActivated = VoidCb(&CMenuFramework::Hide);
		Exit.iFlags |= QMF_NOTIFY;
		Exit.SetCoord(80, 400);

		video.AddItem( video.background );
		video.AddItem( video.banner );

		video.AddItem(Graphic);
		video.AddItem(VidoeMode);
		video.AddItem(Exit);
	}

	video.Show();
}
ADD_MENU( menu_video, UI_Video_Precache, UI_Video_Menu );
