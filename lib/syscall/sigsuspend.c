#include <sys/syscall.h>
#include <signal.h>
#include <stddef.h>
#include <errno.h>

// int sigsuspend(const sigset_t *mask){
//     struct message m;
//     if(mask == NULL){
//         __set_errno(EFAULT);
//         return -1;
//     }
//     m.m1_i1 = *mask;
//     return _syscall(SIGSUSPEND, &m);
// }

