/* =================================================================================== *
	  * =================== TechnoSoftware & Valve Developing =================== *
 * =================================================================================== */

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
//			
//  hud.h
//
// class CHud declaration
//
// CHud handles the message, calculation, and drawing the HUD
//
#pragma once

#define RGB_YELLOWISH 0x00FFA000 //255,160,0
#define RGB_REDISH    0x00FF1010 //255,16,16
#define RGB_GREENISH 0x0000A000 //0,160,0
#define RGB_WHITE 0x00FFFFFF
#define RGB_LIGHTBLUE 0x0072C5FF //114, 197, 255

#define RGB_LIGHTYELLOW 0x00FFA0000F

#include <assert.h>
#include <string.h>

#include "wrect.h"
#include "cl_dll.h"
#include "ammo.h"
#include "csprite.h"
#include "cvardef.h" // cvar_t

enum GameMode_e : byte; // #include "gamemode/mods_const.h"

#define MIN_ALPHA	 100	
#define	HUDELEM_ACTIVE	1
#define CHudMsgFunc(x) int MsgFunc_##x(const char *pszName, int iSize, void *buf)
#define CHudUserCmd(x) void UserCmd_##x()

class RGBA
{
public:
	unsigned char r, g, b, a;
};

enum 
{ 
	MAX_PLAYERS = 33, // including the bomb
	MAX_TEAMS = 3,
	MAX_TEAM_NAME = 16,
	MAX_HOSTAGES = 24,
};

extern const char *sPlayerModelFiles[];
extern wrect_t nullrc;


class CClientSprite;

inline bool BIsValidTModelIndex( int i )
{
	if ( i == 1 || i == 5 || i == 6 || i == 8 || i == 11 )
		return true;
	else
		return false;
}

inline bool BIsValidCTModelIndex( int i )
{
	if ( i == 7 || i == 2 || i == 10 || i == 4 || i == 9)
		return true;
	else
		return false;
}

#define HUD_DRAW         (1 << 0)
#define HUD_THINK        (1 << 1)
#define HUD_ACTIVE       (HUD_DRAW | HUD_THINK)
#define HUD_INTERMISSION (1 << 2)

#define MAX_PLAYER_NAME_LENGTH		32

#define	MAX_MOTD_LENGTH				1536

//
//-----------------------------------------------------
//
class CHudBase
{
public:
	int	  m_iFlags; // active, moving,
	virtual		~CHudBase() {}
	virtual int Init( void ) {return 0;}
	virtual int VidInit( void ) {return 0;}
	virtual int Draw(float flTime) {return 0;}
	virtual void Think(void) {return;}
	virtual void Reset(void) {return;}
	virtual void InitHUDData( void ) {}		// called every time a server is connected to
	virtual void Shutdown( void ) {}

};

struct HUDLIST {
	CHudBase	*p;
	HUDLIST		*pNext;
};



//
//-----------------------------------------------------
//
//#include "voice_status.h"
#include "hud_spectator.h"
#include "followicon.h"
#include "scenariostatus.h"
#include "health.h"
#include "radar.h"
#include "scoreboard.h"
#include "zbs/zbs.h"
#include "zb2/zb2.h"
#include "zb3/zb3.h"
#include "zsh/zsh.h"
#include "gd/gd.h"
#include "original/mod_base.h"
#include "legacy/hud_scoreboard_legacy.h"
#include "zbs/zbs_scoreboard.h"
#include "retina.h"
#include "moe/moe_touch.h"
#include "hud_sub.h"
#include "r_texture.h"
#include "hud2/NewHud.h"
#include "hud2/NewAlarm.h"
#include "hud2/NewFontManager.h"
#include "Original/Classic.h"
#include "interface/interface.h"


//
//-----------------------------------------------------
//
class CHudAmmo: public CHudBase
{
	friend class WeaponsResource;
	friend class HistoryResource;

public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	void Think(void);
	void Reset(void);

	void DrawCrosshair(float flTime);
	int DrawCrosshairEx(float flTime, int weaponid, int iBarSize, float flCrosshairDistance, bool bAdditive, int r, int g, int b, int a);
	void CalcCrosshairSize();
	void CalcCrosshairDrawMode();
	void CalcCrosshairColor();

	int DrawWList(float flTime);
	CHudMsgFunc(CurWeapon);
	CHudMsgFunc(WeaponList);
	CHudMsgFunc(AmmoX);
	CHudMsgFunc(AmmoPickup);
	CHudMsgFunc(WeapPickup);
	CHudMsgFunc(ItemPickup);
	CHudMsgFunc(HideWeapon);
	CHudMsgFunc(Crosshair);
	CHudMsgFunc(Brass);
	CHudMsgFunc(m_bMordenRadar);


	void SlotInput( int iSlot );
	CHudUserCmd(Slot1);
	CHudUserCmd(Slot2);
	CHudUserCmd(Slot3);
	CHudUserCmd(Slot4);
	CHudUserCmd(Slot5);
	CHudUserCmd(Slot6);
	CHudUserCmd(Slot7);
	CHudUserCmd(Slot8);
	CHudUserCmd(Slot9);
	CHudUserCmd(Slot10);
	CHudUserCmd(Close);
	CHudUserCmd(NextWeapon);
	CHudUserCmd(PrevWeapon);
	CHudUserCmd(Adjust_Crosshair);
	CHudUserCmd(Rebuy);
	CHudUserCmd(Autobuy);

