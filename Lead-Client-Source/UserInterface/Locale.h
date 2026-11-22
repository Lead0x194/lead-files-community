#pragma once

#include "Locale_inc.h"

unsigned	LocaleService_GetCodePage();
const char* LocaleService_GetName();
const char*	LocaleService_GetLocaleName();
const char*	LocaleService_GetLocalePath();
bool		LocaleService_IsRTL();
const char*	LocaleService_GetSecurityKey();
int			LocaleService_StringCompareCI( LPCSTR szStringLeft, LPCSTR szStringRight, size_t sizeLength );

void		LocaleService_ForceSetLocale(const char* name, const char* localePath);
void		LocaleService_LoadConfig(const char* fileName);
unsigned	LocaleService_GetLastExp(int level);
int			LocaleService_GetSkillPower(unsigned level);