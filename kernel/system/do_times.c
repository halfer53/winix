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
#include <kernel/clock.h>

int do_times(struct proc *who, struct message *m){
    vptr_t *vp = m->m1_p1;
    struct tms buf;
    int ret;
    
    if(vp){
        if ((ret = copy_from_user(who, (ptr_t *)&buf, (vptr_t *)vp, sizeof(struct tms))) < 0)
            return ret;

        buf.tms_utime = who->time_used;
        buf.tms_stime = who->sys_time_used;
        buf.tms_cstime = 0;
        buf.tms_cutime = 0;

        if ((ret = copy_to_user(who, (vptr_t *)vp, (ptr_t *)&buf, sizeof(struct tms))) < 0)
            return ret;
    }

    return get_uptime();
}
