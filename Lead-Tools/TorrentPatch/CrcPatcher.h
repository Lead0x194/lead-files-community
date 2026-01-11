#pragma once

#include "CCrcTable.h"
#include "PatchInfo.h"
#include "PatchConfig.h"

class EL_PatchThread;

class EL_CrcPatcher
{
public:
	EL_CrcPatcher();
	EL_CrcPatcher(EL_PatchThread* pcPatchThread);
	virtual ~EL_CrcPatcher();

	void Clear( void );
	MA_BOOL Run(EL_PatchConfig& stConfig, const PatchInfo& stPatchInfo, Earth::EA_CStatus *pcStatus);
	MA_BOOL Patch(const EL_CCrcTable& rcTable, const std::wstring& baseUrl, const std::wstring& basePath, const std::wstring& savePath, Earth::EA_CStatus *pcStatus=NULL);
	MA_BOOL Patch(const std::string& tableData, const std::wstring& baseUrl, const std::wstring& basePath, const std::wstring& savePath, Earth::EA_CStatus *pcStatus=NULL);

	bool _DownloadFileFromHttp(MA_LPCWSTR wszURL, MA_LPCWSTR wszLocalPath, const std::wstring& fileName, Earth::EA_CStatus *pcStatus);

protected:
	virtual void CalculateSpeed(const MA_U64 ullDownloadedBytes, const MA_U64 ullFileSize);
	virtual void _OnCompletedDownload(const std::wstring& fileName, const EL_SIZE fileSize);
	virtual void _OnFinished( void );
	virtual bool _IsAlive() const;

	EL_INT	_GetDataFromHttpW(std::vector<EL_BYTE>* outBuffer, const std::wstring& fileUrl, Earth::EA_CStatus *pcStatus, const std::wstring& fileName);
	void UpdateProgress();

protected:
	EL_PatchThread* m_pcPatchThread;
	MA_INT			m_nFileCount;
	MA_INT			m_nCurrentFileIndex;
	MA_U64			m_ullCurrentDownloadedBytes;
	MA_U64			m_ullCurrentFileSize;
	MA_U64			m_ullTotalDownloadedBytes;
	MA_U64			m_ullTotalSize;
	MA_U64			m_ullStartBytes;
	std::wstring	m_strCurrentFileName;
	DWORD			m_dwStartTime;
};
