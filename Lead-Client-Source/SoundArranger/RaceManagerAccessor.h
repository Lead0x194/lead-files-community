#pragma once

#include <GameLib/RaceManager.h>

class CRaceManagerAccessor : public CRaceManager
{
	public:
		CRaceManagerAccessor();
		virtual ~CRaceManagerAccessor();

		void DestroyAccessor();
		
		void SetRaceData(DWORD dwID, CRaceData * pRaceData);
};
