#include "Stdafx.h"

//#include "../Rainbow3DBaseLib/RPackageManager.h"
#include "../PSNetwork/PSDispatcher.h"
#include "../PSNetwork/PSSession.h"
#include "../PSServerBase/PSGuildHouseManager.h"
#include "../PSServerBase/PSMapInfoDataManager.h"
#include "../PSServerBase/PSLogOpr.h"
#include "../PSServerBase/PSUserManager.h"

#include "../PSServerBase/PSPacketExecuter.h"
#include "../PSServerBase/PSPvpEventManager.h"

#include "../PSServerBase/PSGuildHouse.h"
#include "../PSServerBase/PSGuildHouseManager.h"

#include "../PSServerBase/DB/Query/PSQueryBag.h"
#include "../PSServerBase/DB/Query/PSQueryCharacter.h"
#include "../PSServerBase/DB/Query/PSQueryMisc.h"

#include "../PSServerBase/PSWaitUserInfoManager.h"
#include "../PSServerBase/C9TextFilter.h"

#include "../PSServerBase/PSUser.h"

#ifdef _C9_USE_HANGMAE_RANKING_SYSTEM_
#include "../PSServerBase/NhnModule/Ranking/PSRankingManager.h"
#endif	// #ifdef _C9_USE_HANGMAE_RANKING_SYSTEM_

#ifdef _C9_USE_RANKING_SYSTEM
#include "../PSServerBase/C9RankingManager.h"
#endif


#include "../../../Common/C9MapInfoData/C9MapInfoData.h"
#include "../../../Common/C9Base/C9ClientVersion.h"

#include "../../../Common/C9Packet/C9PacketP2PConnection.h"

#ifdef _C9_USE_CONTENT_CONTROL_SYSTEM_
#include "../PSServerBase/PSGameContentManager.h"
#endif	// #ifndef _C9_USE_CONTENT_CONTROL_SYSTEM_

#include "../PSServerBase/PSAntiCrackingSystem.h"
#include "../PSServerBase/PSGameEventManager.h"
#include "../PSNetwork/Registry.h"
#include "../PSServerBase/PSBaseField.h"

#include "../PSServerBase/C9LogOption.h"

#include "GameServerApp.h"

#ifdef _C9_USE_SERVER_KEY_
#include "../C9ServerKeyGenerator/CheckServerKey.h"
#include "../../../Common/C9Base/GetMacAddress.h"
#endif

#include "../PSServerBase/PSTencentLogger.h"

const int cPSMaxNetworkThread = 8;

#include "../../../Common/C9Base/C9MiniDump.h"
extern C9MiniDump g_MiniDumper;

bool InitializeLogger(int nServerNumber)
{
	std::wstring baseLogDirPath = GetServerConfigManager()->GetServerLogDirectory();
	std::wstring logFileName = boost::str(boost::wformat(L"%s_%d") %"GameServer" %nServerNumber);
	if (false == LOG_MANAGER.Initalize(baseLogDirPath, L"GameServerLog\\", logFileName))
	{
		PSLogManager::WriteFileImm(NULL, L"LOG_MANAGER Initalize Failed. path: %s", baseLogDirPath.c_str());
		return false;
	}
	// 디폴트 설정
	LOG_MANAGER.SetOutputSwitch(LOT_FILE);
	LOG_MANAGER.SetOutputSwitch(LOT_CONSOLE);
	LOG_MANAGER.SetOutputSwitch(LOT_DEBUGWINDOW);
	LOG_MANAGER.SetLogLevel(8);


#ifdef _C9_USE_CHANGE_SERVICE_MODE_
	g_ServiceDataType = C9SDT_Dev;
	string strTemp = GetServerConfigManager()->GetServiceDataType();
	if( ! strTemp.empty() )
	{
		g_ServiceDataType = StringToServiceDataType( strTemp.c_str() );
		if( g_ServiceDataType == C9SDT_MAX )
			g_ServiceDataType = C9SDT_Dev;
	}

#endif
	return true;
}

