#include "StdAfx.h"
#include "Locale.h"
#include "PythonApplication.h"
#include "../eterBase/CRC32.h"
#include "../EterPack/EterPackManager.h"
#include "../GameLib/ItemManager.h"
#include "PythonNonPlayer.h"
#include "PythonSkill.h"

const char* LSS_EUROPE		= "EUROPE";

#ifndef LSS_SECURITY_KEY
#define LSS_SECURITY_KEY	"1234abcd5678efgh"
#endif

std::string __SECURITY_KEY_STRING__ = LSS_SECURITY_KEY;

char	MULTI_LOCALE_PATH[256]		= "locale/de";
char	MULTI_LOCALE_NAME[256]		= "de";

bool LocaleService_SetLanguage(const char* c_szName)
{
	char szFileName[256];
	sprintf_s(szFileName, sizeof(szFileName), "locale/%s/locale_game.txt", c_szName);

	if (!CEterPackManager::Instance().isExist(szFileName))
		return false;

	strcpy_s(MULTI_LOCALE_NAME, sizeof(MULTI_LOCALE_NAME), c_szName);
	sprintf_s(MULTI_LOCALE_PATH, sizeof(MULTI_LOCALE_PATH), "locale/%s", MULTI_LOCALE_NAME);
	return true;
}

static const char* DetectSystemLanguage()
{
	static const struct
	{
		WORD wLanguage;
		const char* c_szName;
	} c_akLanguage[] =
	{
		{ LANG_ARABIC, "ae" }, { LANG_CZECH, "cz" }, { LANG_GERMAN, "de" }, { LANG_DANISH, "dk" },
		{ LANG_ENGLISH, "en" }, { LANG_SPANISH, "es" }, { LANG_FRENCH, "fr" }, { LANG_GREEK, "gr" },
		{ LANG_HUNGARIAN, "hu" }, { LANG_ITALIAN, "it" }, { LANG_DUTCH, "nl" }, { LANG_POLISH, "pl" },
		{ LANG_PORTUGUESE, "pt" }, { LANG_ROMANIAN, "ro" }, { LANG_RUSSIAN, "ru" }, { LANG_TURKISH, "tr" },
	};

	const WORD c_wLanguage = PRIMARYLANGID(GetUserDefaultUILanguage());
	for (size_t i = 0; i < sizeof(c_akLanguage) / sizeof(c_akLanguage[0]); ++i)
	{
		if (c_akLanguage[i].wLanguage == c_wLanguage)
			return c_akLanguage[i].c_szName;
	}

	return "en";
}

void LocaleService_LoadConfig(const char* fileName)
{
	strcpy_s(MULTI_LOCALE_NAME, sizeof(MULTI_LOCALE_NAME), DetectSystemLanguage());
	sprintf_s(MULTI_LOCALE_PATH, sizeof(MULTI_LOCALE_PATH), "locale/%s", MULTI_LOCALE_NAME);

	FILE* fp = NULL;
	fopen_s(&fp, fileName, "rt");

	if (!fp)
		return;

	char line[256];
	char command[256];
	char value[256];

	while (fgets(line, sizeof(line), fp))
	{
		if (2 != sscanf_s(line, "%s %s", command, (unsigned)sizeof(command), value, (unsigned)sizeof(value)))
			continue;

		if (_stricmp(command, "LANGUAGE"))
			continue;

		strcpy_s(MULTI_LOCALE_NAME, sizeof(MULTI_LOCALE_NAME), value);
		sprintf_s(MULTI_LOCALE_PATH, sizeof(MULTI_LOCALE_PATH), "locale/%s", MULTI_LOCALE_NAME);
	}

	fclose(fp);
}

static std::map<DWORD, std::string> s_kStringMap;
static std::map<DWORD, std::string> s_kQuestMap;

