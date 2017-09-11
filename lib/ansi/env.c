#include <lib.h>

#define ALIGN1K(x) 	(((((x)-1)>>10)<<10)+1024)

const char** _penviron;

void init_environ(){
    _penviron = (char **)ALIGN1K(get_sp()) - 1;
}