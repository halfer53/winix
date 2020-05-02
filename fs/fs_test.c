//
// Created by bruce on 19/04/20.
//

#include "fs.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>


struct proc pcurr, pcurr2;
struct proc *current_proc;
struct proc *curr_user_proc_in_syscall;
struct superblock *sb;


disk_word_t DISK_RAW[SIZE];
size_t DISK_SIZE;

void emulate_fork(struct proc* p1, struct proc* p2){
    int procnr = p2->proc_nr;
    pid_t pid = p2->pid;
    *p2 = *p1;
    p2->proc_nr = procnr;
    p2->pid = pid;
}

void main(){
    int ret, fd;
    int pipe_fd[2];
    struct superblock sb;
    DISK_SIZE = SIZE;
    init_bitmap();
    char *filename = "/foo.txt";
    char buffer[100];
    pcurr.proc_nr = 1;
    pcurr.pid = 1;
    pcurr2.proc_nr = 2;
    pcurr2.pid = 2;


    ret = makefs(DISK_RAW, DISK_SIZE);
    if(ret){
        printf("makefs failed");
        return;
    }
    memcpy(&sb, DISK_RAW, sizeof(struct superblock));
    current_proc = &pcurr;
    curr_user_proc_in_syscall = current_proc;

    init_fs(DISK_RAW, DISK_SIZE);
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

    ret = sys_pipe(&pcurr, pipe_fd);
    assert(ret == 0);
    emulate_fork(&pcurr, &pcurr2);
    ret = sys_read(&pcurr, pipe_fd[0], buffer, 100);
    assert(ret == SUSPEND);
    ret = sys_write(&pcurr2, pipe_fd[1], "1234", 4);
    assert(ret == 4);

    ret = sys_write(&pcurr2, pipe_fd[1], "1234", 5);
    assert(ret == 5);
    ret = sys_read(&pcurr, pipe_fd[0], buffer, 100);
    assert(ret == 5);
    assert(strcmp(buffer, "1234") == 0);
}