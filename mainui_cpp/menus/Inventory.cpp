/* =================================================================================== *
		   * =================== TechnoSoftware =================== *
	  * ================= Weapon ID System Inventory ================== *
 * =================================================================================== */

#include "Framework.h"
#include "Slider.h"
#include "Bitmap.h"
#include "PicButton.h"
#include "CheckBox.h"
#include "SpinControl.h"
#include "StringArrayModel.h"
#include "Table.h"

#include "../dlls/wpn_shared/const/weaponid_const.h"

#define ART_BACK		"gfx/shell/WeaponGet/hud_weapon_off_bg"
#define ART_UPARROW		"gfx/shell/uparrowp"
#define ART_DOWNARROW	"gfx/shell/sm_dnarw"

static const int MAX_PRIMARY = 80;
static const int MAX_SECONDARY = 17;
static const int MAX_MELEE = 15;
static const int MAX_GRENADE = 4;

 /*=================== Weapon Picture ===================*/

const char* WeaponArtPrimary[] = {
    "",
    "gfx/shell/WeaponGet/halogun",           // WP_HALOGUN
    "gfx/shell/WeaponGet/wondercannonex",    // WP_WONDERCANNONEX
    "gfx/shell/WeaponGet/wondercannon",      // WP_WONDERCANNON
    "gfx/shell/WeaponGet/revivegun",         // WP_REVIVEGUN
    "gfx/shell/WeaponGet/gungnir",           // WP_GUNGNIR
    "gfx/shell/WeaponGet/huntbow",           // WP_HUNTBOW
    "gfx/shell/WeaponGet/sgdrill",           // WP_SGDRILL
    "gfx/shell/WeaponGet/cannon",            // WP_CANNON
    "gfx/shell/WeaponGet/buffak",            // WP_PALADIN
    "gfx/shell/WeaponGet/buffm4",            // WP_BUFFM4A1
    "gfx/shell/WeaponGet/chainsaw",          // WP_CHAINSAW
    "gfx/shell/WeaponGet/gatling",           // WP_GATLING
    "gfx/shell/WeaponGet/gatlingex",         // WP_GATLINGEX
    "gfx/shell/WeaponGet/buffaug",           // WP_BUFFAUG
    "gfx/shell/WeaponGet/starchaserar",      // WP_STARCHASERAR
    "gfx/shell/WeaponGet/balrog3",           // WP_BALROG3
    "gfx/shell/WeaponGet/balrog5",           // WP_BALROG5
    "gfx/shell/WeaponGet/balrog7",           // WP_BALROG7
    "gfx/shell/WeaponGet/balrog11",          // WP_BALROG11
    "gfx/shell/WeaponGet/ethereal",          // WP_ETHEREAL
    "gfx/shell/WeaponGet/stunrifle",         // WP_STUNRIFLE
    "gfx/shell/WeaponGet/bow",               // WP_BOW
    "gfx/shell/WeaponGet/plasmagun",         // WP_PLASMAGUN
    "gfx/shell/WeaponGet/poisongun",         // WP_POISONGUN
    "gfx/shell/WeaponGet/coilmg",            // WP_COILGUN
    "gfx/shell/WeaponGet/broad",             // WP_BROAD
    "gfx/shell/WeaponGet/svdex",             // WP_SVDEX
    "gfx/shell/WeaponGet/tbarrel",           // WP_TBARREL
    "gfx/shell/WeaponGet/kriss",             // WP_KRISS
    "gfx/shell/WeaponGet/scar",              // WP_SCAR
    "gfx/shell/WeaponGet/thompson",          // WP_THOMPSON
    "gfx/shell/WeaponGet/tar21",             // WP_TAR21
    "gfx/shell/WeaponGet/mp7a1",             // WP_MP7A1
    "gfx/shell/WeaponGet/m14ebr",            // WP_M14EBR
    "gfx/shell/WeaponGet/guitar",            // WP_GUITAR
    "gfx/shell/WeaponGet/janus7xmas",        // WP_JANUS7XMAS
    "gfx/shell/WeaponGet/k1a",               // WP_K1A
    "gfx/shell/WeaponGet/as50",              // WP_AS50
    "gfx/shell/WeaponGet/wa2000",            // WP_WA2000
    "gfx/shell/WeaponGet/m95xmas",           // WP_M95XMAS
    "gfx/shell/WeaponGet/m95",               // WP_M95
    "gfx/shell/WeaponGet/m134ex",            // WP_M134EX
    "gfx/shell/WeaponGet/m134hero",          // WP_M134HERO
    "gfx/shell/WeaponGet/m134_xmas",         // WP_M134XMAS
    "gfx/shell/WeaponGet/m1887",             // WP_M1887
    "gfx/shell/WeaponGet/scar556limited",    // WP_SCARH
    "gfx/shell/WeaponGet/scar556limited",    // WP_SCARL
    "gfx/shell/WeaponGet/spas12ex",          // WP_SPAS12EX
    "gfx/shell/WeaponGet/stg44",             // WP_STG44
    "gfx/shell/WeaponGet/violingun",         // WP_VIOLINGUN
    "gfx/shell/WeaponGet/hk23",              // WP_HK23
    "gfx/shell/WeaponGet/k3",                // WP_K3
    "gfx/shell/WeaponGet/m60",               // WP_M60E4
    "gfx/shell/WeaponGet/mp5tiger",          // WP_MP5TIGER
    "gfx/shell/WeaponGet/pkm",               // WP_PKM
    "gfx/shell/WeaponGet/ak47dragon",        // WP_AK47DRAGON
    "gfx/shell/WeaponGet/m4a1dragon",        // WP_M4A1DRAGON
    "gfx/shell/WeaponGet/tmpdragon",         // WP_TMPDRAGON
    "gfx/shell/WeaponGet/p90lapin",          // WP_P90LAPIN
    "gfx/shell/WeaponGet/mg3",               // WP_MG3
    "gfx/shell/WeaponGet/m2",                // WP_M2
    "gfx/shell/WeaponGet/ak47",              // WP_AK47
    "gfx/shell/WeaponGet/aug",               // WP_AUG
    "gfx/shell/WeaponGet/awp",               // WP_AWP
    "gfx/shell/WeaponGet/famas",             // WP_FAMAS
    "gfx/shell/WeaponGet/g3sg1",             // WP_G3SG1
    "gfx/shell/WeaponGet/galil",             // WP_GALIL
    "gfx/shell/WeaponGet/m3",                // WP_M3
    "gfx/shell/WeaponGet/m4a1",              // WP_M4A1
    "gfx/shell/WeaponGet/m249",              // WP_M249
    "gfx/shell/WeaponGet/mac10",             // WP_MAC10
    "gfx/shell/WeaponGet/mp5",               // WP_MP5
    "gfx/shell/WeaponGet/p90",               // WP_P90
    "gfx/shell/WeaponGet/scout",             // WP_SCOUT
    "gfx/shell/WeaponGet/sg550",             // WP_SG550
    "gfx/shell/WeaponGet/sg552",             // WP_SG552
    "gfx/shell/WeaponGet/tmp",               // WP_TMP
    "gfx/shell/WeaponGet/ump45",             // WP_UMP45
    "gfx/shell/WeaponGet/xm1014",            // WP_XM1014
    "gfx/shell/WeaponGet/xm8",               // WP_XM8
};

