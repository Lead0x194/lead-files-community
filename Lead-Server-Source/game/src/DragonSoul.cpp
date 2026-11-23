#include "stdafx.h"
#include "constants.h"
#include "item.h"
#include "item_manager.h"
#include "unique_item.h"
#include "packet.h"
#include "desc.h"
#include "char.h"
#include "dragon_soul_table.h"
#include "log.h"
#include "DragonSoul.h"
#include <boost/lexical_cast.hpp>

typedef std::vector <std::string> TTokenVector;

int Gamble(std::vector<float>& vec_probs)
{
	float range = 0.f;
	for (int i = 0; i < vec_probs.size(); i++)
	{
		range += vec_probs[i];
	}
	float fProb = fnumber(0.f, range);
	float sum = 0.f;
	for (int idx = 0; idx < vec_probs.size(); idx++)
	{
		sum += vec_probs[idx];
		if (sum >= fProb)
			return idx;
	}
	return -1;
}

// weight table (prob_lst) take it random_set.size() doggy index By selecting random_set second return
bool MakeDistinctRandomNumberSet(std::list <float> prob_lst, OUT std::vector<int>& random_set)
{
	int size = static_cast<int>(prob_lst.size());
	int n = static_cast<int>(random_set.size());
	if (size < n)
		return false;

	std::vector <int> select_bit(size, 0);
	for (int i = 0; i < n; i++)
	{
		float range = 0.f;
		for (std::list <float>::iterator it = prob_lst.begin(); it != prob_lst.end(); it++)
		{
			range += *it;
		}
		float r = fnumber (0.f, range);
		float sum = 0.f;
		int idx = 0;
		for (std::list <float>::iterator it = prob_lst.begin(); it != prob_lst.end(); it++)
		{
			while (select_bit[idx++]);

			sum += *it;
			if (sum >= r)
			{
				select_bit[idx - 1] = 1;
				random_set[i] = idx - 1;
				prob_lst.erase(it);
				break;
			}
		}
	}
	return true;
}

/* dragon soul stone Vnum for comment	
 * ITEM VNUM second 10 From 10,000 digits , FEDCBA If you say
 * FE : Dragon Soul Stone Type .	D : rating
 * C : step			B : enforce		
 * A : extra numbers ... 	
 */

BYTE GetType(DWORD dwVnum)
{
	return static_cast<BYTE>(dwVnum / 10000);
}

BYTE GetGradeIdx(DWORD dwVnum)
{
	return (dwVnum / 1000) % 10;
}

BYTE GetStepIdx(DWORD dwVnum)
{
	return (dwVnum / 100) % 10;
}

BYTE GetStrengthIdx(DWORD dwVnum)
{
	return (dwVnum / 10) % 10;
}

bool DSManager::ReadDragonSoulTableFile(const char * c_pszFileName)
{
	m_pTable = new DragonSoulTable();
	return m_pTable->ReadDragonSoulTableFile(c_pszFileName);
}

void DSManager::GetDragonSoulInfo(DWORD dwVnum, BYTE& bType, BYTE& bGrade, BYTE& bStep, BYTE& bStrength) const
{
	bType = GetType(dwVnum);
	bGrade = GetGradeIdx(dwVnum);
	bStep = GetStepIdx(dwVnum);
	bStrength = GetStrengthIdx(dwVnum);
}

bool DSManager::IsValidCellForThisItem(const LPITEM pItem, const TItemPos& Cell) const
{
	if (NULL == pItem)
		return false;

	WORD wBaseCell = GetBasePosition(pItem);
	if (WORD_MAX == wBaseCell)
		return false;

	if (Cell.window_type != DRAGON_SOUL_INVENTORY
		|| (Cell.cell < wBaseCell || Cell.cell >= wBaseCell + DRAGON_SOUL_BOX_SIZE))
	{
		return false;
	}
	else
		return true;

}

WORD DSManager::GetBasePosition(const LPITEM pItem) const
{
	if (NULL == pItem)
		return WORD_MAX;

	BYTE type, grade_idx, step_idx, strength_idx;
	GetDragonSoulInfo(pItem->GetVnum(), type, grade_idx, step_idx, strength_idx);

	BYTE col_type = pItem->GetSubType();
	BYTE row_type = grade_idx;
	if (row_type > DRAGON_SOUL_GRADE_MAX)
		return WORD_MAX;

	return col_type * DRAGON_SOUL_STEP_MAX * DRAGON_SOUL_BOX_SIZE + row_type * DRAGON_SOUL_BOX_SIZE;
}

