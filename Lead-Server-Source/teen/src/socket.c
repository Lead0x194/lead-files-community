/*
 *    Filename: socket.c
 * Description: 소켓 관련 소스. 입력 큐 관련 루틴.
 *
 *      Author: 비엽 (Server), myevan (Client)
 */

#include "structs.h"
#include "utils.h"
#include "db.h"
#include "memory.h"
#include "socket.h"
#include "input.h"
// #include "hash.h"
#include "log.h"

DESC_DATA *	descriptor_list;

/* internal function define */
void		socket_lingeroff(socket_t s);
void            socket_sndbuf(socket_t s, unsigned int opt);
void            socket_rcvbuf(socket_t s, unsigned int opt);
void            socket_timeout(socket_t s, long sec, long usec);
void		socket_reuse(socket_t s);

void		conn_delete_by_desc(DESC_DATA *d);

extern int	input_desc_remain;
extern int	input_bytes_remain;

static struct data_block *	queue_poll = NULL;

void packet_init(PACKET_DATA *buffer)
{
	CREATE(buffer->data, char, DEFAULT_PACKET_BUFFER_SIZE);

	buffer->length	= 0;
	buffer->max_length	= DEFAULT_PACKET_BUFFER_SIZE;
}

void packet_free(PACKET_DATA *buffer)
{
	free(buffer->data);
}

void packet(PACKET_DATA *buffer, const void *data, int length)
{
	// 버퍼가 모자르면.. -_-;
	if ((buffer->length + length) > buffer->max_length)
	{
		buffer->max_length *= 2;

		if (!(buffer->data = (char*)realloc(buffer->data, buffer->max_length)))
		{
			perror("realloc failure"); 
			abort();
		}

		sys_log("SYSTEM: Reallocating memory to MAX %d, Current length %d encoding %d", 
				buffer->max_length, buffer->length, length);
	}

	memcpy(&buffer->data[buffer->length], data, length);
	buffer->length += length;
}

const char *encode_byte(char ind)
{
	static char a[1];

	*((char *) a) = ind;
	return (a);
}

const char *encode_2bytes(sh_int ind)
{
	static char a[2];

	*((sh_int *) a) = ind;
	return (a);
}

const char *encode_4bytes(int ind)
{
	static char a[4];

	*((int *) a) = ind;
	return (a);
}

ssize_t perform_socket_read(socket_t desc, char *read_point, size_t space_left)
{
	ssize_t	ret;

	ret = recv(desc, read_point, space_left, 0);

	if (ret > 0)
		return ret;

	return -1;
}

int socket_write_tcp(socket_t desc, const char *txt, int length)
{
	int bytes_written = send(desc, txt, length, 0);

	// 성공
	if (bytes_written > 0)
		return (bytes_written);

	if (bytes_written == 0)
		return -1;

#ifdef EAGAIN           /* POSIX */
	if (errno == EAGAIN)
		return 0;
#endif

#ifdef EWOULDBLOCK      /* BSD */
	if (errno == EWOULDBLOCK)
		return 0;
#endif

#ifdef EDEADLK          /* Macintosh */
	if (errno == EDEADLK)
		return 0;
#endif
	/* Looks like the error was fatal.  Too bad. */
	return -1;
}

int write_to_descriptor(socket_t desc, const char *data, size_t length)
{
	size_t      total;
	ssize_t     bytes_written;

	total = length;

	do
	{
		if ((bytes_written = socket_write_tcp(desc, data, total)) < 0)
		{
#ifdef EWOULDBLOCK
			if (errno == EWOULDBLOCK)
				errno = EAGAIN;
#endif
			if (errno == EAGAIN)
				sys_log("SYSERR: socket write would block, about to close!");
			else
				sys_log("SYSERR: write to descriptor error");   // '보통' 상대편으로 부터 접속이 끊긴 것이다.

			return -1;
		}
		else
		{
			data        += bytes_written;
			total       -= bytes_written;
		}
	}
	while (total > 0);

	return 0;
}

int socket_init(int port)
{
	int                 s;
	struct sockaddr_in  sa;

	if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{
		perror("socket");
		exit(1);
	}

	socket_reuse(s);
	socket_lingeroff(s);

	sa.sin_family	= AF_INET;
	sa.sin_port		= htons(port);
	sa.sin_addr.s_addr	= htonl(INADDR_ANY);

	if (bind(s, (struct sockaddr *) &sa, sizeof(sa)) < 0)
	{
		perror("bind");
		exit(1);
	}

	sys_log("SYSTEM: BINDING PORT [%d]", port);

	socket_nonblock(s);
	listen(s, SOMAXCONN);
	return s;
}

