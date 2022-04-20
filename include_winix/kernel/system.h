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
struct message *curr_mesg();
void intr_syscall();
struct syscall_ctx* interrupted_syscall_ctx();

void syscall_region_begin();
void syscall_region_end();
void intr_syscall();
struct message *curr_mesg();
struct syscall_ctx *interrupted_syscall_ctx();
void kreport_sysinfo();
void init_syscall_table();
bool is_in_syscall(struct proc* who);
ptr_t* sys_sbrk(struct proc *who, int size);
void set_syscall_mesg_exception(int operation, ptr_t* sp, struct message *m, struct proc* who);
void set_reply_res_errno(struct proc* who, struct message *m);

int sys_kill(struct proc* who, pid_t pid, int signum);

#ifdef _DEBUG
#define SYSCALL_MAP(i,fn)   syscall_table[i] = fn;\
                    syscall_str[i] = #i
#else
#define SYSCALL_MAP(i,fn)   syscall_table[i] = fn;\
                    syscall_str[i] = #i
#endif


#endif
