//
// Created by bruce on 19/04/20.
//

#ifndef FS_CMAKE_UTIL_H
#define FS_CMAKE_UTIL_H

#include <sys/types.h>
#include <sys/ipc.h>

void* kmalloc(unsigned int size);
void kfree(void *ptr);

int do_ls(char* pathname);
int syscall_reply(int reply, int dest, struct message* m);
int syscall_reply2(int syscall_num, int reply, int dest,  struct message* m);
void emulate_fork(struct proc* p1, struct proc* p2);
void mock_init_proc();
void _assert(int expression, int line, char* filename);
void init_tty();

int dkprintf(struct proc* who, int fd, const char* format, ...);
#endif //FS_CMAKE_UTIL_H
