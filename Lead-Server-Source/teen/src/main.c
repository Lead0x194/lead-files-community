#include "structs.h"
#include "utils.h"
#include "socket.h"
#include "config.h"
#include "log.h"
#include "sql.h"

// 게임과 연결되는 소켓
socket_t	mother_desc = 0;
int		shutdowned = FALSE;
int		tics = 0;
int		global_pulse = 0;

int		process_input(DESC_DATA * d);
int		process_output(DESC_DATA * d);
void		process_command(void);

WORD		input_desc_remain = 0;
int		input_bytes_remain = 0;

struct timeval	null_time;

void server_loop(void);
void signal_setup(void);
void insert_all(void);
void heartbeat(int pulse);

struct		data_q	input_q;

void pid_init(void)
{
	FILE        *fp;

	if ((fp = fopen("pid", "w")))
	{
		fprintf(fp, "%d", getpid());
		fclose(fp); 
	}   
	else
	{
		sys_log("SYSERR: main(): could not open file for writing. (filename: ./pid)");
		exit(1);
	}
}


void pid_deinit(void)
{
	remove("./pid");
}

void clean_server()
{
	DESC_DATA	*d, *next_d;

	for (d = descriptor_list; d; d = next_d)
	{
		next_d = d->next;
		socket_close(d);
	}

	if (mother_desc)
		CLOSE_SOCKET(mother_desc);
}

void shutdown_server()
{
	clean_server();
	event_destroy();

	sql_deinit();

	pid_deinit();

	sys_log("Server has been shutdowned.");
	log_destroy();
}

void heartbeat(int pulse)
{
	event_process();

	// 1 초에 한번
	if (!(pulse%PASSES_PER_SEC))
	{
		// 5초에 한번
		if (!(pulse%(PASSES_PER_SEC*5)))
			log_rotate();
	}
}


int main(int argc, char **argv)
{
	input_q.count = 0;

	log_init();

	init_config();

	mother_desc = socket_init(mother_port);

	pid_init();

	signal_setup();

	if (!sql_init())
		return (-1);

	event_init();

	server_loop();

	shutdown_server();

	return 1;
}

void server_loop(void)
{
	struct timeval              opt_time, now, last_time, timeout, before_sleep, process_time;

	fd_set                      input_set, output_set, exc_set, null_set;

	register DESC_DATA          *d, *next_d;

	int                         missed_pulses, maxdesc = 0;

	null_time.tv_sec  = 0;
	null_time.tv_usec = 0;

	opt_time.tv_usec  = OPT_USEC;
	opt_time.tv_sec   = 0;

	gettimeofday(&last_time, (struct timezone *) 0);
	gettimeofday(&before_sleep, (struct timezone *) 0);

	last_time = *timeadd(&last_time, &opt_time);
	FD_ZERO(&null_set);

	while (!shutdowned)
	{
		if (descriptor_list == NULL)
		{
			sys_log("SYSTEM: No connections online. Entering sleep mode.");

			FD_ZERO(&input_set);
			FD_SET(mother_desc, &input_set);

			if (select(mother_desc + 1, &input_set, (fd_set *) 0, (fd_set *) 0, NULL) < 0)
			{
				if (errno == EINTR)
					sys_log("SYSTEM: Woke up by signal");
				else
					perror("select coma");
			}
			else
				sys_log("SYSTEM: Woke up by new connection.");

			gettimeofday(&last_time, (struct timezone *) 0);
			last_time = *timeadd(&last_time, &opt_time);

			gettimeofday(&before_sleep, (struct timezone *) 0);
		}


		FD_ZERO(&input_set);
		FD_ZERO(&output_set);
		FD_ZERO(&exc_set);

		FD_SET(mother_desc, &input_set);

		maxdesc = mother_desc;

		for (d = descriptor_list; d; d = next_d)
		{
			next_d = d->next;

			if (d->descriptor > maxdesc)
				maxdesc = d->descriptor;

			FD_SET(d->descriptor, &input_set);
			FD_SET(d->descriptor, &output_set);
			FD_SET(d->descriptor, &exc_set);
		}

		// 새로운 프레임 스킵/보완 코드 (이게 더 좋다는 것은 아닐 수도 있다. -_-)
		gettimeofday(&now, (struct timezone *) 0);

		process_time    = *timediff(&now, &before_sleep);
		before_sleep    = *timeadd(&now, timediff(&opt_time, &process_time));

		timeout         = *timediff(&last_time, &now);
		last_time       = *timeadd(&last_time, &opt_time);

		if (process_time.tv_sec == 0 && process_time.tv_usec < OPT_USEC)
			missed_pulses = 0;
		else
		{
			missed_pulses  = process_time.tv_sec * PASSES_PER_SEC;
			missed_pulses += process_time.tv_usec / OPT_USEC;

			process_time.tv_sec  = 0;
			process_time.tv_usec = process_time.tv_usec % OPT_USEC;
		}

		if (select(0, (fd_set *) 0, (fd_set *) 0, (fd_set *) 0, &timeout) < 0)
		{
			if (errno != EINTR)
			{
				perror("Select sleep");
				exit(1);
			}
		}

		if (select(maxdesc + 1, &input_set, &output_set, &exc_set, &null_time) < 0)
		{
			perror("select() poll");
			sys_log("SELECT POLL MAXDESC: %d", maxdesc);

			for (d = descriptor_list; d; d = next_d)
			{
				next_d = d->next;
				socket_close(d);
			}

			CLOSE_SOCKET(mother_desc);
			exit(1);
		}

		// 게임 소켓으로 부터 접속을 받는다
		if (FD_ISSET(mother_desc, &input_set))
			new_descriptor(mother_desc);

		for (d = descriptor_list; d; d = next_d)
		{
			next_d = d->next;

			if (FD_ISSET(d->descriptor, &exc_set) || STATE(d) == STATE_CLOSE)
			{
				FD_CLR(d->descriptor, &input_set);
				FD_CLR(d->descriptor, &output_set);

				socket_close(d);
			}
		}

		// 소켓에서 받기 
		for (d = descriptor_list; d; d = next_d)
		{
			next_d = d->next;

			if (FD_ISSET(d->descriptor, &input_set))
				if (process_input(d) < 0)
					socket_close(d);

		}

		process_command();

		// 소켓으로 보내기 처리
		for (d = descriptor_list; d; d = next_d)
		{
			next_d = d->next;

			if (FD_ISSET(d->descriptor, &output_set))
				if (process_output(d) < 0)
					socket_close(d);
		}

		missed_pulses++;

		if (missed_pulses <= 0)
		{
			sys_err("missed_pulses 변수가 양수가 아닙니다!! (%d)", missed_pulses);
			missed_pulses = 1;
		}

		if (missed_pulses > (30 * PASSES_PER_SEC))
		{
			sys_err("%d 초의 펄스를 잃었습니다. (서버에 랙이 발생)", missed_pulses / PASSES_PER_SEC);
			missed_pulses = 30 * PASSES_PER_SEC;
		} 

		while (missed_pulses--) 
			heartbeat(++global_pulse); 

		tics++;
	}
}