bool DSManager::RefreshItemAttributes(LPITEM pDS)
{
	if (!pDS->IsDragonSoul())
	{
		sys_err ("This item(ID : %d) is not DragonSoul.", pDS->GetID());
		return false;
	}

	BYTE ds_type, grade_idx, step_idx, strength_idx;
	GetDragonSoulInfo(pDS->GetVnum(), ds_type, grade_idx, step_idx, strength_idx);

	DragonSoulTable::TVecApplys vec_basic_applys;
	DragonSoulTable::TVecApplys vec_addtional_applys;

	if (!m_pTable->GetBasicApplys(ds_type, vec_basic_applys))
	{
		sys_err ("There is no BasicApply about %d type dragon soul.", ds_type);
		return false;
	}

	if (!m_pTable->GetAdditionalApplys(ds_type, vec_addtional_applys))
	{
		sys_err ("There is no AdditionalApply about %d type dragon soul.", ds_type);
		return false;
	}

	// add_min class add_max is read as a dummy .
	int basic_apply_num, add_min, add_max;
	if (!m_pTable->GetApplyNumSettings(ds_type, grade_idx, basic_apply_num, add_min, add_max))
	{
		sys_err ("In ApplyNumSettings, INVALID VALUES Group type(%d), GRADE idx(%d)", ds_type, grade_idx);
		return false;
	}

	float fWeight = 0.f;
	if (!m_pTable->GetWeight(ds_type, grade_idx, step_idx, strength_idx, fWeight))
	{
		return false;
	}
	fWeight /= 100.f;

	int n = MIN(basic_apply_num, static_cast<int>(vec_basic_applys.size()));
	for (int i = 0; i < n; i++)
	{	
		const SApply& basic_apply = vec_basic_applys[i];
		BYTE bType = basic_apply.apply_type;
		short sValue = (short)(ceil((float)basic_apply.apply_value * fWeight - 0.01f));

		pDS->SetForceAttribute(i, bType, sValue);
	}

	for (int i = DRAGON_SOUL_ADDITIONAL_ATTR_START_IDX; i < ITEM_ATTRIBUTE_MAX_NUM; i++)
	{
		BYTE bType = pDS->GetAttributeType(i);
		short sValue = 0;
		if (APPLY_NONE == bType)
			continue;
		for (int j = 0; j < vec_addtional_applys.size(); j++)
		{
			if (vec_addtional_applys[j].apply_type == bType)
			{
				sValue = vec_addtional_applys[j].apply_value;
				break;
			}
		}
		pDS->SetForceAttribute(i, bType, (short)(ceil((float)sValue * fWeight - 0.01f)));
	}
	return true;
}

bool DSManager::PutAttributes(LPITEM pDS)
{
	if (!pDS->IsDragonSoul())
	{
		sys_err ("This item(ID : %d) is not DragonSoul.", pDS->GetID());
		return false;
	}

	BYTE ds_type, grade_idx, step_idx, strength_idx;
	GetDragonSoulInfo(pDS->GetVnum(), ds_type, grade_idx, step_idx, strength_idx);

	DragonSoulTable::TVecApplys vec_basic_applys;
	DragonSoulTable::TVecApplys vec_addtional_applys;

	if (!m_pTable->GetBasicApplys(ds_type, vec_basic_applys))
	{
		sys_err ("There is no BasicApply about %d type dragon soul.", ds_type);
		return false;
	}
	if (!m_pTable->GetAdditionalApplys(ds_type, vec_addtional_applys))
	{
		sys_err ("There is no AdditionalApply about %d type dragon soul.", ds_type);
		return false;
	}

	
	int basic_apply_num, add_min, add_max;
	if (!m_pTable->GetApplyNumSettings(ds_type, grade_idx, basic_apply_num, add_min, add_max))
	{
		sys_err ("In ApplyNumSettings, INVALID VALUES Group type(%d), GRADE idx(%d)", ds_type, grade_idx);
		return false;
	}

	float fWeight = 0.f;
	if (!m_pTable->GetWeight(ds_type, grade_idx, step_idx, strength_idx, fWeight))
	{
		return false;
	}
	fWeight /= 100.f;

	int n = MIN(basic_apply_num, static_cast<int>(vec_basic_applys.size()));
	for (int i = 0; i < n; i++)
	{	
		const SApply& basic_apply = vec_basic_applys[i];
		BYTE bType = basic_apply.apply_type;
		short sValue = (short)(ceil((float)basic_apply.apply_value * fWeight - 0.01f));

		pDS->SetForceAttribute(i, bType, sValue);
	}

	BYTE additional_attr_num = MIN(number (add_min, add_max), 3);

	std::vector <int> random_set;
	if (additional_attr_num > 0)
	{
		random_set.resize(additional_attr_num);
		std::list <float> list_probs;
		for (int i = 0; i < vec_addtional_applys.size(); i++)
		{
			list_probs.push_back(vec_addtional_applys[i].prob);
		}
		if (!MakeDistinctRandomNumberSet(list_probs, random_set))
		{
			sys_err ("MakeDistinctRandomNumberSet error.");
			return false;
		}

		for (int i = 0; i < additional_attr_num; i++)
		{
			int r = random_set[i];
			const SApply& additional_attr = vec_addtional_applys[r];
			BYTE bType = additional_attr.apply_type;
			short sValue = (short)(ceil((float)additional_attr.apply_value * fWeight - 0.01f));

			pDS->SetForceAttribute(DRAGON_SOUL_ADDITIONAL_ATTR_START_IDX + i, bType, sValue);
		}
	}

	return true;
}

