#include "stdafx.h"
#include "common/tables.h"
#include "config.h"
#include "item_manager.h"
#include "item_manager_private_types.h"
#include "mob_manager.h"
#include "constants.h"
#include "cmd.h"

void PUBLIC_CreateDropList(const std::string& stFileName, bool bWriteChance = true);
void PUBLIC_CreateCommonDropList(const std::string& stFileName, bool bWriteChance = true);
void PUBLIC_CreateGMCommandFile(const std::string& stFileName, int iGMLevel = -1);

std::string stDirName = "public/";

void PUBLIC_CreateLists()
{
	if (!g_CreateDocumentationFiles )
	{
		sys_log(0, "Only public creator server creating public lists.");
		return;
	}

	PUBLIC_CreateDropList(stDirName + "mob_drop.txt", false);
	PUBLIC_CreateDropList(stDirName + "mob_drop_chance.txt", true);
	PUBLIC_CreateCommonDropList(stDirName + "common_drop.txt", false);
	PUBLIC_CreateCommonDropList(stDirName + "common_drop_chance.txt", true);
	PUBLIC_CreateGMCommandFile(stDirName + "command.txt");
	PUBLIC_CreateGMCommandFile(stDirName + "command_player.txt", GM_PLAYER);
	PUBLIC_CreateGMCommandFile(stDirName + "command_trial_gamemaster.txt", GM_LOW_WIZARD);
	PUBLIC_CreateGMCommandFile(stDirName + "command_gamemaster.txt", GM_WIZARD);
	PUBLIC_CreateGMCommandFile(stDirName + "command_super_gamemaster.txt", GM_HIGH_WIZARD);
	PUBLIC_CreateGMCommandFile(stDirName + "command_community_manager.txt", GM_GOD);
	PUBLIC_CreateGMCommandFile(stDirName + "command_administrator.txt", GM_IMPLEMENTOR);
}

void __CreateDropList_WriteMob(char* szLineBuf, size_t lineSize, FILE* pFile, DWORD dwMobRace)
{
	const CMob* pkMob = CMobManager::Instance().Get(dwMobRace);
	if (!pkMob)
	{
		sys_err("cannot get mob by race num %u", dwMobRace);
		return;
	}

	snprintf(szLineBuf, lineSize, "%s [Lv. %u] (%u):\n", pkMob->m_table.szName, pkMob->m_table.bLevel, dwMobRace);
	fputs(szLineBuf, pFile);
}

void __CreateDropList_WriteLevelGroup(char* szLineBuf, size_t lineSize, FILE* pFile, std::map<DWORD, CLevelItemGroup*>::const_iterator& level_it, bool bWriteChance = true, std::set<DWORD>* pkGroupSet = NULL)
{
	snprintf(szLineBuf, lineSize, "\t- From Level %d-%d the following items drop:\n", level_it->second->GetLevelLimitStart(), level_it->second->GetLevelLimitEnd());
	fputs(szLineBuf, pFile);

	const std::vector<CLevelItemGroup::SLevelItemGroupInfo>& rkLevelVec = level_it->second->GetVector();
	for (int i = 0; i < rkLevelVec.size(); ++i)
	{
		if (rkLevelVec[i].dwVNumStart != rkLevelVec[i].dwVNumEnd)
		{
			if (bWriteChance)
				snprintf(szLineBuf, lineSize, "\t\t- one of the following items at %.4f%%:\n", ((float)rkLevelVec[i].dwPct) / 10000.0f);
			else
				snprintf(szLineBuf, lineSize, "\t\t- one of the following items:\n");
			fputs(szLineBuf, pFile);
			for (DWORD dwVnum = rkLevelVec[i].dwVNumStart; dwVnum <= rkLevelVec[i].dwVNumEnd; ++dwVnum)
			{
				const TItemTable* pProto = ITEM_MANAGER::Instance().GetTable(dwVnum);
				if (!pProto)
				{
					sys_err("cannot get proto by item %u mob %u type level", dwVnum, level_it->first);
					continue;
				}

				snprintf(szLineBuf, lineSize, "\t\t\t- %dx %s (%u)\n",
					rkLevelVec[i].iCount, pProto->szName, dwVnum);
				fputs(szLineBuf, pFile);
			}
		}
		else
		{
			const DWORD& dwVnum = rkLevelVec[i].dwVNumStart;
			const TItemTable* pProto = ITEM_MANAGER::Instance().GetTable(dwVnum);
			if (!pProto)
			{
				sys_err("cannot get proto by item %u mob %u type level", dwVnum, level_it->first);
				continue;
			}

			if (bWriteChance)
			{
				snprintf(szLineBuf, lineSize, "\t\t- %dx %s (%u) [Chance %.4f%%]\n",
					rkLevelVec[i].iCount, pProto->szName, dwVnum, ((float)rkLevelVec[i].dwPct) / 10000.0f);
			}
			else
			{
				snprintf(szLineBuf, lineSize, "\t\t- %dx %s (%u)\n",
					rkLevelVec[i].iCount, pProto->szName, dwVnum);
			}
			fputs(szLineBuf, pFile);
		}
	}

	if (rkLevelVec.size() && pkGroupSet)
		pkGroupSet->insert(level_it->first);
}

