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
