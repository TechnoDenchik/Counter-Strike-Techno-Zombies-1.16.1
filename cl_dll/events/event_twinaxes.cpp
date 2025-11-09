/*
*
*    This program is free software; you can redistribute it and/or modify it
*    under the terms of the GNU General Public License as published by the
*    Free Software Foundation; either version 2 of the License, or (at
*    your option) any later version.
*
*    This program is distributed in the hope that it will be useful, but
*    WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program; if not, write to the Free Software Foundation,
*    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*    In addition, as a special exception, the author gives permission to
*    link the code of this program with the Half-Life Game Engine ("HL
*    Engine") and Modified Game Libraries ("MODs") developed by Valve,
*    L.L.C ("Valve").  You must obey the GNU General Public License in all
*    respects for all of the code used other than the HL Engine and MODs
*    from Valve.  If you modify this file, you may extend this exception
*    to your version of the file, but you are not obligated to do so.  If
*    you do not wish to do so, delete this exception statement from your
*    version.
*
*/
#include "events.h"

enum twinaxes
{
	ANIM_IDLE,
	ANIM_IDLE2,

	ANIM_DRAW,

	ANIM_SLASH1,
	ANIM_SLASH2,
	ANIM_SLASH3,
	ANIM_SLASH4,

	ANIM_DUAL_SLASH1,
	ANIM_DUAL_SLASH2,

	ANIM_COMBO,

	ANIM_SKILL1,
	ANIM_SKILL2,
};

static const char *SOUNDS_NAME = "weapons/dgaxe_slash1.wav";

void EV_FireTwinAxes( struct event_args_s *args )
{
    int idx;
    vec3_t origin;

    // 1. Получаем индекс игрока
    idx = args->entindex;

    // 3. Получаем дополнительные параметры
    int anim_type = args->iparam1;  // ANIM_SKILL1
    int skin_index = args->iparam2;  // 2

    float fparam1 = args->fparam1;     // 0
    float fparam2 = args->fparam2;     // 0

    // 4. Получаем позицию и углы
    VectorCopy(args->origin, origin);

    // 6. Устанавливаем анимацию viewmodel
    cl_entity_t* viewmodel = gEngfuncs.GetViewModel();
    if (viewmodel)
    {
        viewmodel->curstate.skin = skin_index;
    }
}