const char* WeaponArtSecondary[] = {
    "",
    "gfx/shell/WeaponGet/voidpistolex",      // WP_VOIDPISTOLEX
    "gfx/shell/WeaponGet/voidpistol",        // WP_VOIDPISTOL
    "gfx/shell/WeaponGet/desperado",         // WP_DESPERADO
    "gfx/shell/WeaponGet/gunkata",           // WP_GUNAKATA
    "gfx/shell/WeaponGet/balrog1",           // WP_BALROG1
    "gfx/shell/WeaponGet/deagled",           // WP_DEAGLED
    "gfx/shell/WeaponGet/infinity",          // WP_INFINITY
    "gfx/shell/WeaponGet/infinityex1",       // WP_INFINITYEX1
    "gfx/shell/WeaponGet/infinityex2",       // WP_INFINITYEX2
    "gfx/shell/WeaponGet/infinitysb",        // WP_INFINITYSB
    "gfx/shell/WeaponGet/infinitysr",        // WP_INFINITYSR
    "gfx/shell/WeaponGet/infinityss",        // WP_INFINITYSS
    "gfx/shell/WeaponGet/deagle",            // WP_DEAGLE
    "gfx/shell/WeaponGet/elites",            // WP_ELITE
    "gfx/shell/WeaponGet/fiveseven",         // WP_FIVESEVEN
    "gfx/shell/WeaponGet/glock18",           // WP_GLOCK18
    "gfx/shell/WeaponGet/usp45",             // WP_USP
};

const char* WeaponArtMelee[] = {
    "",
    "gfx/shell/WeaponGet/dgaxeex",           // WP_TWINAXESEX
    "gfx/shell/WeaponGet/dgaxe",             // WP_TWINAXES
    "gfx/shell/WeaponGet/dualsword",         // WP_DUALSWORD
    "gfx/shell/WeaponGet/stormgiant",        // WP_STORMGIANT
    "gfx/shell/WeaponGet/balrog9",           // WP_BALROG9
    "gfx/shell/WeaponGet/zsh_clawhammer",    // WP_CLAWHAMMER
    "gfx/shell/WeaponGet/dragontail",        // WP_DRAGONCLAW
    "gfx/shell/WeaponGet/katanad",           // WP_DUALKATANA
    "gfx/shell/WeaponGet/nataknifed",        // WP_NATAKNIFED
    "gfx/shell/WeaponGet/snakesword",        // WP_SERPENTBLADE
    "gfx/shell/WeaponGet/thanatos9",         // WP_THANATOS9
    "gfx/shell/WeaponGet/dragonsword",       // WP_DRAGONSWORD
    "gfx/shell/WeaponGet/katana",            // WP_KATANA
    "gfx/shell/WeaponGet/skullaxe",          // WP_SKULLAXE
    "gfx/shell/WeaponGet/defaultknife",      // WP_KNIFE
};

const char* WeaponArtGrenade[] = {
    "",
    "gfx/shell/WeaponGet/sbmine",            // WP_SBMINE
    "gfx/shell/WeaponGet/hegrenade",         // WP_HEGRENADE
    "gfx/shell/WeaponGet/flashbang",         // WP_FLASHBANG
    "gfx/shell/WeaponGet/smokegrenade",      // WP_SMOKEGRENADE
};

/*=================== End Weapon Picture ==================*/

struct WeaponInfo
{
    const char* name;
    const char* art;
    int type; // 1 - primary, 2 - secondary, 3 - melee
    int id;
    float x, y, w, h; // Position for texture
};

class CMenuMapListModel : public CMenuBaseModel
{
public:
    void Update() override;
    int GetColumns() const override { return 2; }
    int GetRows() const override { return m_iNumItems; }
    const char* GetCellText(int line, int column) override
    {
        switch (column)
        {
        case 0: return mapName[line];
        case 1: return mapsDescription[line];
        }
        return NULL;
    }

    char mapName[UI_MAXGAMES][64];
    char mapsDescription[UI_MAXGAMES][64];
    int m_iNumItems;
};

class CMenuInventory : public CMenuFramework
{
public:
    typedef CMenuFramework BaseClass;
    CMenuInventory() : CMenuFramework("CMenuInventory") {}

private:
    // Weapon data
    static const int MAX_WEAPONS = 112;
    WeaponInfo m_Weapons[MAX_WEAPONS];

    // UI elements
    CMenuBitmap background, background1, background2, background3, background4;
    CMenuBitmap uparrow, downarrow;
    CMenuPicButton prim, secon, melee, exit;
    CMenuPicButton primtex, secontex, meleetex;

    // Weapon display and selection items
    CMenuBitmap* m_WeaponDisplays[MAX_WEAPONS];
    CMenuBitmap* m_WeaponTextures[MAX_WEAPONS];

    // Background elements
    CMenuMapListModel mapsListModel;
    CMapSet SetBack1, SetBack2, SetBack3, SetBack4;
    CMapSet SetBack5, SetBack6, SetBack7, SetBack8;
    CMapSet SetBack9, SetBack10, SetBack11, SetBack12;
    CMapSet SetBack13, SetBack14, SetBack15, SetBack16;
    CMapSet SetBack17, SetBack18;

    int stringsprimary, stringssecondary, stringsmelee;
    int pagetypeweapon;

    void _Init() override;
    void _VidInit() override;
    void SaveAndPopMenu() override;

    // Helper methods
    void InitializeWeapons();
    void CreateCommonUI();
    void CreateWeaponItems();
    void CreateNavigationButtons();
    void CreateBackgroundElements();

    // Weapon management
    void GetWp(int primary, int secondary, int melee, int grenade);
    void GetPage(int page);
    void SelectWeapon(int type, int id);
    void SetGun(int value) { EngFuncs::CvarSetValue("wpn_getgun", value); }
    void SetPistol(int value) { EngFuncs::CvarSetValue("wpn_getpistol", value); }
    void SetKnife(int value) { EngFuncs::CvarSetValue("wpn_getknife", value); }
    void SetGrenade(int value) { EngFuncs::CvarSetValue("wpn_getgrenade", value); }

    // Event handlers
    void OnWeaponSelected(int weaponIndex);
    void OnPageButton(int page);
    void OnArrowButton(bool up);
};

static CMenuInventory uiInventory;

