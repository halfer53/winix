//
// Created by bruce on 19/04/20.
//

#ifndef FS_UTIL_CMAKE_H
#define FS_UTIL_CMAKE_H

#include <sys/types.h>
#include <sys/ipc.h>

void* kmalloc(unsigned int size);
void kfree(void *ptr);


int syscall_reply(int reply, int dest, struct message* m);
int syscall_reply2(int syscall_num, int reply, int dest,  struct message* m);

#endif //FS_UTIL_CMAKE_H
