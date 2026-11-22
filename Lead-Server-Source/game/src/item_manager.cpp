#include "stdafx.h"
#include "utils.h"
#include "config.h"
#include "char.h"
#include "char_manager.h"
#include "desc_client.h"
#include "db.h"
#include "log.h"
#include "skill.h"
#include "text_file_loader.h"
#include "priv_manager.h"
#include "questmanager.h"
#include "unique_item.h"
#include "safebox.h"
#include "blend_item.h"
#include "dev_log.h"
#include "locale_service.h"
#include "item.h"
#include "item_manager.h"
#include "item_manager_private_types.h"

#include "common/VnumHelper.h"
#include "DragonSoul.h"
#include "cube.h"

ITEM_MANAGER::ITEM_MANAGER()
	: m_iTopOfTable(0), m_dwVIDCount(0), m_dwCurrentID(0)
{
	m_ItemIDRange.dwMin = m_ItemIDRange.dwMax = m_ItemIDRange.dwUsableItemIDMin = 0;
	m_ItemIDSpareRange.dwMin = m_ItemIDSpareRange.dwMax = m_ItemIDSpareRange.dwUsableItemIDMin = 0;
}

ITEM_MANAGER::~ITEM_MANAGER()
{
	Destroy();
}

void ITEM_MANAGER::Destroy()
{
	itertype(m_VIDMap) it = m_VIDMap.begin();
	for ( ; it != m_VIDMap.end(); ++it) {
		M2_DELETE(it->second);
	}
	m_VIDMap.clear();
}

void ITEM_MANAGER::GracefulShutdown()
{
	TR1_NS::unordered_set<LPITEM>::iterator it = m_set_pkItemForDelayedSave.begin();

	while (it != m_set_pkItemForDelayedSave.end())
		SaveSingleItem(*(it++));

	m_set_pkItemForDelayedSave.clear();
}

bool ITEM_MANAGER::Initialize(TItemTable * table, int size)
{
	if (!m_vec_prototype.empty())
		m_vec_prototype.clear();

	int	i;

	m_vec_prototype.resize(size);
	thecore_memcpy(&m_vec_prototype[0], table, sizeof(TItemTable) * size);
	for (int i = 0; i < size; i++)
	{
		if (0 != m_vec_prototype[i].dwVnumRange)
		{
			m_vec_item_vnum_range_info.push_back( &m_vec_prototype[i]);
		}
	}

	m_map_ItemRefineFrom.clear();
	for (i = 0; i < size; ++i)
	{

		if (m_vec_prototype[i].dwRefinedVnum)
			m_map_ItemRefineFrom.insert(std::make_pair(m_vec_prototype[i].dwRefinedVnum, m_vec_prototype[i].dwVnum));

		// NOTE : QUEST_GIVE The flag is npc Occurs as an event .
		if (m_vec_prototype[i].bType == ITEM_QUEST || IS_SET(m_vec_prototype[i].dwFlags, ITEM_FLAG_QUEST_USE | ITEM_FLAG_QUEST_USE_MULTIPLE))
			quest::CQuestManager::instance().RegisterNPCVnum(m_vec_prototype[i].dwVnum);

		m_map_vid.insert( std::map<DWORD,TItemTable>::value_type( m_vec_prototype[i].dwVnum, m_vec_prototype[i] ) ); 
		if ( test_server )
			sys_log( 0, "ITEM_INFO %d %s ", m_vec_prototype[i].dwVnum, m_vec_prototype[i].szName );	
	}

	int len = 0, len2;
	char buf[512];

	for (i = 0; i < size; ++i)
	{
		len2 = snprintf(buf + len, sizeof(buf) - len, "%5u %-16s", m_vec_prototype[i].dwVnum, m_vec_prototype[i].szName);

		if (len2 < 0 || len2 >= (int) sizeof(buf) - len)
			len += (sizeof(buf) - len) - 1;
		else
			len += len2;

		if (!((i + 1) % 4))
		{
			if ( !test_server )
				sys_log(0, "%s", buf);
			len = 0;
		}
		else
		{
			buf[len++] = '\t';
			buf[len] = '\0';
		}
	}

	if ((i + 1) % 4)
	{
		if ( !test_server )
			sys_log(0, "%s", buf);
	}

	ITEM_VID_MAP::iterator it = m_VIDMap.begin();

	sys_log (1, "ITEM_VID_MAP %d", m_VIDMap.size() );

	while (it != m_VIDMap.end())
	{
		LPITEM item = it->second;
		++it;

		const TItemTable* tableInfo = GetTable(item->GetVnum());

		if (NULL == tableInfo)
		{
			sys_err("cannot reset item table");
			item->SetProto(NULL);
		}

		item->SetProto(tableInfo);
	}

	return true;
}

