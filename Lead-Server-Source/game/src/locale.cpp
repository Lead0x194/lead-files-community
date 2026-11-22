#include "stdafx.h"
#include "locale_service.h"

const char * locale_find(const char *string)
{
	return string;
}

const char* locale_tag(const char* c_szType, unsigned int uiVnum)
{
	static thread_local char s_aszTag[8][24];
	static thread_local int s_iIndex = 0;

	char* szTag = s_aszTag[s_iIndex++ % 8];
	snprintf(szTag, sizeof(s_aszTag[0]), "[%s;%u]", c_szType, uiVnum);
	return szTag;
}

static bool locale_is_tag(const char* c_szText)
{
	static const char* c_aszType[] = { "[LS;", "[LC;", "[IN;", "[MN;", "[SN;" };

	const size_t c_uLength = strlen(c_szText);
	if (c_uLength < 5 || ']' != c_szText[c_uLength - 1])
		return false;

	for (size_t i = 0; i < sizeof(c_aszType) / sizeof(c_aszType[0]); ++i)
	{
		if (!strncmp(c_szText, c_aszType[i], 4))
			return true;
	}

	return false;
}

int locale_vformat(char* szBuffer, size_t uSize, const char* c_szFormat, va_list kArgs)
{
	if (strncmp(c_szFormat, "[LS;", 4))
		return vsnprintf(szBuffer, uSize, c_szFormat, kArgs);

	std::string stResult;
	char szSpec[32];
	char szPiece[512];

	for (const char* p = c_szFormat; *p; ++p)
	{
		if ('%' != *p)
		{
			stResult += *p;
			continue;
		}

		if ('%' == p[1])
		{
			stResult += "%%";
			++p;
			continue;
		}

		const char* c_szEnd = p + 1;
		while (*c_szEnd && strchr("-+ #0123456789.", *c_szEnd))
			++c_szEnd;

		int iLong = 0;
		while ('l' == *c_szEnd || 'h' == *c_szEnd)
		{
			if ('l' == *c_szEnd)
				++iLong;
			++c_szEnd;
		}

		const size_t c_uSpecLength = c_szEnd - p + 1;
		if (!*c_szEnd || c_uSpecLength >= sizeof(szSpec))
		{
			stResult += *p;
			continue;
		}

		memcpy(szSpec, p, c_uSpecLength);
		szSpec[c_uSpecLength] = '\0';

		switch (*c_szEnd)
		{
			case 's':
			{
				const char* c_szValue = va_arg(kArgs, const char*);
				if (!c_szValue)
					c_szValue = "";

				snprintf(szPiece, sizeof(szPiece), szSpec, c_szValue);

				if (locale_is_tag(c_szValue))
				{
					stResult += szPiece;
					break;
				}

				for (const char* q = szPiece; *q; ++q)
				{
					if (';' == *q)
						stResult += '\x1E';
					else if ('[' == *q)
						stResult += '\x1C';
					else if (']' == *q)
						stResult += '\x1D';
					else
						stResult += *q;
				}
				break;
			}

			case 'f':
			case 'g':
			case 'e':
				snprintf(szPiece, sizeof(szPiece), szSpec, va_arg(kArgs, double));
				stResult += szPiece;
				break;

			case 'p':
				snprintf(szPiece, sizeof(szPiece), szSpec, va_arg(kArgs, void*));
				stResult += szPiece;
				break;

			default:
				if (iLong >= 2)
					snprintf(szPiece, sizeof(szPiece), szSpec, va_arg(kArgs, long long));
				else if (1 == iLong)
					snprintf(szPiece, sizeof(szPiece), szSpec, va_arg(kArgs, long));
				else
					snprintf(szPiece, sizeof(szPiece), szSpec, va_arg(kArgs, int));
				stResult += szPiece;
				break;
		}

		p = c_szEnd;
	}

	return snprintf(szBuffer, uSize, "%s", stResult.c_str());
}
