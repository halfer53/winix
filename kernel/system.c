/**
 * 
 * Winix system task for processing system calls
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2016-09-19
 * 
*/

#include <kernel/kernel.h>
#include <kernel/sched.h>
#include <kernel/table.h>
#include <ucontext.h>
#include <winix/list.h>
#include <fs/fs_methods.h>
#include <sys/fcntl.h>
#include <winix/welf.h>
#include <fs/super.h>
#include <winix/ksignal.h>

PRIVATE struct message _message;
PRIVATE int curr_caller_proc_nr;
PRIVATE int curr_syscall;
PRIVATE struct proc *curr_caller;
PRIVATE ucontext_t recv_ctx;

bool trace_syscall = false;

/**
 * Entry point for system task.
 **/
void system_main() {
    int reply;
    syscall_handler_t handler;
    struct message* mesg = &_message;

    kreport_sysinfo();
    getcontext(&recv_ctx);
    // Receive message, do work, repeat.
    while(true) {
        // get a message
        winix_receive(mesg);
        curr_caller_proc_nr = mesg->src;
        curr_caller = get_proc(curr_caller_proc_nr);
        if(!curr_caller)
            continue;

        syscall_region_begin();

        if(mesg->type >= 1 && mesg->type <= _NSYSCALL)
            handler = syscall_table[mesg->type];
        else
            handler = no_syscall;

        if(!handler)
            handler = syscall_not_implemented;
        
        reply = handler(curr_caller, mesg);

        switch(reply){
            case SUSPEND:
            case DONTREPLY:
                break;
            default:
                syscall_reply2(curr_syscall, reply, curr_caller_proc_nr, mesg);
        }

        handle_pendingsig_syscall(curr_caller, reply);
        syscall_region_end();
    }
}

/**
 * print sys info of text, data and bss segment size
 */
 void kreport_sysinfo(){
    int free_mem_begin, mem_end;
    struct superblock* sb = get_sb(get_dev(ROOT_DEV));
    free_mem_begin = peek_next_free_page() * PAGE_LEN;
    mem_end = peek_last_free_page() * PAGE_LEN;
    kprintf("\nWINIX v%d.%d\n", MAJOR_VERSION, MINOR_VERSION);
    kprintf2("\nWINIX v%d.%d\n", MAJOR_VERSION, MINOR_VERSION);
    kprintf("Text Segment: 0x%08lx - 0x%08lx\n", (uintptr_t)&TEXT_BEGIN, (uintptr_t)&TEXT_END);
    kprintf("Data Segment: 0x%08lx - 0x%08lx\n", (uintptr_t)&DATA_BEGIN, (uintptr_t)&DATA_END);
    kprintf("BSS Segment:  0x%08lx - 0x%08lx\n", (uintptr_t)&BSS_BEGIN, (uintptr_t)&BSS_END);
    kprintf("Unallocated:  0x%08x - 0x%08x\n", free_mem_begin, mem_end);
    kprintf("Root Disk %d Blocks Used, %d Remaining\n", sb->s_block_inuse, sb->s_free_blocks);
    kprintf("%ld Pages Free\n", 
    ((unsigned int)(mem_end - free_mem_begin + PAGE_LEN)) / PAGE_LEN); // inclusive
}

void syscall_region_begin(){
    ASSERT(curr_caller != NULL);

    // Bill the user proc's sys_used_time while executing syscall
    // on behalf of the user process
    set_bill_ptr(curr_caller);
    SYSTEM_TASK->flags |= BILLABLE;
    curr_syscall = _message.type;
    curr_caller->syscall_start_time = get_uptime();

    // the following two are defensive statements
    // to ensure the caller is suspended while the system
    // is handling the system call

    curr_caller->state |= STATE_RECEIVING;
    // dequeue_schedule(who);
    
    SET_CALLER(curr_caller);
    // Make sure system doesn't send a message to itself
    
    ASSERT(curr_caller_proc_nr != SYSTEM); 
}

