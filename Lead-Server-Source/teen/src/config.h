#ifndef _CONFIG_H_
#define _CONFIG_H_

void init_config(void);

extern int base_gid1, base_gid2;

extern WORD mother_port;

extern char sql_addr[256];
extern char sql_user[256];
extern char sql_password[256];
extern char sql_db_name[256];
extern char sql_teenage_table[256];
extern char sql_account_table[256];
extern char char_set[256];
extern int  locale_index;

extern int	teen_all_flag;


#endif	// _CONFIG_H_
