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
#include <winix/dev.h>
#include <fs/path.h>

int do_winix_strerror(struct proc* who, struct message* msg){
    char *dst;
    int usr_errno, len;
    struct filp *stderr_file;

    if(!is_vaddr_ok(msg->m1_p1, who))
        return EFAULT;
    len = msg->m1_i1;
    usr_errno = msg->m1_i2;
    
    if(usr_errno <= 0 || usr_errno >= _NERROR)
        return EINVAL;
    dst = (char*)get_physical_addr(msg->m1_p1, who);
    // KDEBUG(("errno %d from %s\n", usr_errno, who->name));
    strlcpy(dst, kstr_error(usr_errno), len);
    return 0;
}

/**
 * printf in winix is a system call, I know it seems rediculous, but let me explain
 * printf is usually a user space function that internally uses write() syscall.
 * Stdout is line buffered to avoid making expensive system calls. While having 
 * printf() in the user space increases security and performance, printf() usually uses 
 * extensive amount of codes, that makes user process text size much  bigger. 
 * Linux addresses this by using dynamic library
 * , but we can't do that in winix, due to the limitation of the compiler
 * since this is a small embedded architecture that has less than 1MB of memory, 
 * @param  who 
 * @param  m   
 * @return     
 */
int do_winix_dprintf(struct proc *who, struct message *m){
    struct filp* file;
    int fd;
    vptr_t* vp1, *vp2;
    void *ptr, *ptr2;
    fd = m->m1_i1;
    vp1 = m->m1_p1;
    vp2 = m->m1_p2;
    if(!is_vaddr_ok(vp1, who) || !is_vaddr_ok(vp2, who))
        return EFAULT;
    if(!is_fd_opened_and_valid(who, fd))
        return EBADF;
    file = who->fp_filp[fd];
    ptr = get_physical_addr(vp1,who);
    ptr2 = get_physical_addr(vp2,who);
    return kprintf_vm(file, ptr, ptr2, who->ctx.rbase);
}
