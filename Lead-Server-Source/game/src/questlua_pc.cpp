
#include "stdafx.h"

#include "config.h"
#include "questmanager.h"
#include "sectree_manager.h"
#include "char.h"
#include "char_manager.h"
#include "affect.h"
#include "item.h"
#include "item_manager.h"
#include "guild_manager.h"
#include "war_map.h"
#include "start_position.h"
#include "marriage.h"
#include "mining.h"
#include "p2p.h"
#include "polymorph.h"
#include "desc_client.h"
#include "messenger_manager.h"
#include "log.h"
#include "utils.h"
#include "unique_item.h"
#include "mob_manager.h"

#include <cctype>
#undef sys_err
#ifndef __WIN32__
#define sys_err(fmt, args...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, ##args)
#else
#define sys_err(fmt, ...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, __VA_ARGS__)
#endif

extern int g_nPortalLimitTime;
extern LPCLIENT_DESC db_clientdesc;
const int ITEM_BROKEN_METIN_VNUM = 28960;

namespace quest 
{
	//
	// "pc" Lua functions
	//
	int pc_has_master_skill(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		bool bHasMasterSkill = false;
		for (int i=0; i< SKILL_MAX_NUM; i++)
			if (ch->GetSkillMasterType(i) >= SKILL_MASTER && ch->GetSkillLevel(i) >= 21)
			{
				bHasMasterSkill = true;
				break;
			}

		lua_pushboolean(L, bHasMasterSkill);
		return 1;
	}

	int pc_remove_skill_book_no_delay(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		ch->RemoveAffect(AFFECT_SKILL_NO_BOOK_DELAY);
		return 0;
	}

	int pc_is_skill_book_no_delay(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		lua_pushboolean(L, ch->FindAffect(AFFECT_SKILL_NO_BOOK_DELAY) ? true : false);
		return 1;
	}

	int pc_learn_grand_master_skill(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isnumber(L, 1)) 
		{
			sys_err("wrong skill index");
			return 0;
		}