void __CreateDropList_WriteDropGroup(char* szLineBuf, size_t lineSize, FILE* pFile, std::map<DWORD, CDropItemGroup*>::const_iterator& drop_it, bool bWriteChance = true, std::set<DWORD>* pkGroupSet = NULL)
{
	const std::vector<CDropItemGroup::SDropItemGroupInfo>& rkDropVec = drop_it->second->GetVector();
	for (int i = 0; i < rkDropVec.size(); ++i)
	{
		if (rkDropVec[i].dwVnumStart != rkDropVec[i].dwVnumEnd)
		{
			if (bWriteChance)
				snprintf(szLineBuf, lineSize, "\t- one of the following items at %.4f%%:\n", ((float)rkDropVec[i].dwPct) / 10000.0f);
			else
				snprintf(szLineBuf, lineSize, "\t- one of the following items:\n");
			fputs(szLineBuf, pFile);
			for (DWORD dwVnum = rkDropVec[i].dwVnumStart; dwVnum <= rkDropVec[i].dwVnumEnd; ++dwVnum)
			{
				const TItemTable* pProto = ITEM_MANAGER::Instance().GetTable(dwVnum);
				if (!pProto)
				{
					sys_err("cannot get proto by item %u mob %u type drop", dwVnum, drop_it->first);
					continue;
				}

				snprintf(szLineBuf, lineSize, "\t\t- %dx %s (%u)\n",
					rkDropVec[i].iCount, pProto->szName, dwVnum);
				fputs(szLineBuf, pFile);
			}
		}
		else
		{
			const DWORD& dwVnum = rkDropVec[i].dwVnumStart;
			const TItemTable* pProto = ITEM_MANAGER::Instance().GetTable(dwVnum);
			if (!pProto)
			{
				sys_err("cannot get proto by item %u mob %u type drop", dwVnum, drop_it->first);
				continue;
			}

			if (bWriteChance)
			{
				snprintf(szLineBuf, lineSize, "\t- %dx %s (%u) [Chance %.4f%%]\n",
					rkDropVec[i].iCount, pProto->szName, dwVnum, ((float)rkDropVec[i].dwPct) / 10000.0f);
			}
			else
			{
				snprintf(szLineBuf, lineSize, "\t- %dx %s (%u)\n",
					rkDropVec[i].iCount, pProto->szName, dwVnum);
			}
			fputs(szLineBuf, pFile);
		}
	}

	if (rkDropVec.size() && pkGroupSet)
		pkGroupSet->insert(drop_it->first);
}

