#include "StdAfx.h"
#include "RaceDataAccessor.h"

BOOL CRaceDataAccessor::HasShape()
{
	return !m_kMap_dwShapeKey_kShape.empty();
}

CRaceDataAccessor::TShapeDictIterator CRaceDataAccessor::GetShapeDictIterator()
{
	return m_kMap_dwShapeKey_kShape.begin();
}

BOOL CRaceDataAccessor::GetNextIterator(TShapeDictIterator & rIterator)
{
	++rIterator;
	return rIterator != m_kMap_dwShapeKey_kShape.end();
}

DWORD CRaceDataAccessor::GetShapeNumber(TShapeDictIterator & rIterator)
{
	if (m_kMap_dwShapeKey_kShape.end() == m_kMap_dwShapeKey_kShape.find(rIterator->first))
	{
		assert(!"CRaceDataAccessor::GetShapeIndex() - Can't find iterator");
		return 0;
	}

	return rIterator->first;
}

const char * CRaceDataAccessor::GetShapeModelFileName(TShapeDictIterator & rIterator)
{
	if (m_kMap_dwShapeKey_kShape.end() == m_kMap_dwShapeKey_kShape.find(rIterator->first))
	{
		assert(!"CRaceDataAccessor::GetShapeIndex() - Can't find iterator");
		return 0;
	}

	SShape & rShape = rIterator->second;

	return rShape.m_stModelFileName.c_str();
}

CRaceDataAccessor::CRaceDataAccessor()
{
}

CRaceDataAccessor::~CRaceDataAccessor()
{
}
