#pragma once

enum
{
	TORRENT_STATE_QUEUED_FOR_CHECKING,
	TORRENT_STATE_CHECKING,
	TORRENT_STATE_DOWNLOADING_METADATA,
	TORRENT_STATE_DOWNLOADING,
	TORRENT_STATE_FINISHED,
	TORRENT_STATE_SEEDING,
	TORRENT_STATE_ALLOCATING,
	TORRENT_STATE_CHECKING_RESUME_DATA,
	TORRENT_STATE_MAX,
};

extern WCHAR* TORRENT_STATE_NAMES[TORRENT_STATE_MAX];

struct EL_PatchStatus
{
	std::wstring	mainState;
	std::wstring	subState;
	std::wstring	m_strNameState;
	MA_FLOAT		mainRatio;
	MA_FLOAT		subRatio;
	MA_INT			m_iPatchIndex;
	MA_INT			m_iPatchCount;
	MA_INT			dnBytesPerSec;
	MA_INT			upBytesPerSec;
	MA_INT			peersCount;
	MA_INT			totalPeersCount;
	MA_INT			seedsCount;
	MA_INT			totalSeedsCount;
	MA_INT			webSeedsCount;
	MA_BOOL			m_bDisplayMainRatioText;
	MA_BOOL			m_bDisplaySubRatioText;

	EL_PatchStatus()
	: mainRatio(0.0f)
	, subRatio(0.0f)
	, m_iPatchIndex(0)
	, m_iPatchCount(0)
	, dnBytesPerSec(-1)
	, upBytesPerSec(-1)
	, peersCount(-1)
	, totalPeersCount(-1)
	, seedsCount(-1)
	, totalSeedsCount(-1)
	, webSeedsCount(-1)
	, m_bDisplayMainRatioText(MA_FALSE)
	, m_bDisplaySubRatioText(MA_FALSE)
	{}

	void Clear()
	{
		mainState.clear();
		subState.clear();
		m_strNameState.clear();
		mainRatio = 0.0f;
		subRatio = 0.0f;
		m_iPatchIndex = 0;
		m_iPatchCount = 0;
		dnBytesPerSec = -1;
		upBytesPerSec = -1;
		peersCount = -1;
		totalPeersCount = -1;
		seedsCount = -1;
		totalSeedsCount = -1;
		webSeedsCount = -1;

		m_bDisplayMainRatioText	= MA_FALSE;
		m_bDisplaySubRatioText	= MA_FALSE;
	}
};