bool DSManager::DragonSoulItemInitialize(LPITEM pItem)
{
	if (NULL == pItem || !pItem->IsDragonSoul())
		return false;
	PutAttributes(pItem);
	int time = DSManager::instance().GetDuration(pItem);
	if (time > 0)
		pItem->SetSocket(ITEM_SOCKET_REMAIN_SEC, time);
	return true;
}

DWORD DSManager::MakeDragonSoulVnum(BYTE bType, BYTE grade, BYTE step, BYTE refine)
{
	return bType * 10000 + grade * 1000 + step * 100 + refine * 10;
}

int DSManager::GetDuration(const LPITEM pItem) const
{
	return pItem->GetDuration();
}

// Function to receive dragon soul and extract dragon heart
bool DSManager::ExtractDragonHeart(LPCHARACTER ch, LPITEM pItem, LPITEM pExtractor)
{
	if (NULL == ch || NULL == pItem)
		return false;
	if (pItem->IsEquipped())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1091]");
		return false;
	}

	DWORD dwVnum = pItem->GetVnum();
	BYTE ds_type, grade_idx, step_idx, strength_idx;
	GetDragonSoulInfo(dwVnum, ds_type, grade_idx, step_idx, strength_idx);

	int iBonus = 0;

	if (NULL != pExtractor)
	{
		iBonus = pExtractor->GetValue(0);
	}

	std::vector <float> vec_chargings;
	std::vector <float> vec_probs;

	if (!m_pTable->GetDragonHeartExtValues(ds_type, grade_idx, vec_chargings, vec_probs))
	{
		return false;
	}

	int idx = Gamble(vec_probs);

	float sum = 0.f;
	if (-1 == idx)
	{
		sys_err ("Gamble is failed. ds_type(%d), grade_idx(%d)", ds_type, grade_idx);
		return false;
	}

	float fCharge = vec_chargings[idx] * (100 + iBonus) / 100.f;
	fCharge = std::MINMAX <float> (0.f, fCharge, 100.f);

	if (fCharge < FLT_EPSILON)
	{
		pItem->SetCount(pItem->GetCount() - 1);
		if (NULL != pExtractor)
		{
			pExtractor->SetCount(pExtractor->GetCount() - 1);
		}
		LogManager::instance().ItemLog(ch, pItem, "DS_HEART_EXTRACT_FAIL", "");
	
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1073]");
		return false;
	}
	else
	{
		LPITEM pDH = ITEM_MANAGER::instance().CreateItem(DRAGON_HEART_VNUM);
		
		if (NULL == pDH)
		{
			sys_err ("Cannot create DRAGON_HEART(%d).", DRAGON_HEART_VNUM);
			return false;
		}

		pItem->SetCount(pItem->GetCount() - 1);
		if (NULL != pExtractor)
		{
			pExtractor->SetCount(pExtractor->GetCount() - 1);
		}

		int iCharge = (int)(fCharge + 0.5f);
		pDH->SetSocket(ITEM_SOCKET_CHARGING_AMOUNT_IDX, iCharge);
		ch->AutoGiveItem(pDH, true);

		std::string s = boost::lexical_cast <std::string> (iCharge);
		s += "%s";
		LogManager::instance().ItemLog(ch, pItem, "DS_HEART_EXTRACT_SUCCESS", s.c_str());
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1072]");
		return true;
	}
}

