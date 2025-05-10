/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#pragma once

#include "hud_sub.h"
#include "r_texture.h"

class CHudWebm : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void Settext();

	void renaining(int iCountDown)
	{
		times = iCountDown;
	}

protected:
	int times;
	int names;

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

	SharedTexture stringtext1;
	SharedTexture stringtext2;
	SharedTexture stringtext3;
	SharedTexture stringtext4;
	SharedTexture stringtext5;
	SharedTexture stringtext6;
	SharedTexture stringtext7;
	SharedTexture stringtext8;
	SharedTexture stringtext9;
	SharedTexture stringtext10;
	SharedTexture stringtext11;
	SharedTexture stringtext12;
	SharedTexture stringtext13;
	SharedTexture stringtext14;
	SharedTexture stringtext15;
	SharedTexture stringtext16;
	SharedTexture stringtext17;
	SharedTexture stringtext18;
	SharedTexture stringtext19;
	SharedTexture stringtext20;
	SharedTexture stringtext21;
	SharedTexture stringtext22;
	SharedTexture stringtext23;
	SharedTexture stringtext24;
	SharedTexture stringtext25;
	SharedTexture stringtext26;
	SharedTexture stringtext27;
	SharedTexture stringtext28;
	SharedTexture stringtext29;
	SharedTexture stringtext30;
	SharedTexture stringtext31;
	SharedTexture stringtext32;
	SharedTexture stringtext33;
	SharedTexture stringtext34;
	SharedTexture stringtext35;
	SharedTexture stringtext36;
	SharedTexture stringtext37;
	SharedTexture stringtext38;
	SharedTexture stringtext39;
	SharedTexture stringtext40;
	SharedTexture stringtext41;
	SharedTexture stringtext42;
	SharedTexture stringtext43;
	SharedTexture stringtext44;
	SharedTexture stringtext45;
	SharedTexture stringtext46;
	SharedTexture stringtext47;
	SharedTexture stringtext48;
	SharedTexture stringtext49;
	SharedTexture stringtext50;
	SharedTexture stringtext51;
	SharedTexture stringtext52;
	SharedTexture stringtext53;
	SharedTexture stringtext54;
	SharedTexture stringtext55;
	SharedTexture stringtext56;
	SharedTexture stringtext57;
	SharedTexture stringtext58;
	SharedTexture stringtext59;
	SharedTexture stringtext60;
	SharedTexture stringtext61;
	SharedTexture stringtext62;
	SharedTexture stringtext63;
	float m_flDisplayTime;
};

class CHudWebm2 : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void Settext();

protected:
	SharedTexture m_pCurTexture;
	SharedTexture stringtext;
	float m_flDisplayTime;
};