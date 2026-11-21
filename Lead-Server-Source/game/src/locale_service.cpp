#include "stdafx.h"
#include "locale_service.h"
#include "constants.h"
#include "banword.h"
#include "utils.h"
#include "mob_manager.h"
#include "empire_text_convert.h"
#include "config.h"
#include "skill_power.h"

using namespace std;

extern string		g_stQuestDir;
extern set<string> 	g_setQuestObjectDir;

string g_stServiceName;
string g_stServiceBasePath = ".";
string g_stServiceMapPath = "data/map";

string g_stLocale = "euckr";
string g_stLocaleFilename;

BYTE PK_PROTECT_LEVEL = 30;

int (*check_name) (const char * str) = NULL;
int (*is_twobyte) (const char * str) = NULL;

int is_twobyte_euckr(const char * str)
{
	return ishan(*str);
}

int check_name_independent(const char * str)
{
	if (CBanwordManager::instance().CheckString(str, strlen(str)))
		return 0;

	// You can't make it with a monster's name. .
	char szTmp[256];
	str_lower(str, szTmp, sizeof(szTmp));

	if (CMobManager::instance().Get(szTmp, false))
		return 0;

	return 1;
}

int check_name_euckr(const char * str)
{
	int		code;
	const char*	tmp;

	if (!str || !*str)
		return 0;

	if ( strlen(str) < 2 || strlen(str) > 12 )
		return 0;

	for (tmp = str; *tmp; ++tmp)
	{
		// If it is not in Korean and is blank, it is wrong.
		if (isnhspace(*tmp))
			return 0;

		// It is suitable if it is a number and not Hangul. .
		if (isnhdigit(*tmp))
			continue;

		// It is suitable if it is in English and not Korean. .   
		if (!ishan(*tmp) && isalpha(*tmp))
			continue;

		code = *tmp;
		code += 256;

		if (code < 176 || code > 200)
			return 0;

		++tmp;

		if (!*tmp)
			break;
	}

	return check_name_independent(str);
}

void LocaleService_LoadLocaleStringFile()
{
	if (g_stLocaleFilename.empty())
		return;

	if (g_bAuthServer)
		return;

	fprintf(stderr, "LocaleService %s\n", g_stLocaleFilename.c_str());

	locale_init(g_stLocaleFilename.c_str());
}

void LocaleService_LoadEmpireTextConvertTables()
{
	char szFileName[256];

	for (int iEmpire = 1; iEmpire <= 3; ++iEmpire)
	{
		snprintf(szFileName, sizeof(szFileName), "%s/lang%d.cvt", LocaleService_GetBasePath().c_str(), iEmpire);
		sys_log(0, "Load %s", szFileName);

		LoadEmpireTextConvertTable(iEmpire, szFileName);
	}
}

static void __LocaleService_Init_DEFAULT()
{
	g_stLocaleFilename = "";

	g_stServiceBasePath = "locale/" + g_stServiceName;
	g_stServiceMapPath = g_stServiceBasePath + "/map";
	g_stQuestDir = g_stServiceBasePath + "/quest";

	g_setQuestObjectDir.clear();	
	g_setQuestObjectDir.insert(g_stQuestDir + "/object");
}

static void __CheckPlayerSlot(const std::string& service_name)
{
	if (PLAYER_PER_ACCOUNT != 4)
	{
		printf("<ERROR> PLAYER_PER_ACCOUNT = %d\n", PLAYER_PER_ACCOUNT);
		exit(0);
	}
}

bool LocaleService_Init(const std::string& c_rstServiceName)
{
	if (!g_stServiceName.empty())
	{
		sys_err("ALREADY exist service");
		return false;
	}

	g_stServiceName = c_rstServiceName;

	__LocaleService_Init_DEFAULT();

	fprintf(stdout, "Setting Locale \"%s\" (Path: %s)\n", g_stServiceName.c_str(), g_stServiceBasePath.c_str());

	__CheckPlayerSlot(g_stServiceName);

	return true;
}

void LocaleService_TransferDefaultSetting()
{
	if (!check_name)
		check_name = check_name_euckr;

	if (!is_twobyte)
		is_twobyte = is_twobyte_euckr;

	if (!exp_table)
		exp_table = exp_table_common;

	if (!CTableBySkill::instance().Check())
		exit(1);

	if (!aiPercentByDeltaLevForBoss)
		aiPercentByDeltaLevForBoss = aiPercentByDeltaLevForBoss_euckr;

	if (!aiPercentByDeltaLev)
		aiPercentByDeltaLev = aiPercentByDeltaLev_euckr;

	if (!aiChainLightningCountBySkillLevel)
		aiChainLightningCountBySkillLevel = aiChainLightningCountBySkillLevel_euckr;
}

const std::string& LocaleService_GetBasePath()
{
	return g_stServiceBasePath;
}

const std::string& LocaleService_GetMapPath()
{
	return g_stServiceMapPath;
}

const std::string& LocaleService_GetQuestPath()
{
	return g_stQuestDir;
}