// Initialize weapon data
void CMenuInventory::InitializeWeapons()
{
    // Primary weapons (0-79)
    int index = 0;
    m_Weapons[index++] = { "Halogun", WeaponArtPrimary[WEAPON_HALOGUN], 1, WEAPON_HALOGUN, 600, 237, 200, 55 };
    m_Weapons[index++] = { "WonderCannonEX",  WeaponArtPrimary[WEAPON_WONDERCANNONEX], 1, WEAPON_WONDERCANNONEX, 600, 307, 200, 55 };
    m_Weapons[index++] = { "WonderCannon",  WeaponArtPrimary[WEAPON_WONDERCANNON], 1, WEAPON_WONDERCANNON, 600, 377, 200, 55 };
    m_Weapons[index++] = { "ReviveGun",  WeaponArtPrimary[WEAPON_REVIVEGUN], 1, WEAPON_REVIVEGUN, 590, 447, 220, 55 };
    m_Weapons[index++] = { "Gungnir",  WeaponArtPrimary[WEAPON_GUNGNIR], 1, WEAPON_GUNGNIR, 590, 520, 220, 50 };
    m_Weapons[index++] = { "HuntBow",  WeaponArtPrimary[WEAPON_HUNTBOW], 1, WEAPON_HUNTBOW, 600, 587, 200, 55 };

    m_Weapons[index++] = { "SgDrill",  WeaponArtPrimary[WEAPON_SGDRILL], 1, WEAPON_SGDRILL, 790, 237, 220, 55 };
    m_Weapons[index++] = { "Cannon",  WeaponArtPrimary[WEAPON_CANNON], 1, WEAPON_CANNON, 800, 307, 200, 55 };
    m_Weapons[index++] = { "Paladin",  WeaponArtPrimary[WEAPON_PALADIN], 1, WEAPON_PALADIN, 805, 377, 195, 55 };
    m_Weapons[index++] = { "Buffm4a1",  WeaponArtPrimary[WEAPON_BUFFM4A1], 1, WEAPON_BUFFM4A1, 790, 447, 220, 55 };
    m_Weapons[index++] = { "ChainSaw",  WeaponArtPrimary[WEAPON_CHAINSAW], 1, WEAPON_CHAINSAW, 790, 517, 220, 55 };
    m_Weapons[index++] = { "Gun Infernal",  WeaponArtPrimary[WEAPON_GATLINGEX], 1, WEAPON_GATLINGEX, 790, 587, 220, 55 };

    m_Weapons[index++] = { "Vulkan",  WeaponArtPrimary[WEAPON_GATLING], 1, WEAPON_GATLING, 600, 237, 200, 55 };
    m_Weapons[index++] = { "BUFF Aug",  WeaponArtPrimary[WEAPON_BUFFAUG], 1, WEAPON_BUFFAUG, 590, 307, 220, 55 };
    m_Weapons[index++] = { "Star Chaser-AR",  WeaponArtPrimary[WEAPON_STARCHASERAR], 1, WEAPON_STARCHASERAR, 590, 377, 220, 55 };
    m_Weapons[index++] = { "Balrog-3",  WeaponArtPrimary[WEAPON_BALROG3], 1, WEAPON_BALROG3, 600, 447, 200, 55 };
    m_Weapons[index++] = { "Balrog-5",  WeaponArtPrimary[WEAPON_BALROG5], 1, WEAPON_BALROG5, 600, 517, 200, 55 };
    m_Weapons[index++] = { "Balrog-7",  WeaponArtPrimary[WEAPON_BALROG7], 1, WEAPON_BALROG7, 610, 587, 180, 55 };

    m_Weapons[index++] = { "Balrog-11",  WeaponArtPrimary[WEAPON_BALROG11], 1, WEAPON_BALROG11, 800, 237, 200, 55 };
    m_Weapons[index++] = { "Ethereal",  WeaponArtPrimary[WEAPON_ETHEREAL], 1, WEAPON_ETHEREAL, 800, 307, 200, 55 };
    m_Weapons[index++] = { "StunRifle",  WeaponArtPrimary[WEAPON_STUNRIFLE], 1, WEAPON_STUNRIFLE, 800, 377, 200, 55 };
    m_Weapons[index++] = { "Bpw",  WeaponArtPrimary[WEAPON_BOW], 1, WEAPON_BOW, 800, 447, 200, 55 };
    m_Weapons[index++] = { "PlasmaGun",  WeaponArtPrimary[WEAPON_PLASMAGUN], 1, WEAPON_PLASMAGUN, 790, 517, 220, 55 };
    m_Weapons[index++] = { "PoisonGun",  WeaponArtPrimary[WEAPON_POISONGUN], 1, WEAPON_POISONGUN, 810, 587, 180, 55 };

    m_Weapons[index++] = { "Coil Gun",  WeaponArtPrimary[WEAPON_COILGUN], 1, WEAPON_COILGUN, 600, 237, 200, 55 };
    m_Weapons[index++] = { "Broad",  WeaponArtPrimary[WEAPON_BROAD], 1, WEAPON_BROAD, 590, 307, 220, 55 };
    m_Weapons[index++] = { "SVDEX",  WeaponArtPrimary[WEAPON_SVDEX], 1, WEAPON_SVDEX, 590, 377, 220, 55 };
    m_Weapons[index++] = { "Triple Barrel",  WeaponArtPrimary[WEAPON_TBARREL], 1, WEAPON_TBARREL, 600, 447, 200, 55 };
    m_Weapons[index++] = { "Kriss",  WeaponArtPrimary[WEAPON_KRISS], 1, WEAPON_KRISS, 600, 517, 200, 55 };
    m_Weapons[index++] = { "Scar",  WeaponArtPrimary[WEAPON_SCAR], 1, WEAPON_SCAR, 610, 587, 180, 55 };

    m_Weapons[index++] = { "Thompson",  WeaponArtPrimary[WEAPON_THOMPSON], 1, WEAPON_THOMPSON, 800, 237, 200, 55 };
    m_Weapons[index++] = { "TAR-21",  WeaponArtPrimary[WEAPON_TAR21], 1, WEAPON_TAR21, 800, 307, 200, 55 };
    m_Weapons[index++] = { "MP7A1",  WeaponArtPrimary[WEAPON_MP7A1], 1, WEAPON_MP7A1, 800, 377, 200, 55 };
    m_Weapons[index++] = { "M14EBR",  WeaponArtPrimary[WEAPON_M14EBR], 1, WEAPON_M14EBR, 800, 447, 200, 55 };
    m_Weapons[index++] = { "Guitar",  WeaponArtPrimary[WEAPON_GUITAR], 1, WEAPON_GUITAR, 790, 517, 220, 55 };
    m_Weapons[index++] = { "Janus 7 Xmas",  WeaponArtPrimary[WEAPON_JANUS7XMAS], 1, WEAPON_JANUS7XMAS, 810, 587, 180, 55 };

    m_Weapons[index++] = { "K1A",  WeaponArtPrimary[WEAPON_K1A], 1, WEAPON_K1A, 600, 237, 200, 55 };
    m_Weapons[index++] = { "AS50",  WeaponArtPrimary[WEAPON_AS50], 1, WEAPON_AS50, 590, 307, 220, 55 };
    m_Weapons[index++] = { "WA2000",  WeaponArtPrimary[WEAPON_WA2000], 1, WEAPON_WA2000, 590, 377, 220, 55 };
    m_Weapons[index++] = { "M95 Xmas",  WeaponArtPrimary[WEAPON_M95XMAS], 1, WEAPON_M95XMAS, 600, 447, 200, 55 };
    m_Weapons[index++] = { "M95",  WeaponArtPrimary[WEAPON_M95], 1, WEAPON_M95, 600, 517, 200, 55 };
    m_Weapons[index++] = { "M134EX",  WeaponArtPrimary[WEAPON_M134EX], 1, WEAPON_M134EX, 610, 587, 180, 55 };

    m_Weapons[index++] = { "M134 Hero",  WeaponArtPrimary[WEAPON_M134HERO], 1, WEAPON_M134HERO, 800, 237, 200, 55 };
    m_Weapons[index++] = { "M134 Xmas",  WeaponArtPrimary[WEAPON_M134XMAS], 1, WEAPON_M134XMAS, 800, 307, 200, 55 };
    m_Weapons[index++] = { "M1887",  WeaponArtPrimary[WEAPON_M1887], 1, WEAPON_M1887, 800, 377, 200, 55 };
    m_Weapons[index++] = { "ScarH",  WeaponArtPrimary[WEAPON_SCARH], 1, WEAPON_SCARH, 800, 447, 200, 55 };
    m_Weapons[index++] = { "ScarL",  WeaponArtPrimary[WEAPON_SCARL], 1, WEAPON_SCARL, 790, 517, 220, 55 };
    m_Weapons[index++] = { "SPAS12EX",  WeaponArtPrimary[WEAPON_SPAS12EX], 1, WEAPON_SPAS12EX, 810, 587, 180, 55 };

    m_Weapons[index++] = { "STG44",  WeaponArtPrimary[WEAPON_STG44], 1, WEAPON_STG44, 600, 237, 200, 55 };
    m_Weapons[index++] = { "ViolinGun",  WeaponArtPrimary[WEAPON_VIOLINGUN], 1, WEAPON_VIOLINGUN, 590, 307, 220, 55 };
    m_Weapons[index++] = { "HK23",  WeaponArtPrimary[WEAPON_HK23], 1, WEAPON_HK23, 590, 377, 220, 55 };
    m_Weapons[index++] = { "K3",  WeaponArtPrimary[WEAPON_K3], 1, WEAPON_K3, 600, 447, 200, 55 };
    m_Weapons[index++] = { "M60E4",  WeaponArtPrimary[WEAPON_M60E4], 1, WEAPON_M60E4, 600, 517, 200, 55 };
    m_Weapons[index++] = { "MP5 Tiger",  WeaponArtPrimary[WEAPON_MP5TIGER], 1, WEAPON_MP5TIGER, 610, 587, 180, 55 };

    m_Weapons[index++] = { "PKM",  WeaponArtPrimary[WEAPON_PKM], 1, WEAPON_PKM, 800, 237, 200, 55 };
    m_Weapons[index++] = { "AK47 Dragon",  WeaponArtPrimary[WEAPON_AK47DRAGON], 1, WEAPON_AK47DRAGON, 800, 307, 200, 55 };
    m_Weapons[index++] = { "M4A1 Dragon",  WeaponArtPrimary[WEAPON_M4A1DRAGON], 1, WEAPON_M4A1DRAGON, 800, 377, 200, 55 };
    m_Weapons[index++] = { "TMP Dragon",  WeaponArtPrimary[WEAPON_TMPDRAGON], 1, WEAPON_TMPDRAGON, 800, 447, 200, 55 };
    m_Weapons[index++] = { "P90 Lapin",  WeaponArtPrimary[WEAPON_P90LAPIN], 1, WEAPON_P90LAPIN, 790, 517, 220, 55 };
    m_Weapons[index++] = { "MG3",  WeaponArtPrimary[WEAPON_MG3], 1, WEAPON_MG3, 810, 587, 180, 55 };

    m_Weapons[index++] = { "M2",  WeaponArtPrimary[WEAPON_M2], 1, WEAPON_M2, 600, 237, 200, 55 };
    m_Weapons[index++] = { "AK47",  WeaponArtPrimary[WEAPON_AK47], 1, WEAPON_AK47, 590, 307, 220, 55 };
    m_Weapons[index++] = { "AUG",  WeaponArtPrimary[WEAPON_AUG], 1, WEAPON_AUG, 590, 377, 220, 55 };
    m_Weapons[index++] = { "AWP",  WeaponArtPrimary[WEAPON_AWP], 1, WEAPON_AWP, 600, 447, 200, 55 };
    m_Weapons[index++] = { "FAMAS",  WeaponArtPrimary[WEAPON_FAMAS], 1, WEAPON_FAMAS, 600, 517, 200, 55 };
    m_Weapons[index++] = { "G3SG1",  WeaponArtPrimary[WEAPON_G3SG1], 1, WEAPON_G3SG1, 610, 587, 180, 55 };

    m_Weapons[index++] = { "GALIL",  WeaponArtPrimary[WEAPON_GALIL], 1, WEAPON_GALIL, 800, 237, 200, 55 };
    m_Weapons[index++] = { "M3",  WeaponArtPrimary[WEAPON_M3], 1, WEAPON_M3, 800, 307, 200, 55 };
    m_Weapons[index++] = { "M4A1",  WeaponArtPrimary[WEAPON_M4A1], 1, WEAPON_M4A1, 800, 377, 200, 55 };
    m_Weapons[index++] = { "M249",  WeaponArtPrimary[WEAPON_M249], 1, WEAPON_M249, 800, 447, 200, 55 };
    m_Weapons[index++] = { "MAC-10",  WeaponArtPrimary[WEAPON_MAC10], 1, WEAPON_MAC10, 790, 517, 220, 55 };
    m_Weapons[index++] = { "MP55",  WeaponArtPrimary[WEAPON_MP5], 1, WEAPON_MP5, 810, 587, 180, 55 };

    m_Weapons[index++] = { "P90",  WeaponArtPrimary[WEAPON_P90], 1, WEAPON_P90, 600, 237, 200, 55 };
    m_Weapons[index++] = { "Scout",  WeaponArtPrimary[WEAPON_SCOUT], 1, WEAPON_SCOUT, 590, 307, 220, 55 };
    m_Weapons[index++] = { "SG550",  WeaponArtPrimary[WEAPON_SG550], 1, WEAPON_SG550, 590, 377, 220, 55 };
    m_Weapons[index++] = { "SG552",  WeaponArtPrimary[WEAPON_SG552], 1, WEAPON_SG552, 600, 447, 200, 55 };
    m_Weapons[index++] = { "TMP",  WeaponArtPrimary[WEAPON_TMP], 1, WEAPON_TMP, 600, 517, 200, 55 };
    m_Weapons[index++] = { "UMP-45",  WeaponArtPrimary[WEAPON_UMP45], 1, WEAPON_UMP45, 610, 587, 180, 55 };

    m_Weapons[index++] = { "XM1014",  WeaponArtPrimary[WEAPON_XM1014], 1, WEAPON_XM1014, 800, 237, 200, 55 };

    // Secondary weapons (80-96)


    m_Weapons[index++] = { "VoidPistolEX",  WeaponArtSecondary[WEAPON_VOIDPISTOLEX], 2, WEAPON_VOIDPISTOLEX, 600, 237, 200, 55 };
    m_Weapons[index++] = { "VoidPistol", WeaponArtSecondary[WEAPON_VOIDPISTOL], 2, WEAPON_VOIDPISTOL, 590, 307, 220, 55 };
    m_Weapons[index++] = { "Python Desperado", WeaponArtSecondary[WEAPON_DESPERADO], 2, WEAPON_DESPERADO, 590, 377, 220, 55 };
    m_Weapons[index++] = { "GunKata", WeaponArtSecondary[WEAPON_GUNAKATA], 2, WEAPON_GUNAKATA, 600, 447, 200, 55 };
    m_Weapons[index++] = { "Balrog-1", WeaponArtSecondary[WEAPON_BALROG1], 2, WEAPON_BALROG1, 600, 517, 200, 55 };
    m_Weapons[index++] = { "Deagle Dual", WeaponArtSecondary[WEAPON_DEAGLED], 2, WEAPON_DEAGLED, 610, 587, 180, 55 };

    m_Weapons[index++] = { "Infinity", WeaponArtSecondary[WEAPON_INFINITY], 2, WEAPON_INFINITY, 800, 237, 200, 55 };
    m_Weapons[index++] = { "InfinityEX1", WeaponArtSecondary[WEAPON_INFINITYEX1], 2, WEAPON_INFINITYEX1, 800, 307, 200, 55 };
    m_Weapons[index++] = { "InfinityEX2", WeaponArtSecondary[WEAPON_INFINITYEX2], 2, WEAPON_INFINITYEX2, 800, 377, 200, 55 };
    m_Weapons[index++] = { "InfinitySB", WeaponArtSecondary[WEAPON_INFINITYSB], 2, WEAPON_INFINITYSB, 800, 447, 200, 55 };
    m_Weapons[index++] = { "InfinitySR", WeaponArtSecondary[WEAPON_INFINITYSR], 2, WEAPON_INFINITYSR, 790, 517, 220, 55 };
    m_Weapons[index++] = { "InfinitySS", WeaponArtSecondary[WEAPON_INFINITYSS], 2, WEAPON_INFINITYSS, 810, 587, 180, 55 };

    m_Weapons[index++] = { "Deagle", WeaponArtSecondary[WEAPON_DEAGLE], 2, WEAPON_DEAGLE, 600, 237, 200, 55 };
    m_Weapons[index++] = { "Dual Elite", WeaponArtSecondary[WEAPON_ELITE], 2, WEAPON_ELITE, 590, 307, 220, 55 };
    m_Weapons[index++] = { "FiveSeven", WeaponArtSecondary[WEAPON_FIVESEVEN], 2, WEAPON_FIVESEVEN, 590, 377, 220, 55 };
    m_Weapons[index++] = { "Glock-18", WeaponArtSecondary[WEAPON_GLOCK18], 2, WEAPON_GLOCK18, 600, 447, 200, 55 };
    m_Weapons[index++] = { "USP", WeaponArtSecondary[WEAPON_USP], 2, WEAPON_USP, 600, 517, 200, 55 };
    //m_Weapons[index++] = { "Deagle Dual", ART_DEAGLED, 2, WEAPON_DEAGLED, 610, 587, 180, 55 };

    //m_Weapons[index++] = { "VoidPistolEX", ART_VOIDPISTOLEX, 2, WEAPON_VOIDPISTOLEX, 590, 237, 220, 55 };
   // m_Weapons[index++] = { "VoidPistol", ART_VOIDPISTOL, 2, WEAPON_VOIDPISTOL, 590, 307, 220, 55 };
   // m_Weapons[index++] = { "Infinity", ART_INFINITY, 2, 3, 610, 377, 180, 55 };
   // m_Weapons[index++] = { "InfinityEX1", ART_INFINITYEX1, 2, 4, 610, 447, 180, 55 };
   // m_Weapons[index++] = { "InfinityEX2", ART_INFINITYEX2, 2, 5, 610, 517, 180, 55 };
   // m_Weapons[index++] = { "InfinitySB", ART_INFINITYSB, 2, 6, 610, 587, 190, 55 };
   // m_Weapons[index++] = { "InfinitySR", ART_INFINITYSR, 2, 7, 790, 237, 220, 55 };

  //  m_Weapons[index++] = { "InfinitySS", ART_INFINITYSS, 2, 8, 790, 307, 220, 55 };

    // Melee weapons (97-111)

    m_Weapons[index++] = { "TwinAxesEX", WeaponArtMelee[WEAPON_TWINAXESEX], 3, WEAPON_TWINAXESEX, 600, 237, 200, 55 };
    m_Weapons[index++] = { "TwinAxes", WeaponArtMelee[WEAPON_TWINAXES], 3, WEAPON_TWINAXES, 590, 307, 220, 55 };
    m_Weapons[index++] = { "DualSword", WeaponArtMelee[WEAPON_DUALSWORD], 3, WEAPON_DUALSWORD, 590, 377, 220, 55 };
    m_Weapons[index++] = { "StormGiant", WeaponArtMelee[WEAPON_STORMGIANT], 3, WEAPON_STORMGIANT, 600, 447, 200, 55 };
    m_Weapons[index++] = { "Balrog9", WeaponArtMelee[WEAPON_BALROG9], 3, WEAPON_BALROG9, 600, 517, 200, 55 };
    m_Weapons[index++] = { "ClawHammer", WeaponArtMelee[WEAPON_CLAWHAMMER], 3, WEAPON_CLAWHAMMER, 610, 587, 180, 55 };

    m_Weapons[index++] = { "DragonClaw", WeaponArtMelee[WEAPON_DRAGONCLAW], 3, WEAPON_DRAGONCLAW, 800, 237, 200, 55 };
    m_Weapons[index++] = { "DualKatana", WeaponArtMelee[WEAPON_DUALKATANA], 3, WEAPON_DUALKATANA, 800, 307, 200, 55 };
    m_Weapons[index++] = { "NataKnifed", WeaponArtMelee[WEAPON_NATAKNIFED], 3, WEAPON_NATAKNIFED, 800, 377, 200, 55 };
    m_Weapons[index++] = { "SnakeSword", WeaponArtMelee[WEAPON_SERPENTBLADE], 3, WEAPON_SERPENTBLADE, 800, 447, 200, 55 };
    m_Weapons[index++] = { "Thanatos9", WeaponArtMelee[WEAPON_THANATOS9], 3, WEAPON_THANATOS9, 790, 517, 220, 55 };
    m_Weapons[index++] = { "DragonSword", WeaponArtMelee[WEAPON_DRAGONSWORD], 3, WEAPON_DRAGONSWORD, 810, 587, 180, 55 };

    m_Weapons[index++] = { "Katana", WeaponArtMelee[WEAPON_KATANA], 3, WEAPON_KATANA, 600, 237, 200, 55 };
    m_Weapons[index++] = { "SkullAxe", WeaponArtMelee[WEAPON_SKULLAXE], 3, WEAPON_SKULLAXE, 590, 307, 220, 55 };
    m_Weapons[index++] = { "Knife", WeaponArtMelee[WEAPON_KNIFE], 3, WEAPON_KNIFE, 590, 377, 220, 55 };
}

