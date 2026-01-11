#ifndef _LOG_H_
#define _LOG_H_

extern void log_init(void);
extern void log_destroy(void);
extern void log_rotate(void);

#define sys_err(fmt, args...)	_sys_err(__FUNCTION__, __LINE__, fmt, ##args)
extern void _sys_err(const char *func, int line, const char *format, ...);
extern void sys_log_header(const char *header);
extern void sys_log(const char *format, ...);
extern void pt_log(const char *format, ...);

#endif	// _LOG_H_
