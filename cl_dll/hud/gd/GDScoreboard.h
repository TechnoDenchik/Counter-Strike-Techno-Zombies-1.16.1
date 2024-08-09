/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "r_texture.h"
#include <memory>
#include "hud_sub.h"

class CHudGDScoreboard : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void OnKillEnemy(int m_iLevel)
	{
		lv = m_iLevel;
	}
protected:

	int lv;
	UniqueTexture newscoreboard;
	UniqueTexture iconct;
	UniqueTexture icont;
	UniqueTexture iconlv;
	UniqueTexture iconlvmax;
	UniqueTexture slash;
	UniqueTexture winhm;
	UniqueTexture winzb;
	UniqueTexture countplayer;
	UniqueTexture countplayer2;
	UniqueTexture iconteamkill;
	UniqueTexture countround;
	UniqueTexture countkill;

	wrect_t m_rcroundmax[10];
	wrect_t m_rcroundnumber[10];
	wrect_t m_rcTeamnumber[10];
	wrect_t m_rcSelfnumber[10];
	wrect_t m_rcToprecord[10];
	wrect_t m_rcToprecord2[10];
	wrect_t m_rcToprecord3[10];
	float m_flDisplayTime;
};