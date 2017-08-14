#include "../winix.h"

int do_printf(struct proc *who, struct message *m){
    void *ptr, *ptr2;
    ptr = get_physical_addr(m->p1,who);
    ptr2 = get_physical_addr(m->p2,who);
    return kprintf_vm(ptr,ptr2,who->rbase);
}
