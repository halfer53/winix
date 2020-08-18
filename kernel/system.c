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

PRIVATE struct message m;
PRIVATE int who_proc_nr;
PRIVATE int curr_syscall;
PRIVATE struct proc *who;
bool trace_syscall = false;

PRIVATE ucontext_t recv_ctx;


/**
 * Entry point for system task.
 **/
void system_main() {
    int reply;
    syscall_handler_t handler;
    struct message* mesg = &m;

    kreport_sysinfo();
    getcontext(&recv_ctx);
    // Receive message, do work, repeat.
    while(true) {
        // get a message
        winix_receive(mesg);
        who_proc_nr = mesg->src;
        who = get_proc(who_proc_nr);

        syscall_region_begin();

        if(mesg->type >= 1 && mesg->type <= _NSYSCALL)
            handler = syscall_table[mesg->type];
        else
            handler = no_syscall;
        
        reply = handler(who,mesg);
        

        switch(reply){
            case SUSPEND:
            case DONTREPLY:
                break;
            default:
                syscall_reply2(mesg->type, reply, who_proc_nr, mesg);
        }

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
    kprintf("Text Segment: 0x%08x - 0x%08x\n", &TEXT_BEGIN, &TEXT_END);
    kprintf("Data Segment: 0x%08x - 0x%08x\n", &DATA_BEGIN, &DATA_END);
    kprintf("BSS Segment:  0x%08x - 0x%08x\n", &BSS_BEGIN, &BSS_END);
    kprintf("Unallocated:  0x%08x - 0x%08x\n", free_mem_begin, mem_end);
    kprintf("Root Disk %d Blocks Used, %d Remaining\n", sb->s_block_inuse, sb->s_free_blocks);
    kprintf("%d Pages Free\n", 
    ((unsigned int)(mem_end - free_mem_begin + PAGE_LEN)) / PAGE_LEN); // inclusive
}

void syscall_region_begin(){

    // Bill the user proc's sys_used_time while executing syscall
    // on behalf of the user process
    set_bill_ptr(who);
    get_proc(SYSTEM)->flags |= BILLABLE;
    curr_syscall = m.type;

    // the following two are defensive statements
    // to ensure the caller is suspended while the system
    // is handling the system call
    who->state |= STATE_RECEIVING;
    dequeue_schedule(who);

    if(is_debugging_syscall())
        if(m.type >= 1 && m.type <= _NSYSCALL)
            kprintf_syscall_request(m.type, m.src);
    
    SET_CALLER(who);
    // Make sure system doesn't send a message to itself
    ASSERT(who != NULL && who_proc_nr != SYSTEM); 
}

void syscall_region_end(){
    get_proc(SYSTEM)->flags &= ~BILLABLE;
    // reset messages
    memset(&m, 0, sizeof(struct message));
    who_proc_nr = 0;
    curr_syscall = 0;
}

int no_syscall(struct proc* who, struct message* m){
    KDEBUG(("Process \"%s (%d)\" performed unknown system call %d\r\n", 
        who->name, who->pid, m->type));
    return ENOSYS;
}

struct message *curr_mesg(){
    return &m;
}

int curr_proc_nr(){
    return who_proc_nr;
}

int curr_syscall_num(){
    return curr_syscall;
}

bool is_in_syscall(struct proc* who){
    return who->state & STATE_RECEIVING && who_proc_nr == who->proc_nr;
}

int syscall_reply(int reply, int dest,struct message* m){
    
    return syscall_reply2(curr_syscall_num(), reply, dest, m);
}

int syscall_reply2(int syscall_num, int reply, int dest, struct message* m){
    char buf[32];
    char* p = buf;
    struct proc* pDest = get_proc(dest);
    if(trace_syscall){
        if(reply < 0){
            p = (char*)kstr_error(reply);
        }else{
            kputd_buf(reply, buf);
        }
        KDEBUG(("Syscall %s return %s to Proc %s[%d]\n",syscall_str[syscall_num] , p, pDest->name, dest));
    }
    if(pDest){
        m->reply_res = reply;
        return do_notify(SYSTEM, dest,m);
    }
    return ERR;
}


void init_syscall_table(){

    SYSCALL_MAP(TIMES, do_times);      //1
    SYSCALL_MAP(EXIT, do_exit);        //2
    SYSCALL_MAP(FORK, do_fork);        //3
    SYSCALL_MAP(VFORK, do_vfork);      //4
    SYSCALL_MAP(EXECVE, do_execve);      //5
    SYSCALL_MAP(BRK, do_brk);          //6
    SYSCALL_MAP(ALARM, do_alarm);      //7
    SYSCALL_MAP(SIGACTION, do_sigaction); //8
    SYSCALL_MAP(SIGRET, do_sigreturn); //9
    SYSCALL_MAP(WAITPID, do_waitpid);        //10
    SYSCALL_MAP(KILL, do_kill);        //11
    SYSCALL_MAP(GETPID, do_getpid);    //12
    SYSCALL_MAP(WINFO, do_winfo);      //13
    SYSCALL_MAP(WINIX_STRERROR, do_winix_strerror);        //14
    SYSCALL_MAP(WINIX_DPRINTF, do_winix_dprintf);    //15
    SYSCALL_MAP(SYSCONF, do_sysconf);  //16
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
}




