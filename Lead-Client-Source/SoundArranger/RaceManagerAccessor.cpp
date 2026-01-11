#include "StdAfx.h"
#include "RaceManagerAccessor.h"

void CRaceManagerAccessor::SetRaceData(DWORD dwID, CRaceData * pRaceData)
{
	assert(pRaceData);

	pRaceData->SetRace(dwID);
	m_RaceDataMap.insert(TRaceDataMap::value_type(dwID, pRaceData));

	Tracenf("CRaceManagerAccessor::SetRaceData(dwRaceIndex=%d)", dwID);
}

void CRaceManagerAccessor::DestroyAccessor()
{
	m_RaceDataMap.clear();
}

CRaceManagerAccessor::CRaceManagerAccessor()
{
}

CRaceManagerAccessor::~CRaceManagerAccessor()
{
}
