#include <kernel/kernel.h>
#include <kernel/table.h>

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
    {"IDLE",   idle_main,           IDLE,       1,  MIN_PRIORITY,   50},
    {"PARALLEL",   parallel_main,   PARALLEL,   1,  MIN_PRIORITY,   50},
    {"SYSTEM", system_main,         SYSTEM,     64, MAX_PRIORITY, KERNEL_STACK_SIZE},
};

// IDLE's process number must be the lowest
BUILD_BUG_ON_FALSE(IDLE == -NUM_TASKS + 1); 

char INIT_PATH[] = "/bin/init";

char *init_argv[] = {
    INIT_PATH,
    NULL
};

char *initial_env[] = {
    "HOME=/",
    "PATH=/bin",
    NULL
};


char *syscall_str[_NSYSCALL] = {{0}};

syscall_handler_t syscall_table[_NSYSCALL] = {{no_syscall}};