LPITEM ITEM_MANAGER::CreateItem(DWORD vnum, DWORD count, DWORD id, bool bTryMagic, int iRarePct, bool bSkipSave)
{
	if (0 == vnum)
		return NULL;

	const TItemTable* table = GetTable(vnum);

	if (NULL == table)
		return NULL;

	LPITEM item = NULL;

	//id If it exists by checking with -- return ! 
	if (m_map_pkItemByID.find(id) != m_map_pkItemByID.end())
	{
		item = m_map_pkItemByID[id];
		LPCHARACTER owner = item->GetOwner();
		sys_err("ITEM_ID_DUP: %u %s owner %p", id, item->GetName(), get_pointer(owner));
		return NULL;
	}

	// Allocate an item
	item = M2_NEW CItem(vnum);

	bool bIsNewItem = (0 == id);

	// Initialize it . set a table
	item->Initialize();
	item->SetProto(table);

	if (item->GetType() == ITEM_ELK) // money ID No need and no need to save .
		item->SetSkipSave(true);

	// Unique ID Let's set
	else if (!bIsNewItem)
	{
		item->SetID(id);
		item->SetSkipSave(true);
	}
	else
	{
		item->SetID(GetNewID());

		if (item->GetType() == ITEM_UNIQUE) // Unique items record the remaining time in the socket when created. .
		{
			if (item->GetValue(2) == 0)
				item->SetSocket(ITEM_SOCKET_UNIQUE_REMAIN_TIME, item->GetValue(0)); // game time unique
			else
			{
				//int globalTime = get_global_time();
				//int lastTime = item->GetValue(0);
				//int endTime = get_global_time() + item->GetValue(0);
				item->SetSocket(ITEM_SOCKET_UNIQUE_REMAIN_TIME, get_global_time() + item->GetValue(0)); // Real-time unique
			}
		}
	}


	switch (item->GetVnum())
	{
		case ITEM_AUTO_HP_RECOVERY_S:
		case ITEM_AUTO_HP_RECOVERY_M:
		case ITEM_AUTO_HP_RECOVERY_L:
		case ITEM_AUTO_HP_RECOVERY_X:
		case ITEM_AUTO_SP_RECOVERY_S:
		case ITEM_AUTO_SP_RECOVERY_M:
		case ITEM_AUTO_SP_RECOVERY_L:
		case ITEM_AUTO_SP_RECOVERY_X:
		case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_XS:
		case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_S:
		case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_XS:
		case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_S:
			if (bIsNewItem)
				item->SetSocket(2, item->GetValue(0), true);
			else
				item->SetSocket(2, item->GetValue(0), false);
			break;
	}

	if (item->GetType() == ITEM_ELK) // Money needs no handling
		;
	else if (item->IsStackable())  // For items that can be combined
	{
		count = MINMAX(1, count, g_ItemCountLimit);

		if (bTryMagic && count <= 1 && IS_SET(item->GetFlag(), ITEM_FLAG_MAKECOUNT))
			count = item->GetValue(1);
	}
	else
		count = 1;

	item->SetVID(++m_dwVIDCount);

	if (bSkipSave == false)
		m_VIDMap.insert(ITEM_VID_MAP::value_type(item->GetVID(), item));

	if (item->GetID() != 0 && bSkipSave == false)
		m_map_pkItemByID.insert(std::map<DWORD, LPITEM>::value_type(item->GetID(), item));

	if (!item->SetCount(count))
		return NULL;

	item->SetSkipSave(false);

	if (item->GetType() == ITEM_UNIQUE && item->GetValue(2) != 0)
		item->StartUniqueExpireEvent();

	for (int i=0 ; i < ITEM_LIMIT_MAX_NUM ; i++)
	{
		// Time is deducted from the time an item is created even if it is not used.
		if (LIMIT_REAL_TIME == item->GetLimitType(i))
		{
			if (item->GetLimitValue(i))
			{
				item->SetSocket(0, time(0) + item->GetLimitValue(i));
			}
			else
			{
				item->SetSocket(0, time(0) + 60*60*24*7);
			}

			item->StartRealTimeExpireEvent();
		}

		// Like existing unique items, the usable time is deducted only when worn.
		else if (LIMIT_TIMER_BASED_ON_WEAR == item->GetLimitType(i))
		{
			// If the item is already being worn, the timer starts. , For new items you create, set the available time. . (
			// When paying through item mall, before entering this logic Socket0 The value must be set .
			if (true == item->IsEquipped())
			{
				item->StartTimerBasedOnWearExpireEvent();
			}
			else if(0 == id)
			{
				TimeT64 duration = item->GetSocket(0);
				if (0 == duration)
					duration = item->GetLimitValue(i);

				if (0 == duration)
					duration = 60 * 60 * 10;	// If there is no information, default 10 time setting

				item->SetSocket(0, duration);
			}
		}
	}

	if (id == 0) // Only processed when new items are created
	{
		// Newly added herbs perform differently.
		if (ITEM_BLEND==item->GetType())
		{
			if (Blend_Item_find(item->GetVnum()))
			{
				Blend_Item_set_value(item);
				return item;
			}
		}

		if (table->sAddonType)
		{
			item->ApplyAddon(table->sAddonType);
		}

		if (bTryMagic)
		{
			if (iRarePct == -1)
				iRarePct = table->bAlterToMagicItemPct;

			if (number(1, 100) <= iRarePct)
				item->AlterToMagicItem();
		}

		if (table->bGainSocketPct)
			item->AlterToSocketItem(table->bGainSocketPct);

		// 50300 == technical training book
		if (vnum == 50300 || vnum == ITEM_SKILLFORGET_VNUM)
		{
			DWORD dwSkillVnum;

			do
			{
				dwSkillVnum = number(1, 111);

				if (NULL != CSkillManager::instance().Get(dwSkillVnum))
					break;
			} while (true);

			item->SetSocket(0, dwSkillVnum);
		}
		else if (ITEM_SKILLFORGET2_VNUM == vnum)
		{
			DWORD dwSkillVnum;

			do
			{
				dwSkillVnum = number(112, 119);

				if (NULL != CSkillManager::instance().Get(dwSkillVnum))
					break;
			} while (true);

			item->SetSocket(0, dwSkillVnum);
		}
	}

	if (item->GetType() == ITEM_QUEST)
	{
		for (itertype (m_map_pkQuestItemGroup) it = m_map_pkQuestItemGroup.begin(); it != m_map_pkQuestItemGroup.end(); it++)
		{
			if (it->second->m_bType == CSpecialItemGroup::QUEST && it->second->Contains(vnum))
			{
				item->SetSIGVnum(it->first);
			}
		}
	}
	else if (item->GetType() == ITEM_UNIQUE)
	{
		for (itertype (m_map_pkSpecialItemGroup) it = m_map_pkSpecialItemGroup.begin(); it != m_map_pkSpecialItemGroup.end(); it++)
		{
			if (it->second->m_bType == CSpecialItemGroup::SPECIAL && it->second->Contains(vnum))
			{
				item->SetSIGVnum(it->first);
			}
		}
	}

	// Processing of newly created dragon soul stones .
	if (item->IsDragonSoul() && 0 == id)
	{
		DSManager::instance().DragonSoulItemInitialize(item);
	}
	return item;
}

void ITEM_MANAGER::DelayedSave(LPITEM item)
{
	if (item->GetID() != 0)
		m_set_pkItemForDelayedSave.insert(item);
}

void ITEM_MANAGER::FlushDelayedSave(LPITEM item)
{
	TR1_NS::unordered_set<LPITEM>::iterator it = m_set_pkItemForDelayedSave.find(item);

	if (it == m_set_pkItemForDelayedSave.end())
	{
		return;
	}

	m_set_pkItemForDelayedSave.erase(it);
	SaveSingleItem(item);
}

void ITEM_MANAGER::SaveSingleItem(LPITEM item)
{
	if (!item->GetOwner())
	{
		DWORD dwID = item->GetID();
		DWORD dwOwnerID = item->GetLastOwnerPID();

		db_clientdesc->DBPacketHeader(HEADER_GD_ITEM_DESTROY, 0, sizeof(DWORD) + sizeof(DWORD));
		db_clientdesc->Packet(&dwID, sizeof(DWORD));
		db_clientdesc->Packet(&dwOwnerID, sizeof(DWORD));

		sys_log(1, "ITEM_DELETE %s:%u", item->GetName(), dwID);
		return;
	}

	sys_log(1, "ITEM_SAVE %s:%d in %s window %d", item->GetName(), item->GetID(), item->GetOwner()->GetName(), item->GetWindow());

	TPlayerItem t;

	t.id = item->GetID();
	t.window = item->GetWindow();
	t.pos = t.window == EQUIPMENT ? item->GetCell() - INVENTORY_MAX_NUM : item->GetCell();
	t.count = item->GetCount();
	t.vnum = item->GetVnum();
	t.owner = (t.window == SAFEBOX || t.window == MALL) ? item->GetOwner()->GetDesc()->GetAccountTable().id : item->GetOwner()->GetPlayerID();
	thecore_memcpy(t.alSockets, item->GetSockets(), sizeof(t.alSockets));
	thecore_memcpy(t.aAttr, item->GetAttributes(), sizeof(t.aAttr));

	db_clientdesc->DBPacketHeader(HEADER_GD_ITEM_SAVE, 0, sizeof(TPlayerItem));
	db_clientdesc->Packet(&t, sizeof(TPlayerItem));
}

