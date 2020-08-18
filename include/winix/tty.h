#ifndef _W_TTY_H_
#define _W_TTY_H_ 1

#include <sys/types.h>
#include <winix/rex.h>

#define TTY_BUFFER_SIZ  (64)

struct tty_state{
    struct device* dev;
    RexSp_t* rex;
    char *bptr, *buffer_end, *read_ptr;
    struct proc* reader;
    char *read_data;
    size_t read_count;
    char buffer[TTY_BUFFER_SIZ];
    pid_t foreground_group;
    pid_t controlling_session;
    bool is_echoing;
};

#endif

