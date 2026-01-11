/*
 *    Filename: event.c
 * Description: 이벤트 관련 (timed event)
 *
 *      Author: 김한주 (aka. 비엽, Cronan), 송영진 (aka. myevan, 빗자루)
 */
#include "structs.h"
#include "utils.h"
#include "priority_queue.h"
#include "log.h"


extern int global_pulse;	/* main.c */


static PRIORITY_QUEUE*	event_q = NULL;          /* 이벤트 queue (리스트) */

/* 이벤트 queue 를 초기화 한다 */
void event_init(void)
{/*{{{*/
    sys_log("Intializing Event Engine.");
    event_q = queue_init();
}
/*}}}*/

/* 이벤트를 생성하고 리턴한다 */
LPEVENT event_create_ex(EVENTFUNC(*func), void *event_obj, long when, const char *file, int line)
{/*{{{*/
    LPEVENT new_event;

    /* 반드시 다음 pulse 이상의 시간이 지난 후에 부르도록 한다. */
    if (when < 1) 
	when = 1;

    CREATE(new_event, EVENT, 1);

    strncpy(new_event->file, file, 63);
    new_event->line = line;
    new_event->func = func;
    new_event->info = event_obj;
    new_event->q_el = queue_enq(event_q, new_event, when + global_pulse);

    return (new_event);
}/*}}}*/

/* 시스템으로 부터 이벤트를 제거한다 */
void event_cancel(LPEVENT event)
{/*{{{*/
    if (!event) 
    {
	sys_err("event_cancel(): try to cancel NULL event");
	return;
    }

    queue_deq(event_q, event->q_el);

    free(event->info);
    free(event);
}/*}}}*/

void event_reset_time(LPEVENT event, long when)
{/*{{{*/
    event->q_el = queue_reque(event_q, event->q_el, when + global_pulse);
}
/*}}}*/

/* 이벤트를 실행할 시간에 도달한 이벤트들을 실행한다 */
void event_process(void)
{/*{{{*/
    EVENT	*the_event;
    long	new_time;
    char	last_file[128];
    int		last_line = -1, last_time = -1;

    // event_q 즉 이벤트 큐의 헤드의 시간보다 현재의 pulse 가 적으면 루프문이 
    // 돌지 않게 된다.
    while ((long) global_pulse >= queue_key(event_q)) 
    {
	new_time = queue_key(event_q);

	if (!(the_event = (LPEVENT) queue_head(event_q))) 
	{
	    sys_err("event_process(): trying to call NULL event");
	    return;
	}

	if (strcmp(the_event->file, last_file) || last_line != the_event->line || last_time != new_time)
	{
	    strcpy(last_file, the_event->file);
	    last_line = the_event->line;
	    last_time = new_time;
//	    sys_log("EVENT: %s %d %d", the_event->file, the_event->line, new_time);
	}

	/*
	 * 리턴 값은 새로운 시간이며 리턴 값이 0 보다 클 경우 이벤트를 다시 추가한다. 
	 * 리턴 값을 0 이상으로 할 경우 event 에 할당된 메모리 정보를 삭제하지 않도록
	 * 주의한다.
	 */
	if ((new_time = (the_event->func) (the_event)) > 0)
	    the_event->q_el = queue_enq(event_q, the_event, new_time + global_pulse);
	else
	{
	    free(the_event->info);
	    free(the_event);
	}
    }
}/*}}}*/

/* 이벤트가 남은 시간을 pulse 단위로 리턴해 준다 */
long event_time(LPEVENT event)
{/*{{{*/
    long when;

    when = queue_elmt_key(event->q_el);
    return (when - global_pulse);
}
/*}}}*/

/* 모든 이벤트를 제거한다 */
void event_destroy()
{/*{{{*/
    LPEVENT the_event;

    if (!event_q)
	return;

    while ((the_event = (LPEVENT) queue_head(event_q))) 
	free(the_event);

    queue_free(event_q);
    event_q = NULL;
}/*}}}*/
