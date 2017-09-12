#include <lib.h>

#define ALIGN1K(x) 	(((((x)-1)>>10)<<10)+1023)

const char** _penviron = NULL;

void init_environ(){
    _penviron = *((const char ***)ALIGN1K(get_sp()));
    // printf("env %x \n", (unsigned int)_penviron + 0x5800);
}