void ITEM_MANAGER::Update()
{
	TR1_NS::unordered_set<LPITEM>::iterator it = m_set_pkItemForDelayedSave.begin();
	TR1_NS::unordered_set<LPITEM>::iterator this_it;

	while (it != m_set_pkItemForDelayedSave.end())
	{
		this_it = it++;
		LPITEM item = *this_it;

		// SLOW_QUERY Anything with a flag is saved only on exit. .
		if (item->GetOwner() && IS_SET(item->GetFlag(), ITEM_FLAG_SLOW_QUERY))
			continue;

		SaveSingleItem(item);

		m_set_pkItemForDelayedSave.erase(this_it);
	}
}

void ITEM_MANAGER::RemoveItem(LPITEM item, const char * c_pszReason)
{
	LPCHARACTER o;

	if ((o = item->GetOwner()))
	{
		char szHint[64];
		snprintf(szHint, sizeof(szHint), "%s %u ", item->GetName(), item->GetCount());
		LogManager::instance().ItemLog(o, item, c_pszReason ? c_pszReason : "REMOVE", szHint);

		// SAFEBOX_TIME_LIMIT_ITEM_BUG_FIX
		if (item->GetWindow() == MALL || item->GetWindow() == SAFEBOX)
		{
			// 20050613.ipkn. If a time-limited item is in the store, the server will crash when the time expires. .
			CSafebox* pSafebox = item->GetWindow() == MALL ? o->GetMall() : o->GetSafebox();
			if (pSafebox)
			{
				pSafebox->Remove(item->GetCell());
			}
		}
		// END_OF_SAFEBOX_TIME_LIMIT_ITEM_BUG_FIX
		else
		{
			o->SyncQuickslot(QUICKSLOT_TYPE_ITEM, static_cast<BYTE>(item->GetCell()), 255);
			item->RemoveFromCharacter();
		}
	}

	M2_DESTROY_ITEM(item);
}

void ITEM_MANAGER::DestroyItem(LPITEM item)
{
	if (item->GetSectree())
		item->RemoveFromGround();

	if (item->GetOwner())
	{
		if (CHARACTER_MANAGER::instance().Find(item->GetOwner()->GetPlayerID()) != NULL)
		{
			sys_err("DestroyItem: GetOwner %s %s!!", item->GetName(), item->GetOwner()->GetName());
			item->RemoveFromCharacter();
		}
		else
		{
			sys_err ("WTH! Invalid item owner. owner pointer : %p", item->GetOwner());
		}
	}

	TR1_NS::unordered_set<LPITEM>::iterator it = m_set_pkItemForDelayedSave.find(item);

	if (it != m_set_pkItemForDelayedSave.end())
		m_set_pkItemForDelayedSave.erase(it);

	DWORD dwID = item->GetID();
	sys_log(2, "ITEM_DESTROY %s:%u", item->GetName(), dwID);

	if (!item->GetSkipSave() && dwID)
	{
		DWORD dwOwnerID = item->GetLastOwnerPID();

		db_clientdesc->DBPacketHeader(HEADER_GD_ITEM_DESTROY, 0, sizeof(DWORD) + sizeof(DWORD));
		db_clientdesc->Packet(&dwID, sizeof(DWORD));
		db_clientdesc->Packet(&dwOwnerID, sizeof(DWORD));
	}
	else
	{
		sys_log(2, "ITEM_DESTROY_SKIP %s:%u (skip=%d)", item->GetName(), dwID, item->GetSkipSave());
	}

	if (dwID)
		m_map_pkItemByID.erase(dwID);

	m_VIDMap.erase(item->GetVID());

	M2_DELETE(item);
}

LPITEM ITEM_MANAGER::Find(DWORD id)
{
	itertype(m_map_pkItemByID) it = m_map_pkItemByID.find(id);
	if (it == m_map_pkItemByID.end())
		return NULL;
	return it->second;
}

LPITEM ITEM_MANAGER::FindByVID(DWORD vid)
{
	ITEM_VID_MAP::iterator it = m_VIDMap.find(vid);

	if (it == m_VIDMap.end())
		return NULL;

	return (it->second);
}

TItemTable * ITEM_MANAGER::GetTable(DWORD vnum)
{
	int rnum = RealNumber(vnum);

	if (rnum < 0)
	{
		for (int i = 0; i < m_vec_item_vnum_range_info.size(); i++)
		{
			TItemTable* p = m_vec_item_vnum_range_info[i];
			if ((p->dwVnum < vnum) &&
				vnum < (p->dwVnum + p->dwVnumRange))
			{
				return p;
			}
		}
			
		return NULL;
	}

	return &m_vec_prototype[rnum];
}

int ITEM_MANAGER::RealNumber(DWORD vnum)
{
	int bot, top, mid;

	bot = 0;
	top = static_cast<int>(m_vec_prototype.size());

	TItemTable * pTable = &m_vec_prototype[0];

	while (1)
	{
		mid = (bot + top) >> 1;

		if ((pTable + mid)->dwVnum == vnum)
			return (mid);

		if (bot >= top)
			return (-1);

		if ((pTable + mid)->dwVnum > vnum)
			top = mid - 1;
		else        
			bot = mid + 1;
	}
}

bool ITEM_MANAGER::IsItemMetin(const DWORD& vnum)
{
	const TItemTable* p = GetTable(vnum);
	return (p && p->bType == ITEM_METIN);
}

bool ITEM_MANAGER::GetVnum(const char * c_pszName, DWORD & r_dwVnum)
{
	int len = static_cast<int>(strlen(c_pszName));

	TItemTable * pTable = &m_vec_prototype[0];

	for (DWORD i = 0; i < m_vec_prototype.size(); ++i, ++pTable)
	{
		if (!_strnicmp(c_pszName, pTable->szName, len))
		{
			r_dwVnum = pTable->dwVnum;
			return true;
		}
	}

	return false;
}

bool ITEM_MANAGER::GetVnumByOriginalName(const char * c_pszName, DWORD & r_dwVnum)
{
	int len = static_cast<int>(strlen(c_pszName));

	TItemTable * pTable = &m_vec_prototype[0];

	for (DWORD i = 0; i < m_vec_prototype.size(); ++i, ++pTable)
	{
		if (!_strnicmp(c_pszName, pTable->szName, len))
		{
			r_dwVnum = pTable->dwVnum;
			return true;
		}
	}

	return false;
}
// 20050503.ipkn.
// iMinimum If it is less than iDefault Setting ( step , iMinimum silver 0 Must be greater than )
// 1, 0 in this way ON/OFF exists to support the way
int GetDropPerKillPct(int iMinimum, int iDefault, int iDeltaPercent, const char * c_pszFlag)
{
	int iVal = 0;

	if ((iVal = quest::CQuestManager::instance().GetEventFlag(c_pszFlag)))
	{
		if (!test_server)
		{
			if (iVal < iMinimum)
				iVal = iDefault;

			if (iVal < 0)
				iVal = iDefault;
		}
	}

	if (iVal == 0)
		return 0;

	// When in basic settings (iDeltaPercent=100) 
	// 40000 iVal It is a constant to give the feeling of one per animal.
	return (40000 * iDeltaPercent / iVal);
}