int process_input(DESC_DATA * t)
{
	char	inbuf[MAX_INPUT_LEN];
	ssize_t     bytes_read;

	bytes_read = perform_socket_read(t->descriptor, inbuf, MAX_INPUT_LEN);

	if (bytes_read < 0)
		return -1;
	else if (bytes_read == 0)
		return 0;

	//    sys_log("DESC #%d receive %d bytes", t->desc_num, bytes_read);
	write_to_q(inbuf, &input_q, bytes_read, t->desc_num);
	return 1;
}


void process_command(void)
{
	static char	inbuf[131072];

	DESC_DATA	*t, *next_t;
	WORD	desc_num;

	char	*buf;
	int		bytes, bytes_proceed;

	buf		= &inbuf[0];

	if (input_bytes_remain > 0)
	{
		bytes = fetch_from_q(&input_q, buf + input_bytes_remain, input_desc_remain);

		if (bytes == 0)
			return;

		bytes += input_bytes_remain;

		for (t = descriptor_list; t; t = next_t)
		{
			next_t = t->next;

			if (t->desc_num == input_desc_remain)
			{
				if (t->input_processor)
				{
					bytes_proceed = ((*t->input_processor) (t, buf, bytes));

					if (bytes_proceed >= bytes)
					{
						input_bytes_remain	= 0;
						input_desc_remain	= 0;
						break;
					}
					else
					{
						if (bytes_proceed > 0)
							memcpy(buf, buf + bytes_proceed, bytes - bytes_proceed);

						input_bytes_remain	= bytes - bytes_proceed;

						sys_log("Transaction unfinished %d, DESC %d, step 2", input_bytes_remain, input_desc_remain);
						// input_desc_remain	= input_desc_remain; // -_-;
						break;
					}
				}
			}
		}

		if (!t)
		{
			input_bytes_remain	= 0;
			input_desc_remain	= 0;
		}

		if (input_bytes_remain != 0)
			return;
	}

	if ((bytes = get_from_q(&input_q, buf, &desc_num)) != 0)
	{
		for (t = descriptor_list; t; t = next_t)
		{
			next_t = t->next;

			if (t->desc_num == desc_num)
			{
				if (t->input_processor)
				{
					bytes_proceed = ((*t->input_processor) (t, buf, bytes));

					if (bytes_proceed >= bytes)
					{
						input_bytes_remain	= 0;
						input_desc_remain	= 0;
						break;
					}
					else
					{
						if (bytes_proceed > 0)
							memcpy(buf, buf + bytes_proceed, bytes - bytes_proceed);

						input_bytes_remain	= bytes - bytes_proceed;
						input_desc_remain	= desc_num;

						sys_log("Transaction Unfinished %d, DESC %d, step 1", input_bytes_remain, input_desc_remain);
						break;
					}
				}
			}
		}

		if (!t)
		{
			input_bytes_remain	= 0;
			input_desc_remain	= 0;
		}
	}
	else
		return;
}


int process_output(DESC_DATA * d)
{
	int result;

	if (d->packet_buffer.length == 0)
		return 0;

	result = write_to_descriptor(d->descriptor, d->packet_buffer.data, d->packet_buffer.length);
	d->packet_buffer.length = 0;

	return (result);
}
