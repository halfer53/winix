#include "winix.h"

/*
    struct proc_config{
        char name[PROC_NAME_LEN];
        void (*entry)();
        int proc_nr;
        int quantum;
        int priority;
    };
*/

struct boot_image boot_table[NUM_TASKS] = {
    {"IDLE",   idle_main,           IDLE,   1,  MIN_PRIORITY,},
    {"SYSTEM", system_main,         SYSTEM, 64, MAX_PRIORITY,},
};

//IDLE's process number must be the lowest
BUILD_BUG_ON_FALSE(IDLE == -NUM_TASKS + 1); 

char *initial_env[] = {
    "HOME=/",
    "PATH=/bin",
    NULL
};

syscall_handler_t syscall_table[_NSYSCALL] = {
    no_syscall,
    do_times,       //SYSCALL_TIMES           1
    do_exit,        //SYSCALL_EXIT            2
    do_fork,        //SYSCALL_FORK            3
    do_vfork,       //SYSCALL_VFORK           4
    do_exec,        //SYSCALL_EXECVE          5
    do_brk,         //SYSCALL_BRK             6
    do_alarm,       //SYSCALL_ALARM           7
    do_sigaction,   //SYSCALL_SIGNAL          8
    do_sigreturn,   //SYSCALL_SIGRET          9
    do_wait,        //SYSCALL_WAIT            10
    do_kill,        //SYSCALL_KILL            11
    do_getpid,      //SYSCALL_GETPID          12
    do_winfo,       //SYSCALL_WINFO           13
    do_getc,        //SYSCALL_GETC            14
    do_printf,      //SYSCALL_PRINTF          15
    do_sysconf      //SYSCALL_SYSCONF         16
};

