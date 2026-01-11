#include "structs.h"
#include "utils.h"
#include "socket.h"
#include "protocol.h"
#include "input.h"
#include "log.h"
#include "sql_account.h"
#include "config.h"
#include <common/teen_packet.h>


#include <map>

extern int		global_pulse;


#define	CHECK_TIME_SEC	10		// 10초



struct conn_event_info
{
	CONN_DATA	*conn;
};

typedef std::map<DWORD,CONN_DATA*>	CONN_MAP;



CONN_MAP	s_conn_map;

void packet_header(PACKET_DATA *buf, BYTE header, WORD desc_num)
{
	packet(buf,	encode_byte(header),		1);
	packet(buf,	encode_2bytes(desc_num),	2);
}

void packet_teen_notice(CONN_DATA *conn, int hour)
{
	DESC_DATA	*desc = GET_DESC(conn);
	PACKET_DATA	*buf = &desc->packet_buffer;

	packet_header(buf, HEADER_TG_TEEN_NOTICE, desc->desc_num);
	packet(buf, GET_LOGIN(conn), MAX_LOGIN_LEN);
	packet(buf, encode_4bytes(hour), 4);

	sys_log("NOTICE: (login,hour) = (%s, %d)", GET_LOGIN(conn), hour);
}

void packet_force_logout(DESC_DATA *desc, CONN_DATA *conn)
{
	PACKET_DATA	*buf = &desc->packet_buffer;

	packet_header(buf, HEADER_TG_FORCE_LOGOUT, desc->desc_num);
	packet(buf, GET_LOGIN(conn), MAX_LOGIN_LEN);
}

void packet_login_notice(CONN_DATA *conn)
{
	DESC_DATA	*desc = GET_DESC(conn);
	if (NULL==desc)	return;
	PACKET_DATA	*buf = &desc->packet_buffer;

	packet_header(buf, HEADER_TG_LOGIN_NOTICE, desc->desc_num);
	packet(buf, GET_LOGIN(conn), MAX_LOGIN_LEN);
	packet(buf, encode_4bytes(GET_ON_TIME(conn)), 4);
	packet(buf, encode_4bytes(GET_OFF_TIME(conn)), 4);
}

void send_force_logout(CONN_DATA *conn)
{
	DESC_DATA	*desc = GET_DESC(conn);
	if (NULL==desc) return;

	sys_log("FORCE_LOGOUT : %d, %s", GET_ID(conn), GET_LOGIN(conn)?GET_LOGIN(conn):"null");
	packet_force_logout(desc, conn);
}

int __online_hour(CONN_DATA *conn)
{
	return (GET_ON_TIME(conn)/HOUR_TO_SEC(1));
}


void send_login_notice(CONN_DATA *conn)
{
	packet_login_notice(conn);
}

void set_notice_step(CONN_DATA *conn)
{
	int hour = __online_hour(conn);
	switch (hour)
	{
		case 0: GET_NOTICE_STEP(conn) = 0; break;
		case 1: GET_NOTICE_STEP(conn) = 0; break;
		case 2: GET_NOTICE_STEP(conn) = 1; break;
		case 3: GET_NOTICE_STEP(conn) = 2; break;
		case 4: GET_NOTICE_STEP(conn) = 3; break;
		default: GET_NOTICE_STEP(conn) = 4; break;
	}
	GET_NOTICE_PULSE(conn) = 0;
}

void send_notice(CONN_DATA *conn)
{
	int	hour = __online_hour(conn);

	switch (hour)
	{
		case 0:
			/* do nothing */
			break;
		case 1:
			if (0==GET_NOTICE_STEP(conn))
			{
				GET_NOTICE_STEP(conn) = 1;
				packet_teen_notice(conn, hour);
			}
			break;
		case 2:
			if (1==GET_NOTICE_STEP(conn))
			{
				GET_NOTICE_STEP(conn) = 2;
				packet_teen_notice(conn, hour);
			}
			break;
		case 3:
			if (2==GET_NOTICE_STEP(conn))
			{
				GET_NOTICE_STEP(conn) = 3;
				packet_teen_notice(conn, hour);
			}
			break;
		case 4:
			if (3==GET_NOTICE_STEP(conn))
			{
				GET_NOTICE_STEP(conn) = 4;
				packet_teen_notice(conn, hour);
			}
			break;
		default:
			if ( 0==(GET_NOTICE_PULSE(conn) % MINUTE_TO_PULSE(15)) )
			{
				packet_teen_notice(conn, hour);
				GET_NOTICE_PULSE(conn) = global_pulse;
			}
			break;
	}
}


EVENTFUNC(conn_event_func)
{
	if (NULL==event || NULL==event->info) return 0;

	struct conn_event_info	*info = (conn_event_info*) event->info;
	CONN_DATA	*conn = info->conn;

	if (NULL==conn)
		return 0;

	GET_ON_TIME(conn) += CHECK_TIME_SEC;

	send_notice(conn);

	return (CHECK_TIME_SEC * PASSES_PER_SEC);
}



void conn_delete_by_desc(DESC_DATA *d)
{
	CONN_MAP::iterator	iter, cur_iter;

	for (iter = s_conn_map.begin(); iter != s_conn_map.end(); )
	{
		cur_iter = iter; iter++;

		CONN_DATA	*conn = cur_iter->second;

		if (GET_DESC(conn) == d)
		{
			s_conn_map.erase(cur_iter);
			account_logout(conn);


			if (GET_EVENT(conn))
			{
				event_cancel(GET_EVENT(conn));
				GET_EVENT(conn) = NULL;
			}
			SAFE_FREE(conn);
		}
	}
}