// Determines success when removing a specific Dragon Soul Stone from the equipment window. , Function that gives byproducts in case of failure .
bool DSManager::PullOut(LPCHARACTER ch, TItemPos DestCell, LPITEM& pItem, LPITEM pExtractor)
{
	if (NULL == ch || NULL == pItem)
	{
		sys_err ("NULL POINTER. ch(%p) or pItem(%p)", ch, pItem);
		return false;
	}

	// target location valid After Hanji inspection , valid If not, it searches for a random empty space. .
	if (!IsValidCellForThisItem(pItem, DestCell))
	{
		int iEmptyCell = ch->GetEmptyDragonSoulInventory(pItem);
		if (iEmptyCell < 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1130]");
			return false;
		}
		else
		{
			DestCell.window_type = DRAGON_SOUL_INVENTORY;
			DestCell.cell = iEmptyCell;
		}
	}

	if (!pItem->IsEquipped() || !pItem->RemoveFromCharacter())
		return false;

	bool bSuccess;
	DWORD dwByProduct = 0;
	int iBonus = 0;
	float fProb;
	float fDice;
	// Determination of success or failure of Dragon Soul Stone extraction .
	{
		DWORD dwVnum = pItem->GetVnum(); 

		BYTE ds_type, grade_idx, step_idx, strength_idx;
		GetDragonSoulInfo(pItem->GetVnum(), ds_type, grade_idx, step_idx, strength_idx);

		// If there is no information to extract, let's assume that success is guaranteed. .
		if (!m_pTable->GetDragonSoulExtValues(ds_type, grade_idx, fProb, dwByProduct))
		{
			pItem->AddToCharacter(ch, DestCell);
			return true;
		}


		if (NULL != pExtractor)
		{
			iBonus = pExtractor->GetValue(ITEM_VALUE_DRAGON_SOUL_POLL_OUT_BONUS_IDX);
			pExtractor->SetCount(pExtractor->GetCount() - 1);
		}
		fDice = fnumber(0.f, 100.f);
		bSuccess = fDice <= (fProb * (100 + iBonus) / 100.f);
	}

	// Extracting and adding or removing a character’s Dragon Soul Stone . By-product provision .
	{
		char buf[128];

		if (bSuccess)
		{
			if (pExtractor)
			{
				snprintf(buf, sizeof(buf), "dice(%d) prob(%d + %d) EXTR(VN:%d)", (int)fDice, (int)fProb, iBonus, pExtractor->GetVnum());
			}
			else
			{
				snprintf(buf, sizeof(buf), "dice(%f) prob(%f)", fDice, fProb);
			}
			LogManager::instance().ItemLog(ch, pItem, "DS_PULL_OUT_SUCCESS", buf);
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1068]");
			pItem->AddToCharacter(ch, DestCell);
			return true;
		}
		else
		{
			if (pExtractor)
			{
				snprintf(buf, sizeof(buf), "dice(%d) prob(%d + %d) EXTR(VN:%d) ByProd(VN:%d)", (int)fDice, (int)fProb, iBonus, pExtractor->GetVnum(), dwByProduct);
			}
			else
			{
				snprintf(buf, sizeof(buf), "dice(%d) prob(%d) ByProd(VNUM:%d)", (int)fDice, (int)fProb, dwByProduct);
			}
			LogManager::instance().ItemLog(ch, pItem, "DS_PULL_OUT_FAILED", buf);
			M2_DESTROY_ITEM(pItem);
			pItem = NULL;
			if (dwByProduct)
			{
				LPITEM pByProduct = ch->AutoGiveItem(dwByProduct, true);
				if (pByProduct)
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1074;%s]", pByProduct->GetName());
				else
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1075]");
			}
			else
				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1075]");
		}
	}

	return bSuccess;
}

bool DSManager::DoRefineGrade(LPCHARACTER ch, TItemPos (&aItemPoses)[DRAGON_SOUL_REFINE_GRID_SIZE])
{
	if (NULL == ch)
		return false;

	if (NULL == aItemPoses)
	{
		return false;
	}

	if (!ch->DragonSoul_RefineWindow_CanRefine())
	{
		sys_err ("%s do not activate DragonSoulRefineWindow. But how can he come here?", ch->GetName());
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;5281]");
		return false;
	}

	// There may be duplicates item pointer to get rid of set use
	// When sending strange packets , duplicated TItemPos There may be , erroneous TItemPos There may be .
	std::set <LPITEM> set_items;
	for (int i = 0; i < DRAGON_SOUL_REFINE_GRID_SIZE; i++)
	{
		if (aItemPoses[i].IsEquipPosition())
			return false;
		LPITEM pItem = ch->GetItem(aItemPoses[i]);
		if (NULL != pItem)
		{
			// Items other than Dragon Soul Stones cannot be in the improvement window. .
			if (!pItem->IsDragonSoul())
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1076]");
				SendRefineResultPacket(ch, DS_SUB_HEADER_REFINE_FAIL_INVALID_MATERIAL, TItemPos(pItem->GetWindow(), pItem->GetCell()));

				return false;
			}

			set_items.insert(pItem);
		}
	}

	if (set_items.size() == 0)
	{
		SendRefineResultPacket(ch, DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MATERIAL, NPOS);
		return false;
	}

	int count = static_cast<int>(set_items.size());
	int need_count = 0;
	int fee = 0;
	std::vector <float> vec_probs;

	BYTE ds_type, grade_idx, step_idx, strength_idx;
	int result_grade;

	// The first thing is used as a standard for reinforcement. .
	std::set <LPITEM>::iterator it = set_items.begin();
	{
		LPITEM pItem = *it;

		GetDragonSoulInfo(pItem->GetVnum(), ds_type, grade_idx, step_idx, strength_idx);
		
		if (!m_pTable->GetRefineGradeValues(ds_type, grade_idx, need_count, fee, vec_probs))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1077]");
			SendRefineResultPacket(ch, DS_SUB_HEADER_REFINE_FAIL_INVALID_MATERIAL, TItemPos(pItem->GetWindow(), pItem->GetCell()));

			return false;
		}
	}
	while (++it != set_items.end())
	{
		LPITEM pItem = *it;

		// Clara ui Because items equipped in are prevented from being uploaded to the improvement window, ,
		// No separate notification processing .
		if (pItem->IsEquipped())
		{
			return false;
		}
		
		if (ds_type != GetType(pItem->GetVnum()) || grade_idx != GetGradeIdx(pItem->GetVnum()))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1076]");
			SendRefineResultPacket(ch, DS_SUB_HEADER_REFINE_FAIL_INVALID_MATERIAL, TItemPos(pItem->GetWindow(), pItem->GetCell()));

			return false;
		}
	}

	// Because the number is checked once in CLA. count != need_count ramen invalid It's very likely that it's Clary. .
	if (count != need_count)
	{
		sys_err ("Possiblity of invalid client. Name %s", ch->GetName());
		BYTE bSubHeader = count < need_count? DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MATERIAL : DS_SUB_HEADER_REFINE_FAIL_TOO_MUCH_MATERIAL;
		SendRefineResultPacket(ch, bSubHeader, NPOS);
		return false;
	}

	if (ch->GetGold() < fee)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;67]");
		SendRefineResultPacket(ch, DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MONEY, NPOS);
		return false;
	}
	
	if (-1 == (result_grade = Gamble(vec_probs)))
	{
		sys_err ("Gamble failed. See RefineGardeTables' probabilities");
		return false;
	}

	LPITEM pResultItem = ITEM_MANAGER::instance().CreateItem(MakeDragonSoulVnum(ds_type, (BYTE)result_grade, 0, 0));

	if (NULL == pResultItem)
	{
		sys_err ("INVALID DRAGON SOUL(%d)", MakeDragonSoulVnum(ds_type, (BYTE)result_grade, 0, 0));
		return false;
	}

	ch->PointChange(POINT_GOLD, -fee);
	int left_count = need_count;
	
	for (std::set <LPITEM>::iterator it = set_items.begin(); it != set_items.end(); it++)
	{
		LPITEM pItem = *it;
		int n = pItem->GetCount();
		if (left_count > n)
		{
			pItem->RemoveFromCharacter();
			M2_DESTROY_ITEM(pItem);
			left_count -= n;
		}
		else
		{
			pItem->SetCount(n - left_count);
		}
	}

	ch->AutoGiveItem(pResultItem, true);

	if (result_grade > grade_idx)
	{
		char buf[128];
		snprintf(buf, sizeof(buf), "GRADE : %d -> %d", grade_idx, result_grade);
		LogManager::instance().ItemLog(ch, pResultItem, "DS_GRADE_REFINE_SUCCESS", buf);
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1082]");
		SendRefineResultPacket(ch, DS_SUB_HEADER_REFINE_SUCCEED, TItemPos (pResultItem->GetWindow(), pResultItem->GetCell()));
		return true;
	}
	else
	{
		char buf[128];
		snprintf(buf, sizeof(buf), "GRADE : %d -> %d", grade_idx, result_grade);
		LogManager::instance().ItemLog(ch, pResultItem, "DS_GRADE_REFINE_FAIL", buf);
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1079]");
		SendRefineResultPacket(ch, DS_SUB_HEADER_REFINE_FAIL, TItemPos (pResultItem->GetWindow(), pResultItem->GetCell()));
		return false;
	}
}

