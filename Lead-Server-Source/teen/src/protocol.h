#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

//#define RECV_LOGIN	0x10
//#define RECV_LOGOUT	0x11
//
//#define SEND_TEEN_NOTICE	0x12
//#define SEND_FORCE_LOGOUT	0x13
//#define SEND_LOGIN_NOTICE	0x14




inline BYTE decode_byte(const BYTE *a)
{
	return (*a);
}

inline WORD decode_2bytes(const BYTE *a)
{
	return (*((WORD *) a));
}

inline INT decode_4bytes(const BYTE *a)
{
	return (*((INT *) a));
}
#endif