bool ITEM_MANAGER::GetDropPct(LPCHARACTER pkChr, LPCHARACTER pkKiller, OUT int& iDeltaPercent, OUT int& iRandRange)
{
 	if (NULL == pkChr || NULL == pkKiller)
		return false;

	int iLevel = pkKiller->GetLevel();
	iDeltaPercent = 100;

	if (!pkChr->IsStone() && pkChr->GetMobRank() >= MOB_RANK_BOSS)
		iDeltaPercent = PERCENT_LVDELTA_BOSS(pkKiller->GetLevel(), pkChr->GetLevel());
	else
		iDeltaPercent = PERCENT_LVDELTA(pkKiller->GetLevel(), pkChr->GetLevel());

	BYTE bRank = pkChr->GetMobRank();

	if (1 == number(1, 50000))
		iDeltaPercent += 1000;
	else if (1 == number(1, 10000))
		iDeltaPercent += 500;

	sys_log(3, "CreateDropItem for level: %d rank: %u pct: %d", iLevel, bRank, iDeltaPercent);
	iDeltaPercent = iDeltaPercent * CHARACTER_MANAGER::instance().GetMobItemRate(pkKiller) / 100;

	//if (pkKiller->GetPoint(POINT_MALL_ITEMBONUS) > 0)
	//iDeltaPercent += iDeltaPercent * pkKiller->GetPoint(POINT_MALL_ITEMBONUS) / 100;
	// ADD_PREMIUM
	if (pkKiller->GetPremiumRemainSeconds(PREMIUM_ITEM) > 0 ||
			pkKiller->IsEquipUniqueGroup(UNIQUE_GROUP_DOUBLE_ITEM))
		iDeltaPercent += iDeltaPercent;
	// END_OF_ADD_PREMIUM

	iRandRange = 4000000;
	iRandRange = iRandRange * 100 / 
		(100 + 
		 (CPrivManager::instance().GetPriv(pkKiller, PRIV_ITEM_DROP) + 
			 pkKiller->IsEquipUniqueItem(UNIQUE_ITEM_DOUBLE_ITEM))?100:0);

		return true;
	}

	bool ITEM_MANAGER::CreateDropItemVector(LPCHARACTER pkChr, LPCHARACTER pkKiller, std::vector<std::pair<int, int>>& vec_item)
	{
		if (!pkChr || !pkKiller || pkChr->IsPolymorphed() || pkChr->IsPC())
			return false;

		vec_item.clear();

		int iLevel = pkKiller->GetLevel();
		BYTE bRank = pkChr->GetMobRank();

		// Common Drop Items
		if (bRank < MOB_RANK_MAX_NUM)
		{
			for (const auto& info : g_vec_pkCommonDropItem[bRank])
			{
				if (info.m_bJob > 0 && info.m_bJob != pkKiller->GetJob() + 1)
					continue;

				for (DWORD vnum = info.m_dwVnumStart; vnum <= info.m_dwVnumEnd; ++vnum)
				{
					vec_item.emplace_back(vnum, info.m_bCount);
				}
			}
		}

		// Drop Item Group
		if (auto it = m_map_pkDropItemGroup.find(pkChr->GetRaceNum()); it != m_map_pkDropItemGroup.end())
		{
			const auto& items = it->second->GetVector();
			for (const auto& item : items)
			{
				for (DWORD vnum = item.dwVnumStart; vnum <= item.dwVnumEnd; ++vnum)
				{
					vec_item.emplace_back(vnum, item.iCount);
				}
			}
		}

		// Mob Item Group
		if (auto it = m_map_pkMobItemGroup.find(pkChr->GetRaceNum()); it != m_map_pkMobItemGroup.end())
		{
			for (const auto* pGroup : it->second)
			{
				if (pGroup && !pGroup->IsEmpty())
				{
					auto groupItems = pGroup->GetItemVector();
					for (const auto& item : groupItems)
					{
						for (DWORD vnum = item.dwItemVnumStart; vnum <= item.dwItemVnumEnd; ++vnum)
							vec_item.emplace_back(vnum, item.iCount);
					}
				}
			}
		}

		if (auto it = m_map_pkLevelItemGroup.find(pkChr->GetRaceNum()); it != m_map_pkLevelItemGroup.end())
		{
			const auto& items = it->second->GetVector();
			for (const auto& item : items)
			{
				for (DWORD vnum = item.dwVNumStart; vnum <= item.dwVNumEnd; ++vnum)
				{
					vec_item.emplace_back(vnum, item.iCount);
				}
			}
		}

		if (pkChr->GetMobDropItemVnum())
		{
			if (m_map_dwEtcItemDropProb.find(pkChr->GetMobDropItemVnum()) != m_map_dwEtcItemDropProb.end())
				vec_item.emplace_back(pkChr->GetMobDropItemVnum(), 1);
		}

		if (pkChr->IsStone() && pkChr->GetDropMetinStoneVnum())
		{
			vec_item.emplace_back(pkChr->GetDropMetinStoneVnum(), 1);
		}
	 
		std::sort(vec_item.begin(), vec_item.end());
		vec_item.erase(std::unique(vec_item.begin(), vec_item.end()), vec_item.end());
		std::stable_sort(vec_item.begin(), vec_item.end(), [](const std::pair<int, int>& f, const std::pair<int, int>& s) {
			return ITEM_MANAGER::instance().IsItemMetin(f.first) > ITEM_MANAGER::instance().IsItemMetin(s.first);
			});

		return !vec_item.empty();
	}

	bool ITEM_MANAGER::CreateDropItem(LPCHARACTER pkChr, LPCHARACTER pkKiller, std::vector<LPITEM>& vec_item)
	{
		if (!pkChr || !pkKiller) return false;

		int iLevel = pkKiller->GetLevel();
		int iDeltaPercent = 100, iRandRange = 4000000;

		if (!GetDropPct(pkChr, pkKiller, iDeltaPercent, iRandRange))
			return false;

		BYTE bRank = pkChr->GetMobRank();

		// Common Drop Items
		if (bRank < MOB_RANK_MAX_NUM)
		{
			for (const auto& info : g_vec_pkCommonDropItem[bRank])
			{
				if (iLevel < info.m_iLevelStart || iLevel > info.m_iLevelEnd) continue;
				if (info.m_bJob > 0 && info.m_bJob != pkKiller->GetJob() + 1) continue;

				int iPercent = (info.m_iPercent * iDeltaPercent) / 100;
				if (iPercent >= number(1, iRandRange))
				{
					DWORD dwVnum = number(info.m_dwVnumStart, info.m_dwVnumEnd);
					if (LPITEM item = CreateItem(dwVnum, info.m_bCount, 0, true))
					{
						if (item->GetType() == ITEM_POLYMORPH && dwVnum == pkChr->GetPolymorphItemVnum())
							item->SetSocket(0, pkChr->GetRaceNum());

						vec_item.push_back(item);
					}
				}
			}
		}

		// Drop Item Group
		if (auto it = m_map_pkDropItemGroup.find(pkChr->GetRaceNum()); it != m_map_pkDropItemGroup.end())
		{
			for (const auto& info : it->second->GetVector())
			{
				int iPercent = (info.dwPct * iDeltaPercent) / 100;
				if (iPercent >= number(1, iRandRange))
				{
					DWORD dwVnum = number(info.dwVnumStart, info.dwVnumEnd);
					if (LPITEM item = CreateItem(dwVnum, info.iCount, 0, true))
					{
						if (item->GetType() == ITEM_POLYMORPH && item->GetVnum() == pkChr->GetPolymorphItemVnum())
							item->SetSocket(0, pkChr->GetRaceNum());

						vec_item.push_back(item);
					}
				}
			}
		}

		// Mob Item Group
		if (auto it = m_map_pkMobItemGroup.find(pkChr->GetRaceNum()); it != m_map_pkMobItemGroup.end())
		{
			for (auto* pGroup : it->second)
			{
				if (pGroup && !pGroup->IsEmpty())
				{
					int iPercent = 10000 * iDeltaPercent / pGroup->GetKillPerDrop();
					if (iPercent >= number(1, iRandRange))
					{
						const auto& info = pGroup->GetOne();
						DWORD dwVnum = number(info.dwItemVnumStart, info.dwItemVnumEnd);
						if (LPITEM item = CreateItem(dwVnum, info.iCount, 0, true, info.iRarePct))
							vec_item.push_back(item);
					}
				}
			}
		}

		// Level Item Group
		if (auto it = m_map_pkLevelItemGroup.find(pkChr->GetRaceNum()); it != m_map_pkLevelItemGroup.end())
		{
			if (it->second->GetLevelLimitStart() <= (DWORD)iLevel && it->second->GetLevelLimitEnd() >= (DWORD)iLevel)
			{
				for (const auto& info : it->second->GetVector())
				{
					if (info.dwPct >= (DWORD)number(1, 1000000))
					{
						DWORD dwVnum = number(info.dwVNumStart, info.dwVNumEnd);
						if (LPITEM item = CreateItem(dwVnum, info.iCount, 0, true))
							vec_item.push_back(item);
					}
				}
			}
		}

		// ETC
		if (DWORD vnum = pkChr->GetMobDropItemVnum(); vnum != 0)
		{
			if (auto it = m_map_dwEtcItemDropProb.find(vnum); it != m_map_dwEtcItemDropProb.end())
			{
				int iPercent = (it->second * iDeltaPercent) / 100;
				if (iPercent >= number(1, iRandRange))
				{
					if (LPITEM item = CreateItem(vnum, 1, 0, true))
						vec_item.push_back(item);
				}
			}
		}

		// Stone
		if (pkChr->IsStone() && pkChr->GetDropMetinStoneVnum())
		{
			int iPercent = (pkChr->GetDropMetinStonePct() * iDeltaPercent) * 400;
			if (iPercent >= number(1, iRandRange))
			{
				if (LPITEM item = CreateItem(pkChr->GetDropMetinStoneVnum(), 1, 0, true))
					vec_item.push_back(item);
			}
		}

		if (pkKiller->IsHorseRiding() && GetDropPerKillPct(1000, 1000000, iDeltaPercent, "horse_skill_book_drop") >= number(1, iRandRange))
		{
			if (LPITEM item = CreateItem(ITEM_HORSE_SKILL_TRAIN_BOOK, 1, 0, true))
				vec_item.push_back(item);
		}

		CreateQuestDropItem(pkChr, pkKiller, vec_item, iDeltaPercent, iRandRange);

		for (const auto& item : vec_item)
			DBManager::instance().SendMoneyLog(MONEY_LOG_DROP, item->GetVnum(), item->GetCount());

		return !vec_item.empty();
	}

	// ADD_GRANDMASTER_SKILL
	int GetThreeSkillLevelAdjust(int level)
	{
		if (level < 40)
			return 32;
		if (level < 45)
			return 16;
		if (level < 50)
			return 8;
		if (level < 55)
			return 4;
		if (level < 60)
			return 2;
		return 1;
	}
	// END_OF_ADD_GRANDMASTER_SKILL

	// DROPEVENT_CHARSTONE
	// drop_char_stone 1
	// drop_char_stone.percent_lv01_10 5
	// drop_char_stone.percent_lv11_30 10
	// drop_char_stone.percent_lv31_MX 15
	// drop_char_stone.level_range	   10
	static struct DropEvent_CharStone
	{
		int percent_lv01_10;
		int percent_lv11_30;
		int percent_lv31_MX;
		int level_range;
		bool alive;

		DropEvent_CharStone()
		{
			percent_lv01_10 =  100;
			percent_lv11_30 =  200;
			percent_lv31_MX =  300;
			level_range = 10;
			alive = false;
		}
	} gs_dropEvent_charStone;

	static int __DropEvent_CharStone_GetDropPercent(int killer_level)
	{
		int killer_levelStep = (killer_level-1)/10;

		switch (killer_levelStep)
		{
			case 0:
				return gs_dropEvent_charStone.percent_lv01_10;

			case 1:
			case 2:
				return gs_dropEvent_charStone.percent_lv11_30;
		}

		return gs_dropEvent_charStone.percent_lv31_MX;
	}

	static void __DropEvent_CharStone_DropItem(CHARACTER & killer, CHARACTER & victim, ITEM_MANAGER& itemMgr, std::vector<LPITEM>& vec_item)
	{
		if (!gs_dropEvent_charStone.alive)
			return;

		int killer_level = killer.GetLevel();
		int dropPercent = __DropEvent_CharStone_GetDropPercent(killer_level);

		int MaxRange = 10000;

		if (number(1, MaxRange) <= dropPercent)
		{
			int log_level = (test_server || killer.GetGMLevel() >= GM_LOW_WIZARD) ? 0 : 1;
			int victim_level = victim.GetLevel();
			int level_diff = victim_level - killer_level;

			if (level_diff >= +gs_dropEvent_charStone.level_range || level_diff <= -gs_dropEvent_charStone.level_range)
			{
				sys_log(log_level, 
						"dropevent.drop_char_stone.level_range_over: killer(%s: lv%d), victim(%s: lv:%d), level_diff(%d)",
						killer.GetName(), killer.GetLevel(), victim.GetName(), victim.GetLevel(), level_diff);	
				return;
			}

			static const int Stones[] = { 30210, 30211, 30212, 30213, 30214, 30215, 30216, 30217, 30218, 30219, 30258, 30259, 30260, 30261, 30262, 30263 };
			int item_vnum = Stones[number(0, _countof(Stones))];

			LPITEM p_item = NULL;

			if ((p_item = itemMgr.CreateItem(item_vnum, 1, 0, true)))
			{
				vec_item.push_back(p_item);

				sys_log(log_level, 
						"dropevent.drop_char_stone.item_drop: killer(%s: lv%d), victim(%s: lv:%d), item_name(%s)",
						killer.GetName(), killer.GetLevel(), victim.GetName(), victim.GetLevel(), p_item->GetName());	
			}
		}
	}

	bool DropEvent_CharStone_SetValue(const std::string& name, int value)
	{
		if (name == "drop_char_stone")
		{
			gs_dropEvent_charStone.alive = value;

			if (value)
				sys_log(0, "dropevent.drop_char_stone = on");
			else
				sys_log(0, "dropevent.drop_char_stone = off");

		}
		else if (name == "drop_char_stone.percent_lv01_10")
			gs_dropEvent_charStone.percent_lv01_10 = value;
		else if (name == "drop_char_stone.percent_lv11_30")
			gs_dropEvent_charStone.percent_lv11_30 = value;
		else if (name == "drop_char_stone.percent_lv31_MX")
			gs_dropEvent_charStone.percent_lv31_MX = value;
		else if (name == "drop_char_stone.level_range")
			gs_dropEvent_charStone.level_range = value;
		else
			return false;

		sys_log(0, "dropevent.drop_char_stone: %d", gs_dropEvent_charStone.alive ? true : false);
		sys_log(0, "dropevent.drop_char_stone.percent_lv01_10: %f", gs_dropEvent_charStone.percent_lv01_10/100.0f);
		sys_log(0, "dropevent.drop_char_stone.percent_lv11_30: %f", gs_dropEvent_charStone.percent_lv11_30/100.0f);
		sys_log(0, "dropevent.drop_char_stone.percent_lv31_MX: %f", gs_dropEvent_charStone.percent_lv31_MX/100.0f);
		sys_log(0, "dropevent.drop_char_stone.level_range: %d", gs_dropEvent_charStone.level_range);

		return true;
	}

	// END_OF_DROPEVENT_CHARSTONE

	// fixme
	// with the above quest Let's take out what to take out. . 
	// This is so dirty ...
	// Wow .. I hate hard coding T_T
	// Quantity item rewards start .
	// by rtsummit Let's fix it, really
	static struct DropEvent_RefineBox
	{
		int percent_low;
		int low;
		int percent_mid;
		int mid;
		int percent_high;
		//int level_range;
		bool alive;

		DropEvent_RefineBox()
		{
			percent_low =  100;
			low = 20;
			percent_mid =  100;
			mid = 45;
			percent_high =  100;
			//level_range = 10;
			alive = false;
		}
	} gs_dropEvent_refineBox;

	static LPITEM __DropEvent_RefineBox_GetDropItem(CHARACTER & killer, CHARACTER & victim, ITEM_MANAGER& itemMgr)
	{
		static const int lowerBox[] = { 50197, 50198, 50199 };
		static const int lowerBox_range = 3;
		static const int midderBox[] = { 50203, 50204, 50205, 50206 };
		static const int midderBox_range = 4;
		static const int higherBox[] = { 50207, 50208, 50209, 50210, 50211 };
		static const int higherBox_range = 5;

		if (victim.GetMobRank() < MOB_RANK_KNIGHT)
			return NULL;

		int killer_level = killer.GetLevel();
		//int level_diff = victim_level - killer_level;

		//if (level_diff >= +gs_dropEvent_refineBox.level_range || level_diff <= -gs_dropEvent_refineBox.level_range)
		//{
		//	sys_log(log_level, 
		//		"dropevent.drop_refine_box.level_range_over: killer(%s: lv%d), victim(%s: lv:%d), level_diff(%d)",
		//		killer.GetName(), killer.GetLevel(), victim.GetName(), victim.GetLevel(), level_diff);	
		//	return NULL;
		//}

		if (killer_level <= gs_dropEvent_refineBox.low)
		{
			if (number (1, gs_dropEvent_refineBox.percent_low) == 1)
			{
				return itemMgr.CreateItem(lowerBox [number (1,lowerBox_range) - 1], 1, 0, true);
			}
		}
		else if (killer_level <= gs_dropEvent_refineBox.mid)
		{
			if (number (1, gs_dropEvent_refineBox.percent_mid) == 1)
			{
				return itemMgr.CreateItem(midderBox [number (1,midderBox_range) - 1], 1, 0, true);
			}
		}
		else
		{
			if (number (1, gs_dropEvent_refineBox.percent_high) == 1)
			{
				return itemMgr.CreateItem(higherBox [number (1,higherBox_range) - 1], 1, 0, true);
			}
		}
		return NULL;
	}

	static void __DropEvent_RefineBox_DropItem(CHARACTER & killer, CHARACTER & victim, ITEM_MANAGER& itemMgr, std::vector<LPITEM>& vec_item)
	{
		if (!gs_dropEvent_refineBox.alive)
			return;

		int log_level = (test_server || killer.GetGMLevel() >= GM_LOW_WIZARD) ? 0 : 1;

		LPITEM p_item = __DropEvent_RefineBox_GetDropItem(killer, victim, itemMgr);

		if (p_item)
		{
			vec_item.push_back(p_item);

			sys_log(log_level, 
				"dropevent.drop_refine_box.item_drop: killer(%s: lv%d), victim(%s: lv:%d), item_name(%s)",
				killer.GetName(), killer.GetLevel(), victim.GetName(), victim.GetLevel(), p_item->GetName());	
		}
	}

	bool DropEvent_RefineBox_SetValue(const std::string& name, int value)
	{
		if (name == "refine_box_drop")
		{
			gs_dropEvent_refineBox.alive = value;

			if (value)
				sys_log(0, "refine_box_drop = on");
			else
				sys_log(0, "refine_box_drop = off");

		}
		else if (name == "refine_box_low")
			gs_dropEvent_refineBox.percent_low = value < 100 ? 100 : value;
		else if (name == "refine_box_mid")
			gs_dropEvent_refineBox.percent_mid = value < 100 ? 100 : value;
		else if (name == "refine_box_high")
			gs_dropEvent_refineBox.percent_high = value < 100 ? 100 : value;
		//else if (name == "refine_box_level_range")
		//	gs_dropEvent_refineBox.level_range = value;
		else
			return false;

		sys_log(0, "refine_box_drop: %d", gs_dropEvent_refineBox.alive ? true : false);
		sys_log(0, "refine_box_low: %d", gs_dropEvent_refineBox.percent_low);
		sys_log(0, "refine_box_mid: %d", gs_dropEvent_refineBox.percent_mid);
		sys_log(0, "refine_box_high: %d", gs_dropEvent_refineBox.percent_high);
		//sys_log(0, "refine_box_low_level_range: %d", gs_dropEvent_refineBox.level_range);

		return true;
	}
	// End of improvement item reward .


	void ITEM_MANAGER::CreateQuestDropItem(LPCHARACTER pkChr, LPCHARACTER pkKiller, std::vector<LPITEM> & vec_item, int iDeltaPercent, int iRandRange)
	{
		LPITEM item = NULL;

		if (!pkChr)
			return;

		if (!pkKiller)
			return;

		sys_log(1, "CreateQuestDropItem victim(%s), killer(%s)", pkChr->GetName(), pkKiller->GetName() );

		// DROPEVENT_CHARSTONE
		__DropEvent_CharStone_DropItem(*pkKiller, *pkChr, *this, vec_item);
		// END_OF_DROPEVENT_CHARSTONE
		__DropEvent_RefineBox_DropItem(*pkKiller, *pkChr, *this, vec_item);

		// christmas socks
		if (quest::CQuestManager::instance().GetEventFlag("xmas_sock"))
		{
			const DWORD SOCK_ITEM_VNUM = 50010;

			int iDropPerKill[MOB_RANK_MAX_NUM] =
			{
				2000,
				1000,
				300,
				50,
				0,
				0,
			};

			if ( iDropPerKill[pkChr->GetMobRank()] != 0 )
			{
				int iPercent = 40000 * iDeltaPercent / iDropPerKill[pkChr->GetMobRank()];

				sys_log(0, "SOCK DROP %d %d", iPercent, iRandRange);
				if (iPercent >= number(1, iRandRange))
				{
					if ((item = CreateItem(SOCK_ITEM_VNUM, 1, 0, true)))
						vec_item.push_back(item);
				}
			}
		}

		// moonlight
		if (quest::CQuestManager::instance().GetEventFlag("drop_moon"))
		{
			const DWORD ITEM_VNUM = 50011;

			int iDropPerKill[MOB_RANK_MAX_NUM] =
			{
				2000,
				1000,
				300,
				50,
				0,
				0,
			};

			if (iDropPerKill[pkChr->GetMobRank()])
			{
				int iPercent = 40000 * iDeltaPercent / iDropPerKill[pkChr->GetMobRank()];

				if (iPercent >= number(1, iRandRange))
				{
					if ((item = CreateItem(ITEM_VNUM, 1, 0, true)))
						vec_item.push_back(item);
				}
			}
		}

		if (pkKiller->GetLevel() >= 15 && abs(pkKiller->GetLevel() - pkChr->GetLevel()) <= 5)
		{
			int pct = quest::CQuestManager::instance().GetEventFlag("hc_drop");

			if (pct > 0)
			{
				const DWORD ITEM_VNUM = 30178;

				if (number(1,100) <= pct)
				{
					if ((item = CreateItem(ITEM_VNUM, 1, 0, true)))
						vec_item.push_back(item);
				}
			}
		}

		if (GetDropPerKillPct(100, 800, iDeltaPercent, "2006_drop") >= number(1, iRandRange))
		{
			sys_log(0, "HEXAGONAL_BOX DROP EVENT ");

			const static DWORD dwVnum = 50037;

			if ((item = CreateItem(dwVnum, 1, 0, true)))
				vec_item.push_back(item);

		}

		// Hexagonal Box Event
		if (GetDropPerKillPct(100, 800, iDeltaPercent, "2007_drop") >= number(1, iRandRange))
		{
			sys_log(0, "HEXAGONAL_BOX DROP EVENT ");

			const static DWORD dwVnum = 50043;

			if ((item = CreateItem(dwVnum, 1, 0, true)))
				vec_item.push_back(item);
		}

		// New year Firework Event
		if (GetDropPerKillPct(/* minimum */ 100, /* default */ 1000, iDeltaPercent, "newyear_fire") >= number(1, iRandRange))
		{
			const DWORD ITEM_VNUM_FIRE = 50107;

			if ((item = CreateItem(ITEM_VNUM_FIRE, 1, 0, true)))
				vec_item.push_back(item);
		}

		// Lunar New Year Wonso Event
		if (GetDropPerKillPct(100, 500, iDeltaPercent, "newyear_moon") >= number(1, iRandRange))
		{
			sys_log(0, "EVENT NEWYEAR_MOON DROP");

			const static DWORD wonso_items[6] = { 50016, 50017, 50018, 50019, 50019, 50019, };
			DWORD dwVnum = wonso_items[number(0,5)];

			if ((item = CreateItem(dwVnum, 1, 0, true)))
				vec_item.push_back(item);
		}

		// Valentine's Day Event. Changed min value to 1 per OGE request. (Other events left as is for now.)
		if (GetDropPerKillPct(1, 800, iDeltaPercent, "valentine_drop") >= number(1, iRandRange))
		{
		sys_log(0, "EVENT VALENTINE_DROP");

		const static DWORD valentine_items[2] = { 50024, 50025 };
		DWORD dwVnum = valentine_items[number(0, 1)];

		if ((item = CreateItem(dwVnum, 1, 0, true)))
			vec_item.push_back(item);
	}

	// ice cream event
	if (GetDropPerKillPct(100, 800, iDeltaPercent, "icecream_drop") >= number(1, iRandRange))
	{
		const static DWORD icecream = 50123;

		if ((item = CreateItem(icecream, 1, 0, true)))
			vec_item.push_back(item);
	}

	// new christmas event
	// 53002 : Baby Reindeer Summon Ticket
	if ((pkKiller->CountSpecifyItem(53002) > 0) && (GetDropPerKillPct(50, 100, iDeltaPercent, "new_xmas_event") >= number(1, iRandRange)))
	{
		const static DWORD xmas_sock = 50010;
		pkKiller->AutoGiveItem (xmas_sock, 1);
	}

	if ((pkKiller->CountSpecifyItem(53007) > 0) && (GetDropPerKillPct(50, 100, iDeltaPercent, "new_xmas_event") >= number(1, iRandRange)))
	{
		const static DWORD xmas_sock = 50010;
		pkKiller->AutoGiveItem (xmas_sock, 1);
	}

	//if (pkChr->GetLevel() >= 30 && (GetDropPerKillPct(50, 100, iDeltaPercent, "ds_drop") >= number(1, iRandRange)))
	//{
	//	const static DWORD dragon_soul_gemstone = 30270;
	//	if ((item = CreateItem(dragon_soul_gemstone, 1, 0, true)))
	//		vec_item.push_back(item);
	//}

	if ( GetDropPerKillPct(100, 800, iDeltaPercent, "halloween_drop") >= number(1, iRandRange) )
	{
		const static DWORD halloween_item = 30321;

		if ( (item=CreateItem(halloween_item, 1, 0, true)) )
			vec_item.push_back(item);
	}
	
	if ( GetDropPerKillPct(100, 800, iDeltaPercent, "ramadan_drop") >= number(1, iRandRange) )
	{
		const static DWORD ramadan_item = 30315;

		if ( (item=CreateItem(ramadan_item, 1, 0, true)) )
			vec_item.push_back(item);
	}

	if ( GetDropPerKillPct(100, 800, iDeltaPercent, "easter_drop") >= number(1, iRandRange) )
	{
		const static DWORD easter_item_base = 50160;

		if ( (item=CreateItem(easter_item_base+number(0,19), 1, 0, true)) )
			vec_item.push_back(item);
	}

	// world cup event
	if ( GetDropPerKillPct(100, 800, iDeltaPercent, "football_drop") >= number(1, iRandRange) )
	{
		const static DWORD football_item = 50096;

		if ( (item=CreateItem(football_item, 1, 0, true)) )
			vec_item.push_back(item);
	}

	// white day event
	if (GetDropPerKillPct(100, 800, iDeltaPercent, "whiteday_drop") >= number(1, iRandRange))
	{
		sys_log(0, "EVENT WHITEDAY_DROP");
		const static DWORD whiteday_items[2] = { ITEM_WHITEDAY_ROSE, ITEM_WHITEDAY_CANDY };
		DWORD dwVnum = whiteday_items[number(0,1)];

		if ((item = CreateItem(dwVnum, 1, 0, true)))
			vec_item.push_back(item);
	}

	// Children's Day Mystery Box Event
	if (pkKiller->GetLevel()>=50)
	{
		if (GetDropPerKillPct(100, 1000, iDeltaPercent, "kids_day_drop_high") >= number(1, iRandRange))
		{
			DWORD ITEM_QUIZ_BOX = 50034;

			if ((item = CreateItem(ITEM_QUIZ_BOX, 1, 0, true)))
				vec_item.push_back(item);
		}
	}
	else
	{
		if (GetDropPerKillPct(100, 1000, iDeltaPercent, "kids_day_drop") >= number(1, iRandRange))
		{
			DWORD ITEM_QUIZ_BOX = 50034;

			if ((item = CreateItem(ITEM_QUIZ_BOX, 1, 0, true)))
				vec_item.push_back(item);
		}
	}

	// olympic drop event
	if (pkChr->GetLevel() >= 30 && GetDropPerKillPct(50, 100, iDeltaPercent, "medal_part_drop") >= number(1, iRandRange))
	{
		const static DWORD drop_items[] = { 30265, 30266, 30267, 30268, 30269 };
		int i = number (0, 4);
		item = CreateItem(drop_items[i]);
		if (item != NULL)
			vec_item.push_back(item);
	}

	// ADD_GRANDMASTER_SKILL
	// Sobite item drop
	if (pkChr->GetLevel() >= 40 && pkChr->GetMobRank() >= MOB_RANK_BOSS && GetDropPerKillPct(/* minimum */ 1, /* default */ 1000, iDeltaPercent, "three_skill_item") / GetThreeSkillLevelAdjust(pkChr->GetLevel()) >= number(1, iRandRange))
	{
		const DWORD ITEM_VNUM = 50513;

		if ((item = CreateItem(ITEM_VNUM, 1, 0, true)))
			vec_item.push_back(item);
	}
	// END_OF_ADD_GRANDMASTER_SKILL

	//
	// seed item drop
	//
	if (GetDropPerKillPct(100, 1000, iDeltaPercent, "dragon_boat_festival_drop") >= number(1, iRandRange))
	{
		const DWORD ITEM_SEED = 50085;

		if ((item = CreateItem(ITEM_SEED, 1, 0, true)))
			vec_item.push_back(item);
	}

	// War God's Blessing for Mannyeon Hancheol drop
	if (pkKiller->GetLevel() >= 15 && quest::CQuestManager::instance().GetEventFlag("mars_drop"))
	{
		const DWORD ITEM_HANIRON = 70035;
		int iDropMultiply[MOB_RANK_MAX_NUM] =
		{
			50,
			30,
			5,
			1,
			0,
			0,
		};

		if (iDropMultiply[pkChr->GetMobRank()] &&
				GetDropPerKillPct(1000, 1500, iDeltaPercent, "mars_drop") >= number(1, iRandRange) * iDropMultiply[pkChr->GetMobRank()])
		{
			if ((item = CreateItem(ITEM_HANIRON, 1, 0, true)))
				vec_item.push_back(item);
		}
	}
}