bool DSManager::DoRefineStep(LPCHARACTER ch, TItemPos (&aItemPoses)[DRAGON_SOUL_REFINE_GRID_SIZE])
{
	if (NULL == ch)
		return false;
	if (NULL == aItemPoses)
	{
		return false;
	}

	if (!ch->DragonSoul_RefineWindow_CanRefine())
	{
		sys_err ("%s do not activate DragonSoulRefineWindow. But how can he come here?", ch->GetName());
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;5282]");
		return false;
	}

	// There may be duplicates item pointer to get rid of set use
	// When sending strange packets , duplicated TItemPos There may be , erroneous TItemPos There may be .
	std::set <LPITEM> set_items;
	for (int i = 0; i < DRAGON_SOUL_REFINE_GRID_SIZE; i++)
	{
		LPITEM pItem = ch->GetItem(aItemPoses[i]);
		if (NULL != pItem)
		{
			// Items other than Dragon Soul Stones cannot be in the improvement window. .
			if (!pItem->IsDragonSoul())
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1084]");
				SendRefineResultPacket(ch, DS_SUB_HEADER_REFINE_FAIL_INVALID_MATERIAL, TItemPos(pItem->GetWindow(), pItem->GetCell()));
				return false;
			}
			set_items.insert(pItem);
		}
	}

	if (set_items.size() == 0)
	{
		SendRefineResultPacket(ch, DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MATERIAL, NPOS);
		return false;
	}

	std::string stGroupName;
	int count = static_cast<int>(set_items.size());
	int need_count = 0;
	int fee = 0;
	std::vector <float> vec_probs;

	BYTE ds_type, grade_idx, step_idx, strength_idx;
	int result_step;

	// The first thing is used as a standard for reinforcement. .
	std::set <LPITEM>::iterator it = set_items.begin(); 
	{
		LPITEM pItem = *it;
		GetDragonSoulInfo(pItem->GetVnum(), ds_type, grade_idx, step_idx, strength_idx);

		if (!m_pTable->GetRefineStepValues(ds_type, step_idx, need_count, fee, vec_probs))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1080]");
			SendRefineResultPacket(ch, DS_SUB_HEADER_REFINE_FAIL_INVALID_MATERIAL, TItemPos(pItem->GetWindow(), pItem->GetCell()));
			return false;
		}
	}

	while(++it != set_items.end())
	{
		LPITEM pItem = *it;
		// Clara ui Because items equipped in are prevented from being uploaded to the improvement window, ,
		// No separate notification processing .
		if (pItem->IsEquipped())
		{
			return false;
		}
		if (ds_type != GetType(pItem->GetVnum()) || grade_idx != GetGradeIdx(pItem->GetVnum()) || step_idx != GetStepIdx(pItem->GetVnum()))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1084]");
			SendRefineResultPacket(ch, DS_SUB_HEADER_REFINE_FAIL_INVALID_MATERIAL, TItemPos(pItem->GetWindow(), pItem->GetCell()));
			return false;
		}
	}

	// Because the number is checked once in CLA. count != need_count ramen invalid It's very likely that it's Clary. .
	if (count != need_count)
	{
		sys_err ("Possiblity of invalid client. Name %s", ch->GetName());
		BYTE bSubHeader = count < need_count? DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MATERIAL : DS_SUB_HEADER_REFINE_FAIL_TOO_MUCH_MATERIAL;
		SendRefineResultPacket(ch, bSubHeader, NPOS);
		return false;
	}
	
	if (ch->GetGold() < fee)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;67]");
		SendRefineResultPacket(ch, DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MONEY, NPOS);
		return false;
	}
	
	float sum = 0.f;

	if (-1 == (result_step = Gamble(vec_probs)))
	{
		sys_err ("Gamble failed. See RefineStepTables' probabilities");
		return false;
	}

	LPITEM pResultItem = ITEM_MANAGER::instance().CreateItem(MakeDragonSoulVnum(ds_type, grade_idx, (BYTE)result_step, 0));

	if (NULL == pResultItem)
	{
		sys_err ("INVALID DRAGON SOUL(%d)", MakeDragonSoulVnum(ds_type, grade_idx, (BYTE)result_step, 0));
		return false;
	}

	ch->PointChange(POINT_GOLD, -fee);
	int left_count = need_count;
	for (std::set <LPITEM>::iterator it = set_items.begin(); it != set_items.end(); it++)
	{
		LPITEM pItem = *it;
		int n = pItem->GetCount();
		if (left_count > n)
		{
			pItem->RemoveFromCharacter();
			M2_DESTROY_ITEM(pItem);
			left_count -= n;
		}
		else
		{
			pItem->SetCount(n - left_count);
		}
	}

	ch->AutoGiveItem(pResultItem, true);
	if (result_step > step_idx)
	{
		char buf[128];
		snprintf(buf, sizeof(buf), "STEP : %d -> %d", step_idx, result_step);
		LogManager::instance().ItemLog(ch, pResultItem, "DS_STEP_REFINE_SUCCESS", buf);
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1078]");
		SendRefineResultPacket(ch, DS_SUB_HEADER_REFINE_SUCCEED, TItemPos (pResultItem->GetWindow(), pResultItem->GetCell()));
		return true;
	}
	else
	{
		char buf[128];
		snprintf(buf, sizeof(buf), "STEP : %d -> %d", step_idx, result_step);
		LogManager::instance().ItemLog(ch, pResultItem, "DS_STEP_REFINE_FAIL", buf);
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1083]");
		SendRefineResultPacket(ch, DS_SUB_HEADER_REFINE_FAIL, TItemPos (pResultItem->GetWindow(), pResultItem->GetCell()));
		return false;
	}
}

