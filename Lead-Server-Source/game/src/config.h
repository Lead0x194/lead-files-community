
#ifndef __INC_METIN_II_GAME_CONFIG_H__
#define __INC_METIN_II_GAME_CONFIG_H__

enum
{
	ADDRESS_MAX_LEN = 15
};

void config_init(const std::string& st_localeServiceName); // default "" is CONFIG

extern char sql_addr[256];

extern WORD mother_port;
extern WORD p2p_port;

extern char db_addr[ADDRESS_MAX_LEN + 1];
extern WORD db_port;

extern int passes_per_sec;
extern int save_event_second_cycle;
extern int ping_event_second_cycle;
extern int test_server;
extern bool	guild_mark_server;
extern BYTE guild_mark_min_level;

extern bool	g_bNoMoreClient;
extern bool	g_bNoRegen;

extern BYTE	g_bChannel;

extern bool	map_allow_find(int index);
extern void	map_allow_copy(int32_t * pl, int size);
extern bool	no_wander;

extern int	g_iUserLimit;
extern uint32_t	g_global_time;

const char *	get_table_postfix();

extern std::string	g_stHostname;
extern std::string	g_stDatabaseCharset;

extern char		g_szPublicIP[16];
extern char		g_szInternalIP[16];

int utf8_char_length(const char * str);
int check_name(const char * str);

extern bool		g_bSkillDisable;

extern int		g_iFullUserCount;
extern int		g_iBusyUserCount;
extern void		LoadStateUserCount();

extern bool	g_bEmpireWhisper;

extern BYTE	g_bAuthServer;

extern BYTE	PK_PROTECT_LEVEL;

extern void	LoadValidCRCList();
extern bool	IsValidProcessCRC(DWORD dwCRC);
extern bool	IsValidFileCRC(DWORD dwCRC);

extern std::string	g_stAuthMasterIP;
extern WORD		g_wAuthMasterPort;

extern std::string	g_stQuestDir;
extern std::set<std::string> g_setQuestObjectDir;

extern int	SPEEDHACK_LIMIT_COUNT;
extern int 	SPEEDHACK_LIMIT_BONUS;

extern int g_iSyncHackLimitCount;

extern int g_server_id;

extern int VIEW_RANGE;
extern int VIEW_BONUS_RANGE;

extern bool g_bCheckMultiHack;
extern bool g_protectNormalPlayer;      // criminal " peace mode " Cannot attack regular users

extern DWORD g_GoldDropTimeLimitValue;

extern int gPlayerMaxLevel;

// NEW CONFIGURATION VARIABLES
extern bool g_CreateDocumentationFiles ;
extern BYTE g_PartyGapLevel;
extern int g_GuildCreateFee;
extern int g_DeathExpLossCap;
extern int g_SkillBookExp;
extern ItemStackType g_ItemCountLimit;
// NEW CONFIGURATION VARIABLES

extern bool g_BlockCharCreation;

#endif /* __INC_METIN_II_GAME_CONFIG_H__ */

