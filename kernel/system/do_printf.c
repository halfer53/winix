/**
 * Syscall in this file: printf
 * Input:    m1_p1: string parameter
 *             m1_p2: options parameter
 *
 * Return:     reply_res: number of bytes being printed
 * 
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:09:58
 * 
*/
#include <kernel/kernel.h>


/**
 * printf in winix is a system call, I know it seems rediculous, but let me explain
 * printf is usually a user space function that internally uses write() syscall.
 * Stdout is line buffered to avoid making expensive system calls. While having 
 * printf() in the user space increases security and performance, printf() usually uses 
 * extensive amount of codes, that makes user process text size much more bigger. 
 * In addition, it's pointless for each user process to have the same printf() in the user space
 * It's just redundant code! Linux addresses this by mapping all processes' user space printf() to a 
 * common address, but we can't do that in winix, due to the limitation of architecture
 * Since this is a small embedded architecture that has less than 1MB of memory, 
 * moving printf() inside the kernel is a good choice in my humble opinion
 * @param  who 
 * @param  m   
 * @return     
 */
int do_printf(struct proc *who, struct message *m){
    vptr_t* vp1, *vp2;
    void *ptr, *ptr2;
    vp1 = m->m1_p1;
    vp2 = m->m1_p2;
    if(!is_vaddr_ok(vp1, who) || !is_vaddr_ok(vp2, who))
        return EFAULT;
    ptr = get_physical_addr(vp1,who);
    ptr2 = get_physical_addr(vp2,who);
    return kprintf_vm(ptr,ptr2,who->rbase);
}
