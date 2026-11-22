#ifndef __INC_METIN2_GAME_LOCALE_H__
#define __INC_METIN2_GAME_LOCALE_H__

extern "C"
{
	const char *locale_find(const char *string);

#define LC_TEXT(str) locale_find(str)
};

const char* locale_tag(const char* c_szType, unsigned int uiVnum);

int locale_vformat(char* szBuffer, size_t uSize, const char* c_szFormat, va_list kArgs);

#define LC_ITEM(vnum) locale_tag("IN", vnum)
#define LC_MOB(vnum) locale_tag("MN", vnum)
#define LC_SKILL(vnum) locale_tag("SN", vnum)

#endif
