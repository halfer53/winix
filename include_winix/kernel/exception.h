/**
 * 
 * Exception-handling routines for WINIX.
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

#ifndef _K_EXCEPTION_H_
#define _K_EXCEPTION_H_ 1

typedef void (*expt_handler_t)( void );

/**
 * Initialises Exception Handlers and configures timer.
 **/
void init_exceptions();
int irq_count();
int* get_exception_stack_top();
int* get_exception_stack_bottom();
struct message* get_exception_m();
void preempt_currproc();
void reset_irq_count();

#define in_interrupt()  (irq_count())
#define enable_interrupt()  RexTimer->Ctrl = 3
#define disable_interrupt() RexTimer->Ctrl = 0
#define EXCEPTION_STACK_SIZE    (PAGE_LEN - 1)
void register_irq(int irq, expt_handler_t handler);
void trigger_gpf(struct proc* who);
void kreport_proc_sigsegv(struct proc* who);
void traceback_stack(struct proc* proc);
void traceback_exception_stack();
void traceback_current_stack();

#endif
