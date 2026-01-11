/*********************************************************************
 * date        : 2007.05.22
 * file        : sql_account.h
 * author      : mhh
 * description : 
 */

#ifndef _sql_account_h_
#define _sql_account_h_

int account_login(DWORD account_id, CONN_DATA *conn);
int account_login_teen_all(DWORD account_id, CONN_DATA *conn);
int account_logout(CONN_DATA *conn);

#endif	/* _sql_account_h_ */

