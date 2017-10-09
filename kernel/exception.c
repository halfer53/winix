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


#include "winix.h"
#include <winix/rex.h>
#include <kernel/clock.h>

PRIVATE struct message m;
PRIVATE int _expt_stack[EXCEPTION_STACK_SIZE];
PRIVATE int* _expt_stack_ptr;

//Number of exception sources
#define NUM_HANDLERS 16

//Handler prototypes
PRIVATE void button_handler();
PRIVATE void parallel_handler();
PRIVATE void serial1_handler();
PRIVATE void serial2_handler();
PRIVATE void gpf_handler();
PRIVATE void syscall_handler();
PRIVATE void break_handler();
PRIVATE void arith_handler();
PRIVATE void no_handler();

//Table of all handlers.
//Position in the table corresponds to relevant bit of $estat
PRIVATE expt_handler_t handlers[NUM_HANDLERS] = {
    no_handler,         //Undefined
    no_handler,         //Undefined
    no_handler,         //Undefined
    no_handler,         //Undefined
    no_handler,         //IRQ0
    button_handler,     //IRQ1
    clock_handler,         //IRQ2
    parallel_handler,     //IRQ3
    serial1_handler,     //IRQ4
    serial2_handler,     //IRQ5
    no_handler,         //IRQ6
    no_handler,         //IRQ7
    gpf_handler,         //GPF
    syscall_handler,     //SYSCALL
    break_handler,         //BREAK
    arith_handler         //ARITH
};

// counts the number of irqs during exception
PRIVATE int _irq_count = 0;

int irq_count(){
    return _irq_count;
}

void reset_irq_count(){
    _irq_count = 0;
}

int* get_exception_stack_top(){
    return _expt_stack;
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
    int stat = RexSp2->Stat;
    if(stat & 1){
        kputc2(RexSp2->Rx);
    }
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
    //is the current process a valid one?
    ASSERT(IS_PROCN_OK(current_proc->proc_nr));
    
    if(!CHECK_STACK(current_proc))
        kinfo("Stack Overflow\n");
    
#ifdef _DEBUG
    kinfo("General Protection Fault: \"%s (%d)\" Rbase=0x%x \n",
        current_proc->name,
        current_proc->pid,
        current_proc->rbase);
    pc = get_physical_addr(get_pc_ptr(current_proc),current_proc);

    kinfo("Virtual  ");
    PRINT_DEBUG_REG(get_virtual_addr(pc,current_proc),
                                    current_proc->sp,
                                    current_proc->ra);

    kinfo("Physical ");
    PRINT_DEBUG_REG(pc, 
        get_physical_addr(current_proc->sp, current_proc),
        get_physical_addr(current_proc->ra, current_proc));    

    kinfo("Current Instruction: 0x%08x\n",*pc);

#if _DEBUG == 2
    kinfo("$1: 0x%08x, $2, 0x%08x, $3, 0x%08x\n",current_proc->regs[0],
                            current_proc->regs[1],current_proc->regs[2]);
    kinfo("$4: 0x%08x, $5, 0x%08x, $6, 0x%08x\n",current_proc->regs[3],
                            current_proc->regs[4],current_proc->regs[5]);
    kinfo("$7: 0x%08x, $8, 0x%08x, $9, 0x%08x\n",current_proc->regs[6],
                            current_proc->regs[7],current_proc->regs[8]);
    kinfo("$10: 0x%08x, $11, 0x%08x, $12, 0x%08x\n",current_proc->regs[9],
                            current_proc->regs[10],current_proc->regs[11]);
    kinfo("$13: 0x%08x, $sp, 0x%08x, $ra, 0x%08x\n",current_proc->regs[12],
                            current_proc->regs[13],current_proc->regs[14]);
#endif
#endif
    //Kill process and call scheduler.
    send_sig(current_proc,SIGSEGV);
    sched();
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

    sp = get_physical_addr(current_proc->sp, current_proc);

    operation = *(sp);                //Operation is the first parameter on the stack
    dest = *(sp+1);                //Destination is second parameter on the stack
    m = (struct message *)get_physical_addr(*(sp+ 2), current_proc);  //Message is the third parameter
    m->src = current_proc->proc_nr;            //Don't trust the who to specify their own source process number

    retval = (int*)&current_proc->regs[0];        //Result is returned in register $1

    //Decode operation
    switch(operation) {
        case WINIX_SENDREC:
            current_proc->s_flags |= RECEIVING;
            //fall through to send

        case WINIX_SEND:
            *retval = do_send(dest, m);
            break;

        case WINIX_RECEIVE:
            *retval = do_receive(m);
            break;

        case WINIX_NOTIFY:
            *retval = do_notify(dest,m);
            break;

        default:
            *retval = ERR;
            break;
    }

    //A system call could potentially make a high-priority process runnable.
    //Run scheduler.
    sched();
}

/**
 * Breakpoint.
 **/
PRIVATE void break_handler() {
    //TODO: implement handling of breakpoints
}

/**
 * Arithmetic Exception.
 *
 * Side Effects:
 *   Current process is killed, and scheduler is called (i.e. this handler does not return).
 **/
PRIVATE void arith_handler() {
    KDEBUG(("Arith Exception: \"%s (%d)\" PC=0x%08x.\r\n", 
        current_proc->name, current_proc->pid, current_proc->pc));
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
    //Loop through $estat and call all relevant handlers.
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
    _expt_stack_ptr = _expt_stack;
    *_expt_stack_ptr = STACK_MAGIC;
    _expt_stack_ptr += EXCEPTION_STACK_SIZE - 1;

    wramp_set_handler(exception_handler);
    RexTimer->Load = 2400 / get_hz(); //currently 60 Hz
    enable_interrupt();
}
