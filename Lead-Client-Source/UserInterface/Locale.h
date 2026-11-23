#pragma once

#include "Locale_inc.h"
#include <string>

unsigned	LocaleService_GetCodePage();
const char* LocaleService_GetName();
const char*	LocaleService_GetLocaleName();
const char*	LocaleService_GetLocalePath();
bool		LocaleService_SetLanguage(const char* c_szName);
void		LocaleService_LoadStrings(const char* c_szLocalePath);
void		LocaleService_ReplaceTags(std::string& rstText);
void		LocaleService_RememberQuestButton(const std::string& c_rstScript);
std::string	LocaleService_GetQuestButtonName(int iIndex);
void		LocaleService_ResolveNameCommand(std::string& rstCommand);
void		LocaleService_LocalizeItemLinks(std::string& rstText);
const char*	LocaleService_GetSecurityKey();
int			LocaleService_StringCompareCI( LPCSTR szStringLeft, LPCSTR szStringRight, size_t sizeLength );

void		LocaleService_ForceSetLocale(const char* name, const char* localePath);
void		LocaleService_LoadConfig(const char* fileName);
unsigned	LocaleService_GetLastExp(int level);
int			LocaleService_GetSkillPower(unsigned level);