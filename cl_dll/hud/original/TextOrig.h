#pragma once

#include "hud_sub.h"
#include "r_texture.h"

class CHudGameStart : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void Settext();

protected:
	int names;
	SharedTexture m_pCurTexture;
	SharedTexture stringtext;
	float m_flDisplayTime;
};

class CHudBombPlant : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void Settext();

protected:
	int names;
	SharedTexture m_pCurTexture;
	SharedTexture stringtext;
	float m_flDisplayTime;
};

class CHudBombDefuse : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void Settext();

protected:
	int names;
	SharedTexture m_pCurTexture;
	SharedTexture stringtext;
	float m_flDisplayTime;
};

class CHudRoundDraw : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void Settext();

protected:
	int names;
	SharedTexture m_pCurTexture;
	SharedTexture stringtext;
	float m_flDisplayTime;
};

class CHudBombnozone : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void Settext();

protected:
	int names;
	SharedTexture m_pCurTexture;
	SharedTexture stringtext;
	float m_flDisplayTime;
};

class CHudBombnoground : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void Settext();

protected:
	int names;
	SharedTexture m_pCurTexture;
	SharedTexture stringtext;
	float m_flDisplayTime;
};

class CHudBombgive : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void Settext();

protected:
	int names;
	SharedTexture m_pCurTexture;
	SharedTexture stringtext;
	float m_flDisplayTime;
};

class CHudBombdrop : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void Settext();

protected:
	int names;
	SharedTexture m_pCurTexture;
	SharedTexture stringtext;
	float m_flDisplayTime;
};

class CHudWPNDrop : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void Settext();

protected:
	int names;
	SharedTexture m_pCurTexture;
	SharedTexture stringtext;
	float m_flDisplayTime;
};

class CHudAK47 : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void Settext();

protected:
	int names;
	SharedTexture m_pCurTexture;
	SharedTexture stringtext;
	float m_flDisplayTime;
};