#include <sys/syscall.h>

// #define ALIGN1K(x) 	(((((x)-1)>>10)<<10)+1023)
const char** _environ = NULL;

// void init_environ(){
//     _environ = *((const char ***)ALIGN1K((int)get_sp()));
//     // printf("user env %x \n", (unsigned int)_environ);
// }

// const char **get_environ(){
//     if(!_environ)
//         init_environ();
//     return _environ;
// }