bool connectDBManager()
{
	PSDBConnctionInfo connection_info;

	if (true == GetServerConfigManager()->GetWorldDBConnectionInfo(connection_info) )
	{
		PSDBConnection* db_world = new (std::nothrow) PSDBConnection(HALF_ASYNC_THREAD + 2);
		if(NULL == db_world)
			return false;

		if (0 == db_world->Connect(connection_info))
		{
			WRITE_LOG(LT_NOTICE, L"Connected to World DB for db thread");
			PS_DB->AddConnection(DT_WORLD, db_world);
		}
		else
		{
			return false;
		}
	}

#ifdef _C9_USE_RANKING_SYSTEM
	if (true == GetServerConfigManager()->GetRankDBConnectionInfo(connection_info) )
	{
		PSDBConnection* db_rank = new (std::nothrow) PSDBConnection(HALF_ASYNC_THREAD + 2);

		if(NULL == db_rank)
			return false;

		if (0 == db_rank->Connect(connection_info))
		{
			WRITE_LOG(LT_NOTICE, L"Connected to Rank DB");
			PS_DB->AddConnection(DT_RANK, db_rank);
		}
		else
		{
			return false;
		}
	}
#endif // _C9_USE_RANKING_SYSTEM

	return true;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// GameServerApp
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

GameServerApp::GameServerApp()
	: m_pGameServer(NULL)
	, m_pUserManager(NULL)
	, m_pNetworkServer(NULL)
	, m_pPSMapInfoDataManager(NULL)
	, m_pGuildHouseManager(NULL)
	, m_pChatFilter(NULL)
	, m_pNameFilter(NULL)
#ifdef _C9_USE_CHN_TLOG_
	, m_pLogOption(NULL)
#endif
	, m_bIsGameGuardAuth(false)
	, m_bDefendDoS(false) 
	, m_nDoSThresholdInSec(0)
	, m_eServerType(PSST_GAMESERVER)
#ifdef _C9_RELAY_SERVER_MODULE_
	, relay_(0)
#endif
//#ifdef _C9_SERVER_PATH_ENGINE_
	, pathEngineDLL_(0)
	, pathEngine_(0)
//#endif
{
	m_nServerNumber = 1;
}

GameServerApp::GameServerApp(int nServerNumber)
	: m_pGameServer(NULL)
	, m_pUserManager(NULL)
	, m_pNetworkServer(NULL)
	, m_pPSMapInfoDataManager(NULL)
	, m_pGuildHouseManager(NULL)
	, m_pChatFilter(NULL)
	, m_pNameFilter(NULL)
	, m_bIsGameGuardAuth(false)
	, m_bDefendDoS(false)
	, m_nDoSSamplingTimeInSec(0)
	, m_nDoSThresholdInSec(0)
	, m_eServerType(PSST_GAMESERVER)
#ifdef _C9_RELAY_SERVER_MODULE_
	, relay_(0)
#endif
//#ifdef _C9_SERVER_PATH_ENGINE_
	, pathEngineDLL_(0)
	, pathEngine_(0)
//#endif
{
	m_nServerNumber = nServerNumber;
}

//-----------------------------------------------------------------------------
GameServerApp::~GameServerApp(void)
{
	PSNetwork::DestroyServer();
}

#ifdef _C9_SERVER_LOCK_
#include "../../../External/Themida/include/ThemidaSDK.h"

__forceinline time_t convertTime(const char* compileTimestamp)
{
	const char* months[] = {
		"Jan",
		"Feb",
		"Mar",
		"Apr",
		"May",
		"Jun",
		"Jul",
		"Aug",
		"Sep",
		"Oct",
		"Nov",
		"Dec"
	};

	char dayName[4];	// Mon, Tue, ...
	char monthName[4];	// Jan, Feb, ...
	int date = 0, year = 0;
	int hour = 0, minute = 0, second = 0;
	sscanf_s(compileTimestamp, "%3s %3s %d %d:%d:%d %d",
		dayName, sizeof(dayName), monthName, sizeof(monthName), &date, &hour, &minute, &second, &year);

	int month = 0;
	for (int i = 0; i < 12; i++)
	{
		if (strcmp(monthName, months[i]) == 0)
		{
			month = i;
			break;
		}
	}

	struct tm t;
	t.tm_year = year - 1900;
	t.tm_mon = month;
	t.tm_mday = date;
	t.tm_hour = hour;
	t.tm_min = minute;
	t.tm_sec = second;
	t.tm_isdst = -1;
	t.tm_wday = 0;
	t.tm_yday = 0;

	// local -> time_t(UTC)
	return mktime(&t);
}

__forceinline bool checkTime()
{
#pragma message(__TIMESTAMP__)

	time_t now = time(NULL);	// UTC
	time_t compile = convertTime(__TIMESTAMP__); // UTC

#ifdef _DEBUG
	WRITE_LOG(LT_INFO, L"time check: now = %I64u, compile = %I64u", now, compile);
#endif

	const uint _expireTime = 60*60*24*30*5; // 5개월
	if (now - compile > _expireTime)
	{
		WRITE_LOG(LT_CRIT, L"server license expired!! gap = %u sec", now - compile);
		return false;
	}

	// 컴파일한 시간 이전이라도 서버가 안뜬다.
	if (compile >= now)
	{
		WRITE_LOG(LT_CRIT, L"server time is not valid!! gap = %u sec", now - compile);
		return false;
	}

	return true;
}
#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// base interface
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool GameServerApp::Initialize()
{
	wchar_t buffer[MAX_PATH];
	::_snwprintf_s(buffer, _TRUNCATE, L"GameServer_%d", m_nServerNumber);
	g_MiniDumper.SetDumpPath(L"C9ServerDump\\GameServer", buffer);

#ifndef _GAME_RELEASE_
	::_snwprintf_s(buffer, _TRUNCATE, L"C9 Game Server No.%d [VER : %d]", m_nServerNumber, _CLIENT_VERSION_);
#else
	::_snwprintf_s(buffer, _TRUNCATE, L"C9 Game Server No.%d", m_nServerNumber);
#endif
	::SetConsoleTitle(buffer);



	if (false == loadConfig(m_nServerNumber))	
	{
		PSLogManager::WriteFileImm(NULL, L"loadConfig Failed.");
		return false;
	}

	if (false == InitializeLogger(m_nServerNumber))
	{
		WRITE_LOG(LT_CRIT, L"InitializeLogger Failed.");
		return false;
	}

	// 이 서버의 로그 설정
	GetServerConfigManager()->SetServerLogConfig();

	WRITE_LOG(LT_CRIT, L"Start C9 Game Server [%S %S]", __DATE__, __TIME__);
	WRITE_LOG(LT_CRIT, L"Client Version[%d] ServiceType[%s]", _CLIENT_VERSION_, AnsiToUni2( ServiceDataTypeToString( g_ServiceDataType ) ) );
#ifdef LESS_DB_THREAD
	WRITE_LOG(LT_NOTICE, L"Less DB Thread");
#endif
	

#ifdef _C9_USE_SERVER_DAILY_RESET_

	WRITE_LOG(LT_CRIT, L"Daily Reset : TM = %d, TR = %d, RC = %d, SC = %d, NB = %d, PRB = %d, PCRC = %d", 
		GetServerConfigManager()->GetResetTimeHour(),
		GetServerConfigManager()->GetTiredRate(),
		GetServerConfigManager()->GetResurrectionCoin(),
		GetServerConfigManager()->GetSurvivalCoin(),
		GetServerConfigManager()->GetNormalBoost(),
		GetServerConfigManager()->GetPcRoomBoost(),
		GetServerConfigManager()->GetPcRoomResurrectionCoin());

#ifdef _C9_USE_SURVIVAL_FORTUNE_MODE_
	WRITE_LOG(LT_CRIT, L"Daily Reset (SurvivalFCoin) : %d", GetServerConfigManager()->GetSurvivalFCoin());
#endif

#endif

	if (false == connectDBManager())
	{
		WRITE_LOG(LT_CRIT, L"[main] SQL Server connect failure !");
		return false;
	}
	WRITE_LOG(LT_NOTICE, L"ConnectDBManager" );

	ASYNC_EXECUTER->Open(HALF_ASYNC_THREAD);



#ifdef _C9_USE_CONTENT_SWITCHING_FROM_FILE_
	loadContentSwitchesConfig();
#endif	// #ifdef _C9_USE_CONTENT_SWITCHING_FROM_FILE_

	loadAntiCrackingConfig();

	loadGameGuardConfig();

#ifndef _C9_USE_CHN_TLOG_
	if (m_bUseChatLog)
	{
		CHAT_LOGGER->open(m_nWorldNumber, m_nServerNumber);
	}
#endif

	if (m_bUseRanking)
	{
#ifdef _C9_USE_HANGMAE_RANKING_SYSTEM_
		if (false == RANKING_MANAGER.Open())
		{
			WRITE_LOG(LT_ERROR, L"Ranking manager initialize failure !");
		}

		if (true == RANKING_MANAGER.IsOpen())
		{
			if (false == RANKING_MANAGER.Connect(m_rankingServerAddress))
			{
				WRITE_LOG(LT_ERROR, L"Ranking manager connect failure !");
			}
		}
#endif	// #ifdef _C9_USE_HANGMAE_RANKING_SYSTEM_
	}

	//------------------------------------------------
	// 1. initialize managers``````````
	if (false == m_ServerDataManager.Init())
	{
		return false;
	}

//#ifdef _C9_SERVER_PATH_ENGINE_
	if (false == initializePathEngine())
	{
		return false;
	}
//#endif

	m_pChannelInfo = new (std::nothrow) PSChannelInfo;
	if (NULL == m_pChannelInfo)
	{
		WRITE_LOG(LT_CRIT, L"[main] ChannelInfo memory allocation failure !");
		return false;
	}

	m_pNetworkServer = PSNetwork::InitServer(m_bDefendDoS, m_nDoSThresholdInSec, m_nDoSSamplingTimeInSec );
	if (NULL == m_pNetworkServer)
	{
		WRITE_LOG(LT_CRIT, L"[main] Network Server initialize failure !");
		return false;
	}

	m_pNetworkServer->Create();
	WRITE_LOG(LT_NOTICE, L"InitNetworkServer" );		

#ifdef _C9_RELAY_SERVER_MODULE_
	relay_ = C9Relay::CreateServerInstance();
	C9Relay::Config::SetListenAddress(relay_, m_strIP.c_str(), (uint16)m_nPort);	// tcp의 리슨포트와 동일하게 가져간다.
	C9Relay::Config::SetUserCertifyCallback(relay_, GameServer::onRelayCertifyUser, NULL);
	if (false == C9Relay::InitServer(relay_))
	{
		WRITE_LOG(LT_CRIT, L"Relay server module init Failed.");
		return false;
	}
	C9Relay::Config::SetRecvCallback(relay_, GameServer::onRelayRecv, NULL);
#endif

	m_pUserManager		= new (std::nothrow) PSUserManager;
	if(NULL == m_pUserManager)
		return false;
	m_pWaitUserManager	= new (std::nothrow) PSWaitUserInfoManager;
	if(NULL == m_pWaitUserManager)
		return false;
	m_pGameServer		= new (std::nothrow) GameServer( m_pNetworkServer, m_nServerNumber );
	if(NULL == m_pGameServer)
		return false;
#ifdef _C9_RELAY_SERVER_MODULE_
	m_pGameServer->SetRelayServer(relay_);
#endif
//#ifdef _C9_SERVER_PATH_ENGINE_
	m_pGameServer->SetPathEngine(pathEngine_);
//#endif
	m_pGuildHouseManager= new (std::nothrow) PSGuildHouseManager;
	if(NULL == m_pGuildHouseManager)
		return false;


	//////////////////////////////////////////////////////////////////////////
	// 채팅, 이름 필터 생성

	m_pChatFilter = new (std::nothrow) C9ChatFilter;
	if (NULL == m_pChatFilter)
	{
		return false;
	}

	m_pNameFilter = new (std::nothrow) C9NameFilter;
	if (NULL == m_pNameFilter)
	{
		return false;
	}

#ifdef _C9_USE_CHN_TLOG_

	m_pLogOption = new (std::nothrow) C9LogOption;
	if (NULL == m_pLogOption)
	{
		return false;
	}

	m_pLogOption->Load();

#endif
	
	
	g_pGameServer->SetLobbyServer(m_eServerType == PSST_LOBBY);

	// todo: 더미다 코드는 추가했지만, 실제 더미다 패커 적용은 나중에 하자.
#ifdef _C9_SERVER_LOCK_
	VM_START

	// time lock - 컴파일한 후로 6개월(?)이 지나면 서버가 안뜬다.
	if (false == checkTime())
	{
		return false;
	}

	VM_END
#endif

	if (true == m_strIP.empty())
	{
		m_pNetworkServer->Accept(NULL, m_nPort);
	}
	else
	{
		m_pNetworkServer->Accept(m_strIP.c_str(), m_nPort);
	}

#ifdef _C9_USE_SERVER_KEY_
	{
		std::string ip;
		uint16 port = 0;
		if (false == m_pNetworkServer->GetBindAddress(ip, port))
		{
			WRITE_LOG(LT_CRIT, L"ServerKey: cannot read bind address !!");
			return false;
		}

		std::string mac = GetMacAddressWithIpAddress(ip.c_str());
		std::string errorString;

		bool serverKeyCheckOK = false;
		for (size_t i = 0; i < serverKeys_.size(); ++i)
		{
			if (CheckServerKey(serverKeys_[i].c_str(), ip.c_str(), mac.c_str(), errorString))
			{
				WRITE_LOG(LT_NOTICE, L"ServerKey: ip/mac found in server key %u.", i+1);

				serverKeyCheckOK = true;
				break;
			}
			else
			{
				WRITE_LOG(LT_NOTICE, L"ServerKey: ip/mac not found (%u)", i);
			}
		}

		if (false == serverKeyCheckOK)
		{
			WRITE_LOG(LT_CRIT, L"ServerKey: check failed - [error:%S]", errorString.c_str());
			return false;
		}
	}
#endif

	m_pNetworkServer->StartThread( cPSMaxNetworkThread );

	// 데이터파일을 읽는다.
	if ( false == m_ServerDataManager.LoadGameData(m_pNetworkServer->CreateSession()) )
	{
		WRITE_LOG(LT_CRIT, L"OnGameDataInitialize: game data loading failed !!");
		return false;
	}

	// 서버 NPC 스크립트 파일 로드
	m_pGameServer->LoadQuestScript();

	// map data load
	if ( false == LoadMapData() )
	{
		WRITE_LOG(LT_CRIT, L"LoadMapData: map data loading failed !!");
		return false;
	}

	// 특정 클래스 사용하지 못하게 설정(월드 설정에 ClassDisable 옵션)
	DefaultWorldParam defaultWorldParam = GetServerConfigManager()->GetDefaultWorldParam();
	for(int i=0; i<defaultWorldParam.mClassDisableCnt; ++i)
	{
		C9DataInfoClass * pclass = g_pDataMgr->GetEditableDataInfo_Class( defaultWorldParam.mClassDisable[i] );
		if( pclass )
			pclass->SetServiceUseable(false);
	}

#if defined(_C9_SERVER_ACTION_CHECK_) || defined(_C9_SERVER_AI_CHECK_)
	// PC 클래스별 액션 파일 로드 (로비나 길드하우스는 로드할 필요가 없음)
	if (false == m_pGameServer->IsLobbyServer() && m_eServerType != PSST_GUILDHOUSE)
	{
		if (false == m_pGameServer->LoadActionData())
		{
			WRITE_LOG(LT_CRIT, L"LoadActionData: action data loading failed !!");
			return false;
		}
	}
#endif

	// 로그인 중에 유저의 패킷 Invoke 무시함
	DefaultDispatcher->SetInvokeTestFunction(InvokeTestFunc);

	WRITE_LOG(LT_NOTICE, L"CreateServer" );	
	WRITE_LOG(LT_NOTICE, L"[main] C9 Game Server initialized successfully.");
	WRITE_LOG(LT_INFO, L"[main] ListenPort = %d", m_nPort );

	RegisterDispatch();

	return true;
}

//-----------------------------------------------------------------------------
void	GameServerApp::Run()
{
	g_pGameServer->Run(m_nServerNumber, m_nMaxUser, m_bIsGameGuardAuth);	
}

//-----------------------------------------------------------------------------
void GameServerApp::Shutdown()
{
	WRITE_LOG(LT_INFO, L"[main] Uninitializing C9 Game Server.");

#ifndef _C9_USE_CHN_TLOG_
	if (m_bUseChatLog)
	{
		CHAT_LOGGER->close();
	}
#endif

	if (m_bUseRanking)
	{
#ifdef _C9_USE_HANGMAE_RANKING_SYSTEM_
		RANKING_MANAGER.Disconnect();
		RANKING_MANAGER.Close();
#endif	// #ifdef _C9_USE_HANGMAE_RANKING_SYSTEM_
	}

	RSAFE_DELETE( m_pChannelInfo );
	RSAFE_DELETE( m_pUserManager );
	RSAFE_DELETE( m_pGameServer );
	RSAFE_DELETE( m_pGuildHouseManager );
	RSAFE_DELETE( m_pChatFilter );
	RSAFE_DELETE( m_pNameFilter );
	RSAFE_DELETE( m_pPSMapInfoDataManager );

	ASYNC_EXECUTER->Close();

	PS_DB->CloseAllConnection();

	WRITE_LOG(LT_INFO, L"[main] C9 Game Server uninitialized successfully.");

	LOG_MANAGER.Finalize();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ServerBase
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void GameServerApp::OnConnect(PSISessionPtr pSession, int ErrorCode)
{
	if (NULL == g_pGameServer)
	{
		return;
	}

	// 월드서버
	if( g_pNetFacadeGS->GetWorldSession() == pSession)
	{
		if(ErrorCode != 0)
		{
			WRITE_LOG(LT_ERROR, L"Connect Fail World Server");
			g_pNetFacadeGS->OnConnectWorldServer(PSISessionPtr());
		}
		else
		{
			g_pNetFacadeGS->OnConnectWorldServer(pSession);
		}
	}

	// 경매장서버
	if(g_pNetFacadeGS->GetAuctionSession() == pSession)
	{
		if(ErrorCode != 0)
		{
			WRITE_LOG(LT_ERROR, L"Connect Fail Auction Server");
			g_pNetFacadeGS->OnConnectAuctionServer(PSISessionPtr());
		}
		else
		{
			g_pNetFacadeGS->OnConnectAuctionServer(pSession);
		}
	}

	// 커뮤니티서버
	if(g_pNetFacadeGS->GetCommunitySession() == pSession)
	{
		if(ErrorCode != 0)
		{
			WRITE_LOG(LT_ERROR, L"Connect Fail Community Server");
			g_pNetFacadeGS->OnConnectCommunityServer(PSISessionPtr());
		}
		else
		{
			g_pNetFacadeGS->OnConnectCommunityServer(pSession);
		}
	}

	// 데이터 캐시 서버
	if (g_pNetFacadeGS->GetDataCacheSession() == pSession)
	{
		if(ErrorCode != 0)
		{
			WRITE_LOG(LT_ERROR, L"Connect Fail Data Cache Server");
			g_pNetFacadeGS->OnConnectDataCacheServer(PSISessionPtr());
		}
		else
		{
			g_pNetFacadeGS->OnConnectDataCacheServer(pSession);
		}
	}

	// 로그서버
	if(g_pNetFacadeGS->GetLogServerSession() == pSession)
	{
		if(ErrorCode != 0)
		{
			WRITE_LOG(LT_ERROR, L"Connect Fail Log Server");
			g_pNetFacadeGS->OnConnectLogServer(PSISessionPtr());
		}
		else
		{
			g_pNetFacadeGS->OnConnectLogServer(pSession);
		}
	}

#ifdef _C9_USE_MONITOR_
	GetAgentMgr()->OnAyncConnect(pSession);
#endif
}


void GameServerApp::OnAccept( PSISessionPtr pSession )
{
	if( ( m_pUserManager == NULL ) )
	{
		WRITE_LOG(LT_ERROR, L"[OnAccept] m_pUserManager is NULL - %p", pSession);
		SendNak( pSession, 0, C9_ERROR_SERVER_FROM_DISCONNECT, true, 810000 );
		pSession->Disconnect();
		return;
	}

	if( ( g_pGameServer == NULL ) )
	{
		WRITE_LOG(LT_ERROR, L"[OnAccept] m_pWorld is NULL - %p", pSession);
		SendNak( pSession, 0, C9_ERROR_SERVER_FROM_DISCONNECT, true, 810001 );
		pSession->Disconnect();
		return;
	}

	if( ( g_pNetFacadeGS->GetWorldSession() == NULL ) )
	{
		WRITE_LOG(LT_ERROR, L"[OnAccept] WorldSession is NULL - %p", pSession);
		SendNak( pSession, 0, C9_ERROR_SERVER_FROM_DISCONNECT, true, 810003 );
		pSession->Disconnect();
		return;
	}
		
	if( g_pNetFacadeGS->GetChannelAllReady() == false )
	{
		WRITE_LOG(LT_ERROR, L"[OnAccept] channel allready is false - %p", pSession);
		SendNak( pSession, 0, C9_ERROR_SERVER_FROM_DISCONNECT, true, 810004 );
		pSession->Disconnect();
		return;
	}

	//동시접속자 체크
	ServerInfos si = GetServerConfigManager()->GetThisServerInfos();
	m_nMaxUser = si.maxUser;
	if ( m_nMaxUser + 200 < (int)GET_USER_COUNT() )
	{
		SendNak(pSession, PS_Certify_Req_TYPE, C9_ERROR_MAX_USER, true, 0);
		return;
	}

#ifdef _C9_NETWORK_STATISTICS_
	//pSession->SetEnableNetworkStatistics(true);
#endif	// #ifdef _C9_NETWORK_STATISTICS_

	//pSession->SetUserState( PS_US_LogIn );

	PS_InitCipher_Ack ack;
	ack.m_ConnectType = PS_CONNECT_GAMESERVER;

	PSStreamBufPtr buf(new PSStreamBuf());
	*buf << ack;

	pSession->SendStreamPtr( buf );

	if( g_pNetFacadeGS )
		g_pNetFacadeGS->OnAccept( pSession );

#ifdef _C9_PROTOCOL_CHANGE_BY_CLIENT_VERSION_
	// 게임 클라이언트의 세션이다.
	((PSSession *)pSession.get())->SetGameSession();
#endif
}

//-----------------------------------------------------------------------------
void GameServerApp::OnClosed( PSISessionPtr pSession )
{
	if(g_pNetFacadeGS)
		g_pNetFacadeGS->OnClosed(pSession);
}

// 이 콜백이 오면, 클라가 접속을 끊도록 NAK를 보낸다.
void GameServerApp::OnError(PSISessionPtr pSession, PSNetworkError errorCode)
{
	if (errorCode)
	{
		// logging
		PSUser* pUser = GET_USER(pSession->get_account());
		if (pUser)
		{
			WRITE_LOG(LT_ERROR, L"OnError: SendNAK(disconnect) to id[%s] - accNo[%I64u], PSNetworkError[%u]",
				pUser->GetAccountId(), pUser->GetAccountNumber(), errorCode);

			// 클라가 접속을 안 끊을 경우를 위해, 서버에서 수 초 후에 접속 끊도록 등록
			GetUserManager()->AddDisconnectUser(pUser->GetAccountNumber());
		}

		SendNak( pSession, 0, C9_ERROR_SERVER_FROM_DISCONNECT, true, errorCode );

		if (PSNET_DOS_DETECTED == errorCode)
		{
#ifdef _C9_USE_CHN_TLOG_
			_sendOfflineTlog(pUser, 7);	
#endif
		}
	}
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// managers
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//-----------------------------------------------------------------------------

bool GameServerApp::loadConfig(int nServerNumber)
{
	m_bUseChatLog	= false;
	m_bUseRanking	= false;
	m_nMaxUser		= 0;
	m_bDefendDoS	= false;
	m_nDoSThresholdInSec	= 0;

	ServerConfigManager * pSCM = GetServerConfigManager();
	if( false == pSCM->LoadServerConfig() )
	{
		PSLogManager::WriteFileImm(NULL, L"LoadServerConfig Failed.");
		return false;
	}

	// Game ==============================================================
	ServerInfos serverInfos = pSCM->GetServerInfos(GameServerType, (uint16)nServerNumber);
	// 서버의 정보가 없으므로 실패
	if( serverInfos.guid == 0 )
	{	
		PSLogManager::WriteFileImm(NULL, L"ServerConfig Loading Failed!!! ServerNo: %d\n", nServerNumber);
		return false;
	}

	pSCM->SetGUID(serverInfos.guid);
	m_nWorldNumber = pSCM->GetWorldNo(serverInfos.guid);

	struct in_addr a;
	a.s_addr		= pSCM->GetIP(serverInfos);
	m_strIP			= inet_ntoa(a);
	m_nPort			= serverInfos.tcpPort;
	m_bUseChatLog	= (1 == serverInfos.chatLog)?true:false;
	m_nMaxUser		= serverInfos.maxUser;
	m_bUseRanking	= (0 < serverInfos.ranking)?true:false;
	m_eServerType	= static_cast<PSSERVER_TYPE>(serverInfos.gameServerType);
	m_dwMapKey		= serverInfos.mapKey;
#ifdef _C9_USE_SERVER_KEY_
	serverKeys_		= pSCM->serverKeys_;
#endif
	if( 0 < serverInfos.defendDos )
	{
		if( 0 == pSCM->GetDefendDosSamplingTimeInSec() )
		{
			m_nDoSSamplingTimeInSec = DEFAULT_DOS_SAMPLING_TIME_IN_SEC;
		}
		else
		{
			m_nDoSSamplingTimeInSec = pSCM->GetDefendDosSamplingTimeInSec();
		}
		if( 0 == pSCM->GetDefendDosThresholdInSec() )
		{
			m_nDoSThresholdInSec = DEFAULT_DOS_THRESHOLD_IN_SEC;
		}
		else
		{
			m_nDoSThresholdInSec = pSCM->GetDefendDosThresholdInSec();
		}

		_tprintf(_T("DOS config ON - sampling period time in sec = %d, recv packet threshold in sec = %d\n"),
			m_nDoSSamplingTimeInSec, m_nDoSThresholdInSec);
	}
	else
	{
		_tprintf(_T("DOS config OFF\n"));
	}
	if( 0 == m_nMaxUser )
	{
		m_nMaxUser = 500;
	}
	if (true == m_bUseRanking)
		m_rankingServerAddress = pSCM->GetRankingServerIP();

	// worldServer ==================================================================
	ServerInfos worldServerInfos = pSCM->GetServerInfos(WorldServerType, (uint16)m_nWorldNumber);

	is_possible_create_new_character = ( worldServerInfos.newCharacter ) ? true : false;

	GetAgentMgr()->SetMonitoring(true);

	return true;
}

bool GameServerApp::loadAntiCrackingConfig()
{
	ServerInfos infos = GetServerConfigManager()->GetThisServerInfos();

	AntiCrackingSystem::SharedInstance().SetAutoBlockProperties(
		(0 < infos.autoBlock), 
		infos.threshold, 
		infos.pcTradeThreshold
	);
	return true;
}

bool GameServerApp::loadGameGuardConfig()
{
	// 월드 서버 기준으로 게임가드 설정
	ServerInfos infos = GetServerConfigManager()->GetServerInfos(WorldServerType, (uint16)m_nWorldNumber);
	m_bIsGameGuardAuth = false;
	if( infos.gameGuard )
	{
		m_bIsGameGuardAuth = true;
	}
	return true;
}

#ifdef _C9_USE_CONTENT_SWITCHING_FROM_FILE_
bool GameServerApp::loadContentSwitchesConfig()
{
	ContentsConfig contentsConfig = GetServerConfigManager()->GetContentsConfig();
	
	GAME_CONTENT_MANAGER.SetContentOnOff(GCCT_USE_IN_GAME_SHOP, contentsConfig.UseInGameShop_);
	GAME_CONTENT_MANAGER.SetContentOnOff(GCCT_USE_NPC_SHOP, contentsConfig.UseNpcShop_);
	GAME_CONTENT_MANAGER.SetContentOnOff(GCCT_USE_PERSONAL_STORE, contentsConfig.UsePersonalStore_);
	GAME_CONTENT_MANAGER.SetContentOnOff(GCCT_USE_PC_TRADE, contentsConfig.UsePcTrade_);
	GAME_CONTENT_MANAGER.SetContentOnOff(GCCT_USE_MAIL, contentsConfig.UseMail_);
	GAME_CONTENT_MANAGER.SetContentOnOff(GCCT_USE_PC_WAREHOUSE, contentsConfig.UsePcWarehouse_);
	GAME_CONTENT_MANAGER.SetContentOnOff(GCCT_USE_GUILD_WAREHOUSE, contentsConfig.UseGuildWarehouse_);
	GAME_CONTENT_MANAGER.SetContentOnOff(GCCT_USE_SPECIAL_ITEM, contentsConfig.UseSpecialItem_);
	GAME_CONTENT_MANAGER.SetContentOnOff(GCCT_USE_ITEM_REPAIR, contentsConfig.UseItemRepair_);
	GAME_CONTENT_MANAGER.SetContentOnOff(GCCT_USE_ITEM_ENCHANT, contentsConfig.UseItemEnchant_);
	GAME_CONTENT_MANAGER.SetContentOnOff(GCCT_USE_ITEM_PICKUP, contentsConfig.UseItemPickup_);
	GAME_CONTENT_MANAGER.SetContentOnOff(GCCT_USE_AUTO_BLOCK, contentsConfig.UseAutoBlock_);
	
	return true;
}
#endif	// #ifdef _C9_USE_CONTENT_SWITCHING_FROM_FILE_

void GameServerApp::SendNak(PSISessionPtr pSession, uint16 wPacketId, ulong dwError, bool bDisconnect, ulong dwParam1)
{
	if (NULL != pSession)
	{
		PS_Packet_Nak Nak;
		Nak.m_bDisconnect = bDisconnect;
		Nak.m_ErrorCode = dwError;
		Nak.m_PacketNumber = wPacketId;
		Nak.m_dwParam1 = dwParam1;

		pSession->Send(Nak);

		if (bDisconnect)
		{
			// pUser가 NULL이 아니어야 유저의 session (NULL이면 다른 서버와의 session)
			PSUser* pUser = GET_USER(pSession->get_account());
			if (pUser)
			{
				WRITE_LOG(LT_ERROR, L"GameServerApp: SendNAK(disconnect) to id[%s] accNo[%I64u] - packetid[%d] error[%u] param1[%u]",
					pUser->GetAccountId(), pUser->GetAccountNumber(), wPacketId, dwError, dwParam1);
			}
		}
	}
}

bool GameServerApp::RegisterDispatch()
{
	DefaultDispatcher->AddHandler( PS_InitCipher_Ack_TYPE, boost::bind( &GameServerApp::OnConnectServer, this, _1, _2 ) );	

	DefaultDispatcher->AddHandler( PS_Cancel_Waiting_Req_TYPE, boost::bind( &GameServerApp::OnCancelWaitingReq, this, _1, _2 ) );	

	DefaultDispatcher->AddHandler( PS_LoginData_Packet_Req_TYPE, boost::bind( &GameServerApp::OnLoginGameServer, this, _1, _2 ) );
	DefaultDispatcher->AddHandler( PS_MoveSelectCharacter_Packet_Req_TYPE, boost::bind( &GameServerApp::OnMoveSelectCharacter, this, _1, _2 ) );	

	DefaultDispatcher->AddHandler( PS_Character_List_Req_TYPE, boost::bind( &GameServerApp::OnRequestCharacterList, this, _1, _2 ) );	
	DefaultDispatcher->AddHandler( PS_Character_FeatureData_Req_TYPE, boost::bind( &GameServerApp::OnRequestCharacterFeatureData, this, _1, _2 ) );	

	DefaultDispatcher->AddHandler( PS_EventData_Ack_TYPE, boost::bind( &GameServerApp::OnEventData, this, _1, _2 ) );	
	DefaultDispatcher->AddHandler( PS_Add_BlockIp_Req_TYPE, boost::bind( &GameServerApp::OnRequestAddBlockIp, this, _1, _2 ) );	
	DefaultDispatcher->AddHandler( PS_Set_Account_Level_Req_TYPE, boost::bind( &GameServerApp::OnRequestSetAccountLevel, this, _1, _2 ) );

	DefaultDispatcher->AddHandler(PS_LoginAccount_Ack_TYPE, boost::bind(&GameServerApp::OnLoginAccountAck, this, _1, _2));
	DefaultDispatcher->AddHandler( PS_EndGame_Req_TYPE, boost::bind( &GameServerApp::OnEndGameReq, this, _1, _2 ) );	
	DefaultDispatcher->AddHandler( PS_Cancel_Waiting_Req_TYPE, boost::bind( &GameServerApp::OnCancelWaitingReq, this, _1, _2 ) );

	DefaultDispatcher->AddHandler( RequestSendProcessInfo_TYPE, boost::bind( &GameServerApp::OnReportPlayerInfo, this, _1, _2 ) );
	DefaultDispatcher->AddHandler( RequestSendPingCheck_TYPE, boost::bind( &GameServerApp::OnPingCheck, this, _1, _2 ) );
	DefaultDispatcher->AddHandler( PS_Change_Server_Config_Req_TYPE, boost::bind( &GameServerApp::OnChangeConfig, this, _1, _2 ) );
	DefaultDispatcher->AddHandler( PS_Player_Who_Req_TYPE, boost::bind( &GameServerApp::OnFindPlayerReq, this, _1, _2 ) );
	DefaultDispatcher->AddHandler( PS_Player_Who_Ack_TYPE, boost::bind( &GameServerApp::OnFindPlayerAck, this, _1, _2 ) );
	DefaultDispatcher->AddHandler( PS_Add_Filter_Text_Req_TYPE, boost::bind( &GameServerApp::OnAddFilterText, this, _1, _2 ) );

	DefaultDispatcher->AddHandler( PS_TopRankData_Ack_TYPE, boost::bind( &GameServerApp::OnTopRankData, this, _1, _2 ) );

	DefaultDispatcher->AddHandler( PS_Change_Nation_Code_TYPE, boost::bind( &GameServerApp::OnChangeNationCode, this, _1, _2 ) );

	GetServerConfigManager()->RegisterHandlers();

	return true;
}

//-----------------------------------------------------------------------------
bool GameServerApp::LoadMapData()
{

	m_pPSMapInfoDataManager = new (std::nothrow) PSMapInfoDataManager();
	if(NULL == m_pPSMapInfoDataManager)
	{
		WRITE_LOG(LT_CRIT, L"[main] m_pPSMapInfoDataManager memory allocation failure !");
		return false;
	}

	static bool bOneTime = false;
	assert(false == bOneTime);
	bOneTime = true;

	const HashMap_DataInfo & mapMapData = g_pDataMgr->GetDataInfoHashMapMap();
	for( HashMap_DataInfo::const_iterator pos = mapMapData.begin(); pos != mapMapData.end(); ++pos )
	{
		C9DataInfoMap * pInfo = (C9DataInfoMap*)pos->second;

		if ( NetFacadeGS::CheckEnterEnableMap( pInfo, m_eServerType, m_dwMapKey, true ) == false )
			continue;

		ulong dwMapID = pInfo->GetKey();	

		string strMapFileName = pInfo->m_strMapFileName.c_str();

		for( int cMapIndex=0; cMapIndex < (int)pInfo->m_lstMapLevelInfo.size(); cMapIndex++ )
		{			
			const C9DataInfoMap::MapLevelInfo &mapinfo = pInfo->m_lstMapLevelInfo[ cMapIndex ];

			if ( mapinfo.bUse == false )
				continue;

			string strMapFileNameLevel = strMapFileName + mapinfo.strFileNameExt;
			strMapFileNameLevel += ".C9MM";

			if ( MapDataManager->LoadBaseMapData( dwMapID, cMapIndex, strMapFileNameLevel.c_str() ) == false )
				return false;
		}

//#ifdef _C9_SERVER_PATH_ENGINE_
		g_pGameServer->GetNaviMesh(pInfo);
//#endif
	}	

	WRITE_LOG(LT_INFO, L"MapDataLoadComplete");	

	return true;
}

bool GameServerApp::OnConnectServer( PSStreamBufPtr pPacket, PSISessionPtr pSession )
{
	PS_InitCipher_Ack* pAck = (PS_InitCipher_Ack*)pPacket->Data();

	switch (pAck->m_ConnectType)
	{
	case PS_CONNECT_LOGSERVER:
		{
			PS_Register_Server_Req req;
			req.m_nServerType	= ST_GameServer;
			req.m_nWorldNumber	= m_nWorldNumber;
			req.m_nServerNumber = m_nServerNumber;

			pSession->Send(req);

			break;
		}
	};

	return true;
}

bool GameServerApp::OnLoginGameServer( PSStreamBufPtr pPacket, PSISessionPtr pSession )
{
	PS_LoginData_Packet_Req* pLoginDataReq = (PS_LoginData_Packet_Req*)pPacket->Data();	

	PSUser* pUser = GET_USER(pSession->get_account());
	if (NULL != pUser)
	{
		return false;
	}

	if (false == CheckLoginState(pSession, pLoginDataReq))
	{
		return false;	
	}

	boost::system::error_code ec;
	boost::asio::ip::tcp::endpoint endpoint = (static_cast<PSSession*>(pSession.get()))->GetSocket().remote_endpoint(ec);
	if (0 != ec.value())
	{
		SendNak(pSession, 0, C9_ERROR_LOGIN_FAILED, true, PSLE_FAILED_AUTH);
		return false;
	}

	const std::string& remoteAddress = endpoint.address().to_string();
	PSEndPoint epPrivate(pLoginDataReq->private_ip, pLoginDataReq->private_port);
	PSEndPoint epPublic(pLoginDataReq->public_ip, pLoginDataReq->public_port);


	// 유저 객체 생성
	pUser = new PSUser();
	if (NULL == pUser)
	{
		SendNak(pSession, 0, C9_ERROR_LOGIN_FAILED, true, PSLE_FAILED_AUTH);
		return false;
	}


	pUser->SetIpAddress(remoteAddress.c_str());
	pUser->SetIpAddressInt(static_cast< int >(endpoint.address().to_v4().to_ulong()));
	pUser->SetMaxCreatePcSlotSize( pLoginDataReq->m_nMaxCreatePcSlotCnt );
	pUser->SetPeerInfo(epPrivate, epPublic, pLoginDataReq->state);
	pUser->SetCertifiedKey(0);
	pUser->SetAccountNumber(pLoginDataReq->m_AccountNo);
	pUser->SetCertifiedKey(pLoginDataReq->m_ddwCertifiedKey);
	pUser->SetClientVersion(pLoginDataReq->m_uiClientVersion);


	if (false == INSERT_USER(pUser, pSession))
	{
		SendNak(pSession, 0, C9_ERROR_LOGIN_FAILED, true, PSLE_FAILED_AUTH);
		return false;
	}



	PS_LoginAccount_Req req;
	req.m_AccNo			= pLoginDataReq->m_AccountNo;
	req.m_CertifiedKey	= pLoginDataReq->m_ddwCertifiedKey;
	req.m_dwIP			= pUser->GetIpAddressInt();
	req.m_nWorldNumber	= g_pNetFacadeGS->GetWorldNumber();
	req.m_nWorldType	= g_pGameServer->GetEventWorld();
	req.m_ServerNumber	= g_pNetFacadeGS->GetServerNumber();

#ifdef _C9_USE_WAITINGLINE_
	//////////////////////////////////////////////////////////////////////////
	// 대기표 (로비서버이고, 채널이동이 아닐경우만....)

	if (PSST_LOBBY == g_pNetFacadeGS->GetServerType() && NULL == g_pNetFacadeGS->GetMoveMapPCData(pLoginDataReq->m_AccountNo))
	{
		if (false == PSWaitUserInfoManager::Pointer()->Insert(req))
		{
			SendNak(pSession, 0, C9_ERROR_LOGIN_FAILED, true, PSLE_FAILED_MAX_WAITING);
		}
	}
	else
	{
		// 대기표 필요 없음
		g_pNetFacadeGS->SendToWorldServer(req);
	}
#else
	g_pNetFacadeGS->SendToWorldServer(req);
#endif

	return true;
}


bool GameServerApp::OnMoveSelectCharacter(  PSStreamBufPtr pPacket, PSISessionPtr pSession )
{
	if (NULL == g_pGameServer)
	{
		return false;
	}

	PSUser* pUser = GET_USER(pSession->get_account());
	if (NULL == pUser)
	{
		return false;
	}

	PSPlayer* pPlayer = pUser->GetPlayer();
	if (NULL == pPlayer)
	{
		return false;
	}
	PSBaseField * pField = pPlayer->GetField();
	if(NULL == pField)
		return false;
	if( false == pField->GetDataInfoMap()->IsMapTypeNormalVillage() )
		return false;

	PSChannelNodeInfoMap& mapChannelNodeInfo = PSChannelInfo::Pointer()->GetChannelNodeInfoMap();

	for (PSChannelNodeInfoMap::iterator i = mapChannelNodeInfo.begin(); mapChannelNodeInfo.end() != i; ++i)
	{
		if (PSST_LOBBY == i->second.m_eServerType)
		{
			PSPacketExecuter::OnMoveMap_Channel( pPlayer, i->second.m_dwServerNumber, false, NULL);

			break;
		}
	}

	return true;
}

bool GameServerApp::CheckLoginState(PSISessionPtr pSession, PS_LoginData_Packet_Req* pLoginDataReq)
{
	// 로그인 쪽에는 모두다 Nak을! 걍 리턴시 유저는 반응 없이 계속 대기.

	RASSERT(NULL != pSession, "GameServerApp::OnLogin에 pSession이 널임");	

	// 동접 검사..
	if (m_nMaxUser + 200 < (int)GET_USER_COUNT() )
	{
		SendNak(pSession, PS_Certify_Req_TYPE, C9_ERROR_MAX_USER, true, 0);
		return false;
	}

	// 클라 버젼..
	if (_CLIENT_VERSION_ != pLoginDataReq->m_dwClientVersion)
	{
		SendNak(pSession, PS_Certify_Req_TYPE, C9_ERROR_CLIENT_VERSION, true, 0);

		return false;
	}

	// 서비스 데이터 타입
	if ((ulong)g_ServiceDataType != pLoginDataReq->m_dwServiceDataType)
	{
		SendNak(pSession, PS_Certify_Req_TYPE, C9_ERROR_CLIENT_SERVICE_DATA_TYPE, true, 0);

		return false;
	}

	return true;

}

bool GameServerApp::InvokeTestFunc(PSISessionPtr pSession, UINT uPacketID)
{
	PSUser* pUser = GET_USER(pSession->get_account());
	if (NULL != pUser)
	{
		if (false == pUser->IsAcceptGameServerComplete() && 
			PS_KEEPALIVE_CHANNEL != uPacketID &&
			P2PConnection::RegisterStunResult_TYPE != uPacketID &&
			PS_Cancel_Waiting_Req_TYPE != uPacketID)	// 대기표 취소 패킷은 허용한다.
		{
			return false;
		}
	}

	return true;
}




bool GameServerApp::OnRequestCharacterList( PSStreamBufPtr pPacket, PSISessionPtr pSession )
{
	//PS_Character_List_Req* pReq = (PS_Character_List_Req*)pPacket->Data();

	PSUser* pUser = GET_USER(pSession->get_account());
	if (NULL == pUser)
	{
		return false;
	}

	PSQueryGetCharacterList* pQuery = new PSQueryGetCharacterList;
	pQuery->SetParameter(pUser->GetAccountNumber());
	
	PSAsyncDBWork* pWork = new PSAsyncDBWork(DT_WORLD, pQuery, pUser->GetAccountNumber());

	ASYNC_EXECUTER->Execute(pWork);

	return true;
}

bool GameServerApp::OnRequestCharacterFeatureData( PSStreamBufPtr pPacket, PSISessionPtr pSession )
{
	PS_Character_FeatureData_Req* pReq = (PS_Character_FeatureData_Req*)pPacket->Data();	

	PSUser* pUser = GET_USER(pSession->get_account());
	if (NULL == pUser)
	{
		return false;
	}

	PSQueryGetCharacterFeatureData* pQuery = new PSQueryGetCharacterFeatureData;
	pQuery->SetParameter(pUser->GetAccountNumber(), pReq->m_CharacterNumber);
	
	PSAsyncDBWork* pWork = new PSAsyncDBWork(DT_WORLD, pQuery, pUser->GetAccountNumber());

	ASYNC_EXECUTER->Execute(pWork);

	return true;
}

bool GameServerApp::OnLoginAccountAck(PSStreamBufPtr pPacket, PSISessionPtr pSession)
{
	PS_LoginAccount_Ack* pAck = (PS_LoginAccount_Ack*)pPacket->Data();

	PSUser* pUser = GET_USER(pAck->m_AccNo);
	if (NULL == pUser)
	{
		return false;
	}

	switch (pAck->m_Result)
	{
	case PS_LoginAccount_Ack::SUCCESS_LOGIN_LOBBY :
		{
			break;
		}
	case PS_LoginAccount_Ack::SUCCESS_MOVE_CHANNEL :
		{
			break;
		}
	case PS_LoginAccount_Ack::SUCCESS_MOVE_LOBBY :
		{
			break;
		}
	case PS_LoginAccount_Ack::SUCCESS_DISCONNED :
		{
			break;
		}
	case PS_LoginAccount_Ack::BLOCKED_IP :	// 차단된 IP 주소
		{
			pUser->SendNak(0, C9_ERROR_BLOCK_IP_ADDRESS, 1, 0, true);
			REMOVE_USER(pUser, true);
			return false;
		}
	case PS_LoginAccount_Ack::NO_TICKET :	// 해당 월드에 입장 티켓이 없음
		{
			pUser->SendNak(0, C9_ERROR_LOGIN_FAILED, true, PSLE_FAILED_AUTH);
			REMOVE_USER(pUser, true);
			return false;
		}
	default:
		{
			pUser->SendNak(0, C9_ERROR_LOGIN_FAILED, true, PSLE_FAILED_AUTH);
			REMOVE_USER(pUser, true);
			return false;
		}
	}

	pUser->SetAccountNumber(pAck->m_AccNo);
	pUser->SetCertifiedKey(pAck->m_CertifiedKey);
	pUser->SetAccountId(pAck->m_AccId);
	pUser->SetAuthLevel(pAck->m_AuthLevel);
	pUser->SetChatBlock(pAck->m_ChatBlockLeft);
	pUser->SetBoardBlock(pAck->m_BoardBlockLeft);
	pUser->SetPubNumber(pAck->m_PubNumber);

#ifdef _C9_USE_AUTO_USER_BLOCK_
	pUser->SetDetectedHackStep(pAck->m_DetectedHack);
#endif	// #ifdef _C9_USE_AUTO_USER_BLOCK_
	pUser->SetAbusingPoint(pAck->m_AbusingPoint);

	pUser->Set2ndPasswordInfo(
		pAck->m_2ndPasswordHash, 
		pAck->m_2ndPasswordQuestionId, 
		pAck->m_2ndPasswordAnswer, 
		pAck->m_2ndPasswordFailedCount, 
		pAck->m_2ndPasswordFailedDateTime, 
		pAck->m_2ndPasswordLockedDateTime
		);

	int eventValues[5] = { 0 };
	if( EVENT_MANAGER.HasGameEvent( eventValues[0], eventValues[1], eventValues[2], eventValues[3], eventValues[4],
		GET_JUMPING_CHARACTER, false ) )
	{
		pUser->SetJumpingCharacterCnt(pAck->m_JumpingCharacterCnt);
	}
	else
	{
		pUser->SetJumpingCharacterCnt(0);
	}

	// 계정 로그인 로그
	LoginAccountLogOpr(pUser);

	pUser->SetLoginAccountComplete(true);

	if (NULL == g_pGameServer)
	{
		pUser->SendNak(0, C9_ERROR_LOGIN_FAILED, true, PSLE_CHANNEL_NULL);
		WRITE_LOG(LT_WARN, L"Login Error - Channel is NULL");
		return 1;
	}

	WRITE_LOG(LT_NOTICE, L"( %s, %S, %I64d ) Accept", pUser->GetAccountId(), pUser->GetIpAddress(), pUser->GetPubNumber());

	if (false == g_pGameServer->InsertUser(pUser))
	{
		pUser->SendNak(0, C9_ERROR_LOGIN_FAILED, true, PSLE_LOGIN_GAME_SERVER_EXIST_IN_CHANNEL);
		WRITE_LOG(LT_WARN, L"Exist User in Channel(Route - LOGIN_GAME_SERVER)");

		REMOVE_USER(pUser, true);

		return 1;
	}

	pUser->SetAcceptGameServerComplete(true);

	if (AntiCrackingSystem::SharedInstance().Enabled())
	{
		if (!pUser->EnableAntiCrackingMode())
		{
			WRITE_LOG(LT_ERROR, L"PSUser::EnableAntiCrackingMode failed !");
			SendNak(pSession, 0, C9_ERROR_LOGIN_FAILED, true, PSLE_INIT_GAMEGUARD);	
			REMOVE_USER(pUser, true);
			return false;
		}
	}

	if (m_eServerType == PSST_LOBBY)
	{
#ifdef _C9_USE_CHN_ASSET_SECURITY_SYSTEM_
		PS_MoveMap_Server_PC_Channel_Req* pMoveChannelReq = g_pNetFacadeGS->GetMoveMapPCData(pAck->m_AccNo);
		if (NULL != pMoveChannelReq)
		{
			pUser->SetCertified2ndPassword(pMoveChannelReq->m_bCertified2ndPassword);
			pUser->SetSafeMode(pMoveChannelReq->m_bIsSafeMode);
		}
#endif	// #ifdef _C9_USE_CHN_ASSET_SECURITY_SYSTEM_

		g_pNetFacadeGS->RemoveMoveMapPCData(pAck->m_AccNo);	


		//////////////////////////////////////////////////////////////////////////
		// 인증 성공 했으니.. 캐릭터 리스트를 보내주자.

		PSQueryGetCharacterList* pQuery = new PSQueryGetCharacterList;
		pQuery->SetParameter(pUser->GetAccountNumber());

		PSAsyncDBWork* pWork = new PSAsyncDBWork(DT_WORLD, pQuery, pUser->GetAccountNumber());

		ASYNC_EXECUTER->Execute(pWork);

		
	}
	else
	{
		if (false == pAck->m_bMoveChannel)
		{
			return false;
		}

		PS_MoveMap_Server_PC_Channel_Req* pMoveChannelReq = g_pNetFacadeGS->GetMoveMapPCData(pAck->m_AccNo);
		if (NULL == pMoveChannelReq)
		{
			WRITE_LOG(LT_NOTICE, L"Not exist PS_MoveMap_Server_PC_Channel_Req (AccountNo : %I64d) !", pAck->m_AccNo);
			return false;
		}

		pUser->SetFromServerNo(pMoveChannelReq->m_dwFromServerNumber);
		pUser->SetCharacterNumber(pMoveChannelReq->m_PlayerNo);
		pUser->OnMoveMapEnd(&pMoveChannelReq->m_MoveMapUserInfo);
		pUser->SetConnectInfo(*pMoveChannelReq);
		pUser->SetIsFirstLogin(pMoveChannelReq->m_bIsFirstLogin);

#ifdef _C9_USE_CHN_ASSET_SECURITY_SYSTEM_
		pUser->SetCertified2ndPassword(pMoveChannelReq->m_bCertified2ndPassword);
		pUser->SetSafeMode(pMoveChannelReq->m_bIsSafeMode);
#endif	// #ifdef _C9_USE_CHN_ASSET_SECURITY_SYSTEM_

#ifdef _C9_USE_CHN_AAS_
		pUser->SetPlayedHour(pMoveChannelReq->m_nPlayedHour);
#endif

#ifdef _C9_USE_INTERNET_CAFE_

		pUser->SetInternetCafeUser(pMoveChannelReq->m_bIsInternetCafeUser);

#ifdef _C9_HANGAME_DORIAN_
		pUser->SetCRMCode(pMoveChannelReq->m_szCRM);
#endif

#endif	// _C9_USE_INTERNET_CAFE_

		g_pNetFacadeGS->RemoveMoveMapPCData(pMoveChannelReq->m_AccountNo);

		PSQueryLoginMoveChannel* pQuery = new PSQueryLoginMoveChannel;

		pQuery->SetParameter(
			pUser->GetAccountNumber(),
			pUser->GetCharacterNumber(),
			pUser->GetIpAddressInt());

		PSAsyncDBWork* pWork = new PSAsyncDBWork(DT_WORLD, pQuery, pUser->GetAccountNumber());

		ASYNC_EXECUTER->Execute(pWork);
	}

	return true;
}

bool GameServerApp::OnEndGameReq(PSStreamBufPtr pPacket, PSISessionPtr pSession)
{
	PS_EndGame_Req* pReq = (PS_EndGame_Req*)pPacket->Data();

	PSUser* pUser = GET_USER(pSession->get_account());
	if (NULL == pUser)
	{
		return false;
	}

	PSPlayer* pPlayer = pUser->GetPlayer();
	if (NULL == pPlayer)
	{
		return false;
	}

	PS_EndGame_Ack ack;

	if (pUser->GetAccountNumber() != pReq->m_AccountNumber || pUser->GetCertifiedKey() != pReq->m_CertifiedKey)
	{
		ack.m_nResult = PS_EndGame_Ack::FAIL;
	}
	else
	{
		if (true == pPlayer->Logout(LOGOUT_ENDGAME))
		{
			REMOVE_USER(pUser, true);
			ack.m_nResult = PS_EndGame_Ack::SUCCESS;
		}
		else
		{
			ack.m_nResult = PS_EndGame_Ack::FAIL;
		}
	}

	if (pSession && pSession->IsConnected())
	{
		pSession->Send(ack);
	}

	return true;
}

bool GameServerApp::OnCancelWaitingReq(PSStreamBufPtr pPacket, PSISessionPtr pSession)
{
	PS_Cancel_Waiting_Req* pReq = (PS_Cancel_Waiting_Req*)pPacket->Data();

	PSUser* pUser = GET_USER(pSession->get_account());
	if (NULL == pUser)
	{
		return false;
	}

	if (pUser->GetAccountNumber() != pReq->account_number || pUser->GetCertifiedKey() != pReq->certifiedkey)
	{
		return false;
	}

	PSWaitUserInfoManager::Pointer()->Remove(pReq->account_number);

// 	PS_Move_Auth_Server_Req req;
// 	req.m_AccNo = pReq->account_number;
// 	req.m_CertifiedKey = pReq->certifiedkey;
// 
// 	g_pNetFacadeGS->SendToWorldServer(req);

	WRITE_LOG(LT_NOTICE, L"OnCanelWaitingReq: disconnect - id[%s] accNo[%I64u]", pUser->GetAccountId(), pUser->GetAccountNumber());

	REMOVE_USER(pUser, true);

	pSession->Disconnect();

	return true;
}

bool GameServerApp::OnEventData(PSStreamBufPtr pPacket, PSISessionPtr pSession)
{
	PS_EventData_Ack* pAck = (PS_EventData_Ack*)pPacket->Data();

	EVENT_MANAGER.SetEvent(pAck);

	m_pGameServer->OnEventData( pAck );

	return true;
}

bool GameServerApp::OnRequestAddBlockIp(PSStreamBufPtr pPacket, PSISessionPtr pSession)
{
	PS_Add_BlockIp_Req* pReq = (PS_Add_BlockIp_Req*)pPacket->Data();

	g_pNetFacadeGS->SendToWorldServer(*pReq);

	return true;
}

bool GameServerApp::OnRequestSetAccountLevel(PSStreamBufPtr pPacket, PSISessionPtr pSession)
{
	PS_Set_Account_Level_Req* pReq = (PS_Set_Account_Level_Req*)pPacket->Data();

	g_pNetFacadeGS->SendToWorldServer(*pReq);

	return true;
}

bool GameServerApp::OnReportPlayerInfo( PSStreamBufPtr pPacket, PSISessionPtr pSession )
{

#ifdef _C9_USE_CHN_TLOG_

	PSUser* pUser = GET_USER(pSession->get_account());
	if (NULL == pUser)
	{
		return false;
	}

	PSPlayer* pPlayer = pUser->GetPlayer();
	if (NULL == pPlayer)
	{
		return false;
	}

	PSBaseField* pField = pPlayer->GetField();
	if (NULL == pField)
	{
		return false;
	}

	RequestSendProcessInfo* pReq = (RequestSendProcessInfo*)pPacket->Data();

	USE_TLOG(L"WebChange")
	{
		PS_Log_Security_Network_Environment_Change log;
		
		log.iUin				= pPlayer->GetUser()->GetPubNumber();
		log.dtEventTime			= ::time(NULL);
		log.iRoleId				= pPlayer->GetCharacterNumber();
		strcpy_s(log.vRoleName, UniToAnsi2(pPlayer->GetName()));

		log.iPing				= pReq->m_nPing;
		log.iPacketLostRate		= 0;

		C9DataInfoMap *pMapInfo = (C9DataInfoMap *)pField->GetDataInfoMap();
		if (pMapInfo)
		{
			log.iMapId				= pMapInfo->m_dwKey;
		}
		else
		{
			log.iMapId				= 0;
		}

		log.iFPS				= pReq->m_nFPS;
		log.iScreenNum			= pReq->m_nSceneNum;
		log.iRenderingPCNum		= pReq->m_nRenderPCNum;
		log.iRenderingNPCNum	= pReq->m_nRenderNPCNum;
		log.iFullScreen			= pReq->m_nisFullScreen;
		log.iMemoryUseRate		= pReq->m_nUseMemory;
		log.iCPUUseRate			= pReq->m_nUseCpu;
		strcpy_s(log.vChannelIp, ConvertIPAddress(htonl(g_pNetFacadeGS->GetServerIP())).c_str());
		log.iClientResolutionX	= pReq->m_nScreenWidth;
		log.iClientResolutionY	= pReq->m_nScreenHeight;

		g_pNetFacadeGS->SendToLogServer(log);
	}

#endif

	return true;
}

bool GameServerApp::OnPingCheck( PSStreamBufPtr pPacket, PSISessionPtr pSession )
{
	RequestSendPingCheck* pReq = (RequestSendPingCheck*)pPacket->Data();

	PSUser* pUser = GET_USER(pSession->get_account());
	if (NULL == pUser)
	{
		return false;
	}

	__time64_t current_ = ::time(NULL);

	if (0 != pUser->GetLastPingTime())
	{
		int64 nGap = current_ - pUser->GetLastPingTime();

		if (nGap <=  5)
		{
			// 원래는 10초에 한번씩 보내준다. 5초 이내에 또 오면 무시하자. 봐서 튕기던지 하자.
			//WRITE_LOG(LT_WARN, L"Sending Ping too often gap : %d ( %s )", nGap, pUser->GetAccountId());
			return false;
		}
	}

	pUser->SetLastPingTime(current_);
	pUser->SetPingCount(pReq->m_nCount);	// 나중에 써먹을 수 있것지??
	pReq->m_timeServer = current_;			// 이것도?? ㅋㅋ
	pSession->Send(*pReq);

	return true;
}

bool GameServerApp::OnChangeConfig( PSStreamBufPtr pPacket, PSISessionPtr pSession )
{
	PS_Change_Server_Config_Req* pReq = (PS_Change_Server_Config_Req*)pPacket->Data();

	PSUser* pUser = NULL;

	if (pSession != g_pNetFacadeGS->GetWorldSession())
	{
		// 월드서버에서 보낸 패킷이 아니면.. 유저 권한 검사
		pUser = GET_USER(pSession->get_account());
		if (NULL == pUser)
		{
			return false;
		}

		if (pUser->GetAuthLevel() < EV_AHL_DUMMY)
		{
			WRITE_LOG(LT_WARN, L"UnAuthorized User Request (%s) AuthLevel : %d", pUser->GetAccountId(), pUser->GetAuthLevel());
			return false;
		}	
	}

	switch (pReq->m_eScope)
	{
	case PS_Change_Server_Config_Req::ALL:
	case PS_Change_Server_Config_Req::CONTINENT:
	case PS_Change_Server_Config_Req::RELOAD:
		{
			g_pNetFacadeGS->SendToWorldServer(*pReq);
			return true;
		}
	case PS_Change_Server_Config_Req::CHANNEL:
		{
			if (PS_Change_Server_Config_Req::LOG == pReq->m_eType)
			{
#ifdef _C9_USE_CHN_TLOG_

				if (pReq->m_szLogOption && 0 < wcslen(pReq->m_szLogOption))
				{
					WRITE_LOG(LT_INFO, L"Log Option : %s, Value : %d", pReq->m_szLogOption, pReq->m_nLogValue);
					C9LogOption::Pointer()->SetOption(pReq->m_szLogOption, (pReq->m_nLogValue == 1) ? true : false);
				}	
				else
				{
					C9LogOption::Pointer()->Reload();
				}

#endif
			}

			if (PS_Change_Server_Config_Req::FILTER == pReq->m_eType)
			{
				

				std::wstring str(pReq->m_szText);

				std::transform(str.begin(), str.end(), str.begin(), toupper);

				m_pChatFilter->AddSlang(str.c_str());
				m_pNameFilter->AddSlang(str.c_str());
			}

			break;
		}
	default:
		{
			WRITE_LOG(LT_ERROR, L"Undefined Request Scope!! ( %d )", pReq->m_eScope);
			return false;
		}
	}

	return true;
}

bool GameServerApp::OnFindPlayerReq( PSStreamBufPtr pPacket, PSISessionPtr pSession )
{
	PS_Player_Who_Req* pReq = (PS_Player_Who_Req*)pPacket->Data();

	PSUser* pUser = GET_USER(pSession->get_account());
	if (NULL == pUser)
	{
		return false;
	}

	PSPlayer* pPlayer = pUser->GetPlayer();
	if (NULL == pPlayer)
	{
		return false;
	}

	pReq->m_nRequestPlayer = pPlayer->GetCharacterNumber();

	g_pNetFacadeGS->SendToWorldServer(*pReq);

	return true;
}

bool GameServerApp::OnFindPlayerAck( PSStreamBufPtr pPacket, PSISessionPtr pSession )
{
	PS_Player_Who_Ack* pAck = (PS_Player_Who_Ack*)pPacket->Data();

	PSPlayer* pPlayer = g_pGameServer->GetPlayer(pAck->m_RequestPlayer);
	if (NULL == pPlayer)
	{
		return false;
	}

	PSUser* pUser = pPlayer->GetUser();
	if (NULL == pUser)
	{
		return false;
	}

	PSISessionPtr pCharacterSession = pUser->GetSession();
	if (pCharacterSession)
	{
		pCharacterSession->Send(*pAck);
	}

	return true;
}

bool GameServerApp::OnAddFilterText( PSStreamBufPtr pPacket, PSISessionPtr pSession )
{
	PS_Add_Filter_Text_Req* pReq = (PS_Add_Filter_Text_Req*)pPacket->Data();

	if (true == pReq->m_bReload)
	{
		C9ChatFilter::Pointer()->Reset();
		C9NameFilter::Pointer()->Reset();
	}

	WRITE_LOG(LT_INFO, L"Added Filter Text : %d", pReq->m_nCount);

	for (int i = 0; i < pReq->m_nCount; ++i)
	{
		switch (pReq->m_Data[i].m_nUseFlag)
		{
		case 1 : 
			{
				C9ChatFilter::Pointer()->AddSlang(pReq->m_Data[i].m_wszText);
				break;
			}
		case 2 :
			{
				C9NameFilter::Pointer()->AddSlang(pReq->m_Data[i].m_wszText);
				break;
			}
		case 3:
			{
				C9ChatFilter::Pointer()->AddSlang(pReq->m_Data[i].m_wszText);
				C9NameFilter::Pointer()->AddSlang(pReq->m_Data[i].m_wszText);
				break;
			}
		default:
			{
				break;
			}
		}
	}

	return true;
}

bool GameServerApp::OnTopRankData(PSStreamBufPtr pPacket, PSISessionPtr pSession)
{
	PS_TopRankData_Ack* pAck = (PS_TopRankData_Ack*)pPacket->Data();

	// todo:
#ifdef _C9_USE_RANKING_SYSTEM

	PVP_RANKING_MANAGER.SetRank(pAck);

#endif

	return true;
}

bool GameServerApp::OnChangeNationCode(PSStreamBufPtr pPacket, PSISessionPtr pSession)
{
	PS_Change_Nation_Code* pReq = (PS_Change_Nation_Code*)pPacket->Data();

	PSUser* pUser = GET_USER(pSession->get_account());
	if (NULL == pUser)
	{
		return false;
	}

	if (pReq->character_number != pUser->GetCharacterNumber())
	{
		return false;
	}

	PSQuerySetNationCode* pQuery = new PSQuerySetNationCode();
	pQuery->SetParameter(pUser->GetCharacterNumber(), pReq->code);

	PSIAsyncWork* pWork = new PSAsyncDBWork(DT_WORLD, pQuery);
	ASYNC_EXECUTER->Execute(pWork);

	return true;
}

//#ifdef _C9_SERVER_PATH_ENGINE_
typedef iPathEngine* (__stdcall* GetPathEngineInterfaceFunction)(iErrorHandler*);

class cErrorHandler : public iErrorHandler
{
	cErrorHandler(){}
public:
	eAction handle(const char* error_type, const char* error_string, const char *const* attributes)
	{
		UNUSED_ARG(attributes);

		WRITE_LOG(LT_NOTICE, L"PATH_ENGINE error: type[%S] string[%S]", error_type, error_string);
		return CONTINUE;
	}
	static cErrorHandler& refInstance()
	{
		static cErrorHandler the_instance;
		return the_instance;
	}
};

bool GameServerApp::initializePathEngine()
{
	pathEngineDLL_ = LoadLibraryA("./pathengine.dll");
	if (0 == pathEngineDLL_)
	{
		WRITE_LOG(LT_ERROR, L"initializePathEngine: LoadLibrary error");
		return false;
	}

	FARPROC	procaddress = GetProcAddress(pathEngineDLL_, (LPCSTR)1);
	if (0 == procaddress)
	{
		WRITE_LOG(LT_ERROR, L"initializePathEngine: GetProcAddress error");
		return false;
	}

	GetPathEngineInterfaceFunction pFunc_GetPathEngineInterfaceFunction = ( GetPathEngineInterfaceFunction ) procaddress;

	pathEngine_ = pFunc_GetPathEngineInterfaceFunction( &cErrorHandler::refInstance() );
	if (0 == pathEngine_)
	{
		WRITE_LOG(LT_ERROR, L"initializePathEngine: cannot get path engine interface");
		return false;
	}

	return true;
}
//#endif
