#pragma once

#include <YmirBase/YmirBaseLink.h>
//#include <YmirLocale/YmirLocaleLink.h>
#include <Earth/EA_LibLink.h>

#include <TinyXML-2.6.1/TinyXMLLibLink.h>
#include <zlib-1.2.3/zlibLibLink.h>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "rpcrt4.lib")

#ifndef NO_LIBTORRENT
#ifdef _DEBUG
	#pragma comment(lib, "libtorrent-0.15.5WMTd.lib")
#else
	#pragma comment(lib, "libtorrent-0.15.5WMT.lib")
#endif
#endif
