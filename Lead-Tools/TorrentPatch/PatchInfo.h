#pragma once

class PatchInfo
{
public:
	typedef std::vector<std::string> WebseedUrls;

	PatchInfo() :
	randomStart(0),
		triggerTime(10),
		triggerBandwidth(4096)
	{
	}

	MA_BOOL		IsTorrentInfo( void ) const			{ return torrentUrl.size() != 0; }
	MA_BOOL		IsCrcPatchInfo( void ) const		{ return crcpatchUrl.size() != 0; }

	// For BitTorrent
	WebseedUrls  webseedUrls;
	std::wstring torrentUrl;

	EL_UINT randomStart;

	EL_UINT triggerTime;
	EL_UINT triggerBandwidth;

	// For CRC32
	std::wstring crcpatchUrl;
	std::wstring crcpatchVersion;
};

typedef std::vector<PatchInfo> PatchInfos;

inline bool PatchInfos_HasCrcListPatch( const PatchInfos& vecPatchInfos )
{
	BOOST_FOREACH( const PatchInfo& patchInfo, vecPatchInfos ) {
		if( patchInfo.crcpatchUrl.empty() == false )
			return true;
	}
	return false;
}

inline bool PatchInfos_HasTorrentPatch( const PatchInfos& vecPatchInfos )
{
	BOOST_FOREACH( const PatchInfo& patchInfo, vecPatchInfos ) {
		if( patchInfo.torrentUrl.empty() == false )
			return true;
	}
	return false;
}

inline MA_INT PatchInfos_GetPatchCount( const PatchInfos& vecPatchInfos )
{
	return vecPatchInfos.size();
}