		lua_pushboolean(L, ch->LearnGrandMasterSkill((long)lua_tonumber(L, 1)));
		return 1;
	}

	int pc_set_warp_location(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isnumber(L, 1)) 
		{
			sys_err("wrong map index");
			return 0;
		}

		if (!lua_isnumber(L, 2) || !lua_isnumber(L, 3))
		{
			sys_err("wrong coodinate");
			return 0;
		}

		ch->SetWarpLocation((long)lua_tonumber(L,1), (long)lua_tonumber(L,2), (long)lua_tonumber(L,3));
		return 0;
	}

	int pc_set_warp_location_local(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isnumber(L, 1)) 
		{
			sys_err("wrong map index");
			return 0;
		}

		if (!lua_isnumber(L, 2) || !lua_isnumber(L, 3))
		{
			sys_err("wrong coodinate");
			return 0;
		}

		long lMapIndex = (long) lua_tonumber(L, 1);
		const TMapRegion * region = SECTREE_MANAGER::instance().GetMapRegion(lMapIndex);

		if (!region)
		{
			sys_err("invalid map index %d", lMapIndex);
			return 0;
		}

		int x = (int) lua_tonumber(L, 2);
		int y = (int) lua_tonumber(L, 3);

		if (x > region->ex - region->sx)
		{
			sys_err("x coordinate overflow max: %d input: %d", region->ex - region->sx, x);
			return 0;
		}

		if (y > region->ey - region->sy)
		{
			sys_err("y coordinate overflow max: %d input: %d", region->ey - region->sy, y);
			return 0;
		}

		ch->SetWarpLocation(lMapIndex, x, y);
		return 0;
	}

	int pc_get_start_location(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		lua_pushnumber(L, g_start_map[ch->GetEmpire()]);
		lua_pushnumber(L, g_start_position[ch->GetEmpire()][0] / 100);
		lua_pushnumber(L, g_start_position[ch->GetEmpire()][1] / 100);
		return 3;
	}

	int pc_warp(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2))
		{
			lua_pushboolean(L, false);
			return 1;
		}

		long map_index = 0;

		if (lua_isnumber(L, 3))
			map_index = (int) lua_tonumber(L,3);

		//PREVENT_HACK
		if ( ch->IsHack() )
		{
			lua_pushboolean(L, false);
			return 1;
		}
		//END_PREVENT_HACK
	
		if ( test_server )
			ch->ChatPacket( CHAT_TYPE_INFO, "[LS;5309;%d;%d;%d]",(int)lua_tonumber(L,1),
					(int)lua_tonumber(L,2),map_index );
		ch->WarpSet((int)lua_tonumber(L, 1), (int)lua_tonumber(L, 2), map_index);
		
		lua_pushboolean(L, true);

		return 1;
	}

	int pc_warp_local(lua_State * L)
	{
		if (!lua_isnumber(L, 1)) 
		{
			sys_err("no map index argument");
			return 0;
		}

		if (!lua_isnumber(L, 2) || !lua_isnumber(L, 3))
		{
			sys_err("no coodinate argument");
			return 0;
		}

		long lMapIndex = (long) lua_tonumber(L, 1);
		const TMapRegion * region = SECTREE_MANAGER::instance().GetMapRegion(lMapIndex);

		if (!region)
		{
			sys_err("invalid map index %d", lMapIndex);
			return 0;
		}

		int x = (int) lua_tonumber(L, 2);
		int y = (int) lua_tonumber(L, 3);

		if (x > region->ex - region->sx)
		{
			sys_err("x coordinate overflow max: %d input: %d", region->ex - region->sx, x);
			return 0;
		}

		if (y > region->ey - region->sy)
		{
			sys_err("y coordinate overflow max: %d input: %d", region->ey - region->sy, y);
			return 0;
		}

		/*
		int iPulse = thecore_pulse();	
		if ( pkChr->GetExchange() || pkChr->GetMyShop() || pkChr->GetShopOwner() || pkChr->IsOpenSafebox() )
		{
			pkChr->ChatPacket( CHAT_TYPE_INFO, "[LS;5265]");

			return;
		}
		//PREVENT_PORTAL_AFTER_EXCHANGE
		// Check time after exchange
		if ( iPulse - pkChr->GetExchangeTime()  < PASSES_PER_SEC(60) )
		{
			pkChr->ChatPacket( CHAT_TYPE_INFO, "[LS;5264]" );
			return;
		}
		//END_PREVENT_PORTAL_AFTER_EXCHANGE
		//PREVENT_ITEM_COPY
		{
			if ( iPulse - pkChr->GetMyShopTime() < PASSES_PER_SEC(60) )
			{
				pkChr->ChatPacket( CHAT_TYPE_INFO, "[LS;5264]" );
				return;
			}

		}
		//END_PREVENT_ITEM_COPY
		*/

		CQuestManager::instance().GetCurrentCharacterPtr()->WarpSet(region->sx + x, region->sy + y);
		return 0;
	}

	int pc_warp_exit(lua_State * L)
	{
		CQuestManager::instance().GetCurrentCharacterPtr()->ExitToSavedLocation();
		return 0;
	}

	int pc_in_dungeon(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, ch->GetDungeon()?1:0);
		return 1;
	}

	int pc_hasguild(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, ch->GetGuild() ? 1 : 0);
		return 1;
	}

	int pc_getguild(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushnumber(L, ch->GetGuild() ? ch->GetGuild()->GetID() : 0);
		return 1;
	}

	int pc_isguildmaster(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		CGuild * g = ch->GetGuild();

		if (g)
			lua_pushboolean(L, (ch->GetPlayerID() == g->GetMasterPID()));
		else
			lua_pushboolean(L, 0);

		return 1;
	}

	int pc_destroy_guild(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		CGuild * g = ch->GetGuild();

		if (g)
			g->RequestDisband(ch->GetPlayerID());

		return 0;
	}

	int pc_remove_from_guild(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		CGuild * g = ch->GetGuild();

		if (g)
			g->RequestRemoveMember(ch->GetPlayerID());

		return 0;
	}

	int pc_give_gold(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isnumber(L, 1))
		{
			sys_err("QUEST : wrong argument");
			return 0;
		}

		GoldType iAmount = static_cast<GoldType>(lua_tonumber(L, 1));

		if (iAmount <= 0)
		{
			sys_err("QUEST : gold amount less then zero");
			return 0;
		}

		DBManager::instance().SendMoneyLog(MONEY_LOG_QUEST, ch->GetPlayerID(), iAmount);
		ch->PointChange(POINT_GOLD, iAmount, true);
		return 0;
	}

	int pc_warp_to_guild_war_observer_position(lua_State* L)
	{
		luaL_checknumber(L, 1);
		luaL_checknumber(L, 2);

		DWORD gid1 = (DWORD)lua_tonumber(L, 1);
		DWORD gid2 = (DWORD)lua_tonumber(L, 2);

		CGuild* g1 = CGuildManager::instance().FindGuild(gid1);
		CGuild* g2 = CGuildManager::instance().FindGuild(gid2);

		if (!g1 || !g2)
		{
			luaL_error(L, "no such guild with id %d %d", gid1, gid2);
		}

		PIXEL_POSITION pos;

		DWORD dwMapIndex = g1->GetGuildWarMapIndex(gid2);

		if (!CWarMapManager::instance().GetStartPosition(dwMapIndex, 2, pos))
		{
			luaL_error(L, "not under warp guild war between guild %d %d", gid1, gid2);
			return 0;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		//PREVENT_HACK
		if ( ch->IsHack() )
			return 0;
		//END_PREVENT_HACK

		ch->SetQuestFlag("war.is_war_member", 0);
		ch->SaveExitLocation();
		ch->WarpSet(pos.x, pos.y, dwMapIndex);
		return 0;
	}

	int pc_give_item_from_special_item_group(lua_State* L)
	{
		luaL_checknumber(L, 1);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		DWORD dwGroupVnum = (DWORD) lua_tonumber(L,1);

		std::vector <DWORD> dwVnums;
		std::vector <DWORD> dwCounts;
		std::vector <LPITEM> item_gets;
		int count = 0;

		ch->GiveItemFromSpecialItemGroup(dwGroupVnum, dwVnums, dwCounts, item_gets, count);
		
		for (int i = 0; i < count; i++)
		{
			if (!item_gets[i])
			{
				if (dwVnums[i] == 1)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1269;%d]", dwCounts[i]);
				}
				else if (dwVnums[i] == 2)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;487]");
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1290;%d]", dwCounts[i]);
				}
			}
		}
		return 0;
	}

	int pc_enough_inventory(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (!lua_isnumber(L, 1))
		{
			lua_pushboolean(L, 0);
			return 1;
		}

		DWORD item_vnum = (DWORD)lua_tonumber(L, 1);
		TItemTable * pTable = ITEM_MANAGER::instance().GetTable(item_vnum);
		if (!pTable)
		{
			lua_pushboolean(L, 0);
			return 1;
		}

		bool bEnoughInventoryForItem = ch->GetEmptyInventory(pTable->bSize) != -1;
		lua_pushboolean(L, bEnoughInventoryForItem);
		return 1;
	}

	int pc_give_item(lua_State* L)
	{
		PC* pPC = CQuestManager::instance().GetCurrentPC();
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isstring(L, 1) || !(lua_isstring(L, 2)||lua_isnumber(L, 2)))
		{
			sys_err("QUEST : wrong argument");
			return 0;
		}

		DWORD dwVnum;

		if (lua_isnumber(L,2)) // If it is a number, give it as a number. .
			dwVnum = (int) lua_tonumber(L, 2);
		else if (!ITEM_MANAGER::instance().GetVnum(lua_tostring(L, 2), dwVnum))
		{
			sys_err("QUEST Make item call error : wrong item name : %s", lua_tostring(L,1));
			return 0;
		}

		int icount = 1;

		if (lua_isnumber(L, 3) && lua_tonumber(L, 3) > 0)
		{
			icount = (int)rint(lua_tonumber(L, 3));

			if (icount <= 0) 
			{
				sys_err("QUEST Make item call error : wrong item count : %g", lua_tonumber(L, 2));
				return 0;
			}
		}

		pPC->GiveItem(lua_tostring(L, 1), dwVnum, icount);

		LogManager::instance().QuestRewardLog(pPC->GetCurrentQuestName().c_str(), ch->GetPlayerID(), ch->GetLevel(), dwVnum, icount);
		return 0;
	}

	int pc_give_or_drop_item(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isstring(L, 1) && !lua_isnumber(L, 1))
		{
			sys_err("QUEST Make item call error : wrong argument");
			lua_pushnumber (L, 0);
			return 1;
		}

		DWORD dwVnum;

		if (lua_isnumber(L, 1)) // If it is a number, give it as a number. .
		{
			dwVnum = (int) lua_tonumber(L, 1);
		}
		else if (!ITEM_MANAGER::instance().GetVnum(lua_tostring(L, 1), dwVnum))
		{
			sys_err("QUEST Make item call error : wrong item name : %s", lua_tostring(L,1));
			lua_pushnumber (L, 0);

			return 1;
		}

		int icount = 1;
		if (lua_isnumber(L,2) && lua_tonumber(L,2)>0)
		{
			icount = (int)rint(lua_tonumber(L,2));
			if (icount<=0) 
			{
				sys_err("QUEST Make item call error : wrong item count : %g", lua_tonumber(L,2));
				lua_pushnumber (L, 0);
				return 1;
			}
		}

		sys_log(0, "QUEST [REWARD] item %s to %s", lua_tostring(L, 1), ch->GetName());

		PC* pPC = CQuestManager::instance().GetCurrentPC();

		LogManager::instance().QuestRewardLog(pPC->GetCurrentQuestName().c_str(), ch->GetPlayerID(), ch->GetLevel(), dwVnum, icount);

		LPITEM item = ch->AutoGiveItem(dwVnum, icount);

		if ( dwVnum >= 80003 && dwVnum <= 80007 )
		{
			LogManager::instance().GoldBarLog(ch->GetPlayerID(), item->GetID(), QUEST, "quest: give_item2");
		}
		
		if (NULL != item)
			lua_pushnumber (L, item->GetID());
		else
			lua_pushnumber (L, 0);
		return 1;
	}

	int pc_give_or_drop_item_and_select(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isstring(L, 1) && !lua_isnumber(L, 1))
		{
			sys_err("QUEST Make item call error : wrong argument");
			return 0;
		}

		DWORD dwVnum;

		if (lua_isnumber(L, 1)) // If it is a number, give it as a number. .
		{
			dwVnum = (int) lua_tonumber(L, 1);
		}
		else if (!ITEM_MANAGER::instance().GetVnum(lua_tostring(L, 1), dwVnum))
		{
			sys_err("QUEST Make item call error : wrong item name : %s", lua_tostring(L,1));
			return 0;
		}

		int icount = 1;
		if (lua_isnumber(L,2) && lua_tonumber(L,2)>0)
		{
			icount = (int)rint(lua_tonumber(L,2));
			if (icount<=0) 
			{
				sys_err("QUEST Make item call error : wrong item count : %g", lua_tonumber(L,2));
				return 0;
			}
		}

		sys_log(0, "QUEST [REWARD] item %s to %s", lua_tostring(L, 1), ch->GetName());

		PC* pPC = CQuestManager::instance().GetCurrentPC();

		LogManager::instance().QuestRewardLog(pPC->GetCurrentQuestName().c_str(), ch->GetPlayerID(), ch->GetLevel(), dwVnum, icount);

		LPITEM item = ch->AutoGiveItem(dwVnum, icount);

		if (NULL != item)
			CQuestManager::Instance().SetCurrentItem(item);

		if ( dwVnum >= 80003 && dwVnum <= 80007 )
		{
			LogManager::instance().GoldBarLog(ch->GetPlayerID(), item->GetID(), QUEST, "quest: give_item2");
		}
		
		return 0;
	}

	int pc_get_current_map_index(lua_State* L)
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetMapIndex());
		return 1;
	}

	int pc_get_x(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushnumber(L, ch->GetX()/100);
		return 1;
	}

	int pc_get_y(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushnumber(L, ch->GetY()/100);
		return 1;
	}

	int pc_get_local_x(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		LPSECTREE_MAP pMap = SECTREE_MANAGER::instance().GetMap(ch->GetMapIndex());

		if (pMap)
			lua_pushnumber(L, (ch->GetX() - pMap->m_setting.iBaseX) / 100);
		else
			lua_pushnumber(L, ch->GetX() / 100);

		return 1;
	}

	int pc_get_local_y(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		LPSECTREE_MAP pMap = SECTREE_MANAGER::instance().GetMap(ch->GetMapIndex());

		if (pMap)
			lua_pushnumber(L, (ch->GetY() - pMap->m_setting.iBaseY) / 100);
		else
			lua_pushnumber(L, ch->GetY() / 100);

		return 1;
	}

	int pc_count_item(lua_State* L)
	{
		if (lua_isnumber(L, -1))
			lua_pushnumber(L,CQuestManager::instance().GetCurrentCharacterPtr()->CountSpecifyItem((DWORD)lua_tonumber(L, -1)));
		else if (lua_isstring(L, -1))
		{
			DWORD item_vnum;

			if (!ITEM_MANAGER::instance().GetVnum(lua_tostring(L,1), item_vnum))
			{
				sys_err("QUEST count_item call error : wrong item name : %s", lua_tostring(L,1));
				lua_pushnumber(L, 0);
			}
			else
			{
				lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->CountSpecifyItem(item_vnum));
			}
		}
		else
			lua_pushnumber(L, 0);

		return 1;
	}

	int pc_remove_item(lua_State* L)
	{
		if (lua_gettop(L) == 1)
		{
			DWORD item_vnum;

			if (lua_isnumber(L,1))
			{
				item_vnum = (DWORD)lua_tonumber(L, 1);
			}
			else if (lua_isstring(L,1))
			{
				if (!ITEM_MANAGER::instance().GetVnum(lua_tostring(L,1), item_vnum))
				{
					sys_err("QUEST remove_item call error : wrong item name : %s", lua_tostring(L,1));
					return 0;
				}
			}
			else
			{
				sys_err("QUEST remove_item wrong argument");
				return 0;
			}

			sys_log(0,"QUEST remove a item vnum %d of %s[%d]", item_vnum, CQuestManager::instance().GetCurrentCharacterPtr()->GetName(), CQuestManager::instance().GetCurrentCharacterPtr()->GetPlayerID());
			CQuestManager::instance().GetCurrentCharacterPtr()->RemoveSpecifyItem(item_vnum);
		}
		else if (lua_gettop(L) == 2)
		{
			DWORD item_vnum;

			if (lua_isnumber(L, 1))
			{
				item_vnum = (DWORD)lua_tonumber(L, 1);
			}
			else if (lua_isstring(L, 1))
			{
				if (!ITEM_MANAGER::instance().GetVnum(lua_tostring(L,1), item_vnum))
				{
					sys_err("QUEST remove_item call error : wrong item name : %s", lua_tostring(L,1));
					return 0;
				}
			}
			else
			{
				sys_err("QUEST remove_item wrong argument");
				return 0;
			}

			DWORD item_count = (DWORD) lua_tonumber(L, 2);
			sys_log(0, "QUEST remove items(vnum %d) count %d of %s[%d]",
					item_vnum,
					item_count,
					CQuestManager::instance().GetCurrentCharacterPtr()->GetName(),
					CQuestManager::instance().GetCurrentCharacterPtr()->GetPlayerID());

			CQuestManager::instance().GetCurrentCharacterPtr()->RemoveSpecifyItem(item_vnum, item_count);
		}
		return 0;
	}

	int pc_get_leadership(lua_State * L)
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetLeadershipSkillLevel());
		return 1;
	}

	int pc_reset_point(lua_State * L)
	{
		CQuestManager::instance().GetCurrentCharacterPtr()->ResetPoint(CQuestManager::instance().GetCurrentCharacterPtr()->GetLevel());
		return 0;
	}

	int pc_get_playtime(lua_State* L)
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetRealPoint(POINT_PLAYTIME));
		return 1;
	}

	int pc_get_vid(lua_State* L)
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetVID());
		return 1;
	}
	int pc_get_name(lua_State* L)
	{
		lua_pushstring(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetName());
		return 1;
	}

	int pc_get_next_exp(lua_State* L)  
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetNextExp());
		return 1;
	}

	int pc_get_exp(lua_State* L)  
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetExp());
		return 1;
	}

	int pc_get_race(lua_State* L)  
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetRaceNum());
		return 1;
	}

	int pc_change_sex(lua_State* L)  
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->ChangeSex());
		return 1;
	}

	int pc_get_job(lua_State* L)  
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetJob());
		return 1;
	}

	int pc_get_max_sp(lua_State* L)  
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetMaxSP());
		return 1;
	}

	int pc_get_sp(lua_State * L)  
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetSP());
		return 1;
	}

	int pc_change_sp(lua_State * L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("invalid argument");
			lua_pushboolean(L, 0);
			return 1;
		}

		long val = (long) lua_tonumber(L, 1);

		if (val == 0)
		{
			lua_pushboolean(L, 0);
			return 1;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (val > 0) // Since it increases, it returns success unconditionally.
			ch->PointChange(POINT_SP, val);
		else if (val < 0)
		{
			if (ch->GetSP() < -val)
			{
				lua_pushboolean(L, 0);
				return 1;
			}

			ch->PointChange(POINT_SP, val);
		}

		lua_pushboolean(L, 1);
		return 1;
	}

	int pc_get_max_hp(lua_State * L)  
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetMaxHP());
		return 1;
	}

	int pc_get_hp(lua_State * L)  
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetHP());
		return 1;
	}

	int pc_get_level(lua_State * L)  
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetLevel());
		return 1;
	}

	int pc_set_level(lua_State * L)  
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("invalid argument");
			return 0;
		}
		else
		{
			int newLevel = static_cast<int>(lua_tonumber(L, 1));
			LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();


			sys_log(0,"QUEST [LEVEL] %s jumpint to level %d", ch->GetName(), (int)rint(lua_tonumber(L,1)));

			PC* pPC = CQuestManager::instance().GetCurrentPC();
			LogManager::instance().QuestRewardLog(pPC->GetCurrentQuestName().c_str(), ch->GetPlayerID(), ch->GetLevel(), newLevel, 0);
			
			// point : skill , Sub skill , Stats
			ch->PointChange(POINT_SKILL, newLevel - ch->GetLevel());
			ch->PointChange(POINT_SUB_SKILL, newLevel < 10 ? 0 : newLevel - MAX(ch->GetLevel(), 9));
			ch->PointChange(POINT_STAT, ((MINMAX(1, newLevel, 90) - ch->GetLevel()) * 3) + ch->GetPoint(POINT_LEVEL_STEP));
			// level
			ch->PointChange(POINT_LEVEL, newLevel - ch->GetLevel(), false, true);
			//HP, SP
			ch->SetRandomHP((newLevel - 1) * number(JobInitialPoints[ch->GetJob()].hp_per_lv_begin, JobInitialPoints[ch->GetJob()].hp_per_lv_end));
			ch->SetRandomSP((newLevel - 1) * number(JobInitialPoints[ch->GetJob()].sp_per_lv_begin, JobInitialPoints[ch->GetJob()].sp_per_lv_end));


			// recovery
			ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
			ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
			
			ch->ComputePoints();
			ch->PointsPacket();
			ch->SkillLevelPacket();

			return 0;
		}
	}

	int pc_get_weapon(lua_State * L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentCharacterPtr()->GetWear(WEAR_WEAPON);

		if (!item)
			lua_pushnumber(L, 0);
		else
			lua_pushnumber(L, item->GetVnum());

		return 1;
	}

	int pc_get_armor(lua_State * L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentCharacterPtr()->GetWear(WEAR_BODY);

		if (!item)
			lua_pushnumber(L, 0);
		else
			lua_pushnumber(L, item->GetVnum());

		return 1;
	}

	int pc_get_wear(lua_State * L)
	{		
		if (!lua_isnumber(L, 1))
		{
			sys_err("QUEST wrong set flag");
			return 0;
		}

		BYTE bCell = (BYTE)lua_tonumber(L, 1);

		LPITEM item = CQuestManager::instance().GetCurrentCharacterPtr()->GetWear(bCell);


		if (!item)
			lua_pushnil(L);
		else
			lua_pushnumber(L, item->GetVnum());

		return 1;
	}

	int pc_get_money(lua_State * L)
	{ 
		lua_pushnumber(L, static_cast<lua_Number>(CQuestManager::instance().GetCurrentCharacterPtr()->GetGold()));
		return 1;
	}

	// 20050725.myevan. While using the cloak of seclusion, a bug occurred where the values ​​of good and evil were doubled during soul training.
	// Calculate using actual good and bad values .
	int pc_get_real_alignment(lua_State* L)
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetRealAlignment()/10);
		return 1;
	}

	int pc_get_alignment(lua_State* L)
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetAlignment()/10);
		return 1;
	}

	int pc_change_alignment(lua_State * L)
	{
		int alignment = (int)(lua_tonumber(L, 1)*10);
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		ch->UpdateAlignment(alignment);
		return 0;
	}

	int pc_change_money(lua_State * L)
	{
		GoldType gold = static_cast<GoldType>(lua_tonumber(L, -1));

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (gold + ch->GetGold() < 0)
			sys_err("QUEST wrong ChangeGold %lld (now %lld)", static_cast<long long>(gold), static_cast<long long>(ch->GetGold()));
		else
		{
			DBManager::instance().SendMoneyLog(MONEY_LOG_QUEST, ch->GetPlayerID(), gold);
			ch->PointChange(POINT_GOLD, gold, true);
		}

		return 0;
	}

	int pc_set_another_quest_flag(lua_State* L)
	{
		if (!lua_isstring(L, 1) || !lua_isstring(L, 2) || !lua_isnumber(L, 3))
		{
			sys_err("QUEST wrong set flag");
			return 0;
		}
		else
		{
			const char * sz = lua_tostring(L, 1);
			const char * sz2 = lua_tostring(L, 2);
			CQuestManager & q = CQuestManager::Instance();
			PC * pPC = q.GetCurrentPC();
			pPC->SetFlag(string(sz)+"."+sz2, int(rint(lua_tonumber(L,3))));
			return 0;
		}
	}

	int pc_get_another_quest_flag(lua_State* L)
	{
		if (!lua_isstring(L,1) || !lua_isstring(L,2))
		{
			sys_err("QUEST wrong get flag");
			return 0;
		}
		else
		{
			const char* sz = lua_tostring(L,1);
			const char* sz2 = lua_tostring(L,2);
			CQuestManager& q = CQuestManager::Instance();
			PC* pPC = q.GetCurrentPC();
			if (!pPC)
			{
				return 0;
			}
			lua_pushnumber(L,static_cast<lua_Number>(pPC->GetFlag(string(sz)+"."+sz2)));
			return 1;
		}
	}

	int pc_get_flag(lua_State* L)
	{
		if (!lua_isstring(L,-1))
		{
			sys_err("QUEST wrong get flag");
			return 0;
		}
		else
		{
			const char* sz = lua_tostring(L,-1);
			CQuestManager& q = CQuestManager::Instance();
			PC* pPC = q.GetCurrentPC();
			lua_pushnumber(L,static_cast<lua_Number>(pPC->GetFlag(sz)));
			return 1;
		}
	}

	int pc_get_quest_flag(lua_State* L)
	{
		if (!lua_isstring(L,-1))
		{
			sys_err("QUEST wrong get flag");
			return 0;
		}
		else
		{
			const char* sz = lua_tostring(L,-1);
			CQuestManager& q = CQuestManager::Instance();
			PC* pPC = q.GetCurrentPC();
			lua_pushnumber(L,static_cast<lua_Number>(pPC->GetFlag(pPC->GetCurrentQuestName() + "."+sz)));
			if ( test_server )
				sys_log( 0 ,"GetQF ( %s . %s )", pPC->GetCurrentQuestName().c_str(), sz );
		}
		return 1;
	}

	int pc_set_flag(lua_State* L)
	{
		if (!lua_isstring(L,1) || !lua_isnumber(L,2))
		{
			sys_err("QUEST wrong set flag");
		}
		else
		{
			const char* sz = lua_tostring(L,1);
			CQuestManager& q = CQuestManager::Instance();
			PC* pPC = q.GetCurrentPC();
			pPC->SetFlag(sz, int(rint(lua_tonumber(L,2))));
		}
		return 0;
	}

	int pc_set_quest_flag(lua_State* L)
	{
		if (!lua_isstring(L,1) || !lua_isnumber(L,2))
		{
			sys_err("QUEST wrong set flag");
		}
		else
		{
			const char* sz = lua_tostring(L,1);
			CQuestManager& q = CQuestManager::Instance();
			PC* pPC = q.GetCurrentPC();
			pPC->SetFlag(pPC->GetCurrentQuestName()+"."+sz, int(rint(lua_tonumber(L,2))));
		}
		return 0;
	}

	int pc_del_quest_flag(lua_State *L)
	{
		if (!lua_isstring(L, 1))
		{
			sys_err("argument error");
			return 0;
		}

		const char * sz = lua_tostring(L, 1);
		PC * pPC = CQuestManager::instance().GetCurrentPC();
		pPC->DeleteFlag(pPC->GetCurrentQuestName()+"."+sz);
		return 0;
	}

	int pc_give_exp2(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();
		if (!lua_isnumber(L,1))
			return 0;

		sys_log(0,"QUEST [REWARD] %s give exp2 %d", ch->GetName(), (int)rint(lua_tonumber(L,1)));

		DWORD exp = (DWORD)rint(lua_tonumber(L,1));

		PC* pPC = CQuestManager::instance().GetCurrentPC();
		LogManager::instance().QuestRewardLog(pPC->GetCurrentQuestName().c_str(), ch->GetPlayerID(), ch->GetLevel(), exp, 0);
		ch->PointChange(POINT_EXP, exp);
		return 0;
	}

	int pc_give_exp(lua_State* L)
	{
		if (!lua_isstring(L,1) || !lua_isnumber(L,2))
			return 0;

		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();

		sys_log(0,"QUEST [REWARD] %s give exp %s %d", ch->GetName(), lua_tostring(L,1), (int)rint(lua_tonumber(L,2)));

		DWORD exp = (DWORD)rint(lua_tonumber(L,2));

		PC* pPC = CQuestManager::instance().GetCurrentPC();

		LogManager::instance().QuestRewardLog(pPC->GetCurrentQuestName().c_str(), ch->GetPlayerID(), ch->GetLevel(), exp, 0);

		pPC->GiveExp(lua_tostring(L,1), exp);
		return 0;
	}

	int pc_give_exp_perc(lua_State* L)
	{
		CQuestManager & q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();

		if (!ch || !lua_isstring(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3))
			return 0;

		int lev = (int)rint(lua_tonumber(L,2));
		double proc = (lua_tonumber(L,3));

		sys_log(0, "QUEST [REWARD] %s give exp %s lev %d percent %g%%", ch->GetName(), lua_tostring(L, 1), lev, proc);

		DWORD exp = (DWORD)((exp_table[MINMAX(0, lev, PLAYER_EXP_TABLE_MAX)] * proc) / 100);
		PC * pPC = CQuestManager::instance().GetCurrentPC();
		
		LogManager::instance().QuestRewardLog(pPC->GetCurrentQuestName().c_str(), ch->GetPlayerID(), ch->GetLevel(), exp, 0);

		pPC->GiveExp(lua_tostring(L, 1), exp);
		return 0;
	}

	int pc_get_empire(lua_State* L)  
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetEmpire());
		return 1;
	}

	int pc_get_part(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();
		if (!lua_isnumber(L,1))
		{
			lua_pushnumber(L, 0);
			return 1;
		}
		int part_idx = (int)lua_tonumber(L, 1);
		lua_pushnumber(L, ch->GetPart(part_idx));
		return 1;
	}

	int pc_set_part(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();
		if (!lua_isnumber(L,1) || !lua_isnumber(L,2))
		{
			return 0;
		}
		int part_idx = (int)lua_tonumber(L, 1);
		int part_value = (int)lua_tonumber(L, 2);
		ch->SetPart(part_idx, part_value);
		ch->UpdatePacket();
		return 0;
	}

	int pc_get_skillgroup(lua_State* L)  
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetSkillGroup());
		return 1;
	}

	int pc_set_skillgroup(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
			sys_err("QUEST wrong skillgroup number");
		else
		{
			CQuestManager & q = CQuestManager::Instance();
			LPCHARACTER ch = q.GetCurrentCharacterPtr();

			ch->SetSkillGroup((BYTE) rint(lua_tonumber(L, 1)));
		}
		return 0;
	}

	int pc_is_polymorphed(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, ch->IsPolymorphed());
		return 1;
	}

	int pc_remove_polymorph(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		ch->RemoveAffect(AFFECT_POLYMORPH);
		ch->SetPolymorph(0);
		return 0;
	}

	int pc_polymorph(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		DWORD dwVnum = (DWORD) lua_tonumber(L, 1);
		int iDuration = (int) lua_tonumber(L, 2);
		ch->AddAffect(AFFECT_POLYMORPH, POINT_POLYMORPH, dwVnum, AFF_POLYMORPH, iDuration, 0, true);
		return 0;
	}

	int pc_is_mount(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, ch->GetMountVnum());
		return 1;
	}

	int pc_mount(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
			return 0;

		int length = 60;

		if (lua_isnumber(L, 2))
			length = (int)lua_tonumber(L, 2);

		DWORD mount_vnum = (DWORD)lua_tonumber(L, 1);

		if (length < 0)
			length = 60;

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		ch->RemoveAffect(AFFECT_MOUNT);
		ch->RemoveAffect(AFFECT_MOUNT_BONUS);

		// If a horse is summoned and follows you around, get rid of the horse first.
		if (ch->GetHorse())
			ch->HorseSummon(false);

		if (mount_vnum)
		{
			ch->AddAffect(AFFECT_MOUNT, POINT_MOUNT, mount_vnum, AFF_NONE, length, 0, true);
			switch (mount_vnum)
			{
			case 20201:
			case 20202:
			case 20203:
			case 20204:
			case 20213:
			case 20216:
			ch->AddAffect(AFFECT_MOUNT, POINT_MOV_SPEED, 30, AFF_NONE, length, 0, true, true);
			break;

			case 20205:
			case 20206:
			case 20207:
			case 20208:
			case 20214:
			case 20217:
			ch->AddAffect(AFFECT_MOUNT, POINT_MOV_SPEED, 40, AFF_NONE, length, 0, true, true);
			break;

			case 20209:
			case 20210:
			case 20211:
			case 20212:
			case 20215:
			case 20218:
			ch->AddAffect(AFFECT_MOUNT, POINT_MOV_SPEED, 50, AFF_NONE, length, 0, true, true);
			break;

			}
		}
		
		return 0;
	}

	int pc_mount_bonus(lua_State* L)
	{
		BYTE applyOn = static_cast<BYTE>(lua_tonumber(L, 1));
		long value = static_cast<long>(lua_tonumber(L, 2));
		long duration = static_cast<long>(lua_tonumber(L, 3));

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if( NULL != ch )
		{
			ch->RemoveAffect(AFFECT_MOUNT_BONUS);
			ch->AddAffect(AFFECT_MOUNT_BONUS, aApplyInfo[applyOn].bPointType, value, AFF_NONE, duration, 0, false);
		}

		return 0;
	}

	int pc_unmount(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		ch->RemoveAffect(AFFECT_MOUNT);
		ch->RemoveAffect(AFFECT_MOUNT_BONUS);
		if (ch->IsHorseRiding())
			ch->StopRiding();
		return 0;
	}

	int pc_get_horse_level(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushnumber(L, ch->GetHorseLevel());
		return 1;
	}

	int pc_get_horse_hp(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (ch->GetHorseLevel())
			lua_pushnumber(L, ch->GetHorseHealth());
		else
			lua_pushnumber(L, 0);

		return 1;
	}

	int pc_get_horse_stamina(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (ch->GetHorseLevel())
			lua_pushnumber(L, ch->GetHorseStamina());
		else
			lua_pushnumber(L, 0);

		return 1;
	}

	int pc_is_horse_alive(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, ch->GetHorseLevel() > 0 && ch->GetHorseHealth()>0);
		return 1;
	}

	int pc_revive_horse(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		ch->ReviveHorse();
		return 0;
	}

	int pc_have_map_scroll(lua_State* L)
	{
		if (!lua_isstring(L, 1))
		{
			lua_pushboolean(L, 0);
			return 1;
		}

		const char * szMapName = lua_tostring(L, 1);
		const TMapRegion * region = SECTREE_MANAGER::instance().FindRegionByPartialName(szMapName);

		if (!region)
		{
			lua_pushboolean(L, 0);
			return 1;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		bool bFind = false;
		for (int iCell = 0; iCell < INVENTORY_MAX_NUM; iCell++)
		{
			LPITEM item = ch->GetInventoryItem(iCell);
			if (!item)
				continue;

			if (item->GetType() == ITEM_USE && 
					item->GetSubType() == USE_TALISMAN && 
					(item->GetValue(0) == 1 || item->GetValue(0) == 2))
			{
				TimeT64 x = item->GetSocket(0);
				TimeT64 y = item->GetSocket(1);
				//if ((x-item_x)*(x-item_x)+(y-item_y)*(y-item_y)<r*r)
				if (region->sx <=x && region->sy <= y && x <= region->ex && y <= region->ey)
				{
					bFind = true;
					break;
				}
			}
		}

		lua_pushboolean(L, bFind);
		return 1;
	}

	int pc_get_war_map(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushnumber(L, ch->GetWarMap() ? ch->GetWarMap()->GetMapIndex() : 0);
		return 1;
	}

	int pc_have_pos_scroll(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isnumber(L,1) || !lua_isnumber(L,2))
		{
			sys_err("invalid x y position");
			lua_pushboolean(L, 0);
			return 1;
		}

		if (!lua_isnumber(L,2))
		{
			sys_err("invalid radius");
			lua_pushboolean(L, 0);
			return 1;
		}

		int x = (int)lua_tonumber(L, 1);
		int y = (int)lua_tonumber(L, 2);
		float r = (float)lua_tonumber(L, 3);

		bool bFind = false;
		for (int iCell = 0; iCell < INVENTORY_MAX_NUM; iCell++)
		{
			LPITEM item = ch->GetInventoryItem(iCell);
			if (!item)
				continue;

			if (item->GetType() == ITEM_USE && 
					item->GetSubType() == USE_TALISMAN && 
					(item->GetValue(0) == 1 || item->GetValue(0) == 2))
			{
				TimeT64 item_x = item->GetSocket(0);
				TimeT64 item_y = item->GetSocket(1);
				if ((x-item_x)*(x-item_x)+(y-item_y)*(y-item_y)<r*r)
				{
					bFind = true;
					break;
				}
			}
		}

		lua_pushboolean(L, bFind);
		return 1;
	}

	int pc_get_equip_refine_level(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		int cell = (int) lua_tonumber(L, 1);
		if (cell < 0 || cell >= WEAR_MAX_NUM)
		{
			sys_err("invalid wear position %d", cell);
			lua_pushnumber(L, 0);
			return 1;
		}

		LPITEM item = ch->GetWear(cell);
		if (!item)
		{
			lua_pushnumber(L, 0);
			return 1;
		}

		lua_pushnumber(L, item->GetRefineLevel());
		return 1;
	}

	int pc_refine_equip(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2))
		{
			sys_err("invalid argument");
			lua_pushboolean(L, 0);
			return 1;
		}

		int cell = (int) lua_tonumber(L, 1);
		int level_limit = (int) lua_tonumber(L, 2);
		int pct = lua_isnumber(L, 3) ? (int)lua_tonumber(L, 3) : 100;

		LPITEM item = ch->GetWear(cell);
		if (!item)
		{
			lua_pushboolean(L, 0);
			return 1;
		}

		if (item->GetRefinedVnum() == 0)
		{
			lua_pushboolean(L, 0);
			return 1;
		}

		if (item->GetRefineLevel()>level_limit)
		{
			lua_pushboolean(L, 0);
			return 1;
		}

		if (pct == 100 || number(1, 100) <= pct)
		{
			// improvement success
			lua_pushboolean(L, 1);

			LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(item->GetRefinedVnum(), 1, 0, false);

			if (pkNewItem)
			{
				for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
					if (!item->GetSocket(i))
						break;
					else
						pkNewItem->SetSocket(i, 1);

				int set = 0;
				for (int i=0; i<ITEM_SOCKET_MAX_NUM; i++)
				{
					TimeT64 socket = item->GetSocket(i);
					if (socket > 2 && socket != 28960)
					{
						pkNewItem->SetSocket(set++, socket);
					}
				}

				item->CopyAttributeTo(pkNewItem);

				ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE SUCCESS)");

				// some tuits need here -_- pkNewItem->AddToCharacter(this, bCell); 
				pkNewItem->EquipTo(ch, cell);

				ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);

				LogManager::instance().ItemLog(ch, pkNewItem, "REFINE SUCCESS (QUEST)", pkNewItem->GetName());
			}
		}
		else
		{
			// improvement failed
			lua_pushboolean(L, 0);
		}

		return 1;
	}

	int pc_get_skill_level(lua_State * L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("invalid argument");
			lua_pushnumber(L, 0);
			return 1;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		DWORD dwVnum = (DWORD) lua_tonumber(L, 1);
		lua_pushnumber(L, ch->GetSkillLevel(dwVnum));

		return 1;
	}

	int pc_aggregate_monster(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		ch->AggregateMonster();
		return 0;
	}

	int pc_forget_my_attacker(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		ch->ForgetMyAttacker();
		return 0;
	}

	int pc_attract_ranger(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		ch->AttractRanger();
		return 0;
	}

	int pc_select_pid(lua_State* L)
	{
		DWORD pid = (DWORD) lua_tonumber(L, 1);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		LPCHARACTER new_ch = CHARACTER_MANAGER::instance().FindByPID(pid);

		if (new_ch)
		{
			CQuestManager::instance().GetPC(new_ch->GetPlayerID());

			lua_pushnumber(L, ch->GetPlayerID());
		}
		else
		{
			lua_pushnumber(L, 0);
		}

		return 1;
	}

	int pc_select_vid(lua_State* L)
	{
		DWORD vid = (DWORD) lua_tonumber(L, 1);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		LPCHARACTER new_ch = CHARACTER_MANAGER::instance().Find(vid);

		if (new_ch)
		{
			CQuestManager::instance().GetPC(new_ch->GetPlayerID());

			lua_pushnumber(L, (DWORD)ch->GetVID());
		}
		else
		{
			lua_pushnumber(L, 0);
		}

		return 1;
	}

	int pc_get_sex(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushnumber(L, GET_SEX(ch)); /* 0==MALE, 1==FEMALE */
		return 1;
	}

	int pc_is_engaged(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, marriage::CManager::instance().IsEngaged(ch->GetPlayerID()));
		return 1;
	}

	int pc_is_married(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, marriage::CManager::instance().IsMarried(ch->GetPlayerID()));
		return 1;
	}

	int pc_is_engaged_or_married(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, marriage::CManager::instance().IsEngagedOrMarried(ch->GetPlayerID()));
		return 1;
	}

	int pc_is_gm(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, ch->GetGMLevel() >= GM_HIGH_WIZARD);
		return 1;
	}

	int pc_get_gm_level(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushnumber(L, ch->GetGMLevel());
		return 1;
	}

	int pc_mining(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		LPCHARACTER npc = CQuestManager::instance().GetCurrentNPCCharacterPtr();
		ch->mining(npc);
		return 0;
	}

	int pc_diamond_refine(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
		{
			lua_pushboolean(L, 0);
			return 1;
		}

		int cost = (int) lua_tonumber(L, 1);
		int pct = (int)lua_tonumber(L, 2);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		LPCHARACTER npc = CQuestManager::instance().GetCurrentNPCCharacterPtr();
		LPITEM item = CQuestManager::instance().GetCurrentItem();

		if (item)
			lua_pushboolean(L, mining::OreRefine(ch, npc, item, cost, pct, NULL));
		else
			lua_pushboolean(L, 0);

		return 1;
	}

	int pc_ore_refine(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
		{
			lua_pushboolean(L, 0);
			return 1;
		}

		int cost = (int) lua_tonumber(L, 1);
		int pct = (int)lua_tonumber(L, 2);
		int metinstone_cell = (int)lua_tonumber(L, 3);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		LPCHARACTER npc = CQuestManager::instance().GetCurrentNPCCharacterPtr();
		LPITEM item = CQuestManager::instance().GetCurrentItem();

		LPITEM metinstone_item = ch->GetInventoryItem(metinstone_cell);

		if (item && metinstone_item)
			lua_pushboolean(L, mining::OreRefine(ch, npc, item, cost, pct, metinstone_item));
		else
			lua_pushboolean(L, 0);

		return 1;
	}

	int pc_clear_skill(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if ( ch == NULL ) return 0;

		ch->ClearSkill();

		return 0;
	}

	int pc_clear_sub_skill(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if ( ch == NULL ) return 0;

		ch->ClearSubSkill();

		return 0;
	}

	int pc_set_skill_point(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
		{
			return 0;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		int newPoint = (int) lua_tonumber(L, 1);

		ch->SetRealPoint(POINT_SKILL, newPoint);
		ch->SetPoint(POINT_SKILL, ch->GetRealPoint(POINT_SKILL));
		ch->PointChange(POINT_SKILL, 0);
		ch->ComputePoints();
		ch->PointsPacket();

		return 0;
	}

	// RESET_ONE_SKILL	
	int pc_clear_one_skill(lua_State* L)
	{
		int vnum = (int)lua_tonumber(L, 1);
		sys_log(0, "%d skill clear", vnum);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if ( ch == NULL )
		{
			sys_log(0, "skill clear fail");
			lua_pushnumber(L, 0);
			return 1;
		}

		sys_log(0, "%d skill clear", vnum);

		ch->ResetOneSkill(vnum);

		return 0;
	}
	// END_RESET_ONE_SKILL

	int pc_is_clear_skill_group(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		lua_pushboolean(L, ch->GetQuestFlag("skill_group_clear.clear") == 1);
	
		return 1;
	}

	int pc_save_exit_location(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		ch->SaveExitLocation();

		return 0;
	}

	// teleport 
	int pc_teleport ( lua_State * L )
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		int x=0,y=0;	
		if ( lua_isnumber(L, 1) )
		{
			// Local name: Warp
			const int TOWN_NUM = 10;
			struct warp_by_town_name
			{
				const char* name;
				DWORD x;
				DWORD y;
			} ws[TOWN_NUM] = 
			{
			    {"Yeongan",  4743,   9548}, // Shinsoo Village 1
			    {"Imha",     3235,   9086}, // Shinsoo Village 2
			    {"Wae-An",   3531,   8829}, // Shinsoo Village 3
			    {"Joan",     638,    1664}, // Chunjo Village 1
			    {"Bokjung",  1745,   1909}, // Chunjo Village 2
			    {"Bak-Go",   1455,   2400}, // Chunjo Village 3
			    {"Pyungmoo", 9599,   2692}, // Jinno Village 1
			    {"Bakra",    8036,   2984}, // Jinno Village 2
			    {"Jayang",   8639,   2460}, // Jinno Village 3
			    {"Sohan",    4350,   2143}, // Mount Sohan (Ice Mountain)
			};
			int idx  = (int)lua_tonumber(L, 1);

			x = ws[idx].x;
			y = ws[idx].y;
			goto teleport_area;
		}

		else
		{
			const char * arg1 = lua_tostring(L, 1);

			LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg1);

			if (!tch)
			{
				const CCI* pkCCI = P2P_MANAGER::instance().Find(arg1);

				if (pkCCI)
				{
					if (pkCCI->bChannel != g_bChannel)
					{
						ch->ChatPacket(CHAT_TYPE_INFO, "[LS;5201;%d;%d]", pkCCI->bChannel, g_bChannel);
					}
					else
					{

						PIXEL_POSITION pos;

						if (!SECTREE_MANAGER::instance().GetCenterPositionOfMap(pkCCI->lMapIndex, pos))
						{
							ch->ChatPacket(CHAT_TYPE_INFO, "[LS;5079;%d]", pkCCI->lMapIndex);
						}
						else
						{
							ch->ChatPacket(CHAT_TYPE_INFO, "[LS;5274;%d;%d]", pos.x, pos.y);
							ch->WarpSet(pos.x, pos.y);
							lua_pushnumber(L, 1 );
						}
					}
				}
				else if (NULL == CHARACTER_MANAGER::instance().FindPC(arg1))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;5220]");
				}

				lua_pushnumber(L, 0 );

				return 1;
			}
			else
			{
				x = tch->GetX() / 100;
				y = tch->GetY() / 100;
			}
		}

