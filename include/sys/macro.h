#ifndef _SYS_MACRO_H_
#define _SYS_MACRO_H_

#define MAKEDEV(dmajor, dminor) ((((unsigned int)dmajor << 8) & 0xFF00U) | ((unsigned int)dminor & 0xFFFF00FFU))
#define DEV_MAJOR(devnum)           (((unsigned int)devnum & 0xFF00U) >> 8)
#define DEV_MINOR(devnum)           ((unsigned int)devnum & 0xFFFF00FFU)

#endif