static void LoadStringFile(const char* c_szFileName, std::map<DWORD, std::string>& rkMap)
{
	rkMap.clear();

	const VOID* pvData;
	CMappedFile kFile;
	if (!CEterPackManager::Instance().Get(kFile, c_szFileName, &pvData))
		return;

	CMemoryTextFileLoader kTextFileLoader;
	kTextFileLoader.Bind(kFile.Size(), pvData);

	CTokenVector kTokenVector;
	for (DWORD i = 0; i < kTextFileLoader.GetLineCount(); ++i)
	{
		if (!kTextFileLoader.SplitLineByTab(i, &kTokenVector) || kTokenVector.size() < 2)
			continue;

		rkMap[static_cast<DWORD>(atoi(kTokenVector[0].c_str()))] = kTokenVector[1];
	}
}

void LocaleService_LoadStrings(const char* c_szLocalePath)
{
	char szFileName[256];

	sprintf_s(szFileName, sizeof(szFileName), "%s/locale_string.txt", c_szLocalePath);
	LoadStringFile(szFileName, s_kStringMap);

	sprintf_s(szFileName, sizeof(szFileName), "%s/locale_quest.txt", c_szLocalePath);
	LoadStringFile(szFileName, s_kQuestMap);
}

static std::string FormatString(const std::string& c_rstFormat, const std::vector<std::string>& c_rkTokenVector)
{
	std::string stResult;
	size_t uToken = 1;

	for (size_t i = 0; i < c_rstFormat.length(); ++i)
	{
		if ('%' != c_rstFormat[i])
		{
			stResult += c_rstFormat[i];
			continue;
		}

		if (i + 1 < c_rstFormat.length() && '%' == c_rstFormat[i + 1])
		{
			stResult += '%';
			++i;
			continue;
		}

		size_t j = i + 1;
		while (j < c_rstFormat.length() && strchr("-+#0123456789.hlIz", c_rstFormat[j]))
			++j;

		if (j < c_rstFormat.length() && strchr("diuxXsfcgp", c_rstFormat[j]))
		{
			if (uToken < c_rkTokenVector.size())
				stResult += c_rkTokenVector[uToken++];

			i = j;
		}
		else
		{
			stResult += '%';
		}
	}

	return stResult;
}

static bool ExpandTag(const std::string& c_rstTag, std::string& rstResult)
{
	std::vector<std::string> kTokenVector(1);
	int iDepth = 0;

	for (char c : c_rstTag)
	{
		if ('[' == c)
			++iDepth;
		else if (']' == c)
			--iDepth;

		if (';' == c && 0 == iDepth)
			kTokenVector.push_back("");
		else
			kTokenVector.back() += c;
	}

	if (kTokenVector.size() < 2)
		return false;

	const std::string c_rstType = kTokenVector[0];
	const bool c_bString = "LS" == c_rstType;
	const bool c_bQuest = "LC" == c_rstType;
	if (!c_bString && !c_bQuest && "IN" != c_rstType && "MN" != c_rstType && "SN" != c_rstType)
		return false;

	for (size_t i = 1; i < kTokenVector.size(); ++i)
		LocaleService_ReplaceTags(kTokenVector[i]);

	const DWORD c_dwID = static_cast<DWORD>(atoi(kTokenVector[1].c_str()));
	kTokenVector.erase(kTokenVector.begin());

	if (c_bString || c_bQuest)
	{
		const std::map<DWORD, std::string>& c_rkMap = c_bString ? s_kStringMap : s_kQuestMap;
		std::map<DWORD, std::string>::const_iterator itor = c_rkMap.find(c_dwID);
		if (c_rkMap.end() == itor)
			return false;

		if (c_bString)
		{
			size_t uSpecCount = 0;
			const std::string& c_rstFormat = itor->second;
			for (size_t i = 0; i + 1 < c_rstFormat.length(); ++i)
			{
				if ('%' != c_rstFormat[i])
					continue;

				if ('%' != c_rstFormat[i + 1])
					++uSpecCount;

				++i;
			}

			while (uSpecCount > 0 && kTokenVector.size() > uSpecCount + 1)
			{
				const std::string c_stLast = kTokenVector.back();
				kTokenVector.pop_back();
				kTokenVector.back() += ';' + c_stLast;
			}
		}

		if (c_bQuest && kTokenVector.size() < 2)
			rstResult = itor->second;
		else
			rstResult = FormatString(itor->second, kTokenVector);
		return true;
	}

	if ("IN" == c_rstType)
	{
		CItemData* pItemData;
		if (!CItemManager::Instance().GetItemDataPointer(c_dwID, &pItemData))
			return false;

		rstResult = pItemData->GetName();
		return true;
	}

	if ("MN" == c_rstType)
	{
		rstResult = CPythonNonPlayer::Instance().GetMonsterName(c_dwID);
		return !rstResult.empty();
	}

	CPythonSkill::TSkillData* pSkillData;
	if (!CPythonSkill::Instance().GetSkillData(c_dwID, &pSkillData))
		return false;

	rstResult = pSkillData->strName;
	return true;
}

