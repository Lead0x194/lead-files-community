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

string g_stServiceBasePath = "common";
string g_stServiceMapPath = "common/map";

string g_stDatabaseCharset = "utf8mb4";

BYTE PK_PROTECT_LEVEL = 30;

int utf8_char_length(const char * str)
{
	const unsigned char c = static_cast<unsigned char>(*str);

	int length = 1;
	if (c >= 0xF0)
		length = 4;
	else if (c >= 0xE0)
		length = 3;
	else if (c >= 0xC0)
		length = 2;

	for (int i = 1; i < length; ++i)
	{
		if ((static_cast<unsigned char>(str[i]) & 0xC0) != 0x80)
			return i;
	}

	return length;
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

int check_name(const char * str)
{
	if (!str || strlen(str) > CHARACTER_NAME_MAX_LEN)
		return 0;

	int count = 0;

	for (const unsigned char * tmp = (const unsigned char *) str; *tmp; ++count)
	{
		if (*tmp < 0x80)
		{
			if (!isalnum(*tmp))
				return 0;

			++tmp;
			continue;
		}

		if ((tmp[0] & 0xF0) != 0xE0 || (tmp[1] & 0xC0) != 0x80 || (tmp[2] & 0xC0) != 0x80)
			return 0;

		const int code = ((tmp[0] & 0x0F) << 12) | ((tmp[1] & 0x3F) << 6) | (tmp[2] & 0x3F);

		if (code < 0xAC00 || code > 0xD7A3)
			return 0;

		tmp += 3;
	}

	if (count < 2 || count > 12)
		return 0;

	return check_name_independent(str);
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

static void __CheckPlayerSlot()
{
	if (PLAYER_PER_ACCOUNT != 4)
	{
		printf("<ERROR> PLAYER_PER_ACCOUNT = %d\n", PLAYER_PER_ACCOUNT);
		exit(0);
	}
}

void LocaleService_Init()
{
	g_stQuestDir = g_stServiceBasePath + "/quest";

	g_setQuestObjectDir.clear();
	g_setQuestObjectDir.insert(g_stQuestDir + "/object");

	__CheckPlayerSlot();
}

void LocaleService_TransferDefaultSetting()
{
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