void syscall_region_end(){
    SYSTEM_TASK->flags &= ~BILLABLE;
    // reset messages
    memset(&_message, 0, sizeof(struct message));
    curr_caller_proc_nr = 0;
    curr_syscall = 0;
}

void set_syscall_mesg_exception(int operation, ptr_t* osp, struct message *m, struct proc* who){
    unsigned long *sp = (unsigned long*)osp;
    switch (operation)
    {
    case LSEEK:
        m->m1_i3 = *(sp + 2);
        /* FALLTHRU */
    case EXIT:
    case KILL:
    case SETPGID:
    case DUP2:
        m->m1_i2 = *(sp + 1);
        /* FALLTHRU */
    case ALARM:
    case SYSCONF:
    case SIGSUSPEND:
    case GETPGID:
    case CLOSE:
    case DUP:
    case UMASK:
    case CSLEEP:
    case SBRK:
        m->m1_i1 = *sp;
        break;

    case WAITPID:
    case WINFO:
    case STRERROR:
    case SIGPROCMASK:
    case OPEN:
    case READ:
    case WRITE:
    case CREAT:
    case MKNOD:
    case CHOWN:
    case CHMOD:
    case FSTAT:
    case GETDENT:
    case ACCESS:
    case MKDIR:
    case SIGNAL:
    case GETCWD:
        m->m1_i1 = *sp++;
        m->m1_p1 = (void*)*sp++;
        m->m1_i2 = *sp;
        break;
        
    case EXECVE:
        m->m1_p3 = (void*)*(sp + 2);
        /* FALLTHRU */
    case STAT:
    case LINK:
    case STATFS:
        m->m1_p2 = (void *)*(sp + 1);
        /* FALLTHRU */
    case TIMES:
    case BRK:
    case SIGPENDING:
    case PIPE:
    case CHDIR:
    case UNLINK:
        m->m1_p1 = (void*)*sp;
        break;

    case SIGACTION:
    case DPRINTF:
    case SETTIMER:
        m->m1_i1 = *sp++;
        m->m1_p1 = (void *)*sp++;
        m->m1_p2 = (void *)*sp;
        break;
    
    case FCNTL:
    case IOCTL:
        m->m1_i1 = *sp++;
        m->m1_i2 = *sp++;
        m->m1_p1 = (void *)get_virtual_addr(sp, who);
        break;

    // case GETPID:
    // case VFORK:
    // case SETSID:
    default:
        break;
    }
}

int set_syscall_reply(struct proc* who, int reply, int syscall_num){
    if(reply < 0){
        *(USER_ERRNO(who)) = -reply;
        reply = syscall_num == GETCWD ? 0 : -1;
    }
    who->ctx.m.regs[0] = reply;
    return reply;
}

int no_syscall(struct proc* who, struct message* m){
    klog("Process \"%s (%d)\" performed unknown system call %d\r\n", 
        who->name, who->pid, m->type);
    return -ENOSYS;
}

int syscall_not_implemented(struct proc* who, struct message *m){
    kerror("%s[%d] performed syscall %d that is not linked\n", 
            who->name, who->proc_nr, m->type);
    return -ENOSYS;
}

struct message *get_ipc_mesg(){
    return &_message;
}

int curr_proc_nr(){
    return curr_caller_proc_nr;
}

int curr_syscall_num(){
    return curr_syscall;
}

bool is_in_syscall(struct proc* who){
    return who->state & STATE_RECEIVING && curr_caller_proc_nr == who->proc_nr;
}

int syscall_reply(int reply, int dest,struct message* m){
    
    return syscall_reply2(curr_syscall_num(), reply, dest, m);
}

