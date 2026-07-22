#ifndef __INC_LIBTHECORE_STDAFX_H__
#define __INC_LIBTHECORE_STDAFX_H__

#if defined(__GNUC__)
#define INLINE __inline__
#elif defined(_MSC_VER)
#define INLINE inline
#endif

#ifdef __WIN32__
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <conio.h>
#include <process.h>
#include <limits.h>
#include <math.h>
#include <locale.h>
#include <io.h>
#include <direct.h>
#include <fcntl.h>

#include "xdirent.h"
#include "xgetopt.h"

#define S_ISDIR(m)      (m & _S_IFDIR)

#define __USE_SELECT__

#define PATH_MAX _MAX_PATH

// C runtime library adjustments
#define strlcat(dst, src, size) strcat_s(dst, size, src)
#define strlcpy(dst, src, size) strncpy_s(dst, size, src, _TRUNCATE)
#define strtoull(str, endptr, base) _strtoui64(str, endptr, base)
#define strcasecmp(s1, s2) _stricmp(s1, s2)
#define strncasecmp(s1, s2, n) _strnicmp(s1, s2, n)
#define atoll(str) _atoi64(str)
#define localtime_r(timet, result) localtime_s(result, timet)
#define gmtime_r(timet, result) gmtime_s(result, timet)
#define strtok_r(s, delim, ptrptr) strtok_s(s, delim, ptrptr)
#define strdup(str) _strdup(str)

// Portable file-open wrappers: route the standard (POSIX) signatures to the
// bounded MSVC *_s variants on Windows. Defined after <stdio.h> so the system
// prototypes are already parsed; POSIX builds use the native functions.
static INLINE FILE * __thecore_fopen(const char * filename, const char * mode)
{
	FILE * fp = NULL;
	return (fopen_s(&fp, filename, mode) == 0) ? fp : NULL;
}
#define fopen(filename, mode) __thecore_fopen((filename), (mode))

static INLINE FILE * __thecore_freopen(const char * filename, const char * mode, FILE * stream)
{
	FILE * fp = NULL;
	return (freopen_s(&fp, filename, mode, stream) == 0) ? fp : NULL;
}
#define freopen(filename, mode, stream) __thecore_freopen((filename), (mode), (stream))

#include <boost/typeof/typeof.hpp>
#define typeof(t) BOOST_TYPEOF(t)

// dummy declaration of non-supported signals
#define SIGUSR1     30  /* user defined signal 1 */
#define SIGUSR2     31  /* user defined signal 2 */

inline void usleep(unsigned long usec) {
        ::Sleep(usec / 1000);
}
inline unsigned sleep(unsigned sec) {
        ::Sleep(sec * 1000);
        return 0;
}
inline double rint(double x)
{
        return ::floor(x+.5);
}


#else

#if !defined(__FreeBSD__) && !defined(__LINUX__)
#define __USE_SELECT__
#ifdef __CYGWIN__
#define _POSIX_SOURCE 1
#endif
#endif

#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <dirent.h>

#include <sys/stat.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>

#include <sys/signal.h>
#include <sys/wait.h>

#include <pthread.h>
#include <semaphore.h>

#ifdef __FreeBSD__
#include <sys/event.h>
#endif

#ifdef __LINUX__
#include <sys/epoll.h>
#endif

// Compatibility for code that calls MSVC CRT names directly. The Windows
// branch above maps the POSIX names onto the MSVC ones; here we map the MSVC
// names back onto their POSIX equivalents so either spelling builds anywhere.
#include <strings.h>
#define _stricmp   strcasecmp
#define _strnicmp  strncasecmp
#define _write     write
#define _close     close
typedef void * PVOID;

#endif

#ifndef false
#define false   0
#define true    (!false)
#endif

#ifndef FALSE
#define FALSE   false
#define TRUE    (!FALSE)
#endif

#include "typedef.h"
#include "heart.h"
#include "fdwatch.h"
#include "socket.h"
#include "kstbl.h"
#include "hangul.h"
#include "buffer.h"
#include "signal.h"
#include "log.h"
#include "main.h"
#include "utils.h"
#include "crypt.h"
#include "memcpy.h"

#endif // __INC_LIBTHECORE_STDAFX_H__
