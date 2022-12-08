#include <stddef.h>
#include <unistd.h>

static char *_brk = NULL;

void *sbrk(int incr){
    if(_brk == NULL)
        goto ret;
    
    if (incr == 0)
        return _brk;
ret:
    return (_brk = ptr_wramp_syscall(SBRK, incr));
}