#include "StdAfx.h"
#include "MainFrm.h"

#include <EterBase/Utils.h>

void CMainFrame::LoadSoundInformation()
{
	ControlSoundInformationFiles(TRUE);
}

void CMainFrame::SaveSoundInformation()
{
	ControlSoundInformationFiles(FALSE);
	LogBox("저장이 완료 되었습니다", "알림", GetSafeHwnd());
}

void CMainFrame::ControlSoundInformationFiles(BOOL isLoad)
{
	TCharacterDataMap::iterator itor = ms_CharacterDataMap.begin();

	for (; itor != ms_CharacterDataMap.end(); ++itor)
	{
		TCharacterData & rCharacterData = itor->second;

		///////////////////////////////////////////////////////////////////////////////////////////
		const char * c_szPathHeaderName = "d:/ymir work/";

		if (strncmp(rCharacterData.strPathName.c_str(), c_szPathHeaderName, strlen(c_szPathHeaderName)))
		{
			LogBoxf("파일 경로의 시작이 d:\\ymir work가 아닙니다. (path: %s)", rCharacterData.strPathName.c_str());
			continue;
		}
		///////////////////////////////////////////////////////////////////////////////////////////

		std::string strSifPath;
		strSifPath = "sound/";
		strSifPath += rCharacterData.strPathName.substr(strlen(c_szPathHeaderName), -1);

		MyCreateDirectory(strSifPath.c_str());

		/////

		if (MODEL_TYPE_EFFECT == rCharacterData.eModelType)
		{
			std::string strModelPath;
			std::string strModelName;
			std::string strModelExtension;

			GetFilePathNameExtension(rCharacterData.strModelScriptFileName.c_str(),
									rCharacterData.strModelScriptFileName.length(),
									&strModelPath, &strModelName, &strModelExtension);

			std::string strSoundInformationFileName;

			strSoundInformationFileName = strSifPath;
			strSoundInformationFileName += strModelName;
			strSoundInformationFileName += ".mss";

			if (isLoad)
			{
				if (!NSound::LoadSoundInformationPiece(strSoundInformationFileName.c_str(), rCharacterData.SoundDataVector))
					SetStatusBarString(NSound::GetResultString());
			}
			else
			{
				if (!NSound::SaveSoundInformationPiece(strSoundInformationFileName.c_str(), rCharacterData.SoundDataVector))
					SetStatusBarString(NSound::GetResultString());
			}
		}
		else
		{
			TMotionDataMap::iterator itorMotion = rCharacterData.MotionDataMap.begin();

			for (; itorMotion != rCharacterData.MotionDataMap.end(); ++itorMotion)
			{
				TMotionData & rMotionData = itorMotion->second;

				std::string strMotionPath;
				std::string strMotionName;
				std::string strMotionExtension;
				GetFilePathNameExtension(rMotionData.strFullFileName.c_str(),
										rMotionData.strFullFileName.length(),
										&strMotionPath, &strMotionName, &strMotionExtension);

				std::string strSoundInformationFileName;
				strSoundInformationFileName = strSifPath;
				strSoundInformationFileName += strMotionName;
				strSoundInformationFileName += ".mss";

				if (isLoad)
				{
					if (!NSound::LoadSoundInformationPiece(strSoundInformationFileName.c_str(), rMotionData.SoundDataVector))
						SetStatusBarString(NSound::GetResultString());
				}
				else
				{
					if (!NSound::SaveSoundInformationPiece(strSoundInformationFileName.c_str(), rMotionData.SoundDataVector))
						SetStatusBarString(NSound::GetResultString());
				}
			}
		}
	}

	ms_isEdit = false;
}

CMainFrame::EModelType GetModelType(const char * c_szTypeName)
{
	CMainFrame::EModelType eModelType = CManager::MODEL_TYPE_NONE;
	if (0 == strcmp(c_szTypeName, "PLAYER"))
	{
		eModelType = CManager::MODEL_TYPE_PLAYER;
	}
	else if (0 == strcmp(c_szTypeName, "ENEMY"))
	{
		eModelType = CManager::MODEL_TYPE_ENEMY;
	}
	else if (0 == strcmp(c_szTypeName, "EFFECT"))
	{
		eModelType = CManager::MODEL_TYPE_EFFECT;
	}

	return eModelType;
}

