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
    fd = sys_open(current_proc, filename ,O_CREAT | O_RDWR);
    printf("fd open /foo.txt return fd %d\n", fd);

    char abc[] = "abcdefghijklmnopqrstuvwxyz";
    printf("Writing abc\n");
    sys_write(current_proc, fd, "abc", 4);
    printf("Closing /foo.txt\n");
    sys_close(current_proc, fd);

    fd = sys_open(current_proc, filename ,O_RDONLY);
    printf("fd reopen /foo.txt return fd %d\n", fd);

    sys_read(current_proc, fd, buffer, 4);
    sys_close(current_proc, fd);
    printf("Read got %s\n", buffer);

//    char c = 'a';
//
//    for (i = 0; i < 2048; i++) {
//        sys_write(current_proc, fd, &c, 1);
//        c++;
//        if (c == 'z')
//            c = 'a';
//    }
//    sys_write(current_proc,fd, "a", 2);
//    sys_close(current_proc, fd);
//
//    char block_buffer[1024];
//    fd = sys_open(current_proc, "/foo.txt",O_RDONLY);
//    sys_read(current_proc, fd,block_buffer,2049);
//    sys_close(current_proc, fd);
//
//    printf("\nread foo.txt\n");
//    printf("Got \"%s\" from foo.txt\n",block_buffer);

}