int syscall_reply2(int syscall_num, int reply, int dest, struct message* m){
    char buf[32];
    const char* p = buf;
    struct proc* pDest = get_proc(dest);
    if(trace_syscall && syscall_num > 0 && syscall_num < _NSYSCALL){
        if(reply < 0){
            p = kstr_error(reply);
        }else{
            kputd_buf(reply, buf);
        }
        klog("Syscall %s return %s to Proc %s[%d]\n",syscall_str[syscall_num] , p, pDest->name, dest);
    }
    if(pDest){
        m->type = syscall_num;
        m->reply_res = reply;
        return do_notify(SYSTEM, dest,m);
    }
    return -ESRCH;
}


void init_syscall_table(){

    SYSCALL_MAP(TIMES, do_times);     
    SYSCALL_MAP(EXIT, do_exit);    
    SYSCALL_MAP(FORK, do_fork);    
    SYSCALL_MAP(VFORK, do_vfork);     
    SYSCALL_MAP(EXECVE, do_execve);  
    SYSCALL_MAP(BRK, do_brk);         
    SYSCALL_MAP(ALARM, do_alarm);   
    SYSCALL_MAP(SIGACTION, do_sigaction); 
    SYSCALL_MAP(SIGRET, do_sigreturn); 
    SYSCALL_MAP(WAITPID, do_waitpid);      
    SYSCALL_MAP(KILL, do_kill);       
    SYSCALL_MAP(GETPID, do_getpid);   
    SYSCALL_MAP(WINFO, do_winfo);     
    SYSCALL_MAP(STRERROR, do_winix_strerror);      
    SYSCALL_MAP(DPRINTF, do_winix_dprintf);   
    SYSCALL_MAP(SYSCONF, do_sysconf);  
    SYSCALL_MAP(SIGSUSPEND, do_sigsuspend);
    SYSCALL_MAP(SIGPENDING, do_sigpending);
    SYSCALL_MAP(SIGPROCMASK, do_sigprocmask);
    SYSCALL_MAP(SETPGID, do_setpgid);
    SYSCALL_MAP(GETPGID, do_getpgid);
    SYSCALL_MAP(OPEN, do_open);
    SYSCALL_MAP(CREAT, do_creat);
    SYSCALL_MAP(CLOSE, do_close);
    SYSCALL_MAP(READ, do_read);
    SYSCALL_MAP(WRITE, do_write);
    SYSCALL_MAP(PIPE, do_pipe);
    SYSCALL_MAP(MKNOD, do_mknod);
    SYSCALL_MAP(CHDIR, do_chdir);
    SYSCALL_MAP(CHOWN, do_chown);
    SYSCALL_MAP(CHMOD, do_chmod);
    SYSCALL_MAP(STAT, do_stat);
    SYSCALL_MAP(FSTAT, do_fstat);
    SYSCALL_MAP(DUP, do_dup);
    SYSCALL_MAP(DUP2, do_dup2);
    SYSCALL_MAP(LINK, do_link);
    SYSCALL_MAP(UNLINK, do_unlink);
    SYSCALL_MAP(GETDENT, do_getdents);
    SYSCALL_MAP(ACCESS, do_access);
    SYSCALL_MAP(MKDIR, do_mkdir);
    SYSCALL_MAP(SYNC, do_sync);
    SYSCALL_MAP(LSEEK, do_lseek);
    SYSCALL_MAP(UMASK, do_umask);
    SYSCALL_MAP(FCNTL, do_fcntl);
    SYSCALL_MAP(IOCTL, do_ioctl);
    SYSCALL_MAP(SETSID, do_setsid);
    SYSCALL_MAP(CSLEEP, do_csleep);
    SYSCALL_MAP(GETPPID, do_getppid);
    SYSCALL_MAP(SIGNAL, do_signal);
    SYSCALL_MAP(SBRK, do_sbrk);  
    SYSCALL_MAP(STATFS, do_statfs);
    SYSCALL_MAP(GETCWD, do_getcwd);
    SYSCALL_MAP(TFORK, do_tfork);
    SYSCALL_MAP(SCHED_YIELD, do_sched_yield);
}




