/**
 * 
 * Main Entry point for Winix
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
#include "table.c"
#include <shell_codes.c>

/**
 * Entry point for WINIX.
 **/
void main() {
    struct proc *p = NULL;
    int i;
    //Print boot message.

    init_bitmap();
    init_mem_table();
    init_proc();
    init_timer();

    for(i = 0; i < sizeof(boot_table) / sizeof(struct proc_config); i++)
    {
        struct proc_config* pconfig = &boot_table[i];
        if(pconfig->iskernel_proc){
            p = start_kernel_proc(pconfig->entry, pconfig->name, pconfig->quantum);
            p->proc_nr = pconfig->pid;
        }else{//user proc
            p = start_user_proc(pconfig->image_array, pconfig->image_len, 
                        (size_t)pconfig->entry, pconfig->quantum, pconfig->name);
            
        }
        ASSERT(p != NULL);
    }

    p = start_user_proc(shell_code,shell_code_length,shell_pc, 3,"Shell");
    ASSERT(p != NULL);
    p->parent = 1;//hack 

    init_slab(shell_code,shell_code_length);
    
    new_timer(REBALANCE_TIMEOUT, rebalance_queues);
    //Initialise exceptions
    init_exceptions();
    //Kick off first task. Note: never returns
    sched();
}
