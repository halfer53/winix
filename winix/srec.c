#include <kernel/kernel.h>
#include <winix/srec.h>

PRIVATE unsigned int syscall_code = 0x200d0000;

int build_initial_stack(struct proc* who, int argc, char** argv){
    struct initial_frame init_stack;
    struct initial_frame* pstack = &init_stack;

    //setup argc and argv before
    who->ra = who->sp -1;

    pstack->operation = WINIX_SENDREC;
    pstack->dest = SYSTEM;
    pstack->pm = (struct message*)(who->sp - sizeof(syscall_code) - sizeof(struct message));
    pstack->m.type = SYSCALL_EXIT;
    pstack->m.m1_i1 = EXIT_MAGIC;
    pstack->syscall_code = syscall_code;
    
    return build_user_stack(who, pstack, sizeof(struct initial_frame));
}