	bool FHasSecondaryAmmo() { return m_pWeapon && m_pWeapon->iAmmo2Type > 0; }

public:

	SharedTexture ammoclips;
	SharedTexture ammofloat;
	UniqueTexture ammoboard;
	UniqueTexture ammoboard2;
	UniqueTexture weaponboard;

	wrect_t m_rcAmmoclip[10];
	wrect_t m_rcAmmofloat[10];

	float m_fFade;
	RGBA  m_rgba;
	WEAPON *m_pWeapon;
	int	m_HUD_bucket0;
	int m_HUD_selection;

	int m_iAlpha;
	int m_R, m_G, m_B;
	int m_cvarR, m_cvarG, m_cvarB;
	int m_iCurrentCrosshair;
	int m_iCrosshairScaleBase;
	float m_flCrosshairDistance;
	bool m_bAdditive;
	bool m_bObserverCrosshair ;
	bool m_bMordenRadar;
	bool m_bDrawCrosshair;
	int m_iAmmoLastCheck;

	cvar_t *m_pClCrosshairColor;
	cvar_t *m_pClCrosshairTranslucent;
	cvar_t *m_pClCrosshairSize;
	cvar_t *m_pClDynamicCrosshair;
	cvar_t *m_pHud_FastSwitch;
	cvar_t *m_pHud_DrawHistory_Time;

	cvar_t *cl_crosshair_type;
};

//
//-----------------------------------------------------
//

class CHudAmmoSecondary: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	void Reset( void );
	int Draw(float flTime);

	CHudMsgFunc(SecAmmoVal);
	CHudMsgFunc(SecAmmoIcon);

private:
	enum {
		MAX_SEC_AMMO_VALUES = 4
	};

	SharedTexture winhm;
	SharedTexture winzb;
	wrect_t m_rcTeamnumber[10];
	wrect_t m_rcSelfnumber[10];

	int m_HUD_ammoicon; // sprite indices
	int m_iAmmoAmounts[MAX_SEC_AMMO_VALUES];
	float m_fFade;
};

#define FADE_TIME 100


//
//-----------------------------------------------------
//
class CHudGeiger: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	CHudMsgFunc(Geiger);
	
private:
	int m_iGeigerRange;

};

//
//-----------------------------------------------------
//
class CHudTrain: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	CHudMsgFunc(Train);

private:
	HSPRITE m_hSprite;
	int m_iPos;

};

//
//-----------------------------------------------------
//
//  MOTD in cs16 must render HTML, so it disabled
//

class CHudMOTD : public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw( float flTime );
	void Reset( void );

	CHudMsgFunc(MOTD);
	void Scroll( int dir );
	void Scroll( float amount );
	float scroll;
	bool m_bShow;
	cvar_t *cl_hide_motd;

protected:
	static int MOTD_DISPLAY_TIME;
	char m_szMOTD[ MAX_MOTD_LENGTH ];
	
	int m_iLines;
	int m_iMaxLength;
	bool ignoreThisMotd;
};

//
//-----------------------------------------------------
//
class CHudStatusBar : public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw( float flTime );
	void Reset( void );
	void ParseStatusString( int line_num );

	CHudMsgFunc(StatusText);
	CHudMsgFunc(StatusValue);

protected:
	enum {
		MAX_STATUSTEXT_LENGTH = 128,
		MAX_STATUSBAR_VALUES = 8,
		MAX_STATUSBAR_LINES = 2,
	};

	char m_szStatusText[MAX_STATUSBAR_LINES][MAX_STATUSTEXT_LENGTH];  // a text string describing how the status bar is to be drawn
	char m_szStatusBar[MAX_STATUSBAR_LINES][MAX_STATUSTEXT_LENGTH];	// the constructed bar that is drawn
	int m_iStatusValues[MAX_STATUSBAR_VALUES];  // an array of values for use in the status bar

	int m_bReparseString; // set to TRUE whenever the m_szStatusBar needs to be recalculated

	// an array of colors...one color for each line
	float *m_pflNameColors[MAX_STATUSBAR_LINES];

	cvar_t *hud_centerid;
};


struct extra_player_info_t 
{
	short frags;
	short deaths;
	short playerclass;
	short teamnumber;
	char teamname[MAX_TEAM_NAME];
	bool has_c4;
	bool vip;
	bool dead;
	bool zombie;
	bool hero;
	bool showhealth;
	bool nextflash;
	bool talking;
	Vector origin;
	int health;
	int radarflashes;
	float radarflashtime;
	float radarflashtimedelta;
	char location[32];
};

struct RoundPlayerInfo
{
	int kill[2];
	int assist;
	float assisttime[3][MAX_PLAYERS + 1];
	float totaldmg[MAX_PLAYERS + 1];
	int revenge;

	int iHealth;
	int iMaxHealth;
};

struct team_info_t 
{
	char name[MAX_TEAM_NAME];
	short frags;
	short deaths;
	short ownteam;
	short players;
	int already_drawn;
	int scores_overriden;
	int sumping;
	int teamnumber;
};

struct hostage_info_t
{
	vec3_t origin;
	float radarflashtimedelta;
	float radarflashtime;
	bool dead;
	bool nextflash;
	int radarflashes;
};

