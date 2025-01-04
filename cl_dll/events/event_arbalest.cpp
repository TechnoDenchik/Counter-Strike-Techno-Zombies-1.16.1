/*
event_gungnir.cpp
Copyright (C) 2019 Moemod Hymei

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#include "events.h"

enum
{
	ARBALEST_IDLE,
	ARBALEST_RELOAD,
	ARBALEST_DRAW,
	ARBALEST_SHOOT_A,
	ARBALEST_SHOOT_B_START,
	ARBALEST_SHOOT_B_LOOP,
	ARBALEST_SHOOT_B_END,
	ARBALEST_SHOOT_B_CHARGING_LOOP,
	ARBALEST_SHOOT_B_CHARGING_END,
	ARBALEST_SHOOT_C,
	ARBALEST_SHOOT_B_CHARGE,
};

void EV_FireArbalest( event_args_t *args )
{
	vec3_t ShellVelocity, ShellOrigin;
	vec3_t vecSrc, vecAiming;

	int idx = args->entindex;
	Vector origin( args->origin );
	Vector angles(
		args->iparam1 / 100.0f + args->angles[0],
		args->iparam2 / 100.0f + args->angles[1],
		args->angles[2] );
	Vector velocity( args->velocity );
	Vector forward, right, up;
	AngleVectors( angles, forward, right, up );

	if ( EV_IsLocal( args->entindex ) )
	{
		++g_iShotsFired;

		switch (args->iparam1)
		{
		case 0:
		{
			PLAY_EVENT_SOUND("weapons/halogun_idle.wav");
			gEngfuncs.pEventAPI->EV_WeaponAnimation(ARBALEST_IDLE, 2);
			break;
		}
		case 1:
		{
			PLAY_EVENT_SOUND("weapons/halogun-1.wav");
			gEngfuncs.pEventAPI->EV_WeaponAnimation(ARBALEST_SHOOT_A, 2);
			break;
		}
		case 2:
		{
			PLAY_EVENT_SOUND("weapons/halogun_shootB_start.wav");
			gEngfuncs.pEventAPI->EV_WeaponAnimation(ARBALEST_SHOOT_B_START, 2);
			break;
		}
		case 3:
		{
			PLAY_EVENT_SOUND("weapons/halogun-2.wav");
			gEngfuncs.pEventAPI->EV_WeaponAnimation(ARBALEST_SHOOT_B_LOOP, 2);
			break;
		}
		case 4:
		{
			PLAY_EVENT_SOUND("weapons/halogun-2_end.wav");
			gEngfuncs.pEventAPI->EV_WeaponAnimation(ARBALEST_SHOOT_B_END, 2);
			break;
		}
		case 5:
		{
			PLAY_EVENT_SOUND("weapons/gungnir_charge_loop.wav");
			gEngfuncs.pEventAPI->EV_WeaponAnimation(ARBALEST_SHOOT_B_CHARGING_LOOP, 2);
			break;
		}
		case 6:
		{
			PLAY_EVENT_SOUND("weapons/halogun_shootB_charging_end.wav");
			gEngfuncs.pEventAPI->EV_WeaponAnimation(ARBALEST_SHOOT_B_CHARGING_END, 2);
			break;
		}
		case 7:
		{
			PLAY_EVENT_SOUND("weapons/halogun-3.wav");
			gEngfuncs.pEventAPI->EV_WeaponAnimation(ARBALEST_SHOOT_C, 2);
			break;
		}
		case 8:
		{
			PLAY_EVENT_SOUND("weapons/halogun-2_exp1.wav");
			gEngfuncs.pEventAPI->EV_WeaponAnimation(ARBALEST_SHOOT_B_CHARGE, 2);
			break;
		}
		}
			

		EV_MuzzleFlash();
		if( !gHUD.cl_righthand->value )
		{
			EV_GetDefaultShellInfo( args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -8.0, -10.0, 0);
		}
		else
		{
			EV_GetDefaultShellInfo( args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -8.0, 10.0, 0);
		}
	}
	else
	{
		EV_GetDefaultShellInfo( args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, 4.0, 0);
	}


	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[ YAW ], g_iRShell, TE_BOUNCE_SHELL);

	

}