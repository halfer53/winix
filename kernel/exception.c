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
#include <kernel/sched.h>
#include <winix/ksignal.h>
#include <libgen.h>

PRIVATE struct message _message;
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
    return &_message;
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

#define LIMIT 10
void _traceback_stack(struct proc* proc, ptr_t **stack_start, ptr_t** stack_end){
    vptr_t *vtext_start, *vtext_end;
    ptr_t **p = stack_start;
    reg_t *data, *instruction;
    char *filename;
    int i = 0;

    if (p >= stack_end){
        kwarn("sp %p past %p\n", (void *)p, (void *)stack_end);
        return;
    }

    vtext_start = get_virtual_addr(proc->text_top, proc);
    vtext_end = vtext_start + proc->text_size;

    // KDEBUG(("vtext_top: %p vtext_end %p stack %p stack_end %p\n", 
    //     (void *)vtext_start, (void *)vtext_end, (void *)p, (void *)stack_end));

    kprintf("Call Stack:\n");
    while (p < stack_end && i < LIMIT){
        data = *p;
        instruction = get_physical_addr(data, proc);
        if (*instruction > 0x100000 && vtext_start <= data && data <= vtext_end){
            kprintf("  - Virtual Addr: 0x%lx, Instruction: 0x%x\n", (uintptr_t)data, *instruction);
            i++;
        }
        p++;
    }
    filename = basename(proc->name);
    kprintf("HINT: type `python3 tools/kdbg.py %s <vir address>` to debug\n",
        filename);
}

void traceback_stack(struct proc* proc){
    ptr_t **stack_start, **stack_end;

    if (proc == curr_scheduling_proc && !in_interrupt()){
        wramp_syscall(WINIX_STACK_TRACE);
        return;
    }
    stack_start = (ptr_t **)get_physical_addr(proc->ctx.m.sp, proc);
    stack_end = (ptr_t **)proc->stack_top + proc->stack_size;

    _traceback_stack(proc, stack_start, stack_end);
}

void traceback_exception_stack(){
    if(!in_interrupt())
        return;
    kprintf("traceback_exception_stack\n");
    _traceback_stack(SYSTEM_TASK, get_sp(), (ptr_t **)get_exception_stack_bottom());
}

void traceback_current_stack(){
    if(in_interrupt())
        traceback_exception_stack();
    else
        traceback_stack(curr_scheduling_proc);
}

#define PRINT_DEBUG_REG(pc,sp,ra)\
    kprintf("$pc 0x%04lx, $sp 0x%04lx $ra 0x%04lx\n",pc,sp,ra)

void kreport_proc_sigsegv(struct proc* who){
    ptr_t* pc;
#ifdef _DEBUG
    if (who->sig_table[SIGSEGV].sa_handler != SIG_DFL)
        return;
    if (who->flags & PROC_NO_GPF)
        return;

    if(!is_vaddr_accessible(who->ctx.m.sp, who)){
        kprintf("\nStack Overflow");
    }
    kprintf("\nGeneral Protection Fault: \"%s (%d)\"\n",
        who->name,
        who->pid);
    kprintf("Rbase=0x%lx, Stack Top=0x%lx, vStack Top=0x%lx\n", 
        (uintptr_t)who->ctx.rbase,
        (uintptr_t)who->stack_top,
        (uintptr_t)get_virtual_addr(who->stack_top, who));
    pc = get_physical_addr(get_pc_ptr(who),who);

    kprintf("Virtual  ");
    PRINT_DEBUG_REG((uintptr_t)who->ctx.m.pc, (uintptr_t)who->ctx.m.sp, (uintptr_t)who->ctx.m.ra);

    kprintf("Physical ");
    PRINT_DEBUG_REG((uintptr_t)pc, 
        (uintptr_t)get_physical_addr(who->ctx.m.sp, who),
        (uintptr_t)get_physical_addr(who->ctx.m.ra, who));
    kprintf("Memory: ");
    kreport_ptable(who);  
    traceback_stack(who);
    
#endif
}


void trigger_gpf(struct proc* who){
    
    // is the current process a valid one?
    ASSERT(IS_PROCN_OK(who->proc_nr));
    
    if(IS_KERNEL_PROC(curr_scheduling_proc)){
        kreport_proc_sigsegv(curr_scheduling_proc);
        PANIC("kernel crashed");
    }

    // Kill process and call scheduler.
    send_sig(curr_scheduling_proc, SIGSEGV);
}

/**
 * General Protection Fault.
 *
 * Side Effects:
 *   Current process is killed.
 *   Scheduler is called (i.e. this handler does not return).
 **/
PRIVATE void gpf_handler() {
    trigger_gpf(curr_scheduling_proc);
    sched();
}


/**
 * System Call.
 *
 **/
PRIVATE void syscall_handler() {
    int dest, operation, ret;
    struct message *m;
    ptr_t *sp;
    bool direct_syscall = false;

    if(!curr_scheduling_proc )
        goto end;

    sp = get_physical_addr(curr_scheduling_proc->ctx.m.sp, curr_scheduling_proc);

    operation = *(sp);                // Operation is the first parameter on the stack

    if(operation > 0 && operation < _NSYSCALL){ // direct syscall mode
        m = USER_TMP_MESSAGE(curr_scheduling_proc);
        curr_scheduling_proc->flags |= DIRECT_SYSCALL;
        m->type = operation;
        dest = SYSTEM;
        sp++;
        set_syscall_mesg_exception(operation, sp, m, curr_scheduling_proc);
        operation = WINIX_SENDREC;
        direct_syscall = true;
        
    }else{ // traditional IPC mode
        dest = *(sp+1);                // Destination is second parameter on the stack
        m = (struct message *)get_physical_addr(*((unsigned long*)(sp+ 2)), curr_scheduling_proc);  // Message is the third parameter
    }

    m->src = curr_scheduling_proc->proc_nr;            // Don't trust the who to specify their own source process number

    // Decode operation
    switch(operation) {
        case WINIX_SENDREC:
            curr_scheduling_proc->state |= STATE_RECEIVING;
            /* FALLTHRU */

        case WINIX_SEND:
            ret = do_send(curr_scheduling_proc, dest, m);
            if(ret < 0)
                curr_scheduling_proc->state &= ~STATE_RECEIVING;
            break;

        case WINIX_RECEIVE:
            ret = do_receive(m);
            break;

        case WINIX_NOTIFY:
            ret = do_notify(m->src, dest,m);
            break;
        
        case WINIX_STACK_TRACE:
            traceback_stack(curr_scheduling_proc);
            break;

        default:
            ret = -EINVAL;
            break;
    }
    if (!direct_syscall || ret < 0){
        curr_scheduling_proc->ctx.m.regs[0] = ret;
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
    klog("Arith Exception: \"%s (%d)\" PC=0x%08lx.\r\n", 
        curr_scheduling_proc->name, curr_scheduling_proc->pid, (uintptr_t)curr_scheduling_proc->ctx.m.pc);
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
