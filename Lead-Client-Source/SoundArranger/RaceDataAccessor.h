#pragma once

#include <GameLib/RaceData.h>

class CRaceDataAccessor : public CRaceData
{
	public:
		typedef std::map<DWORD, SShape> TShapeDict;
		typedef TShapeDict::iterator TShapeDictIterator;

	public:
		CRaceDataAccessor();
		virtual ~CRaceDataAccessor();

		BOOL HasShape();

		// Iterator
		TShapeDictIterator GetShapeDictIterator();
		BOOL GetNextIterator(TShapeDictIterator & rIterator);

		DWORD GetShapeNumber(TShapeDictIterator & rIterator);
		const char * GetShapeModelFileName(TShapeDictIterator & rIterator);
};