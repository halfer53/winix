#include <lib.h>

void __sigreturn(){
    struct message m;
    _syscall(SYSCALL_SIGRET, &m);
}