void socket_close(DESC_DATA *d)
{
	packet_free(&d->packet_buffer);

	CLOSE_SOCKET(d->descriptor);

	REMOVE_FROM_TW_LIST(d, descriptor_list, prev, next);

	if (d->desc_num == input_desc_remain)
	{
		input_desc_remain	= 0;
		input_bytes_remain	= 0;
	}

	conn_delete_by_desc(d);

	sys_log("SYSTEM: closing socket. DESC #%d", d->descriptor); 
	free(d);
}

socket_t socket_accept(socket_t s, struct sockaddr_in *peer)
{
	socket_t desc;
	size_t i;

	i = sizeof(*peer);

	if ((desc = accept(s, (struct sockaddr *) peer, &i)) == -1)
	{
		sys_log("accept");
		return -1;
	}

	if (desc >= 65500)
	{
		sys_log("SYSERR: SOCKET FD 65500 LIMIT! %d", desc);
		CLOSE_SOCKET(desc);
		return -1;
	}

	socket_sndbuf(desc, 233016);
	socket_rcvbuf(desc, 233016);
	socket_timeout(desc, 15, 0);

	socket_lingeroff(desc);
	return (desc);
}

#ifndef O_NONBLOCK
#define O_NONBLOCK O_NDELAY
#endif

void socket_nonblock(socket_t s)
{
	int flags;

	flags = fcntl(s, F_GETFL, 0);
	flags |= O_NONBLOCK;

	if (fcntl(s, F_SETFL, flags) < 0) 
	{
		perror("SYSERR: Fatal error executing nonblock (comm.c)");
		exit(1);
	}
}

void socket_dontroute(socket_t s)
{
	int set;

	if (setsockopt(s, SOL_SOCKET, SO_DONTROUTE, (const void *) &set, sizeof(int)) < 0)
	{
		perror("SYSERR: Fatal error executing setsockopt()");
		exit(1);
	}
}

void socket_lingeroff(socket_t s)
{
	struct linger linger;

	linger.l_onoff	= 0;
	linger.l_linger	= 0;

	if (setsockopt(s, SOL_SOCKET, SO_LINGER, &linger, sizeof(linger)) < 0)
	{
		perror("setsockopt: LINGER");
		CLOSE_SOCKET(s);
		exit(1);
	}
}

void socket_rcvbuf(socket_t s, unsigned int opt)
{
	size_t optlen;

	optlen      = sizeof(opt);

	if (setsockopt(s, SOL_SOCKET, SO_RCVBUF, (const unsigned int *) &opt, optlen) < 0)
	{
		perror("setsockopt: rcvbuf");
		close(s);
		exit(1);
	}

	opt         = 0;
	optlen      = sizeof(opt);

	if (getsockopt(s, SOL_SOCKET, SO_RCVBUF, (unsigned int *) &opt, &optlen) < 0)
	{
		perror("getsockopt: rcvbuf");
		close(s);
		exit(1);
	}

	sys_log("SYSTEM: %d: receive buffer changed to %d", s, opt);
}

void socket_sndbuf(socket_t s, unsigned int opt)
{
	size_t	optlen;

	optlen      = sizeof(opt);

	if (setsockopt(s, SOL_SOCKET, SO_SNDBUF, (const unsigned int *) &opt, optlen) < 0)
	{
		perror("setsockopt: sndbuf");
		exit(1);
	}

	opt         = 0;
	optlen      = sizeof(opt);

	if (getsockopt(s, SOL_SOCKET, SO_SNDBUF, (unsigned int *) &opt, &optlen) < 0)
	{
		perror("getsockopt: sndbuf");
		exit(1);
	}

	sys_log("SYSTEM: %d: send buffer changed to %d", s, opt);
}

