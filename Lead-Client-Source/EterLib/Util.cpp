#include "StdAfx.h"
#include "../eterPack/EterPackManager.h"

#include "TextFileLoader.h"

void PrintfTabs(FILE * File, int iTabCount, const char * c_szString, ...)
{
	va_list args;
	va_start(args, c_szString);

	static char szBuf[1024];
	_vsnprintf_s(szBuf, sizeof(szBuf), _TRUNCATE, c_szString, args);
	va_end(args);

	for (int i = 0; i < iTabCount; ++i)
		fprintf(File, "    ");

	fprintf(File, szBuf);
}

bool LoadTextData(const char * c_szFileName, CTokenMap & rstTokenMap)
{
	LPCVOID pMotionData;
	CMappedFile File;

	if (!CEterPackManager::Instance().Get(File, c_szFileName, &pMotionData))
		return false;

	CMemoryTextFileLoader textFileLoader;
	CTokenVector stTokenVector;

	textFileLoader.Bind(File.Size(), pMotionData);

	for (DWORD i = 0; i < textFileLoader.GetLineCount(); ++i)
	{
		if (!textFileLoader.SplitLine(i, &stTokenVector))
			continue;

		if (2 != stTokenVector.size())
			return false;

		stl_lowers(stTokenVector[0]);
		stl_lowers(stTokenVector[1]);

		rstTokenMap[stTokenVector[0]] = stTokenVector[1];
	}

	return true;
}

bool LoadMultipleTextData(const char * c_szFileName, CTokenVectorMap & rstTokenVectorMap)
{
	LPCVOID pModelData;
	CMappedFile File;

	if (!CEterPackManager::Instance().Get(File, c_szFileName, &pModelData))
		return false;

	DWORD i;

	CMemoryTextFileLoader textFileLoader;
	CTokenVector stTokenVector;

	textFileLoader.Bind(File.Size(), pModelData);

	for (i = 0; i < textFileLoader.GetLineCount(); ++i)
	{
		if (!textFileLoader.SplitLine(i, &stTokenVector))
			continue;

		stl_lowers(stTokenVector[0]);

		// Start or End
		if (0 == stTokenVector[0].compare("start"))
		{
			CTokenVector stSubTokenVector;

			stl_lowers(stTokenVector[1]);
			std::string key = stTokenVector[1];
			stTokenVector.clear();

			for (i=i+1; i < textFileLoader.GetLineCount(); ++i)
			{
				if (!textFileLoader.SplitLine(i, &stSubTokenVector))
					continue;

				stl_lowers(stSubTokenVector[0]);

				if (0 == stSubTokenVector[0].compare("end"))
				{
					break;
				}

				for (DWORD j = 0; j < stSubTokenVector.size(); ++j)
				{
					stTokenVector.push_back(stSubTokenVector[j]);
				}
			}

			rstTokenVectorMap.insert(CTokenVectorMap::value_type(key, stTokenVector));
		}
		else
		{
			std::string key = stTokenVector[0];
			stTokenVector.erase(stTokenVector.begin());
			rstTokenVectorMap.insert(CTokenVectorMap::value_type(key, stTokenVector));
		}
	}

	return true;
}

D3DXVECTOR3 TokenToVector(CTokenVector & rVector)
{
	if (3 != rVector.size())
	{
		assert(!"Size of token vector which will be converted to vector is not 3");
		return D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}

	return D3DXVECTOR3(static_cast<float>(atof(rVector[0].c_str())),
						static_cast<float>(atof(rVector[1].c_str())),
						static_cast<float>(atof(rVector[2].c_str())));
}

D3DXCOLOR TokenToColor(CTokenVector & rVector)
{
	if (4 != rVector.size())
	{
		assert(!"Size of token vector which will be converted to color is not 4");
		return D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	}

	return D3DXCOLOR(static_cast<float>(atof(rVector[0].c_str())),
						static_cast<float>(atof(rVector[1].c_str())),
						static_cast<float>(atof(rVector[2].c_str())),
						static_cast<float>(atof(rVector[3].c_str())));
}

///////////////////////////////////////////////////////////////////////////////////////////////////

static bool			gs_isRTL=false;

