#pragma once

#include "Locale_inc.h"

bool		LocaleService_IsEUROPE();

unsigned	LocaleService_GetCodePage();
const char* LocaleService_GetName();
const char*	LocaleService_GetLocaleName();
const char*	LocaleService_GetLocalePath();
const char*	LocaleService_GetSecurityKey();
BOOL		LocaleService_IsLeadByte( const char chByte );
int			LocaleService_StringCompareCI( LPCSTR szStringLeft, LPCSTR szStringRight, size_t sizeLength );

void		LocaleService_ForceSetLocale(const char* name, const char* localePath);
void		LocaleService_LoadConfig(const char* fileName);
unsigned	LocaleService_GetLastExp(int level);
int			LocaleService_GetSkillPower(unsigned level);