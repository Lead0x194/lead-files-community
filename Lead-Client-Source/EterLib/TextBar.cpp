#include "StdAfx.h"
#include "TextBar.h"
#include "../eterlib/Util.h"

static std::wstring ToWideText(const char * c_szText)
{
	const int iTextLength = static_cast<int>(strlen(c_szText));
	const int iWideLength = MultiByteToWideChar(CP_UTF8, 0, c_szText, iTextLength, NULL, 0);

	std::wstring wstrText(iWideLength, L'\0');

	if (iWideLength > 0)
		MultiByteToWideChar(CP_UTF8, 0, c_szText, iTextLength, &wstrText[0], iWideLength);

	return wstrText;
}

void CTextBar::__SetFont(int fontSize, bool isBold)
{
	LOGFONT logFont;

	memset(&logFont, 0, sizeof(LOGFONT));

	logFont.lfHeight			= fontSize;
	logFont.lfEscapement		= 0;
	logFont.lfOrientation		= 0;

	if (isBold)
		logFont.lfWeight			= FW_BOLD;
	else
		logFont.lfWeight			= FW_NORMAL;
	
	logFont.lfItalic			= FALSE;
	logFont.lfUnderline			= FALSE;
	logFont.lfStrikeOut			= FALSE;
	logFont.lfCharSet			= DEFAULT_CHARSET;
	logFont.lfOutPrecision		= OUT_DEFAULT_PRECIS;
	logFont.lfClipPrecision		= CLIP_DEFAULT_PRECIS;
	logFont.lfQuality			= ANTIALIASED_QUALITY;
	logFont.lfPitchAndFamily	= DEFAULT_PITCH;
	strcpy_s(logFont.lfFaceName, sizeof(logFont.lfFaceName), GetDefaultFontFace());
	m_hFont = CreateFontIndirect(&logFont);

		
	HDC hdc = m_dib.GetDCHandle();
	m_hOldFont = (HFONT)SelectObject(hdc, m_hFont);
	
}

void CTextBar::SetTextColor(int r, int g, int b)
{
	HDC hDC = m_dib.GetDCHandle();
	::SetTextColor(hDC, RGB(r, g, b));
}
		
void CTextBar::GetTextExtent(const char * c_szText, SIZE* p_size)
{
	HDC hDC = m_dib.GetDCHandle();
	const std::wstring wstrText = ToWideText(c_szText);
	GetTextExtentPoint32W(hDC, wstrText.c_str(), static_cast<int>(wstrText.length()), p_size);
}

void CTextBar::TextOut(int ix, int iy, const char * c_szText)
{
	const std::wstring wstrText = ToWideText(c_szText);
	HDC hDC = m_dib.GetDCHandle();
	::SetBkColor(hDC, 0);
	::TextOutW(hDC, ix, iy, wstrText.c_str(), static_cast<int>(wstrText.length()));
	Invalidate();
}

void CTextBar::OnCreate()
{
	m_dib.SetBkMode(TRANSPARENT);

	__SetFont(m_fontSize, m_isBold);
}

CTextBar::CTextBar(int fontSize, bool isBold)
{
	m_hOldFont = NULL;	
	m_fontSize = fontSize;
	m_isBold = isBold;
	
}

CTextBar::~CTextBar()
{
	HDC hdc = m_dib.GetDCHandle();
	SelectObject(hdc, m_hOldFont);
}
