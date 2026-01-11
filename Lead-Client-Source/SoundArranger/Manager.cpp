#include "StdAfx.h"
#include "manager.h"

#include <EffectLib/EffectManager.h>
#include <MilesLib/SoundManager.h>

int CManager::ms_iMode;
int CManager::ms_icurType;
bool CManager::ms_isEdit;
bool CManager::ms_PlayFlag;
bool CManager::ms_LoopFlag;
DWORD CManager::ms_dwLocalFrame;
float CManager::ms_fLocalTime;
float CManager::ms_fLimitTime;
int CManager::ms_iFrameCount;
int CManager::ms_iLoopCount;
int CManager::ms_icurLoopCount;
float CManager::ms_fMotionStartTime;
float CManager::ms_fElapsedTime;
CManager::TTimeNodeList CManager::ms_TimeNodeList;
DWORD CManager::ms_dwModelIndex;
DWORD CManager::ms_dwShapeIndex;
DWORD CManager::ms_dwMotionIndex;
DWORD CManager::ms_dwSoundIndex;

CRaceDataAccessor CManager::ms_RaceData;
CRaceManagerAccessor CManager::ms_RaceManager;
CActorInstanceAccessor CManager::ms_ActorInstance;
CEffectData * CManager::ms_pEffectData;

CManager::TCharacterDataMap CManager::ms_CharacterDataMap;
CManager::TChangedDataObserverList CManager::ms_ChangedDataObserverList;
CDynamicPool<CEffectInstance> CManager::ms_EffectInstancePool;
CManager::TEffectInstanceList CManager::ms_EffectInstanceList;
CManager::TSoundFileNameMap CManager::ms_SoundFileNameMap;
CManager::TReserveMotionDeque CManager::ms_ReserveMotionDeque;
CManager::TItemDataMap CManager::ms_ItemDataMap;

void CManager::SetLocalTime(float fTime)
{
	ms_fLocalTime = fTime;
	ms_dwLocalFrame = DWORD(ms_fLocalTime / 0.033f);
}

void CManager::SetLimitTime(float fTime)
{
	ms_fLimitTime = fTime;
	ms_iFrameCount = fTime / 0.033333f;
}

DWORD CManager::GetCurrentModelIndex()
{
	return ms_dwModelIndex;
}

DWORD CManager::GetCurrentMotionIndex()
{
	return ms_dwMotionIndex;
}

DWORD CManager::GetCurrentSoundIndex()
{
	return ms_dwSoundIndex;
}

BOOL CManager::isModel()
{
	return 0 != ms_dwModelIndex;
}

BOOL CManager::isMotion()
{
	return 0 != ms_dwMotionIndex;
}

BOOL CManager::isSound()
{
	return 0 != ms_dwSoundIndex;
}

BOOL CManager::isEffect()
{
	return MODEL_TYPE_EFFECT == ms_icurType;
}

float CManager::GetCurrentTime()
{
	return float(CTimer::Instance().GetCurrentMillisecond()) / 1000.0f;
}

float CManager::GetLimitTime()
{
	return ms_fLimitTime;
}

float CManager::GetLocalTime()
{
	return ms_fLocalTime;
}

struct FChangedModel
{
	void operator() (CManager::IChangedDataObserver * pObserver)
	{
		pObserver->OnChangedModel();
	}
};

struct FChangedMotion
{
	void operator() (CManager::IChangedDataObserver * pObserver)
	{
		pObserver->OnChangedMotion();
	}
};

struct FChangedSound
{
	void operator() (CManager::IChangedDataObserver * pObserver)
	{
		pObserver->OnChangedSound();
	}
};

struct FChangedReserveMotion
{
	void operator() (CManager::IChangedDataObserver * pObserver)
	{
		pObserver->OnChangedReserveMotion();
	}
};

void CManager::SetModelIndex(DWORD dwIndex)
{
	if (dwIndex == ms_dwModelIndex)
		return;

	BOOL bChangedModel = FALSE;
	if (ms_dwModelIndex != dwIndex)
	{
		bChangedModel = TRUE;
	}

	ms_dwModelIndex = dwIndex;
	ms_dwMotionIndex = GetRandomMotionIndex(dwIndex);
	Refresh();

	if (bChangedModel)
		for_each(ms_ChangedDataObserverList.begin(), ms_ChangedDataObserverList.end(), FChangedModel());
}