DWORD ITEM_MANAGER::GetRefineFromVnum(DWORD dwVnum)
{
	itertype(m_map_ItemRefineFrom) it = m_map_ItemRefineFrom.find(dwVnum);
	if (it != m_map_ItemRefineFrom.end())
		return it->second;
	return 0;
}

const CSpecialItemGroup* ITEM_MANAGER::GetSpecialItemGroup(DWORD dwVnum)
{
	itertype(m_map_pkSpecialItemGroup) it = m_map_pkSpecialItemGroup.find(dwVnum);
	if (it != m_map_pkSpecialItemGroup.end())
	{
		return it->second;
	}
	return NULL;
}

const CSpecialAttrGroup* ITEM_MANAGER::GetSpecialAttrGroup(DWORD dwVnum)
{
	itertype(m_map_pkSpecialAttrGroup) it = m_map_pkSpecialAttrGroup.find(dwVnum);
	if (it != m_map_pkSpecialAttrGroup.end())
	{
		return it->second;
	}
	return NULL;
}

// pkNewItem A function that returns all properties and socket values ​​to .
// Previously char_item.cpp Local function in the file TransformRefineItem Copy as is
void ITEM_MANAGER::CopyAllAttrTo(LPITEM pkOldItem, LPITEM pkNewItem)
{
	// ACCESSORY_REFINE
	if (pkOldItem->IsAccessoryForSocket())
	{
		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			pkNewItem->SetSocket(i, pkOldItem->GetSocket(i));
		}
		//pkNewItem->StartAccessorySocketExpireEvent();
	}
	// END_OF_ACCESSORY_REFINE
	else
	{
		// Here, broken stones are automatically cleaned.
		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			if (!pkOldItem->GetSocket(i))
				break;
			else
				pkNewItem->SetSocket(i, 1);
		}

		// Socket Settings
		int slot = 0;

		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			TimeT64 socket = pkOldItem->GetSocket(i);
			const int ITEM_BROKEN_METIN_VNUM = 28960; // This is the same constant 3 Are there any places? ... Let’s make it one T_T I’m going to pass Hong.
			if (socket > 2 && socket != ITEM_BROKEN_METIN_VNUM)
				pkNewItem->SetSocket(slot++, socket);
		}

	}

	// Magic item settings
	pkOldItem->CopyAttributeTo(pkNewItem);
}

bool ITEM_MANAGER::GetVnumRangeByString(const std::string& stVnumRange, DWORD& r_dwVnumStart, DWORD& r_dwVnumEnd)
{
	int iPos;

	if ((iPos = static_cast<int>(stVnumRange.find("~"))) > 0)
	{
		std::string vnum_start, vnum_end;
		vnum_start.assign(stVnumRange, 0, iPos);
		vnum_end.assign(stVnumRange, iPos + 1, stVnumRange.length());

		if (!str_is_number(vnum_start.c_str()) || !str_is_number(vnum_end.c_str()))
			return false;

		int iRange[2];

		str_to_number(iRange[0], vnum_start.c_str());
		str_to_number(iRange[1], vnum_end.c_str());
		if (iRange[0] > iRange[1])
		{
			int iFirstVnum = iRange[0];
			iRange[0] = iRange[1];
			iRange[1] = iFirstVnum;
		}

		r_dwVnumStart = iRange[0];
		r_dwVnumEnd = iRange[1];

		return true;
	}

	return false;
}