int CALLBACK EnumFontFamExProc(CONST LOGFONT* plogFont, CONST TEXTMETRIC* /*textMetric*/, DWORD /*dwWord*/, LPARAM lParam)
{
	return _stricmp((const char*)lParam, plogFont->lfFaceName);
}

bool IsDefaultRTL()
{
	return gs_isRTL;
}

void SetDefaultRTL(bool isRTL)
{
	gs_isRTL = isRTL;
}

int GetUTF8CharLength(const char* c_szText)
{
	const unsigned char lead = static_cast<unsigned char>(*c_szText);

	if (lead < 0x80)
		return 1;

	int length = 2;

	if (lead >= 0xF0)
		length = 4;
	else if (lead >= 0xE0)
		length = 3;

	int count = 1;

	while (count < length && (static_cast<unsigned char>(c_szText[count]) & 0xC0) == 0x80)
		++count;

	return count;
}

const char * GetDefaultFontFace()
{
	const char* fontFace = "Arial";

	LOGFONT logFont;

	memset(&logFont, 0, sizeof(logFont));

	logFont.lfCharSet = DEFAULT_CHARSET;

	HDC hDC=GetDC(NULL);

	const bool isInstalled = EnumFontFamiliesEx(hDC, &logFont, (FONTENUMPROC)EnumFontFamExProc, (LPARAM)fontFace, 0) == 0;

	ReleaseDC(NULL, hDC);

	return isInstalled ? fontFace : "";
}


int __base64_get( const int c )
{
	if( 'A' <= c && c <= 'Z' )
		return c-'A';
	if( 'a' <= c && c <= 'z' )
		return c - 'a' + 26;
	if( '0' <= c && c <= '9' )
		return c - '0' + 52;
	if( c == '+' )
		return 62;
	if( c == '/' )
		return 63;
	if( c == '=' )	// end of line
		return -1;
	return -2;	// non value;
}

void __strcat1(char * str,int i)
{
	// Append a single character without the deprecated strcat; identical semantics
	// (the caller owns the buffer, exactly as before).
	size_t len = strlen(str);
	str[len] = (char)i;
	str[len + 1] = '\0';
}

void base64_decode(const char * str,char * resultStr)
{
	int nCount=0, i=0, r, result;
	size_t length = strlen(str);
	char szDest[5]="";

	resultStr[0] = '\0';
	while(nCount < length)
	{
		i=0;
		strcpy_s(szDest, sizeof(szDest), "");
		while(nCount<length && i<4)	// You get 4 bytes.
		{			
			r = str[nCount++];
			result = __base64_get(r);
			if(result!=-2)
			{
				if(result!=-1)
					szDest[i++] = result;
				else szDest[i++] = '@';	// It's end (because number 64 is not used when decoding)
			}
		}

		if(i==4)	// All four sources were obtained. Start decode
		{	
			if( nCount+3 >= length )	// The end of data has been reached.
			{
				if( szDest[1] == '@' )
				{
					__strcat1(resultStr,(szDest[0]<<2));		
					break;
				}// exit while loop
				else 
					__strcat1(resultStr,(szDest[0]<<2 | szDest[1]>>4));	// 1 Byte
				if( szDest[2] == '@' )
				{
					__strcat1(resultStr,(szDest[1]<<4));
					break;
				}
				else 
					__strcat1(resultStr,(szDest[1]<<4 | szDest[2]>>2));	// 2 Byte				
				if( szDest[3] == '@' )
				{
					__strcat1(resultStr,(szDest[2]<<6));
					break;
				}
				else 
					__strcat1(resultStr,(szDest[2]<<6 | szDest[3]));	// 3 Byte				
			}
			else
			{
				__strcat1(resultStr,(szDest[0]<<2 | szDest[1]>>4));	// 1 Byte
				__strcat1(resultStr,(szDest[1]<<4 | szDest[2]>>2));	// 2 Byte				
				__strcat1(resultStr,(szDest[2]<<6 | szDest[3]));	// 3 Byte
			}
		}
		
	}// end of while

	for (i = 0; i < strlen(resultStr); i++) 
	{
		char c = resultStr[i];
		int xy = i + 5;
		resultStr[i] = char(c ^ xy);
	}	
	// E
}
