#include "winix.h"
#include <init_codes.c>

/*
    struct proc_config{
        char name[PROC_NAME_LEN];
        void (*entry)();
        int pid;
        int quantum;
        bool iskernel_proc;
        unsigned int *image_array;
        int image_len;
    };
*/

struct proc_config boot_table[] = {
    {"SYSTEM", system_main,         SYSTEM_TASK,    64, true,   NULL,       0                   },
    {"INIT",   (void(*)())init_pc,  INIT,           1,  false,  init_code,  sizeof(init_code)   },
    {"IDLE",   idle_main,           IDLE,           1,  true,   NULL,       0                   },
};

