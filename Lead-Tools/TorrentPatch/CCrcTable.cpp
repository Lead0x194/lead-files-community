#include "Metin2TorrentConsole.h"
#include "CCrcTable.h"
#include "Helper.h"

EL_CCrcTable::EL_CCrcTable()
{
}

EL_CCrcTable::~EL_CCrcTable()
{
}

EL_CCrcTable EL_CCrcTable::DiffWithTable(const EL_CCrcTable& target)
{
	EL_CCrcTable table;

	for( VecFileInfoCItr it = m_vecFileInfo.begin(); it != m_vecFileInfo.end(); it++ ) {
		const EL_FileInfo& info = *it;
		EL_FileInfo targetInfo;
		bool bSameFile = false;
		if (true == target.FindFile(info.path, &targetInfo))
		{
			if (targetInfo.CRC32 == info.CRC32 &&
				targetInfo.size == info.size &&
				targetInfo.dwTimeHigh == info.dwTimeHigh &&
				targetInfo.dwTimeLow == info.dwTimeLow)
				bSameFile = true;
		}

		if (false == bSameFile)
			table.m_vecFileInfo.push_back(info);
	}

	for (EL_SIZE index = 0; index < table.m_vecFileInfo.size(); ++index)
	{
		const EL_FileInfo& info = table.m_vecFileInfo.at(index);
		table.m_mapFileIndex.insert(std::make_pair(info.path, index));
	}

	return table;
}

EL_CCrcTable EL_CCrcTable::DiffWithFiles(void)
{
	EL_CCrcTable table;

	for( VecFileInfoCItr it = m_vecFileInfo.begin(); it != m_vecFileInfo.end(); it++ ) {
		const EL_FileInfo& info = *it;

		bool bDiffFile = EL_TRUE;
		HANDLE hFile = CreateFile(info.path.c_str(),			// name of the file
								 GENERIC_READ,				// desired access
								 FILE_SHARE_READ,			// share mode
								NULL,						// security attributes
								OPEN_EXISTING,				// creation disposition
								FILE_ATTRIBUTE_NORMAL,		// flags and attr
								NULL);						// template file
		if ( hFile != INVALID_HANDLE_VALUE ) {
			LARGE_INTEGER llFileSize;
			FILETIME stModifiedTime;
			if ( ::GetFileSizeEx (hFile, &llFileSize) && ::GetFileTime (hFile, NULL, NULL, &stModifiedTime) ) {
				if (llFileSize.QuadPart == info.size &&
					stModifiedTime.dwHighDateTime == info.dwTimeHigh &&
					stModifiedTime.dwLowDateTime == info.dwTimeLow)
					bDiffFile = EL_FALSE;
			}
			::CloseHandle(hFile);
		}
		if( bDiffFile )
			table.m_vecFileInfo.push_back(info);
	}
	for (EL_SIZE index = 0; index < table.m_vecFileInfo.size(); ++index)
	{
		const EL_FileInfo& info = table.m_vecFileInfo.at(index);
		table.m_mapFileIndex.insert(std::make_pair(info.path, index));
	}
	return table;
}

bool EL_CCrcTable::Load(const std::string& data)
{
	// 0: CRC32
	// 1: Size
	// 2: HighDate
	// 3: LowDate
	// 4: FilePath
	//EL_SIZE step = 0;
	EL_SIZE pos = 0;
	EL_FileInfo fileInfo;
	bool bUseUTF8 = false;

	unsigned char abyBOM[3] = { 0xEF, 0xBB, 0xBF };
	// Check BOM
	if( (data[0] == (char) abyBOM[0]) && (data[1] == (char) abyBOM[1]) && (data[2] == (char) abyBOM[2]) ) {
		pos += 3;
		bUseUTF8 = true;
	}

	while(pos < data.size())
	{
		EL_SIZE end_pos = data.find_first_of("\r\n", pos);
		const std::string line = data.substr(pos, std::string::npos != end_pos ? end_pos - pos : end_pos);

		if( line[0] == '#' ) {
			if( MA_StringCompareA( line.c_str(), "# encoding: utf-8" ) == 0 || MA_StringCompareA( line.c_str(), "# encoding: utf8" ) == 0 )
				bUseUTF8 = true;
		} else {
			fileInfo = EL_FileInfo();

			DWORD dwFileCRC;
			DWORD dwSize;
			DWORD dwHighDate;
			DWORD dwLowDate;
			char szFileName[MA_MAX_PATH] = "";

			sscanf(line.c_str(), "%8x %u %u %u %s", &dwFileCRC, &dwSize, &dwHighDate, &dwLowDate, szFileName);

			char * pszBackslash = strchr(szFileName, '\\');

			while (pszBackslash)
			{
				*pszBackslash = '/';
				pszBackslash = strchr(pszBackslash + 1, '\\');
			}

			if( szFileName[0] ) {
				MA_WCHAR wszFileName[MA_MAX_PATH];

				if( bUseUTF8 )
					Mantle::MA_ECtoWC( (MA_LPCESTR) szFileName, MA_StringLengthA(szFileName), wszFileName, MA_ARRAYCOUNT(wszFileName) );
				else
					Mantle::MA_MBtoWC( szFileName, MA_StringLengthA(szFileName), wszFileName, MA_ARRAYCOUNT(wszFileName) );

				fileInfo.path = wszFileName;
				fileInfo.CRC32 = dwFileCRC;
				fileInfo.size = dwSize;
				fileInfo.dwTimeHigh = dwHighDate;
				fileInfo.dwTimeLow = dwLowDate;
				Add(fileInfo);
			}
		}
		end_pos = data.find_first_of('\n', pos);
		pos = std::string::npos != end_pos ? end_pos + 1 : end_pos;
	}

	return true;
}

void EL_CCrcTable::Add(const EL_FileInfo& fileInfo)
{
	std::map<std::wstring, EL_SIZE>::const_iterator itor = m_mapFileIndex.find(fileInfo.path);
	if (m_mapFileIndex.end() != itor)
	{
		m_vecFileInfo[itor->second] = fileInfo;
	}
	else
	{
		m_vecFileInfo.push_back(fileInfo);
		m_mapFileIndex.insert(std::make_pair(fileInfo.path, m_vecFileInfo.size() - 1));
	}
}

bool EL_CCrcTable::FindFile(const std::wstring& path, EL_FileInfo* outInfo) const
{
	std::map<std::wstring, EL_SIZE>::const_iterator itor = m_mapFileIndex.find(path);
	if (m_mapFileIndex.end() != itor)
	{
		*outInfo = m_vecFileInfo.at(itor->second);
		return true;
	}
	return false;
}
