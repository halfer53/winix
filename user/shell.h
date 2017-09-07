
#ifndef _SHELL_H_
#define _SHELL_H_ 1

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <debug.h>
#include <ucontext.h>
#include <sys/times.h>


#define CMD_PROTOTYPE(name)    int name(int argc, char**argv)

#define BUF_LEN         32
#define MAX_COMMANDS    5
#define MAX_ARGS        10

struct cmdLine{
    char buf[BUF_LEN];
    int argc;
    char cmdStart[MAX_COMMANDS];
    int numCommands;
    char *argv[MAX_ARGS];
    int append;
};

//Maps strings to function pointers
struct cmd {
    int (*handle)(int argc, char **argv);
    char *name;
};


int parse(char *line, struct cmdLine *sc);

int test_so();
int test_float();
int test_alarm(int input);
int test_signal(int input);
int test_thread(int num);
int test_malloc();
int test_fork();

#define isspace(c)    (c==' ')
#define isPrintable(c)    ('!' <= c && c <= '~')

#endif