BOOL CMainFrame::LoadSoundScript(const char * c_szFileName)
{
	CTextFileLoader TextFileLoader;
	if (!TextFileLoader.Load(c_szFileName))
		return FALSE;

	TextFileLoader.SetTop();
	LoadSoundScriptGroup(TVI_ROOT, TextFileLoader);

	return TRUE;
}

BOOL CMainFrame::LoadSoundScriptGroup(HTREEITEM hParent, CTextFileLoader & rTextFileLoader)
{
	for (DWORD i = 0; i < rTextFileLoader.GetChildNodeCount(); ++i)
	{
		if (!rTextFileLoader.SetChildNode(i))
			return FALSE;

		std::string strGroupName;
		if (!rTextFileLoader.GetCurrentNodeName(&strGroupName))
			return FALSE;

		std::string strGroupType;
		if (!rTextFileLoader.GetTokenString("type", &strGroupType))
			return FALSE;

		if (0 == strGroupType.compare("GROUP"))
		{
			HTREEITEM hGroupItem = m_resModelList.InsertItem(_T(strGroupName.c_str()), 0, 1, hParent, TVI_LAST);
			if (!LoadSoundScriptGroup(hGroupItem, rTextFileLoader))
				return FALSE;
		}
		else if (0 == strGroupType.compare("PLAYER"))
		{
			if (!LoadSoundScriptModel(hParent, rTextFileLoader, MODEL_TYPE_PLAYER))
				return FALSE;
		}
		else if (0 == strGroupType.compare("NPC"))
		{
			if (!LoadSoundScriptModel(hParent, rTextFileLoader, MODEL_TYPE_NPC))
				return FALSE;
		}
		else if (0 == strGroupType.compare("ENEMY"))
		{
			if (!LoadSoundScriptModel(hParent, rTextFileLoader, MODEL_TYPE_ENEMY))
				return FALSE;
		}
		else if (0 == strGroupType.compare("EFFECT"))
		{
			if (!LoadSoundScriptEffect(hParent, rTextFileLoader))
				return FALSE;
		}

		rTextFileLoader.SetParentNode();
	}

	return TRUE;
}

BOOL CMainFrame::LoadSoundScriptModel(HTREEITEM hParent, CTextFileLoader & rTextFileLoader, EModelType eModelType)
{
	TCharacterData CharacterData;
	CharacterData.eModelType = eModelType;

	if (!rTextFileLoader.GetCurrentNodeName(&CharacterData.strName))
	{
		assert(!"FALSE");
		return FALSE;
	}

	std::string strPathName;
	if (!rTextFileLoader.GetTokenString("pathname", &strPathName))
	{
		assert(!"FALSE");
		return FALSE;
	}

	StringPath(strPathName.c_str(), CharacterData.strPathName);
	CharacterData.strPathName += "/";

	if (!rTextFileLoader.GetTokenString("modelscriptfilename", &CharacterData.strModelScriptFileName))
	{
		assert(!"FALSE");
		return FALSE;
	}

	// Equipment
	LoadSoundScriptEquipment(rTextFileLoader, CharacterData.EquipmentDataList);

	DWORD dwMotionCount;
	if (!rTextFileLoader.GetTokenDoubleWord("motioncount", &dwMotionCount))
	{
		assert(!"FALSE");
		return FALSE;
	}

	for (DWORD i = 0; i < dwMotionCount; ++i)
	{
		if (!rTextFileLoader.SetChildNode("motion", i))
		{
			assert(!"FALSE");
			return FALSE;
		}

		TMotionData MotionData;

		if (!rTextFileLoader.GetTokenString("name", &MotionData.strName))
		{
			assert(!"FALSE");
			return FALSE;
		}
		std::string strFileName;
		if (!rTextFileLoader.GetTokenString("motionscriptfilename", &strFileName))
		{
			assert(!"FALSE");
			return FALSE;
		}

		MotionData.strFullFileName = CharacterData.strPathName;
		MotionData.strFullFileName += strFileName;
		MotionData.strFullFileName += ".msa";

		LoadSoundScriptEquipment(rTextFileLoader, MotionData.EquipmentDataList);

		DWORD dwMotionCRC32 = GetCaseCRC32(MotionData.strFullFileName.c_str(), MotionData.strFullFileName.length());
		CharacterData.MotionDataMap.insert(TMotionDataMap::value_type(dwMotionCRC32, MotionData));

		BOOL isBasisMotion;
		if (rTextFileLoader.GetTokenBoolean("basismotion", &isBasisMotion))
		{
			if (isBasisMotion)
				CharacterData.BasisMotionIDVector.push_back(dwMotionCRC32);
		}

		/////
		rTextFileLoader.SetParentNode();
	}

	DWORD dwCRC32 = GetCaseCRC32(CharacterData.strPathName.c_str(), CharacterData.strPathName.length());
	ms_CharacterDataMap.insert(TCharacterDataMap::value_type(dwCRC32, CharacterData));

	int iIconIndex = 2;
	if (MODEL_TYPE_PLAYER == eModelType)
		iIconIndex = 2;
	else if (MODEL_TYPE_ENEMY == eModelType)
		iIconIndex = 3;

	HTREEITEM hcurItem = m_resModelList.InsertItem(_T(CharacterData.strName.c_str()), iIconIndex, iIconIndex, hParent, TVI_LAST);
	m_resModelList.SetItemData(hcurItem, dwCRC32);

	return TRUE;
}

