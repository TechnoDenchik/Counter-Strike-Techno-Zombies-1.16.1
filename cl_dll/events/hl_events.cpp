/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
#include "events.h"

int g_iRShell, g_iPShell, g_iBlackSmoke, g_iShotgunShell;

//extern "C"
DECLARE_EVENT(FireAK47);
DECLARE_EVENT(FireAUG);
DECLARE_EVENT(FireAWP);
DECLARE_EVENT(CreateExplo);
DECLARE_EVENT(CreateSmoke);
DECLARE_EVENT(FireDEAGLE);
DECLARE_EVENT(DecalReset);
DECLARE_EVENT(FireEliteLeft);
DECLARE_EVENT(FireEliteRight);
DECLARE_EVENT(FireFAMAS);
DECLARE_EVENT(Fire57);
DECLARE_EVENT(FireG3SG1);
DECLARE_EVENT(FireGALIL);
DECLARE_EVENT(Fireglock18);
DECLARE_EVENT(Knife);
DECLARE_EVENT(FireM249);
DECLARE_EVENT(FireM3);
DECLARE_EVENT(FireM4A1);
DECLARE_EVENT(FireMAC10);
DECLARE_EVENT(FireMP5);
DECLARE_EVENT(FireP228);
DECLARE_EVENT(FireP90);
DECLARE_EVENT(FireScout);
DECLARE_EVENT(FireSG550);
DECLARE_EVENT(FireSG552);
DECLARE_EVENT(FireTMP);
DECLARE_EVENT(FireUMP45);
DECLARE_EVENT(FireUSP);
DECLARE_EVENT(Vehicle);
DECLARE_EVENT(FireXM1014);
DECLARE_EVENT(TrainPitchAdjust);
DECLARE_EVENT(FireAK47L);
DECLARE_EVENT(FireDeagleD_Left);
DECLARE_EVENT(FireDeagleD_Right);
DECLARE_EVENT(FireMP7A1D_Left);
DECLARE_EVENT(FireMP7A1D_Right);
DECLARE_EVENT(FireWA2000);
DECLARE_EVENT(FireM95);
DECLARE_EVENT(FireAS50);
DECLARE_EVENT(FireKRISS);
DECLARE_EVENT(FireTHOMPSON);
DECLARE_EVENT(FireM1887);
DECLARE_EVENT(FireTAR21);
DECLARE_EVENT(FireXM8C);
DECLARE_EVENT(FireXM8S);
DECLARE_EVENT(FireScarH);
DECLARE_EVENT(FireScarL);
DECLARE_EVENT(FireCannon);
DECLARE_EVENT(FireK1a);
DECLARE_EVENT(FireInfinity_Left);
DECLARE_EVENT(FireInfinity_Right);
DECLARE_EVENT(FireSVDEX);
DECLARE_EVENT(FireMP7A1C);
DECLARE_EVENT(FireMP7A1P);
DECLARE_EVENT(FireMG3);
DECLARE_EVENT(FireM14EBR);
DECLARE_EVENT(FireInfinityS);
DECLARE_EVENT(FireVoidPistol);
DECLARE_EVENT(FireVoidPistolEx);
DECLARE_EVENT(FireTwinAxes);
DECLARE_EVENT(FireWonderCannon);
DECLARE_EVENT(FireWonderCannonEx);
DECLARE_EVENT(FireArbalest);
DECLARE_EVENT(FireChainsaw);
DECLARE_EVENT(FireSgDrill);
DECLARE_EVENT(FireGunKata);
DECLARE_EVENT(FireBuffM4A1);
DECLARE_EVENT(FirePaladin);
DECLARE_EVENT(FirePaladin2);
DECLARE_EVENT(FireGuitar);
DECLARE_EVENT(FireBALROG1);
DECLARE_EVENT(Fire2BALROG1);
DECLARE_EVENT(FireStarchaserar);
DECLARE_EVENT(FireBALROG3);
DECLARE_EVENT(Fire2BALROG3);
DECLARE_EVENT(FireBuffAUG);
DECLARE_EVENT(EgonFire);
DECLARE_EVENT(EgonStop);
DECLARE_EVENT(FireAK47DRAGON);
DECLARE_EVENT(FireBLASTER);
DECLARE_EVENT(FireBROAD);
DECLARE_EVENT(FireHK23);
DECLARE_EVENT(FireK3);
DECLARE_EVENT(FireM4A1DRAGON);
DECLARE_EVENT(FireM60E4);
DECLARE_EVENT(FireMP5TIGER);
DECLARE_EVENT(FireP90LAPIN);
DECLARE_EVENT(FirePKM);
DECLARE_EVENT(FireSTUNRIFLE);
DECLARE_EVENT(FireTMPDRAGON);
DECLARE_EVENT(FireHuntBow);
DECLARE_EVENT(FireDesperado);
DECLARE_EVENT(FireBalrog7);
DECLARE_EVENT(FireGatling);
DECLARE_EVENT(FireGatlingex);
DECLARE_EVENT(FireTbarrel);
DECLARE_EVENT(FireStg44);


