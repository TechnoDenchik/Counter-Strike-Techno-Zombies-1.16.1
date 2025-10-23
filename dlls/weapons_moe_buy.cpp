/*
weapons_moe_buy.cpp - CSMoE Gameplay server : Weapon buy command handler
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

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "gamerules.h"
#include "client.h"
#include "weapons_moe_buy.h"

#include <algorithm>

#ifndef CLIENT_DLL
#include "gamemode/mods.h"
#endif

struct MoEWeaponBuyInfo_s
{
	const char *pszClassName;
	const char *pszDisplayName;
	int iCost;
	InventorySlotType iSlot;
	TeamName team;
};

static MoEWeaponBuyInfo_s g_MoEWeaponBuyInfo[] = {
	{ "weapon_infinity", "Infinity Black/Sliver", 1500, PISTOL_SLOT, UNASSIGNED },
	{ "weapon_infinityex1", "Infinity Red/Sliver", 1500, PISTOL_SLOT, UNASSIGNED },
	{ "weapon_infinityex2", "Infinity Red/Gold", 1500, PISTOL_SLOT, UNASSIGNED },
	{ "weapon_infinityss", "Infinity Sliver", 600, PISTOL_SLOT, UNASSIGNED },
	{ "weapon_infinitysr", "Infinity Red",600, PISTOL_SLOT, UNASSIGNED },
	{ "weapon_infinitysb", "Infinity Black",600, PISTOL_SLOT, UNASSIGNED },
	{ "weapon_deagled", "Dual Deagle",760, PISTOL_SLOT, UNASSIGNED },
	
	{ "weapon_m1887", "M1887", 2800, PRIMARY_WEAPON_SLOT, UNASSIGNED },

	{ "weapon_kriss", "Kriss", 1700, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_k1a", "K1A", 1850, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_thompson", "Thompson", 2000, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_mp7a1c", "MP7A1", 2150, PRIMARY_WEAPON_SLOT, UNASSIGNED },

	{ "weapon_m14ebr", "M14 EBR", 3100, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_buffaug", "BuffAug", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_xm8c", "XM8", 3250, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_scarl", "Scar", 3250, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_tar21", "TAR-21", 3600, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_sgdrill",         "Magnum Drill",                 3400, PRIMARY_WEAPON_SLOT, UNASSIGNED},
	{ "weapon_buffm4a1",         "M4A1 Buff",                 3000, PRIMARY_WEAPON_SLOT, UNASSIGNED},
	{ "weapon_paladin", "AK-47 Paladin", 2700, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_m95", "Barrett M95", 5000, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_chainsaw",         "Ripper",                  7500, PRIMARY_WEAPON_SLOT, UNASSIGNED},
	{ "weapon_wa2000", "WA2000", 4600, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_as50", "AS50", 5000, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_starchaserar",      "Star Chaser AR",               5000, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_mg3", "MG-3", 5750, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_bow",      "BOW",               6200, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_cannon", "Blackdragon Cannon", 3800, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_gungnir", "Gungnir", 6700, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_quantum", "QuantumHorizon", 8800, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_arbalest", "Arbalest", 6300, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_wondercannon", "Heaven Splitter", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_wondercannonex", "Asura Hell Splitter", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },

	{ "weapon_shelteraxe", "ShelterAxe", 0, KNIFE_SLOT, UNASSIGNED },
	{ "weapon_voidpistol", "VoidPistol", 3400, PISTOL_SLOT, UNASSIGNED },
	{ "weapon_voidpistolex", "VoidPistolEx", 3300, PISTOL_SLOT, UNASSIGNED },

	{ "weapon_desperado", "Python Desperado", 0, PISTOL_SLOT, UNASSIGNED },

	{ "weapon_knife", "Seal Knife", 0, KNIFE_SLOT, UNASSIGNED },
	{ "knife_skullaxe", "Skull-9", 0, KNIFE_SLOT, UNASSIGNED },
	{ "knife_dragonsword", "Green Dragon Crescent Blade", 0, KNIFE_SLOT, UNASSIGNED },
	{ "knife_dualsword", "Dual Sword Phantom Slayer",0, KNIFE_SLOT, UNASSIGNED},
	{ "weapon_twinaxes", "Twin Shadow Axes", 0, KNIFE_SLOT, UNASSIGNED },
	{ "weapon_twinaxesex", "Twin Shadow AxesEx", 0, KNIFE_SLOT, UNASSIGNED },

	{ "knife_thanatos9", "Thanatos 9", 0, KNIFE_SLOT, UNASSIGNED },
	{ "knife_stormgiant", "Storm Giant", 0, KNIFE_SLOT, UNASSIGNED },
	{ "knife_snakesword", "Snake Sword", 0, KNIFE_SLOT, UNASSIGNED },
	{ "knife_nataknifed", "Nata Knife", 0, KNIFE_SLOT, UNASSIGNED },
	{ "knife_katanad", "Dual Katana", 0, KNIFE_SLOT, UNASSIGNED },
	{ "knife_dragontail", "Dragon Claw", 0, KNIFE_SLOT, UNASSIGNED },
	{ "knife_zsh_clawhammer", "Claw Hammer", 0, KNIFE_SLOT, UNASSIGNED },
	{ "knife_balrog9", "Balrog 9", 0, KNIFE_SLOT, UNASSIGNED },

	{ "weapon_balrog1", "Balrog 1", 600, PISTOL_SLOT, UNASSIGNED },

	{ "weapon_balrog3", "Balrog 3", 2700, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_balrog5", "Balrog 5", 3300, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_balrog7", "Balrog 7", 4800, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_balrog11", "Balrog 11", 3100, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_guitar", "Guitar", 3000, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_m134hero", "M134 Hero", 5400, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_plasmagun", "Plasma Gun", 3600, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_poisongun", "Poison Gun", 3800, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_ethereal", "Ethereal", 2450, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_spas12ex", "SPAS-12 EX", 2600, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_violingun", "Violin Gun", 3250, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_sbmine", "Sattelite Mine", 0, GRENADE_SLOT, UNASSIGNED },

	{ "weapon_ak47dragon", "AK-47 Dragon", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_blaster", "Coil MG", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_broad", "Broad Divine", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_hk23", "HK-23", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_k3", "K3", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_m4a1dragon", "M4A1 Dragon", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_m60e4", "M60E4", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_mp5tiger", "MP5-Tiger", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_p90lapin", "P90-Lapin", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_pkm", "PKM", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_stunrifle", "Stun Rifle", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_tmpdragon", "TMP Dragon", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },

	{ "weapon_huntbow", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },

	{ "weapon_gatling", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_gatlingex", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },

	{ "weapon_tbarrel", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_stg44", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_svdex", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },

	{ "weapon_mg3xmas", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_mg3", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_m134xmas", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_m134ex", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_m95xmas", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_m2", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_janus7xmas", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
};

static MoEWeaponBuyInfo_s g_MoEWeaponBuyInfo2[] = {
	{ "weapon_infinity", "Infinity Black/Sliver", 0, PISTOL_SLOT, UNASSIGNED },
	{ "weapon_infinityex1", "Infinity Red/Sliver", 0, PISTOL_SLOT, UNASSIGNED },
	{ "weapon_infinityex2", "Infinity Red/Gold", 0, PISTOL_SLOT, UNASSIGNED },
	{ "weapon_infinityss", "Infinity Sliver", 0, PISTOL_SLOT, UNASSIGNED },
	{ "weapon_infinitysr", "Infinity Red",0, PISTOL_SLOT, UNASSIGNED },
	{ "weapon_infinitysb", "Infinity Black",0, PISTOL_SLOT, UNASSIGNED },
	{ "weapon_deagled", "Dual Deagle",0, PISTOL_SLOT, UNASSIGNED },

	{ "weapon_m1887", "M1887", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },

	{ "weapon_kriss", "Kriss", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_k1a", "K1A", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_thompson", "Thompson", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_mp7a1c", "MP7A1", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },

	{ "weapon_m14ebr", "M14 EBR", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_buffaug", "BuffAug", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_xm8c", "XM8", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_scarl", "Scar", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_tar21", "TAR-21", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_sgdrill",         "Magnum Drill",                 0, PRIMARY_WEAPON_SLOT, UNASSIGNED},
	{ "weapon_buffm4a1",         "M4A1 Buff",                 0, PRIMARY_WEAPON_SLOT, UNASSIGNED},
	{ "weapon_paladin", "AK-47 Paladin", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_m95", "Barrett M95", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_chainsaw",         "Ripper",                  0, PRIMARY_WEAPON_SLOT, UNASSIGNED},
	{ "weapon_wa2000", "WA2000", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_as50", "AS50", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_starchaserar",      "Star Chaser AR",               0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_mg3", "MG-3", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_bow",      "BOW",               0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_cannon", "Blackdragon Cannon", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_gungnir", "Gungnir", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_quantum", "QuantumHorizon", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_arbalest", "Arbalest", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_wondercannon", "Heaven Splitter", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_wondercannonex", "Asura Hell Splitter", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_egon", "Egon Gun : HL", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_shelteraxe", "ShelterAxe", 0, KNIFE_SLOT, UNASSIGNED },
	{ "weapon_voidpistol", "VoidPistol", 0, PISTOL_SLOT, UNASSIGNED },
	{ "weapon_voidpistolex", "VoidPistolEx", 0, PISTOL_SLOT, UNASSIGNED },

	{ "weapon_desperado", "Python Desperado", 0, PISTOL_SLOT, UNASSIGNED },

	{ "weapon_knife", "Seal Knife", 0, KNIFE_SLOT, UNASSIGNED },
	{ "knife_skullaxe", "Skull-9", 0, KNIFE_SLOT, UNASSIGNED },
	{ "knife_dragonsword", "Green Dragon Crescent Blade", 0, KNIFE_SLOT, UNASSIGNED },
	{ "knife_dualsword", "Dual Sword Phantom Slayer",0, KNIFE_SLOT, UNASSIGNED},
	{ "weapon_twinaxes", "Twin Shadow Axes", 0, KNIFE_SLOT, UNASSIGNED },
	{ "weapon_twinaxesex", "Twin Shadow AxesEx", 0, KNIFE_SLOT, UNASSIGNED },

	{ "knife_thanatos9", "Thanatos 9", 0, KNIFE_SLOT, UNASSIGNED },
	{ "knife_stormgiant", "Storm Giant", 0, KNIFE_SLOT, UNASSIGNED },
	{ "knife_snakesword", "Snake Sword", 0, KNIFE_SLOT, UNASSIGNED },
	{ "knife_nataknifed", "Nata Knife", 0, KNIFE_SLOT, UNASSIGNED },
	{ "knife_katanad", "Dual Katana", 0, KNIFE_SLOT, UNASSIGNED },
	{ "knife_dragontail", "Dragon Claw", 0, KNIFE_SLOT, UNASSIGNED },
	{ "knife_zsh_clawhammer", "Claw Hammer", 0, KNIFE_SLOT, UNASSIGNED },
	{ "knife_balrog9", "Balrog 9", 0, KNIFE_SLOT, UNASSIGNED },

	{ "weapon_balrog1", "Balrog 1",0, PISTOL_SLOT, UNASSIGNED },

	{ "weapon_balrog3", "Balrog 3", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_balrog5", "Balrog 5", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_balrog7", "Balrog 7", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_balrog11", "Balrog 11", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_guitar", "Guitar", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_m134hero", "M134 Hero", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_plasmagun", "Plasma Gun", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_poisongun", "Poison Gun", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_ethereal", "Ethereal", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_spas12ex", "SPAS-12 EX", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_violingun", "Violin Gun", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_zombibombz", "Zombie Bomb", 0, GRENADE_SLOT, TERRORIST },
	{ "weapon_sbmine", "Sattelite Mine", 0, GRENADE_SLOT, UNASSIGNED },

	{ "weapon_ak47dragon", "AK-47 Dragon", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_blaster", "Coil MG", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_broad", "Broad Divine", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_hk23", "HK-23", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_k3", "K3", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_m4a1dragon", "M4A1 Dragon", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_m60e4", "M60E4", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_mp5tiger", "MP5-Tiger", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_p90lapin", "P90-Lapin", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_pkm", "PKM", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_stunrifle", "Stun Rifle", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_tmpdragon", "TMP Dragon", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },

	{ "weapon_huntbow", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_gatling", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_gatlingex", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_tbarrel", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_stg44", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_svdex", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },

	{ "weapon_mg3xmas", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_mg3", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_m134xmas", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_m134ex", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_m95xmas", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_m2", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_janus7xmas", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
};

static MoEWeaponBuyInfo_s g_MoEWeaponBuyInfo3[] = {
	{ "weapon_infinity", "Infinity Black/Sliver", 1500, PISTOL_SLOT, UNASSIGNED },
	{ "weapon_infinityex1", "Infinity Red/Sliver", 1500, PISTOL_SLOT, UNASSIGNED },
	{ "weapon_infinityex2", "Infinity Red/Gold", 1500, PISTOL_SLOT, UNASSIGNED },
	{ "weapon_infinityss", "Infinity Sliver", 600, PISTOL_SLOT, UNASSIGNED },
	{ "weapon_infinitysr", "Infinity Red",600, PISTOL_SLOT, UNASSIGNED },
	{ "weapon_infinitysb", "Infinity Black",600, PISTOL_SLOT, UNASSIGNED },
	{ "weapon_deagled", "Dual Deagle",760, PISTOL_SLOT, UNASSIGNED },

	{ "weapon_m1887", "M1887", 2800, PRIMARY_WEAPON_SLOT, UNASSIGNED },

	{ "weapon_kriss", "Kriss", 1700, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_k1a", "K1A", 1850, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_thompson", "Thompson", 2000, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_mp7a1c", "MP7A1", 2150, PRIMARY_WEAPON_SLOT, UNASSIGNED },

	{ "weapon_m14ebr", "M14 EBR", 3100, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_buffaug", "BuffAug", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_xm8c", "XM8", 3250, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_scarl", "Scar", 3250, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_tar21", "TAR-21", 3600, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_sgdrill", "Magnum Drill",                 3400, PRIMARY_WEAPON_SLOT, UNASSIGNED},
	{ "weapon_buffm4a1", "M4A1 Buff",                 3000, PRIMARY_WEAPON_SLOT, UNASSIGNED},
	{ "weapon_paladin", "AK-47 Paladin", 2700, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_m95", "Barrett M95", 5000, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_chainsaw",         "Ripper",                  7500, PRIMARY_WEAPON_SLOT, UNASSIGNED},
	{ "weapon_wa2000", "WA2000", 4600, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_as50", "AS50", 5000, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_starchaserar",      "Star Chaser AR",               5000, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_mg3", "MG-3", 5750, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_bow", "BOW",               6200, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_cannon", "Blackdragon Cannon", 3800, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_gungnir", "Gungnir", 6700, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_quantum", "QuantumHorizon", 8800, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_arbalest", "Arbalest", 6300, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_wondercannon", "Heaven Splitter", 7300, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_wondercannonex", "Asura Hell Splitter", 8500, PRIMARY_WEAPON_SLOT, UNASSIGNED },

	{ "weapon_shelteraxe", "ShelterAxe", 0, KNIFE_SLOT, UNASSIGNED },
	{ "weapon_voidpistol", "VoidPistol", 3400, PISTOL_SLOT, UNASSIGNED },
	{ "weapon_voidpistolex", "VoidPistolEx", 3300, PISTOL_SLOT, UNASSIGNED },

	{ "weapon_desperado", "Python Desperado", 0, PISTOL_SLOT, UNASSIGNED },

	{ "weapon_balrog1", "Balrog 1", 600, PISTOL_SLOT, UNASSIGNED },

	{ "weapon_balrog3", "Balrog 3", 2700, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_balrog5", "Balrog 5", 3300, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_balrog7", "Balrog 7", 4800, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_balrog11", "Balrog 11", 3100, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_guitar", "Guitar", 3000, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_m134hero", "M134 Hero", 5400, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_plasmagun", "Plasma Gun", 3600, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_poisongun", "Poison Gun", 3800, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_ethereal", "Ethereal", 2450, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_spas12ex", "SPAS-12 EX", 2600, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_violingun", "Violin Gun", 3250, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_sbmine", "Sattelite Mine", 0, GRENADE_SLOT, UNASSIGNED },

	{ "weapon_ak47dragon", "AK-47 Dragon", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_blaster", "Coil MG", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_broad", "Broad Divine", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_hk23", "HK-23", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_k3", "K3", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_m4a1dragon", "M4A1 Dragon", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_m60e4", "M60E4", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_mp5tiger", "MP5-Tiger", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_p90lapin", "P90-Lapin", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_pkm", "PKM", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_stunrifle", "Stun Rifle", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_tmpdragon", "TMP Dragon", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },

	{ "weapon_huntbow", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },

	{ "weapon_gatling", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_gatlingex", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_tbarrel", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_stg44", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_svdex", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },

	{ "weapon_mg3xmas", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_mg3", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_m134xmas", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_m134ex", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_m95xmas", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_m2", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
	{ "weapon_janus7xmas", "FailNaught", 0, PRIMARY_WEAPON_SLOT, UNASSIGNED },
};

bool HasPlayerItem(CBasePlayer *pPlayer, const char *pszCheckItem)
{
	for (CBasePlayerItem *pItem : pPlayer->m_rgpPlayerItems)
	{
		while (pItem != nullptr)
		{
			if (FClassnameIs(pItem->pev, pszCheckItem))
				return true;

			pItem = pItem->m_pNext;
		}
	}
	return false;
}

bool MoE_HandleBuyCommands(CBasePlayer *pPlayer, const char *pszCommand)
{
	if (!pPlayer->CanPlayerBuy(true))
		return false;

	if (HasPlayerItem(pPlayer, pszCommand))
	{
		ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#Cstrike_Already_Own_Weapon");
		return false;
	}

	if (g_pModRunning->DamageTrack() == DT_NONE || g_pModRunning->DamageTrack() == DT_ZBS)
	{
		auto iter = std::find_if
		(
			std::begin(g_MoEWeaponBuyInfo), std::end(g_MoEWeaponBuyInfo), [pszCommand](const MoEWeaponBuyInfo_s& info)
			{
				return !strcmp(info.pszClassName, pszCommand);
			}
		);

		if (iter != std::end(g_MoEWeaponBuyInfo))
		{
			if (pPlayer->m_iAccount < iter->iCost)
			{
				ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#Not_Enough_Money");
				BlinkAccount(pPlayer, 2);

				return false;
			}

			switch (iter->iSlot)
			{
			case PRIMARY_WEAPON_SLOT:
				DropPrimary(pPlayer);
				break;
			case PISTOL_SLOT:
				DropSecondary(pPlayer);
				break;
			case KNIFE_SLOT:
				if (pPlayer->m_rgpPlayerItems[KNIFE_SLOT])
				{
					pPlayer->RemovePlayerItem(pPlayer->m_rgpPlayerItems[KNIFE_SLOT]);
				}
				break;
			default:
				break;
			}

			pPlayer->GiveNamedItem(iter->pszClassName);
			pPlayer->AddAccount(-iter->iCost);

			return true;
		}
	}
	else if(g_pModRunning->DamageTrack() == DT_ZSH)
	{
		auto iter = std::find_if
		(
			std::begin(g_MoEWeaponBuyInfo), std::end(g_MoEWeaponBuyInfo), [pszCommand](const MoEWeaponBuyInfo_s& info)
			{
				return !strcmp(info.pszClassName, pszCommand);
			}
		);

		if (iter != std::end(g_MoEWeaponBuyInfo))
		{
			if (pPlayer->m_iAccount < iter->iCost)
			{
				ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#Not_Enough_Money");
				BlinkAccount(pPlayer, 2);

				return false;
			}

			switch (iter->iSlot)
			{
			case PRIMARY_WEAPON_SLOT:
				DropPrimary(pPlayer);
				break;
			case PISTOL_SLOT:
				DropSecondary(pPlayer);
				break;
			case KNIFE_SLOT:
				if (pPlayer->m_rgpPlayerItems[KNIFE_SLOT])
				{
					pPlayer->RemovePlayerItem(pPlayer->m_rgpPlayerItems[KNIFE_SLOT]);
				}
				break;
			default:
				break;
			}

			pPlayer->GiveNamedItem(iter->pszClassName);
			pPlayer->AddAccount(-iter->iCost);

			return true;
		}
	}
	else
	{
		auto iter = std::find_if
		(
			std::begin(g_MoEWeaponBuyInfo2), std::end(g_MoEWeaponBuyInfo2), [pszCommand](const MoEWeaponBuyInfo_s& info)
			{
				return !strcmp(info.pszClassName, pszCommand);
			}
		);

		if (iter != std::end(g_MoEWeaponBuyInfo2))
		{
			switch (iter->iSlot)
			{
			case PRIMARY_WEAPON_SLOT:
				DropPrimary(pPlayer);
				break;
			case PISTOL_SLOT:
				DropSecondary(pPlayer);
				break;
			case KNIFE_SLOT:
				if (pPlayer->m_rgpPlayerItems[KNIFE_SLOT])
				{
					pPlayer->RemovePlayerItem(pPlayer->m_rgpPlayerItems[KNIFE_SLOT]);
				}
				break;
			case GRENADE_SLOT:
				if (pPlayer->m_rgpPlayerItems[GRENADE_SLOT])
				{
					pPlayer->RemovePlayerItem(pPlayer->m_rgpPlayerItems[GRENADE_SLOT]);
				}
				break;
			default:
				break;
			}

			pPlayer->GiveNamedItem(iter->pszClassName);

			return true;
		}
	}
	return false;
}