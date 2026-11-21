#ifndef __LOCALE_SERVCICE__
#define __LOCALE_SERVCICE__

void LocaleService_Init();
void LocaleService_LoadEmpireTextConvertTables();
void LocaleService_TransferDefaultSetting();
const std::string& LocaleService_GetBasePath();
const std::string& LocaleService_GetMapPath();
const std::string& LocaleService_GetQuestPath();

#endif
