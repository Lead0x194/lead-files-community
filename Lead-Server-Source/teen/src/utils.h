#ifndef _utils_h_chat_
#define _utils_h_chat_

#define FALSE	0
#define TRUE	(!FALSE)

#define NO	FALSE
#define YES	TRUE

#define LOWER(c)                (((c)>='A'  && (c) <= 'Z') ? ((c)+('a'-'A')) : (c))
#define UPPER(c)                (((c)>='a'  && (c) <= 'z') ? ((c)+('A'-'a')) : (c))

void lower_string(char *src, char *dest, int len);

#define ishan(ch) (((ch) & 0xE0) > 0x90)
#define ishanasc(ch) (isascii(ch) || ishan(ch))
#define ishanalp(ch) (isalpha(ch) || ishan(ch))
#define isnhdigit(ch) (!ishan(ch) && isdigit(ch))
#define isnhspace(ch) (!ishan(ch) && isspace(ch))

extern struct timeval *timediff(struct timeval *a, struct timeval *b);
extern struct timeval *timeadd(struct timeval *a, struct timeval *b);
struct tm *		tm_calc(const struct tm *curr_tm, int days);


extern char *first_han(unsigned char *str);
extern int check_han(char *str);
//extern int is_hangul(unsigned char *str);
extern int under_han(unsigned char *str);

#define str_cmp strcasecmp

extern char *str_dup(const char *source);
extern int get_filename(char *orig_name, char *fname, BYTE type);
//extern void sys_log(const char *format, ...);
//extern void pt_log(const char *format, ...);
extern void printdata(unsigned char *data, int bytes);
extern int filesize(FILE *fp);

#define TOKEN(string)   if (!str_cmp(token_string, string))
extern void parse_token(char *src, char *token, char *value);

#define SAFE_FREE(p)	do {	\
		if (p)					\
		{						\
			free(p);			\
			(p) = NULL;			\
		}						\
	} while(0)

#define CREATE(result, type, number)  do { 			\
    if (!((result) = (type *) calloc ((number), sizeof(type)))) \
    { perror("malloc failure"); abort(); } } while(0)

    // Next 와 Prev 가 있는 리스트에 추가
#define INSERT_TO_TW_LIST(item, head, prev, next)       \
    if (!(head))                                    \
{                                               \
    head         = item;                        \
	(head)->prev = (head)->next = NULL;         \
}                                               \
else                                            \
{                                               \
    (head)->prev = item;                        \
	(item)->next = head;                        \
	(item)->prev = NULL;                        \
	head         = item;                        \
}

#define REMOVE_FROM_TW_LIST(item, head, prev, next)     \
if ((item) == (head))                           \
{                                               \
    if (((head) = (item)->next))                \
	(head)->prev = NULL;                    \
}                                               \
else                                            \
{                                               \
    if ((item)->next)                           \
	(item)->next->prev = (item)->prev;      \
	    if ((item)->prev)                           \
		(item)->prev->next = (item)->next;      \
}


#define INSERT_TO_LIST(item, head, next)                \
	(item)->next = (head);                          \
	(head) = (item);                                \
			
#define REMOVE_FROM_LIST(item, head, next)              \
	if ((item) == (head))                           \
	   head = (item)->next;                         \
	else                                            \
	{                                               \
	    temp = head;                                \
							\
	    while (temp && (temp->next != (item)))      \
		temp = temp->next;                      \
		    					\
	    if (temp)                                   \
		temp->next = (item)->next;              \
	}

#define STRNCPY(dest,src,len) do {	\
		strncpy(dest, src, len);	\
		dest[len] = 0;				\
	} while(0)

#endif // _utils_h_chat_