extern hud_player_info_t	g_PlayerInfoList[MAX_PLAYERS+1];	   // player info from the engine
extern extra_player_info_t  g_PlayerExtraInfo[MAX_PLAYERS+1];  
extern extra_player_info_t  g_location[MAX_PLAYERS + 32]; // additional player info sent directly to the client dll
extern team_info_t			g_TeamInfo[MAX_TEAMS+1];
extern hostage_info_t		g_HostageInfo[MAX_HOSTAGES+1];
extern RoundPlayerInfo      g_PlayerExtraInfoEx[MAX_PLAYERS + 1];
extern int					g_IsSpectator[MAX_PLAYERS+1];


class AlarmBasicdata
{
public:
	AlarmBasicdata() {};
public:
	bool bInitialized;

	char szName[32];
	char szSound[64];

	bool bHasMsgBox;
	bool bHasAlarm;

	wchar_t m_wcsAlarmText[64];
	wchar_t m_wcsAlarmDesc[256];
	wchar_t m_wcsRibbonText[64];
	wchar_t m_wcsRibbonDesc[512];

	SharedTexture iTexture;
	int iRibbonRequire;
	SharedTexture iTextureRibbon;


	int index;

};

typedef struct AlarmDisplay_s
{
	bool m_bPlaying;
	bool m_bReset;
	bool m_bBackGround;
	float m_flDisplayTime;

	// Alarm, bRibbon
	std::vector<std::pair<AlarmBasicdata, bool>> m_vecAlarm;

	int m_iCount_Ribbon[ALARM_LAST];
	std::vector<int> m_vecCountRibbonC;
}AlarmDisplay_t;

class CHudNewAlarm : public CHudBase
{
public:
	CHudNewAlarm();
	~CHudNewAlarm();
public:
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);
	void InitHUDData(void);
	void Shutdown();

	CHudMsgFunc(ResetRound);

public:

	//NewAlarm From Sme
	AlarmBasicdata m_AlarmDefault[ALARM_LAST];

	std::vector<AlarmBasicdata> m_vecAlarmCustom;
	AlarmDisplay_t m_AlarmDisplay;
	bool bFirstblood;

	int RedrawAlarm(float flTime);
	AlarmBasicdata InitAlarm(char* szName, int iType, bool bCustom);
	void SetAlarm(int iAlarm, bool bCustom = false);

	int FindAlarm(char* szName, int iType);
	bool IsPlaying(void)
	{
		if (m_AlarmDisplay.m_vecAlarm.size())
			return true;

		return false;
	}
	bool IsPlaying(int iAlarm, bool bCustom);

	int GetWide(const wchar_t* str);
	void DrawVguiTexts(int x, int y, int r, int g, int b, int a, const wchar_t* str);
private:
	UniqueTexture m_iTextureBG;
	UniqueTexture m_iTextureLogoBG;
	bool bTextureLoaded;
	bool bAlarmRead;

};

CHudNewAlarm& NewAlarm(void);

typedef struct deathinfo_s
{
	bool bDraw = false;
	int iPlayer = 0;
	int iDist = 0;
	long iDamage[6]{ 0 };
	int iShot[5]{};
	char szWeapon[64]{};
	int iHealth[2]{};
	float fDisplaytime = 0.0;
}
deathinfo_t;

class CHudDeathInfo
{
public:
	CHudDeathInfo();
	~CHudDeathInfo();

	void Init(void);
	void VidInit(void);
	void Set(deathinfo_t data, int type); //killer = 1, victim = 2
	void Redraw(void);

	void ReplaceTokenFirst(char* szString, const char* szToken, const char* szSwitch);
	void ReplaceTokenAll(char* szString, const char* szToken, const char* szSwitch);
	char* UnicodeToUTF8(const wchar_t* str);
	wchar_t* UTF8ToUnicode(const char* str);

	void InitDeathInfo(deathinfo_t* temp);

	CHudMsgFunc(DeathInfo);

private:
	deathinfo_t Killer;
	deathinfo_t Victim;
	deathinfo_t Victim2;

	char m_szMsgEnemyDeath[64];
	char m_szBody[5][32];
	char m_szAmmoCount[16];

	char m_szMsgDamageToKiller[64];
	char m_szMsgKillerState[64];
	char m_szMsgDamagedNone[64];

	char m_szTempKiller[512], m_szTempVictim[512];
	wchar_t m_wszTotalKillerText[512], m_wszTotalVictimText[512];
};

CHudDeathInfo& HudDeathInfo(void);

//
//-----------------------------------------------------
//
class CHudDeathNotice : public CHudBase
{
public:
	int Init( void );
	void Reset(void);
	void InitHUDData( void );
	int VidInit( void );
	void Shutdown(void);
	int Draw( float time );

	void headshots();
	void crazy();
	void excellent();
	void knife();
	void incredible();
	void cantbelieve();

	CHudMsgFunc(DeathMsg);

private:
	
	int m_HUD_d_skull;  // sprite index of skull icon
	int m_HUD_d_headshot;
	cvar_t *hud_deathnotice_time;

	int m_killNums, m_multiKills;
	int m_iconIndex;
	bool m_showIcon, m_showKill;
	float m_killEffectTime, m_killIconTime;
protected:

	SharedTexture ribbon_headshot;
	SharedTexture ribbon_crazy;
	SharedTexture ribbon_excellent;
	SharedTexture ribbon_knife;
	SharedTexture ribbon_incredible;
	SharedTexture ribbon_cantbelieve;
	SharedTexture m_pCurTexture;
	float m_flDisplayTime;
private:
	int m_killBg[3];
	int m_deathBg[3];
	int m_KM_Number0;
	int m_KM_Number1;
	int m_KM_Number2;
	int m_KM_Number3;
	int m_KM_KillText;
	int m_KM_Icon_Head;
	int m_KM_Icon_Knife;
	int m_KM_Icon_Frag;
};

//
//-----------------------------------------------------
//
class CHudMenu : public CHudBase
{
public:
	int Init( void );
	void InitHUDData( void );
	int VidInit( void );
	void Reset( void );
	int Draw( float flTime );

	CHudMsgFunc(ShowMenu);
	CHudMsgFunc(BuyClose);
	CHudMsgFunc(VGUIMenu);
	// server sends false when spectating is not allowed, and true when allowed
	CHudMsgFunc(AllowSpec);

	CHudUserCmd(OldStyleMenuClose);
	CHudUserCmd(OldStyleMenuOpen);
	CHudUserCmd(ShowVGUIMenu);

	void ShowVGUIMenu( int menuType ); // cs16client extension

	void SelectMenuItem( int menu_item );

	int m_fMenuDisplayed;
	bool m_bAllowSpec;
	cvar_t *_extended_menus;
	int m_bitsValidSlots;
	float m_flShutoffTime;
	int m_fWaitingForMore;

};

//
//-----------------------------------------------------
//
class CHudSayText : public CHudBase
{
public:
	int Init( void );
	void InitHUDData( void );
	int VidInit( void );
	int Draw( float flTime );
	CHudMsgFunc(SayText);
	void SayTextPrint( const char *pszBuf, int iBufSize, int clientIndex = -1 );
	void SayTextPrint( char szBuf[3][256] );
	void EnsureTextFitsInOneLineAndWrapIfHaveTo( int line );
	friend class CHudSpectator;

private:

	struct cvar_s *	m_HUD_saytext;
	struct cvar_s *	m_HUD_saytext_time;
};

#define DMG_IMAGE_LIFE		2	// seconds that image is up

#define DMG_IMAGE_POISON	0
#define DMG_IMAGE_ACID		1
#define DMG_IMAGE_COLD		2
#define DMG_IMAGE_DROWN		3
#define DMG_IMAGE_BURN		4
#define DMG_IMAGE_NERVE		5
#define DMG_IMAGE_RAD		6
#define DMG_IMAGE_SHOCK		7
//tf defines
#define DMG_IMAGE_CALTROP	8
#define DMG_IMAGE_TRANQ		9
#define DMG_IMAGE_CONCUSS	10
#define DMG_IMAGE_HALLUC	11
#define NUM_DMG_TYPES		12
// instant damage

#define DMG_GENERIC			0			// generic damage was done
#define DMG_CRUSH			(1 << 0)	// crushed by falling or moving object
#define DMG_BULLET			(1 << 1)	// shot
#define DMG_SLASH			(1 << 2)	// cut, clawed, stabbed
#define DMG_BURN			(1 << 3)	// heat burned
#define DMG_FREEZE			(1 << 4)	// frozen
#define DMG_FALL			(1 << 5)	// fell too far
#define DMG_BLAST			(1 << 6)	// explosive blast damage
#define DMG_CLUB			(1 << 7)	// crowbar, punch, headbutt
#define DMG_SHOCK			(1 << 8)	// electric shock
#define DMG_SONIC			(1 << 9)	// sound pulse shockwave
#define DMG_ENERGYBEAM		(1 << 10)	// laser or other high energy beam 
#define DMG_NEVERGIB		(1 << 12)	// with this bit OR'd in, no damage type will be able to gib victims upon death
#define DMG_ALWAYSGIB		(1 << 13)	// with this bit OR'd in, any damage type can be made to gib victims upon death.


// time-based damage
//mask off TF-specific stuff too
#define DMG_TIMEBASED		(~(0xff003fff))	// mask for time-based damage


#define DMG_DROWN			(1 << 14)	// Drowning
#define DMG_FIRSTTIMEBASED  DMG_DROWN

#define DMG_PARALYZE		(1 << 15)	// slows affected creature down
#define DMG_NERVEGAS		(1 << 16)	// nerve toxins, very bad
#define DMG_POISON			(1 << 17)	// blood poisioning
#define DMG_RADIATION		(1 << 18)	// radiation exposure
#define DMG_DROWNRECOVER	(1 << 19)	// drowning recovery
#define DMG_ACID			(1 << 20)	// toxic chemicals or acid burns
#define DMG_SLOWBURN		(1 << 21)	// in an oven
#define DMG_SLOWFREEZE		(1 << 22)	// in a subzero freezer
#define DMG_MORTAR			(1 << 23)	// Hit by air raid (done to distinguish grenade from mortar)

