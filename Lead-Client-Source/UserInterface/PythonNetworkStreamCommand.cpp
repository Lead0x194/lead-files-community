#include "StdAfx.h"
#include "PythonNetworkStream.h"
#include "PythonNonPlayer.h"
#include "AbstractApplication.h"
#include "AbstractPlayer.h"
#include "AbstractCharacterManager.h"
#include "AbstractChat.h"
#include "InstanceBase.h"

#define ishan(ch)		(((ch) & 0xE0) > 0x90)
#define ishanasc(ch)	(isascii(ch) || ishan(ch))
#define ishanalp(ch)	(isalpha(ch) || ishan(ch))
#define isnhdigit(ch)	(!ishan(ch) && isdigit(ch))
#define isnhspace(ch)	(!ishan(ch) && isspace(ch))

#define LOWER(c)		(((c) >= 'A' && (c) <= 'Z') ? ((c) + ('a' - 'A')) : (c))
#define UPPER(c)		(((c) >= 'a' && (c) <= 'z') ? ((c) + ('A' - 'a')) : (c))

void SkipSpaces(char **string)
{
    for (; **string != '\0' && isnhspace((unsigned char) **string); ++(*string));
}

char *OneArgument(char *argument, char *first_arg)
{
    char mark = FALSE;

    if (!argument)
    {
        *first_arg = '\0';
        return NULL;
    }

    SkipSpaces(&argument);

    while (*argument)
    {
        if (*argument == '\"')
        {
            mark = !mark;
            ++argument;
            continue;
        }

        if (!mark && isnhspace((unsigned char) *argument))
            break;

		*(first_arg++) = LOWER(*argument);
		++argument;
    }

    *first_arg = '\0';

    SkipSpaces(&argument);
    return (argument);
}

void AppendMonsterList(const CPythonNonPlayer::TMobTableList & c_rMobTableList, const char * c_szHeader, int iType)
{
	DWORD dwMonsterCount = 0;
	std::string strMonsterList = c_szHeader;

	CPythonNonPlayer::TMobTableList::const_iterator itor = c_rMobTableList.begin();
	for (; itor!=c_rMobTableList.end(); ++itor)
	{
		const CPythonNonPlayer::TMobTable * c_pMobTable = *itor;
		if (iType == c_pMobTable->bRank)
		{
			if (dwMonsterCount != 0)
				strMonsterList += ", ";
			strMonsterList += CPythonNonPlayer::Instance().GetMonsterName(c_pMobTable->dwVnum);
			if (++dwMonsterCount > 5)
				break;
		}
	}
	if (dwMonsterCount > 0)
	{
		IAbstractChat& rkChat=IAbstractChat::GetSingleton();
		rkChat.AppendChat(CHAT_TYPE_INFO, strMonsterList.c_str());
	}
}

bool CPythonNetworkStream::ClientCommand(const char * c_szCommand)
{
	return false;
}

bool SplitToken(const char * c_szLine, CTokenVector * pstTokenVector, const char * c_szDelimeter = " ")
{
	pstTokenVector->reserve(10);
	pstTokenVector->clear();

	std::string stToken;
	std::string strLine = c_szLine;

	std::string::size_type basePos = 0;

	do
	{
		std::string::size_type beginPos = strLine.find_first_not_of(c_szDelimeter, basePos);
		if (beginPos == std::string::npos)
			return false;

		std::string::size_type endPos;

		if (strLine[beginPos] == '"')
		{
			++beginPos;
			endPos = strLine.find_first_of("\"", beginPos);

			if (endPos == std::string::npos)
				return false;
			
			basePos = endPos + 1;
		}
		else
		{
			endPos = strLine.find_first_of(c_szDelimeter, beginPos);
			basePos = endPos;
		}

		pstTokenVector->push_back(strLine.substr(beginPos, endPos - beginPos));

		// Additional code. Check if there is a tab at the end. - [levites]
		if (int(strLine.find_first_not_of(c_szDelimeter, basePos)) < 0)
			break;
	} while (basePos < strLine.length());

	return true;
}

