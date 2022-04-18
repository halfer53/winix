#include <sys/types.h>
#include <stddef.h>
#include "../user/bash/parse.h"
#include <assert.h>
#include <string.h>

void test_given_cmdline_when_4_pipes_should_return_4_argc(){
    struct cmdLine cmd;
    int ret = parse("ls | grep | wc | cat", &cmd);
    assert(ret == 0);
    assert(cmd.numCommands == 4);
    assert(strcmp(cmd.argv[cmd.cmdStart[0]], "ls") == 0);
    assert(strcmp(cmd.argv[cmd.cmdStart[1]], "grep") == 0);
    assert(strcmp(cmd.argv[cmd.cmdStart[2]], "wc") == 0);
    assert(strcmp(cmd.argv[cmd.cmdStart[3]], "cat") == 0);
}