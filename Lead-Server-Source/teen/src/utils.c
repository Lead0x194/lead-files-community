#include "structs.h"
#include "utils.h"
#include "db.h"

#include <stdarg.h>
extern struct timeval null_time;

#define ishprint(x)     ((((x) & 0xE0) > 0x90) || isprint(x))

void printdata(unsigned char *data, int bytes)
{
	int                 i, j, k;
	unsigned char       *p;

	fprintf(stderr, "------------------------------------------------------------------\n");
	j = bytes;
	while (1)
	{
		k       = j >= 16 ? 16 : j;

		p       = data;
		for (i = 0; i < 16; i++)
		{
			if (i >= k)
				fprintf(stderr, "   ");
			else
				fprintf(stderr, "%02x ", *p);
			p++;
		}

		fprintf(stderr, "| ");

		p       = data;
		for (i = 0; i < k; i++)
		{
			if (i >= k)
				fprintf(stderr, " ");
			else
				fprintf(stderr, "%c", ishprint(*p) ? *p : '.');
			p++;
		}

		fprintf(stderr, "\n");

		j       -= 16;
		data    += 16;

		if (j <= 0)
			break;
	}

	fprintf(stderr, "------------------------------------------------------------------\n");
}


struct timeval *timediff(struct timeval *a, struct timeval *b)
{
	static struct timeval rslt;

	if (a->tv_sec < b->tv_sec)
		return &null_time;
	else if (a->tv_sec == b->tv_sec)
	{
		if (a->tv_usec < b->tv_usec)
			return &null_time;
		else
		{
			rslt.tv_sec         = 0;
			rslt.tv_usec        = a->tv_usec - b->tv_usec;
			return &rslt;
		}
	}
	else
	{                      /* a->tv_sec > b->tv_sec */
		rslt.tv_sec = a->tv_sec - b->tv_sec;

		if (a->tv_usec < b->tv_usec)
		{
			rslt.tv_usec = a->tv_usec + 1000000 - b->tv_usec;
			rslt.tv_sec--;
		} else
			rslt.tv_usec = a->tv_usec - b->tv_usec;

		return &rslt;
	}
}

struct timeval *timeadd(struct timeval *a, struct timeval *b)
{
	static struct timeval rslt;

	rslt.tv_sec         = a->tv_sec     +       b->tv_sec;
	rslt.tv_usec        = a->tv_usec    +       b->tv_usec;

	while (rslt.tv_usec >= 1000000)
	{
		rslt.tv_usec -= 1000000;
		rslt.tv_sec++;
	}

	return &rslt;
}


char *time_str(time_t ct)
{
	static char *time_s;

	time_s      = asctime(localtime(&ct));

	time_s[strlen(time_s) - 6] = '\0';
	time_s      += 4;

	return (time_s);
}



void lower_string(char *src, char *dest, int len)
{
	char        *tmp;
	int         i;

	for (i = 0, tmp = src; *tmp && i < len; tmp++, i++)
		*(dest++) = LOWER(*tmp);

	// 뒤 부분을 널로 채운다.
	for (; i < len; i++)
		*(dest++) = '\0';
}

char *str_dup(const char *source)
{
	char *new_line;

	CREATE(new_line, char, strlen(source) + 1);
	return (strcpy(new_line, source));
}


int filesize(FILE *fp)
{
	int pos;
	int size;

	pos = ftell(fp);

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, pos, SEEK_SET);

	return (size);
}


/* "Name : 비엽" 과 같이 "항목 : 값" 으로 이루어진 문자열에서 
   항목을 token 으로, 값을 value 로 복사하여 리턴한다. */
void parse_token(char *src, char *token, char *value)
{
	char *tmp;

	for (tmp = src; *tmp && *tmp != ':'; tmp++)
	{
		if (isnhspace(*tmp))
			continue;

		*(token++) = LOWER(*tmp);
	}

	*token = '\0';

	for (tmp += 2; *tmp; tmp++)
	{
		if (*tmp == '\n' || *tmp == '\r')
			continue;

		*(value++) = *tmp;
	}   

	*value = '\0';
}   


struct tm *tm_calc(const struct tm *curr_tm, int days)
{
	char		yoon = FALSE;
	static struct tm	new_tm;
	int			monthdays[12] = { 31, 28, 31, 30, 31, 30, 31, 30, 31, 30, 31 };

	if (!curr_tm)
	{
		time_t time_s = time(0);
		new_tm = *localtime(&time_s);
	}
	else
		memcpy(&new_tm, curr_tm, sizeof(struct tm));

	if (new_tm.tm_mon == 1)
	{
		if (!((new_tm.tm_year + 1900) % 4))
		{
			if (!((new_tm.tm_year + 1900) % 100))
			{
				if (!((new_tm.tm_year + 1900) % 400))
					yoon = TRUE;
			}
			else
				yoon = TRUE;
		}

		if (yoon)
			new_tm.tm_mday += 1;
	}

	if (yoon)
		monthdays[1] = 29;
	else
		monthdays[1] = 28;

	new_tm.tm_mday += days;

	if (new_tm.tm_mday <= 0)
	{
		new_tm.tm_mon--;

		if (new_tm.tm_mon < 0)
		{
			new_tm.tm_year--;
			new_tm.tm_mon = 11;
		}

		new_tm.tm_mday = monthdays[new_tm.tm_mon] + new_tm.tm_mday;
	}
	else if (new_tm.tm_mday > monthdays[new_tm.tm_mon])
	{
		new_tm.tm_mon++;

		if (new_tm.tm_mon > 11)
		{
			new_tm.tm_year++;
			new_tm.tm_mon = 0;
		}

		new_tm.tm_mday = monthdays[new_tm.tm_mon] - new_tm.tm_mday;
	}

	return (&new_tm);
}
