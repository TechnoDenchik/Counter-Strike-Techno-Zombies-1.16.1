#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"
#include "TextOrig.h"

#include "string.h"
#include "assert.h"
#include <numeric>
#include <tuple>

inline void DrawTexturePart(const CTextureRef& tex, const wrect_t& rect, int x1, int y1, float scale = 1.0f)
{
	tex.Bind();

	float w = tex.w();
	float h = tex.h();

	x1 *= gHUD.m_flScale;
	y1 *= gHUD.m_flScale;
	scale *= gHUD.m_flScale;

	int x2 = x1 + (rect.right - rect.left) * scale;
	int y2 = y1 + (rect.bottom - rect.top) * scale;

	gEngfuncs.pTriAPI->Begin(TRI_QUADS);
	gEngfuncs.pTriAPI->TexCoord2f(rect.left / w, rect.top / h);
	gEngfuncs.pTriAPI->Vertex3f(x1, y1, 0);
	gEngfuncs.pTriAPI->TexCoord2f(rect.left / w, rect.bottom / h);
	gEngfuncs.pTriAPI->Vertex3f(x1, y2, 0);
	gEngfuncs.pTriAPI->TexCoord2f(rect.right / w, rect.bottom / h);
	gEngfuncs.pTriAPI->Vertex3f(x2, y2, 0);
	gEngfuncs.pTriAPI->TexCoord2f(rect.right / w, rect.top / h);
	gEngfuncs.pTriAPI->Vertex3f(x2, y1, 0);
	gEngfuncs.pTriAPI->End();
}

int CHudGameStart::VidInit(void)
{
	if (!stringtext)
		stringtext = R_LoadTextureShared("resource/hud/zb3/hud_string_bg", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	return 1;
}

int CHudGameStart::Draw(float time)
{
	if (!m_pCurTexture)
		return 1;

	if (time > m_flDisplayTime + 3.00f)
	{
		m_pCurTexture = nullptr;
		return 1;
	}

	int x = ScreenWidth / 1.995;
	int y = ScreenHeight / 1.4;
	int y2 = ScreenHeight / 1.4;

	const float flScale = 0.0f;
	const int r = 153, g = 97, b = 7;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255 * std::min(5.0f - (time - m_flDisplayTime), 1.0f));

	stringtext->Bind();
	DrawUtils::Draw2DQuadScaled(x - 500 / 2, y - 38, x + 500 / 2, y - 8);

	char szbuffer[64];
	sprintf(szbuffer, "Игра началась!");

	DrawUtils::DrawHudString(x - 50, y2 - 32, ScreenWidth, szbuffer, r, g, b, flScale);

	return 1;
}

void CHudGameStart::Settext()
{
	m_pCurTexture = stringtext;
	m_flDisplayTime = gHUD.m_flTime;
}

int CHudBombPlant::VidInit(void)
{
	if (!stringtext)
		stringtext = R_LoadTextureShared("resource/hud/zb3/hud_string_bg", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	return 1;
}

int CHudBombPlant::Draw(float time)
{
	if (!m_pCurTexture)
		return 1;

	if (time > m_flDisplayTime + 3.00f)
	{
		m_pCurTexture = nullptr;
		return 1;
	}

	int x = ScreenWidth / 1.995;
	int y = ScreenHeight / 1.4;
	int y2 = ScreenHeight / 1.4;

	const float flScale = 0.0f;
	const int r = 153, g = 97, b = 7;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255 * std::min(5.0f - (time - m_flDisplayTime), 1.0f));

	stringtext->Bind();
	DrawUtils::Draw2DQuadScaled(x - 500 / 2, y - 38, x + 500 / 2, y - 8);

	char szbuffer[64];
	sprintf(szbuffer, "Бомба установлена");

	DrawUtils::DrawHudString(x - 50, y2 - 32, ScreenWidth, szbuffer, r, g, b, flScale);

	return 1;
}

void CHudBombPlant::Settext()
{
	m_pCurTexture = stringtext;
	m_flDisplayTime = gHUD.m_flTime;
}

int CHudBombDefuse::VidInit(void)
{
	if (!stringtext)
		stringtext = R_LoadTextureShared("resource/hud/zb3/hud_string_bg", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	return 1;
}

int CHudBombDefuse::Draw(float time)
{
	if (!m_pCurTexture)
		return 1;

	if (time > m_flDisplayTime + 3.00f)
	{
		m_pCurTexture = nullptr;
		return 1;
	}

	int x = ScreenWidth / 1.995;
	int y = ScreenHeight / 1.4;
	int y2 = ScreenHeight / 1.4;

	const float flScale = 0.0f;
	const int r = 153, g = 97, b = 7;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255 * std::min(5.0f - (time - m_flDisplayTime), 1.0f));

	stringtext->Bind();
	DrawUtils::Draw2DQuadScaled(x - 500 / 2, y - 38, x + 500 / 2, y - 8);

	char szbuffer[64];
	sprintf(szbuffer, "Бомба обезврежена");

	DrawUtils::DrawHudString(x - 50, y2 - 32, ScreenWidth, szbuffer, r, g, b, flScale);

	return 1;
}

