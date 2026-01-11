#ifndef __LAUNCHER_DEF_H__
#define __LAUNCHER_DEF_H__

#ifndef IS_SET
#define IS_SET(flag,bit)                ((flag) & (bit))
#endif

#ifndef SET_BIT
#define SET_BIT(var,bit)                ((var) |= (bit))
#endif

#ifndef REMOVE_BIT
#define REMOVE_BIT(var,bit)             ((var) &= ~(bit))
#endif

enum ECmdFlag
{
	NO_DATA_PATCH = (1 << 0),
	AUTO_START = (1 << 1),
	CLIENT_PAUSE = (1 << 2),
};

#endif