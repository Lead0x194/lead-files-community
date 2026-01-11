/*
 *    Filename: signal.c
 * Description: 시그널 관련 함수.
 *
 *      Author: 비엽 (Server), myevan (Client)
 */

#include <signal.h>
#include <sys/wait.h>

#include "structs.h"
#include "socket.h"
#include "utils.h"
#include "log.h"

#define RETSIGTYPE void
#define my_signal signal

extern int shutdowned;
extern int tics;

pid_t waitpid(pid_t pid, int *status, int options);


RETSIGTYPE reap(int sig)
{
	while (waitpid(-1, NULL, WNOHANG) > 0);
	my_signal(SIGCHLD, reap);
}


RETSIGTYPE checkpointing(int sig)
{
	if (!tics) 
	{
		sys_log("SYSERR: CHECKPOINT shutdown: tics did not updated.");
		abort();
	} else
		tics = 0;
}


RETSIGTYPE hupsig(int sig)
{
	shutdowned = TRUE;

	sys_log("SYSERR: SIGHUP, SIGINT, SIGTERM signal has been received. shutting down.");
}

void signal_setup(void)
{
	struct itimerval	itime;
	struct timeval	interval;

	interval.tv_sec	= 60;
	interval.tv_usec	= 0;

	itime.it_interval	= interval;
	itime.it_value	= interval;

	setitimer(ITIMER_VIRTUAL, &itime, NULL);
	my_signal(SIGVTALRM, checkpointing);

	/* just to be on the safe side: */
	my_signal(SIGHUP,	hupsig);
	my_signal(SIGCHLD,	reap);
	my_signal(SIGINT,	hupsig);
	my_signal(SIGTERM,	hupsig);
	my_signal(SIGPIPE,	SIG_IGN);
	my_signal(SIGALRM,	SIG_IGN);
}