void CHudBombDefuse::Settext()
{
	m_pCurTexture = stringtext;
	m_flDisplayTime = gHUD.m_flTime;
}

int CHudRoundDraw::VidInit(void)
{
	if (!stringtext)
		stringtext = R_LoadTextureShared("resource/hud/zb3/hud_string_bg", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	return 1;
}

int CHudRoundDraw::Draw(float time)
{
	if (!m_pCurTexture)
		return 1;

	if (time > m_flDisplayTime + 3.00f)
	{
		m_pCurTexture = nullptr;
		return 1;
	}

	int x = ScreenWidth / 1.995;
	int y = ScreenHeight / 1.4;
	int y2 = ScreenHeight / 1.4;

	const float flScale = 0.0f;
	const int r = 153, g = 97, b = 7;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255 * std::min(5.0f - (time - m_flDisplayTime), 1.0f));

	stringtext->Bind();
	DrawUtils::Draw2DQuadScaled(x - 500 / 2, y - 38, x + 500 / 2, y - 8);

	char szbuffer[64];
	sprintf(szbuffer, "Раунд закончился ничьёй");

	DrawUtils::DrawHudString(x - 50, y2 - 32, ScreenWidth, szbuffer, r, g, b, flScale);

	return 1;
}

void CHudRoundDraw::Settext()
{
	m_pCurTexture = stringtext;
	m_flDisplayTime = gHUD.m_flTime;
}

int CHudBombnozone::VidInit(void)
{
	if (!stringtext)
		stringtext = R_LoadTextureShared("resource/hud/zb3/hud_string_bg", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	return 1;
}

int CHudBombnozone::Draw(float time)
{
	if (!m_pCurTexture)
		return 1;

	if (time > m_flDisplayTime + 2.0f)
	{
		m_pCurTexture = nullptr;
		return 1;
	}

	int x = ScreenWidth / 1.995;
	int y = ScreenHeight / 1.4;
	int y2 = ScreenHeight / 1.4;

	const float flScale = 0.0f;
	const int r = 153, g = 97, b = 7;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255 * std::min(5.0f - (time - m_flDisplayTime), 1.0f));

	stringtext->Bind();
	DrawUtils::Draw2DQuadScaled(x - 500 / 2, y - 38, x + 500 / 2, y - 8);

	char szbuffer[64];
	sprintf(szbuffer, "Бомба должна находится в зоне закладки");

	DrawUtils::DrawHudString(x - 140, y2 - 32, ScreenWidth, szbuffer, r, g, b, flScale);

	return 1;
}

void CHudBombnozone::Settext()
{
	m_pCurTexture = stringtext;
	m_flDisplayTime = gHUD.m_flTime;
}

int CHudBombnoground::VidInit(void)
{
	if (!stringtext)
		stringtext = R_LoadTextureShared("resource/hud/zb3/hud_string_bg", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	return 1;
}

int CHudBombnoground::Draw(float time)
{
	if (!m_pCurTexture)
		return 1;

	if (time > m_flDisplayTime + 2.0f)
	{
		m_pCurTexture = nullptr;
		return 1;
	}

	int x = ScreenWidth / 1.995;
	int y = ScreenHeight / 1.4;
	int y2 = ScreenHeight / 1.4;

	const float flScale = 0.0f;
	const int r = 153, g = 97, b = 7;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255 * std::min(5.0f - (time - m_flDisplayTime), 1.0f));

	stringtext->Bind();
	DrawUtils::Draw2DQuadScaled(x - 500 / 2, y - 38, x + 500 / 2, y - 8);

	char szbuffer[64];
	sprintf(szbuffer, "Бомба должна находится на земле");

	DrawUtils::DrawHudString(x - 130, y2 - 32, ScreenWidth, szbuffer, r, g, b, flScale);

	return 1;
}

void CHudBombnoground::Settext()
{
	m_pCurTexture = stringtext;
	m_flDisplayTime = gHUD.m_flTime;
}