void CMenuInventory::CreateBackgroundElements()
{
    // Main backgrounds
    background1.iFlags = QMF_INACTIVE;
    background1.SetRect(200, 225, 235, 81);
    background1.SetPicture(ART_BACK);
    background1.SetRenderMode(QM_DRAWTRANS, QM_DRAWTRANS, QM_DRAWTRANS);

    background2.iFlags = QMF_INACTIVE;
    background2.SetRect(600, 225, 400, 480);
    background2.SetPicture(ART_BACK);
    background2.SetRenderMode(QM_DRAWTRANS, QM_DRAWTRANS, QM_DRAWTRANS);

    background3.iFlags = QMF_INACTIVE;
    background3.SetRect(200, 346, 235, 81);
    background3.SetPicture(ART_BACK);
    background3.SetRenderMode(QM_DRAWTRANS, QM_DRAWTRANS, QM_DRAWTRANS);

    background4.iFlags = QMF_INACTIVE;
    background4.SetRect(200, 463, 235, 81);
    background4.SetPicture(ART_BACK);
    background4.SetRenderMode(QM_DRAWTRANS, QM_DRAWTRANS, QM_DRAWTRANS);

    // Background set elements
    SetBack4.iFlags = QMF_INACTIVE;
    SetBack4.SetCharSize(QM_SMALLFONT);
    SetBack4.SetModel(&mapsListModel);
    SetBack4.SetRect(600, 190, 400, 510);

    SetBack1.iFlags = QMF_INACTIVE;
    SetBack1.SetCharSize(QM_SMALLFONT);
    SetBack1.SetModel(&mapsListModel);
    SetBack1.SetRect(200, 193, 230, 111);

    SetBack2.iFlags = QMF_INACTIVE;
    SetBack2.SetCharSize(QM_SMALLFONT);
    SetBack2.SetModel(&mapsListModel);
    SetBack2.SetRect(200, 313, 230, 111);

    SetBack3.iFlags = QMF_INACTIVE;
    SetBack3.SetCharSize(QM_SMALLFONT);
    SetBack3.SetModel(&mapsListModel);
    SetBack3.SetRect(200, 433, 230, 111);

    // Additional background elements
    float bgX = 610, bgY = 205, bgWidth = 180, bgHeight = 90;
    CMapSet* bgSets[] = { &SetBack5, &SetBack6, &SetBack7, &SetBack8, &SetBack9,
                           &SetBack10, &SetBack11, &SetBack12, &SetBack13, &SetBack14,
                           &SetBack15, &SetBack16, &SetBack17, &SetBack18 };

    for (int i = 0; i < 14; i++) {
        bgSets[i]->iFlags = QMF_INACTIVE;
        bgSets[i]->SetCharSize(QM_SMALLFONT);
        bgSets[i]->SetModel(&mapsListModel);

        if (i % 2 == 0) {
            bgSets[i]->SetRect(bgX, bgY, bgWidth, bgHeight);
        }
        else {
            bgSets[i]->SetRect(bgX + 200, bgY, bgWidth, bgHeight);
            bgY += 70;
        }
    }
}

