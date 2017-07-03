#include "../winix.h"



static unsigned long sigframe_code[] = {0x1ee10001,0x200d0000};
//addui sp,sp, 1
//syscall

PRIVATE size_t sizeof_sigframe(){
    sigframe_t arr[2];
	size_t size = (char*)&arr[1] - (char*)&arr[0];
	return size;
}

PRIVATE void build_user_stack(proc_t *who, void *src, size_t len){
    unsigned long *sp = get_physical_addr(who->sp,who);
    sp -= len;
    memcpy(sp,src,len );
    who->sp = get_virtual_addr(sp,who);
}

//TODO: set sigcode and len together

void real_send_signal(proc_t *who,int signum){
    unsigned long *sp;
    unsigned long *ra;
    proc_t *task;
    static message_t sigret_mesg;
    static sigframe_t sigframe;

    sp = who->sp;
    sp = get_physical_addr(sp,who);
    
    build_user_stack(who,who,PROCESS_CONTEXT_LEN);
    build_user_stack(who,sigframe_code,SIGFRAME_CODE_LEN);

    ra = who->sp;
    sigret_mesg.type = SYSCALL_SIGRET;
    sigret_mesg.i1 = signum;
    build_user_stack(who,&sigret_mesg,MESSAGE_LEN);

    sigframe.signum = signum;
    sigframe.operation = WINIX_SEND;
    sigframe.dest = SYSTEM_TASK;
    sigframe.pm = (message_t *)who->sp;
    build_user_stack(who,&sigframe,sizeof_sigframe());

    //restoring the context after sigreturn
    //space for saving the old context
    // sp -= PROCESS_CONTEXT_LEN;
    // memcpy(sp,who,PROCESS_CONTEXT_LEN );
    

    //assembly code for invoking sigreturn
    // sp -= SIGFRAME_CODE_LEN;
    // memcpy(sp,sigframe_code,SIGFRAME_CODE_LEN);
    

    //message_t for sigreturn system call
    
    // sp-= MESSAGE_LEN;
    // memcpy(sp,&sigret_mesg,MESSAGE_LEN);

    // size = sizeof_sigframe();
    // sp-= size;
    // memcpy(sp,&sigframe,size);

    //operating and dest for syscall
    //see exception.c
    // sp--;
    // *sp = (unsigned long)get_virtual_addr(sp+1,who);
    // *--sp = SYSTEM_TASK;
    // *--sp = WINIX_SEND;
    // *--sp = signum;

    who->pc = (void (*)())who->sig_table[signum].sa_handler;
    who->ra = ra;
    // who->sp = get_virtual_addr(sp,who);

    // ra = get_physical_addr(who->ra,who);
    // sp = get_physical_addr(who->sp+1,who);
    // kprintf("sig sp %x\n",who->sp);
    // kprintf("sig op %d dest %d mes %x VA ra %x sp %x pc %x\n",*sp, *(sp+1), *(sp+2), who->ra, who->sp,who->sig_table[signum].sa_handler);

    if(current_proc != who){
        enqueue_tail(ready_q[current_proc->priority], current_proc);
    }
    delete_proc(ready_q[who->priority],who);
    
    current_proc = who;

    current_proc->ticks_left = current_proc->quantum;
    who->flags = 0;//reset flags
    who->priority = TOP_PRIORITY;

    kprintf("%d %d %d\n",who->flags,who->proc_index,curr_mesg()->src);
    if(who->flags & RECEIVING && who->proc_index == curr_mesg()->src){
		get_proc(SYSTEM_TASK)->pc = &intr_syscall;
        kprintf("interrupt syscall\n");
	}
    process_overview();
    wramp_load_context();
}

void send_signal(proc_t *who, int signum){
    if(who->sig_table[signum].sa_handler == SIG_DFL){
        kprintf("Taking Default Action: kill %s [%d]",who->name,who->proc_index);
        end_process(who);
        return;
    }
    real_send_signal(who,signum);
}

