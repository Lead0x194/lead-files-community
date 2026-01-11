#pragma once

#ifdef _C9_RELAY_SERVER_MODULE_
#include "../C9RelayServerLib/C9RelayServerLib.h"
#endif
//#ifdef _C9_SERVER_PATH_ENGINE_
#pragma warning(push)
#pragma warning(disable:4100)
#include "../../../Client/Src/Rainbow3D/Rainbow3DScene/i_pathengine.h"
#pragma warning(pop)
//#endif

class RPackageManager;
class PSMapInfoDataManager;

class PSGuildHouseManager;
class PSUserManager;
class PSWaitUserInfoManager;
class C9ChatFilter;
class C9NameFilter;
class C9LogOption;
class PSServerDataManager;


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// GameServerApp
//		: please 정리 좀...
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class GameServerApp : public PSNetworkEventMgr
{
private:

	// managers
	GameServer*				m_pGameServer;
	PSUserManager*			m_pUserManager;
	PSWaitUserInfoManager*	m_pWaitUserManager;
	PSMapInfoDataManager*	m_pPSMapInfoDataManager;
	PSGuildHouseManager*	m_pGuildHouseManager;
	C9ChatFilter*			m_pChatFilter;
	C9NameFilter*			m_pNameFilter;
	C9LogOption	*			m_pLogOption;
	PSServerDataManager 	m_ServerDataManager;
	
	// 
	PSChannelInfo			*m_pChannelInfo;

	// working.. comment 부탁..
	PSIServer				*m_pNetworkServer;
	int						m_nWorldNumber;
	int						m_nServerNumber;
	std::string				m_strIP;
	int						m_nPort;
	bool					m_bUseChatLog;
	bool					m_bUseRanking;
	bool					m_bDefendDoS;				// DOS 측정 할 건지 여부
	int						m_nDoSSamplingTimeInSec;	// 몇 초 동안
	int						m_nDoSThresholdInSec;			// 몇 개 오면 접종할건지
	int						m_nMaxUser;
	bool					m_bIsGameGuardAuth;

	PSSERVER_TYPE			m_eServerType;
	ulong					m_dwMapKey;
#ifdef _C9_USE_SERVER_KEY_
	std::vector<std::string> serverKeys_;					// 서버 구동을 위한 키
#endif

	std::string				m_rankingServerAddress;
#ifdef _C9_RELAY_SERVER_MODULE_
	C9Relay::HSERVER		relay_;						// UDP relay 서버 모듈
#endif
//#ifdef _C9_SERVER_PATH_ENGINE_
	HMODULE					pathEngineDLL_;				// path engine DLL 핸들
	iPathEngine*			pathEngine_;				// iPathEngine 인터페이스
//#endif

public:
	static bool				ms_quit;

public:

	GameServerApp();
	explicit GameServerApp(int nServerNumber);
	virtual ~GameServerApp();

	///////////////////////////////////////////////////////
	// 1. base interface
	///////////////////////////////////////////////////////

	virtual bool		Initialize();

	virtual void		Run();

	virtual void		Shutdown();


	///////////////////////////////////////////////////////
	// 2. Network event
	///////////////////////////////////////////////////////

	virtual void		OnAccept( PSISessionPtr session );
	virtual void		OnClosed( PSISessionPtr session );

	virtual void		OnConnect(PSISessionPtr pSession, int ErrorCode);
	virtual void		OnError(PSISessionPtr pSession, PSNetworkError errorCode);


	///////////////////////////////////////////////////////
	// 3. manager facades
	///////////////////////////////////////////////////////

	PSUserManager*		GetUserManager();


private:

	////////////////////////////////////////////////////////////////////////
	// internal implements
	////////////////////////////////////////////////////////////////////////

	//void				registerDispatchers();
	bool				loadConfig(int nServerNumber);
	bool				loadAntiCrackingConfig();
	bool				loadGameGuardConfig();
#ifdef _C9_USE_CONTENT_SWITCHING_FROM_FILE_
	bool				loadContentSwitchesConfig();
#endif	// #ifdef _C9_USE_CONTENT_SWITCHING_FROM_FILE_

	void				SendNak(PSISessionPtr pSession, uint16 wPacketId, ulong dwError, bool bDisconnect, ulong dwParam1);
//#ifdef _C9_SERVER_PATH_ENGINE_
	bool				initializePathEngine();
//#endif


public:
	bool				RegisterDispatch();
	bool				LoadMapData();

	///////////////////////////////////////////////////////
	// packet handlers
	///////////////////////////////////////////////////////

	bool				OnConnectServer( PSStreamBufPtr pPacket, PSISessionPtr pSession );
	bool				OnLoginGameServer( PSStreamBufPtr pPacket, PSISessionPtr pSession );
	bool				OnRegisterChannel( PSStreamBufPtr pPacket, PSISessionPtr pSession );
	bool				OnWorldServerLogin( PSStreamBufPtr pPacket, PSISessionPtr pSession );
	bool				OnMoveMap_Channel_Req( PSStreamBufPtr pPacket, PSISessionPtr pSession );
	bool				OnMoveSelectCharacter(  PSStreamBufPtr pPacket, PSISessionPtr pSession  );
	bool				OnEventData(PSStreamBufPtr pPacket, PSISessionPtr pSession);
	bool				OnRequestAddBlockIp(PSStreamBufPtr pPacket, PSISessionPtr pSession);
	bool				OnRequestSetAccountLevel(PSStreamBufPtr pPacket, PSISessionPtr pSession);
	bool				OnLoginAccountAck(PSStreamBufPtr pPacket, PSISessionPtr pSession);
	bool				OnEndGameReq(PSStreamBufPtr pPacket, PSISessionPtr pSession);
	bool				OnCancelWaitingReq(PSStreamBufPtr pPacket, PSISessionPtr pSession);

	bool				CheckLoginState(PSISessionPtr pSession, PS_LoginData_Packet_Req* pLoginDataReq);

	bool				OnRequestCharacterList( PSStreamBufPtr pPacket, PSISessionPtr pSession );
	bool				OnRequestCharacterFeatureData( PSStreamBufPtr pPacket, PSISessionPtr pSession );

	bool				OnReportPlayerInfo( PSStreamBufPtr pPacket, PSISessionPtr pSession );
	bool				OnPingCheck( PSStreamBufPtr pPacket, PSISessionPtr pSession );
	bool				OnChangeConfig( PSStreamBufPtr pPacket, PSISessionPtr pSession );
	bool				OnFindPlayerReq( PSStreamBufPtr pPacket, PSISessionPtr pSession );
	bool				OnFindPlayerAck( PSStreamBufPtr pPacket, PSISessionPtr pSession );
	bool				OnAddFilterText( PSStreamBufPtr pPacket, PSISessionPtr pSession );

	static bool			InvokeTestFunc(PSISessionPtr pSession, UINT uPacketID);

	bool				OnTopRankData(PSStreamBufPtr pPacket, PSISessionPtr pSession);

	bool				OnChangeNationCode(PSStreamBufPtr pPacket, PSISessionPtr pSession);
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// inline function
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


inline PSUserManager*	GameServerApp::GetUserManager()
{
	return m_pUserManager;
}

//-----------------------------------------------------------------------------

#define DEFAULT_DOS_SAMPLING_TIME_IN_SEC	10		// 10초 단위로 sampling
#define DEFAULT_DOS_THRESHOLD_IN_SEC		100		// 1초에 100개 이상 오면 접종