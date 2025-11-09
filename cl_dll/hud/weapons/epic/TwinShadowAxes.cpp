#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"
#include "TwinShadowAxes.h"

#include "parsemsg.h"
#include "r_efx.h"
#include "event_api.h"
#include "com_model.h"
#include "calcscreen.h"
#include "util_vector.h"

#include "cl_entity.h"

#include <string.h>
#include "string.h"
#include "assert.h"
#include <numeric>
#include <tuple>

int CHudTwinAxes::Init(void)
{
	gHUD.AddHudElem(this);

	m_iFlags |= HUD_DRAW;

	return 1;
}

int CHudTwinAxes::VidInit(void)
{
	return 1;
}

bool CHudTwinAxes::CheckForPlayer(cl_entity_s* pEnt)
{
	if (pEnt && pEnt->model && pEnt->model->name[0] && pEnt->player)
		return true;

	return false;
}

int CHudTwinAxes::Draw(float fltime)
{
	if (!m_pCurTexture)
		return 1;

	int x = ScreenWidth / 2;
	int y = ScreenHeight / 4;

	const float flScale = 0.0f;
	const int r = 255, g = 255, b = 255;
	wrect_t rc;

	extra_player_info_t* pplayer;
	TEMPENTITY* temp;
	int spr;

	for (int i = 1; i < 33; i++)
	{
		if (i != gHUD.m_Scoreboard.m_iPlayerNum)
		{
			cl_entity_t* ent = gEngfuncs.GetEntityByIndex(i);
			
		
			/*if (!CheckForPlayer(ent))
				continue;

			Vector origin = ent->origin;
			origin.z += 33.0;

			float xyScreen[2];
			if (CalcScreen(origin, xyScreen))
			{
				gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
				gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
				spr = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/ef_dgaxe_smoke.spr");

				gEngfuncs.pEfxAPI->R_DefaultSprite(vec3_origin, spr, 40);
			}*/
		}
	}

	return 1;
}