//TF ADDITIONS
#define DMG_IGNITE			(1 << 24)	// Players hit by this begin to burn
#define DMG_RADIUS_MAX		(1 << 25)	// Radius damage with this flag doesn't decrease over distance
#define DMG_RADIUS_QUAKE	(1 << 26)	// Radius damage is done like Quake. 1/2 damage at 1/2 radius.
#define DMG_IGNOREARMOR		(1 << 27)	// Damage ignores target's armor
#define DMG_AIMED			(1 << 28)   // Does Hit location damage
#define DMG_WALLPIERCING	(1 << 29)	// Blast Damages ents through walls

#define DMG_CALTROP				(1<<30)
#define DMG_HALLUC				(1<<31)

// TF Healing Additions for TakeHealth
#define DMG_IGNORE_MAXHEALTH	DMG_IGNITE
// TF Redefines since we never use the originals
#define DMG_NAIL				DMG_SLASH
#define DMG_NOT_SELF			DMG_FREEZE


#define DMG_TRANQ				DMG_MORTAR
#define DMG_CONCUSS				DMG_SONIC


struct DAMAGE_IMAGE
{
	float fExpire;
	float fBaseline;
	int	x, y;
};

class CHudHealth : public CHudBase
{
	friend class CHudBattery;
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(float fTime);
	virtual void Reset(void);
	void InitHUDData(void);
	CHudMsgFunc(Battery);
	CHudMsgFunc(ArmorType);

	int MsgFunc_Health(const char* pszName, int iSize, void* pbuf);
	int MsgFunc_Damage(const char* pszName, int iSize, void* pbuf);
	int MsgFunc_ScoreAttrib(const char* pszName, int iSize, void* pbuf);
	int MsgFunc_ClCorpse(const char* pszName, int iSize, void* pbuf);
	int m_iHealth;
private:
enum armortype_t {
		Vest = 0,
		VestHelm
	} m_enArmorType;

 int	 m_iBat;
	CClientSprite m_hEmpty[VestHelm + 1];
	CClientSprite m_hFull[VestHelm + 1];

	float m_fFade;
	int	  m_iHeight;		// width of the battery innards

	
	int m_HUD_dmg_bio;
	int m_HUD_cross;
	float m_fAttack[4];

	void DrawPain(float fTime);
	void DrawDamage(float fTime);
	void DrawHealthBar(float flTime);
	void CalcDamageDirection(Vector vecFrom);
	void UpdateTiles(float fTime, long bits);
	void DrawPlayerLocation(void);

	HSPRITE m_hSprite;
	UniqueTexture m_health_board;
	UniqueTexture m_ihealthes;
	UniqueTexture m_ihealthes_top;
	UniqueTexture m_plus;
	UniqueTexture m_armors;
	wrect_t ihealth[10];
	wrect_t iarmors[10];
	HSPRITE m_hDamage;
	Vector2D m_vAttackPos[4];
	DAMAGE_IMAGE m_dmg[NUM_DMG_TYPES];
	float m_flTimeFlash;
	int	m_bitsDamage;
	cvar_t* cl_radartype;
};


//
//-----------------------------------------------------
//
/*
class CHudBattery: public CHudBase
{

public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	void InitHUDData( void );
	CHudMsgFunc(Battery);
	CHudMsgFunc(ArmorType);
	static int	 m_iBat;
private:
	enum armortype_t {
		Vest = 0,
		VestHelm
	} m_enArmorType;

	CClientSprite m_hEmpty[VestHelm + 1];
	CClientSprite m_hFull[VestHelm + 1];
	
	float m_fFade;
	int	  m_iHeight;		// width of the battery innards
};*/


//
//-----------------------------------------------------
//
class CHudFlashlight: public CHudBase
{
public:
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);
	//int DrawNewHudFlashLight(float flTime);
	void Reset(void);
	CHudMsgFunc(Flashlight);
	CHudMsgFunc(FlashBat);

private:
	CClientSprite m_hSprite1;
	CClientSprite m_hSprite2;
	CClientSprite m_hBeam;
	float m_flBat;
	int	  m_iBat;
	int	  m_fOn;
	float m_fFade;
	int	  m_iWidth;
};

//
//-----------------------------------------------------
//
const int maxHUDMessages = 16;
struct message_parms_t
{
	client_textmessage_t	*pMessage;
	float	time;
	int x, y;
	int	totalWidth, totalHeight;
	int width;
	int lines;
	int lineLength;
	int length;
	int r, g, b;
	int text;
	int fadeBlend;
	float charTime;
	float fadeTime;
};

//
//-----------------------------------------------------
//

class CHudTextMessage: public CHudBase
{
public:
	int Init( void );
	static char *LocaliseTextString( const char *msg, char *dst_buffer, int buffer_size );
	static char *BufferedLocaliseTextString( const char *msg );
	static char *LookupString( char *msg_name, int *msg_dest = NULL );
	CHudMsgFunc(TextMsg);
};

//
//-----------------------------------------------------
//

class CHudMessage: public CHudBase
{
public:
	friend class CHudTextMessage;
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	CHudMsgFunc(HudText);
	CHudMsgFunc(GameTitle);
	CHudMsgFunc(HudTextArgs);
	CHudMsgFunc(HudTextPro);

	float FadeBlend( float fadein, float fadeout, float hold, float localTime );
	int	XPosition( float x, int width, int lineWidth );
	int YPosition( float y, int height );

