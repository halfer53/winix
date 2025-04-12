//
// Created by bruce on 19/04/20.
//

#ifndef FS_CMAKE_UTIL_H_
#define FS_CMAKE_UTIL_H_

#include <stddef.h>
#include <sys/ipc.h>
#include <kernel/proc.h>

void* kmalloc(size_t nitimes, size_t size);
void kfree(void *ptr);

int do_ls(char* pathname);
int syscall_reply(int reply, int dest, struct message* m);
int syscall_reply2(int syscall_num, int reply, int dest,  struct message* m);
void emulate_fork(struct proc* p1, struct proc* p2);
clock_t get_uptime();
void mock_init_proc();
void _assert(int expression, int line, char* filename);
void init_tty();
size_t strlcpy(char *dest, const char *src, size_t n);
void set_start_unix_time(clock_t t);
void init_disk();

#endif //FS_CMAKE_UTIL_H_
