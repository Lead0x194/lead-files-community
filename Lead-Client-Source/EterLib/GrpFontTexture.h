#pragma once

#include "GrpTexture.h"
#include "GrpImageTexture.h"
#include "GrpDIB.h"

#include <vector>
#include <map>

class CGraphicFontTexture : public CGraphicTexture
{
public:
	typedef struct SCharacterInfomation
	{
		short index;
		short width;
		short height;
		float left;
		float top;
		float right;
		float bottom;
		float advance;
		float inkOffset;
	} TCharacterInfomation;

	typedef std::vector<TCharacterInfomation*>		TPCharacterInfomationVector;

public:
	CGraphicFontTexture();
	virtual ~CGraphicFontTexture();

	void Destroy();
	bool Create(const char* c_szFontName, int fontSize, bool bItalic);

	bool CreateDeviceObjects();
	void DestroyDeviceObjects();

	bool CheckTextureIndex(DWORD dwTexture);
	CGraphicImageTexture* GetTexture(DWORD dwTexture);
	void SelectTexture(DWORD dwTexture);

	bool UpdateTexture();

	TCharacterInfomation* GetCharacterInfomation(wchar_t keyValue);
	TCharacterInfomation* UpdateCharacterInfomation(wchar_t code);

	bool IsEmpty() const;

protected:
	void Initialize();

	bool AppendTexture();

	HFONT CreateFontHandle();

protected:		
	typedef std::vector<CGraphicImageTexture*>				TGraphicImageTexturePointerVector;
	typedef std::map<wchar_t, TCharacterInfomation>			TCharacterInfomationMap;

protected:		
	CGraphicDib	m_dib;

	HFONT	m_hFontOld;
	HFONT	m_hFont;

	TGraphicImageTexturePointerVector m_pFontTextureVector;

	TCharacterInfomationMap m_charInfoMap;

	int m_x;
	int m_y;
	int m_step;		
	bool m_isDirty;

	TCHAR	m_fontName[LF_FACESIZE];
	LONG	m_fontSize;
	bool	m_bItalic;
};