void LocaleService_ReplaceTags(std::string& rstText)
{
	if (std::string::npos == rstText.find('['))
		return;

	std::string stResult;
	size_t i = 0;

	while (i < rstText.length())
	{
		if ('[' == rstText[i])
		{
			int iDepth = 0;
			size_t uEnd = i;
			for (; uEnd < rstText.length(); ++uEnd)
			{
				if ('[' == rstText[uEnd])
					++iDepth;
				else if (']' == rstText[uEnd] && 0 == --iDepth)
					break;
			}

			std::string stExpanded;
			if (uEnd < rstText.length() && ExpandTag(rstText.substr(i + 1, uEnd - i - 1), stExpanded))
			{
				stResult += stExpanded;
				i = uEnd + 1;
				continue;
			}
		}

		stResult += rstText[i++];
	}

	rstText.swap(stResult);
}

static std::map<int, std::string> s_kQuestButtonNameMap;

void LocaleService_RememberQuestButton(const std::string& c_rstScript)
{
	static const std::string c_stHead = "[QUESTBUTTON idx;";
	static const std::string c_stName = "|name;";

	if (0 != c_rstScript.compare(0, c_stHead.length(), c_stHead))
		return;

	const size_t c_uName = c_rstScript.find(c_stName);
	if (std::string::npos == c_uName)
		return;

	int iDepth = 0;
	size_t uEnd = c_uName + c_stName.length();
	for (; uEnd < c_rstScript.length(); ++uEnd)
	{
		const char c = c_rstScript[uEnd];
		if ('[' == c)
			++iDepth;
		else if (0 == iDepth && (']' == c || '|' == c))
			break;
		else if (']' == c)
			--iDepth;
	}

	const size_t c_uStart = c_uName + c_stName.length();
	s_kQuestButtonNameMap[static_cast<int>(atoi(c_rstScript.c_str() + c_stHead.length()))] = c_rstScript.substr(c_uStart, uEnd - c_uStart);
}

std::string LocaleService_GetQuestButtonName(int iIndex)
{
	std::map<int, std::string>::const_iterator itor = s_kQuestButtonNameMap.find(iIndex);
	if (s_kQuestButtonNameMap.end() == itor)
		return "";

	std::string stName(itor->second);
	LocaleService_ReplaceTags(stName);
	return stName;
}

