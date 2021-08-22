/**
 * 
 * Winix exception handler
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
#include <kernel/exception.h>
#include <winix/rex.h>
#include <kernel/clock.h>

PRIVATE struct message m;
PRIVATE int* _expt_stack_ptr;

// Number of exception sources
#define NUM_HANDLERS 16

// Handler prototypes
PRIVATE void button_handler();
PRIVATE void parallel_handler();
PRIVATE void serial1_handler();
PRIVATE void serial2_handler();
PRIVATE void gpf_handler();
PRIVATE void syscall_handler();
PRIVATE void break_handler();
PRIVATE void arith_handler();
PRIVATE void no_handler();

// Table of all handlers.
// Position in the table corresponds to relevant bit of $estat
PRIVATE expt_handler_t handlers[NUM_HANDLERS] = {
    no_handler,         // Undefined
    no_handler,         // Undefined
    no_handler,         // Undefined
    no_handler,         // Undefined
    no_handler,         // IRQ0
    button_handler,     // IRQ1
    clock_handler,         // IRQ2
    parallel_handler,     // IRQ3
    serial1_handler,     // IRQ4
    serial2_handler,     // IRQ5
    no_handler,         // IRQ6
    no_handler,         // IRQ7
    gpf_handler,         // GPF
    syscall_handler,     // SYSCALL
    break_handler,         // BREAK
    arith_handler         // ARITH
};

// counts the number of irqs during exception
PRIVATE int _irq_count = 0;

int irq_count(){
    return _irq_count;
}

void register_irq(int irq, expt_handler_t handler){
    if( irq < 0 || irq >= 15)
        return;
    handlers[irq] = handler;
}

void preempt_currproc(){
    if(curr_scheduling_proc){
        enqueue_schedule(curr_scheduling_proc);
        curr_scheduling_proc = NULL;
    }
}

void reset_irq_count(){
    _irq_count = 0;
}

int* get_exception_stack_top(){
    return (int*)0;
}

int* get_exception_stack_bottom(){
    return _expt_stack_ptr;
}

struct message* get_exception_m(){
    return &m;
}

/**
 * User Interrupt Button (IRQ1)
 **/
PRIVATE void button_handler() {
    RexUserInt->Iack = 0;
}


/**
 * Parallel Port (IRQ3)
 **/
PRIVATE void parallel_handler() {
    RexParallel->Iack = 0;
}

/**
 * Serial Port 1 (IRQ4)
 **/
PRIVATE void serial1_handler() {
    RexSp1->Iack = 0;
}

/**
 * Serial Port 2 (IRQ5)
 **/
PRIVATE void serial2_handler() {
    // int stat = RexSp2->Stat;
    // if(stat & 1){
    //     kputc2(RexSp2->Rx);
    // }
    RexSp2->Iack = 0;
}

#define PRINT_DEBUG_REG(pc,sp,ra)\
    kprintf("$pc 0x%04x, $sp 0x%04x $ra 0x%04x\n",pc,sp,ra)

/**
 * General Protection Fault.
 *
 * Side Effects:
 *   Current process is killed.
 *   Scheduler is called (i.e. this handler does not return).
 **/
PRIVATE void gpf_handler() {
    ptr_t* pc;
    int sp;
    int val;
    // is the current process a valid one?
    ASSERT(IS_PROCN_OK(curr_scheduling_proc->proc_nr));
    trace_syscall = false;
    stop_debug_scheduling();
#ifdef _DEBUG
    sp = (int)(get_physical_addr(curr_scheduling_proc->ctx.m.sp, curr_scheduling_proc)) - (int)curr_scheduling_proc->stack_top;
    if(!is_vaddr_accessible(curr_scheduling_proc->ctx.m.sp, curr_scheduling_proc)){
        kprintf("\nStack Overflow");
    }
    kprintf("\nGeneral Protection Fault: \"%s (%d)\" Rbase=0x%x Stack Top=0x%x\n",
        curr_scheduling_proc->name,
        curr_scheduling_proc->pid,
        curr_scheduling_proc->ctx.rbase,
        curr_scheduling_proc->stack_top);
    pc = get_physical_addr(get_pc_ptr(curr_scheduling_proc),curr_scheduling_proc);

    kprintf("Virtual  ");
    PRINT_DEBUG_REG(get_virtual_addr(pc,curr_scheduling_proc),
                                    sp,
                                    curr_scheduling_proc->ctx.m.ra);

    kprintf("Physical ");
    PRINT_DEBUG_REG(pc, 
        get_physical_addr(curr_scheduling_proc->ctx.m.sp, curr_scheduling_proc),
        get_physical_addr(curr_scheduling_proc->ctx.m.ra, curr_scheduling_proc));  
    kreport_ptable(curr_scheduling_proc);  

    // kprintf("Current Instruction: 0x%08x\n",*pc);
#endif

    
    if(IS_KERNEL_PROC(curr_scheduling_proc))
        _panic("kernel crashed",NULL);

    // Kill process and call scheduler.
    send_sig(curr_scheduling_proc,SIGSEGV);
    sched();
}