CONN_DATA *conn_find(DWORD account_id)
{
	CONN_MAP::iterator	iter;

	iter = s_conn_map.find(account_id);
	if (iter!=s_conn_map.end())
	{
		CONN_DATA *conn = iter->second;
		return conn;
	}

	return NULL;
}


CONN_DATA* conn_insert(CONN_DATA *conn)
{
	s_conn_map.insert(std::make_pair(GET_ID(conn), conn));

	return conn;
}


void conn_delete(DWORD account_id)
{
	CONN_MAP::iterator	iter;

	iter = s_conn_map.find(account_id);

	if (iter==s_conn_map.end())
		return;
	CONN_DATA	*conn = iter->second;
	s_conn_map.erase(iter);

	sys_log("CONN_DELETE : %d, %s", GET_ID(conn), GET_LOGIN(conn)?GET_LOGIN(conn):"null");

	if (GET_EVENT(conn))
	{
		event_cancel(GET_EVENT(conn));
		GET_EVENT(conn) = NULL;
	}
	account_logout(conn);
	SAFE_FREE(conn);
} 




int input_login(DESC_DATA *d, const char *data, int plen)
{
	const int	PACKET_LENGTH = 4;

	if (plen < PACKET_LENGTH)
		return 0;

	DWORD	account_id = decode_4bytes((const BYTE*)data);

//	sys_log("LOGIN: %u", account_id);

	/* 기존 정보가 있으면 접속을 끊으라고 알려줌 */
	CONN_DATA	*conn = conn_find(account_id);
	if (conn)
	{
		send_force_logout(conn);
		//conn_delete(GET_ID(conn));

		// 새로운 접속자도 접속을 끊는다.
		{
			PACKET_DATA	*buf = &d->packet_buffer;

			packet_header(buf, HEADER_TG_FORCE_LOGOUT, d->desc_num);
			packet(buf, GET_LOGIN(conn), MAX_LOGIN_LEN);
		}
		return PACKET_LENGTH;
	}

	conn = (CONN_DATA*) calloc(1, sizeof(CONN_DATA));
	GET_ID(conn)	= account_id;
	GET_DESC(conn)	= d;
	GET_EVENT(conn)	= NULL;


	int login_succ = FALSE;
	if (teen_all_flag)
		login_succ = account_login_teen_all(account_id, conn);
	else
		login_succ = account_login(account_id, conn);

	if (FALSE==login_succ)
	{
		//conn_delete(GET_ID(conn));
		SAFE_FREE(conn);
		return PACKET_LENGTH;
	}

	conn_insert(conn);

	if (YES==GET_TEENAGE(conn))
	{
		conn_event_info	*info = (conn_event_info*) calloc(1, sizeof(conn_event_info));
		info->conn = conn;
		GET_EVENT(conn) = event_create(conn_event_func, info, (CHECK_TIME_SEC * PASSES_PER_SEC));
	}
	else
	{
		GET_EVENT(conn) = NULL;
	}

	if (YES==GET_TEENAGE(conn))
	{
		send_login_notice(conn);
		set_notice_step(conn);
		send_notice(conn);
	}

	sys_log("CONN_INSERT : %u, %s", GET_ID(conn), GET_LOGIN(conn));

	return PACKET_LENGTH;
} /* end of input_login() */


int input_logout(DESC_DATA *d, const char *data, int plen)
{
	const int PACKET_LENGTH = 4;

	if (plen < PACKET_LENGTH)
		return 0;

	DWORD account_id = decode_4bytes((const BYTE*)data);

//	sys_log("LOGOUT: %u", account_id);

	CONN_DATA	*conn = conn_find(account_id);
	if (NULL==conn)
	{
		return PACKET_LENGTH;
	}

	// update online time and logout time
	{
		conn_delete(account_id);
	}

	return PACKET_LENGTH;
} /* end of input_logout() */



int input_handshake(DESC_DATA *d, char *orig, int bytes)
{
	if (bytes < 4)
		return 0;

	if (d->handshake == *(DWORD *) orig)
		assign_state(d, STATE_MAIN);
	else
	{
		sys_log("SYSERR: BAD_HANDSHAKE: %d %d", d->handshake, *(DWORD *) orig);
		assign_state(d, STATE_CLOSE);
	}

	return 4;
}


int input_main(DESC_DATA *d, char *orig, int bytes)
{
	char 	*data = orig;
	BYTE	header, last_header = 0;
	WORD	desc_num;
	int		i, pl, last_pl = 0, bytes_proceed = 0;

	for (i = bytes; i > 0;)
	{
		header = (BYTE) *(data++);
		i--;

		// desc_num 은 2 bytes 이므로 2 bytes 보다 내용이 적으면 bytes 를 리턴
		if (i < 2)
			return (bytes_proceed);

		desc_num = *((WORD *) data);

		data	+= 2;
		i	-= 2;

		pl	= 0;

		switch (header)
		{
			case HEADER_GT_LOGIN:
				if (!(pl = input_login(d, data, i)))
					return (bytes_proceed);

				break;

			case HEADER_GT_LOGOUT:
				if (!(pl = input_logout(d, data, i)))
					return (bytes_proceed);

				break;

			default:
				sys_log("UNKNOWN HEADER: %d, LAST HEADER: %d(%d), REMAIN BYTES: %d, DESC: %d", 
						header, last_header, last_pl, i, d->desc_num);

				printdata((BYTE*)orig, bytes);
				assign_state(d, STATE_CLOSE);
				return (bytes);
		}

		data		+= pl;
		i		-= pl;

		bytes_proceed	+= 3 + pl;

		last_pl		= pl;
		last_header	= header;
	}

	return (bytes);
}
