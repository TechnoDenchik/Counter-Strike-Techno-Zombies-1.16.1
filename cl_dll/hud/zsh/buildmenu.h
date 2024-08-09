/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "r_texture.h"
#include <memory>
#include "hud_sub.h"

class CHudZSHBuildmenu : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;
	int m_iTeamScore_T, m_iTeamScore_CT;


protected:
	UniqueTexture newscoreboard;
	UniqueTexture textstring;
	UniqueTexture healthboard;
	UniqueTexture healthboard2;
	SharedTexture iconwinhm;
	SharedTexture iconwinzb;
	UniqueTexture iconhm;
	UniqueTexture iconzb;
	UniqueTexture ammoboard;
	UniqueTexture weaponboard;
	UniqueTexture iconround;
	UniqueTexture iconclassmilitia;
	UniqueTexture winhm;
	UniqueTexture winzb;
	UniqueTexture countplayer;
	UniqueTexture ammocounts;
	SharedTexture m_pCurTexture;
	float m_flDisplayTime;
	//virtual	void HumanWin();
	//virtual void ZombieWin();
wrect_t m_rcAmmocount[10];
	wrect_t m_rcTeamnumber[10];
	wrect_t m_rcSelfnumber[10];
	wrect_t m_rcToprecord[10];


	
private:
	int m_iBGIndex;
	int m_iTextIndex;
	int m_iTTextIndex, m_iCTTextIndex;
	int countgetammo;


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

//CHudZSHScoreboard m_zshscore;