void __CreateDropList_WriteMobGroup(char* szLineBuf, size_t lineSize, FILE* pFile, std::map<DWORD, std::vector<CMobItemGroup*> >::const_iterator& mob_it, bool bWriteChance = true, std::set<DWORD>* pkGroupSet = NULL)
{
	for (int i = 0; i < mob_it->second.size(); ++i)
	{
		CMobItemGroup* pGroup = mob_it->second[i];

		if (bWriteChance)
			snprintf(szLineBuf, lineSize, "\t- one of the following items from every %d. monster:\n", pGroup->GetKillPerDrop());
		else
			snprintf(szLineBuf, lineSize, "\t- random item with specific chance from the following items (only one droppable at a time):\n");
		fputs(szLineBuf, pFile);

		const std::vector<int> rkMobProbVec = pGroup->GetProbVector();
		const std::vector<CMobItemGroup::SMobItemGroupInfo> rkMobItemVec = pGroup->GetItemVector();
		for (int i = 0; i < rkMobItemVec.size(); ++i)
		{
			for (DWORD dwVnum = rkMobItemVec[i].dwItemVnumStart; dwVnum <= rkMobItemVec[i].dwItemVnumEnd; ++dwVnum)
			{
				const TItemTable* pProto = ITEM_MANAGER::Instance().GetTable(dwVnum);
				if (!pProto)
				{
					sys_err("cannot get proto by item %u mob %u type kill", dwVnum, mob_it->first);
					continue;
				}

				int iProb = rkMobProbVec[i];
				if (i > 0)
					iProb -= rkMobProbVec[i - 1];

				if (bWriteChance)
				{
					snprintf(szLineBuf, lineSize, "\t\t- %dx %s (%u) [Chance %d (compared to other items!)]\n",
						rkMobItemVec[i].iCount, pProto->szName, dwVnum, iProb);
				}
				else
				{
					snprintf(szLineBuf, lineSize, "\t\t- %dx %s (%u)\n",
						rkMobItemVec[i].iCount, pProto->szName, dwVnum);
				}
				fputs(szLineBuf, pFile);
			}
		}
	}

	if (pkGroupSet)
		pkGroupSet->insert(mob_it->first);
}

void PUBLIC_CreateDropList(const std::string& stFileName, bool bWriteChance)
{
	sys_log(0, "PUBLIC: Create Drop Item List \"%s\" (chance: %d)", stFileName.c_str(), bWriteChance);

	FILE* fp = fopen(stFileName.c_str(), "w");
	if (!fp)
	{
		sys_err("cannot open file to write \"%s\" !!", stFileName.c_str());
		return;
	}

	char szCurrentLine[512];
	const size_t currentLineSize = sizeof(szCurrentLine);

	const std::map<DWORD, std::vector<CMobItemGroup*> >& rkMobMap = ITEM_MANAGER::Instance().GetMobItemGroupMap();
	std::set<DWORD> kMobSet;
	const std::map<DWORD, CDropItemGroup*>& rkDropMap = ITEM_MANAGER::Instance().GetDropItemGroupMap();
	const std::map<DWORD, CLevelItemGroup*>& rkLevelMap = ITEM_MANAGER::Instance().GetLevelItemGroupMap();
	std::set<DWORD> kLevelSet;

	for (itertype(rkDropMap) it = rkDropMap.begin(); it != rkDropMap.end(); ++it)
	{
		__CreateDropList_WriteMob(szCurrentLine, currentLineSize, fp, it->first);

		itertype(rkLevelMap) level_it = rkLevelMap.find(it->first);
		if (level_it != rkLevelMap.end())
			__CreateDropList_WriteLevelGroup(szCurrentLine, currentLineSize, fp, level_it, bWriteChance, &kMobSet);

		itertype(rkMobMap) mob_it = rkMobMap.find(it->first);
		if (mob_it != rkMobMap.end())
			__CreateDropList_WriteMobGroup(szCurrentLine, currentLineSize, fp, mob_it, bWriteChance, &kMobSet);

		__CreateDropList_WriteDropGroup(szCurrentLine, currentLineSize, fp, it, bWriteChance);

		fputs("\n", fp);
	}

	for (itertype(rkMobMap) it = rkMobMap.begin(); it != rkMobMap.end(); ++it)
	{
		if (kMobSet.find(it->first) != kMobSet.end())
			continue;

		__CreateDropList_WriteMob(szCurrentLine, currentLineSize, fp, it->first);

		itertype(rkLevelMap) level_it = rkLevelMap.find(it->first);
		if (level_it != rkLevelMap.end())
			__CreateDropList_WriteLevelGroup(szCurrentLine, currentLineSize, fp, level_it, bWriteChance, &kMobSet);

		__CreateDropList_WriteMobGroup(szCurrentLine, currentLineSize, fp, it, bWriteChance);

		fputs("\n", fp);
	}

	for (itertype(rkLevelMap) it = rkLevelMap.begin(); it != rkLevelMap.end(); ++it)
	{
		if (kLevelSet.find(it->first) != kLevelSet.end())
			continue;

		__CreateDropList_WriteMob(szCurrentLine, currentLineSize, fp, it->first);

		__CreateDropList_WriteLevelGroup(szCurrentLine, currentLineSize, fp, it, bWriteChance);

		fputs("\n", fp);
	}

	fclose(fp);
}

