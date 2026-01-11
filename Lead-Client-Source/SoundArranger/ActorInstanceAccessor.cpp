#include "StdAfx.h"
#include "ActorInstanceAccessor.h"

#include <GameLib/RaceData.h>

void CActorInstanceAccessor::SetRaceDataPointer(CRaceData * pRaceData)
{
	m_pkCurRaceData = pRaceData;

	CGraphicThingInstance::Clear();
	CGraphicThingInstance::ReserveModelThing(CRaceData::PART_MAX_NUM);
	CGraphicThingInstance::ReserveModelInstance(CRaceData::PART_MAX_NUM);

	CGraphicThing* pModelThing = pRaceData->GetBaseModelThing();
	RegisterModelThing(0, pModelThing);

	SetModelInstance(0, 0, 0);
}

void CActorInstanceAccessor::StartAnimation(float fTime)
{
	m_kCurMotNode.dwcurFrame = DWORD(fTime / 0.033f);
}

void CActorInstanceAccessor::UpdateFrame()
{
	MotionEventProcess();
	++m_kCurMotNode.dwcurFrame;
}

void CActorInstanceAccessor::SetMotion(DWORD dwMotionKey)
{
	// Temporary - For strange motion start
	if (m_LODControllerVector.empty())
		return;

	CGrannyLODController * pController = m_LODControllerVector[0];
	CGrannyModelInstance * pModelInstance = pController->GetModelInstance();

	if (pModelInstance)
	{
		pModelInstance->SetLocalTime(0.001f);
		CGraphicThingInstance::SetMotion(dwMotionKey);
	}
}

void CActorInstanceAccessor::SetMotionData(CRaceMotionData * pMotionData)
{
	m_pkCurRaceMotionData = pMotionData;
}

void CActorInstanceAccessor::RefreshMotionEvent(MOTION_KEY dwMotionKey)
{
	CRaceMotionData * pMotionData;
	if (m_pkCurRaceData->GetMotionDataPointer(dwMotionKey, &pMotionData))
	{
		// 궁극적으로는 Instance를 만들지 않고 Data로 처리 하게 한다. - [levites]
		for (DWORD i = 0; i < pMotionData->GetMotionEventDataCount(); ++i)
		{
			const CRaceMotionData::TMotionEventData * c_pData;
			if (!pMotionData->GetMotionEventDataPointer(i, &c_pData))
				continue;

			if (CRaceMotionData::MOTION_EVENT_TYPE_EFFECT == c_pData->iType)
			{
//				const CRaceMotionData::TMotionEffectEventData * c_pEffectData = (const CRaceMotionData::TMotionEffectEventData *)c_pData;
//				CEffectManager::Instance().RegisterEffect(c_pEffectData->strEffectFileName.c_str());

				/////

/*
				std::string strFileName;
				StringPath(c_szFileName, strFileName);
				DWORD dwCRC = GetCaseCRC32(strFileName.c_str(), strFileName.length());

				CEffectData * pEffect;
				if (CEffectManager::Instance().GetEffectData(dwCRC, &pEffect))
				{
					DWORD i;
					for (i = 0; i < pEffectData->GetParticleCount(); ++i)
					{
						CParticleSystemData * pParticle = pEffectData->GetParticlePointer(i);
						pParticle->GetParticlePropertyPointer();
					}

					for (i = 0; i < pEffectData->GetMeshCount(); ++i)
					{
						CEffectMesh * pMesh = pEffectData->GetMeshPointer(i);
					}
				}
*/
			}

			TMotionEventInstance MotionEventInstance;
			MotionEventInstance.iType = c_pData->iType;
			MotionEventInstance.iMotionEventIndex = i;
			MotionEventInstance.fStartingTime = c_pData->fStartingTime;
			MotionEventInstance.c_pMotionData = c_pData;
		}
	}
}

float CActorInstanceAccessor::GetMotionDuration(DWORD dwMotionKey)
{
	return CActorInstance::GetMotionDuration(dwMotionKey);
}

void CActorInstanceAccessor::ClearAttachingEffect()
{
	__ClearAttachingEffect();
}

CActorInstanceAccessor::CActorInstanceAccessor()
{
}
CActorInstanceAccessor::~CActorInstanceAccessor()
{
}
