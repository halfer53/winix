#include <signal.h>
#include <fcntl.h>
#include <sys/tty.h>
#include <fs/common.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <assert.h>

char shell_path[] = "/bin/bash";
char *shell_argv[] = {
    shell_path,
    NULL
};

int pgid;

void block_signals()
{
    int i;
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGSEGV);
    sigprocmask(SIG_SETMASK, &mask, NULL);
    for (i = 1; i < _NSIG; i++)
    {
        signal(i, SIG_IGN);
    }
}

void init_dev()
{
    int ret;

    ret = mkdir("/dev", 0x755);
    assert(ret == 0);

    ret = mknod("/dev/tty1", 0x755, TTY1_DEV_NUM);
    assert(ret == 0);

    ret = mknod("/dev/tty2", 0x755, TTY2_DEV_NUM);
    assert(ret == 0);

    ret = mknod("/dev/sda", 0x755, ROOT_DEV);
    assert(ret == 0);
}

void init_tty()
{
    int fd, ret;
    ret = setsid();
    assert(ret == getpid());
    fd = open("/dev/tty1", O_RDONLY);
    assert(fd == 0);
    ret = dup(fd);
    assert(ret == 1);
    ret = dup(fd);
    assert(ret == 2);
    ret = ioctl(STDIN_FILENO, TIOCSCTTY, 0);
    assert(ret == 0);
    pgid = getpgid(0);
    ret = ioctl(STDIN_FILENO, TIOCSPGRP, &pgid);
    assert(ret == 0);
}

void start_init_routine()
{
    pid_t pid;
    int status;
    block_signals();
    while (1)
    {
        pid = wait(&status);
        if (pid == -1)
        {
            break;
        }
        // printf("INIT: child %d exit status %d\n", pid, WEXITSTATUS(status));
    }

    printf("Shutting down...\n");
}

void run_unit_test()
{
    pid_t pid;
    int status;
    char *argv[] = {shell_path, "-c", "test", "run", ">", "test.log", NULL};
    pid = vfork();
    if (!pid){
        execv(shell_path, argv);
        _exit(1);
    }
    wait(&status);
    assert(WEXITSTATUS(status) == 0);
}

void run_shell(){
    if (!vfork())
    {
        execv(shell_path, shell_argv);
        _exit(1);
    }
}

int main(int argc, char **argv)
{
    init_dev();
    init_tty();

    run_unit_test();
    run_shell();
    start_init_routine();
    return 0;
}
