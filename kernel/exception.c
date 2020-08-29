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
    if(current_proc){
        enqueue_schedule(current_proc);
        current_proc = NULL;
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
    ptr_t* sp;
    ptr_t* pc;
    // is the current process a valid one?
    ASSERT(IS_PROCN_OK(current_proc->proc_nr));
    
    if(*current_proc->stack_top != STACK_MAGIC)
        kprintf("Stack Overflow\n");
    
#ifdef _DEBUG
    kprintf("\nGeneral Protection Fault: \"%s (%d)\" Rbase=0x%x \n",
        current_proc->name,
        current_proc->pid,
        current_proc->ctx.rbase);
    pc = get_physical_addr(get_pc_ptr(current_proc),current_proc);

    kprintf("Virtual  ");
    PRINT_DEBUG_REG(get_virtual_addr(pc,current_proc),
                                    current_proc->ctx.m.sp,
                                    current_proc->ctx.m.ra);

    kprintf("Physical ");
    PRINT_DEBUG_REG(pc, 
        get_physical_addr(current_proc->ctx.m.sp, current_proc),
        get_physical_addr(current_proc->ctx.m.ra, current_proc));    

    // kprintf("Current Instruction: 0x%08x\n",*pc);
#endif

    
    if(IS_KERNEL_PROC(current_proc))
        _panic("kernel crashed",NULL);

    // Kill process and call scheduler.
    send_sig(current_proc,SIGSEGV);
    sched();
}


void do_direct_syscall(int num){
    struct message msg;
    

}

/**
 * System Call.
 *
 **/
PRIVATE void syscall_handler() {
    int dest, operation;
    struct message *m;
    int *retval;
    ptr_t *sp;

    sp = get_physical_addr(current_proc->ctx.m.sp, current_proc);

    operation = *(sp);                // Operation is the first parameter on the stack

    if(operation > 0 && operation < _NSYSCALL){ // direct syscall mode
        // m = (struct message*)sys_sbrk(current_proc, sizeof(struct message));
        m = kmalloc(sizeof(struct message));
        m->type = operation;
        dest = SYSTEM;
        sp++;
        switch (operation)
        {
        case LSEEK:
            m->m1_i3 = *(sp + 2);
        case DUP2:
            m->m1_i2 = *(sp + 1);
        case CLOSE:
        case DUP:
        case UMASK:
            m->m1_i1 = *sp;
            break;

        case READ:
        case WRITE:
        case MKNOD:
        case CHOWN:
        case CHMOD:
        case FSTAT:
        case GETDENT:
        case ACCESS:
        case MKDIR:
            m->m1_i1 = *sp++;
            m->m1_p1 = (void*)*sp++;
            m->m1_i2 = *sp;
            break;

        case STAT:
        case LINK:
            m->m1_p2 = (void *)*(sp + 1);
        case PIPE:
        case CHDIR:
        case UNLINK:
            m->m1_p1 = (void*)*sp;
            break;
        
        default:
            break;
        }
        operation = WINIX_SENDREC;
        current_proc->flags |= DIRECT_SYSCALL;

    }else{ // traditional IPC mode

        dest = *(sp+1);                // Destination is second parameter on the stack
        m = (struct message *)get_physical_addr(*(sp+ 2), current_proc);  // Message is the third parameter
    }

    m->src = current_proc->proc_nr;            // Don't trust the who to specify their own source process number
    retval = (int*)&current_proc->ctx.m.regs[0];        // Result is returned in $1

    // Decode operation
    switch(operation) {
        case WINIX_SENDREC:
            current_proc->state |= STATE_RECEIVING;
            // fall through to send

        case WINIX_SEND:
            *retval = do_send(dest, m);
            if(*retval < 0)
                current_proc->state &= ~STATE_RECEIVING;
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

    // A system call could potentially make a high-priority process runnable.
    // Run scheduler.
    sched();
}

/**
 * Breakpoint.
 **/
PRIVATE void break_handler() {
}

/**
 * Arithmetic Exception.
 *
 * Side Effects:
 *   Current process is killed, and scheduler is called (i.e. this handler does not return).
 **/
PRIVATE void arith_handler() {
    kprintf("Arith Exception: \"%s (%d)\" PC=0x%08x.\r\n", 
        current_proc->name, current_proc->pid, current_proc->ctx.m.pc);
    send_sig(current_proc,SIGFPE);
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
    RexTimer->Load = 2400 / get_hz(); // currently 60 Hz
    enable_interrupt();
}