	void MessageAdd( const char *pName, float time );
	void MessageAdd(client_textmessage_t * newMessage );
	void MessageDrawScan( client_textmessage_t *pMessage, float time );
	void MessageScanStart( void );
	void MessageScanNextChar( void );
	void Reset( void );

private:
	client_textmessage_t		*m_pMessages[maxHUDMessages];
	float						m_startTime[maxHUDMessages];
	message_parms_t				m_parms;
	float						m_gameTitleTime;
	client_textmessage_t		*m_pGameTitle;

	int m_HUD_title_life;
	int m_HUD_title_half;
};

//
//-----------------------------------------------------
//
#define MAX_SPRITE_NAME_LENGTH	24

class CHudStatusIcons: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	void Reset( void );
	void Shutdown(void);
	int Draw(float flTime);
	CHudMsgFunc(StatusIcon);

	enum {
		MAX_ICONSPRITENAME_LENGTH = MAX_SPRITE_NAME_LENGTH,
		MAX_ICONSPRITES = 4,
	};

	
	//had to make these public so CHud could access them (to enable concussion icon)
	//could use a friend declaration instead...
	void EnableIcon( const char *pszIconName, unsigned char red, unsigned char green, unsigned char blue );
	void DisableIcon( const char *pszIconName );
	
	
	friend class CHudScoreboard;
protected:
	UniqueTexture b_iconimage;
private:

	typedef struct
	{
		char szSpriteName[MAX_ICONSPRITENAME_LENGTH];
		HSPRITE spr;
		wrect_t rc;
		unsigned char r, g, b;
		unsigned char secR, secG, secB;
		float flTimeToChange;
	} icon_sprite_t;

	icon_sprite_t m_IconList[MAX_ICONSPRITES];
	int m_tgaC4[2];
};
 

//
//-----------------------------------------------------
//
#define MONEY_YPOS ScreenHeight - 3 * gHUD.m_iFontHeight

class CHudMoney : public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw( float flTime );
	CHudMsgFunc(Money);
	CHudMsgFunc(BlinkAcct);

private:
	int m_iMoneyCount;
	int m_iDelta;
	int m_iBlinkAmt;
	float m_fBlinkTime;

	UniqueTexture m_iDollarBG;
	UniqueTexture m_iDollar;
	UniqueTexture money_count;
	
	wrect_t rc_money_count[10];
	float m_fFade;

	CClientSprite m_hDollar;
	CClientSprite m_hPlus;
	CClientSprite m_hMinus;
	//newhud
	
	int m_NEWHUD_hDollar;
	int m_NEWHUD_hMinus;
};
//
//-----------------------------------------------------
//
class CHudRadio: public CHudBase
{
public:
	int Init( void );
	void Voice(int entindex, bool bTalking );
	// play a sentence from a radio
	// [byte] unknown (always 1)
	// [string] sentence name
	// [short] unknown. (always 100, it's a volume?)
	CHudMsgFunc(SendAudio);
	CHudMsgFunc(ReloadSound);
	CHudMsgFunc(BotVoice);
};

//
//-----------------------------------------------------
//
class CHudTimer: public CHudBase
{
	friend class CHudSpectatorGui;
	friend class CHudZB3ScoreBoard;
	friend class CHudScenarioStatus;
public:
	//CHudTimer();
	int Init( void );
	int VidInit( void );
	void Reset(void);
	int Draw(float fTime);
//	int DrawNEWHudTimer(float fTime);
	// set up the timer.
	// [short]
	CHudMsgFunc(RoundTime);
	// show the timer
	// [empty]
	CHudMsgFunc(ShowTimer);
protected:

private:
	UniqueTexture m_pTexture_Black;
	UniqueTexture m_timer;
	UniqueTexture m_colon;
	wrect_t rc_m_timer_s[10];
	wrect_t rc_m_timer_m[10];
	//NewHud
	UniqueTexture m_iColon_Bottom;
	SharedTexture m_iNum_Bottom;
	wrect_t m_iNum_BottomC[10];
	int m_HUD_timer;
	int m_iTime;
	float m_fStartTime;
	bool m_bPanicColorChange;
	float m_flPanicTime;
	int m_closestRight;
};
//
//-----------------------------------------------------
//
class CHudProgressBar: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw( float flTime );

	// start progress bar
	// [short] Duration
	CHudMsgFunc(BarTime);

	// [short] Duration
	// [short] percent
	CHudMsgFunc(BarTime2);
	CHudMsgFunc(BotProgress);

private:
	int m_iDuration;
	float m_fPercent;
	float m_fStartTime;
	char m_szHeader[256];
	const char *m_szLocalizedHeader;
};

//
//-----------------------------------------------------
//
// class for drawing sniper scope
class CHudSniperScope: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw( float flTime );
	void Shutdown( void );

private:
	float left, right, centerx, centery;
	int m_iScopeArc[4];
};

//
//-----------------------------------------------------
//

class CHudNVG: public CHudBase
{
public:
	int Init( void );
	int VidInit(void);
	int Draw( float flTime );
	CHudMsgFunc(NVGToggle);

	CHudUserCmd(NVGAdjustUp);
	CHudUserCmd(NVGAdjustDown);
private:
	int m_iAlpha;
	cvar_t *cl_fancy_nvg;
	dlight_t *m_pLight;
	float m_LightStypeBackup;
};

