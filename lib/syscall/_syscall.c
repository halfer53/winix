#include <sys/syscall.h>

int _syscall(int syscall_num, struct message *m){
    m->type = syscall_num;
    return winix_sendrec(SYSTEM_TASK, m);
}

// Print the user space heap
int enable_syscall_tracing(){
    struct message m;
    m.m1_i1 = WINFO_TRACE_SYSCALL;
    return _syscall(WINFO, &m);
}