teleport_area:

		x *= 100;
		y *= 100;

		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;5274;%d;%d]", x, y);
		ch->WarpSet(x,y);
		ch->Stop();
		lua_pushnumber(L, 1 );
		return 1;
	}

	int pc_set_skill_level(lua_State* L)
	{
		DWORD dwVnum = (DWORD)lua_tonumber(L, 1);
		BYTE byLev = (BYTE)lua_tonumber(L, 2);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		ch->SetSkillLevel(dwVnum, byLev);

		ch->SkillLevelPacket();

		return 0;
	}

	int pc_give_polymorph_book(lua_State* L)
	{
		if ( !lua_isnumber(L, 1) && !lua_isnumber(L, 2) && !lua_isnumber(L, 3) && !lua_isnumber(L, 4) )
		{
			sys_err("Wrong Quest Function Arguments: pc_give_polymorph_book");
			return 0;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		CPolymorphUtils::instance().GiveBook(ch, (DWORD)lua_tonumber(L, 1), (DWORD)lua_tonumber(L, 2), (BYTE)lua_tonumber(L, 3), (BYTE)lua_tonumber(L, 4));

		return 0;
	}

	int pc_upgrade_polymorph_book(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		LPITEM pItem = CQuestManager::instance().GetCurrentItem();

		bool ret = CPolymorphUtils::instance().BookUpgrade(ch, pItem);

		lua_pushboolean(L, ret);

		return 1;
	}

	int pc_get_premium_remain_sec(lua_State* L)
	{
		int	remain_seconds	= 0;
		int	premium_type	= 0;
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isnumber(L, 1)) 
		{
			sys_err("wrong premium index (is not number)");
			return 0;
		}

		premium_type = (int)lua_tonumber(L,1);
		switch (premium_type)
		{
			case PREMIUM_EXP:
			case PREMIUM_ITEM:
			case PREMIUM_SAFEBOX:
			case PREMIUM_AUTOLOOT:
			case PREMIUM_FISH_MIND:
			case PREMIUM_MARRIAGE_FAST:
			case PREMIUM_GOLD:
				break;

			default:
				sys_err("wrong premium index %d", premium_type);
				return 0;
		}

		remain_seconds = ch->GetPremiumRemainSeconds(premium_type);

		lua_pushnumber(L, remain_seconds);
		return 1;
	}	

	int pc_send_block_mode(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		ch->SetBlockModeForce((BYTE)lua_tonumber(L, 1));

		return 0;
	}

	int pc_change_empire(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		
		lua_pushnumber(L, ch->ChangeEmpire((unsigned char)lua_tonumber(L, 1)));

		return 1;
	}

	int pc_get_change_empire_count(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		lua_pushnumber(L, ch->GetChangeEmpireCount());

		return 1;
	}

	int pc_set_change_empire_count(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		ch->SetChangeEmpireCount();

		return 0;
	}
	
	int pc_change_name(lua_State* L)
	{
		// Return value
		//        0: New name set but user did not log out
		//        1: No string passed from script
		//        2: Failed check_name validation
		//        3: Same name already in use
		//        4: Success
		//        5: Feature not supported

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if ( ch->GetNewName().size() != 0 )
		{
			lua_pushnumber(L, 0);
			return 1;
		}

		if ( !lua_isstring(L, 1) )
		{
			lua_pushnumber(L, 1);
			return 1;
		}

		const char * szName = lua_tostring(L, 1);

		if ( check_name(szName) == false )
		{
			lua_pushnumber(L, 2);
			return 1;
		}

		char szQuery[1024];
		snprintf(szQuery, sizeof(szQuery), "SELECT COUNT(*) FROM player%s WHERE name='%s'", get_table_postfix(), szName);
		std::unique_ptr<SQLMsg> pmsg(DBManager::instance().DirectQuery(szQuery));

		if ( pmsg->Get()->uiNumRows > 0 )
		{
			MYSQL_ROW row = mysql_fetch_row(pmsg->Get()->pSQLResult);

			int	count = 0;
			str_to_number(count, row[0]);

			// There already exists a character with that name
			if ( count != 0 )
			{
				lua_pushnumber(L, 3);
				return 1;
			}
		}

		DWORD pid = ch->GetPlayerID();
		db_clientdesc->DBPacketHeader(HEADER_GD_FLUSH_CACHE, 0, sizeof(DWORD));
		db_clientdesc->Packet(&pid, sizeof(DWORD));

		/* delete messenger list */
		MessengerManager::instance().RemoveAllList(ch->GetName());

		/* change_name_log */
		LogManager::instance().ChangeNameLog(pid, ch->GetName(), szName, ch->GetDesc()->GetHostName());

		snprintf(szQuery, sizeof(szQuery), "UPDATE player%s SET name='%s' WHERE id=%u", get_table_postfix(), szName, pid);
		SQLMsg * msg = DBManager::instance().DirectQuery(szQuery);
		M2_DELETE(msg);

		ch->SetNewName(szName);
		lua_pushnumber(L, 4);
		return 1;
	}

	int pc_is_dead(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if ( ch != NULL )
		{
			lua_pushboolean(L, ch->IsDead());
			return 1;
		}

		lua_pushboolean(L, true);

		return 1;
	}

	int pc_reset_status( lua_State* L )
	{
		if ( lua_isnumber(L, 1) )
		{
			int idx = (int)lua_tonumber(L, 1);

			if ( idx >= 0 && idx < 4 )
			{
				LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
				int point = POINT_NONE;
				char buf[128];

				switch ( idx )
				{
					case 0 : point = POINT_HT; break;
					case 1 : point = POINT_IQ; break;
					case 2 : point = POINT_ST; break;
					case 3 : point = POINT_DX; break;
					default : lua_pushboolean(L, false); return 1;
				}

				int old_val = ch->GetRealPoint(point);
				int old_stat = ch->GetRealPoint(POINT_STAT);

				ch->SetRealPoint(point, 1);
				ch->SetPoint(point, ch->GetRealPoint(point));

				ch->PointChange(POINT_STAT, old_val-1);

				if ( point == POINT_HT )
				{
					BYTE job = ch->GetJob();
					ch->SetRandomHP((ch->GetLevel()-1) * number(JobInitialPoints[job].hp_per_lv_begin, JobInitialPoints[job].hp_per_lv_end));
				}
				else if ( point == POINT_IQ )
				{
					BYTE job = ch->GetJob();
					ch->SetRandomSP((ch->GetLevel()-1) * number(JobInitialPoints[job].sp_per_lv_begin, JobInitialPoints[job].sp_per_lv_end));
				}

				ch->ComputePoints();
				ch->PointsPacket();

				if ( point == POINT_HT )
				{
					ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
				}
				else if ( point == POINT_IQ )
				{
					ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
				}

				switch ( idx )
				{
					case 0 :
						snprintf(buf, sizeof(buf), "reset ht(%d)->1 stat_point(%d)->(%d)", old_val, old_stat, ch->GetRealPoint(POINT_STAT));
						break;
					case 1 :
						snprintf(buf, sizeof(buf), "reset iq(%d)->1 stat_point(%d)->(%d)", old_val, old_stat, ch->GetRealPoint(POINT_STAT));
						break;
					case 2 :
						snprintf(buf, sizeof(buf), "reset st(%d)->1 stat_point(%d)->(%d)", old_val, old_stat, ch->GetRealPoint(POINT_STAT));
						break;
					case 3 :
						snprintf(buf, sizeof(buf), "reset dx(%d)->1 stat_point(%d)->(%d)", old_val, old_stat, ch->GetRealPoint(POINT_STAT));
						break;
				}

				LogManager::instance().CharLog(ch, 0, "RESET_ONE_STATUS", buf);

				lua_pushboolean(L, true);
				return 1;
			}
		}

		lua_pushboolean(L, false);
		return 1;
	}

	int pc_get_ht( lua_State* L )
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushnumber(L, ch->GetRealPoint(POINT_HT));
		return 1;
	}

	int pc_set_ht( lua_State* L )
	{
		if ( lua_isnumber(L, 1) == false )
			return 1;

		int newPoint = (int)lua_tonumber(L, 1);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		int usedPoint = newPoint - ch->GetRealPoint(POINT_HT);
		ch->SetRealPoint(POINT_HT, newPoint);
		ch->PointChange(POINT_HT, 0);
		ch->PointChange(POINT_STAT, -usedPoint);
		ch->ComputePoints();
		ch->PointsPacket();
		return 1;
	}

	int pc_get_iq( lua_State* L )
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushnumber(L, ch->GetRealPoint(POINT_IQ));
		return 1;
	}

	int pc_set_iq( lua_State* L )
	{
		if ( lua_isnumber(L, 1) == false )
			return 1;

		int newPoint = (int)lua_tonumber(L, 1);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		int usedPoint = newPoint - ch->GetRealPoint(POINT_IQ);
		ch->SetRealPoint(POINT_IQ, newPoint);
		ch->PointChange(POINT_IQ, 0);
		ch->PointChange(POINT_STAT, -usedPoint);
		ch->ComputePoints();
		ch->PointsPacket();
		return 1;
	}
	
	int pc_get_st( lua_State* L )
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushnumber(L, ch->GetRealPoint(POINT_ST));
		return 1;
	}

	int pc_set_st( lua_State* L )
	{
		if ( lua_isnumber(L, 1) == false )
			return 1;

		int newPoint = (int)lua_tonumber(L, 1);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		int usedPoint = newPoint - ch->GetRealPoint(POINT_ST);
		ch->SetRealPoint(POINT_ST, newPoint);
		ch->PointChange(POINT_ST, 0);
		ch->PointChange(POINT_STAT, -usedPoint);
		ch->ComputePoints();
		ch->PointsPacket();
		return 1;
	}
	
	int pc_get_dx( lua_State* L )
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushnumber(L, ch->GetRealPoint(POINT_DX));
		return 1;
	}

	int pc_set_dx( lua_State* L )
	{
		if ( lua_isnumber(L, 1) == false )
			return 1;

		int newPoint = (int)lua_tonumber(L, 1);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		int usedPoint = newPoint - ch->GetRealPoint(POINT_DX);
		ch->SetRealPoint(POINT_DX, newPoint);
		ch->PointChange(POINT_DX, 0);
		ch->PointChange(POINT_STAT, -usedPoint);
		ch->ComputePoints();
		ch->PointsPacket();
		return 1;
	}

	int pc_is_near_vid( lua_State* L )
	{
		if ( !lua_isnumber(L, 1) || !lua_isnumber(L, 2) )
		{
			lua_pushboolean(L, false);
		}
		else
		{
			LPCHARACTER pMe = CQuestManager::instance().GetCurrentCharacterPtr();
			LPCHARACTER pOther = CHARACTER_MANAGER::instance().Find( (DWORD)lua_tonumber(L, 1) );

			if ( pMe != NULL && pOther != NULL )
			{
				lua_pushboolean(L, (DISTANCE_APPROX(pMe->GetX() - pOther->GetX(), pMe->GetY() - pOther->GetY()) < (int)lua_tonumber(L, 2)*100));
			}
			else
			{
				lua_pushboolean(L, false);
			}
		}

		return 1;
	}

	int pc_get_socket_items( lua_State* L )
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		lua_newtable( L );

		if ( pChar == NULL ) return 1;

		int idx = 1;

		// I don't think there's a need for the Dragon Soul Stone slot. .
		// This function seems to be a function for declassification. .
		for ( int i=0; i < INVENTORY_MAX_NUM + WEAR_MAX_NUM; i++ )
		{
			LPITEM pItem = pChar->GetInventoryItem(i);

			if ( pItem != NULL )
			{
				if ( pItem->IsEquipped() == false )
				{
					int j = 0;
					for (; j < ITEM_SOCKET_MAX_NUM; j++ )
					{
						TimeT64 socket = pItem->GetSocket(j);

						if ( socket > 2 && socket != ITEM_BROKEN_METIN_VNUM )
						{
							TItemTable* pItemInfo = ITEM_MANAGER::instance().GetTable( static_cast<DWORD>(socket) );
							if ( pItemInfo != NULL )
							{
								if ( pItemInfo->bType == ITEM_METIN ) break;
							}
						}
					}

					if ( j >= ITEM_SOCKET_MAX_NUM ) continue;

					lua_newtable( L );

					{
						lua_pushstring( L, pItem->GetName() );
						lua_rawseti( L, -2, 1 );

						lua_pushnumber( L, i );
						lua_rawseti( L, -2, 2 );
					}

					lua_rawseti( L, -2, idx++ );
				}
			}
		}

		return 1;
	}

	int pc_get_empty_inventory_count( lua_State* L )
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		if ( pChar != NULL )
		{
			lua_pushnumber(L, pChar->CountEmptyInventory());
		}
		else
		{
			lua_pushnumber(L, 0);
		}

		return 1;
	}

	int pc_get_logoff_interval( lua_State* L )
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		if ( pChar != NULL )
		{
			lua_pushnumber(L, pChar->GetLogOffInterval());
		}
		else
		{
			lua_pushnumber(L, 0);
		}

		return 1;
	}

	int pc_get_player_id( lua_State* L )
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		if ( pChar != NULL )
		{
			lua_pushnumber( L, pChar->GetPlayerID() );
		}
		else
		{
			lua_pushnumber( L, 0 );
		}

		return 1;
	}

	int pc_get_account_id( lua_State* L )
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		if ( pChar != NULL )
		{
			if ( pChar->GetDesc() != NULL )
			{
				lua_pushnumber( L, pChar->GetDesc()->GetAccountTable().id );
				return 1;
			}
		}

		lua_pushnumber( L, 0 );
		return 1;
	}

	int pc_get_account( lua_State* L )
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		if( NULL != pChar )
		{
			if( NULL != pChar->GetDesc() )
			{
				lua_pushstring( L, pChar->GetDesc()->GetAccountTable().login );
				return 1;
			}
		}

		lua_pushstring( L, "" );
		return 1;
	}

	int pc_is_riding(lua_State* L)
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		if( NULL != pChar )
		{
			bool is_riding = pChar->IsRiding();

			lua_pushboolean(L, is_riding);

			return 1;
		}

		lua_pushboolean(L, false);
		return 1;
	}

	int pc_get_special_ride_vnum(lua_State* L)
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		if (NULL != pChar)
		{
			LPITEM Unique1 = pChar->GetWear(WEAR_UNIQUE1);
			LPITEM Unique2 = pChar->GetWear(WEAR_UNIQUE2);

			if (NULL != Unique1)
			{
				if (UNIQUE_GROUP_SPECIAL_RIDE == Unique1->GetSpecialGroup())
				{
					lua_pushnumber(L, Unique1->GetVnum());
					lua_pushnumber(L, static_cast<lua_Number>(Unique1->GetSocket(2)));
					return 2;
				}
			}

			if (NULL != Unique2)
			{
				if (UNIQUE_GROUP_SPECIAL_RIDE == Unique2->GetSpecialGroup())
				{
					lua_pushnumber(L, Unique2->GetVnum());
					lua_pushnumber(L, static_cast<lua_Number>(Unique2->GetSocket(2)));
					return 2;
				}
			}
		}

		lua_pushnumber(L, 0);
		lua_pushnumber(L, 0);

		return 2;
	}

	int pc_can_warp(lua_State* L)
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		if (NULL != pChar)
		{
			lua_pushboolean(L, pChar->CanWarp());
		}
		else
		{
			lua_pushboolean(L, false);
		}

		return 1;
	}

	int pc_dec_skill_point(lua_State* L)
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		if (NULL != pChar)
		{
			pChar->PointChange(POINT_SKILL, -1);
		}

		return 0;
	}

	int pc_get_skill_point(lua_State* L)
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		if (NULL != pChar)
		{
			lua_pushnumber(L, static_cast<lua_Number>(pChar->GetPoint(POINT_SKILL)));
		}
		else
		{
			lua_pushnumber(L, 0);
		}

		return 1;
	}

	int pc_get_channel_id(lua_State* L)
	{
		lua_pushnumber(L, g_bChannel);

		return 1;
	}

	int pc_give_poly_marble(lua_State* L)
	{
		const int dwVnum = static_cast<int>(lua_tonumber(L, 1));

		const CMob* MobInfo = CMobManager::instance().Get(dwVnum);

		if (NULL == MobInfo)
		{
			lua_pushboolean(L, false);
			return 1;
		}

		if (0 == MobInfo->m_table.dwPolymorphItemVnum)
		{
			lua_pushboolean(L, false);
			return 1;
		}

		LPITEM item = ITEM_MANAGER::instance().CreateItem( MobInfo->m_table.dwPolymorphItemVnum );

		if (NULL == item)
		{
			lua_pushboolean(L, false);
			return 1;
		}

		item->SetSocket(0, dwVnum);

		const LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		int iEmptyCell = ch->GetEmptyInventory(item->GetSize());

		if (-1 == iEmptyCell)
		{
			M2_DESTROY_ITEM(item);
			lua_pushboolean(L, false);
			return 1;
		}

		item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyCell));

		const PC* pPC = CQuestManager::instance().GetCurrentPC();

		LogManager::instance().QuestRewardLog(pPC->GetCurrentQuestName().c_str(), ch->GetPlayerID(), ch->GetLevel(), MobInfo->m_table.dwPolymorphItemVnum, dwVnum);

		lua_pushboolean(L, true);

		return 1;
	}

	int pc_get_sig_items (lua_State* L)
	{
		DWORD group_vnum = (DWORD)lua_tonumber (L, 1);
		const LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		int count = 0;
		for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
		{
			if (ch->GetInventoryItem(i) != NULL && ch->GetInventoryItem(i)->GetSIGVnum() == group_vnum)
			{
				lua_pushnumber(L, ch->GetInventoryItem(i)->GetID());
				count++;
			}
		}

		return count;
	}		

	int pc_charge_cash(lua_State * L)
	{
		TRequestChargeCash packet;

		int amount = lua_isnumber(L, 1) ? (int)lua_tonumber(L, 1) : 0;
		std::string strChargeType = lua_isstring(L, 2) ? lua_tostring(L, 2) : "";

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		
		if (NULL == ch || NULL == ch->GetDesc() || 1 > amount || 50000 < amount)
		{
			lua_pushboolean(L, 0);
			return 1;
		}

		packet.dwAID = ch->GetDesc()->GetAccountTable().id;
		packet.dwAmount = (DWORD)amount;
		packet.eChargeType = ERequestCharge_Cash;

		if (0 < strChargeType.length())
			std::transform(strChargeType.begin(), strChargeType.end(), strChargeType.begin(), (int(*)(int))std::tolower);

		if ("mileage" == strChargeType)
			packet.eChargeType = ERequestCharge_Mileage;

		db_clientdesc->DBPacketHeader(HEADER_GD_REQUEST_CHARGE_CASH, 0, sizeof(TRequestChargeCash));
		db_clientdesc->Packet(&packet, sizeof(packet));

		lua_pushboolean(L, 1);
		return 1;
	}

	int pc_give_award(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isstring(L, 3) )
		{
			sys_err("QUEST give award call error : wrong argument");
			lua_pushnumber (L, 0);
			return 1;
		}

		DWORD dwVnum = (int) lua_tonumber(L, 1);

		int icount = (int) lua_tonumber(L, 2);

		sys_log(0, "QUEST [award] item %d to login %s", dwVnum, ch->GetDesc()->GetAccountTable().login);

		DBManager::instance().Query("INSERT INTO item_award (login, vnum, count, given_time, why, mall)select '%s', %d, %d, now(), '%s', 1 from DUAL where not exists (select login, why from item_award where login = '%s' and why  = '%s') ;", 
			ch->GetDesc()->GetAccountTable().login,
			dwVnum, 
			icount, 
			lua_tostring(L,3),
			ch->GetDesc()->GetAccountTable().login,
			lua_tostring(L,3));

		lua_pushnumber (L, 0);
		return 1;
	}
	int pc_give_award_socket(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isstring(L, 3) || !lua_isstring(L, 4) || !lua_isstring(L, 5) || !lua_isstring(L, 6) )
		{
			sys_err("QUEST give award call error : wrong argument");
			lua_pushnumber (L, 0);
			return 1;
		}

		DWORD dwVnum = (int) lua_tonumber(L, 1);

		int icount = (int) lua_tonumber(L, 2);

		sys_log(0, "QUEST [award] item %d to login %s", dwVnum, ch->GetDesc()->GetAccountTable().login);

		DBManager::instance().Query("INSERT INTO item_award (login, vnum, count, given_time, why, mall, socket0, socket1, socket2)select '%s', %d, %d, now(), '%s', 1, %s, %s, %s from DUAL where not exists (select login, why from item_award where login = '%s' and why  = '%s') ;", 
			ch->GetDesc()->GetAccountTable().login,
			dwVnum, 
			icount, 
			lua_tostring(L,3),
			lua_tostring(L,4),
			lua_tostring(L,5),
			lua_tostring(L,6),
			ch->GetDesc()->GetAccountTable().login,
			lua_tostring(L,3));

		lua_pushnumber (L, 0);
		return 1;
	}

	int pc_get_informer_type(lua_State* L)	// germany gift feature
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		if( pChar != NULL )
		{
			//sys_err("quest cmd test %s", pChar->GetItemAward_cmd() );
			lua_pushstring(L, pChar->GetItemAward_cmd() );
		}
		else
			lua_pushstring(L, "" );

		return 1;
	}

	int pc_get_informer_item(lua_State* L)
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		if( pChar != NULL )
		{
			lua_pushnumber(L, pChar->GetItemAward_vnum() );
		}
		else
			lua_pushnumber(L,0);

		return 1;
	}

	int pc_get_killee_drop_pct(lua_State* L)
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();
		LPCHARACTER pKillee = pChar->GetQuestNPC();

		int iDeltaPercent, iRandRange;
		if (NULL == pKillee || !ITEM_MANAGER::instance().GetDropPct(pKillee, pChar, iDeltaPercent, iRandRange))
		{
			sys_err("killee is null");
			lua_pushnumber(L, -1);
			lua_pushnumber(L, -1);

			return 2;
		}

		lua_pushnumber(L, iDeltaPercent);
		lua_pushnumber(L, iRandRange);
		
		return 2;
	}

	void RegisterPCFunctionTable()
	{
		luaL_reg pc_functions[] = 
		{
			{ "get_wear",		pc_get_wear			},
			{ "get_player_id",	pc_get_player_id	},
			{ "get_account_id", pc_get_account_id	},
			{ "get_account",	pc_get_account		},
			{ "get_level",		pc_get_level		},
			{ "set_level",		pc_set_level		},
			{ "get_next_exp",		pc_get_next_exp		},
			{ "get_exp",		pc_get_exp		},
			{ "get_job",		pc_get_job		},
			{ "get_race",		pc_get_race		},
			{ "change_sex",		pc_change_sex	},
			{ "gethp",			pc_get_hp		},
			{ "get_hp",			pc_get_hp		},
			{ "getmaxhp",		pc_get_max_hp		},
			{ "get_max_hp",		pc_get_max_hp		},
			{ "getsp",			pc_get_sp		},
			{ "get_sp",			pc_get_sp		},
			{ "getmaxsp",		pc_get_max_sp		},
			{ "get_max_sp",		pc_get_max_sp		},
			{ "change_sp",		pc_change_sp		},
			{ "getmoney",		pc_get_money		},
			{ "get_money",		pc_get_money		},
			{ "get_real_alignment",	pc_get_real_alignment	},
			{ "get_alignment",		pc_get_alignment	},
			{ "getweapon",		pc_get_weapon		},
			{ "get_weapon",		pc_get_weapon		},
			{ "getarmor",		pc_get_armor		},
			{ "get_armor",		pc_get_armor		},
			{ "getgold",		pc_get_money		},
			{ "get_gold",		pc_get_money		},
			{ "changegold",		pc_change_money		},
			{ "changemoney",		pc_change_money		},
			{ "changealignment",	pc_change_alignment	},
			{ "change_gold",		pc_change_money		},
			{ "change_money",		pc_change_money		},
			{ "change_alignment",	pc_change_alignment	},
			{ "getname",		pc_get_name		},
			{ "get_name",		pc_get_name		},
			{ "get_vid",		pc_get_vid		},
			{ "getplaytime",		pc_get_playtime		},
			{ "get_playtime",		pc_get_playtime		},
			{ "getleadership",		pc_get_leadership	},
			{ "get_leadership",		pc_get_leadership	},
			{ "getqf",			pc_get_quest_flag	},
			{ "setqf",			pc_set_quest_flag	},
			{ "delqf",			pc_del_quest_flag	},
			{ "getf",			pc_get_another_quest_flag},
			{ "setf",			pc_set_another_quest_flag},
			{ "get_x",			pc_get_x		},
			{ "get_y",			pc_get_y		},
			{ "getx",			pc_get_x		},
			{ "gety",			pc_get_y		},
			{ "get_local_x",		pc_get_local_x		},
			{ "get_local_y",		pc_get_local_y		},
			{ "getcurrentmapindex",	pc_get_current_map_index},
			{ "get_map_index",		pc_get_current_map_index},
			{ "give_exp",		pc_give_exp		},
			{ "give_exp_perc",		pc_give_exp_perc	},
			{ "give_exp2",		pc_give_exp2		},
			{ "give_item",		pc_give_item		},
			{ "give_item2",		pc_give_or_drop_item	},
			{ "give_item2_select",		pc_give_or_drop_item_and_select	},
			{ "give_gold",		pc_give_gold		},
			{ "count_item",		pc_count_item		},
			{ "remove_item",		pc_remove_item		},
			{ "countitem",		pc_count_item		},
			{ "removeitem",		pc_remove_item		},
			{ "reset_point",		pc_reset_point		},
			{ "has_guild",		pc_hasguild		},
			{ "hasguild",		pc_hasguild		},
			{ "get_guild",		pc_getguild		},
			{ "getguild",		pc_getguild		},
			{ "isguildmaster",		pc_isguildmaster	},
			{ "is_guild_master",	pc_isguildmaster	},
			{ "destroy_guild",		pc_destroy_guild	},
			{ "remove_from_guild",	pc_remove_from_guild	},
			{ "in_dungeon",		pc_in_dungeon		},
			{ "getempire",		pc_get_empire		},
			{ "get_empire",		pc_get_empire		},
			{ "get_skill_group",	pc_get_skillgroup	},
			{ "set_skill_group",	pc_set_skillgroup	},
			{ "warp",			pc_warp			},
			{ "warp_local",		pc_warp_local		},
			{ "warp_exit",		pc_warp_exit		},
			{ "set_warp_location",	pc_set_warp_location	},
			{ "set_warp_location_local",pc_set_warp_location_local },
			{ "get_start_location",	pc_get_start_location	},
			{ "has_master_skill",	pc_has_master_skill	},
			{ "set_part",		pc_set_part		},
			{ "get_part",		pc_get_part		},
			{ "is_polymorphed",		pc_is_polymorphed	},
			{ "remove_polymorph",	pc_remove_polymorph	},
			{ "is_mount",		pc_is_mount		},
			{ "polymorph",		pc_polymorph		},
			{ "mount",			pc_mount		},
			{ "mount_bonus",	pc_mount_bonus	},
			{ "unmount",		pc_unmount		},
			{ "warp_to_guild_war_observer_position", pc_warp_to_guild_war_observer_position	},
			{ "give_item_from_special_item_group", pc_give_item_from_special_item_group	},
			{ "learn_grand_master_skill", pc_learn_grand_master_skill	},
			{ "is_skill_book_no_delay",	pc_is_skill_book_no_delay}, 
			{ "remove_skill_book_no_delay",	pc_remove_skill_book_no_delay}, 

			{ "enough_inventory",	pc_enough_inventory	},
			{ "get_horse_level",	pc_get_horse_level	}, // TO BE DELETED XXX
			{ "is_horse_alive",		pc_is_horse_alive	}, // TO BE DELETED XXX
			{ "revive_horse",		pc_revive_horse		}, // TO BE DELETED XXX
			{ "have_pos_scroll",	pc_have_pos_scroll	},
			{ "have_map_scroll",	pc_have_map_scroll	},
			{ "get_war_map",		pc_get_war_map		},
			{ "get_equip_refine_level",	pc_get_equip_refine_level },
			{ "refine_equip",		pc_refine_equip		},
			{ "get_skill_level",	pc_get_skill_level	},
			{ "aggregate_monster",	pc_aggregate_monster	},
			{ "forget_my_attacker",	pc_forget_my_attacker	},
			{ "pc_attract_ranger",	pc_attract_ranger	},
			{ "select",			pc_select_vid		},
			{ "get_sex",		pc_get_sex		},
			{ "is_married",		pc_is_married		},
			{ "is_engaged",		pc_is_engaged		},
			{ "is_engaged_or_married",	pc_is_engaged_or_married},
			{ "is_gm",			pc_is_gm		},
			{ "get_gm_level",		pc_get_gm_level		},
			{ "mining",			pc_mining		},
			{ "ore_refine",		pc_ore_refine		},
			{ "diamond_refine",		pc_diamond_refine	},

			// RESET_ONE_SKILL
			{ "clear_one_skill",        pc_clear_one_skill      },
			// END_RESET_ONE_SKILL

			{ "clear_skill",                pc_clear_skill          },
			{ "clear_sub_skill",    pc_clear_sub_skill      },
			{ "set_skill_point",    pc_set_skill_point      },

			{ "is_clear_skill_group",	pc_is_clear_skill_group		},

			{ "save_exit_location",		pc_save_exit_location		},
			{ "teleport",				pc_teleport },

			{ "set_skill_level",        pc_set_skill_level      },

            { "give_polymorph_book",    pc_give_polymorph_book  },
            { "upgrade_polymorph_book", pc_upgrade_polymorph_book },
            { "get_premium_remain_sec", pc_get_premium_remain_sec },
   
			{ "send_block_mode",		pc_send_block_mode	},
			
			{ "change_empire",			pc_change_empire	},
			{ "get_change_empire_count",	pc_get_change_empire_count	},
			{ "set_change_empire_count",	pc_set_change_empire_count	},

			{ "change_name",			pc_change_name },

			{ "is_dead",				pc_is_dead	},

			{ "reset_status",		pc_reset_status	},
			{ "get_ht",				pc_get_ht	},
			{ "set_ht",				pc_set_ht	},
			{ "get_iq",				pc_get_iq	},
			{ "set_iq",				pc_set_iq	},
			{ "get_st",				pc_get_st	},
			{ "set_st",				pc_set_st	},
			{ "get_dx",				pc_get_dx	},
			{ "set_dx",				pc_set_dx	},

			{ "is_near_vid",		pc_is_near_vid	},

			{ "get_socket_items",	pc_get_socket_items	},
			{ "get_empty_inventory_count",	pc_get_empty_inventory_count	},

			{ "get_logoff_interval",	pc_get_logoff_interval	},

			{ "is_riding",			pc_is_riding	},
			{ "get_special_ride_vnum",	pc_get_special_ride_vnum	},

			{ "can_warp",			pc_can_warp		},

			{ "dec_skill_point",	pc_dec_skill_point	},
			{ "get_skill_point",	pc_get_skill_point	},

			{ "get_channel_id",		pc_get_channel_id	},

			{ "give_poly_marble",	pc_give_poly_marble	},
			{ "get_sig_items",		pc_get_sig_items	},

			{ "charge_cash",		pc_charge_cash		},
			
			{ "get_informer_type",	pc_get_informer_type	},	// germany gift feature
			{ "get_informer_item",  pc_get_informer_item	},

			{ "give_award",			pc_give_award			},	// Gold bars paid once per Japanese account
			{ "give_award_socket",	pc_give_award_socket	},	// Provide items to mall inventory . Function for configuring socket .

			{ "get_killee_drop_pct",	pc_get_killee_drop_pct	}, /* mob_vnum.kill at the event killee and pc with level difference , pc Item drop probability considering premium drop rate, etc. .
																    * return The value is ( molecule , denominator ).
																    * ( The words are complicated , CreateDropItem of GetDropPct of iDeltaPercent, iRandRange cast return You can think of it as doing it. .)
																	* ( I wonder if this would be more difficult T_T )
																	* caution : kill event For use only in !
																	*/

			{ NULL,			NULL			}
		};

		CQuestManager::instance().AddLuaFunctionTable("pc", pc_functions);
	}
};