void CManager::SetMotionIndex(DWORD dwIndex)
{
	if (dwIndex == ms_dwMotionIndex)
		return;

	switch (ms_iMode)
	{
		case MODE_EDIT:
			ms_dwMotionIndex = dwIndex;
			Refresh();

			for_each(ms_ChangedDataObserverList.begin(), ms_ChangedDataObserverList.end(), FChangedMotion());
			break;

		case MODE_VIEW:
			ReserveMotionIndex(dwIndex);
			for_each(ms_ChangedDataObserverList.begin(), ms_ChangedDataObserverList.end(), FChangedReserveMotion());
			break;
	}
}

void CManager::ReserveMotionIndex(DWORD dwIndex)
{
	ms_ReserveMotionDeque.push_back(dwIndex);
}

void CManager::SetSoundIndex(DWORD dwIndex)
{
	ms_dwSoundIndex = dwIndex;

	for_each(ms_ChangedDataObserverList.begin(), ms_ChangedDataObserverList.end(), FChangedSound());
}

void CManager::ClearSoundIndex()
{
	SetSoundIndex(0);
}

void CManager::InsertSoundData(DWORD dwModelIndex, DWORD dwMotionIndex, float fTime, const char * c_szFileName)
{
	TCharacterData * pCharacterData;
	if (!GetCharacterDataPointer(dwModelIndex, &pCharacterData))
		return;

	NSound::TSoundData SoundData;
	SoundData.fTime = fTime;
	SoundData.strSoundFileName = c_szFileName;

	if (isEffect())
	{
		InsertSoundData(&pCharacterData->SoundDataVector, SoundData);
	}
	else
	{
		TMotionDataMap::iterator itorMotion = pCharacterData->MotionDataMap.find(ms_dwMotionIndex);
		if (itorMotion == pCharacterData->MotionDataMap.end())
			return;

		TMotionData * pMotionData = &itorMotion->second;

		InsertSoundData(&pMotionData->SoundDataVector, SoundData);
	}
}

void CManager::InsertSoundData(NSound::TSoundDataVector * pSoundDataVector, const NSound::TSoundData & c_rSoundData)
{
	int iSize = int(pSoundDataVector->size()) - 1;
	int i;

	for (i = 0; i < iSize; ++i)
	{
		if (c_rSoundData.fTime >= pSoundDataVector->at(i).fTime)
		if (c_rSoundData.fTime <= pSoundDataVector->at(i+1).fTime)
			break;
	}

	NSound::TSoundDataVector::iterator itor = pSoundDataVector->begin();

	for (int j = 0; j < i; ++j)
		++itor;

	pSoundDataVector->insert(itor, c_rSoundData);
}

