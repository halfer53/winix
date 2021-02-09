#include <kernel/kernel.h>
/**
 * Syscall in this file: sigpending
 * Input:   
 *
 * Return:  m1_i1: pending masks
 * 
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:10:17
 * 
*/
int do_sigpending(struct proc* who, struct message* m){
    vptr_t *vp = m->m1_p1;
    ptr_t *p;
    if(!is_vaddr_accessible(vp, who))
        return EFAULT;
    p = get_physical_addr(vp, who);
    *p = who->sig_pending;
    // m->m1_i1 = who->sig_pending;
    return OK;
}

