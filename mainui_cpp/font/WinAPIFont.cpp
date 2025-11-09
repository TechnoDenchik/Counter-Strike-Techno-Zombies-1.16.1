/*
WinAPIFont.cpp - Win32 Font backend
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

#if defined _WIN32 && defined(MAINUI_USE_CUSTOM_FONT_RENDER) && !defined(MAINUI_USE_STB)
#include <stdarg.h>
#include <malloc.h>
#include <limits>
#include <algorithm> // Добавить этот заголовок

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#include "FontManager.h"
#include "WinAPIFont.h"

bool ABCCacheLessFunc(const abc_t& a, const abc_t& b)
{
	return a.ch < b.ch;
}

CWinAPIFont::CWinAPIFont() :
	CBaseFont(),
	m_ABCCache(0, 0, ABCCacheLessFunc),
	m_hFont(NULL),
	m_hDC(NULL),
	m_hDIB(NULL),
	m_pBuf(NULL),
	m_bFound(false)
{
	m_rgiBitmapSize[0] = 0;
	m_rgiBitmapSize[1] = 0;
}

CWinAPIFont::~CWinAPIFont()
{
	ReleaseResources();
}

void CWinAPIFont::ReleaseResources()
{
	if (m_hFont)
	{
		::DeleteObject(m_hFont);
		m_hFont = NULL;
	}

	if (m_hDIB)
	{
		::DeleteObject(m_hDIB);
		m_hDIB = NULL;
	}

	if (m_hDC)
	{
		::DeleteDC(m_hDC);
		m_hDC = NULL;
	}

	m_pBuf = NULL;
}

bool CWinAPIFont::ValidateBufferAccess(int x, int y, const Size& sz) const
{
	return (x >= 0) && (x < sz.w) && (y >= 0) && (y < sz.h);
}

wchar_t CWinAPIFont::ConvertCharToWide(int ch) const
{
	// Handle basic ASCII and BMP Unicode characters
	if (ch >= 0 && ch <= 0xD7FF)
		return (wchar_t)ch;
	else if (ch >= 0xE000 && ch <= 0xFFFF)
		return (wchar_t)ch;
	else
		return L'?'; // Fallback for unsupported characters
}

int CALLBACK FontEnumProc(const LOGFONTA* lpelfe, const TEXTMETRICA* lpntme, DWORD FontType, LPARAM lpParam)
{
	CWinAPIFont* font = (CWinAPIFont*)lpParam;
	if (font)
	{
		font->m_bFound = true;
	}
	return 0; // Stop enumeration after first match
}

bool CWinAPIFont::Create(const char* name, int tall, int weight, int blur, float brighten,
	int outlineSize, int scanlineOffset, float scanlineScale, int flags)
{
	// Release any existing resources
	ReleaseResources();

	// Validate input parameters
	if (!name || name[0] == '\0')
	{
		Con_DPrintf("Invalid font name\n");
		return false;
	}

	if (tall <= 0 || tall > 256)
	{
		Con_DPrintf("Invalid font height: %d\n", tall);
		return false;
	}

	Q_strncpy(m_szName, name, sizeof(m_szName));
	m_szName[sizeof(m_szName) - 1] = '\0';

	m_iTall = tall + 6;
	m_iWeight = weight;
	m_iFlags = flags;
	m_iBlur = blur;
	m_fBrighten = brighten;
	m_iOutlineSize = outlineSize;
	m_iScanlineOffset = scanlineOffset;
	m_fScanlineScale = scanlineScale;

	// Create device context
	m_hDC = ::CreateCompatibleDC(NULL);
	if (!m_hDC)
	{
		DWORD error = ::GetLastError();
		Con_DPrintf("Couldn't create windows font %s: CreateCompatibleDC failed (error %d)\n", name, error);
		return false;
	}

	int charset = DEFAULT_CHARSET;
	m_bFound = false;

	// Check if font exists
	LOGFONTA font = { 0 };
	font.lfCharSet = DEFAULT_CHARSET;
	font.lfPitchAndFamily = 0;
	Q_strncpy(font.lfFaceName, m_szName, sizeof(font.lfFaceName));
	font.lfFaceName[sizeof(font.lfFaceName) - 1] = '\0';

	::EnumFontFamiliesExA(m_hDC, &font, &FontEnumProc, (LPARAM)this, 0);
	if (!m_bFound)
	{
		Con_DPrintf("Couldn't create windows font %s: no font found\n", name);
		ReleaseResources();
		return false;
	}

	// Create the font
	m_hFont = ::CreateFontA(
		m_iTall, 0, 0, 0, m_iWeight,
		(m_iFlags & FONT_ITALIC) ? TRUE : FALSE,
		(m_iFlags & FONT_UNDERLINE) ? TRUE : FALSE,
		(m_iFlags & FONT_STRIKEOUT) ? TRUE : FALSE,
		charset, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font.lfFaceName);

	if (!m_hFont)
	{
		DWORD error = ::GetLastError();
		Con_DPrintf("Couldn't create windows font %s: CreateFont failed (error %d)\n", name, error);
		ReleaseResources();
		return false;
	}

	// Set as active font
	::SetMapMode(m_hDC, MM_TEXT);
	HGDIOBJ oldFont = ::SelectObject(m_hDC, m_hFont);
	if (!oldFont)
	{
		Con_DPrintf("Couldn't select font into DC\n");
		ReleaseResources();
		return false;
	}

	::SetTextAlign(m_hDC, TA_LEFT | TA_TOP | TA_UPDATECP);

	// Get text metrics
	TEXTMETRIC tm = { 0 };
	if (!GetTextMetrics(m_hDC, &tm))
	{
		DWORD error = ::GetLastError();
		Con_DPrintf("Couldn't create windows font %s: GetTextMetrics failed (error %d)\n", name, error);
		ReleaseResources();
		return false;
	}

	m_iHeight = tm.tmHeight + 2 * m_iOutlineSize;
	m_iMaxCharWidth = tm.tmMaxCharWidth;
	m_iAscent = tm.tmAscent;

	m_rgiBitmapSize[0] = tm.tmMaxCharWidth + m_iOutlineSize * 2;
	m_rgiBitmapSize[1] = tm.tmHeight + m_iOutlineSize * 2;

	// Ensure reasonable bitmap size
	if (m_rgiBitmapSize[0] <= 0) m_rgiBitmapSize[0] = 32;
	if (m_rgiBitmapSize[1] <= 0) m_rgiBitmapSize[1] = 32;
	if (m_rgiBitmapSize[0] > 1024) m_rgiBitmapSize[0] = 1024;
	if (m_rgiBitmapSize[1] > 1024) m_rgiBitmapSize[1] = 1024;

	// Create DIB section for rendering
	BITMAPINFOHEADER header = { 0 };
	header.biSize = sizeof(header);
	header.biWidth = m_rgiBitmapSize[0];
	header.biHeight = -m_rgiBitmapSize[1]; // Top-down DIB
	header.biPlanes = 1;
	header.biBitCount = 32;
	header.biCompression = BI_RGB;
	header.biSizeImage = 0;

	m_hDIB = ::CreateDIBSection(m_hDC, (BITMAPINFO*)&header, DIB_RGB_COLORS, (void**)&m_pBuf, NULL, 0);
	if (!m_hDIB || !m_pBuf)
	{
		DWORD error = ::GetLastError();
		Con_DPrintf("Couldn't create DIB section for font %s (error %d)\n", name, error);
		ReleaseResources();
		return false;
	}

	HGDIOBJ oldBitmap = ::SelectObject(m_hDC, m_hDIB);
	if (!oldBitmap)
	{
		Con_DPrintf("Couldn't select DIB into DC\n");
		ReleaseResources();
		return false;
	}

	// Clear the bitmap to black
	::PatBlt(m_hDC, 0, 0, m_rgiBitmapSize[0], m_rgiBitmapSize[1], BLACKNESS);

	return true;
}

void CWinAPIFont::GetCharRGBA(int ch, Point pt, Size sz, unsigned char* rgba, Size& drawSize)
{
	// Validate inputs
	if (!rgba || sz.w <= 0 || sz.h <= 0 || !m_hDC || !m_hFont)
	{
		drawSize.w = 0;
		drawSize.h = 0;
		return;
	}

	// Set us up to render into our dib
	::SelectObject(m_hDC, m_hFont);

	int a, b, c;
	GetCharABCWidths(ch, a, b, c);

	int wide = b;
	if (m_iFlags & FONT_UNDERLINE)
	{
		wide += (a + c);
	}

	int tall = m_iHeight;
	GLYPHMETRICS glyphMetrics;
	MAT2 mat2 = { { 0, 1 }, { 0, 0 }, { 0, 0 }, { 0, 1 } };
	DWORD bytesNeeded = 0;

	// Try to get the glyph directly using GetGlyphOutline
	bytesNeeded = ::GetGlyphOutlineW(m_hDC, ConvertCharToWide(ch), GGO_GRAY8_BITMAP, &glyphMetrics, 0, NULL, &mat2);

	if (bytesNeeded > 0 && bytesNeeded < 1024 * 1024) // Reasonable size limit
	{
		// Allocate buffer for glyph data
		unsigned char* lpbuf = (unsigned char*)_alloca(bytesNeeded);
		if (!lpbuf)
		{
			// Fall back to bitmap method
			goto use_bitmap_method;
		}

		DWORD result = ::GetGlyphOutlineW(m_hDC, ConvertCharToWide(ch), GGO_GRAY8_BITMAP, &glyphMetrics, bytesNeeded, lpbuf, &mat2);
		if (result == GDI_ERROR)
		{
			goto use_bitmap_method;
		}

		// Rows are on DWORD boundaries
		int bufferWidth = glyphMetrics.gmBlackBoxX;
		while (bufferWidth % 4 != 0)
		{
			bufferWidth++;
		}

		// Calculate vertical positioning
		int pushDown = m_iAscent - glyphMetrics.gmptGlyphOrigin.y;

		// Calculate horizontal start position
		int xstart = 0;

		// Adjust start if antialiased bitmap is wider than character
		if ((int)glyphMetrics.gmBlackBoxX >= b + 2)
		{
			xstart = (glyphMetrics.gmBlackBoxX - b) / 2;
		}

		// Clear target area
		memset(rgba, 0, sz.w * sz.h * 4);

		// Copy glyph data to target buffer
		for (unsigned int j = 0; j < glyphMetrics.gmBlackBoxY; j++)
		{
			for (unsigned int i = xstart; i < glyphMetrics.gmBlackBoxX; i++)
			{
				int x = i - xstart + m_iBlur + m_iOutlineSize;
				int y = j + pushDown;

				if (ValidateBufferAccess(x, y, sz))
				{
					unsigned char grayscale = lpbuf[(j * bufferWidth + i)];

					float r, g, b, a;
					if (grayscale)
					{
						r = g = b = 1.0f;
						a = (grayscale + 0) / 64.0f;
						if (a > 1.0f) a = 1.0f;
					}
					else
					{
						r = g = b = 0.0f;
						a = 0.0f;
					}

					unsigned char* dst = &rgba[(y * sz.w + x) * 4];
					dst[0] = (unsigned char)(r * 255.0f);
					dst[1] = (unsigned char)(g * 255.0f);
					dst[2] = (unsigned char)(b * 255.0f);
					dst[3] = (unsigned char)(a * 255.0f);
				}
			}
		}

		drawSize.w = glyphMetrics.gmBlackBoxX - xstart + m_iOutlineSize * 2 + m_iBlur * 2;
		drawSize.h = glyphMetrics.gmBlackBoxY;
	}
	else
	{
	use_bitmap_method:
		// Use render-to-bitmap method
		::SetBkColor(m_hDC, RGB(0, 0, 0));
		::SetTextColor(m_hDC, RGB(255, 255, 255));
		::SetBkMode(m_hDC, OPAQUE);

		// Clear the bitmap
		::PatBlt(m_hDC, 0, 0, m_rgiBitmapSize[0], m_rgiBitmapSize[1], BLACKNESS);

		if (m_iFlags & FONT_UNDERLINE)
		{
			::MoveToEx(m_hDC, 0, 0, NULL);
		}
		else
		{
			::MoveToEx(m_hDC, -a, 0, NULL);
		}

		// Render the character
		wchar_t wch = ConvertCharToWide(ch);
		::ExtTextOutW(m_hDC, 0, 0, 0, NULL, &wch, 1, NULL);

		::SetBkMode(m_hDC, TRANSPARENT);

		// Clamp dimensions to valid ranges
		wide = std::min(wide, m_rgiBitmapSize[0]);
		tall = std::min(tall, m_rgiBitmapSize[1]);
		wide = std::min(wide, sz.w);
		tall = std::min(tall, sz.h);

		// Clear target area
		memset(rgba, 0, sz.w * sz.h * 4);

		// Copy from DIB to target buffer
		for (int j = (int)m_iOutlineSize; j < tall - (int)m_iOutlineSize; j++)
		{
			for (int i = (int)m_iOutlineSize; i < wide - (int)m_iOutlineSize; i++)
			{
				if (ValidateBufferAccess(i, j, sz))
				{
					unsigned char* src = &m_pBuf[(i + j * m_rgiBitmapSize[0]) * 4];
					unsigned char* dst = &rgba[(i + j * sz.w) * 4];

					// Don't draw anything for tab characters
					unsigned char r, g, b;
					if (ch == '\t')
					{
						r = g = b = 0;
					}
					else
					{
						r = src[0];
						g = src[1];
						b = src[2];
					}

					// Generate alpha based on luminance conversion
					dst[0] = r;
					dst[1] = g;
					dst[2] = b;
					dst[3] = (unsigned char)((float)r * 0.34f + (float)g * 0.55f + (float)b * 0.11f);
				}
			}
		}

		drawSize.w = wide + m_iOutlineSize + m_iBlur * 2;
		drawSize.h = tall;
	}

	// Apply post-processing effects
	ApplyBlur(sz, rgba);
	ApplyOutline(Point(0, 0), sz, rgba);
	ApplyScanline(sz, rgba);
	ApplyStrikeout(sz, rgba);
}

void CWinAPIFont::GetCharABCWidths(int ch, int& a, int& b, int& c)
{
	// Look for it in the cache
	abc_t finder = { ch };
	int i = m_ABCCache.Find(finder);
	if (m_ABCCache.IsValidIndex(i))
	{
		a = m_ABCCache[i].a;
		b = m_ABCCache[i].b;
		c = m_ABCCache[i].c;
		return;
	}

	// Not in cache, get from Windows
	::SelectObject(m_hDC, m_hFont);
	ABC abc;

	// Try Unicode first, then ANSI
	if (::GetCharABCWidthsW(m_hDC, ConvertCharToWide(ch), ConvertCharToWide(ch), &abc) ||
		::GetCharABCWidthsA(m_hDC, ch, ch, &abc))
	{
		finder.a = abc.abcA;
		finder.b = abc.abcB;
		finder.c = abc.abcC;
	}
	else
	{
		// Fallback: use max char width
		finder.a = 0;
		finder.b = m_iMaxCharWidth;
		finder.c = 0;
	}

	// Apply effects padding
	finder.a -= m_iBlur + m_iOutlineSize;
	finder.b += m_iBlur + m_iOutlineSize;

	// Adjust for outline
	if (m_iOutlineSize)
	{
		if (finder.a < 0)
			finder.a += m_iOutlineSize;

		if (finder.c < 0)
			finder.c += m_iOutlineSize;
	}

	// Ensure non-negative dimensions
	finder.a = max(finder.a, 0);
	finder.b = max(finder.b, 0);
	finder.c = max(finder.c, 0);

	a = finder.a;
	b = finder.b;
	c = finder.c;

	// Add to cache
	m_ABCCache.Insert(finder);
}

bool CWinAPIFont::HasChar(int ch) const
{
	if (!m_hDC || !m_hFont)
		return false;

	// Basic check - in real implementation you might want to use 
	// GetGlyphIndices or other methods to check character support
	return (ch >= 0 && ch <= 0xFFFF);
}

#endif // _WIN32 && MAINUI_USE_CUSTOM_FONT_RENDER && !MAINUI_USE_STB