void CManager::Refresh()
{
	ms_PlayFlag = false;
	ms_fLocalTime = 0.001f;
	ms_dwLocalFrame = 0;
	SetLimitTime(0.0f);
	ms_fMotionStartTime = float(CTimer::Instance().GetCurrentMillisecond()) / 1000.0f;
	ms_TimeNodeList.clear();
	ms_ActorInstance.__SetLocalTime(ms_fLocalTime);

	//////////////////////////////////////

	ms_RaceData.Destroy();
	ms_ActorInstance.Clear();
	ms_ActorInstance.ClearAttachingEffect();
	ms_pEffectData->Clear();
	CEffectManager::Instance().DeleteAllInstances();
	DestroyEffect();

	//////////////////////////////////////

	TCharacterDataMap::iterator itorModel = ms_CharacterDataMap.find(ms_dwModelIndex);
	if (itorModel == ms_CharacterDataMap.end())
		return;

	TCharacterData & rCharacterData = itorModel->second;
	ms_icurType = rCharacterData.eModelType;

	if (MODEL_TYPE_EFFECT == rCharacterData.eModelType)
	{
		const char * c_szFileName = rCharacterData.strModelScriptFileName.c_str();
		ms_pEffectData->Clear();
		if (!ms_pEffectData->LoadScript(c_szFileName))
		{
			Tracef(" Failed to load effect : %s\n", c_szFileName);
		}
		SetLimitTime(1.0f);
	}
	else
	{
		// Model
		ms_RaceData.Destroy();
		ms_RaceData.LoadRaceData(rCharacterData.strModelScriptFileName.c_str());
		ms_ActorInstance.SetRaceDataPointer(&ms_RaceData);
		ms_ActorInstance.RefreshActorInstance();
		ms_ActorInstance.SetShape(ms_dwShapeIndex);
		ms_ActorInstance.SetFlyTarget(D3DXVECTOR3(0.0f, -1000.0f, 50.0f));
		ms_ActorInstance.SetFishingPosition(D3DXVECTOR3(0.0f, -300.0f, 0.0f));
		RefreshEquipment(rCharacterData.EquipmentDataList); // Equipment of Model

		// Motion - Temporary
		//          시간이 나면 전체 모션을 전부 등록하는 방식으로 바꾼다.
		//          그런 형태가 되면 Refresh는 모델이 바뀌었을 때만 재셋팅 된다.
		//          또한 ViewMode도 구현이 수월해질 것이라 예상한다.
		ms_RaceData.RegisterMotionMode(0);
		TMotionDataMap::iterator itorMotion = rCharacterData.MotionDataMap.find(ms_dwMotionIndex);
		if (itorMotion != rCharacterData.MotionDataMap.end())
		{
			TMotionData & rMotionData = itorMotion->second;

			DWORD dwID = 0;

			ms_RaceData.RegisterMotionData(0, 0, rMotionData.strFullFileName.c_str());
			CRaceMotionData * pRaceMotionData;
			if (ms_RaceData.GetMotionDataPointer(dwID, &pRaceMotionData))
			{
				const char * c_szFileName = pRaceMotionData->GetMotionFileName();
				CGraphicThing * pMotion = (CGraphicThing *)CResourceManager::Instance().GetResourcePointer(c_szFileName);
				ms_ActorInstance.RegisterMotionThing(dwID, pMotion);
				ms_ActorInstance.SetMotion(0);
				ms_ActorInstance.SetMotionData(pRaceMotionData);
				ms_ActorInstance.SetPosition(0.0f, 0.0f, 0.0f);

				SetLimitTime(ms_ActorInstance.GetMotionDuration(0));
				ms_iLoopCount = pRaceMotionData->GetLoopCount();
				ms_icurLoopCount = ms_iLoopCount;
			}

			// Equipment of Motion
			RefreshEquipment(rMotionData.EquipmentDataList);
		}
	}
}

void CManager::RefreshEquipment(const TEquipmentDataList & c_rEquipmentDataList)
{
	TEquipmentDataList::const_iterator itor = c_rEquipmentDataList.begin();
	for (; itor != c_rEquipmentDataList.end(); ++itor)
	{
		const TEquipmentData & c_rEquipmentData = *itor;

		ms_RaceData.RegisterAttachingBoneName(c_rEquipmentData.iPartIndex,
												c_rEquipmentData.strAttachingBoneName.c_str());

		ms_ActorInstance.AttachWeapon(0, c_rEquipmentData.iPartIndex, c_rEquipmentData.pItemData);
	}
}

void CManager::BlendMotion(DWORD dwIndex, float fBlendTime)
{
	if (ms_dwMotionIndex == dwIndex)
		return;

	TCharacterDataMap::iterator itorModel = ms_CharacterDataMap.find(ms_dwModelIndex);
	if (itorModel == ms_CharacterDataMap.end())
		return;

	TCharacterData & rCharacterData = itorModel->second;
	TMotionDataMap::iterator itorMotion = rCharacterData.MotionDataMap.find(dwIndex);
	if (itorMotion != rCharacterData.MotionDataMap.end())
	{
		const std::string & c_rMotionFileName = itorMotion->second.strFullFileName;
		CGraphicThing * pMotion = (CGraphicThing *)CResourceManager::Instance().GetResourcePointer(c_rMotionFileName.c_str());

		/////

		ms_dwMotionIndex = dwIndex;
		for_each(ms_ChangedDataObserverList.begin(), ms_ChangedDataObserverList.end(), FChangedMotion());
	}
}

void CManager::RegisterChangedDataObserver(IChangedDataObserver * pObserver)
{
	ms_ChangedDataObserverList.push_back(pObserver);
}

BOOL CManager::GetCharacterDataPointer(DWORD dwIndex, TCharacterData ** ppCharacterData)
{
	TCharacterDataMap::iterator itorModel = ms_CharacterDataMap.find(dwIndex);
	if (itorModel == ms_CharacterDataMap.end())
		return FALSE;

	*ppCharacterData = &itorModel->second;

	return TRUE;
}

