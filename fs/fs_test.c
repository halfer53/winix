//
// Created by bruce on 9/05/20.
//
#include "fs.h"
#include <assert.h>

int do_tests(){
    struct proc pcurr2;
    int ret, fd;
    int pipe_fd[2];
    struct superblock sb;
    init_bitmap();
    char *filename = "/foo.txt";
    char buffer[100];

    pcurr2.pid = 2;
    pcurr2.proc_nr = 2;

    init_fs();
    mock_init_proc();
    fd = sys_open(current_proc, filename ,O_CREAT | O_RDWR, 0775);
    assert(fd == 0);

    ret = sys_write(current_proc, fd, "abc", 3);
    assert(ret == 3);
    ret = sys_write(current_proc, fd, "def", 4);
    assert(ret == 4);
    ret = sys_read(current_proc, fd, buffer, 100);
    assert(ret == 0);
    ret = sys_lseek(current_proc,fd, 0, SEEK_SET);
    assert(ret == 0);
    ret = sys_read(current_proc, fd, buffer, 100);
    assert(ret == 7);
    assert(strcmp(buffer, "abcdef") == 0);
    ret = sys_close(current_proc, fd);
    assert(ret == 0);

    fd = sys_open(current_proc, filename ,O_RDONLY, 0775);
    assert(fd == 0);

    ret = sys_read(current_proc, fd, buffer, 100);
    assert(ret == 7);
    assert(strcmp(buffer, "abcdef") == 0);
    ret = sys_read(current_proc, fd, buffer, 100);
    assert(ret == 0);
//    sys_close(current_proc, fd);

    ret = sys_pipe(current_proc, pipe_fd);
    assert(ret == 0);
    emulate_fork(current_proc, &pcurr2);
    ret = sys_read(current_proc, pipe_fd[0], buffer, 100);
    assert(ret == SUSPEND);
    ret = sys_write(&pcurr2, pipe_fd[1], "1234", 4);
    assert(ret == 4);

    ret = sys_write(&pcurr2, pipe_fd[1], "1234", 5);
    assert(ret == 5);
    ret = sys_read(current_proc, pipe_fd[0], buffer, 100);
    assert(ret == 5);
    assert(strcmp(buffer, "1234") == 0);

    ret = sys_mkdir(current_proc, "/dev", 0x755);
    assert(ret == 0);
    printf("Do LS: ");
//    do_ls("/");

    ret = sys_access(current_proc, "/dev/bar.txt", F_OK);
    assert(ret != 0);

    ret = sys_creat(current_proc, "/dev/bar.txt", 0x777);
    assert(ret > 0);

    ret = sys_access(current_proc, "/dev/bar.txt", F_OK);
    assert(ret == 0);

    ret = sys_chdir(current_proc, "/dev");
    assert(ret == 0);

    printf("Do dev LS: ");
//    do_ls("/dev");
//    do_ls(".");
    return 0;
}