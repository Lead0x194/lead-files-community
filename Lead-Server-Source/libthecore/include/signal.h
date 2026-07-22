#ifndef __INC_LIBTHECORE_SIGNAL_H__
#define __INC_LIBTHECORE_SIGNAL_H__

#ifdef __LINUX__
// This project header shadows the C library header on Linux. Pull in the next
// signal.h before declaring the libthecore signal helpers.
#include_next <signal.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif
    extern void signal_setup();
    extern void signal_timer_disable();
    extern void signal_timer_enable(int timeout_seconds);

#ifdef __cplusplus
};
#endif

#endif