BOOL CMainFrame::LoadSoundScriptEffect(HTREEITEM hParent, CTextFileLoader & rTextFileLoader)
{
	TCharacterData CharacterData;
	CharacterData.eModelType = MODEL_TYPE_EFFECT;

	if (!rTextFileLoader.GetCurrentNodeName(&CharacterData.strName))
	{
		assert(!"FALSE");
		return FALSE;
	}

	std::string strPathName;
	if (!rTextFileLoader.GetTokenString("pathname", &strPathName))
	{
		assert(!"FALSE");
		return FALSE;
	}

	StringPath(strPathName.c_str(), CharacterData.strPathName);
	CharacterData.strPathName += "/";

	std::string strFileName;
	if (!rTextFileLoader.GetTokenString("effectfilename", &strFileName))
	{
		assert(!"FALSE");
		return FALSE;
	}
	CharacterData.strModelScriptFileName = CharacterData.strPathName;
	CharacterData.strModelScriptFileName += strFileName;

	DWORD dwCRC32 = GetCaseCRC32(CharacterData.strModelScriptFileName.c_str(), CharacterData.strModelScriptFileName.length());
	ms_CharacterDataMap.insert(TCharacterDataMap::value_type(dwCRC32, CharacterData));

	HTREEITEM hcurItem = m_resModelList.InsertItem(_T(CharacterData.strName.c_str()), 4, 4, hParent, TVI_LAST);
	m_resModelList.SetItemData(hcurItem, dwCRC32);

	return TRUE;
}

BOOL CMainFrame::LoadSoundScriptEquipment(CTextFileLoader & rTextFileLoader, TEquipmentDataList & rEquipmentDataList)
{
	CTokenVector * pEquipmentDataTokenVector;
	if (rTextFileLoader.GetTokenVector("equipmentdata", &pEquipmentDataTokenVector))
	{
		for (DWORD j = 0; j < pEquipmentDataTokenVector->size(); j+=3)
		{
			TEquipmentData EquipmentData;
			EquipmentData.iPartIndex = atoi(pEquipmentDataTokenVector->at(j).c_str());
			EquipmentData.strAttachingBoneName = pEquipmentDataTokenVector->at(j+1);
			StringPath(pEquipmentDataTokenVector->at(j+2).c_str(), EquipmentData.strItemScriptFileName);

			DWORD dwCRC32 = GetCaseCRC32(EquipmentData.strItemScriptFileName.c_str(), EquipmentData.strItemScriptFileName.length());
			TItemDataMap::iterator itorItem = ms_ItemDataMap.find(dwCRC32);
			if (ms_ItemDataMap.end() == itorItem)
			{
				EquipmentData.pItemData = new CItemData;
//				EquipmentData.pItemData->LoadItemData(EquipmentData.strItemScriptFileName.c_str());
				EquipmentData.pItemData->SetDefaultItemData("", EquipmentData.strItemScriptFileName.c_str());
				ms_ItemDataMap.insert(TItemDataMap::value_type(dwCRC32, EquipmentData.pItemData));
			}
			else
			{
				EquipmentData.pItemData = itorItem->second;
			}

			rEquipmentDataList.push_back(EquipmentData);
		}
	}

	return TRUE;
}
