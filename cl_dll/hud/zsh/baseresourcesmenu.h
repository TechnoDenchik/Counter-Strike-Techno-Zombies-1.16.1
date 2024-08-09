/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "r_texture.h"
#include <memory>
#include "hud_sub.h"

class CHudZSHMenuResources : public IBaseHudSub
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
	
wrect_t m_rcAmmocount[10];
	wrect_t m_rcTeamnumber[10];
	wrect_t m_rcSelfnumber[10];
	wrect_t m_rcToprecord[10];
	wrect_t m_rcNumber_Large[10];
	wrect_t m_rcNumber_Small[10];

	
private:
	int m_iBGIndex;
	int m_iTextIndex;
	int m_iTTextIndex, m_iCTTextIndex;
	int countgetammo;
};