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
#include <termios.h>

char shell_path[] = "/bin/wsh";
char *shell_argv[] = {
    shell_path,
    NULL
};
char *initial_env[] = {
    "HOME=/",
    "PATH=/bin",
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

void init_system(){
    int ret;

    ret = mkdir("/tmp", 0777);
    assert(ret == 0);
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

void init_termios(int fd){
    struct termios termios;
    int ret;

    ret = tcgetattr(fd, &termios);
    assert(ret == 0);
    
    termios.c_lflag = ISIG | ICANON | ECHO | ECHOE | ECHOK;
    termios.c_iflag = ICRNL | IMAXBEL;
    termios.c_oflag = 0;
    termios.c_cflag = CREAD;
    termios.c_cc[VINTR]    =   03;  /* ^C */
    termios.c_cc[VQUIT]    =  034;  /* ^\ */
    termios.c_cc[VERASE]   =    8;  /* backspace */
    termios.c_cc[VKILL]    =  025;  /* ^U */
    termios.c_cc[VEOF]     =   04;  /* ^D */
    termios.c_cc[VSTART]   =  021;  /* ^Q */
    termios.c_cc[VSTOP]    =  023;  /* ^S */
    termios.c_cc[VSUSP]    =  032;  /* ^Z */
    termios.c_cc[VLNEXT]   =  026;  /* ^V */
    termios.c_cc[VWERASE]  =  027;  /* ^W */
    termios.c_cc[VREPRINT] =  022;  /* ^R */
    termios.c_cc[VEOL]     =    0;
    termios.c_cc[VEOL2]    =    0;

    termios.c_cc[VTIME]  = 0;
    termios.c_cc[VMIN]   = 1;

    ret = tcsetattr(fd, TCSANOW, &termios);
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

    init_termios(STDIN_FILENO);

    fd = open("/dev/tty2", O_RDWR);
    assert(fd == 3);
    init_termios(fd);
    close(fd);
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

char testlog_path[] = "/var/log/test.log";

void run_unit_test()
{
    char* const argv[] = {shell_path, "-c", "test", "run", ">", testlog_path, NULL};
    execve(shell_path, argv, initial_env);
}

#define BUFFER_SIZE 64

void wait_for_unit_test(pid_t pid){
    int status, exit_status, fd, ret;

    ret = waitpid(pid, &status, 0);
    exit_status = WEXITSTATUS(status);
    if (exit_status){
        char buf[BUFFER_SIZE];
        printf("Unit test failed, exit status %d\n", exit_status);
        fd = open(testlog_path, O_RDONLY);
        while((ret = read(fd, buf, BUFFER_SIZE * sizeof(char))) > 0){
            write(STDERR_FILENO, buf, ret);
        }
        close(fd);
    }
    assert(exit_status == 0);
}

void run_shell(){
    execve(shell_path, shell_argv, initial_env);
}

int init_tty2_environment(){
    int ret, fd;

    ret = setsid();
    assert(ret == getpid());

    fd = open("/dev/tty2", O_RDWR);
    assert(fd > 0);
    
    ret = ioctl(fd, TIOCSCTTY, 0);
    assert(ret == 0);
    pgid = getpgid(0);
    ret = tcsetpgrp(fd, pgid);
    assert(ret == 0);
    return fd;
}

pid_t run_in_tty2(int fd, void (*func)()){
    int ret;
    pid_t pid;
    pid = tfork();
    if (!pid){
        ret = dup2(fd, STDIN_FILENO);
        assert(ret == 0);
        ret = dup2(fd, STDOUT_FILENO);
        assert(ret == 1);
        ret = dup2(fd, STDERR_FILENO);
        assert(ret == 2);

        ret = close(fd);
        assert(ret == 0);
        func();
        _exit(1);
    }
    return pid;
}

int main(int argc, char **argv)
{
    pid_t pid;
    int fd;
    init_dev();
    init_tty();
    init_system();

    if (!vfork())
        run_shell();

    if (!tfork()){
        fd = init_tty2_environment();
        pid = run_in_tty2(fd, run_unit_test);
        wait_for_unit_test(pid);
        run_in_tty2(fd, run_shell);
        exit(0);
    }
    
    
    start_init_routine();
    return 0;
}