//
//-----------------------------------------------------
//

class CHudSpectatorGui: public CHudBase
{
public:
	int Init();
	int VidInit();
	int Draw( float flTime );
	void InitHUDData();
	void Reset();
	void Shutdown();

	CHudMsgFunc( SpecHealth );
	CHudMsgFunc( SpecHealth2 );

	CHudUserCmd( ToggleSpectatorMenu );
	CHudUserCmd( ToggleSpectatorMenuOptions );
	CHudUserCmd( ToggleSpectatorMenuOptionsSettings );
	CHudUserCmd( ToggleSpectatorMenuSpectateOptions );

	void CalcAllNeededData( );

	bool m_bBombPlanted;
	int m_iPlayerLastPointedAt;
	static float m_fTextScale;

private:	
	// szMapName is 64 bytes only. Removing "maps/" and ".bsp" gived me this result
	class Labels
	{
	public:
		short m_iTerrorists;
		short m_iCounterTerrorists;
		char m_szTimer[64];
		char m_szMap[64];
		char m_szNameAndHealth[80];
	} label;
	int m_hTimerTexture;

	enum {
		ROOT_MENU = (1<<0),
		MENU_OPTIONS = (1<<1),
		MENU_OPTIONS_SETTINGS = (1<<2),
		MENU_SPEC_OPTIONS = (1<<3)
	};
	byte m_menuFlags;
};

//
//-----------------------------------------------------
//

class CHudHeadName : public CHudBase
{
public:
	int Init();
	int VidInit();
	int Draw(float flTime);
	bool CheckForPlayer(cl_entity_s *pEnt);
private:
	SharedTexture ourforces;
};

//
//-----------------------------------------------------
//

class CHudHitIndicator : public CHudBase
{
public:
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);
	void InitHUDData(void);
	void Shutdown();
	void Draw2DQuad_Custom(float x, float y, float scale, int num, int iTexID, int alpha);
	CHudMsgFunc(HitMsg);

private:
	cvar_t* hud_hitindicator_style;
	int current_style;
	SharedTexture m_iTex[5];


};

class CHudSiFiammo : public CHudBase
{
public:
	int Init(void);
	void InitHUDData(void);
	int VidInit(void);
	void Shutdown(void);
	int Draw(float flTime);
	void Draw2DQuad_Custom(float x, float y, float scale, int num, int r, int g, int b, int alpha);

private:
	SharedTexture m_iTex;
	cvar_t* hud_sifiammo_style;
	int current_style;
};

class CHud
{
public:
	CHud() : m_pHudList(NULL), m_iSpriteCount(0)  {}
	~CHud();			// destructor, frees allocated memory // thanks, Captain Obvious

	void Init( void );
	void VidInit( void );
	void Think( void );
	void Shutdown( void );
	int Redraw( float flTime, int intermission );
	int UpdateClientData( client_data_t *cdata, float time );
	void AddHudElem(CHudBase *p);

	inline float GetSensitivity() { return m_flMouseSensitivity; }
	inline HSPRITE GetSprite( int index )
	{
		assert( index >= -1 && index <= m_iSpriteCount );

		return (index >= 0) ? m_rghSprites[index] : 0;
	}

	inline wrect_t& GetSpriteRect( int index )
	{
		assert( index >= -1 && index <= m_iSpriteCount );

		return (index >= 0) ? m_rgrcRects[index]: nullrc;
	}

	// GetSpriteIndex()
	// searches through the sprite list loaded from hud.txt for a name matching SpriteName
	// returns an index into the gHUD.m_rghSprites[] array
	// returns -1 if sprite not found
	inline int GetSpriteIndex( const char *SpriteName )
	{
		// look through the loaded sprite name list for SpriteName
		for ( int i = 0; i < m_iSpriteCount; i++ )
		{
			if ( strnicmp( SpriteName, m_rgszSpriteNames + (i * MAX_SPRITE_NAME_LENGTH), MAX_SPRITE_NAME_LENGTH ) == 0 )
				return i;
		}

		gEngfuncs.Con_Printf( "GetSpriteIndex: %s sprite not found", SpriteName );
		return -1; // invalid sprite
	}

	inline short GetCharWidth ( unsigned char ch )
	{
		return m_scrinfo.charWidths[ ch ];
	}
	inline int GetCharHeight( )
	{
		return m_scrinfo.iCharHeight;
	}

	inline bool IsCZero( )
	{
		return m_bIsCZero;
	}


	float   m_flTime;      // the current client time
	float   m_fOldTime;    // the time at which the HUD was last redrawn
	double  m_flTimeDelta; // the difference between flTime and fOldTime
	float   m_flScale;     // hud_scale->value
	Vector	m_vecOrigin;
	Vector	m_vecAngles;
	int		m_iKeyBits;
	int		m_iHideHUDDisplay;
	int		m_iFOV;
	int		m_Teamplay;
	int		m_iRes;
	cvar_t *m_pCvarDraw;
	cvar_t *cl_shadows;
	cvar_t *fastsprites;
	cvar_t *cl_predict;
	cvar_t *cl_weapon_wallpuff;
	cvar_t *cl_weapon_sparks;
	cvar_t *zoom_sens_ratio;
	cvar_t *cl_lw;
	cvar_t *cl_righthand;
	cvar_t *cl_weather;
	cvar_t *cl_minmodels;
	cvar_t *cl_min_t;
	cvar_t *cl_min_ct;
	cvar_t *cl_gunsmoke;
	cvar_t *default_fov;
	cvar_t *hud_textmode;
	cvar_t *hud_colored;
	cvar_t *sv_skipshield;
	cvar_t *m_hudstyle;
	cvar_t* m_alarmstyle;

