/*
 *    Filename: structs.h
 * Description: 각종 헤더 포함 밑 구조체 밑 중요한(?)부분을 가지고 있는 헤더.
 *
 *      Author: 비엽 (server), myevan (Client)
 */
#ifndef _STRUCTS_H_
#define _STRUCTS_H_

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <netdb.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>

#include "length.h"

#include "macros.h"

#include "event.h"


int atoi(const char *nptr);

#define OPT_USEC			20000
// PASSES_PER_SEC 은 1000000 / OPT_USEC 다.
#define PASSES_PER_SEC  	(50)
#define RL_SEC          	* PASSES_PER_SEC

typedef signed char             sbyte;
typedef unsigned char           ubyte;
typedef signed short int        sh_int;
typedef unsigned short int      ush_int;
typedef int                     socket_t;
typedef struct descriptor_data  DESC_DATA;
typedef struct packet_data      PACKET_DATA;
typedef struct affect_data      AFFECT_DATA;
typedef struct npc_relation	NPC_RELATION;

typedef unsigned int            DWORD;
typedef unsigned short int      WORD;

typedef char                    byte;

typedef unsigned char           BYTE;
typedef unsigned int            LONG;
typedef unsigned int            INT;

typedef sh_int			OBJ_NUM;

/************************************************************************
 * 디스크립터 관련 define 들 						*
 ************************************************************************/
#define CLOSE_SOCKET(sock)	close(sock);

#define DEFAULT_PACKET_BUFFER_SIZE	131072
#define MAX_INPUT_LEN		65535

#define MAX_NAME_LENGTH         16
#define MAX_OBJ_NAME_LENGTH     32
#define MAX_SOCIAL_LENGTH       16
#define MAX_TITLE_LENGTH        16
#define MAX_PWD_LENGTH          16 

#define STATE_CLOSE		0
#define STATE_HANDSHAKE		1
#define STATE_MAIN		2

#define MAX_OBJ_AFFECT                  4       // 0 1 2 3
#define MAX_OBJ_VALUES                  5       // 0 1 2 3 4
#define MAX_OBJ_RESOURCES               4

#define DESCTYPE_GAME		0
#define DESCTYPE_ACCOUNT	1
#define DESCTYPE_CLIENT		2

/************************************************************************
 * 디스크립터 관련 구조체						*
 ************************************************************************/
struct data_block
{
    int			memory_bytes;
    int			bytes;

    WORD		id;

    char		*data;

    struct data_block	*next;
};


struct data_q
{
    int			count;

    struct data_block	*head;
    struct data_block	*tail;
};


struct packet_data
{
    int		max_length;
    int		length;

    char	*data;
};


struct descriptor_data
{
    int				(*input_processor) (struct descriptor_data *d, char *data, int bytes_read);

    BYTE			type;

    WORD			users;			// 이 서버에 연결된 사용자 수

    WORD			desc_num;

    int				descriptor;	
    int				server_descriptor;

    WORD			poll;			// POLL 의 위치
    int 			state;

    char 			host[MAX_HOST_LENGTH];
    char			inbuf[MAX_INPUT_LEN];
    DWORD			handshake;

    PACKET_DATA			packet_buffer;

    struct descriptor_data	*prev;
    struct descriptor_data	*next;
};


#define MAX_LOGIN_LEN	30

typedef struct conn_data_s
{
	DWORD		account_id;
	char		login[MAX_LOGIN_LEN+1];
	int			teenage;
	int			on_time;
	int			off_time;
	DESC_DATA	*desc;

	int			notice_step;
	int			notice_pulse;
	EVENT		*event;
} CONN_DATA;

#endif	// _STRUCTS_H_