static std::string NormalizeName(const std::string& c_rstName)
{
	std::wstring wstName(c_rstName.length() + 1, L'\0');
	const int c_iLength = MultiByteToWideChar(CP_UTF8, 0, c_rstName.c_str(), static_cast<int>(c_rstName.length()), &wstName[0], static_cast<int>(wstName.length()));
	wstName.resize(c_iLength > 0 ? c_iLength : 0);
	if (!wstName.empty())
		CharLowerBuffW(&wstName[0], static_cast<DWORD>(wstName.length()));

	std::wstring wstResult;
	for (size_t i = 0; i < wstName.length(); ++i)
	{
		wchar_t c = wstName[i];
		if (L'"' == c)
			continue;

		if (L'_' == c)
			c = L' ';

		if (L' ' == c && (wstResult.empty() || L' ' == wstResult[wstResult.length() - 1]))
			continue;

		if (L'+' == c && !wstResult.empty() && L' ' == wstResult[wstResult.length() - 1])
			wstResult.erase(wstResult.length() - 1);

		wstResult += c;
	}

	while (!wstResult.empty() && L' ' == wstResult[wstResult.length() - 1])
		wstResult.erase(wstResult.length() - 1);

	std::string stResult(wstResult.length() * 4 + 1, '\0');
	const int c_iBytes = WideCharToMultiByte(CP_UTF8, 0, wstResult.c_str(), static_cast<int>(wstResult.length()), &stResult[0], static_cast<int>(stResult.length()), NULL, NULL);
	stResult.resize(c_iBytes > 0 ? c_iBytes : 0);
	return stResult;
}

static DWORD FindVnumByName(bool bItem, const std::string& c_rstName)
{
	if (bItem)
	{
		const CItemManager::TItemMap& c_rkItemMap = CItemManager::Instance().GetItemMap();
		for (CItemManager::TItemMap::const_iterator itor = c_rkItemMap.begin(); itor != c_rkItemMap.end(); ++itor)
		{
			if (NormalizeName(itor->second->GetName()) == c_rstName)
				return itor->first;
		}

		return 0;
	}

	const std::map<DWORD, std::string>& c_rkNameMap = CPythonNonPlayer::Instance().GetNameMap();
	for (std::map<DWORD, std::string>::const_iterator itor = c_rkNameMap.begin(); itor != c_rkNameMap.end(); ++itor)
	{
		if (NormalizeName(itor->second) == c_rstName)
			return itor->first;
	}

	return 0;
}

void LocaleService_ResolveNameCommand(std::string& rstCommand)
{
	const size_t c_uNameStart = rstCommand.find(' ');
	if (std::string::npos == c_uNameStart || c_uNameStart + 1 >= rstCommand.length())
		return;

	const std::string c_stCommand = rstCommand.substr(0, c_uNameStart);
	const bool c_bItem = "/i" == c_stCommand || "/item" == c_stCommand;
	const bool c_bMonster = "/m" == c_stCommand || "/mob" == c_stCommand;
	if (!c_bItem && !c_bMonster)
		return;

	const std::string c_stRest = rstCommand.substr(c_uNameStart + 1);
	if (c_stRest.empty() || isdigit(static_cast<unsigned char>(c_stRest[0])))
		return;

	std::vector<size_t> kEndVector;
	if ('"' == c_stRest[0])
	{
		const size_t c_uQuote = c_stRest.find('"', 1);
		kEndVector.push_back(std::string::npos == c_uQuote ? c_stRest.length() : c_uQuote + 1);
	}
	else
	{
		kEndVector.push_back(c_stRest.length());
		for (size_t i = c_stRest.length(); i > 0; --i)
		{
			if (' ' == c_stRest[i - 1])
				kEndVector.push_back(i - 1);
		}
	}

	for (size_t i = 0; i < kEndVector.size(); ++i)
	{
		const std::string c_stName = NormalizeName(c_stRest.substr(0, kEndVector[i]));
		if (c_stName.empty())
			continue;

		const DWORD c_dwVnum = FindVnumByName(c_bItem, c_stName);
		if (0 == c_dwVnum)
			continue;

		rstCommand = c_stCommand + " " + std::to_string(c_dwVnum) + c_stRest.substr(kEndVector[i]);
		return;
	}
}

