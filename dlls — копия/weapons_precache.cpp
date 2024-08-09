#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "gamerules.h"
#include "weapons.h"
#include "weapons_precache.h"


// called by worldspawn

void W_Precache()
{
	Q_memset(CBasePlayerItem::ItemInfoArray, 0, ARRAYSIZE(CBasePlayerItem::ItemInfoArray));
	Q_memset(CBasePlayerItem::AmmoInfoArray, 0, ARRAYSIZE(CBasePlayerItem::AmmoInfoArray));
	giAmmoIndex = 0;

	// custom items...

	// common world objects
	UTIL_PrecacheOther("item_suit");
	UTIL_PrecacheOther("item_battery");
	UTIL_PrecacheOther("item_antidote");
	UTIL_PrecacheOther("item_security");
	UTIL_PrecacheOther("item_longjump");
	UTIL_PrecacheOther("item_kevlar");
	UTIL_PrecacheOther("item_assaultsuit");
	UTIL_PrecacheOther("item_thighpack");

	// awp magnum
	UTIL_PrecacheOtherWeapon("weapon_awp");
	UTIL_PrecacheOther("ammo_338magnum");

	UTIL_PrecacheOtherWeapon("weapon_g3sg1");
	UTIL_PrecacheOtherWeapon("weapon_ak47");
	UTIL_PrecacheOtherWeapon("weapon_scout");
	UTIL_PrecacheOther("ammo_762nato");

	// m249
	UTIL_PrecacheOtherWeapon("weapon_m249");
	UTIL_PrecacheOther("ammo_556natobox");

	UTIL_PrecacheOtherWeapon("weapon_m4a1");
	UTIL_PrecacheOtherWeapon("weapon_sg552");
	UTIL_PrecacheOtherWeapon("weapon_aug");
	UTIL_PrecacheOtherWeapon("weapon_sg550");
	UTIL_PrecacheOther("ammo_556nato");

	// shotgun
	UTIL_PrecacheOtherWeapon("weapon_m3");
	UTIL_PrecacheOtherWeapon("weapon_xm1014");
	UTIL_PrecacheOther("ammo_buckshot");

	UTIL_PrecacheOtherWeapon("weapon_usp");
	UTIL_PrecacheOtherWeapon("weapon_mac10");
	UTIL_PrecacheOtherWeapon("weapon_ump45");
	UTIL_PrecacheOther("ammo_45acp");

	UTIL_PrecacheOtherWeapon("weapon_fiveseven");
	UTIL_PrecacheOtherWeapon("weapon_p90");
	UTIL_PrecacheOther("ammo_57mm");

	// deagle
	UTIL_PrecacheOtherWeapon("weapon_deagle");
	UTIL_PrecacheOther("ammo_50ae");

	// p228
	UTIL_PrecacheOtherWeapon("weapon_p228");
	UTIL_PrecacheOther("ammo_357sig");

	// knife
	UTIL_PrecacheOtherWeapon("weapon_knife");

	UTIL_PrecacheOtherWeapon("weapon_glock18");
	UTIL_PrecacheOtherWeapon("weapon_mp5navy");
	UTIL_PrecacheOtherWeapon("weapon_tmp");
	UTIL_PrecacheOtherWeapon("weapon_elite");
	UTIL_PrecacheOther("ammo_9mm");

	UTIL_PrecacheOtherWeapon("weapon_flashbang");
	UTIL_PrecacheOtherWeapon("weapon_hegrenade");
	UTIL_PrecacheOtherWeapon("weapon_smokegrenade");
	UTIL_PrecacheOtherWeapon("weapon_c4");
	UTIL_PrecacheOtherWeapon("weapon_galil");
	UTIL_PrecacheOtherWeapon("weapon_famas");

	UTIL_PrecacheOtherWeapon("weapon_mp7a1d");
	UTIL_PrecacheOther("ammo_46mm");
	UTIL_PrecacheOtherWeapon("weapon_ak47l");
	UTIL_PrecacheOtherWeapon("weapon_deagled");
	UTIL_PrecacheOtherWeapon("weapon_wa2000");
	UTIL_PrecacheOtherWeapon("weapon_m95");
	UTIL_PrecacheOtherWeapon("weapon_as50");
	UTIL_PrecacheOther("ammo_50bmg");
	UTIL_PrecacheOtherWeapon("weapon_kriss");
	UTIL_PrecacheOtherWeapon("weapon_thompson");
	UTIL_PrecacheOtherWeapon("weapon_m1887");
	UTIL_PrecacheOtherWeapon("weapon_tar21");
	UTIL_PrecacheOtherWeapon("weapon_xm8c");
	UTIL_PrecacheOtherWeapon("weapon_xm8s");
	UTIL_PrecacheOtherWeapon("weapon_scarl");
	UTIL_PrecacheOtherWeapon("weapon_scarh");
	UTIL_PrecacheOtherWeapon("weapon_m14ebr");

	UTIL_PrecacheOtherWeapon("weapon_cannon");
	UTIL_PrecacheOther("ammo_cannon");
	UTIL_PrecacheOtherWeapon("weapon_gungnir");
	UTIL_PrecacheOther("ammo_gungnir");
	UTIL_PrecacheOtherWeapon("knife_skullaxe");
	UTIL_PrecacheOtherWeapon("weapon_infinityss");
	UTIL_PrecacheOtherWeapon("weapon_infinitysb");
	UTIL_PrecacheOtherWeapon("weapon_infinitysr");
	UTIL_PrecacheOtherWeapon("weapon_mp7a1c");
	UTIL_PrecacheOtherWeapon("weapon_mp7a1p");
	UTIL_PrecacheOtherWeapon("knife_zombi");
	UTIL_PrecacheOtherWeapon("weapon_infinityex1");
	UTIL_PrecacheOtherWeapon("weapon_infinityex2");

	if (g_pGameRules->IsDeathmatch())
	{
		// container for dropped deathmatch weapons
		UTIL_PrecacheOther("weaponbox");
	}

	g_sModelIndexFireball = PRECACHE_MODEL("sprites/zerogxplode.spr");	// fireball
	g_sModelIndexWExplosion = PRECACHE_MODEL("sprites/WXplo1.spr");		// underwater fireball
	g_sModelIndexSmoke = PRECACHE_MODEL("sprites/steam1.spr");		// smoke
	g_sModelIndexBubbles = PRECACHE_MODEL("sprites/bubble.spr");		// bubbles
	g_sModelIndexBloodSpray = PRECACHE_MODEL("sprites/bloodspray.spr");	// initial blood
	g_sModelIndexBloodDrop = PRECACHE_MODEL("sprites/blood.spr");		// splattered blood

	g_sModelIndexSmokePuff = PRECACHE_MODEL("sprites/smokepuff.spr");
	g_sModelIndexFireball2 = PRECACHE_MODEL("sprites/eexplo.spr");
	g_sModelIndexFireball3 = PRECACHE_MODEL("sprites/fexplo.spr");
	g_sModelIndexFireball4 = PRECACHE_MODEL("sprites/fexplo1.spr");
	g_sModelIndexRadio = PRECACHE_MODEL("sprites/radio.spr");

	g_sModelIndexCTGhost = PRECACHE_MODEL("sprites/b-tele1.spr");
	g_sModelIndexTGhost = PRECACHE_MODEL("sprites/c-tele1.spr");
	g_sModelIndexC4Glow = PRECACHE_MODEL("sprites/ledglow.spr");

	g_sModelIndexLaser = PRECACHE_MODEL((char*)g_pModelNameLaser);
	g_sModelIndexLaserDot = PRECACHE_MODEL("sprites/laserdot.spr");

	// used by explosions
	PRECACHE_MODEL("models/grenade.mdl");
	PRECACHE_MODEL("sprites/explode1.spr");

	PRECACHE_SOUND("weapons/debris1.wav");		// explosion aftermaths
	PRECACHE_SOUND("weapons/debris2.wav");		// explosion aftermaths
	PRECACHE_SOUND("weapons/debris3.wav");		// explosion aftermaths

	PRECACHE_SOUND("weapons/grenade_hit1.wav");	// grenade
	PRECACHE_SOUND("weapons/grenade_hit2.wav");	// grenade
	PRECACHE_SOUND("weapons/grenade_hit3.wav");	// grenade

	PRECACHE_SOUND("weapons/bullet_hit1.wav");	// hit by bullet
	PRECACHE_SOUND("weapons/bullet_hit2.wav");	// hit by bullet

	PRECACHE_SOUND("items/weapondrop1.wav");	// weapon falls to the ground
	PRECACHE_SOUND("weapons/generic_reload.wav");
}