void CMenuInventory::CreateWeaponItems()
{
    for (int i = 0; i < MAX_WEAPONS; i++) {
        // Create weapon display (large image)
        m_WeaponDisplays[i] = new CMenuBitmap();
        m_WeaponDisplays[i]->iFlags = QMF_NOTIFY;

        // Set position based on weapon type
        switch (m_Weapons[i].type) {
        case 1: // Primary
            m_WeaponDisplays[i]->SetRect(200, 225, 235, 81);
            break;
        case 2: // Secondary
            m_WeaponDisplays[i]->SetRect(200, 343, 235, 81);
            break;
        case 3: // Melee
            m_WeaponDisplays[i]->SetRect(200, 463, 235, 81);
            break;
        }

        m_WeaponDisplays[i]->SetPicture(m_Weapons[i].art);
        m_WeaponDisplays[i]->SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

        // Create weapon texture (selection button)
        m_WeaponTextures[i] = new CMenuBitmap();
        m_WeaponTextures[i]->iFlags = QMF_NOTIFY;
        m_WeaponTextures[i]->SetRect(m_Weapons[i].x, m_Weapons[i].y, m_Weapons[i].w, m_Weapons[i].h);
        m_WeaponTextures[i]->SetPicture(m_Weapons[i].art);
        m_WeaponTextures[i]->SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);

        switch (i) {
        case 0: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(0); }); break;
        case 1: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(1); }); break;
        case 2: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(2); }); break;
        case 3: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(3); }); break;
        case 4: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(4); }); break;
        case 5: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(5); }); break;
        case 6: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(6); }); break;
        case 7: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(7); }); break;
        case 8: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(8); }); break;
        case 9: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(9); }); break;
        case 10: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(10); }); break;
        case 11: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(11); }); break;
        case 12: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(12); }); break;
        case 13: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(13); }); break;
        case 14: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(14); }); break;
        case 15: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(15); }); break;
        case 16: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(16); }); break;
        case 17: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(17); }); break;
        case 18: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(18); }); break;
        case 19: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(19); }); break;
        case 20: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(20); }); break;
        case 21: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(21); }); break;
        case 22: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(22); }); break;
        case 23: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(23); }); break;
        case 24: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(24); }); break;
        case 25: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(25); }); break;
        case 26: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(26); }); break;
        case 27: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(27); }); break;
        case 28: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(28); }); break;
        case 29: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(29); }); break;
        case 30: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(30); }); break;
        case 31: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(31); }); break;
        case 32: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(32); }); break;
        case 33: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(33); }); break;
        case 34: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(34); }); break;
        case 35: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(35); }); break;
        case 36: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(36); }); break;
        case 37: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(37); }); break;
        case 38: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(38); }); break;
        case 39: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(39); }); break;
        case 40: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(40); }); break;
        case 41: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(41); }); break;
        case 42: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(42); }); break;
        case 43: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(43); }); break;
        case 44: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(44); }); break;
        case 45: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(45); }); break;
        case 46: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(46); }); break;
        case 47: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(47); }); break;
        case 48: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(48); }); break;
        case 49: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(49); }); break;
        case 50: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(50); }); break;
        case 51: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(51); }); break;
        case 52: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(52); }); break;
        case 53: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(53); }); break;
        case 54: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(54); }); break;
        case 55: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(55); }); break;
        case 56: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(56); }); break;
        case 57: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(57); }); break;
        case 58: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(58); }); break;
        case 59: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(59); }); break;
        case 60: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(60); }); break;
        case 61: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(61); }); break;
        case 62: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(62); }); break;
        case 63: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(63); }); break;
        case 64: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(64); }); break;
        case 65: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(65); }); break;
        case 66: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(66); }); break;
        case 67: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(67); }); break;
        case 68: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(68); }); break;
        case 69: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(69); }); break;
        case 70: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(70); }); break;
        case 71: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(71); }); break;
        case 72: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(72); }); break;
        case 73: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(73); }); break;
        case 74: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(74); }); break;
        case 75: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(75); }); break;
        case 76: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(76); }); break;
        case 77: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(77); }); break;
        case 78: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(78); }); break;
        case 79: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(79); }); break;
        case 80: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(80); }); break;
        case 81: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(81); }); break;
        case 82: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(82); }); break;
        case 83: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(83); }); break;
        case 84: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(84); }); break;
        case 85: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(85); }); break;
        case 86: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(86); }); break;
        case 87: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(87); }); break;
        case 88: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(88); }); break;
        case 89: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(89); }); break;
        case 90: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(90); }); break;
        case 91: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(91); }); break;
        case 92: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(92); }); break;
        case 93: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(93); }); break;
        case 94: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(94); }); break;
        case 95: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(95); }); break;
        case 96: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(96); }); break;
        case 97: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(97); }); break;
        case 98: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(98); }); break;
        case 99: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(99); }); break;
        case 100: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(100); }); break;
        case 101: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(101); }); break;
        case 102: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(102); }); break;
        case 103: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(103); }); break;
        case 104: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(104); }); break;
        case 105: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(105); }); break;
        case 106: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(106); }); break;
        case 107: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(107); }); break;
        case 108: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(108); }); break;
        case 109: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(109); }); break;
        case 110: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(110); }); break;
        case 111: SET_EVENT_MULTI(m_WeaponTextures[i]->onActivated, { uiInventory.OnWeaponSelected(111); }); break;
        }
        AddItem(m_WeaponDisplays[i]);
        AddItem(m_WeaponTextures[i]);
    }
}