void __CreateCommonDropList_WriteItems(char* szLineBuf, size_t lineSize, FILE* pFile, const std::vector<BYTE>* vec_ranks, const CItemDropInfo* info, bool bWriteChance)
{
	const char* szRankNames[MOB_RANK_MAX_NUM + 1] = {
		"PAWN",
		"S_PAWN",
		"KNIGHT",
		"S_KNIGHT",
		"BOSS",
		"KING",
		"KING_NO_METIN",
	};

	for (DWORD dwVnum = info->m_dwVnumStart; dwVnum <= info->m_dwVnumEnd; ++dwVnum)
	{
		const TItemTable* pTable = ITEM_MANAGER::instance().GetTable(dwVnum);
		if (!pTable)
		{
			sys_err("cannot get table by vnum %u", dwVnum);
			continue;
		}

		int iWroteSize = snprintf(szLineBuf, lineSize, "\t- %ux %s (%u)",
			info->m_bCount, pTable->szName, dwVnum);
		if (info->m_iLevelEnd > 0)
			iWroteSize += snprintf(szLineBuf + iWroteSize, lineSize - iWroteSize, " from Level %u - %u",
				info->m_iLevelStart, info->m_iLevelEnd);
		else
			iWroteSize += snprintf(szLineBuf + iWroteSize, lineSize - iWroteSize, " from Level %u",
				info->m_iLevelStart);
		iWroteSize += snprintf(szLineBuf + iWroteSize, lineSize - iWroteSize, " at Rank %u [%s]",
			(*vec_ranks)[0], szRankNames[(*vec_ranks)[0]]);
		for (int i = 1; i < vec_ranks->size(); ++i)
			iWroteSize += snprintf(szLineBuf + iWroteSize, lineSize - iWroteSize, ", %u [%s]", (*vec_ranks)[i], szRankNames[(*vec_ranks)[i]]);
		if (bWriteChance)
			iWroteSize += snprintf(szLineBuf + iWroteSize, lineSize - iWroteSize, " [Chance %.4f%%]", ((float)info->m_iPercent) / 10000.0f);
		fputs(szLineBuf, pFile);

		fputs("\n", pFile);
	}
}

typedef struct SDropIndex {
	DWORD	dwVnumStart;
	DWORD	dwVnumEnd;
	BYTE	bStartLevel;
	BYTE	bEndLevel;

	SDropIndex(DWORD dwVnumStart, DWORD dwVnumEnd, BYTE bStartLevel, BYTE bEndLevel) :
		dwVnumStart(dwVnumStart), dwVnumEnd(dwVnumEnd), bStartLevel(bStartLevel), bEndLevel(bEndLevel)
	{
	}

	bool operator==(const SDropIndex& other) const
	{
		return other.dwVnumStart == dwVnumStart && other.dwVnumEnd == dwVnumEnd &&
			other.bStartLevel == bStartLevel && other.bEndLevel == bEndLevel;
	}
	bool operator<(const SDropIndex& other) const
	{
		return other.dwVnumStart < dwVnumStart;
	}
} TDropIndex;