	cvar_t *cl_headname;
#ifdef __ANDROID__
	cvar_t *cl_android_force_defaults;
#endif

	HSPRITE m_hGasPuff;

	int m_iFontHeight;
	int m_NEWHUD_iFontWidth;
	int m_NEWHUD_iFontWidth_Dollar;
	int m_NEWHUD_iFontHeight;
	int m_NEWHUD_iFontHeight_Dollar;
	int m_iMapHeight;
	CHudAmmo        m_Ammo;
	CHudHealth      m_Health;
	CHudSpectator   m_Spectator;
	CHudGeiger      m_Geiger;
	CHudHitIndicator m_HitIndicator;
	CHudTrain       m_Train;
	CHudFlashlight  m_Flash;
	CHudMessage     m_Message;
	CHudStatusBar   m_StatusBar;
	CHudDeathNotice m_DeathNotice;
	CHudSayText     m_SayText;
	CHudMenu        m_Menu;
	CHudAmmoSecondary m_AmmoSecondary;
	CHudTextMessage m_TextMessage;
	CHudStatusIcons m_StatusIcons;
	CHudScoreboard  m_Scoreboard;
	
	CHudDrawFontText m_DrawFontText;
	CHudMOTD        m_MOTD;
	CHudMoney       m_Money;
	CHudTimer       m_Timer;
	CHudRadio       m_Radio;
	CHudProgressBar m_ProgressBar;
	CHudSniperScope m_SniperScope;
	CHudNVG         m_NVG;
	CHudRadar       m_Radar;
	CHudSpectatorGui m_SpectatorGui;
	CHudDeathInfo m_DeathInfo;
	CHudNewHud m_NewHud;
	CHudNewAlarm  m_NewAlarm;
	//CHudDrawFontText m_DrawFontText;
	CHudFollowIcon	m_FollowIcon;
	CHudScenarioStatus m_scenarioStatus;
	CHudSiFiammo m_HudSiFiammo;
	CHudHeadName	m_HeadName;
	CHudRetina		m_Retina;
	CHudScoreBoardLegacy m_legacy_score;
	CHudZBS	m_ZBS;
	CHudZB2 m_ZB2;
	CHudClassic m_CLS;
	CHudGunDeath m_gd;
	CHudZB3 m_ZB3;
	CHudZSH m_ZSH;
	CHudMoeTouch m_MoeTouch;
	//CHudInterface m_HudInterface;
	// user messages
	CHudMsgFunc(Damage);
	CHudMsgFunc(GameMode);
	CHudMsgFunc(Logo);
	CHudMsgFunc(ResetHUD);
	CHudMsgFunc(InitHUD);
	CHudMsgFunc(ViewMode);
	CHudMsgFunc(SetFOV);
	CHudMsgFunc(Concuss);
	CHudMsgFunc(ShadowIdx);

	// Screen information
	SCREENINFO	m_scrinfo;
	// As Xash3D can fake m_scrinfo for hud scailing
	// we will use a real screen parameters
	SCREENINFO  m_truescrinfo;

	int	m_iWeaponBits;
	int	m_fPlayerDead;
	int m_iIntermission;
	int m_iNoConsolePrint;
	bool m_bMordenRadar;

	int m_NEWHUD_number_0;
	int m_NEWHUD_dollar_number_0;
	int m_iWeaponGet;
	int m_NEWHUD_hPlus;

	// sprite indexes
	int m_HUD_number_0;

	char m_szServerName[64];

	GameMode_e m_iModRunning;

private:
	HUDLIST	*m_pHudList;
	HSPRITE	m_hsprLogo;
	int	m_iLogo;
	client_sprite_t	*m_pSpriteList;
	int	m_iSpriteCount;
	int	m_iSpriteCountAllRes;
	float m_flMouseSensitivity;
	int	m_iConcussionEffect;
	int	m_iForceCamera;
	int m_iForceChaseCam;
	int m_iFadeToBlack;
	bool m_bIsCZero;

	// the memory for these arrays are allocated in the first call to CHud::VidInit(), when the hud.txt and associated sprites are loaded.
	// freed in ~CHud()
	HSPRITE *m_rghSprites;	/*[HUD_SPRITE_COUNT]*/			// the sprites loaded from hud.txt
	wrect_t *m_rgrcRects;	/*[HUD_SPRITE_COUNT]*/
	char *m_rgszSpriteNames; /*[HUD_SPRITE_COUNT][MAX_SPRITE_NAME_LENGTH]*/
};

extern CHud gHUD;
extern cvar_t *sensitivity;
extern long g_iDamage[MAX_CLIENTS + 1];
extern long g_iDamageTotal[MAX_CLIENTS + 1];
extern double g_flDamageInAll;
extern int g_iTeamNumber;
extern int g_iUser1;
extern int g_iUser2;
extern int g_iUser3;