void CMenuInventory::CreateNavigationButtons()
{
    // Page buttons
    prim.SetNameAndStatus(L("Gun"), L(""));
    prim.iFlags |= QMF_NOTIFY;
    prim.SetCoord(600, 190);
    SET_EVENT_MULTI(prim.onActivated, {
        uiInventory.OnPageButton(1);
        });

    secon.SetNameAndStatus(L("Pistol"), L(""));
    secon.iFlags |= QMF_NOTIFY;
    secon.SetCoord(680, 190);
    SET_EVENT_MULTI(secon.onActivated, {
        uiInventory.OnPageButton(2);
        });

    melee.SetNameAndStatus(L("Knife"), L(""));
    melee.iFlags |= QMF_NOTIFY;
    melee.SetCoord(780, 190);
    SET_EVENT_MULTI(melee.onActivated, {
        uiInventory.OnPageButton(3);
        });

    // Weapon type labels
    primtex.SetNameAndStatus(L("CstzUI_WP_Gun"), L(""));
    primtex.iFlags |= QMF_INACTIVE;
    primtex.SetCoord(200, 195);

    secontex.SetNameAndStatus(L("CstzUI_WP_Pistol"), L(""));
    secontex.iFlags |= QMF_INACTIVE;
    secontex.SetCoord(200, 315);

    meleetex.SetNameAndStatus(L("CstzUI_WP_Knife"), L(""));
    meleetex.iFlags |= QMF_INACTIVE;
    meleetex.SetCoord(200, 435);

    // Arrow buttons
    uparrow.iFlags = QMF_NOTIFY;
    uparrow.SetRect(1025, 395, 25, 25);
    uparrow.SetPicture(ART_UPARROW);
    uparrow.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
    SET_EVENT_MULTI(uparrow.onActivated, {
        uiInventory.OnArrowButton(true);
        });

    downarrow.iFlags = QMF_NOTIFY;
    downarrow.SetRect(1025, 445, 25, 25);
    downarrow.SetPicture(ART_DOWNARROW);
    downarrow.SetRenderMode(QM_DRAWHOLES, QM_DRAWHOLES, QM_DRAWHOLES);
    SET_EVENT_MULTI(downarrow.onActivated, {
        uiInventory.OnArrowButton(false);
        });

    // Exit button
    exit.SetNameAndStatus(L("GameUI_Close"), L(""));
    exit.onActivated = VoidCb(&CMenuInventory::Hide);
    exit.iFlags |= QMF_NOTIFY;
    exit.SetCoord(30, 600);
}

