/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "events.h"

enum ak47paladin_e
{
	AK47_IDLE1,
	AK47_RELOAD,
	AK47_DRAW,
	AK47_SHOOT1,
	AK47_SHOOT2,
	AK47_SHOOT3,
};

#ifndef M_PI
#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h
#endif

void EV_PaladinFireEffect(vec3_t vecSrc, vec3_t vecForward, vec3_t vecVelocity, int idx, int iModel)
{
	if (!iModel)
		iModel = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/muzzleflash41.spr");
	TEMPENTITY *ent;
	int i;
	int ran;
	float c, s;
	for (i = 0; i < 12; i++)
	{
		ent = gEngfuncs.pEfxAPI->R_DefaultSprite(vecSrc, iModel, 30);
		if (!ent)
			break;
		ent->flags |= FTENT_CLIENTCUSTOM | FTENT_COLLIDEALL;
		ent->flags |= FTENT_ROTATE;
		ent->entity.curstate.scale = 0.01;
		ent->entity.baseline.fuser1 = 0.1;	// CSO: baseline.fuser2
		ent->callback = [](tempent_s *ent, float frametime, float currenttime)
		{
			if (ent->entity.baseline.fuser1 > ent->entity.curstate.scale)
				ent->entity.curstate.scale += 0.01;
		};

		ent->hitcallback = [](tempent_s *ent, pmtrace_t *tr)
		{
			ent->entity.baseline.origin = ent->entity.baseline.origin * 0.01f;
			ent->entity.curstate.framerate = 60;
		};

		ent->entity.curstate.rendermode = kRenderTransAdd;
		ent->entity.curstate.renderamt = 150;
		ent->entity.curstate.renderfx = kRenderFxFadeSlow;
		ent->entity.origin = vecSrc;

		if (vecVelocity)
		{
			ent->entity.baseline.origin.x += vecVelocity.x - 2.3;
			ent->entity.baseline.origin.y += vecVelocity.y - 2.3;
			ent->entity.baseline.origin.z += vecVelocity.z - 2.3;
		}

		ent->clientIndex = idx;
	}
}

void EV_FirePaladin( event_args_t *args )
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
		gEngfuncs.pEventAPI->EV_WeaponAnimation(AK47_SHOOT1, 2);
		
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

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[ YAW ], g_iRShell, TE_BOUNCE_SHELL);
	
	EV_GetGunPosition( args, vecSrc, origin );
	VectorCopy( forward, vecAiming );
	
	Vector vSpread( args->fparam1, args->fparam2, 0.0f );

	cl_entity_t *ent = gEngfuncs.GetViewModel();

	if (EV_IsLocal(idx))
		EV_PaladinFireEffect(ent->attachment[0], origin, velocity, idx, 0);
	else
		EV_PaladinFireEffect(vecSrc + 40.0 * forward, forward, velocity, idx, 0);
		//EV_PaladinFireEffect(ent->attachment[0], origin, velocity, idx, 0);
}

void EV_PaladinFire2Effect(vec3_t vecSrc, vec3_t vecForward, vec3_t vecVelocity, int idx, int iModel)
{
	if (!iModel)
		iModel = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/muzzleflash42.spr");
	TEMPENTITY* ent;
	int i;
	int ran;
	float c, s;
	for (i = 0; i < 12; i++)
	{
		ent = gEngfuncs.pEfxAPI->R_DefaultSprite(vecSrc, iModel, 30);
		if (!ent)
			break;
		ent->flags |= FTENT_CLIENTCUSTOM | FTENT_COLLIDEALL;
		ent->flags |= FTENT_ROTATE;
		ent->entity.curstate.scale = 0.01;
		ent->entity.baseline.fuser1 = 0.1;	// CSO: baseline.fuser2
		ent->callback = [](tempent_s* ent, float frametime, float currenttime)
			{
				if (ent->entity.baseline.fuser1 > ent->entity.curstate.scale)
					ent->entity.curstate.scale += 0.01;
			};

		ent->hitcallback = [](tempent_s* ent, pmtrace_t* tr)
			{
				ent->entity.baseline.origin = ent->entity.baseline.origin * 0.01f;
				ent->entity.curstate.framerate = 60;
			};

		ent->entity.curstate.rendermode = kRenderTransAdd;
		ent->entity.curstate.renderamt = 150;
		ent->entity.curstate.renderfx = kRenderFxFadeSlow;
		ent->entity.origin = vecSrc;

		if (vecVelocity)
		{
			ent->entity.baseline.origin.x += vecVelocity.x - 2.3;
			ent->entity.baseline.origin.y += vecVelocity.y - 2.3;
			ent->entity.baseline.origin.z += vecVelocity.z - 2.3;
		}

		ent->clientIndex = idx;
	}
}

void EV_FirePaladin2(event_args_t* args)
{
	vec3_t ShellVelocity, ShellOrigin;
	vec3_t vecSrc, vecAiming;

	int idx = args->entindex;
	Vector origin(args->origin);
	Vector angles(
		args->iparam1 / 100.0f + args->angles[0],
		args->iparam2 / 100.0f + args->angles[1],
		args->angles[2]);
	Vector velocity(args->velocity);
	Vector forward, right, up;
	AngleVectors(angles, forward, right, up);

	if (EV_IsLocal(args->entindex))
	{
		++g_iShotsFired;
		gEngfuncs.pEventAPI->EV_WeaponAnimation(AK47_SHOOT3, 2);

		if (!gHUD.cl_righthand->value)
		{
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -8.0, -10.0, 0);
		}
		else
		{
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -8.0, 10.0, 0);
		}

		if (gHUD.cl_gunsmoke->value)
		{
			cl_entity_t* ent = gEngfuncs.GetViewModel();

			if (ent)
			{
				EV_CS16Client_CreateSmoke(SMOKE_RIFLE, ent->attachment[0], forward, 3, 0.3, 20, 20, 20, false, velocity);
			}
		}
	}
	else
	{
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, 4.0, 0);
	}


	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], g_iRShell, TE_BOUNCE_SHELL);

	EV_GetGunPosition(args, vecSrc, origin);
	VectorCopy(forward, vecAiming);

	Vector vSpread(args->fparam1, args->fparam2, 0.0f);

	cl_entity_t* ent = gEngfuncs.GetViewModel();

	if (EV_IsLocal(idx))
		EV_PaladinFire2Effect(ent->attachment[0], origin, velocity, idx, 0);
	else
		EV_PaladinFire2Effect(vecSrc + 40.0 * forward, forward, velocity, idx, 0);
		//EV_PaladinFire2Effect(ent->attachment[0], origin, velocity, idx, 0);
}