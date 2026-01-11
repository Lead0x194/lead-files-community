#include "structs.h"
#include "utils.h"
#include "log.h"

BYTE	base_gid1, base_gid2;
WORD	mother_port = 10077;

char sql_addr[256]		= {0};
char sql_user[256]		= {0};
char sql_password[256]		= {0};
char sql_db_name[256]		= {0};
char sql_teenage_table[256]		= {0};
char sql_account_table[256]		= {0};
char char_set[256]		= {0};
int  locale_index		= 0;
int	teen_all_flag		= 0;


void init_config(void)
{
	FILE	*fp;

	char	buf[256];
	char	token_string[256];
	char	value_string[256];

	if (!(fp = fopen("CONFIG", "r")))
	{
		sys_log("Can not open CONFIG");
		exit(1);
	}

	while (fgets(buf, 256, fp))
	{
		parse_token(buf, token_string, value_string);

		TOKEN("sql_addr")
			strncpy(sql_addr, value_string, 256);
		TOKEN("sql_user")
			strncpy(sql_user, value_string, 256);
		TOKEN("sql_password")
			strncpy(sql_password, value_string, 256);
		TOKEN("sql_db_name")
			strncpy(sql_db_name, value_string, 256);
		TOKEN("sql_teenage_table")
			strncpy(sql_teenage_table, value_string, 256);
		TOKEN("sql_account_table")
			strncpy(sql_account_table, value_string, 256);
		TOKEN("port")
			mother_port	= atoi(value_string);
		TOKEN("char_set")
			strncpy(char_set, value_string, 256);
		TOKEN("teen_all")
			teen_all_flag = atoi(value_string);
	}

	// check arg
#define FAIL_EXIT(val)				\
	if (val[0]==0)				\
	{						\
		sys_err("FAIL -> open %s", #val);	\
			exit(1);				\
	}

	FAIL_EXIT(sql_addr);
	FAIL_EXIT(sql_user);
	FAIL_EXIT(sql_password);
	FAIL_EXIT(sql_db_name);
	FAIL_EXIT(sql_teenage_table);
#undef FAIL_EXIT

	fclose(fp);
}
