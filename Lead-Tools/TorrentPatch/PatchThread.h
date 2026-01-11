#pragma once

#include "PatchInfo.h"
#include "PatchConfig.h"

class EL_CrcPatcher;
class EL_TorrentPatcher;
class EL_TorrentPatchPanel;
class EL_TorrentPatchTaskBarIcon;

struct SDeleteFileInfo
{
	typedef std::list<SDeleteFileInfo>	ListType;

	enum EDeleteTime
	{
		BEFORE_PATCH = 0,
		AFTER_PATCH,			
	};

	SDeleteFileInfo(const std::string& path, EDeleteTime delTime = BEFORE_PATCH) : filePath(path), deleteTime(delTime)
	{}

	void DoDelete(EDeleteTime delTime);

	std::string filePath;
	EDeleteTime deleteTime;
};


enum EL_EPatcherState
{
	EL_EPatcherState_Initialize,
	EL_EPatcherState_DownloadConfigFile,
	EL_EPatcherState_StartupSession,
	EL_EPatcherState_ForegroundPatch,
	EL_EPatcherState_RunGame,
	EL_EPatcherState_BackgroundPatch,
	EL_EPatcherState_Done,
	EL_EPatcherState_Max
};

enum ETransferStage
{
	ETRANSFER_STAGE_FOREGROUND,
	ETRANSFER_STAGE_BACKGROUND,
	ETRANSFER_STAGE_IDLE
};

class EL_PatchThread : public wxThread
{
public:
	friend class EL_CrcPatcher;

	EL_PatchThread(EL_TorrentPatchPanel* panelPtr, EL_TorrentPatchTaskBarIcon* taskBarIconPtr);
	virtual ~EL_PatchThread();

public:
	virtual void* Entry();
	virtual void OnExit();
	void	Terminate();

	MA_BOOL	LoadPatchConfiguration( Earth::EA_CStatus* pcStatus );
	MA_BOOL	ProcessPatchStage( ETransferStage eStage, PatchInfos& vecPatchInfo, Earth::EA_CStatus* pcStatus );
	MA_BOOL	ProcessIdleStage( void );
	MA_BOOL	InitNetwork( Earth::EA_CStatus* pcStatus );
	void	CloseNetwork();

	void	UpdateUI( void );
	void	SetUIMainState( MA_LPCWSTR wszText, MA_BOOL bDisplayMainRatioText = MA_TRUE );
	void	SetUISubState( MA_LPCWSTR wszText, MA_BOOL bDisplaySubRatioText = MA_FALSE );
	void	SetUINameState( MA_LPCWSTR wszText );
	void	SetUITotalProgress( MA_FLOAT fRatio );
	void	SetUICurrentProgress( MA_FLOAT fRatio, MA_BOOL bAffectMainRatio = MA_TRUE );
	void	SetUICurrentFileProgress( MA_FLOAT fTotalSizeRatio, MA_FLOAT fCurrentFileRatio );
	void	SetUICurrentSpeed( MA_FLOAT fDownloadBPS, MA_FLOAT fUploadBPS = -1.0f ); /* Bytes Per Seconds */
	void	SetUICurrentPeers( MA_INT nActiveSeeds, MA_INT nSeeds, MA_INT nActivePeers, MA_INT nPeers );
	void	SetUINextSubState( void );

public:
	MA_BOOL	UpdateSpawning( void );

	bool	__IsNewPatcher();
	bool	__BackToMainPatcher();
	bool	__LaunchNewPatcher();
	bool	__DownloadNewPatcherFromWeb(bool bToMainPatcher);

	bool	__ParseDeleteFileInfoElement(TiXmlElement* pkFirstElement);
	bool	__ParsePatchElement(TiXmlElement* pkFirstElement, const std::string& xmlElementName, EL_PatchConfig* retConfig, PatchInfos* retPatchInfos, Earth::EA_CStatus* pcStatus = NULL);
	bool	__ParsePatchInfoXmlData(const std::string& xmlData, EL_PatchConfig* retConfig, PatchInfos* retFgPatchInfos, PatchInfos* retBgPatchInfos, Earth::EA_CStatus* pcStatus = NULL);

	//void __SetForegroundTransferState(libtorrent::session& ses, EL_PatchConfig& config);
	//void __SetBackgroundTransferState(libtorrent::session& ses, EL_PatchConfig& config);

public:
	virtual bool	_IsAlive();
	virtual MA_BOOL	_PollRunEvent();

private:
	virtual void	_PopRunEvent();

	virtual void	_Notify(const std::wstring& text);

	virtual void	_OnConfig(const EL_PatchConfig& config);
	virtual void	_OnSpawned();
	virtual void	_OnFinished();
	virtual void	_OnReadyForGameStart(bool bRunWithTerminate);
	virtual void	_OnError(const std::wstring& text);
	virtual bool	_OnNoticeEULA(const std::wstring& url, const wxSize& size);

public:
	static bool	sGetCRC32(MA_LPCTSTR tszFileName, EL_UINT* puCRC32);
	static bool	sGetSelfCRC32(EL_UINT* puCRC32);

protected:
	EL_TorrentPatchPanel*		m_panelPtr;
	EL_TorrentPatchTaskBarIcon*	m_taskBarIconPtr;
	volatile bool				m_bTerminate;
	ETransferStage				m_eTransferStage;

	PatchInfos					m_vecForegroundPatchInfos;
	PatchInfos					m_vecBackgroundPatchInfos;
	SDeleteFileInfo::ListType	m_listDeleteFileInfo;

	EL_PatchConfig				m_stPatchConfig;
	EL_PatchStatus				m_stPatchStatus;

	EL_TorrentPatcher*			m_pcTorrentPatcher;
	EL_CrcPatcher*				m_pcCrcPatcher;
};
