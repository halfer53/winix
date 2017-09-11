#include "winix.h"
#include <init_bin.c>
#include <shell_bin.c>

/*
    struct proc_config{
        char name[PROC_NAME_LEN];
        void (*entry)();
        int pid;
        int quantum;
        int priority;
        bool iskernel_proc;
        unsigned int *image_array;
        int image_len;
    };
*/

struct proc_config boot_table[] = {
    {"SYSTEM", system_main,         SYSTEM, 64, MAX_PRIORITY,       true,   NULL,       0                   },
    {"INIT",   (void(*)())init_pc,  INIT,   8,  MAX_PRIORITY,       false,  init_code,  init_code_length    },
    {"IDLE",   idle_main,           IDLE,   1,  MIN_PRIORITY,       true,   NULL,       0                   },
};

