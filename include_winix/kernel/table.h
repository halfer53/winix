#ifndef _K_TABLE_H_
#define _K_TABLE_H_

#include <sys/syscall.h>
#include <sys/ipc.h>
#include <winix/syscall_ctx.h>
#include <kernel/proc.h>
#include <kernel/clock.h>
#include <kernel/system.h>
#include <kernel/idle.h>

void parallel_main();

#define INITILISE_ARRAY(n,val)  

struct boot_image{
    char name[PROC_NAME_LEN];
    void (*entry)();
    int proc_nr;
    int quantum;
    int priority;
    int stack_size;
};

extern struct boot_image boot_table[NUM_TASKS];
extern char *initial_env[];
extern char *init_argv[];
extern char INIT_PATH[];
extern syscall_handler_t syscall_table[_NSYSCALL];
extern char *syscall_str[_NSYSCALL];

struct proc *start_kernel_proc(struct boot_image* task);

int no_syscall(struct proc* who, struct message* m);
int syscall_not_implemented(struct proc* who, struct message *m);
int do_times(struct proc* who, struct message* m);
int do_exit(struct proc* who, struct message* m);
int do_fork(struct proc* who, struct message* m);
int do_vfork(struct proc* who, struct message* m);
int do_execve(struct proc* who, struct message* m);
int do_brk(struct proc* who, struct message* m);
int do_sbrk(struct proc* who, struct message* m);
int do_alarm(struct proc* who, struct message* m);
int do_sigaction(struct proc* who, struct message* m);
int do_sigreturn(struct proc* who, struct message* m);
int do_waitpid(struct proc* who, struct message* m);
int do_kill(struct proc* who, struct message* m);
int do_getpid(struct proc* who, struct message* m);
int do_winfo(struct proc* who, struct message* m);
int do_winix_strerror(struct proc* who, struct message* m);
int do_winix_dprintf(struct proc* who, struct message* m);
int do_sysconf(struct proc* who, struct message* m);
int do_sigsuspend(struct proc* who, struct message* m);
int do_sigpending(struct proc* who, struct message* m);
int do_sigprocmask(struct proc* who, struct message* m);
int do_getpgid(struct proc* who, struct message* m);
int do_setpgid(struct proc* who, struct message* m);
int do_open(struct proc* who, struct message* msg);
int do_creat(struct proc* who, struct message* msg);
int do_close(struct proc* who, struct message* msg);
int do_read(struct proc* who, struct message* msg);
int do_write(struct proc* who, struct message* msg);
int do_pipe(struct proc* who, struct message* msg);
int do_mknod(struct proc* who, struct message* msg);
int do_chdir(struct proc* who, struct message* msg);
int do_chown(struct proc* who, struct message* msg);
int do_chmod(struct proc* who, struct message* msg);
int do_stat(struct proc* who, struct message* msg);
int do_fstat(struct proc* who, struct message* msg);
int do_dup(struct proc* who, struct message* msg);
int do_dup2(struct proc* who, struct message* msg);
int do_link(struct proc* who, struct message* msg);
int do_unlink(struct proc* who, struct message* msg);
int do_getdents(struct proc* who, struct message* msg);
int do_access(struct proc* who, struct message* msg);
int do_mkdir(struct proc* who, struct message* msg);
int do_sync(struct proc* who, struct message* msg);
int do_lseek(struct proc* who, struct message* msg);
int do_umask(struct proc* who, struct message* msg);
int do_fcntl(struct proc* who, struct message* msg);
int do_setsid(struct proc* who, struct message* m);
int do_ioctl(struct proc* who, struct message* m);
int do_csleep(struct proc* who, struct message* m);
int do_getppid(struct proc* who, struct message *m);
int do_signal(struct proc* who, struct message *m);
int do_statfs(struct proc* who, struct message *msg);
int do_getcwd(struct proc* who, struct message *m);
int do_tfork(struct proc* who, struct message* m);
int do_sched_yield(struct proc* who, struct message* m);


#endif

