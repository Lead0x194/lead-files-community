// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#ifndef TORRENTPATCH_METIN2TORRENTCONSOLE_H_
#define TORRENTPATCH_METIN2TORRENTCONSOLE_H_

//#ifndef _WIN32_WINNT		// Windows XP 이상에서만 기능을 사용할 수 있습니다.                   
//#define _WIN32_WINNT 0x0501	// 다른 버전의 Windows에 맞도록 적합한 값으로 변경해 주십시오.

// WIN98 지원
#define WINVER         0x0410
#define _WIN32_WINNT   0x0403 // Windows 2000
#define _WIN32_WINDOWS 0x0410 // WIN98
#define _WIN32_IE      0x0401 // 인터넷 익스플로러 4.1
//#endif						

// Windows 헤더 파일:
#include <winsock2.h>
#include <windows.h>

#include <wx-2.8.10/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx-2.8.10/wx.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

#include <YmirBase/YmirBase.h>
#include <YmirBase/Primitives.h>
#include <YmirBase/Log.h>

#include <Earth/Earth.h>
#include <Earth/EA_CStatus.h>
#include <Mantle/MA_StringCharSet.h>
#include <Mantle/MA_StringNumeric.h>
#include <Mantle/MA_STLString.h>

#include <TinyXML-2.6.1/TinyXML.h>

//#include <boost/filesystem/path.hpp>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>

#ifdef _DEBUG
#define TORRENT_DEBUG
#endif

#define _SFX_ENABLE

#define APP_VERSION_NAME			_T("v1.8.2")
#define APP_NAME					"YmirPatcher"
#define TAPP_NAME					_T("YmirPatcher")
#define TAPP_FULLNAME				_T("YmirPatcher") ## APP_VERSION_NAME
#define DIR_LOG						_T("temp")

#define FILENAME_PATCH_VER			_T("patch.ver")
#define FILENAME_PATCH_URL_XML		_T("TorrentPatch.url.xml")
#define FILENAME_APP_ICON			_T("TorrentPatch.ico")

#ifndef _DEBUG
#define UPDATE_PATCHER_SELF
#define USE_FAST_SELFUPDATE
#endif

#include "CLocaleManager.h"

#include "PatchInfo.h"
#include "PatchConfig.h"
#include "PatchStatus.h"

#endif  // TORRENTPATCH_METIN2TORRENTCONSOLE_H_