void CMenuInventory::CreateCommonUI()
{
    CreateBackgroundElements();
    CreateWeaponItems();
    CreateNavigationButtons();
}

void CMenuInventory::_Init(void)
{
    stringsprimary = 1;
    stringssecondary = 1;
    stringsmelee = 1;

    InitializeWeapons();
    CreateCommonUI();

    // Add all UI elements in order
    AddItem(background);
    AddItem(background1);
    AddItem(background2);
    AddItem(background3);
    AddItem(background4);

    // Add background sets
    AddItem(SetBack1);
    AddItem(SetBack2);
    AddItem(SetBack3);
    AddItem(SetBack4);
    AddItem(SetBack5);
    AddItem(SetBack6);
    AddItem(SetBack7);
    AddItem(SetBack8);
    AddItem(SetBack9);
    AddItem(SetBack10);
    AddItem(SetBack11);
    AddItem(SetBack12);
    AddItem(SetBack13);
    AddItem(SetBack14);
    AddItem(SetBack15);
    AddItem(SetBack16);
    AddItem(SetBack17);
    AddItem(SetBack18);

    // Add navigation
    AddItem(uparrow);
    AddItem(downarrow);
    AddItem(prim);
    AddItem(secon);
    AddItem(melee);
    AddItem(primtex);
    AddItem(secontex);
    AddItem(meleetex);
    AddItem(exit);
}

