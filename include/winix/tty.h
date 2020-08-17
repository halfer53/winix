#ifndef _W_TTY_H_
#define _W_TTY_H_ 1

#include <sys/types.h>

struct tty_private {
    pid_t foreground_group;
    pid_t controlling_session;
};

#endif

