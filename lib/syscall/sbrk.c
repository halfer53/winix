#include <stddef.h>
#include <unistd.h>

static char *_brk = NULL;

void *sbrk(int incr){
    char *newbrk;

    if(_brk == NULL)
        _brk = ptr_wramp_syscall(SBRK, 0);
    
    if (incr == 0)
        return _brk;

    newbrk = ptr_wramp_syscall(SBRK, incr);
    _brk = newbrk;
    return newbrk;
}