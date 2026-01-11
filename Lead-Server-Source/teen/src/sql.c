#include "structs.h"
#include "utils.h"
#include "socket.h"
#include "config.h"
#include "sql.h"
#include "log.h"

SQL *	sql_account		= NULL;

char	query_buf[MAX_QUERY_LENGTH+1] = { 0, };

int sql_init()
{
	sql_account = sql_create(sql_addr, sql_user, sql_password, sql_db_name);

	if (!sql_account)
	{
		sys_err("cannot create sql_account");
		return 0;
	}


	if (!sql_async_start(sql_account))
	{
		sql_deinit();
		return 0;
	}

	return 1;
}

void sql_deinit()
{
	sql_destroy(&sql_account);
}

int sql_account_query(const char *buf)
{
	return sql_query(sql_account, buf);
}

void sql_ping()
{
	if (sql_account)
		mysql_ping(&sql_account->sql_conn);
}

