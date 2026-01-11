#pragma once

#include <Mantle/MA_STLVector.h>
#include <Mantle/MA_STLMap.h>

struct EL_FileInfo
{
	std::wstring path;
	DWORD CRC32;
	DWORD dwTimeHigh;
	DWORD dwTimeLow;
	EL_SIZE size;
};

class EL_CCrcTable
{
	MA_DEFINE_STL_VECTOR(FileInfo, EL_FileInfo);
	MA_DEFINE_STL_MAP(FileIndex, Mantle::MA_STLTString, MA_UINT);
public:
	EL_CCrcTable();
	~EL_CCrcTable();

	EL_CCrcTable		DiffWithTable(const EL_CCrcTable& target);
	EL_CCrcTable		DiffWithFiles();
	bool				Load(const std::string& data);
	void				Add(const EL_FileInfo& fileInfo);
	bool				FindFile(const std::wstring& path, EL_FileInfo* outInfo) const;
	const VecFileInfo&	GetFiles() const { return m_vecFileInfo; }
	MA_SIZE				Count(void) const { return m_vecFileInfo.size(); }

protected:
	VecFileInfo						m_vecFileInfo;
	MapFileIndex					m_mapFileIndex;
};