int CHudBombgive::VidInit(void)
{
	if (!stringtext)
		stringtext = R_LoadTextureShared("resource/hud/zb3/hud_string_bg", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	return 1;
}

int CHudBombgive::Draw(float time)
{
	if (!m_pCurTexture)
		return 1;

	if (time > m_flDisplayTime + 2.0f)
	{
		m_pCurTexture = nullptr;
		return 1;
	}

	int x = ScreenWidth / 1.995;
	int y = ScreenHeight / 1.4;
	int y2 = ScreenHeight / 1.4;

	const float flScale = 0.0f;
	const int r = 153, g = 97, b = 7;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255 * std::min(5.0f - (time - m_flDisplayTime), 1.0f));

	stringtext->Bind();
	DrawUtils::Draw2DQuadScaled(x - 500 / 2, y - 38, x + 500 / 2, y - 8);

	char szbuffer[64];
	sprintf(szbuffer, "Вы подобрали бомбу");

	DrawUtils::DrawHudString(x - 65, y2 - 32, ScreenWidth, szbuffer, r, g, b, flScale);

	return 1;
}

void CHudBombgive::Settext()
{
	m_pCurTexture = stringtext;
	m_flDisplayTime = gHUD.m_flTime;
}

int CHudBombdrop::VidInit(void)
{
	if (!stringtext)
		stringtext = R_LoadTextureShared("resource/hud/zb3/hud_string_bg", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	return 1;
}

int CHudBombdrop::Draw(float time)
{
	if (!m_pCurTexture)
		return 1;

	if (time > m_flDisplayTime + 2.0f)
	{
		m_pCurTexture = nullptr;
		return 1;
	}

	int x = ScreenWidth / 1.995;
	int y = ScreenHeight / 1.4;
	int y2 = ScreenHeight / 1.4;

	const float flScale = 0.0f;
	const int r = 153, g = 97, b = 7;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255 * std::min(5.0f - (time - m_flDisplayTime), 1.0f));

	stringtext->Bind();
	DrawUtils::Draw2DQuadScaled(x - 500 / 2, y - 38, x + 500 / 2, y - 8);

	char szbuffer[64];
	sprintf(szbuffer, "Вы выбросили бомбу");

	DrawUtils::DrawHudString(x - 65, y2 - 32, ScreenWidth, szbuffer, r, g, b, flScale);

	return 1;
}

void CHudBombdrop::Settext()
{
	m_pCurTexture = stringtext;
	m_flDisplayTime = gHUD.m_flTime;
}

int CHudWPNDrop::VidInit(void)
{
	if (!stringtext)
		stringtext = R_LoadTextureShared("resource/hud/zb3/hud_string_bg", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	return 1;
}

int CHudWPNDrop::Draw(float time)
{
	if (!m_pCurTexture)
		return 1;

	if (time > m_flDisplayTime + 2.0f)
	{
		m_pCurTexture = nullptr;
		return 1;
	}

	int x = ScreenWidth / 1.995;
	int y = ScreenHeight / 1.4;
	int y2 = ScreenHeight / 1.4;

	const float flScale = 0.0f;
	const int r = 153, g = 97, b = 7;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255 * std::min(5.0f - (time - m_flDisplayTime), 1.0f));

	stringtext->Bind();
	DrawUtils::Draw2DQuadScaled(x - 500 / 2, y - 38, x + 500 / 2, y - 8);

	char szbuffer[64];
	sprintf(szbuffer, "Это оружие нельзя выбрсить");

	DrawUtils::DrawHudString(x - 100, y2 - 32, ScreenWidth, szbuffer, r, g, b, flScale);

	return 1;
}

void CHudWPNDrop::Settext()
{
	m_pCurTexture = stringtext;
	m_flDisplayTime = gHUD.m_flTime;
}

int CHudAK47::VidInit(void)
{
	if (!stringtext)
		stringtext = R_LoadTextureShared("resource/hud/zb3/hud_string_bg", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	return 1;
}

int CHudAK47::Draw(float time)
{
	if (!m_pCurTexture)
		return 1;

	if (time > m_flDisplayTime + 0.0f)
	{
		m_pCurTexture = nullptr;
		return 1;
	}

	int x = ScreenWidth / 1.995;
	int y = ScreenHeight / 1.4;
	int y2 = ScreenHeight / 1.4;

	const float flScale = 0.0f;
	const int r = 153, g = 97, b = 7;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);

	stringtext->Bind();
	DrawUtils::Draw2DQuadScaled(x - 500 / 2, y - 38, x + 500 / 2, y - 8);

	char szbuffer[64];
	sprintf(szbuffer, "AK47");

	DrawUtils::DrawHudString(x - 100, y2 - 32, ScreenWidth, szbuffer, r, g, b, flScale);

	return 1;
}

void CHudAK47::Settext()
{
	m_pCurTexture = stringtext;
	m_flDisplayTime = gHUD.m_flTime;
}