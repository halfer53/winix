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

    ret = mkdir("/dev", 0755);
    assert(ret == 0);

    ret = mkdir("/var", 0755);
    assert(ret == 0);

    ret = mkdir("/var/log/", 0755);
    assert(ret == 0);

    ret = mknod("/dev/tty1", 0755, TTY1_DEV_NUM);
    assert(ret == 0);

    ret = mknod("/dev/tty2", 0755, TTY2_DEV_NUM);
    assert(ret == 0);

    ret = mknod("/dev/sda", 0755, ROOT_DEV);
    assert(ret == 0);
}

void init_tty()
{
    int fd, ret;
    ret = setsid();
    assert(ret == getpid());
    fd = open("/dev/tty1", O_RDWR);
    assert(fd == 0);
    ret = dup(fd);
    assert(ret == 1);
    ret = dup(fd);
    assert(ret == 2);
    ret = ioctl(STDIN_FILENO, TIOCSCTTY, 0);
    assert(ret == 0);
    pgid = getpgid(0);
    ret = tcsetpgrp(STDIN_FILENO, pgid);
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

pid_t run_unit_test()
{
    pid_t pid;
    char *argv[] = {shell_path, "-c", "test", "run", ">", "/var/log/test.log", NULL};
    pid = vfork();
    if (!pid){
        execv(shell_path, argv);
        _exit(1);
    }
    return pid;
}

void wait_for_unit_test(pid_t pid){
    int status, exit_status, fd, ret;

    ret = waitpid(pid, &status, 0);
    exit_status = WEXITSTATUS(status);
    if (exit_status){
        char buf[64];
        disable_syscall_tracing();
        printf("Unit test failed, exit status %d\n", exit_status);
        fd = open("test.log", O_RDONLY);
        while((ret = read(fd, buf, 64 * sizeof(char))) > 0){
            write(STDOUT_FILENO, buf, ret);
        }
        close(fd);
    }
    assert(exit_status == 0);
}

void run_shell(){
    if (!vfork())
    {
        execv(shell_path, shell_argv);
        _exit(1);
    }
}

void start_tty2_shell(){
    int ret;
    int fd = open("/dev/tty2", O_RDWR);
    assert(fd > 0);
    
    ret = ioctl(fd, TIOCSCTTY, 0);
    assert(ret == 0);
    pgid = getpgid(0);
    ret = tcsetpgrp(fd, pgid);
    assert(ret == 0);

    if(!tfork()){
        
        ret = dup2(fd, STDIN_FILENO);
        assert(ret == 0);
        ret = dup2(fd, STDOUT_FILENO);
        assert(ret == 1);
        ret = dup2(fd, STDERR_FILENO);
        assert(ret == 2);
        ret = close(fd);
        assert(ret == 0);

        run_shell();
        exit(0);
    }else{
        wait(NULL);
    }
}

int main(int argc, char **argv)
{
    pid_t pid;
    init_dev();
    init_tty();
    run_shell();

    pid = run_unit_test();
    wait_for_unit_test(pid);
    start_tty2_shell();
    
    start_init_routine();
    return 0;
}
