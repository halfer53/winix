#include <kernel/kernel.h>

int do_sigpending(struct proc* who, struct message* m){
    vptr_t* vset = m->m1_p1;
    ptr_t* set;
    if(!vset || !is_addr_accessible(vset, who))
        return EFAULT;
    set = get_physical_addr(vset,who);
    *set = who->sig_pending;
    return OK;
}