bool IsDragonSoulRefineMaterial(LPITEM pItem)
{
	if (pItem->GetType() != ITEM_MATERIAL)
		return false;
	return (pItem->GetSubType() == MATERIAL_DS_REFINE_NORMAL ||
		pItem->GetSubType() == MATERIAL_DS_REFINE_BLESSED ||
		pItem->GetSubType() == MATERIAL_DS_REFINE_HOLLY);
}

bool DSManager::DoRefineStrength(LPCHARACTER ch, TItemPos (&aItemPoses)[DRAGON_SOUL_REFINE_GRID_SIZE])
{
	if (NULL == ch)
		return false;
	if (NULL == aItemPoses)
	{
		return false;
	}

	if (!ch->DragonSoul_RefineWindow_CanRefine())
	{
		sys_err ("%s do not activate DragonSoulRefineWindow. But how can he come here?", ch->GetName());
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;5282]");
		return false;
	}

	// There may be duplicates item pointer to get rid of set use
	// When sending strange packets , duplicated TItemPos There may be , erroneous TItemPos There may be .
	std::set <LPITEM> set_items;
	for (int i = 0; i < DRAGON_SOUL_REFINE_GRID_SIZE; i++)
	{
		LPITEM pItem = ch->GetItem(aItemPoses[i]);
		if (pItem)
		{
			set_items.insert(pItem);
		}
	}
	if (set_items.size() == 0)
	{
		return false;
	}

	int fee;

	LPITEM pRefineStone = NULL;
	LPITEM pDragonSoul = NULL;
	for (std::set <LPITEM>::iterator it = set_items.begin(); it != set_items.end(); it++)
	{
		LPITEM pItem = *it;
		// Clara ui Because items equipped in are prevented from being uploaded to the improvement window, ,
		// No separate notification processing .
		if (pItem->IsEquipped())
		{
			return false;
		}

		// Only dragon soul stones and reinforcement stones can be in the improvement window. .
		// And there should only be one .
		if (pItem->IsDragonSoul())
		{
			if (pDragonSoul != NULL)
			{
				SendRefineResultPacket(ch, DS_SUB_HEADER_REFINE_FAIL_TOO_MUCH_MATERIAL, TItemPos(pItem->GetWindow(), pItem->GetCell()));
				return false;	
			}
			pDragonSoul = pItem;
		}
		else if(IsDragonSoulRefineMaterial(pItem))
		{
			if (pRefineStone != NULL)
			{
				SendRefineResultPacket(ch, DS_SUB_HEADER_REFINE_FAIL_TOO_MUCH_MATERIAL, TItemPos(pItem->GetWindow(), pItem->GetCell()));
				return false;	
			}
			pRefineStone = pItem;
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1076]");
			SendRefineResultPacket(ch, DS_SUB_HEADER_REFINE_FAIL_INVALID_MATERIAL, TItemPos(pItem->GetWindow(), pItem->GetCell()));
			return false;
		}
	}

	BYTE bType, bGrade, bStep, bStrength;
	
	if (!pDragonSoul || !pRefineStone)
	{
		SendRefineResultPacket(ch, DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MATERIAL, NPOS);

		return false;
	}
	
	if (NULL != pDragonSoul)
	{
		GetDragonSoulInfo(pDragonSoul->GetVnum(), bType, bGrade, bStep, bStrength);

		float fWeight = 0.f;
		// Dragon Soul Stone that cannot be strengthened if it has no weight value.
		if (!m_pTable->GetWeight(bType, bGrade, bStep, bStrength + 1, fWeight))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1085]");
			SendRefineResultPacket(ch, DS_SUB_HEADER_REFINE_FAIL_MAX_REFINE, TItemPos(pDragonSoul->GetWindow(), pDragonSoul->GetCell()));
			return false;
		}
		// When strengthened, the weight 0 If so, it should not be strengthened any further. .
		if (fWeight < FLT_EPSILON)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1085]");
			SendRefineResultPacket(ch, DS_SUB_HEADER_REFINE_FAIL_MAX_REFINE, TItemPos(pDragonSoul->GetWindow(), pDragonSoul->GetCell()));
			return false;
		}
	}

	float fProb;
	if (!m_pTable->GetRefineStrengthValues(bType, pRefineStone->GetSubType(), bStrength, fee, fProb))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1085]");
		SendRefineResultPacket(ch, DS_SUB_HEADER_REFINE_FAIL_INVALID_MATERIAL, TItemPos(pDragonSoul->GetWindow(), pDragonSoul->GetCell()));

		return false;
	}

	if (ch->GetGold() < fee)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;67]");
		SendRefineResultPacket(ch, DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MONEY, NPOS);
		return false;
	}
	
	ch->PointChange(POINT_GOLD, -fee);
	LPITEM pResult = NULL;
	BYTE bSubHeader;

	if (fnumber(0.f, 100.f) <= fProb)
	{
		pResult = ITEM_MANAGER::instance().CreateItem(MakeDragonSoulVnum(bType, bGrade, bStep, bStrength + 1));
		if (NULL == pResult)
		{
			sys_err ("INVALID DRAGON SOUL(%d)", MakeDragonSoulVnum(bType, bGrade, bStep, bStrength + 1));
			return false;
		}
		pDragonSoul->RemoveFromCharacter();

		pDragonSoul->CopyAttributeTo(pResult);
		RefreshItemAttributes(pResult);

		pDragonSoul->SetCount(pDragonSoul->GetCount() - 1);
		pRefineStone->SetCount(pRefineStone->GetCount() - 1);

		char buf[128];
		snprintf(buf, sizeof(buf), "STRENGTH : %d -> %d", bStrength, bStrength + 1);
		LogManager::instance().ItemLog(ch, pDragonSoul, "DS_STRENGTH_REFINE_SUCCESS", buf);
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1087]");
		ch->AutoGiveItem(pResult, true);
		bSubHeader = DS_SUB_HEADER_REFINE_SUCCEED;
	}
	else
	{
		if (bStrength != 0)
		{
			pResult = ITEM_MANAGER::instance().CreateItem(MakeDragonSoulVnum(bType, bGrade, bStep, bStrength - 1));
			if (NULL == pResult)
			{
				sys_err ("INVALID DRAGON SOUL(%d)", MakeDragonSoulVnum(bType, bGrade, bStep, bStrength - 1));
				return false;
			}
			pDragonSoul->CopyAttributeTo(pResult);
			RefreshItemAttributes(pResult);
		}
		bSubHeader = DS_SUB_HEADER_REFINE_FAIL;

		char buf[128];
		snprintf(buf, sizeof(buf), "STRENGTH : %d -> %d", bStrength, bStrength - 1);
		// strength Reinforcement can be broken if it fails , Leave a log based on the original item .
		LogManager::instance().ItemLog(ch, pDragonSoul, "DS_STRENGTH_REFINE_FAIL", buf);

		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1088]");
		pDragonSoul->SetCount(pDragonSoul->GetCount() - 1);
		pRefineStone->SetCount(pRefineStone->GetCount() - 1);
		if (NULL != pResult)
			ch->AutoGiveItem(pResult, true);
		
	}
	
	SendRefineResultPacket(ch, bSubHeader, NULL == pResult? NPOS : TItemPos (pResult->GetWindow(), pResult->GetCell()));

	return true;
}

