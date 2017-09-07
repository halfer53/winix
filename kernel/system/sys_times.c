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
#include "../winix.h"
#include <sys/times.h>

int do_times(struct proc *who, struct message *m){
    m->m2_l2 = who->time_used;
    m->m2_l3 = who->sys_time_used;
    return get_uptime();
}
