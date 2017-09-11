#ifndef _K_SREC_H_
#define _K_SREC_H_

#include <kernel/proc.h>



struct initial_frame{
    int operation;
    int dest;
    struct message *pm;
    struct message m;
    unsigned int syscall_code;
};

int build_initial_stack(struct proc* who, int argc, char** argv);

#endif
