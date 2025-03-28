#include "events.h"

enum quantum_e
{
	QUANT_IDLE1,
	QUANT_RELOAD,
	QUANT_DRAW,

	QUANT_SHOOT1,
	QUANT_SHOOT2,
	QUANT_SHOOT3,

	QUANT_REV_IDLE,
	QUANT_REV_RELOAD,

	QUANT_REV_DRAW,
	QUANT_REV_SHOOT1,
	QUANT_REV_SHOOT2,
	QUANT_REV_SHOOT3,

	QUANT_REV
};

static const char *SOUNDS_NAME[] =
{
	"weapons/revivegun-1.wav",
	"weapons/revivegun-2.wav"
};

void EV_Fire3Quantum( event_args_t *args )
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
		gEngfuncs.pEventAPI->EV_WeaponAnimation(Com_RandomLong(QUANT_SHOOT1, QUANT_SHOOT3), 2);
		EV_MuzzleFlash();
		if( !gHUD.cl_righthand->value )
		{
			EV_GetDefaultShellInfo( args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -8.0, -10.0, 0);
		}
		else
		{
			EV_GetDefaultShellInfo( args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -8.0, 10.0, 0);
		}

		if( gHUD.cl_gunsmoke->value )
		{
			cl_entity_t *ent = gEngfuncs.GetViewModel();

			if( ent )
			{
				EV_CS16Client_CreateSmoke( SMOKE_RIFLE, ent->attachment[0], forward, 3, 0.3, 20, 20, 20, false, velocity );
			}
		}
	}
	else
	{
		EV_GetDefaultShellInfo( args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, 4.0, 0);
	}


	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], g_iRShell, TE_BOUNCE_SHELL);

	PLAY_EVENT_SOUND( SOUNDS_NAME[Com_RandomLong(0, 1)] );

	EV_GetGunPosition( args, vecSrc, origin );
	VectorCopy( forward, vecAiming );

	Vector vSpread( args->fparam1, args->fparam2, 0.0f );
	EV_HLDM_FireBullets( idx, forward, right,	up, 1, vecSrc, vecAiming, vSpread, 8192.0, BULLET_PLAYER_762MM, 2 );
}
