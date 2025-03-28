/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "events.h"

enum buffm4a1_e
{
	M4A1_IDLE,
	M4A1_RELOAD,
	M4A1_DRAW,
	M4A1_SHOOT1,
	M4A1_SHOOT2,
	M4A1_SHOOT3,
};

void EV_FireBuffM4A1( event_args_t *args )
{
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	vec3_t vecSrc, vecAiming;
	int    sequence = Com_RandomLong(M4A1_SHOOT1, M4A1_SHOOT3), idx = args->entindex;
	Vector origin( args->origin );
	Vector angles(
		args->iparam1 / 100.0f + args->angles[0],
		args->iparam2 / 100.0f + args->angles[1],
		args->angles[2]
		);
	Vector velocity( args->velocity );
	Vector forward, right, up;

	AngleVectors( angles, forward, right, up );

	if ( EV_IsLocal( idx ) )
	{
		++g_iShotsFired;
		EV_MuzzleFlash();
		if( args->bparam1 )
		{
			sequence = Com_RandomLong( M4A1_SHOOT1, M4A1_SHOOT3 );
		}
		gEngfuncs.pEventAPI->EV_WeaponAnimation(Com_RandomLong(M4A1_SHOOT1, M4A1_SHOOT3), 2);
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

	EV_GetGunPosition( args, vecSrc, origin );
	VectorCopy( forward, vecAiming );
	Vector vSpread;
	
	vSpread.x = args->fparam1;
	vSpread.y = args->fparam2;
	EV_HLDM_FireBullets( idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192.0, BULLET_PLAYER_556MM, 2 );
}