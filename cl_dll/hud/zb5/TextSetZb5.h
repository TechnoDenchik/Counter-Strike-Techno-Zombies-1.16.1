/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#pragma once

#include "hud_sub.h"
#include "r_texture.h"

class CHudTextZB5 : public IBaseHudSub
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
	SharedTexture m_pCurTexture;
	SharedTexture stringtext;
	float m_flDisplayTime;
};

class CHudText2ZB5 : public IBaseHudSub
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

class CHudTextNum : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void evolution ( int evovalue)
	{
		evo = evovalue;
	}

	duration_t tNextsecond;
	time_point_t timesecond;
	duration_t tDeltasecond;

	time_point_t timetx;

protected:

	int evo;

	UniqueTexture levelhumanbg;
	UniqueTexture levelzombiebg;

	UniqueTexture levelhuman;
	UniqueTexture levelzombie;

	UniqueTexture leveliconh;
	UniqueTexture leveliconz;

	UniqueTexture iconlvh;
	UniqueTexture iconlvz;

	UniqueTexture buttonvh;
	UniqueTexture buttonvz;

	UniqueTexture iconmaxlvh;
	UniqueTexture iconmaxlvz;

	UniqueTexture iconxh;
	UniqueTexture iconxz;

	

	UniqueTexture texnumsmallh;
	UniqueTexture texnumsmallz;

	UniqueTexture texhealthsmallh;

	wrect_t m_rctexnumsmallh[10];
	wrect_t m_rctexnumsmallz[10];

	wrect_t m_rctexhealthsmallh[10];

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

	SharedTexture m_pCurTexture;
	SharedTexture stringtext;
	float m_flDisplayTime;
};



class CHudKillZB5 : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	
	bool go;
	void getgo();
	
	duration_t tNextsecond2;
	time_point_t timesecond2;
	duration_t tDeltasecond2;

	void ikills(int killz)
	{
		kill = killz;
	}

protected:

	int kill;

	time_point_t timetx2;

	UniqueTexture texnumbigh;
	UniqueTexture texnumbigz;

	wrect_t m_rctexnumbigh[10];
	wrect_t m_rctexnumbigz[10];

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

	SharedTexture stringtextz1;
	SharedTexture stringtextz2;
	SharedTexture stringtextz3;
	SharedTexture stringtextz4;
	SharedTexture stringtextz5;
	SharedTexture stringtextz6;
	SharedTexture stringtextz7;
	SharedTexture stringtextz8;
	SharedTexture stringtextz9;
	SharedTexture stringtextz10;
	SharedTexture stringtextz11;
	SharedTexture stringtextz12;
	SharedTexture stringtextz13;
	SharedTexture stringtextz14;
	SharedTexture stringtextz15;
	SharedTexture stringtextz16;
	SharedTexture stringtextz17;
	SharedTexture stringtextz18;
	SharedTexture stringtextz19;
	SharedTexture stringtextz20;
	SharedTexture stringtextz21;
	SharedTexture stringtextz22;
	SharedTexture stringtextz23;
	SharedTexture stringtextz24;
	SharedTexture stringtextz25;
	SharedTexture stringtextz26;
	SharedTexture stringtextz27;
	SharedTexture stringtextz28;
	SharedTexture stringtextz29;
	SharedTexture stringtextz30;
	SharedTexture stringtextz31;
	SharedTexture stringtextz32;

	float m_flDisplayTime;
};