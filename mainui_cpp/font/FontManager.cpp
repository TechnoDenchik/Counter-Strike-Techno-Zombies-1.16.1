/*
FontManager.cpp - font manager
Copyright (C) 2017 a1batross

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/
#include <locale.h>
#include <algorithm>
#include "FontManager.h"
#include "BaseMenu.h"
#include "Utils.h"

#include "BaseFontBackend.h"

#if defined(MAINUI_USE_FREETYPE)
#include "FreeTypeFont.h"
#elif defined(MAINUI_USE_STB)
#include "StbFont.h"
#elif defined(_WIN32)
#include "WinAPIFont.h"
#endif

#include "BitmapFont.h"

#if defined __ANDROID__ || defined CS16CLIENT
#define DEFAULT_MENUFONT "RobotoCondensed"
#define DEFAULT_CONFONT  "DroidSans"
#define DEFAULT_WEIGHT   1000
#else
#define DEFAULT_MENUFONT "Trebuchet MS"
#define DEFAULT_CONFONT  "Tahoma"
#define DEFAULT_WEIGHT   500
#endif

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

CFontManager g_FontMgr;

CFontManager::CFontManager() :
	m_flLastFontCreationTime(0.0f),
	m_iLastFontTall(0),
	m_hLastFont(0)
{
#ifdef MAINUI_USE_FREETYPE
	if (FT_Init_FreeType(&CFreeTypeFont::m_Library) != 0)
	{
		Con_DPrintf("CFontManager: Failed to initialize FreeType library\n");
	}
#endif
	m_Fonts.EnsureCapacity(8); // Start with more capacity
}

CFontManager::~CFontManager()
{
	DeleteAllFonts();
#ifdef MAINUI_USE_FREETYPE
	if (CFreeTypeFont::m_Library)
	{
		FT_Done_FreeType(CFreeTypeFont::m_Library);
		CFreeTypeFont::m_Library = NULL;
	}
#endif
}

bool CFontManager::ValidateFontHandle(HFont font) const
{
	if (font <= 0 || font > m_Fonts.Count())
		return false;

	CBaseFont* pFont = m_Fonts[font - 1];
	return (pFont != NULL);
}

void CFontManager::VidInit(void)
{
	static float prevScale = 0.0f;

	float scale = uiStatic.scaleY;

	// Only recreate fonts if scale changed significantly or first init
	if (!prevScale
#ifndef SCALE_FONTS // complete disables font re-rendering
		|| fabs(scale - prevScale) > 0.1f
#endif
		)
	{
		Con_DPrintf("CFontManager::VidInit: Recreating fonts (scale: %.2f, prev: %.2f)\n", scale, prevScale);

		DeleteAllFonts();

		// Create fonts with error checking
		uiStatic.hDefaultFont = CFontBuilder(DEFAULT_MENUFONT, UI_MED_CHAR_HEIGHT * scale, DEFAULT_WEIGHT)
			.SetHandleNum(QM_DEFAULTFONT)
			.Create();

		if (!ValidateFontHandle(uiStatic.hDefaultFont))
		{
			Con_DPrintf("CFontManager::VidInit: Failed to create default font!\n");
		}

		uiStatic.hSmallFont = CFontBuilder(DEFAULT_MENUFONT, UI_SMALL_CHAR_HEIGHT * scale, DEFAULT_WEIGHT)
			.SetHandleNum(QM_SMALLFONT)
			.Create();

		uiStatic.hBigFont = CFontBuilder(DEFAULT_MENUFONT, UI_BIG_CHAR_HEIGHT * scale, DEFAULT_WEIGHT)
			.SetHandleNum(QM_BIGFONT)
			.Create();

		uiStatic.hBoldFont = CFontBuilder(DEFAULT_MENUFONT, UI_MED_CHAR_HEIGHT * scale, 1000)
			.SetHandleNum(QM_BOLDFONT)
			.Create();

#ifdef MAINUI_RENDER_PICBUTTON_TEXT
		uiStatic.hLightBlur = CFontBuilder(DEFAULT_MENUFONT, UI_MED_CHAR_HEIGHT * scale, 1000)
			.SetHandleNum(QM_LIGHTBLUR)
			.SetBlurParams(2, 1.0f)
			.Create();

		uiStatic.hHeavyBlur = CFontBuilder(DEFAULT_MENUFONT, UI_MED_CHAR_HEIGHT * scale, 1000)
			.SetHandleNum(QM_HEAVYBLUR)
			.SetBlurParams(8, 1.75f)
			.Create();
#endif

		uiStatic.hConsoleFont = CFontBuilder(DEFAULT_CONFONT, UI_CONSOLE_CHAR_HEIGHT * scale, 500)
			.SetOutlineSize()
			.Create();

		prevScale = scale;

		Con_DPrintf("CFontManager::VidInit: Created %d fonts\n", m_Fonts.Count());
	}
}

void CFontManager::DeleteAllFonts()
{
	for (int i = 0; i < m_Fonts.Count(); i++)
	{
		if (m_Fonts[i])
		{
			delete m_Fonts[i];
			m_Fonts[i] = NULL;
		}
	}
	m_Fonts.RemoveAll();

	// Reset cache
	m_iLastFontTall = 0;
	m_hLastFont = 0;
}

void CFontManager::DeleteFont(HFont hFont)
{
	if (!ValidateFontHandle(hFont))
		return;

	CBaseFont* font = m_Fonts[hFont - 1];
	if (font)
	{
		// Update cache if deleting cached font
		if (hFont == m_hLastFont)
		{
			m_iLastFontTall = 0;
			m_hLastFont = 0;
		}

		delete font;
		m_Fonts[hFont - 1] = NULL;

		// Don't remove from array to preserve handle indices
		// Actual removal happens in DeleteAllFonts
	}
}

CBaseFont* CFontManager::GetIFontFromHandle(HFont font)
{
	if (!ValidateFontHandle(font))
		return NULL;

	return m_Fonts[font - 1];
}

int CFontManager::GetEllipsisWide(HFont font)
{
	CBaseFont* pFont = GetIFontFromHandle(font);
	return pFont ? pFont->GetEllipsisWide() : 0;
}

void CFontManager::GetCharABCWide(HFont font, int ch, int& a, int& b, int& c)
{
	CBaseFont* pFont = GetIFontFromHandle(font);
	if (pFont)
		pFont->GetCharABCWidths(ch, a, b, c);
	else
		a = b = c = 0;
}

int CFontManager::GetCharacterWidth(HFont font, int ch)
{
	int a, b, c;
	GetCharABCWide(font, ch, a, b, c);
	return a + b + c;
}

int CFontManager::GetCharacterWidthScaled(HFont font, int ch, int height)
{
	int width = GetCharacterWidth(font, ch);

#ifdef SCALE_FONTS
	int fontTall = GetFontTall(font);
	if (fontTall > 0 && height != fontTall)
	{
		width = (int)(width * ((float)height / (float)fontTall));
	}
#endif

	return width;
}

HFont CFontManager::GetFontByName(const char* name)
{
	if (!name || !name[0])
		return -1;

	for (int i = 0; i < m_Fonts.Count(); i++)
	{
		CBaseFont* font = m_Fonts[i];
		if (font && !stricmp(name, font->GetName()))
			return i + 1;
	}
	return -1;
}

int CFontManager::GetFontTall(HFont font)
{
	// Simple cache for common case
	if (font == m_hLastFont && m_iLastFontTall > 0)
		return m_iLastFontTall;

	CBaseFont* pFont = GetIFontFromHandle(font);
	if (pFont)
	{
		m_hLastFont = font;
		m_iLastFontTall = pFont->GetTall();
		return m_iLastFontTall;
	}

	return 0;
}

int CFontManager::GetFontAscent(HFont font)
{
	CBaseFont* pFont = GetIFontFromHandle(font);
	return pFont ? pFont->GetAscent() : 0;
}

bool CFontManager::GetFontUnderlined(HFont font)
{
	CBaseFont* pFont = GetIFontFromHandle(font);
	return pFont ? (pFont->GetFlags() & FONT_UNDERLINE) != 0 : false;
}

void CFontManager::GetTextSize(HFont fontHandle, const char* text, int* wide, int* tall, int size)
{
	if (wide) *wide = 0;
	if (tall) *tall = 0;

	CBaseFont* font = GetIFontFromHandle(fontHandle);
	if (!font || !text || !text[0])
		return;

	int fontTall = font->GetHeight();
	int _wide = 0, _tall = fontTall;
	int x = 0;
	int i = 0;
	const char* ch = text;

	EngFuncs::UtfProcessChar(0);

	while (*ch && (size < 0 || i < size))
	{
		// Skip colorcodes
		if (IsColorString(ch))
		{
			ch += 2;
			continue;
		}

		int uch = EngFuncs::UtfProcessChar((unsigned char)*ch);
		if (uch)
		{
			if (uch == '\n')
			{
				// Newline - advance height and reset x
				if (*(ch + 1) != '\0') // Don't add height for trailing newline
				{
					_tall += fontTall;
				}
				x = 0;
			}
			else
			{
				int a, b, c;
				font->GetCharABCWidths(uch, a, b, c);
				x += a + b + c;
				_wide = Q_max(_wide, x);
			}
		}
		i++;
		ch++;
	}

	EngFuncs::UtfProcessChar(0);

	if (tall) *tall = _tall;
	if (wide) *wide = _wide;
}

int CFontManager::CutText(HFont fontHandle, const char* text, int height, int visibleSize, bool reverse,
	bool stopAtWhitespace, int* width, bool* remaining)
{
	// Initialize output parameters
	if (remaining) *remaining = false;
	if (width) *width = 0;

	CBaseFont* font = GetIFontFromHandle(fontHandle);
	if (!font || !text || !text[0] || visibleSize <= 0)
		return 0;

#ifdef SCALE_FONTS
	if (height > 0)
	{
		visibleSize = (int)((float)visibleSize / (float)height * (float)font->GetTall());
	}
#endif

	int _wide = 0;
	int whiteSpacePos = 0;
	const char* ch = text;
	const char* lastWhiteSpace = NULL;
	int lastWhiteSpaceWidth = 0;

	EngFuncs::UtfProcessChar(0);

	if (!reverse)
	{
		// Forward text cutting
		while (*ch)
		{
			// Skip colorcodes
			if (IsColorString(ch))
			{
				ch += 2;
				continue;
			}

			int uch = EngFuncs::UtfProcessChar((unsigned char)*ch);
			if (!uch)
			{
				ch++;
				continue;
			}

			if (uch == '\n')
				break; // Stop at newlines for single-line text

			int a, b, c;
			font->GetCharABCWidths(uch, a, b, c);
			int charWidth = a + b + c;

			// Check if adding this character would exceed visible size
			if (_wide + charWidth > visibleSize)
				break;

			// Track last whitespace for word wrapping
			if (stopAtWhitespace && (uch == ' ' || uch == '\t'))
			{
				lastWhiteSpace = ch;
				lastWhiteSpaceWidth = _wide;
			}

			_wide += charWidth;
			ch++;
		}

		EngFuncs::UtfProcessChar(0);

		if (remaining && *ch) *remaining = true;
		if (width) *width = _wide;

		// Prefer breaking at whitespace if requested
		if (stopAtWhitespace && lastWhiteSpace && lastWhiteSpace != ch - 1)
		{
			if (width) *width = lastWhiteSpaceWidth;
			return lastWhiteSpace - text + 1; // Include the whitespace
		}

		return ch - text;
	}
	else
	{
		// Reverse text cutting - calculate total width first
		const char* end = text;
		int totalWidth = 0;

		while (*end)
		{
			if (IsColorString(end))
			{
				end += 2;
				continue;
			}

			int uch = EngFuncs::UtfProcessChar((unsigned char)*end);
			if (uch && uch != '\n')
			{
				int a, b, c;
				font->GetCharABCWidths(uch, a, b, c);
				totalWidth += a + b + c;

				if (stopAtWhitespace && (uch == ' ' || uch == '\t'))
				{
					lastWhiteSpace = end;
					lastWhiteSpaceWidth = totalWidth;
				}
			}
			end++;
		}

		EngFuncs::UtfProcessChar(0);

		if (totalWidth <= visibleSize)
		{
			if (width) *width = totalWidth;
			return 0; // No need to cut from start
		}

		// Remove characters from start until it fits
		_wide = totalWidth;
		ch = text;

		while (*ch && _wide > visibleSize)
		{
			if (IsColorString(ch))
			{
				ch += 2;
				continue;
			}

			int uch = EngFuncs::UtfProcessChar((unsigned char)*ch);
			if (uch && uch != '\n')
			{
				int a, b, c;
				font->GetCharABCWidths(uch, a, b, c);
				_wide -= a + b + c;
			}
			ch++;
		}

		EngFuncs::UtfProcessChar(0);

		if (remaining) *remaining = true;
		if (width) *width = _wide;

		// Prefer breaking at whitespace if requested
		if (stopAtWhitespace && lastWhiteSpace && lastWhiteSpace >= ch)
		{
			if (width) *width = totalWidth - lastWhiteSpaceWidth;
			return lastWhiteSpace - text + 1;
		}

		return ch - text;
	}
}

int CFontManager::GetTextWide(HFont font, const char* text, int size)
{
	int wide = 0;
	GetTextSize(font, text, &wide, NULL, size);
	return wide;
}

int CFontManager::GetTextHeight(HFont fontHandle, const char* text, int size)
{
	CBaseFont* font = GetIFontFromHandle(fontHandle);
	if (!font || !text || !text[0])
		return 0;

	int height = font->GetHeight();
	int lines = 1;
	int i = 0;

	// Count newlines
	while (*text && (size < 0 || i < size))
	{
		if (*text == '\n')
			lines++;
		text++;
		i++;
	}

	return height * lines;
}

int CFontManager::GetTextHeightExt(HFont fontHandle, const char* text, int height, int visibleWidth, int size)
{
	if (!text || !text[0] || visibleWidth <= 0)
		return 0;

	CBaseFont* font = GetIFontFromHandle(fontHandle);
	if (!font)
		return 0;

	const char* textPos = text;
	int totalHeight = 0;
	int processedChars = 0;

	while (*textPos && (size < 0 || processedChars < size))
	{
		int charsThisLine = CutText(fontHandle, textPos, height, visibleWidth, false, true);
		if (charsThisLine == 0)
			break;

		totalHeight += height;
		textPos += charsThisLine;
		processedChars += charsThisLine;
	}

	return totalHeight;
}

int CFontManager::GetTextWideScaled(HFont font, const char* text, const int height, int size)
{
	CBaseFont* pFont = GetIFontFromHandle(font);
	if (!pFont)
		return 0;

	int textWidth = GetTextWide(font, text, size);

#ifdef SCALE_FONTS
	int fontTall = pFont->GetTall();
	if (fontTall > 0 && height != fontTall)
	{
		textWidth = (int)(textWidth * ((float)height / (float)fontTall));
	}
#endif

	return textWidth;
}

void CFontManager::UploadTextureForFont(CBaseFont* font)
{
	if (!font)
		return;

	// Upload only needed character ranges for performance
	charRange_t range[] =
	{
		{ 33, 126 },        // ASCII printable range
		{ 0x0400, 0x045F }, // Cyrillic range
		// Add more ranges as needed for specific languages
	};

	font->UploadGlyphsForRanges(range, ARRAYSIZE(range));
}

int CFontManager::DrawCharacter(HFont fontHandle, int ch, Point pt, int charH, const unsigned int color, bool forceAdditive)
{
	CBaseFont* font = GetIFontFromHandle(fontHandle);
	return font ? font->DrawCharacter(ch, pt, charH, color, forceAdditive) : 0;
}

void CFontManager::DebugDraw(HFont fontHandle)
{
	CBaseFont* font = GetIFontFromHandle(fontHandle);
	if (font)
		font->DebugDraw();
}

HFont CFontBuilder::Create()
{
	CBaseFont* font = NULL;

	// Check for existing font first (font deduplication)
	if (m_hForceHandle == -1) // Only deduplicate if not forcing a handle
	{
		for (int i = 0; i < g_FontMgr.m_Fonts.Count(); i++)
		{
			CBaseFont* existingFont = g_FontMgr.m_Fonts[i];
			if (existingFont && existingFont->IsEqualTo(m_szName, m_iTall, m_iWeight, m_iBlur, m_iFlags))
			{
				Con_DPrintf("CFontBuilder: Reusing existing font %s (%dpx)\n", m_szName, m_iTall);
				return i + 1;
			}
		}
	}

	// Create appropriate font backend based on configuration
#if defined(MAINUI_USE_FREETYPE)
	font = new CFreeTypeFont();
#elif defined(MAINUI_USE_STB)
	font = new CStbFont();
#elif defined(_WIN32) && defined(MAINUI_USE_CUSTOM_FONT_RENDER)
	font = new CWinAPIFont();
#else
	font = new CBitmapFont();
#endif

	if (!font)
	{
		Con_DPrintf("CFontBuilder: Failed to allocate font object for %s\n", m_szName);
		return -1;
	}

	double startTime = Sys_DoubleTime();

	bool created = font->Create(m_szName, m_iTall, m_iWeight, m_iBlur, m_fBrighten,
		m_iOutlineSize, m_iScanlineOffset, m_fScanlineScale, m_iFlags);

	if (!created)
	{
		Con_DPrintf("CFontBuilder: Primary font backend failed for %s, falling back to bitmap font\n", m_szName);
		delete font;

		// Fallback to bitmap font
		font = new CBitmapFont();
		if (!font->Create("Bitmap Font", m_iTall, m_iWeight, m_iBlur, m_fBrighten,
			m_iOutlineSize, m_iScanlineOffset, m_fScanlineScale, m_iFlags))
		{
			Con_DPrintf("CFontBuilder: Bitmap font fallback also failed!\n");
			delete font;
			return -1;
		}
	}

	// Upload glyphs to texture
	g_FontMgr.UploadTextureForFont(font);

	double endTime = Sys_DoubleTime();
	g_FontMgr.m_flLastFontCreationTime = (float)(endTime - startTime);

	Con_DPrintf("CFontBuilder: Rendered %s (%dpx, %d weight) in %.3f seconds\n",
		font->GetName(), m_iTall, m_iWeight, g_FontMgr.m_flLastFontCreationTime);

	// Insert font at specific handle or add to tail
	HFont resultHandle;
	if (m_hForceHandle != -1 && g_FontMgr.m_Fonts.IsValidIndex(m_hForceHandle))
	{
		// Replace existing font at forced handle
		if (g_FontMgr.m_Fonts[m_hForceHandle])
		{
			delete g_FontMgr.m_Fonts[m_hForceHandle];
		}
		g_FontMgr.m_Fonts[m_hForceHandle] = font;
		resultHandle = m_hForceHandle + 1;
	}
	else
	{
		// Add to end of list
		resultHandle = g_FontMgr.m_Fonts.AddToTail(font) + 1;
	}

	return resultHandle;
}