BOOL CManager::GetCharacterMotionDataPointer(DWORD dwModelIndex, DWORD dwMotionIndex, TMotionData ** ppMotionData)
{
	TCharacterData * pCharacterData;
	if (!GetCharacterDataPointer(dwModelIndex, &pCharacterData))
		return FALSE;

	TMotionDataMap::iterator itorMotion = pCharacterData->MotionDataMap.find(ms_dwMotionIndex);
	if (itorMotion == pCharacterData->MotionDataMap.end())
		return FALSE;

	*ppMotionData = &itorMotion->second;

	return TRUE;
}

BOOL CManager::GetCharacterSoundDataVector(DWORD dwModelIndex, DWORD dwMotionIndex, NSound::TSoundDataVector ** ppSoundDataVector)
{
	if (MODEL_TYPE_EFFECT == ms_icurType)
	{
		TCharacterData * pCharacterData;
		if (!GetCharacterDataPointer(dwModelIndex, &pCharacterData))
			return FALSE;

		*ppSoundDataVector = &(pCharacterData->SoundDataVector);
	}
	else
	{
		TMotionData * pMotionData;
		if (!GetCharacterMotionDataPointer(dwModelIndex, dwMotionIndex, &pMotionData))
			return FALSE;

		*ppSoundDataVector = &(pMotionData->SoundDataVector);
	}
	return TRUE;
}

BOOL CManager::GetSoundFileName(DWORD dwSoundIndex, const char ** c_pszFileName)
{
	TSoundFileNameMap::iterator itor = ms_SoundFileNameMap.find(dwSoundIndex);
	if (itor == ms_SoundFileNameMap.end())
		return FALSE;

	*c_pszFileName = itor->second.c_str();

	return TRUE;
}

DWORD CManager::GetRandomMotionIndex(DWORD dwModelIndex)
{
	TCharacterData * pCharacterData;
	if (GetCharacterDataPointer(GetCurrentModelIndex(), &pCharacterData))
	if (!pCharacterData->BasisMotionIDVector.empty())
	{
		int iRandomMotionArrayIndex = rand() % int(pCharacterData->BasisMotionIDVector.size());
		DWORD dwRandomMotionIndex = pCharacterData->BasisMotionIDVector[iRandomMotionArrayIndex];
		return dwRandomMotionIndex;
	}

	return 0;
}

void CManager::TimeProcess()
{
	// Getting Elapsed Time
	static DWORD s_dwLastTime = CTimer::Instance().GetCurrentMillisecond();
	ms_fElapsedTime = float(CTimer::Instance().GetCurrentMillisecond() - s_dwLastTime) / 1000.0f;
	s_dwLastTime = CTimer::Instance().GetCurrentMillisecond();
	// Getting Elapsed Time

	switch (ms_iMode)
	{
		case MODE_EDIT:
			if (ms_PlayFlag)
			{
				ms_fLocalTime += ms_fElapsedTime;

				UpdateSound();
				++ms_dwLocalFrame;

				CRaceMotionData * pRaceMotionData;
				if (ms_RaceData.GetMotionDataPointer(0, &pRaceMotionData))
				if (pRaceMotionData->IsLoopMotion())
				{
					if (ms_fLocalTime > pRaceMotionData->GetLoopEndTime())
					{
						if (ms_icurLoopCount > 1 || ms_icurLoopCount == -1)
						{
							if (ms_icurLoopCount > 1)
								--ms_icurLoopCount;
							SetLocalTime(pRaceMotionData->GetLoopStartTime());
						}
					}
				}

				if (ms_fLocalTime > ms_fLimitTime)
				{
					ms_fLocalTime = 0.001f;
					ms_dwLocalFrame = 0;
					ms_icurLoopCount = ms_iLoopCount;

					ms_PlayFlag = false;

					if (ms_LoopFlag)
						PlayMotion(TRUE);
				}
			}
			break;
		case MODE_VIEW:
			ms_fLocalTime += ms_fElapsedTime;
			if (ms_fLocalTime-ms_fMotionStartTime > ms_fLimitTime - 0.3f)
			{
				ms_fMotionStartTime = float(CTimer::Instance().GetCurrentMillisecond()) / 1000.0f;

				if (ms_ReserveMotionDeque.empty())
				{
					DWORD dwRandomMotionIndex = GetRandomMotionIndex(GetCurrentModelIndex());
					if (0 != dwRandomMotionIndex)
					{
						BlendMotion(dwRandomMotionIndex, 0.3f);
					}
				}
				else
				{
					DWORD dwMotionIndex = ms_ReserveMotionDeque.front();
					BlendMotion(dwMotionIndex, 0.3f);
					ms_ReserveMotionDeque.pop_front();
					for_each(ms_ChangedDataObserverList.begin(), ms_ChangedDataObserverList.end(), FChangedReserveMotion());
				}
			}
			UpdateSound();
			++ms_dwLocalFrame;
			break;
	}
}

