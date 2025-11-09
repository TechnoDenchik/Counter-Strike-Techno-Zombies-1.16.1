/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "r_texture.h"
#include <memory>
#include "hud_sub.h"
#include "zb5/TextSetZb5.h"



class CHudZB3ScoreBoard : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;
	void InitHUDData(void);
	

	CHudTextNum ev;
protected:
	

	UniqueTexture m_iCharacterBG_New_Bottom;
	UniqueTexture newscoreboard;
	UniqueTexture slash;
	UniqueTexture textstring;
	UniqueTexture iconhm;
	UniqueTexture iconzb;
	UniqueTexture iconct;
	UniqueTexture icont;
	UniqueTexture iconlv;
	UniqueTexture iconlvmax;
	UniqueTexture ammoboard;
	UniqueTexture weaponboard;
	UniqueTexture iconround;
	UniqueTexture winhm;
	UniqueTexture winzb;
	UniqueTexture countplayer;
	UniqueTexture countplayer2;
	UniqueTexture iconteamkill;
	UniqueTexture countround;
	UniqueTexture countkill;

	UniqueTexture m_iIcon_Speed;
	UniqueTexture m_iIcon_Damage;

	SharedTexture m_iNum_Character;

	wrect_t m_iNum_CharacterC[10];

	UniqueTexture newscoreboarddm;
	UniqueTexture iconkill;
	UniqueTexture iconmy;
	UniqueTexture whitebig;
	UniqueTexture icononest;

	UniqueTexture m_iColon_Bottom;
	SharedTexture m_iNum_Bottom;
	wrect_t m_iNum_BottomC[10];

	SharedTexture m_pCurTexture;
	UniqueTexture m_iCharacter;
	std::string m_szLastModel;
	wrect_t m_rcroundmax[10];
	wrect_t m_rcroundnumber[10];
	wrect_t m_rcTeamnumber[10];
	wrect_t m_rcSelfnumber[10];
	wrect_t m_rcToprecord[10];
	wrect_t m_rcToprecord2[10];
	wrect_t m_rcToprecord3[10];

	SharedTexture m_pCurTexture1;
	SharedTexture m_pCurTexture2;
	SharedTexture m_pCurTexture3;
	SharedTexture m_pCurTexture4;
	SharedTexture m_pCurTexture5;
	SharedTexture m_pCurTexture6;
	SharedTexture m_pCurTexture7;
	SharedTexture m_pCurTexture8;
	SharedTexture m_pCurTexture9;
	SharedTexture m_pCurTexture10;
	SharedTexture m_pCurTexture11;
	SharedTexture m_pCurTexture12;
	SharedTexture m_pCurTexture13;
	SharedTexture m_pCurTexture14;
	SharedTexture m_pCurTexture15;
	SharedTexture m_pCurTexture16;
	SharedTexture m_pCurTexture17;
	SharedTexture m_pCurTexture18;
	SharedTexture m_pCurTexture19;
	SharedTexture m_pCurTexture20;
	SharedTexture m_pCurTexture21;
	SharedTexture m_pCurTexture22;
	SharedTexture m_pCurTexture23;
	SharedTexture m_pCurTexture24;
	SharedTexture m_pCurTexture25;
	SharedTexture m_pCurTexture26;
	SharedTexture m_pCurTexture27;
	SharedTexture m_pCurTexture28;
	SharedTexture m_pCurTexture29;
	SharedTexture m_pCurTexture30;
	SharedTexture m_pCurTexture31;
	SharedTexture m_pCurTexture32;
	SharedTexture m_pCurTexture33;
	SharedTexture m_pCurTexture34;
	SharedTexture m_pCurTexture35;
	SharedTexture m_pCurTexture36;
	SharedTexture m_pCurTexture37;
	SharedTexture m_pCurTexture38;
	SharedTexture m_pCurTexture39;
	SharedTexture m_pCurTexture40;
	SharedTexture m_pCurTexture41;
	SharedTexture m_pCurTexture42;
	SharedTexture m_pCurTexture43;
	SharedTexture m_pCurTexture44;
	SharedTexture m_pCurTexture45;
	SharedTexture m_pCurTexture46;
	SharedTexture m_pCurTexture47;
	SharedTexture m_pCurTexture48;
	SharedTexture m_pCurTexture49;
	SharedTexture m_pCurTexture50;
	SharedTexture m_pCurTexture51;
	SharedTexture m_pCurTexture52;
	SharedTexture m_pCurTexture53;
	SharedTexture m_pCurTexture54;
	SharedTexture m_pCurTexture55;
	SharedTexture m_pCurTexture56;
	SharedTexture m_pCurTexture57;
	SharedTexture m_pCurTexture58;
	SharedTexture m_pCurTexture59;
	SharedTexture m_pCurTexture60;
	SharedTexture m_pCurTexture61;
	SharedTexture m_pCurTexture62;
	SharedTexture m_pCurTexture63;

	
	float m_flPanicTime;
	bool m_bPanicColorChange;
	float m_flDisplayTime;	

};