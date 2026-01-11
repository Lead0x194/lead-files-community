/*
 *    Filename: macros.h
 * Description: 각종 매크로들이 정의되어 있는 헤더. structs.h 에서 include 함.
 *
 *      Author: 비엽 (server), myevan (Client)
 */
#ifndef _MACROS_H_
#define _MACROS_H_

/*
 * 디스크립터 관련 매크로
 */

#define STATE(d)		((d)->state)


#define GET_ID(conn)			((conn)->account_id)
#define GET_LOGIN(conn)			((conn)->login)
#define GET_TEENAGE(conn)		((conn)->teenage)
#define GET_ON_TIME(conn)		((conn)->on_time)
#define GET_OFF_TIME(conn)		((conn)->off_time)
#define	GET_DESC(conn)			((conn)->desc)
#define GET_NOTICE_STEP(conn)	((conn)->notice_step)
#define GET_NOTICE_PULSE(conn)	((conn)->notice_pulse)
#define GET_EVENT(conn)			((conn)->event)


#define HOUR_TO_SEC(x)	((x)*60*60)
#define SEC_TO_HOUR(x)	((x)/(60*60))

#define MINUTE_TO_SEC(x)	((x)*60)
#define SEC_TO_MINUTE(x)	((x)/60)

#define SEC_TO_PULSE(sec)		((sec) * PASSES_PER_SEC)
#define MINUTE_TO_PULSE(min)	SEC_TO_PULSE((min)*60)
#define HOUR_TO_PULSE(hour)		MINUTE_TO_PULSE((hour)*60)

#endif	// _MACROS_H_
