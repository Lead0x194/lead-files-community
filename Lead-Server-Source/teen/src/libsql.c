#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <zlib.h>

#include "libsql.h"
#include "tellwait.h"
#include "log.h"


extern void clean_server();

// internal functions
void		sql_async_loop(SQL * sql);
static void	gzlog_rotate(const char *basename, int count);

SQL * sql_create(const char *addr, const char *user, const char *password, const char *dbname)
{
	SQL * sql;

	sql = (SQL *) calloc(1, sizeof(SQL));
	sql->async_query_pipe[0] = -1;
	sql->async_query_pipe[1] = -1;

	mysql_init(&sql->sql_conn);

	if (char_set[0])
		mysql_options(&sql->sql_conn, MYSQL_SET_CHARSET_NAME, char_set);


	strncpy(sql->addr, addr, 64);
	sql->addr[64] = '\0';

	strncpy(sql->user, user, 64);
	sql->user[64] = '\0';

	strncpy(sql->password, password, 64);
	sql->password[64] = '\0';

	strncpy(sql->dbname, dbname, 64);
	sql->dbname[64] = '\0';

	sys_log("SQL: trying to connect %s@%s db %s", sql->user, sql->addr, sql->dbname);

	if (!mysql_real_connect(&sql->sql_conn, sql->addr, sql->user, sql->password, sql->dbname, 0, NULL, 0))
	{
		sql_error(sql);
		free(sql);
		return NULL;
	}

	return (sql);
}

SQL * sql_clone(SQL * sql)
{
	SQL * new_sql;

	if (!(new_sql = sql_create(sql->addr, sql->user, sql->password, sql->dbname)))
		return NULL;

	new_sql->async_query_pipe[0] = sql->async_query_pipe[0];
	new_sql->async_query_pipe[1] = sql->async_query_pipe[1];
	return (new_sql);
}

int sql_async_start(SQL * sql)
{
	int pid;

	if (pipe(sql->async_query_pipe) < 0)
	{
		perror("pipe");
		return 0;
	}

	TELL_WAIT();

	if ((pid = fork()) < 0)
	{
		perror("fork");
		return 0;
	}

	if (pid > 0) // parent
	{
		close(sql->async_query_pipe[0]); // 읽기는 필요 없다.
		sql->async_query_pipe[0] = -1;

		WAIT_CHILD();
	}
	else // child
	{
		SQL * new_sql;

		close(sql->async_query_pipe[1]); // 쓰기는 필요 없다
		sql->async_query_pipe[1] = -1;

		new_sql = sql_clone(sql);

		clean_server();

		if (!new_sql)
			exit(1);

		TELL_PARENT(getppid());
		sql_async_loop(new_sql);

		sql_destroy(&new_sql);
		exit(1);
	}

	return 1;
}

void sql_async_loop(SQL * sql)
{
	gzFile	fp;
	int		len;
	char	filename[256], basename[256];
	char	buf[65535];
	char	query[1024+1];
	int		lines = 0;

	const char	* null;
	const char	* start;

	mkdir("async_query", S_IRWXU);

	sprintf(basename, "async_query/%s:%s", sql->addr, sql->dbname);
	sprintf(filename, "%s.gz", basename);

	fp = gzopen(filename, "w");

	if (!fp)
	{
		fprintf(stderr, "SYSERR: sql_async_loop: gzopen: cannot open %s", filename);
		return;
	}

	while (1)
	{
		len = read(sql->async_query_pipe[0], buf, 65535);

		if (len <= 0) // 부모 프로세스가 종료 했거나 에러면 루프를 탈출 한다.
			break;

		//fprintf(stderr, "READ: %d ", len);
		null = buf;
		start = buf;

		while (len > 0 && (null = (const char*)memchr(null, '\0', len)))
		{
			snprintf(query, 1024, "%s", start);
			++null;

			len	-= (null - start);
			start = null;

			//fprintf(stderr, "LEN: %d ", len);
			//sys_log("ASYNC_QUERY: %s;\n", query);

			if (sql_query(sql, query) < 0)
				sql_error(sql);

			if (-1 == gzputs(fp, query))
				fprintf(stderr, "SYSERR: gzputs: %s\n", gzerror(fp, NULL));

			if (-1 == gzputs(fp, ";\n"))
				fprintf(stderr, "SYSERR: gzputs: %s\n", gzerror(fp, NULL));

			//gzflush(fp, Z_FINISH);
			++lines;
		}
	}

	gzclose(fp);

	if (0 == lines)
	{
		sprintf(filename, "%s.gz", basename);
		unlink(filename);
	}
	else
		gzlog_rotate(basename, 9);
}

void sql_destroy(SQL ** r_sql)
{
	SQL * sql;

	if (!r_sql)
		return;

	sql = *r_sql;

	if (!sql)
		return;

	if (sql->async_query_pipe[0] != -1)
		sql->async_query_pipe[0] = 0;

	if (sql->async_query_pipe[1] != -1)
		sql->async_query_pipe[1] = 0;

	mysql_close(&sql->sql_conn);
	*r_sql = NULL;
}

int sql_query(SQL * sql, const char *buf)
{
	if (!sql)
		return 0;

	//    fprintf(stderr, "SQL_INFO : P     (%x)\n", sql);
	//    fprintf(stderr, "SQL_INFO : addr  (%s)\n", sql->addr);
	//    fprintf(stderr, "SQL_INFO : user  (%s)\n", sql->user);
	//    fprintf(stderr, "SQL_INFO : query (%s)\n", buf);
	//    fprintf(stderr, "SQL_INFO : dbname(%s)\n", sql->dbname);

	return (mysql_query(&sql->sql_conn, buf));
}

int sql_async_query(SQL * sql, const char *buf)
{
	if (!sql)
		return 0;

	if (sql->async_query_pipe[1] == -1)
		return 0;

	if (write(sql->async_query_pipe[1], buf, strlen(buf) + 1) < 0)
	{
		perror("write");
		return 0;
	}

	return 1;
}

MYSQL_RES * sql_result(SQL * sql)
{
	return mysql_store_result(&sql->sql_conn);
}

char *sql_string(SQL * sql, char *buf)
{
	static char qbuf[4096+256];
	int len;

	len = strlen(buf);

	if (len >= 2048)
	{
		sys_err("sql_string: query buffer length overflow: %d\n"
				"            %s", len, buf);
		exit(1);
	}

	mysql_real_escape_string(&sql->sql_conn, qbuf, buf, len);
	return (qbuf);
}

int sql_affected_rows(SQL * sql)
{
	return mysql_affected_rows(&sql->sql_conn);
}

int sql_num_rows(MYSQL_RES * res)
{
	return mysql_num_rows(res);
}

MYSQL_ROW sql_fetch_row(MYSQL_RES * res)
{
	return mysql_fetch_row(res);
}

void sql_free_result(MYSQL_RES * res)
{
	if (!res)
		return;

	mysql_free_result(res);
}

void _sql_error(SQL * sql, const char *file, const int line, const char *function)
{
	fprintf(stderr, "SQL: %s:%d:%s() | %s\n", file, line, function, mysql_error(&sql->sql_conn));
	fflush(stderr);
}   

static void gzlog_rotate(const char *basename, int count)
{
	char filename[256], filename2[256];
	int i;

	sprintf(filename, "%s.1.gz", basename);
	unlink(filename);

	for (i = 2; i <= count; ++i)
	{
		sprintf(filename2, "%s.%d.gz", basename, i);
		rename(filename2, filename);
		strcpy(filename, filename2);
	}

	sprintf(filename, "%s.gz", basename);
	rename(filename, filename2);
}