void DSManager::SendRefineResultPacket(LPCHARACTER ch, BYTE bSubHeader, const TItemPos& pos)
{
	TPacketGCDragonSoulRefine pack;
	pack.bSubType = bSubHeader;

	if (pos.IsValidItemPosition())
	{
		pack.Pos = pos;
	}
	LPDESC d = ch->GetDesc();
	if (NULL == d)
	{
		return ;
	}
	else
	{
		d->Packet(&pack, sizeof(pack));
	}
}

int DSManager::LeftTime(LPITEM pItem) const
{
	if (pItem == NULL)
		return false;

	// For now timer based on wear Only the dragon soul stone does not disappear even if time runs out. .
	if (pItem->GetProto()->cLimitTimerBasedOnWearIndex >= 0)
	{
		return static_cast<int>(pItem->GetSocket(ITEM_SOCKET_REMAIN_SEC));
	}
	// different limit type Since all Dragon Soul Stones disappear over time, the items entered here are judged to have some time left. .
	else
	{
		return INT_MAX;
	}
}

bool DSManager::IsTimeLeftDragonSoul(LPITEM pItem) const
{
	if (pItem == NULL)
		return false;

	// For now timer based on wear Only the dragon soul stone does not disappear even if time runs out. .
	if (pItem->GetProto()->cLimitTimerBasedOnWearIndex >= 0)
	{
		return pItem->GetSocket(ITEM_SOCKET_REMAIN_SEC) > 0;
	}
	// different limit type Since all Dragon Soul Stones disappear over time, the items entered here are judged to have some time left. .
	else
	{
		return true;
	}
}

