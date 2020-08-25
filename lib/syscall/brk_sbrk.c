#include <sys/syscall.h>
#include <stddef.h>

static char* _brk = NULL; // data segment break cache

int brk(void *addr){
    struct message m;
    int ret = 0;
    m.m1_p1 = addr;
    ret = _syscall(BRK, &m);
    _brk = m.m1_p1;
    if(ret < 0)
        return -1;
    return ret;
}

void *sbrk(int incr){
    char *newsize, *oldsize;

    if(_brk == NULL){
        brk((void *)0xffffffff); // initialise _brk
    }

    oldsize = _brk;
    newsize = _brk + incr;
    if ((incr > 0 && newsize < oldsize) || (incr < 0 && newsize > oldsize))
        goto return_err;
    
    if(incr == 0)
        return _brk;
    
    if (brk(newsize) == 0)
        return(oldsize);

    return_err:
        return NULL;
        
}