/**
 * System Call.
 *
 **/
PRIVATE void syscall_handler() {
    int dest, operation;
    struct message *m;
    struct proc* target;
    int *retval;
    int reply;
    ptr_t *sp;

    if(!curr_scheduling_proc )
        goto end;

    sp = get_physical_addr(curr_scheduling_proc->ctx.m.sp, curr_scheduling_proc);

    operation = *(sp);                // Operation is the first parameter on the stack

    if(operation > 0 && operation < _NSYSCALL){ // direct syscall mode
        // m = (struct message*)sys_sbrk(curr_scheduling_proc, sizeof(struct message));
        m = USER_TMP_MESSAGE(curr_scheduling_proc);
        curr_scheduling_proc->flags |= DIRECT_SYSCALL;
        m->type = operation;
        dest = SYSTEM;
        sp++;
        set_syscall_mesg_exception(operation, sp, m, curr_scheduling_proc);
        operation = WINIX_SENDREC;
        
    }else{ // traditional IPC mode

        dest = *(sp+1);                // Destination is second parameter on the stack
        m = (struct message *)get_physical_addr(*(sp+ 2), curr_scheduling_proc);  // Message is the third parameter
    }

    m->src = curr_scheduling_proc->proc_nr;            // Don't trust the who to specify their own source process number
    retval = (int*)&curr_scheduling_proc->ctx.m.regs[0];        // Result is returned in $1

    // Decode operation
    switch(operation) {
        case WINIX_SENDREC:
            curr_scheduling_proc->state |= STATE_RECEIVING;
            // fall through to send

        case WINIX_SEND:
            *retval = do_send(dest, m);
            if(*retval < 0)
                curr_scheduling_proc->state &= ~STATE_RECEIVING;
            break;

        case WINIX_RECEIVE:
            *retval = do_receive(m);
            break;

        case WINIX_NOTIFY:
            *retval = do_notify(m->src, dest,m);
            break;

        default:
            *retval = ERR;
            break;
    }

end:
    // A system call could potentially make a high-priority process runnable.
    // Run scheduler.
    sched();
}

/**
 * Breakpoint.
 **/
PRIVATE void break_handler() {
    KDEBUG(("break"));
}

/**
 * Arithmetic Exception.
 *
 * Side Effects:
 *   Current process is killed, and scheduler is called (i.e. this handler does not return).
 **/
PRIVATE void arith_handler() {
    kprintf("Arith Exception: \"%s (%d)\" PC=0x%08x.\r\n", 
        curr_scheduling_proc->name, curr_scheduling_proc->pid, curr_scheduling_proc->ctx.m.pc);
    send_sig(curr_scheduling_proc,SIGFPE);
    sched();
}

/**
 * Generic handler, should never be called.
 *
 * Side Effects:
 *   System Panic! Does not return.
 **/
PRIVATE void no_handler() {
    PANIC("Unhandled Exception");
}

/**
 * The global exception handler.
 * All relevant exception handlers will be called.
 **/
PRIVATE void exception_handler(int estat) {
    int i;
    _irq_count = 0;
    // Loop through $estat and call all relevant handlers.
    for(i = NUM_HANDLERS; i >= 0; i--) {
        if(estat & (1 << i)) {
            _irq_count++;
            handlers[i]();
        }
    }
}

/**
 * Initialises interrupts and exceptions.
 *
 * Side Effects:
 *   System exception handler is initialised.
 *   Timer is configured to generate regular interrupts.
 **/
void init_exceptions() {
    /**
     * Exception use the first page as the stack
     */
    _expt_stack_ptr = (int *)0;
    *_expt_stack_ptr = STACK_MAGIC;
    _expt_stack_ptr += EXCEPTION_STACK_SIZE;

    wramp_set_handler(exception_handler);
    RexTimer->Load = 2400 / HZ; // currently 60 Hz
    enable_interrupt();
}
