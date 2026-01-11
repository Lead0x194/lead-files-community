#include <sys/types.h>
#include <dirent.h>
#include <stdarg.h>


#include "structs.h"
#include "utils.h"

typedef struct log_file_s*	LPLOGFILE;
typedef struct log_file_s	LOGFILE;

struct log_file_s
{
	char*	filename;
	FILE*	fp;

	int		last_hour;
	int		last_day;
};

LPLOGFILE	log_file_sys = NULL;
LPLOGFILE	log_file_err = NULL;
LPLOGFILE	log_file_pt = NULL;
static char	log_dir[32] = { 0, };

// Internal functions
LPLOGFILE log_file_init(const char * filename, const char * openmode);
void log_file_destroy(LPLOGFILE logfile);
void log_file_rotate(LPLOGFILE logfile);
void log_file_check(LPLOGFILE logfile);
void log_file_set_dir(const char *dir);


void log_init(void)
{
	log_file_set_dir("./log");

	log_file_sys = log_file_init("syslog", "a+");
	log_file_err = log_file_init("syserr", "a+");
	log_file_pt = log_file_init("PTS", "w");
}


void log_destroy(void)
{
	log_file_destroy(log_file_sys);
	log_file_destroy(log_file_err);
	log_file_destroy(log_file_pt);
}


void log_rotate(void)
{
	log_file_check(log_file_sys);
	log_file_check(log_file_err);
	log_file_check(log_file_pt);

	log_file_rotate(log_file_sys);
}


void _sys_err(const char *func, int line, const char *format, ...)
{
	va_list args;
	time_t ct = time(0);
	char *time_s = asctime(localtime(&ct));

	if (!log_file_err)
		return;

	time_s[strlen(time_s) - 1] = '\0';

	// log_file_err 에 출력
	va_start(args, format);
	fprintf(log_file_err->fp, "%-15.15s :: %s: ", time_s + 4, func);
	vfprintf(log_file_err->fp, format, args);
	va_end(args);
	fputc('\n', log_file_err->fp);
	fflush(log_file_err->fp);

	// log_file_sys 에도 출력
	va_start(args, format);
	fprintf(log_file_sys->fp, "SYSERR: %-15.15s :: %s: ", time_s + 4, func);
	vfprintf(log_file_sys->fp, format, args);
	va_end(args);
	fputc('\n', log_file_sys->fp);
	fflush(log_file_sys->fp);
}


static char sys_log_header_string[33] = { 0, };

void sys_log_header(const char *header)
{
	strncpy(sys_log_header_string, header, 32);
}


void sys_log(const char *format, ...)
{
	va_list	args;

	if (!log_file_sys)
		return;

	fprintf(log_file_sys->fp, sys_log_header_string);

	va_start(args, format);
	vfprintf(log_file_sys->fp, format, args);
	va_end(args);

	fprintf(log_file_sys->fp, "\n");
	fflush(log_file_sys->fp);
}


void pt_log(const char *format, ...)
{
	va_list	args;

	if (!log_file_pt)
		return;

	va_start(args, format);
	vfprintf(log_file_pt->fp, format, args);
	va_end(args);

	fprintf(log_file_pt->fp, "\n");
	fflush(log_file_pt->fp);
}


LPLOGFILE log_file_init(const char* filename, const char * openmode)
{
	LPLOGFILE	logfile;
	FILE*	fp;
	struct tm	curr_tm;
	time_t	time_s;

	time_s = time(0);
	curr_tm = *localtime(&time_s);

	fp = fopen(filename, openmode);

	if (!fp)
		return NULL;

	logfile = (LPLOGFILE) malloc(sizeof(LOGFILE));
	logfile->filename = strdup(filename);
	logfile->fp	= fp;
	logfile->last_hour = curr_tm.tm_hour;
	logfile->last_day = curr_tm.tm_mday;

	return (logfile);
}


void log_file_destroy(LPLOGFILE logfile)
{
	if (logfile->filename)
	{
		free(logfile->filename);
		logfile->filename = NULL;
	}

	if (logfile->fp)
	{
		fclose(logfile->fp);
		logfile->fp = NULL;
	}

	free(logfile);
}


void log_file_check(LPLOGFILE logfile)
{
	struct stat	sb;

	// 파일이 없으므로 다시 연다.
	if (stat(logfile->filename, &sb) != 0 && errno == ENOENT)
	{
		fclose(logfile->fp);
		logfile->fp = fopen(logfile->filename, "a+");
	}
}


void log_file_delete_old(const char *filename)
{
	struct stat		sb;
	struct dirent**	namelist;
	int			n, num1, num2;
	char		buf[32];
	char		system_cmd[64];
	struct tm		new_tm;

	stat(filename, &sb);

	if (!S_ISDIR(sb.st_mode))
		return;

	new_tm = *tm_calc(NULL, -3);
	n = scandir(filename, &namelist, 0, alphasort);

	sprintf(buf, "%04d%02d%02d", new_tm.tm_year + 1900, new_tm.tm_mon + 1, new_tm.tm_mday);
	num1 = atoi(buf);

	if (n < 0)
		perror("scandir");
	else
	{
		while (n-- > 0)
		{
			if (namelist[n]->d_name[0] == '.')
				continue;

			if (!isdigit(*namelist[n]->d_name))
				continue;

			num2 = atoi(namelist[n]->d_name);

			if (num2 <= num1)
			{
				sprintf(system_cmd, "rm -rf %s/%s", filename, namelist[n]->d_name);
				system(system_cmd);

				sys_log("%s: SYSTEM_CMD: %s", __FUNCTION__, system_cmd);
			}
		}
	}

	free(namelist);
}


void log_file_rotate(LPLOGFILE logfile)
{
	struct tm	curr_tm;
	time_t	time_s;
	char	dir[128];
	char	system_cmd[128];

	time_s = time(0);
	curr_tm = *localtime(&time_s);

	if (curr_tm.tm_mday != logfile->last_day)
	{
		struct tm new_tm;
		new_tm = *tm_calc(&curr_tm, -3);

		sprintf(system_cmd, "rm -rf %s/%04d%02d%02d", log_dir, new_tm.tm_year + 1900, new_tm.tm_mon + 1, new_tm.tm_mday);
		system(system_cmd);

		sys_log("%s: SYSTEM_CMD: %s", __FUNCTION__, system_cmd);

		logfile->last_day = curr_tm.tm_mday;
	}

	if (curr_tm.tm_hour != logfile->last_hour)
	{
		struct stat	stat_buf;

		snprintf(dir, 128, "%s/%04d%02d%02d", log_dir, curr_tm.tm_year + 1900, curr_tm.tm_mon + 1, curr_tm.tm_mday);

		if (stat(dir, &stat_buf) != 0 || S_ISDIR(stat_buf.st_mode))
			mkdir(dir, S_IRWXU);

		sys_log("SYSTEM: LOG ROTATE (%04d-%02d-%02d %d)", 
				curr_tm.tm_year + 1900, curr_tm.tm_mon + 1, curr_tm.tm_mday, logfile->last_hour);

		// 로그 파일을 닫고
		fclose(logfile->fp);

		// 옮긴다.
		snprintf(system_cmd, 128, "mv %s %s/%s.%02d", logfile->filename, dir, logfile->filename, logfile->last_hour);
		system(system_cmd);

		// 마지막 저장시간 저장
		logfile->last_hour = curr_tm.tm_hour;

		// 로그 파일을 다시 연다.	
		logfile->fp = fopen(logfile->filename, "a+");
	}
}


void log_file_set_dir(const char *dir)
{
	strcpy(log_dir, dir);
	log_file_delete_old(log_dir);
}
