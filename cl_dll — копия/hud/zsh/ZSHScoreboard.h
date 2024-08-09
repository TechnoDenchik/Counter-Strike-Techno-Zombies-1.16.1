#include "r_texture.h"
#include <memory>
#include "hud_sub.h"

class CHudZSHScoreboard : public IBaseHudSub
{
public:
	friend class CHudZSH;
	int VidInit(void) override;
	int Draw(float time) override;
	int m_iTeamScore_T, m_iTeamScore_CT;
	void UpdateDay(int day)
	{
		days = day;
	}

protected:
	int days;

	int maxwoods;
	int maxmetal;

	int woods;
	int metal;

	UniqueTexture newscoreboardzsh;
	UniqueTexture textstring;
	UniqueTexture countplayer;
	UniqueTexture countplayer2;
	UniqueTexture count;

	SharedTexture m_pCurTexture;
	
	UniqueTexture zsht_gungergauge_bg;
	UniqueTexture zsht_moon_icon;
	UniqueTexture zsht_sun_icon;
	UniqueTexture noisegauge_bg;
	UniqueTexture noisegauge_bg_red;
	UniqueTexture survivalfail;
	UniqueTexture survivalsuccess;

	wrect_t m_rcAmmocount[10];
	wrect_t m_rcTeamnumber[10];
	wrect_t m_rcSelfnumber[10];
	wrect_t m_rcToprecord[10];
	wrect_t m_rcToprecord2[10];


	
private:

	int m_iBGIndex;
	int m_iTextIndex;
	int m_iTTextIndex, m_iCTTextIndex;
	int countgetammo;
	float m_flDisplayTime;

	wrect_t m_rcNumber_Large[10];
	wrect_t m_rcNumber_Small[10];

	int m_iOriginalBG;
	int m_iTeamDeathBG;
	int m_iUnitehBG;
	int m_iNum_L;
	int m_iNum_S;
	int m_iText_CT;
	int m_iText_T;
	int m_iText_TR;
	int m_iText_HM;
	int m_iText_ZB;
	int m_iText_1st;
	int m_iText_Kill;
	int m_iText_Round;
};