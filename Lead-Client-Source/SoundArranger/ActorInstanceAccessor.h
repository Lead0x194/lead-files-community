#pragma once

#include <GameLib/ActorInstance.h>

class CActorInstanceAccessor : public CActorInstance
{
	public:
		CActorInstanceAccessor();
		virtual ~CActorInstanceAccessor();

		void ClearAttachingEffect();

		void SetRaceDataPointer(CRaceData * pRaceData);

		void StartAnimation(float fTime);
		void UpdateFrame();

		void SetMotion(MOTION_KEY dwMotionKey);
		void SetMotionData(CRaceMotionData * pMotionData);

		void RefreshMotionEvent(MOTION_KEY dwMotionKey);
		float GetMotionDuration(DWORD dwMotionKey);
};