#include <lib.h>


// int getchar(){
//     struct message m;
//     return _syscall(GETC, &m);
// }


void perror(const char *s){
    struct message m;
    m.m1_p1 = (void*)s;
    m.m1_i1 = errno;
    _syscall(PERROR, &m);
}

