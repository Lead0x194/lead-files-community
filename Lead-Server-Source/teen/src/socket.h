/*
 *    Filename: socket.c
 * Description: 소켓 관련 함수 헤더.
 *
 *      Author: 비엽 (server), myevan (Client)
 */
#ifndef _SOCKET_H_
#define _SOCKET_H_

#define	PORT_NUM		54300
#define MAX_ALLOW_USER		2024

extern	DESC_DATA	*descriptor_list;

void	packet_init(PACKET_DATA *buffer);
void	packet_free(PACKET_DATA *buffer);
void	packet(PACKET_DATA *buffer, const void *data, int length);
void	assign_state(DESC_DATA * d, int state);
void	init_descriptor(DESC_DATA *newd, socket_t desc, char *host);
int	new_descriptor(int s);

ssize_t perform_socket_read(socket_t desc, char *read_point, size_t space_left);
int write_to_descriptor(socket_t desc, const char *data, size_t length);

int	 socket_init(int port);
socket_t socket_accept(socket_t s, struct sockaddr_in *peer);
void	 socket_close(DESC_DATA *d);

void	socket_nonblock(socket_t s);
void	socket_dontroute(socket_t s);
void	socket_lingeroff(socket_t s);

void	flush_queues(struct data_q *queue);
int	fetch_from_q(struct data_q *dest, char *str, WORD id);
int	get_from_q(struct data_q *dest, char *str, WORD *id);
void	write_to_q(char *str, struct data_q *dest, int bytes, WORD id);

const char *encode_byte(char ind);
const char *encode_2bytes(sh_int ind);
const char *encode_4bytes(int ind);

#endif	// _SOCKET_H_
