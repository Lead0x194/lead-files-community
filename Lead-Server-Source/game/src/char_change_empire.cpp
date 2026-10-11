
#include "stdafx.h"
#include "config.h"
#include "char.h"
#include "char_manager.h"
#include "db.h"
#include "guild_manager.h"
#include "marriage.h"

/*
   Return Value
		0 : unknown error or query error
		1 : Trying to change it to the same empire
		2 : There is a character who has joined a guild
		3 : There is a married character

		999 : Successful movement of empire
*/
int CHARACTER::ChangeEmpire(BYTE empire)
{
	if (GetEmpire() == empire)
		return 1;

	char szQuery[1024+1];
	DWORD dwAID;
	DWORD dwPID[4];
	memset(dwPID, 0, sizeof(dwPID));

	{
		// 1. everything in my account pid get it
		snprintf(szQuery, sizeof(szQuery), 
				"SELECT id, pid1, pid2, pid3, pid4 FROM player_index%s WHERE (pid1=%u OR pid2=%u OR pid3=%u OR pid4=%u) AND empire=%u",
				get_table_postfix(), GetPlayerID(), GetPlayerID(), GetPlayerID(), GetPlayerID(), GetEmpire());

		std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery(szQuery));

		if (msg->Get()->uiNumRows == 0)
		{
			return 0;
		}

		MYSQL_ROW row = mysql_fetch_row(msg->Get()->pSQLResult);

		str_to_number(dwAID, row[0]);
		str_to_number(dwPID[0], row[1]);
		str_to_number(dwPID[1], row[2]);
		str_to_number(dwPID[2], row[3]);
		str_to_number(dwPID[3], row[4]);
	}

	const int loop = 4;

	{
		// 2. Retrieves guild information for each character .
		//   If at least one character is in a guild , Empire cannot be moved .
		DWORD dwGuildID[4];
		CGuild * pGuild[4];
		SQLMsg * pMsg = NULL;
		
		for (int i = 0; i < loop; ++i)
		{
			snprintf(szQuery, sizeof(szQuery), "SELECT guild_id FROM guild_member%s WHERE pid=%u", get_table_postfix(), dwPID[i]);

			pMsg = DBManager::instance().DirectQuery(szQuery);

			if (pMsg != NULL)
			{
				if (pMsg->Get()->uiNumRows > 0)
				{
					MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);

					str_to_number(dwGuildID[i], row[0]);

					pGuild[i] = CGuildManager::instance().FindGuild(dwGuildID[i]);

					if (pGuild[i] != NULL)
					{
						M2_DELETE(pMsg);
						return 2;
					}
				}
				else
				{
					dwGuildID[i] = 0;
					pGuild[i] = NULL;
				}

				M2_DELETE(pMsg);
			}
		}
	}

	{
		// 3. Obtain marriage information for each character .
		//   If even one character is married, he or she cannot move to the empire. .
		for (int i = 0; i < loop; ++i)
		{
			if (marriage::CManager::instance().IsEngagedOrMarried(dwPID[i]) == true)
				return 3;
		}
	}
	
	{
		// 4. db Update the empire information of .
		snprintf(szQuery, sizeof(szQuery), "UPDATE player_index%s SET empire=%u WHERE (pid1=%u OR pid2=%u OR pid3=%u OR pid4=%u) AND empire=%u",
				get_table_postfix(), empire, GetPlayerID(), GetPlayerID(), GetPlayerID(), GetPlayerID(), GetEmpire());

		std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery(szQuery));

		if (msg->Get()->uiAffectedRows > 0)
		{
			// 5. Add empire change history .
			SetChangeEmpireCount();

			return 999;
		}
	}

	return 0;
}

int CHARACTER::GetChangeEmpireCount() const
{
	char szQuery[1024+1];
	DWORD dwAID = GetAID();

	if (dwAID == 0)
		return 0;

	snprintf(szQuery, sizeof(szQuery), "SELECT change_count FROM change_empire WHERE account_id = %u", dwAID);

	SQLMsg * pMsg = DBManager::instance().DirectQuery(szQuery);

	if (pMsg != NULL)
	{
		if (pMsg->Get()->uiNumRows == 0)
		{
			M2_DELETE(pMsg);
			return 0;
		}

		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);

		DWORD count = 0;
		str_to_number(count, row[0]);

		M2_DELETE(pMsg);

		return count;
	}

	return 0;
}

void CHARACTER::SetChangeEmpireCount()
{
	char szQuery[1024+1];

	DWORD dwAID = GetAID();

	if (dwAID == 0) return;

	int count = GetChangeEmpireCount();

	if (count == 0)
	{
		count++;
		snprintf(szQuery, sizeof(szQuery), "INSERT INTO change_empire VALUES(%u, %d, NOW())", dwAID, count);
	}
	else
	{
		count++;
		snprintf(szQuery, sizeof(szQuery), "UPDATE change_empire SET change_count=%d WHERE account_id=%u", count, dwAID);
	}

	std::unique_ptr<SQLMsg> pmsg(DBManager::instance().DirectQuery(szQuery));
}

DWORD CHARACTER::GetAID() const
{
	char szQuery[1024+1];
	DWORD dwAID = 0;

	snprintf(szQuery, sizeof(szQuery), "SELECT id FROM player_index%s WHERE (pid1=%u OR pid2=%u OR pid3=%u OR pid4=%u) AND empire=%u",
			get_table_postfix(), GetPlayerID(), GetPlayerID(), GetPlayerID(), GetPlayerID(), GetEmpire());

	SQLMsg* pMsg = DBManager::instance().DirectQuery(szQuery);

	if (pMsg != NULL)
	{
		if (pMsg->Get()->uiNumRows == 0)
		{
			M2_DELETE(pMsg);
			return 0;
		}

		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);

		str_to_number(dwAID, row[0]);

		M2_DELETE(pMsg);

		return dwAID;
	}
	else
	{
		return 0;
	}
}

