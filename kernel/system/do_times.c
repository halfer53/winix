/**
 * Syscall in this file: time
 * Input:    
 *      
 * Return:     
 *          m2_l1  return
 *          m2_l2: tms_utime
 *          m2_l3: tms_stime
 *          m2_l4: tms_cutime
 *          m2_l5: tms_cstime
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:10:29
 * 
*/
#include <kernel/kernel.h>
#include <sys/times.h>

int do_times(struct proc *who, struct message *m){
    vptr_t *vp = m->m1_p1;
    ptr_t *p;
    struct tms* buf;
    if(vp && !is_vaddr_accessible(vp, who))
        return EFAULT;
    if(vp){
        buf = (struct tms*)get_physical_addr(vp, who);
        buf->tms_utime = who->time_used;
        buf->tms_stime = who->sys_time_used;
        buf->tms_cstime = 0;
        buf->tms_cutime = 0;
    }
    // m->m2_l1 = get_uptime(); 
    // m->m2_l2 = who->time_used;
    // m->m2_l3 = who->sys_time_used;
    return get_uptime();
}
