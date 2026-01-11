#pragma once

#ifndef NO_LIBTORRENT
class EL_PatchThread;

namespace libtorrent
{
	class session;
};

class EL_TorrentPatcher
{
public:
	EL_TorrentPatcher();
	EL_TorrentPatcher(EL_PatchThread* pcPatchThread);

	bool Begin(EL_PatchConfig& stConfig, Earth::EA_CStatus *pcStatus);
	bool Patch(EL_PatchConfig& stConfig, PatchInfos& cPatchInfos, Earth::EA_CStatus *pcStatus);
	bool Seeding(EL_PatchConfig& stConfig);
	bool End( void );

public:
	virtual ~EL_TorrentPatcher();	

protected:
	bool __TorrentMain(EL_PatchConfig& config, EL_PatchStatus& status, PatchInfos& cPatchInfos, Earth::EA_CStatus *pcStatus=NULL);
	bool __DownloadPatches(const PatchInfos& patchInfos, EL_PatchConfig& config, Earth::EA_CStatus *pcStatus=NULL);
	bool __DownloadTorrentBody(const std::string& torrentHead, const PatchInfo& patchInfo, EL_PatchConfig& config, const std::string& fastResumeName, Earth::EA_CStatus *pcStatus=NULL);
	void __SetBandwidth(MA_UINT bandwidth);

	void UpdateProgressPeerAndSpeed();

private:

private:
	libtorrent::session*		m_pcSession;
	EL_PatchThread*				m_pcPatchThread;
};

#endif
