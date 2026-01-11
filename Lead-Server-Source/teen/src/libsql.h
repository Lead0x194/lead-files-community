#ifndef __INC_LIBSQL_H__
#define __INC_LIBSQL_H__

#include <mysql/mysql.h>

typedef struct TSQL
{
	MYSQL	sql_conn;

	char	addr[64+1];
	char	user[64+1];
	char	password[64+1];
	char	dbname[64+1];

	int		async_query_pipe[2];
} SQL;

extern SQL *		sql_create(const char *addr, const char *user, const char *password, const char *db_name);
extern void		sql_destroy(SQL **);
extern int		sql_async_start(SQL *);
extern int		sql_query(SQL *, const char *);
extern int		sql_async_query(SQL *, const char *);
extern char *		sql_string(SQL *, char *);
extern MYSQL_RES *	sql_result(SQL *);
extern MYSQL_ROW	sql_fetch_row(MYSQL_RES *);
extern void		sql_free_result(MYSQL_RES *);
extern int		sql_num_rows(MYSQL_RES *);
extern int		sql_affected_rows(SQL *);

#define sql_error(sql)	_sql_error(sql, __FILE__, __LINE__, __FUNCTION__)
void			_sql_error(SQL * sql, const char *file, const int line, const char *function);

extern char	char_set[256];


#endif
