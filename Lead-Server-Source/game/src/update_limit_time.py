#!/usr/local/bin/python
import time

desc = """#ifndef __LIMIT_TIME__
#define __LIMIT_TIME__

#define ENABLE_LIMIT_TIME
#define GLOBAL_LIMIT_TIME %dUL // %s
#define TIME_OVER_PONG_DOWN_RATE 	50000
#define TIME_OVER_LOGIN_DOWN_RATE 	10000
#endif
"""

limitTime = time.mktime(time.localtime()) + 3600 * 24 * 180 * 2

open("limit_time.h", "w").write(desc % (limitTime, time.asctime(time.localtime(limitTime))))