DECLARE_EVENT(FireMG3xmas);
DECLARE_EVENT(FireMG3);
DECLARE_EVENT(FireM134xmas);
DECLARE_EVENT(FireM134ex);
DECLARE_EVENT(FireM95xmas);
DECLARE_EVENT(FireM2);
DECLARE_EVENT(FireJanus7xmas);

/*
======================
Game_HookEvents

Associate script file name with callback functions.  Callback's must be extern "C" so
 the engine doesn't get confused about name mangling stuff.  Note that the format is
 always the same.  Of course, a clever mod team could actually embed parameters, behavior
 into the actual .sc files and create a .sc file parser and hook their functionality through
 that.. i.e., a scripting system.

That was what we were going to do, but we ran out of time...oh well.
======================
*/
void Game_HookEvents( void )
{
	HOOK_EVENT( ak47, FireAK47 );
	HOOK_EVENT( aug, FireAUG );
	HOOK_EVENT( awp, FireAWP );
	HOOK_EVENT( createexplo, CreateExplo );
	HOOK_EVENT( createsmoke, CreateSmoke );
	HOOK_EVENT( deagle, FireDEAGLE );
	HOOK_EVENT( decal_reset, DecalReset );
	HOOK_EVENT( elite_left, FireEliteLeft );
	HOOK_EVENT( elite_right, FireEliteRight );
	HOOK_EVENT( famas, FireFAMAS );
	HOOK_EVENT( fiveseven, Fire57 );
	HOOK_EVENT( g3sg1, FireG3SG1 );
	HOOK_EVENT( galil, FireGALIL );
	HOOK_EVENT( glock18, Fireglock18 );
	HOOK_EVENT( knife, Knife );
	HOOK_EVENT( m249, FireM249 );
	HOOK_EVENT( m3, FireM3 );
	HOOK_EVENT( m4a1, FireM4A1 );
	HOOK_EVENT( mac10, FireMAC10 );
	HOOK_EVENT( mp5n, FireMP5 );
	HOOK_EVENT( p228, FireP228 );
	HOOK_EVENT( p90, FireP90 );
	HOOK_EVENT( scout, FireScout );
	HOOK_EVENT( sg550, FireSG550 );
	HOOK_EVENT( sg552, FireSG552 );
	HOOK_EVENT( tmp, FireTMP );
	HOOK_EVENT( ump45, FireUMP45 );
	HOOK_EVENT( usp, FireUSP );
	HOOK_EVENT( vehicle, Vehicle );
	HOOK_EVENT( xm1014, FireXM1014 );
	HOOK_EVENT(ak47l, FireAK47L);
	HOOK_EVENT(deagled_left, FireDeagleD_Left);
	HOOK_EVENT(deagled_right, FireDeagleD_Right);
	HOOK_EVENT(mp7a1d_left, FireMP7A1D_Left);
	HOOK_EVENT(mp7a1d_right, FireMP7A1D_Right);
	HOOK_EVENT(wa2000, FireWA2000);
	HOOK_EVENT(m95, FireM95);
	HOOK_EVENT(as50, FireAS50);
	HOOK_EVENT(kriss, FireKRISS);
	HOOK_EVENT(thompson, FireTHOMPSON);
	HOOK_EVENT(m1887, FireM1887);
	HOOK_EVENT(tar21, FireTAR21);
	HOOK_EVENT(xm8c, FireXM8C);
	HOOK_EVENT(xm8s, FireXM8S);
	HOOK_EVENT(scarh, FireScarH);
	HOOK_EVENT(scarl, FireScarL);
	HOOK_EVENT(cannon, FireCannon);
	HOOK_EVENT(k1a, FireK1a);
	HOOK_EVENT(infinity_left, FireInfinity_Left);
	HOOK_EVENT(infinity_right, FireInfinity_Right);
	HOOK_EVENT(svdex, FireSVDEX);
	HOOK_EVENT(mp7a1c, FireMP7A1C);
	HOOK_EVENT(mp7a1p, FireMP7A1P);
	HOOK_EVENT(mg3, FireMG3);
	HOOK_EVENT(m14ebr, FireM14EBR);
	HOOK_EVENT(infinityss, FireInfinityS);
	HOOK_EVENT(infinitysb, FireInfinityS);
	HOOK_EVENT(infinitysr, FireInfinityS);
	HOOK_EVENT(voidpistol, FireVoidPistol);
	HOOK_EVENT(voidpistolex, FireVoidPistolEx);
	HOOK_EVENT(twinaxes, FireTwinAxes);
	HOOK_EVENT(wondercannon, FireWonderCannon);
	HOOK_EVENT(wondercannonex, FireWonderCannonEx);
	HOOK_EVENT(arbalest, FireArbalest);
	HOOK_EVENT(chainsaw, FireChainsaw);
	HOOK_EVENT(sgdrill, FireSgDrill);
	HOOK_EVENT(gunkata, FireGunKata);
	HOOK_EVENT(buffm4a1, FireBuffM4A1);
	HOOK_EVENT(ak47paladin, FirePaladin);
	HOOK_EVENT(ak47paladin2, FirePaladin2);
	HOOK_EVENT(guitar, FireGuitar);
	HOOK_EVENT(balrog1, FireBALROG1);
	HOOK_EVENT(2balrog1, Fire2BALROG1);
	HOOK_EVENT(balrog3, FireBALROG3);
	HOOK_EVENT(2balrog3, Fire2BALROG3);
	HOOK_EVENT(2starchaserar, FireStarchaserar);
	HOOK_EVENT(buffaug, FireBuffAUG);
	HOOK_EVENT(egon_fire, EgonFire);
	HOOK_EVENT(egon_stop, EgonStop);
	HOOK_EVENT(ak47dragon, FireAK47DRAGON);
	HOOK_EVENT(blaster, FireBLASTER);
	HOOK_EVENT(broad, FireBROAD);
	HOOK_EVENT(hk23, FireHK23);
	HOOK_EVENT(k3, FireK3);
	HOOK_EVENT(m4a1dragon, FireM4A1DRAGON);
	HOOK_EVENT(m60e4, FireM60E4);
	HOOK_EVENT(mp5tiger, FireMP5TIGER);
	HOOK_EVENT(p90lapin, FireP90LAPIN);
	HOOK_EVENT(pkm, FirePKM);
	HOOK_EVENT(stunrifle, FireSTUNRIFLE);
	HOOK_EVENT(tmpdragon, FireTMPDRAGON);
	HOOK_EVENT(huntbow, FireHuntBow);
	HOOK_EVENT(desperado, FireDesperado);
	HOOK_EVENT(balrog7, FireBalrog7);
	HOOK_EVENT(gatling, FireGatling);
	HOOK_EVENT(gatlingex, FireGatlingex);
	HOOK_EVENT(tbarrel, FireTbarrel);
	HOOK_EVENT(stg44, FireStg44);

	HOOK_EVENT(mg3xmas, FireMG3xmas);
	HOOK_EVENT(m134xmas, FireM134xmas);
	HOOK_EVENT(m134ex, FireM134ex);
	HOOK_EVENT(m95xmas, FireM95xmas);
	HOOK_EVENT(m2, FireM2);
	HOOK_EVENT(janus7xmas, FireJanus7xmas);
}