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

PRIVATE struct message m;
PRIVATE int who_proc_nr;
PRIVATE int curr_syscall;
PRIVATE struct proc *who;

PRIVATE ucontext_t recv_ctx;

void test_list();

/**
 * Entry point for system task.
 **/
void system_main() {
    int reply;
    syscall_handler_t handler;
    struct message* mesg = &m;

    kreport_sysinfo();
    getcontext(&recv_ctx);
    
    //Receive message, do work, repeat.
    while(true) {
        //get a message
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
                syscall_reply(reply, who_proc_nr, mesg);
        }

        syscall_region_end();
        
    }
}

/**
 * print sys info of text, data and bss segment size
 */
 void kreport_sysinfo(){
    int free_mem_begin, mem_end;
    free_mem_begin = peek_next_free_page() * PAGE_LEN;
    mem_end = peek_last_free_page() * PAGE_LEN;
    kprintf("\r\nWINIX v%d.%d\r\n", MAJOR_VERSION, MINOR_VERSION);
    kprintf("Text Segment: 0x%08x - 0x%08x\r\n", &TEXT_BEGIN, &TEXT_END);
    kprintf("Data Segment: 0x%08x - 0x%08x\r\n", &DATA_BEGIN, &DATA_END);
    kprintf("BSS Segment:  0x%08x - 0x%08x\r\n", &BSS_BEGIN, &BSS_END);
    kprintf("Unallocated:  0x%08x - 0x%08x\r\n", free_mem_begin, mem_end);
    kprintf("%d kWords Free\r\n", 
    ((unsigned int)(mem_end - free_mem_begin + PAGE_LEN)) / PAGE_LEN); //inclusive
}

void syscall_region_begin(){

    //Bill the user proc's sys_used_time while executing syscall
    //on behalf of the user process
    set_bill_ptr(who);
    get_proc(SYSTEM)->flags |= BILLABLE;
    curr_syscall = m.type;

    //the following two are defensive statements
    //to ensure the caller is suspended while the system
    //is handling the system call
    who->flags |= RECEIVING;
    dequeue_schedule(who);

    //Make sure system doesn't send a message to itself
    ASSERT(who != NULL && who_proc_nr != SYSTEM); 
}

void syscall_region_end(){
    get_proc(SYSTEM)->flags &= ~BILLABLE;
    //reset messages
    memset(&m, 0, sizeof(struct message));
    who_proc_nr = 0;
    curr_syscall = 0;
}

// /**
//  * interrupt the current executing system call
//  * and start receiving syscalls again
//  */
// void intr_syscall(){
//     if(who_proc_nr){
//         syscall_reply(EINTR, who_proc_nr, &m);
//         setcontext(&recv_ctx);
//     }
// }

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
    return who->state & RECEIVING && who_proc_nr == who->proc_nr;
}


void init_syscall_table(){

    SYSCALL_MAP(TIMES, do_times);      //1
    SYSCALL_MAP(EXIT, do_exit);        //2
    SYSCALL_MAP(FORK, do_fork);        //3
    SYSCALL_MAP(VFORK, do_vfork);      //4
    SYSCALL_MAP(EXECVE, do_exec);      //5
    SYSCALL_MAP(BRK, do_brk);          //6
    SYSCALL_MAP(ALARM, do_alarm);      //7
    SYSCALL_MAP(SIGACTION, do_sigaction); //8
    SYSCALL_MAP(SIGRET, do_sigreturn); //9
    SYSCALL_MAP(WAITPID, do_waitpid);        //10
    SYSCALL_MAP(KILL, do_kill);        //11
    SYSCALL_MAP(GETPID, do_getpid);    //12
    SYSCALL_MAP(WINFO, do_winfo);      //13
    SYSCALL_MAP(GETC, do_getc);        //14
    SYSCALL_MAP(PRINTF, do_printf);    //15
    SYSCALL_MAP(SYSCONF, do_sysconf);  //16
    SYSCALL_MAP(SIGSUSPEND, do_sigsuspend);
    SYSCALL_MAP(SIGPENDING, do_sigpending);
    SYSCALL_MAP(SIGPROCMASK, do_sigprocmask);
    SYSCALL_MAP(SETPGID, do_setpgid);
    SYSCALL_MAP(GETPGID, do_getpgid);
}