// sec : seconds, usec : microseconds
void socket_timeout(socket_t s, long sec, long usec)
{
	struct timeval      rcvopt;
	struct timeval      sndopt;
	size_t		optlen = sizeof(rcvopt);

	rcvopt.tv_sec = sndopt.tv_sec = sec;
	rcvopt.tv_usec = sndopt.tv_usec = usec;

	if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &rcvopt, optlen) < 0)
	{
		perror("setsockopt: timeout");
		close(s);
		exit(1);
	}

	if (getsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &rcvopt, &optlen) < 0)
	{
		perror("setsockopt: timeout");
		close(s);
		exit(1);
	}

	optlen = sizeof(sndopt);

	if (setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, &sndopt, optlen) < 0)
	{
		perror("setsockopt: timeout");
		close(s);
		exit(1);
	}

	if (getsockopt(s, SOL_SOCKET, SO_SNDTIMEO, &sndopt, &optlen) < 0)
	{
		perror("setsockopt: timeout");
		close(s);
		exit(1);
	}

	sys_log("SYSTEM: %d: TIMEOUT RCV: %d.%d, SND: %d.%d", s, rcvopt.tv_sec, rcvopt.tv_usec, sndopt.tv_sec, sndopt.tv_usec);
}

void socket_reuse(socket_t s)
{
	int opt = 1;

	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		perror("setsockopt: reuse");
		close(s);
		exit(1);
	}
}

void flush_queues(struct data_q *queue)
{
	char buf[MAX_INPUT_LEN];
	WORD tmp;

	while (get_from_q(queue, buf, &tmp));
}

int fetch_from_q(struct data_q *dest, char *data, WORD id)
{
	struct data_block	*tblock;
	struct data_block	*prev = NULL;

	int bytes;

	if (!dest->head)
		return 0;

	for (tblock = dest->head; tblock; tblock = tblock->next)
	{
		if (tblock->id == id)
		{
			bytes	= tblock->bytes;

			memcpy(data, tblock->data, bytes);

			// 이전 데이터가 없으면 리스트의 head 이다.
			if (!prev)
			{
				// tblock 이 head 이자 tail 이면 리스트에 아무 것도 없는 것이다.
				if (tblock == dest->tail)
					dest->head = dest->tail = NULL;
				// tail 이 아니면 head 를 tblock 의 next 멤버로 만든다.
				else
					dest->head = tblock->next;
			}
			// 이전 데이터가 있으면 리스트의 중간이다.
			else
			{
				// 이전 데이터의 다음을 tblock 의 다음으로 만든다.
				prev->next = tblock->next;

				// 만약 tblock (fetch 할 데이터) 이 tail 이라면
				if (tblock == dest->tail)
				{
					// tail 이므로 next 가 없을 것이다. 그러므로 이전 리스트가 tail 로 된다.
					dest->tail = prev;
				}
			}

			// free 하지 않고 리스트에 넣는다.
			tblock->next    = queue_poll;
			queue_poll      = tblock;

			dest->count--;
			return (bytes);
		}

		prev = tblock;
	}

	return 0;
}

int get_from_q(struct data_q *dest, char *str, WORD *id)
{
	struct data_block	*new_block;
	int			bytes;

	if (!dest->head)
		return 0;

	new_block	= dest->head;
	bytes	= dest->head->bytes;
	*id		= dest->head->id;

	memcpy(str, dest->head->data, bytes);

	dest->head = dest->head->next;

	// free 하지 않고 리스트에 넣는다.
	new_block->next     = queue_poll;
	queue_poll          = new_block;
	// free(new_block->data);
	// free(new_block);

	dest->count--;

	return (bytes);
}

void write_to_q(char *data, struct data_q *dest, int bytes, WORD id)
{
	struct data_block    *new_block;

	// queue poll 이 있다면,
	if (queue_poll)
	{
		new_block       = queue_poll;
		queue_poll      = queue_poll->next;
	}
	else
		CREATE(new_block, struct data_block, 1);

	if (new_block->memory_bytes < bytes)
	{
		if (new_block->memory_bytes == 0)
			CREATE(new_block->data, char, bytes);
		else
			new_block->data = (char*)realloc(new_block->data, bytes);

		// memory_bytes 는 메모리 상의 진짜 데이터 이다.
		new_block->memory_bytes = bytes;
	}

	memcpy(new_block->data, data, bytes);
	new_block->bytes		= bytes;
	new_block->id		= id;

	if (!dest->head)
	{
		new_block->next = NULL;
		dest->head = dest->tail = new_block;
	}
	else
	{
		dest->tail->next = new_block;
		dest->tail	 = new_block;

		new_block->next  = NULL;
	}

	dest->count++;
}

