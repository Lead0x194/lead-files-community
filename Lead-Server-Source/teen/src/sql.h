#ifndef __INC_METIN_I_SQL_H__
#define __INC_METIN_I_SQL_H__

#include "libsql.h"

#ifndef FALSE
#define FALSE	0
#define TRUE	(!FALSE)
#endif


#define MAX_QUERY_LENGTH	1024
extern char	query_buf[MAX_QUERY_LENGTH+1];


extern SQL *	sql_account;

int	     	sql_init();
void		sql_deinit();

int		sql_account_query(const char *buf);

void		sql_ping();

#endif
