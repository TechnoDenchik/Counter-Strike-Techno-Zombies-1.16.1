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

	void renaining(int evoskin)
	{
		skin = evoskin;
	}

	void SetName(int names)
	{
		m_iPlayerNum = names;
	}

	duration_t tNextsecond1;
	time_point_t timesecond1;
	duration_t tDeltasecond1;

	signed barstring = -150;

protected:
	int skin;
	int m_iPlayerNum = 0;
	int m_iPlayerNum2;
	bool m_pCurTexture;
	SharedTexture stringtext;
	SharedTexture stringtext2;
	SharedTexture stringtext3;
	SharedTexture stringtext4;
	float m_flDisplayTime;
};

class CHudWebm2 : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void Settext();

	duration_t tNextsecond1;
	time_point_t timesecond1;
	duration_t tDeltasecond1;

	signed barstring = 0;
	signed barstring2 = 774;
	signed barstring3 = -38;

protected:
	float m_flDisplayTime;

	bool m_startrenine;
	bool m_startadded;
	bool m_pCurTexture;

	SharedTexture stringtext;
	SharedTexture stringtext2;
};

class CHudWebm3 : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void Settext();

	void evotype(int evotype)
	{
		evoskin = evotype;
	}

	signed barstring = 0;
	signed barstring2 = 774;
	signed barstring3 = -38;

protected:
	int evoskin;
	float m_flDisplayTime;

	bool m_startrenine;
	bool m_startadded;
	bool m_pCurTexture;

	SharedTexture stringtext;

	SharedTexture zbtank;
	SharedTexture zbspeed;
	SharedTexture zbheavy;
	SharedTexture zbheal;
	SharedTexture zbpsycho;
	SharedTexture zbdeimos;
	SharedTexture zbganimed;
	SharedTexture zbbanchee;
	SharedTexture zbstamper;
	SharedTexture zbresident;
	SharedTexture zbboomer;
	SharedTexture zbbooster;
	SharedTexture zbflying;
	SharedTexture zbchina;
	SharedTexture zbaksha;
	SharedTexture zbpass;
	SharedTexture zbrevival;

	SharedTexture skilltank;
	SharedTexture skillspeed;
	SharedTexture skillheavy;
	SharedTexture skillheal;
	SharedTexture skillpsycho;
	SharedTexture skilldeimos;
	SharedTexture skillganimed;
	SharedTexture skillbanchee;
	SharedTexture skillstamper;
	SharedTexture skillresident;
	SharedTexture skillboomer;
	SharedTexture skillbooster;
	SharedTexture skillflyinng;
	SharedTexture skillflyinng2;
	SharedTexture skillchina;
	SharedTexture skillaksha;
	SharedTexture skillpass;
	SharedTexture skillrevival;

	SharedTexture bufftank;
	SharedTexture buffspeed;
	SharedTexture buffheavy;
	SharedTexture buffheal;
	SharedTexture buffpsycho;
	SharedTexture buffdeimos;
	SharedTexture buffganimed;
	SharedTexture buffbanchee;
	SharedTexture buffstamper;
	SharedTexture buffresident;
	SharedTexture buffboomer;
	SharedTexture buffbooster;
	SharedTexture buffflying;
	SharedTexture buffchina;
	SharedTexture buffaksha;
	SharedTexture buffpass;
	SharedTexture buffrevival;
};

class CHudWebm4 : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void Settext(int flstep, float flstep2);

	void SetResult(bool iresult)
	{
		m_pCurTexture = iresult;
	}

protected:
	float flprogress = 0.0f;
	float flprogress2;

	int m_flStepEvo;
	int step;

	bool m_pCurTexture;

	duration_t tNextsecond1;
	time_point_t timesecond1;
	duration_t tDeltasecond1;

	duration_t tNextsecond2;
	time_point_t timesecond2;
	duration_t tDeltasecond2;

	SharedTexture stringtext;
	SharedTexture stringtext2;

	UniqueTexture texnumsmallh;
	UniqueTexture texnumsmallz;

	wrect_t m_rctexnumsmallh[10];
	wrect_t m_rctexnumsmallz[10];
};