/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#pragma once
#include <vector>

#include "r_texture.h"

class CHudInfoShelterIcon : public CHudBase
{
public:
	int Init(void) override;
	int VidInit(void) override;
	void Reset(void) override;
	int Draw(float time) override;
	void Shutdown(void) override;

public:
	int m_iBombTargetsNum;
	int g_iUser1;
	float m_flDisplayTime;
	bool getcommand;
	bool getcommand2;
	bool getcommand3;

	duration_t tNextAttackMen;
	time_point_t tWorldTimeMen;
	duration_t tDeltaMen;

	UniqueTexture m_pTexture_BombAB[2];
	UniqueTexture m_pTexture_Supplybox;
	UniqueTexture m_pTexture_woods;
	UniqueTexture m_icon_player[2];
	Vector m_vecBombTargets[2];
	Vector m_vecPlayerlocation[2];
};

class CHudInfoWoodIcon : public CHudBase
{
public:
	int Init(void) override;
	int VidInit(void) override;
	void Reset(void) override;
	int Draw(float time) override;
	void Shutdown(void) override;
	bool CheckForPlayer(cl_entity_s* pEnt);
public:
	int m_iBombTargetsNum;
	int g_iUser1;
	UniqueTexture m_pTexture_BombAB[2];
	UniqueTexture m_pTexture_Supplybox;
	UniqueTexture m_pTexture_woods;
	UniqueTexture m_icon_player[2];
	Vector m_vecBombTargets[2];
	Vector m_vecPlayerlocation[2];
};

class CHudInfoMetalIcon : public CHudBase
{
public:
	int Init(void) override;
	int VidInit(void) override;
	void Reset(void) override;
	int Draw(float time) override;
	void Shutdown(void) override;

public:
	int m_iBombTargetsNum;
	int g_iUser1;
	UniqueTexture m_pTexture_BombAB[2];
	UniqueTexture m_pTexture_Supplybox;
	UniqueTexture m_pTexture_woods;
	UniqueTexture m_icon_player[2];
	Vector m_vecBombTargets[2];
	Vector m_vecPlayerlocation[2];
};

class CHudInfoZombieIcon : public CHudBase
{
public:
	int Init(void) override;
	int VidInit(void) override;
	void Reset(void) override;
	int Draw(float time) override;
	void Shutdown(void) override;

public:
	int m_iBombTargetsNum;
	int g_iUser1;
	UniqueTexture m_pTexture_BombAB[2];
	UniqueTexture m_pTexture_Supplybox;
	UniqueTexture m_pTexture_woods;
	UniqueTexture m_icon_player[2];
	Vector m_vecBombTargets[2];
	Vector m_vecPlayerlocation[2];
};

class CHudInfoBuyZoneIcon : public CHudBase
{
public:
	int Init(void) override;
	int VidInit(void) override;
	void Reset(void) override;
	int Draw(float time) override;
	void Shutdown(void) override;

public:
	int m_iBombTargetsNum;
	int g_iUser1;
	UniqueTexture m_pTexture_BombAB[2];
	UniqueTexture m_pTexture_Supplybox;
	UniqueTexture m_pTexture_woods;
	UniqueTexture m_icon_player[2];
	Vector m_vecBombTargets[2];
	Vector m_vecPlayerlocation[2];
};