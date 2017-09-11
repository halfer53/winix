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

char *initial_env[]{
    "PATH=/bin",
    NULL
};

