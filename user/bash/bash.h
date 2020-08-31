
#ifndef _SHELL_H_
#define _SHELL_H_ 1

#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <sys/statfs.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/debug.h>
#include <ucontext.h>
#include <sys/times.h>
#include "parse.h"


#define CMD_PROTOTYPE(name)    int name(int argc, char**argv)


// Maps strings to function pointers
struct cmd_internal {
    int (*handle)(int argc, char **argv);
    char *name;
};

#define isPrintable(c)    ('!' <= c && c <= '~')

#endif