void LocaleService_LocalizeItemLinks(std::string& rstText)
{
	static const std::string c_stLink = "|Hitem:";
	static const std::string c_stNameStart = "|h[";
	static const std::string c_stNameEnd = "]|h";

	size_t uPos = 0;
	while (std::string::npos != (uPos = rstText.find(c_stLink, uPos)))
	{
		uPos += c_stLink.length();

		const DWORD c_dwVnum = static_cast<DWORD>(strtoul(rstText.c_str() + uPos, NULL, 16));
		const size_t c_uNameStart = rstText.find(c_stNameStart, uPos);
		if (std::string::npos == c_uNameStart)
			break;

		const size_t c_uNameEnd = rstText.find(c_stNameEnd, c_uNameStart);
		if (std::string::npos == c_uNameEnd)
			break;

		CItemData* pItemData;
		if (CItemManager::Instance().GetItemDataPointer(c_dwVnum, &pItemData))
		{
			const size_t c_uStart = c_uNameStart + c_stNameStart.length();
			rstText.replace(c_uStart, c_uNameEnd - c_uStart, pItemData->GetName());
		}

		uPos = c_uNameStart;
	}
}

unsigned LocaleService_GetLastExp(int level)
{
	static const int GUILD_LEVEL_MAX = 20;

	static DWORD INTERNATIONAL_GUILDEXP_LIST[GUILD_LEVEL_MAX+1] = 
	{
		0,			// 0
		6000UL,		// 1
		18000UL,	// 2
		36000UL,	// 3
		64000UL,	// 4
		94000UL,	// 5
		130000UL,	// 6
		172000UL,	// 7
		220000UL,	// 8
		274000UL,	// 9
		334000UL,	// 10
		400000UL,	// 11
		600000UL,	// 12
		840000UL,	// 13
		1120000UL,	// 14
		1440000UL,	// 15
		1800000UL,	// 16
		2600000UL,	// 17
		3200000UL,	// 18
		4000000UL,	// 19		
		16800000UL	// 20		
	};

	if (level < 0 && level >= GUILD_LEVEL_MAX)
		return 0;
	
	return INTERNATIONAL_GUILDEXP_LIST[level];	
}

int LocaleService_GetSkillPower(unsigned level)
{
	static const unsigned SKILL_POWER_NUM = 50;

	if (level >= SKILL_POWER_NUM)
		return 0;

	// 0 5 6 8 10 12 14 16 18 20 22 24 26 28 30 32 34 36 38 40 50 52 54 56 58 60 63 66 69 72 82 85 88 91 94 98 102 106 110 115 125 125 125 125 125
	static unsigned INTERNATIONAL_SKILL_POWERS[SKILL_POWER_NUM]=
	{
		0, 
			5,  6,  8, 10, 12, 
			14, 16, 18, 20, 22, 
			24, 26, 28, 30, 32, 
			34, 36, 38, 40, 50, // master
			52, 54, 56, 58, 60, 
			63, 66, 69, 72, 82, // grand_master
			85, 88, 91, 94, 98, 
			102,106,110,115,125,// perfect_master
			125,	
	};
	return INTERNATIONAL_SKILL_POWERS[level];
}

const char*	LocaleService_GetSecurityKey()
{
	return __SECURITY_KEY_STRING__.c_str();
}

const char* LocaleService_GetName()				{ return LSS_EUROPE; }
unsigned int LocaleService_GetCodePage()		{ return CP_UTF8; }
const char*	LocaleService_GetLocaleName()		{ return MULTI_LOCALE_NAME; }
const char*	LocaleService_GetLocalePath()		{ return MULTI_LOCALE_PATH; }

void LocaleService_ForceSetLocale(const char* name, const char* localePath)
{
	strcpy_s(MULTI_LOCALE_NAME, sizeof(MULTI_LOCALE_NAME), name);
	strcpy_s(MULTI_LOCALE_PATH, sizeof(MULTI_LOCALE_PATH), localePath);
}


int LocaleService_StringCompareCI( LPCSTR szStringLeft, LPCSTR szStringRight, size_t sizeLength )
{
	return strnicmp( szStringLeft, szStringRight, sizeLength );
}