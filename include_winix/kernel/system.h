/**
 * 
 * WINIX System Task.
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2016-09-19
 * 
*/


#ifndef _K_SYSTEM_H_
#define _K_SYSTEM_H_ 1

#include <winix/syscall_ctx.h>

/**
 * Entry point for the system task.
 **/
void system_main();
void intr_syscall();
struct syscall_ctx* interrupted_syscall_ctx();

void syscall_region_begin();
void syscall_region_end();
void intr_syscall();
struct message *get_ipc_mesg();
struct syscall_ctx *interrupted_syscall_ctx();
void kreport_sysinfo();
void init_syscall_table();
bool is_in_syscall(struct proc* who);
ptr_t* sys_sbrk(struct proc *who, int size);
void set_syscall_mesg_exception(int operation, ptr_t* sp, struct message *m, struct proc* who);
int set_syscall_reply(struct proc* who, int ret, int syscall_num);
int curr_syscall_num();
int sys_kill(struct proc* who, pid_t pid, int signum);

#ifdef _DEBUG
#define SYSCALL_MAP(i,fn)   _syscall_map(i,fn,#i)
#else
#define SYSCALL_MAP(i,fn)   _syscall_map(i,fn, NULL)
#endif


#endif