void CMenuInventory::OnWeaponSelected(int weaponIndex)
{
    int type = m_Weapons[weaponIndex].type;
    int id = m_Weapons[weaponIndex].id;

    // Hide all weapons of the same type
    int start = 0, end = 0;
    switch (type) {
    case 1: start = 0; end = 78; break;   // Primary: 0-78 (79 оружий)
    case 2: start = 79; end = 95; break;  // Secondary: 79-95 (17 оружий)
    case 3: start = 96; end = 110; break; // Melee: 96-110 (15 оружий)
    }
    for (int i = start; i <= end; i++) {
        m_WeaponDisplays[i]->Hide();
    }

    // Show selected weapon
    m_WeaponDisplays[weaponIndex]->Show();

    // Set CVAR
    switch (type) {
    case 1: SetGun(id); break;
    case 2: SetPistol(id); break;
    case 3: SetKnife(id); break;
    }
}

void CMenuInventory::OnPageButton(int page)
{
    GetPage(page);
    EngFuncs::PlayLocalSound("media/launch_deny2.wav");
}

void CMenuInventory::OnArrowButton(bool up)
{
    int* currentStrings = nullptr;
    int maxPages = 1;

    switch (pagetypeweapon) {
    case 1:
        currentStrings = &stringsprimary;
        maxPages = 7; // Primary: 7 страниц
        break;
    case 2:
        currentStrings = &stringssecondary;
        maxPages = 2; // Secondary: 2 страницы
        break;
    case 3:
        currentStrings = &stringsmelee;
        maxPages = 2; // Melee: 2 страницы
        break;
    }

    if (up) {
        if (*currentStrings > 1) {
            (*currentStrings)--;
        }
        else {
            EngFuncs::PlayLocalSound("media/launch_deny2.wav");
            return;
        }
    }
    else {
        if (*currentStrings < maxPages) {
            (*currentStrings)++;
        }
        else {
            EngFuncs::PlayLocalSound("media/launch_deny2.wav");
            return;
        }
    }

    GetPage(pagetypeweapon);
}

void CMenuInventory::GetWp(int primary, int secondary, int melee, int grenade)
{
    // Hide all weapons first
    for (int i = 0; i < MAX_WEAPONS; i++) {
        m_WeaponDisplays[i]->Hide();
    }

    // Show selected weapons
    if (primary >= 1 && primary <= 79) {
        m_WeaponDisplays[primary - 1]->Show();
    }
    if (secondary >= 1 && secondary <= 17) {
        m_WeaponDisplays[79 + secondary - 1]->Show(); // 79-95
    }
    if (melee >= 1 && melee <= 15) {
        m_WeaponDisplays[96 + melee - 1]->Show(); // 96-110
    }
}

void CMenuInventory::GetPage(int page)
{
    pagetypeweapon = page;

    // Hide all textures first
    for (int i = 0; i < MAX_WEAPONS; i++) {
        m_WeaponTextures[i]->Hide();
    }

    // Determine which textures to show
    int start = 0, end = 0;
    int* currentStrings = nullptr;

    switch (page) {
    case 1: // Primary (79 оружий = 7 страниц по 12 оружий)
        start = 0; end = 78;
        currentStrings = &stringsprimary;
        downarrow.Show();
        uparrow.Show();

        // Show textures based on current page (7 страниц по 12 оружий)
        if (*currentStrings == 1) {
            for (int i = 0; i <= 11; i++) m_WeaponTextures[i]->Show();
        }
        else if (*currentStrings == 2) {
            for (int i = 12; i <= 23; i++) m_WeaponTextures[i]->Show();
        }
        else if (*currentStrings == 3) {
            for (int i = 24; i <= 35; i++) m_WeaponTextures[i]->Show();
        }
        else if (*currentStrings == 4) {
            for (int i = 36; i <= 47; i++) m_WeaponTextures[i]->Show();
        }
        else if (*currentStrings == 5) {
            for (int i = 48; i <= 59; i++) m_WeaponTextures[i]->Show();
        }
        else if (*currentStrings == 6) {
            for (int i = 60; i <= 71; i++) m_WeaponTextures[i]->Show();
        }
        else if (*currentStrings == 7) {
            for (int i = 72; i <= 78; i++) m_WeaponTextures[i]->Show(); // последняя страница (7 оружий)
        }
        break;

    case 2: // Secondary (17 оружий = 2 страницы)
        start = 79; end = 95;
        currentStrings = &stringssecondary;
        downarrow.Show();
        uparrow.Show();

        // Show textures based on current page
        if (*currentStrings == 1) {
            for (int i = 79; i <= 90; i++) m_WeaponTextures[i]->Show(); // первая страница (12 оружий)
        }
        else if (*currentStrings == 2) {
            for (int i = 91; i <= 95; i++) m_WeaponTextures[i]->Show(); // вторая страница (5 оружий)
        }
        break;

    case 3: // Melee (15 оружий = 2 страницы)
        start = 96; end = 110;
        currentStrings = &stringsmelee;
        downarrow.Show();
        uparrow.Show();

        // Show textures based on current page
        if (*currentStrings == 1) {
            for (int i = 96; i <= 107; i++) m_WeaponTextures[i]->Show(); // первая страница (12 оружий)
        }
        else if (*currentStrings == 2) {
            for (int i = 108; i <= 110; i++) m_WeaponTextures[i]->Show(); // вторая страница (3 оружия)
        }
        break;
    }
}

void CMenuInventory::_VidInit()
{
    GetPage(1);

    int Gun = EngFuncs::GetCvarFloat("wpn_getgun");
    int Pistol = EngFuncs::GetCvarFloat("wpn_getpistol");
    int Knife = EngFuncs::GetCvarFloat("wpn_getknife");
    int Grenade = EngFuncs::GetCvarFloat("wpn_getgrenade");

    GetWp(Gun, Pistol, Knife, Grenade);
}

void CMenuInventory::SaveAndPopMenu()
{
    CMenuFramework::SaveAndPopMenu();
}

void UI_Inventory_Precache(void)
{
    for (int i = 0; i < MAX_PRIMARY; i++) {
        EngFuncs::PIC_Load(WeaponArtPrimary[i]);
    }
    for (int i = 0; i < MAX_SECONDARY; i++) {
        EngFuncs::PIC_Load(WeaponArtSecondary[i]);
    }
    for (int i = 0; i < MAX_MELEE; i++) {
        EngFuncs::PIC_Load(WeaponArtMelee[i]);
    }
    for (int i = 0; i < MAX_GRENADE; i++) {
        EngFuncs::PIC_Load(WeaponArtGrenade[i]);
    }
}

void UI_Inventory_Menu(void)
{
    uiInventory.Show();
}

ADD_MENU(menu_inventory, UI_Inventory_Precache, UI_Inventory_Menu);