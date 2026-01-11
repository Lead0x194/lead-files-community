/*********************************************************************
 * date        : 2007.05.22
 * file        : sql_account.c
 * author      : mhh
 * description : 
 */

#define _sql_account_c_

#include "structs.h"
#include "utils.h"
#include "socket.h"
#include "config.h"
#include "log.h"
#include "sql.h"
#include "sql_account.h"


int account_get_login(DWORD account_id, CONN_DATA *conn)
{
	snprintf(query_buf, MAX_QUERY_LENGTH,
			"SELECT "
			"login "
			"FROM %s WHERE id='%u' LIMIT 1",
			sql_account_table,
			account_id);

	if (sql_account_query(query_buf)<0)
	{
		sql_error(sql_account);
		return FALSE;
	}

	MYSQL_RES	*result = NULL;
	MYSQL_ROW	row;
	char		ret;

	if (!(result = sql_result(sql_account)))
	{
		sql_error(sql_account);
		return FALSE;
	}

	if (mysql_num_rows(result)<=0)
		ret = FALSE;
	else
		ret = TRUE;

	if (FALSE==ret)
	{
		sql_free_result(result);
		return FALSE;
	}

	if (!(row = sql_fetch_row(result)))
	{
		sql_free_result(result);
		return FALSE;
	}

	char	login[MAX_LOGIN_LEN+1] = {0};
	lower_string( row[0] , login , MAX_LOGIN_LEN ) ;

	sql_free_result(result);

	GET_ID(conn)		= account_id;
	STRNCPY(GET_LOGIN(conn), login, MAX_LOGIN_LEN);
	GET_TEENAGE(conn)	= NO;
	GET_ON_TIME(conn)	= 0;
	GET_OFF_TIME(conn)	= 0;

	return TRUE;
} /* end of account_login() */


static
MYSQL_RES*	fetch_teen_info(DWORD account_id)
{
	snprintf(query_buf, MAX_QUERY_LENGTH,
			"SELECT "
			"teenage, "			// 청소년인가?
			"on_time, "		// 누적 온라인시간
			"off_time, "	// 누적 오프라인시간
			"UNIX_TIMESTAMP(NOW()) - UNIX_TIMESTAMP(logout_time) " // 오프라인 시간 = (현재시간 - 로그아웃시간)
			"FROM %s WHERE account_id='%u' LIMIT 1",
			sql_teenage_table,
			account_id);

	if (sql_account_query(query_buf)<0)
	{
		sql_error(sql_account);
		return NULL;
	}

	MYSQL_RES *result = sql_result(sql_account);
	if (!result)
		return NULL;

	if (mysql_num_rows(result)<=0)
	{
		sql_free_result(result);
		return NULL;
	}

	return result;
}


int account_login(DWORD account_id, CONN_DATA *conn)
{
	if (FALSE == account_get_login(account_id, conn))
		return FALSE;

	MYSQL_RES	*result = fetch_teen_info(account_id);
	if (!result)
		return FALSE;

	int ret = (mysql_num_rows(result) > 0) ? TRUE : FALSE;
	if (FALSE == ret)
	{
		sql_free_result(result);
		return TRUE;
	}

	MYSQL_ROW	row = sql_fetch_row(result);;
	if (!row)
	{
		sql_free_result(result);
		return FALSE;
	}

	int		teenage		= strtol(row[0], NULL, 10);
	int		on_time		= strtol(row[1], NULL, 10);
	int		off_time	= strtol(row[2], NULL, 10);
	int		logout_time	= strtol(row[3], NULL, 10);

	if (logout_time<0)
		logout_time = 0;

	sql_free_result(result);

	GET_TEENAGE(conn)	= teenage ? YES : NO;
	GET_ON_TIME(conn)	= on_time;
	GET_OFF_TIME(conn)	= off_time + logout_time;


	// 누적 오프라인시간이 5시간 이상이면 온라인시간은 0
	if (GET_OFF_TIME(conn) >= HOUR_TO_SEC(5))
	{
		GET_ON_TIME(conn)	= 0;
		GET_OFF_TIME(conn)	= 0;
	}
	else if (GET_OFF_TIME(conn)<0)
		GET_OFF_TIME(conn) = 0;

	// update on_time, off_time
	if (YES==GET_TEENAGE(conn))
	{
		snprintf(query_buf, MAX_QUERY_LENGTH,
				"UPDATE %s SET on_time=%d, off_time=%d "
				"WHERE account_id=%d",
				sql_teenage_table,
				GET_ON_TIME(conn), GET_OFF_TIME(conn), GET_ID(conn));

		sql_async_query(sql_account, query_buf);
	}

	return TRUE;
} /* end of account_login() */


static
int insert_teen_info(DWORD account_id, const char *login)
{
	snprintf(query_buf, MAX_QUERY_LENGTH,
			"INSERT INTO %s "
			"(account_id, login, teenage) values(%u, '%s', 1)",
			sql_teenage_table,
			account_id,
			login);

	if (sql_account_query(query_buf)<0)
	{
		sql_error(sql_account);
		return FALSE;
	}

	return TRUE;
}

int account_login_teen_all(DWORD account_id, CONN_DATA *conn)
{
	if (FALSE == account_get_login(account_id, conn))
		return FALSE;

	MYSQL_RES	*result = fetch_teen_info(account_id);

	if (NULL == result)
	{
		// insert new teenage_info
		insert_teen_info(account_id, GET_LOGIN(conn));

		result = fetch_teen_info(account_id);
	}

	MYSQL_ROW	row = sql_fetch_row(result);;
	if (!row)
	{
		sql_free_result(result);
		return FALSE;
	}

	int		teenage		= strtol(row[0], NULL, 10);
	int		on_time		= strtol(row[1], NULL, 10);
	int		off_time	= strtol(row[2], NULL, 10);
	int		logout_time	= strtol(row[3], NULL, 10);

	if (logout_time<0)
		logout_time = 0;

	sql_free_result(result);

	GET_TEENAGE(conn)	= teenage ? YES : NO;
	GET_ON_TIME(conn)	= on_time;
	GET_OFF_TIME(conn)	= off_time + logout_time;


	// 누적 오프라인시간이 5시간 이상이면 온라인시간은 0
	if (GET_OFF_TIME(conn) >= HOUR_TO_SEC(5))
	{
		GET_ON_TIME(conn)	= 0;
		GET_OFF_TIME(conn)	= 0;
	}
	else if (GET_OFF_TIME(conn)<0)
		GET_OFF_TIME(conn) = 0;

	// update on_time, off_time
	if (YES==GET_TEENAGE(conn))
	{
		snprintf(query_buf, MAX_QUERY_LENGTH,
				"UPDATE %s SET on_time=%d, off_time=%d "
				"WHERE account_id=%d",
				sql_teenage_table,
				GET_ON_TIME(conn), GET_OFF_TIME(conn), GET_ID(conn));

		sql_async_query(sql_account, query_buf);
	}

	return TRUE;
} /* end of account_login() */




int account_logout(CONN_DATA *conn)
{
	// update on_time, logout_time
	snprintf(query_buf, MAX_QUERY_LENGTH,
			"UPDATE %s SET on_time=%d, logout_time=NOW() "
			"WHERE account_id=%u",
			sql_teenage_table, GET_ON_TIME(conn),
			GET_ID(conn));
	sql_async_query(sql_account, query_buf);
	return TRUE;
} /* end of account_login() */

