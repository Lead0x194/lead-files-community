#pragma once

struct EL_PatchConfig
{
	EL_PatchConfig()
	: minPort(6881)
	, maxPort(6891)
	, maxHalfOpenConnections(4)
	, maxUploadsPerTorrent(4)
	, maxConnectionsPerTorrent(50)
	, maxUploadsPerSession(16)
	, maxConnectionsPerSession(200)
	, seedingDuration(3)
	, saveDir(EL_LTEXT("./"))
	, currentBandwidth(0)
	, foregroundBandwidth(0)
	, backgroundBandwidth(0)
	, eulaVer(0)
	, eulaWidth(640)
	, eulaHeight(480)
	, webSeedTriggerSeconds(30)
	, webSeedTriggerBandwidth(4096)
	, isWebSeedEnable(true)
	, isRunPathEnable(true)
	, isPostPatchEnable(false)
	, newPatcherCRC32(0)
	{
	}

	EL_PatchConfig(const EL_PatchConfig & config)
	: minPort(config.minPort)
	, maxPort(config.maxPort)
	, maxHalfOpenConnections(config.maxHalfOpenConnections)
	, maxUploadsPerTorrent(config.maxUploadsPerTorrent)
	, maxConnectionsPerTorrent(config.maxConnectionsPerTorrent)
	, maxUploadsPerSession(config.maxUploadsPerSession)
	, maxConnectionsPerSession(config.maxConnectionsPerSession)
	, seedingDuration(config.seedingDuration)
	, isWebSeedEnable(config.isWebSeedEnable)
	, isRunPathEnable(config.isRunPathEnable)
	, saveDir(config.saveDir)
	, runPath(config.runPath)
	, bandwidths(config.bandwidths)
	, currentBandwidth(config.currentBandwidth)
	, foregroundBandwidth(config.foregroundBandwidth)
	, backgroundBandwidth(config.backgroundBandwidth)
	, remoteConfigXmlPath(config.remoteConfigXmlPath)
	, isPostPatchEnable(config.isPostPatchEnable)
	, newPatcherCRC32(config.newPatcherCRC32)
	, newPatcherPath(config.newPatcherPath)
	, eulaVer(config.eulaVer)
	, eulaWidth(config.eulaWidth)
	, eulaHeight(config.eulaHeight)
	, eulaUrl(config.eulaUrl)
	, webSeedTriggerSeconds(config.webSeedTriggerSeconds)
	, webSeedTriggerBandwidth(config.webSeedTriggerSeconds)
	, xTrapPatcherArg( config.xTrapPatcherArg )
	{
	}

	std::wstring remoteConfigXmlPath;
	std::wstring saveDir;

	bool isRunPathEnable;
	bool isPostPatchEnable;

	std::wstring newPatcherURL;
	std::wstring newPatcherPath;
	std::wstring curPatcherPath;
	EL_UINT	newPatcherCRC32;
	std::wstring runPath;

	// EULA(End-User License Agreement)
	EL_UINT eulaVer;
	EL_UINT eulaWidth;
	EL_UINT eulaHeight;
	std::wstring eulaUrl;

	// BitTorrent
	typedef std::vector<EL_INT> Bandwidths;

	EL_UINT minPort;
	EL_UINT maxPort;

	EL_UINT maxHalfOpenConnections;

	EL_UINT maxUploadsPerTorrent;
	EL_UINT maxConnectionsPerTorrent;

	EL_UINT maxUploadsPerSession;
	EL_UINT maxConnectionsPerSession;	
	
	EL_UINT seedingDuration;
	bool isWebSeedEnable;
	
	EL_UINT	currentBandwidth;
	EL_UINT foregroundBandwidth;
	EL_UINT backgroundBandwidth;

	EL_UINT webSeedTriggerSeconds;
	EL_UINT webSeedTriggerBandwidth;

	Bandwidths bandwidths;

	//XTrap
	std::string xTrapPatcherArg;
};
