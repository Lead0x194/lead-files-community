/*
 *    Filename: event.h
 * Description: 이벤트 관련 (timed event)
 *
 *      Author: 김한주 (aka. 비엽, Cronan), 송영진 (aka. myevan, 빗자루)
 */

#ifndef __INC_METIN_EVENT_H__
#define __INC_METIN_EVENT_H__

typedef struct priority_q_element PRIORITY_Q_ELEMENT;
typedef struct event_info	EVENT;
typedef struct event_info *	LPEVENT;


#define EVENTFUNC(name) long (name) (LPEVENT event)

struct event_info
{
    EVENTFUNC(*func);
    void*		info;
    PRIORITY_Q_ELEMENT*	q_el;

    char		file[64];
    int			line;
};

/* 함수 선언 */
extern void event_init();
extern void event_destroy();

extern LPEVENT event_create_ex(EVENTFUNC(*func), void *event_obj, long when, const char *file, int line);
#define event_create(func, event_obj, when) event_create_ex(func, event_obj, when, __FILE__, __LINE__)

extern void event_cancel(LPEVENT event);
extern void event_process();
extern long event_time(LPEVENT event);
extern void event_reset_time(LPEVENT event, long when);

#endif