void CManager::UpdateSound()
{
	NSound::TSoundDataVector * pSoundDataVector;
	if (!GetCharacterSoundDataVector(GetCurrentModelIndex(), GetCurrentMotionIndex(), &pSoundDataVector))
		return;

	float fcurTime = GetCurrentTime();
	for (TTimeNodeList::iterator itor = ms_TimeNodeList.begin(); itor != ms_TimeNodeList.end();)
	{
		TTimeNode & rTimeNode = *itor;

		NSound::TSoundInstanceVector SoundInstanceVector;
		NSound::DataToInstance(*pSoundDataVector, &SoundInstanceVector);

		// Start - Update SoundInstanceVector
		for (DWORD i = 0; i < SoundInstanceVector.size(); ++i)
		{
			const NSound::TSoundInstance & c_rSoundInstance = SoundInstanceVector[i];

			if (c_rSoundInstance.dwFrame == ms_dwLocalFrame)
			{
				CSoundManager::Instance().PlaySound2D(c_rSoundInstance.strSoundFileName.c_str());
			}
		}
		// End - Update SoundInstanceVector

		if (fcurTime - rTimeNode.fStartTime >= ms_fLimitTime)
		{
			itor = ms_TimeNodeList.erase(itor);
		}
		else
		{
			++itor;
		}
	}
}

void CManager::PlayMotion(BOOL bLoopFlag)
{
	if (MODE_VIEW == ms_iMode)
		return;

	if (!isMotion() && !isEffect())
		return;

	if (ms_PlayFlag)
	{
		SetLocalTime(0.0f);
		//return;
	}

	ms_PlayFlag = true;
	ms_LoopFlag = bLoopFlag;

	// Temporary
	if (MODEL_TYPE_EFFECT == ms_icurType)
	{
		CEffectInstance * pInstance = ms_EffectInstancePool.Alloc();
		pInstance->Clear();
		pInstance->SetEffectDataPointer(ms_pEffectData);
		ms_EffectInstanceList.push_back(pInstance);

		TTimeNode TimeNode;
		TimeNode.fStartTime = GetCurrentTime();
		TimeNode.fLastTime = GetCurrentTime();
		ms_TimeNodeList.push_back(TimeNode);
	}
	else
	{
		TTimeNode TimeNode;
		TimeNode.fStartTime = GetCurrentTime() - GetLocalTime() + 0.001f;
		TimeNode.fLastTime = GetCurrentTime();
		ms_TimeNodeList.push_back(TimeNode);

		ms_ActorInstance.RefreshMotionEvent(0);
		ms_ActorInstance.StartAnimation(GetLocalTime());
	}
	// Temporary
}

void CManager::RegisterSoundFileName(DWORD dwID, const char * c_szFileName)
{
	ms_SoundFileNameMap.insert(TSoundFileNameMap::value_type(dwID, c_szFileName));
}

void CManager::ClearAllSoundFileName()
{
	ms_SoundFileNameMap.clear();
}

void CManager::Edit()
{
	ms_isEdit = true;
}

void CManager::UnEdit()
{
	ms_isEdit = false;
}

bool CManager::isEdit()
{
	return ms_isEdit;
}

bool CManager::canEdit()
{
	return MODE_EDIT == ms_iMode;
}

void CManager::DestroyEffect()
{
	TEffectInstanceList::iterator itor = ms_EffectInstanceList.begin();
	for (; itor != ms_EffectInstanceList.end(); ++itor)
	{
		CEffectInstance * pInstance = *itor;
		pInstance->Clear();
	}
	ms_EffectInstanceList.clear();
}

CManager::CManager()
{
	ms_iLoopCount = 0;
	ms_icurLoopCount = 0;
	ms_fMotionStartTime = 0.0f;
}

CManager::~CManager()
{
}