int valid_host(const char *host)
{
	return 1;

	static struct valid_ip
	{
		const char*	ip;
		BYTE		network;
		BYTE		mask;
	} valid_ip_info[] = {
		{ "10.0.0",         0,      0   },
		{ "10.10.0",         0,      0   },
		{ "10.10.100",         0,      0   },
		{ "172.20.0",         0,      0   },
		{ "192.168.10",         0,      0   },
		{ "192.168.1",         0,      0   },
		{ "127.0.0",            1,      255 },
		{ "210.123.10",         128,    128 },
		{ "210.181.251",        112,    240 },
		{ "211.93.159",         90,     240 },
		{ "211.91.13",          244,    248 },
		{ "61.153.3",           192,    192 },
		{ "210.51.9",		131,	248 },
		{ "210.38.2",		20,	248 },
		{ "218.104.201",        106,    255 },
		{ "218.104.201",        107,    255 },
		{ "218.104.201",        108,    255 },
		{ "61.151.251",         69,     251 },
		{ "211.100.19",		64,	128 },
		{ "61.156.17",          201,    251 },  
		{ "61.240.232",         17,     251 },
		{ "218.30.22",          115,    251 },
		{ "202.102.249",	161,	164 },
		{ "218.11.243",         227,    237 },  
		{ "220.95.239",         121,    125 },  
		{ "61.55.137",			0,		0   },	// cibn cnc1
		{ "61.180.228",			0,		0   },	// cibn cnc2
		{ "218.7.219",			0,      0   },	// cibn cnc3
		{ "121.11.87",			0,      0   },	// cibn te1
		{ "222.221.3",			0,      0   },	// cibn te2
		{ "61.147.119",			0,      0   },	// cibn te3
		{ "222.221.3",			0,      0   },	// cibn te4
		{ "218.99.6",			0,      0   },	// cibn te5
		{ "218.25.92",			0,      0   },	// cibn te6

		{ "\n",                 0,      0 },
	}; 

	int         i, j;   
	char        ip_addr[256];

	for (i = 0; *valid_ip_info[i].ip != '\n'; i++)
	{
		j = 255 - valid_ip_info[i].mask;

		do
		{

			sprintf(ip_addr, "%s.%d", valid_ip_info[i].ip, valid_ip_info[i].network + j);

			if (!strcmp(host, ip_addr))
				return TRUE;

			if (!j)
				break;
		}
		while (j--);
	}

	return FALSE;
}

void init_descriptor(DESC_DATA *newd, socket_t desc, char *host)
{
	static WORD desc_numbers = 0;

	packet_init(&newd->packet_buffer);

	strncpy(newd->host, host, MAX_HOST_LENGTH);
	*(newd->host + MAX_HOST_LENGTH) = '\0';

	newd->descriptor    = desc;

	assign_state(newd, STATE_HANDSHAKE);

	if (++desc_numbers >= 65535)
		desc_numbers = 1;

	newd->desc_num	= desc_numbers;
	newd->handshake	= random() % (1024 * 1024);

	sys_log("SYSTEM: new connection from [%s]", newd->host);
}

int new_descriptor(int s)
{
	socket_t                    desc;
	int                         sockets_connected = 0;
	DESC_DATA                   *newd;
	static struct sockaddr_in   peer;

	if ((desc = socket_accept(s, &peer)) == -1)
		return -1;

	for (newd = descriptor_list; newd; newd = newd->next)
		sockets_connected++;

	if (sockets_connected >= MAX_ALLOW_USER)
	{
		CLOSE_SOCKET(desc);
		return 0;
	}

	if (!valid_host(inet_ntoa(peer.sin_addr)))
	{
		sys_log("\007\007NOT A VALID SITE [%s]", inet_ntoa(peer.sin_addr));
		CLOSE_SOCKET(desc);
		return 0;
	}

	CREATE(newd, DESC_DATA, 1);

	init_descriptor(newd, desc, inet_ntoa(peer.sin_addr));

	/* 리스트에 추가 */
	INSERT_TO_TW_LIST(newd, descriptor_list, prev, next);

	packet(&newd->packet_buffer, encode_4bytes(newd->handshake ^ HANDSHAKE_XOR), 4);
	return 0;
}

void assign_state(DESC_DATA * d, int state)
{
	switch (state)
	{
		case STATE_CLOSE:
			d->input_processor = NULL;
			break;

		case STATE_HANDSHAKE:
			d->input_processor = input_handshake;
			break;

		case STATE_MAIN:
			d->input_processor = input_main;
			break;
	}

	d->state = state;
}
