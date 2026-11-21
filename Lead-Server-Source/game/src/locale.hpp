#ifndef __INC_METIN2_GAME_LOCALE_H__
#define __INC_METIN2_GAME_LOCALE_H__

extern "C"
{
	const char *locale_find(const char *string);

#define LC_TEXT(str) locale_find(str)
};

#endif