bool DSManager::IsActiveDragonSoul(LPITEM pItem) const
{
	return pItem->GetSocket(ITEM_SOCKET_DRAGON_SOUL_ACTIVE_IDX);
}

bool DSManager::ActivateDragonSoul(LPITEM pItem)
{
	if (NULL == pItem)
		return false;
	LPCHARACTER pOwner = pItem->GetOwner();
	if (NULL == pOwner)
		return false;

	int deck_idx = pOwner->DragonSoul_GetActiveDeck();
	
	if (deck_idx < 0)
		return false;
	
	if (INVENTORY_MAX_NUM + WEAR_MAX_NUM + DS_SLOT_MAX * deck_idx <= pItem->GetCell() &&
			pItem->GetCell() < INVENTORY_MAX_NUM + WEAR_MAX_NUM + DS_SLOT_MAX * (deck_idx + 1))
	{
		if (IsTimeLeftDragonSoul(pItem) && !IsActiveDragonSoul(pItem))
		{
			char buf[128];
			snprintf (buf, sizeof(buf), "LEFT TIME(%d)", LeftTime(pItem));
			LogManager::instance().ItemLog(pOwner, pItem, "DS_ACTIVATE", buf);
			pItem->ModifyPoints(true);
			pItem->SetSocket(ITEM_SOCKET_DRAGON_SOUL_ACTIVE_IDX, 1);

			pItem->StartTimerBasedOnWearExpireEvent();
		}
		return true;
	}
	else
		return false;
}

bool DSManager::DeactivateDragonSoul(LPITEM pItem, bool bSkipRefreshOwnerActiveState)
{
	if (NULL == pItem)
		return false;

	LPCHARACTER pOwner = pItem->GetOwner();
	if (NULL == pOwner)
		return false;

	if (!IsActiveDragonSoul(pItem))
		return false;

	char buf[128];
	pItem->StopTimerBasedOnWearExpireEvent();
	pItem->SetSocket(ITEM_SOCKET_DRAGON_SOUL_ACTIVE_IDX, 0);
	pItem->ModifyPoints(false);

	snprintf (buf, sizeof(buf), "LEFT TIME(%d)", LeftTime(pItem));
	LogManager::instance().ItemLog(pOwner, pItem, "DS_DEACTIVATE", buf);

	if (false == bSkipRefreshOwnerActiveState)
		RefreshDragonSoulState(pOwner);

	return true;
}

void DSManager::RefreshDragonSoulState(LPCHARACTER ch)
{
	if (NULL == ch)
		return ;
	for (int i = WEAR_MAX_NUM; i < WEAR_MAX_NUM + DS_SLOT_MAX * DRAGON_SOUL_DECK_MAX_NUM; i++)
	{
		LPITEM pItem = ch->GetWear(i);
		if (pItem != NULL)
		{
			if(IsActiveDragonSoul(pItem))
			{
				return;
			}
		}
	}
	ch->DragonSoul_DeactivateAll();
}

DSManager::DSManager()
{
	m_pTable = NULL;
}

DSManager::~DSManager()
{
	if (m_pTable)
		delete m_pTable;
}