void CPythonNetworkStream::ServerCommand(char * c_szCommand)
{
	// #0000811: [M2EU] Block console window function
	if (strcmpi(c_szCommand, "ConsoleEnable") == 0)
		return;

	if (m_apoPhaseWnd[PHASE_WINDOW_GAME])
	{
		bool isTrue;
		if (PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], 
			"BINARY_ServerCommand_Run", 
			Py_BuildValue("(s)", c_szCommand),
			&isTrue
		))
		{
			if (isTrue)
				return;
		}
	}
	else if (m_poSerCommandParserWnd)
	{
		bool isTrue;
		if (PyCallClassMemberFunc(m_poSerCommandParserWnd, 
			"BINARY_ServerCommand_Run", 
			Py_BuildValue("(s)", c_szCommand),
			&isTrue
		))
		{
			if (isTrue)
				return;
		}
	}

	CTokenVector TokenVector;
	if (!SplitToken(c_szCommand, &TokenVector))
		return;
	if (TokenVector.empty())
		return;

	const char * szCmd = TokenVector[0].c_str();
	
	if (!strcmpi(szCmd, "quit"))
	{
		PostQuitMessage(0);
	}
	else if (!strcmpi(szCmd, "BettingMoney"))
	{
		if (2 != TokenVector.size())
		{
			TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %s", c_szCommand);
			return;
		}

		//UINT uMoney= atoi(TokenVector[1].c_str());		
		
	}
	// GIFT NOTIFY
	else if (!strcmpi(szCmd, "gift"))
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "Gift_Show", Py_BuildValue("()")); 	
	}
	// CUBE
	else if (!strcmpi(szCmd, "cube"))
	{
		if (TokenVector.size() < 2)
		{
			TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %s", c_szCommand);
			return;
		}

		if ("open" == TokenVector[1])
		{
			if (3 > TokenVector.size())
			{
				TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %s", c_szCommand);
				return;
			}

			DWORD npcVNUM = (DWORD)atoi(TokenVector[2].c_str());
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Cube_Open", Py_BuildValue("(i)", npcVNUM));
		}
		else if ("close" == TokenVector[1])
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Cube_Close", Py_BuildValue("()"));
		}
		else if ("reload" == TokenVector[1])
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Reload_Cube", Py_BuildValue("()"));
		}
		else if ("info" == TokenVector[1])
		{
			if (5 != TokenVector.size())
			{
				TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %s", c_szCommand);
				return;
			}

			UINT gold = static_cast<UINT>(atoi(TokenVector[2].c_str()));
			UINT itemVnum = static_cast<UINT>(atoi(TokenVector[3].c_str()));
			UINT count = static_cast<UINT>(atoi(TokenVector[4].c_str()));
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Cube_UpdateInfo", Py_BuildValue("(iii)", gold, itemVnum, count));
		}
		else if ("success" == TokenVector[1])
		{
			if (4 != TokenVector.size())
			{
				TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %s", c_szCommand);
				return;
			}

			UINT itemVnum = static_cast<UINT>(atoi(TokenVector[2].c_str()));
			UINT count = static_cast<UINT>(atoi(TokenVector[3].c_str()));
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Cube_Succeed", Py_BuildValue("(ii)", itemVnum, count));
		}
		else if ("fail" == TokenVector[1])
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Cube_Failed", Py_BuildValue("()"));
		}
		else if ("r_list" == TokenVector[1])
		{
			// result list (/cube r_list npcVNUM resultCount resultText)
			// 20383 4 72723,1/72725,1/72730.1/50001,5 <- This gives a list separated by the "/" character.
			if (5 != TokenVector.size())
			{
				TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %d", c_szCommand, 5);
				return;
			}

			DWORD npcVNUM = (DWORD)atoi(TokenVector[2].c_str());

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Cube_ResultList", Py_BuildValue("(is)", npcVNUM, TokenVector[4].c_str()));
		}
		else if ("m_info" == TokenVector[1])
		{
			// material list (/cube m_info requestStartIndex resultCount MaterialText)
			// ex) requestStartIndex: 0, resultCount: 5 - MaterialText contains all the materials needed to make items 0 to 4 that the NPC can make.
			// If there are multiple items, as in the example above, use the "@" character as a separator.
			// 0 5 125,1|126,2|127,2|123,5&555,5&555,4/120000 <- In this way, the server gives the Claro list.

			if (5 != TokenVector.size())
			{
				TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %d", c_szCommand, 5);
				return;
			}

			UINT requestStartIndex = (UINT)atoi(TokenVector[2].c_str());
			UINT resultCount = (UINT)atoi(TokenVector[3].c_str());

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Cube_MaterialInfo", Py_BuildValue("(iis)", requestStartIndex, resultCount, TokenVector[4].c_str()));
		}
	}
	// CUEBE_END
	else if (!strcmpi(szCmd, "ObserverCount"))
	{
		if (2 != TokenVector.size())
		{
			TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %s", c_szCommand);
			return;
		}

		UINT uObserverCount= static_cast<UINT>(atoi(TokenVector[1].c_str()));

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], 
			"BINARY_BettingGuildWar_UpdateObserverCount", 
			Py_BuildValue("(i)", uObserverCount)
		);
	}
	else if (!strcmpi(szCmd, "ObserverMode"))
	{		
		if (2 != TokenVector.size())
		{
			TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %s", c_szCommand);
			return;
		}

		UINT uMode= static_cast<UINT>(atoi(TokenVector[1].c_str()));

		IAbstractPlayer& rkPlayer=IAbstractPlayer::GetSingleton();
		rkPlayer.SetObserverMode(uMode ? true : false);

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], 
			"BINARY_BettingGuildWar_SetObserverMode", 
			Py_BuildValue("(i)", uMode)
		);
	}
	else if (!strcmpi(szCmd, "ObserverTeamInfo"))
	{		
	}
	else if (!strcmpi(szCmd, "StoneDetect"))
	{
		if (4 != TokenVector.size())
		{
			TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %s", c_szCommand);
			return;
		}

		// vid distance(1-3) angle(0-360)
		DWORD dwVID = static_cast<DWORD>(atoi(TokenVector[1].c_str()));
		BYTE byDistance = static_cast<BYTE>(atoi(TokenVector[2].c_str()));
		float fAngle = static_cast<float>(atof(TokenVector[3].c_str()));
		fAngle = fmod(540.0f - fAngle, 360.0f);
		Tracef("StoneDetect [VID:%d] [Distance:%d] [Angle:%d->%f]\n", dwVID, byDistance, atoi(TokenVector[3].c_str()), fAngle);

		IAbstractCharacterManager& rkChrMgr=IAbstractCharacterManager::GetSingleton();

		CInstanceBase * pInstance = rkChrMgr.GetInstancePtr(dwVID);
		if (!pInstance)
		{
			TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Not Exist Instance", c_szCommand);
			return;
		}

		TPixelPosition PixelPosition;
		D3DXVECTOR3 v3Rotation(0.0f, 0.0f, fAngle);
		pInstance->NEW_GetPixelPosition(&PixelPosition);

		PixelPosition.y *= -1.0f;

		switch (byDistance)
		{
			case 0:
				CEffectManager::Instance().RegisterEffect("d:/ymir work/effect/etc/firecracker/find_out.mse");
				CEffectManager::Instance().CreateEffect("d:/ymir work/effect/etc/firecracker/find_out.mse", PixelPosition, v3Rotation);
				break;
			case 1:
				CEffectManager::Instance().RegisterEffect("d:/ymir work/effect/etc/compass/appear_small.mse");
				CEffectManager::Instance().CreateEffect("d:/ymir work/effect/etc/compass/appear_small.mse", PixelPosition, v3Rotation);
				break;
			case 2:
				CEffectManager::Instance().RegisterEffect("d:/ymir work/effect/etc/compass/appear_middle.mse");
				CEffectManager::Instance().CreateEffect("d:/ymir work/effect/etc/compass/appear_middle.mse", PixelPosition, v3Rotation);
				break;
			case 3:
				CEffectManager::Instance().RegisterEffect("d:/ymir work/effect/etc/compass/appear_large.mse");
				CEffectManager::Instance().CreateEffect("d:/ymir work/effect/etc/compass/appear_large.mse", PixelPosition, v3Rotation);
				break;
			default:
				TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Distance", c_szCommand);
				break;
		}

#ifdef _DEBUG
		IAbstractChat& rkChat=IAbstractChat::GetSingleton();
		rkChat.AppendChat(CHAT_TYPE_INFO, c_szCommand);
#endif
	}
	else if (!strcmpi(szCmd, "StartStaminaConsume"))
	{
		if (3 != TokenVector.size())
		{
			TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %s", c_szCommand);
			return;
		}

		DWORD dwConsumePerSec = static_cast<DWORD>(atoi(TokenVector[1].c_str()));
		DWORD dwCurrentStamina = static_cast<DWORD>(atoi(TokenVector[2].c_str()));

		IAbstractPlayer& rPlayer=IAbstractPlayer::GetSingleton();
		rPlayer.StartStaminaConsume(dwConsumePerSec, dwCurrentStamina);
	}
	
	else if (!strcmpi(szCmd, "StopStaminaConsume"))
	{
		if (2 != TokenVector.size())
		{
			TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %d", c_szCommand, TokenVector.size());
			return;
		}

		DWORD dwCurrentStamina = static_cast<DWORD>(atoi(TokenVector[1].c_str()));

		IAbstractPlayer& rPlayer=IAbstractPlayer::GetSingleton();
		rPlayer.StopStaminaConsume(dwCurrentStamina);
	}
	else if (!strcmpi(szCmd, "messenger_auth"))
	{
		const std::string & c_rstrName = TokenVector[1].c_str();
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnMessengerAddFriendQuestion", Py_BuildValue("(s)", c_rstrName.c_str()));
	}
	else if (!strcmpi(szCmd, "combo"))
	{
		int iFlag = static_cast<int>(atoi(TokenVector[1].c_str()));
		IAbstractPlayer& rPlayer=IAbstractPlayer::GetSingleton();
		rPlayer.SetComboSkillFlag(iFlag);
		m_bComboSkillFlag = iFlag ? true : false;
	}
	else if (!strcmpi(szCmd, "setblockmode"))
	{
		int iFlag = static_cast<int>(atoi(TokenVector[1].c_str()));
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnBlockMode", Py_BuildValue("(i)", iFlag));
	}
	// Emotion Start
	else if (!strcmpi(szCmd, "french_kiss"))
	{
		int iVID1 = static_cast<int>(atoi(TokenVector[1].c_str()));
		int iVID2 = static_cast<int>(atoi(TokenVector[2].c_str()));

		IAbstractCharacterManager & rkChrMgr = IAbstractCharacterManager::GetSingleton();
		CInstanceBase * pInstance1 = rkChrMgr.GetInstancePtr(iVID1);
		CInstanceBase * pInstance2 = rkChrMgr.GetInstancePtr(iVID2);
		if (pInstance1 && pInstance2)
			pInstance1->ActDualEmotion(*pInstance2, CRaceMotionData::NAME_FRENCH_KISS_START, CRaceMotionData::NAME_FRENCH_KISS_START);
	}
	else if (!strcmpi(szCmd, "kiss"))
	{
		int iVID1 = static_cast<int>(atoi(TokenVector[1].c_str()));
		int iVID2 = static_cast<int>(atoi(TokenVector[2].c_str()));

		IAbstractCharacterManager & rkChrMgr = IAbstractCharacterManager::GetSingleton();
		CInstanceBase * pInstance1 = rkChrMgr.GetInstancePtr(iVID1);
		CInstanceBase * pInstance2 = rkChrMgr.GetInstancePtr(iVID2);
		if (pInstance1 && pInstance2)
			pInstance1->ActDualEmotion(*pInstance2, CRaceMotionData::NAME_KISS_START, CRaceMotionData::NAME_KISS_START);
	}
	else if (!strcmpi(szCmd, "slap"))
	{
		int iVID1 = static_cast<int>(atoi(TokenVector[1].c_str()));
		int iVID2 = static_cast<int>(atoi(TokenVector[2].c_str()));

		IAbstractCharacterManager & rkChrMgr = IAbstractCharacterManager::GetSingleton();
		CInstanceBase * pInstance1 = rkChrMgr.GetInstancePtr(iVID1);
		CInstanceBase * pInstance2 = rkChrMgr.GetInstancePtr(iVID2);
		if (pInstance1 && pInstance2)
			pInstance1->ActDualEmotion(*pInstance2, CRaceMotionData::NAME_SLAP_HURT_START, CRaceMotionData::NAME_SLAP_HIT_START);
	}
	else if (!strcmpi(szCmd, "clap"))
	{
		int iVID = static_cast<int>(atoi(TokenVector[1].c_str()));
		IAbstractCharacterManager & rkChrMgr = IAbstractCharacterManager::GetSingleton();
		CInstanceBase * pInstance = rkChrMgr.GetInstancePtr(iVID);
		if (pInstance)
			pInstance->ActEmotion(CRaceMotionData::NAME_CLAP);
	}
	else if (!strcmpi(szCmd, "cheer1"))
	{
		int iVID = static_cast<int>(atoi(TokenVector[1].c_str()));
		IAbstractCharacterManager & rkChrMgr = IAbstractCharacterManager::GetSingleton();
		CInstanceBase * pInstance = rkChrMgr.GetInstancePtr(iVID);
		if (pInstance)
			pInstance->ActEmotion(CRaceMotionData::NAME_CHEERS_1);
	}
	else if (!strcmpi(szCmd, "cheer2"))
	{
		int iVID = static_cast<int>(atoi(TokenVector[1].c_str()));
		IAbstractCharacterManager & rkChrMgr = IAbstractCharacterManager::GetSingleton();
		CInstanceBase * pInstance = rkChrMgr.GetInstancePtr(iVID);
		if (pInstance)
			pInstance->ActEmotion(CRaceMotionData::NAME_CHEERS_2);
	}
	else if (!strcmpi(szCmd, "dance1"))
	{
		int iVID = static_cast<int>(atoi(TokenVector[1].c_str()));
		IAbstractCharacterManager & rkChrMgr = IAbstractCharacterManager::GetSingleton();
		CInstanceBase * pInstance = rkChrMgr.GetInstancePtr(iVID);
		if (pInstance)
			pInstance->ActEmotion(CRaceMotionData::NAME_DANCE_1);
	}
	else if (!strcmpi(szCmd, "dance2"))
	{
		int iVID = static_cast<int>(atoi(TokenVector[1].c_str()));
		IAbstractCharacterManager & rkChrMgr = IAbstractCharacterManager::GetSingleton();
		CInstanceBase * pInstance = rkChrMgr.GetInstancePtr(iVID);
		if (pInstance)
			pInstance->ActEmotion(CRaceMotionData::NAME_DANCE_2);
	}
	else if (!strcmpi(szCmd, "dig_motion"))
	{
		int iVID = static_cast<int>(atoi(TokenVector[1].c_str()));
		IAbstractCharacterManager & rkChrMgr = IAbstractCharacterManager::GetSingleton();
		CInstanceBase * pInstance = rkChrMgr.GetInstancePtr(iVID);
		if (pInstance)
			pInstance->ActEmotion(CRaceMotionData::NAME_DIG);
	}
	// Emotion End
	else
	{
		static std::map<std::string, int> s_emotionDict;

		static bool s_isFirst = true;
		if (s_isFirst)
		{
			s_isFirst = false;

			s_emotionDict["dance3"] = CRaceMotionData::NAME_DANCE_3;
			s_emotionDict["dance4"] = CRaceMotionData::NAME_DANCE_4;
			s_emotionDict["dance5"] = CRaceMotionData::NAME_DANCE_5;
			s_emotionDict["dance6"] = CRaceMotionData::NAME_DANCE_6;
			s_emotionDict["congratulation"] = CRaceMotionData::NAME_CONGRATULATION;
			s_emotionDict["forgive"] = CRaceMotionData::NAME_FORGIVE;
			s_emotionDict["angry"] = CRaceMotionData::NAME_ANGRY;
			s_emotionDict["attractive"] = CRaceMotionData::NAME_ATTRACTIVE;
			s_emotionDict["sad"] = CRaceMotionData::NAME_SAD;
			s_emotionDict["shy"] = CRaceMotionData::NAME_SHY;
			s_emotionDict["cheerup"] = CRaceMotionData::NAME_CHEERUP;
			s_emotionDict["banter"] = CRaceMotionData::NAME_BANTER;
			s_emotionDict["joy"] = CRaceMotionData::NAME_JOY;
		}

		std::map<std::string, int>::iterator f = s_emotionDict.find(szCmd);
		if (f == s_emotionDict.end())
		{
			TraceError("Unknown Server Command %s | %s", c_szCommand, szCmd);
		}
		else
		{
			int emotionIndex = f->second;

			int iVID = static_cast<int>(atoi(TokenVector[1].c_str()));
			IAbstractCharacterManager & rkChrMgr = IAbstractCharacterManager::GetSingleton();
			CInstanceBase * pInstance = rkChrMgr.GetInstancePtr(iVID);

			if (pInstance)
				pInstance->ActEmotion(emotionIndex);
		}		
	}
}