void PUBLIC_CreateCommonDropList(const std::string& stFileName, bool bWriteChance)
{
	sys_log(0, "PUBLIC: Create Common Drop Item List \"%s\" (chance: %d)", stFileName.c_str(), bWriteChance);

	FILE* fp = fopen(stFileName.c_str(), "w");
	if (!fp)
	{
		sys_err("cannot open file to write \"%s\" !!", stFileName.c_str());
		return;
	}

	char szCurrentLine[512];
	const size_t currentLineSize = sizeof(szCurrentLine);

	snprintf(szCurrentLine, currentLineSize, "Alternative Item Drops (Monster independent):");
	fputs(szCurrentLine, fp);
	fputs("\n", fp);

	std::map<TDropIndex, std::pair<std::vector<BYTE>, const CItemDropInfo*> > map_commonDrop;
	for (int rank = 0; rank < MOB_RANK_MAX_NUM; ++rank)
	{
		for (int i = 0; i < g_vec_pkCommonDropItem[rank].size(); ++i)
		{
			const CItemDropInfo* info = &g_vec_pkCommonDropItem[rank][i];
			TDropIndex index(info->m_dwVnumStart, info->m_dwVnumEnd, info->m_iLevelStart, info->m_iLevelEnd);
			itertype(map_commonDrop) it = map_commonDrop.find(index);
			if (it != map_commonDrop.end())
				it->second.first.push_back(rank);
			else
			{
				std::pair<std::vector<BYTE>, const CItemDropInfo*> data;
				data.first.push_back(rank);
				data.second = info;
				map_commonDrop.insert(std::pair<TDropIndex, std::pair<std::vector<BYTE>, const CItemDropInfo*> >(index, data));
			}
		}
	}

	for (itertype(map_commonDrop) it = map_commonDrop.begin(); it != map_commonDrop.end(); ++it)
	{
		__CreateCommonDropList_WriteItems(szCurrentLine, currentLineSize, fp, &it->second.first, it->second.second, bWriteChance);
	}

	fclose(fp);
}

void PUBLIC_CreateGMCommandFile(const std::string& stFileName, int iGMLevel)
{
	sys_log(0, "PUBLIC: Create GM Command List \"%s\" (gm_level: %d)", stFileName.c_str(), iGMLevel);

	FILE* fp = fopen(stFileName.c_str(), "w");
	if (!fp)
	{
		sys_err("cannot open file to write \"%s\" !!", stFileName.c_str());
		return;
	}

	const char* arGMNameList[GM_MAX_NUM] = {
		"Player",
		"Trial GameMaster",
		"GameMaster",
		"Super GameMaster",
		"Community Manager",
		"Server Administrator"
	};

	char szCurrentLine[512];
	const size_t currentLineSize = sizeof(szCurrentLine);

	if (iGMLevel >= GM_PLAYER && iGMLevel <= GM_IMPLEMENTOR)
		snprintf(szCurrentLine, currentLineSize, "Chat commands for %s:\n", arGMNameList[iGMLevel]);
	else
		snprintf(szCurrentLine, currentLineSize, "Chat commands:\n");
	fputs(szCurrentLine, fp);

	for (int i = 0; *cmd_info[i].command != '\n'; ++i)
	{
		if (iGMLevel == -1)
		{
			snprintf(szCurrentLine, currentLineSize, "\t/%s (required rank: %s)\n", cmd_info[i].command, arGMNameList[cmd_info[i].gm_level]);
		}
		else if (iGMLevel >= cmd_info[i].gm_level && (iGMLevel == GM_PLAYER || cmd_info[i].gm_level != GM_PLAYER))
		{
			snprintf(szCurrentLine, currentLineSize, "\t/%s\n", cmd_info[i].command);
		}
		else
			continue;

		fputs(szCurrentLine, fp);
	}

	fclose(fp);
}
