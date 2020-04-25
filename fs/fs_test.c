//
// Created by bruce on 19/04/20.
//

#include "fs.h"
#include <stdio.h>
#include <string.h>

struct proc pcurr;
struct proc *current_proc;
struct superblock *sb;


disk_word_t DISK_RAW[SIZE];
size_t DISK_SIZE;


void main(){
    int ret, fd, fd2, fd3, fd4, fd5;
    struct superblock sb;
    DISK_SIZE = SIZE;
    init_bitmap();
    char *filename = "/foo.txt";
    char buffer[100];


    ret = makefs(DISK_RAW, DISK_SIZE);
    if(ret){
        printf("makefs failed");
        return;
    }
    memcpy(&sb, DISK_RAW, sizeof(struct superblock));
    disk_word_t *foo = DISK_RAW;
    current_proc = &pcurr;

    init_fs(DISK_RAW, DISK_SIZE);
    fd = sys_open(current_proc, filename ,O_CREAT | O_RDWR, 0775);
    printf("fd open /foo.txt return fd %d\n", fd);

    char abc[] = "abcdefghijklmnopqrstuvwxyz";
    printf("Writing abc\n");
    sys_write(current_proc, fd, "abc", 3);
    sys_write(current_proc, fd, "def", 4);
    printf("Closing /foo.txt\n");
    sys_close(current_proc, fd);

    fd = sys_open(current_proc, filename ,O_RDONLY, 0775);
    printf("fd reopen /foo.txt return fd %d\n", fd);

    ret = sys_read(current_proc, fd, buffer, 100);
    printf("Read got %s, ret %d\n", buffer, ret);
    ret = sys_read(current_proc, fd, buffer, 100);
    printf("Read got %s, ret %d\n", buffer, ret);
    sys_close(current_proc, fd);


}