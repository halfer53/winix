
#ifndef _SHELL_H_
#define _SHELL_H_ 1

#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/statfs.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stddef.h>
#include <errno.h>
#include <ucontext.h>
#include <sys/wait.h>
#include <sys/times.h>
#include <bsd/string.h>
#include "parse.h"
#include <assert.h>
#include <termios.h>
#include <readline/readline.h>
#include <stdbool.h>


#define CMD_PROTOTYPE(name)    int name(int argc, char**argv)


// Maps strings to function pointers
struct cmd_internal {
    int (*handle)(int argc, char **argv);
    char *name;
};

#define isPrintable(c)    ('!' <= c && c <= '~')

#ifndef __wramp__

extern const char **environ;
static int tfork(){
    return fork();
}
static int enable_syscall_tracing(){
    return 0;
}
static int disable_syscall_tracing(){
    return 0;
}
#define WINFO       1
#define WINFO_MEM   1
#define WINFO_SLAB  1
static int wramp_syscall(int num, ...){
    return 0;
}

#endif


#endif
