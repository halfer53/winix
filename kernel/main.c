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
#include <init_bin.c>
#include <shell_bin.c>

struct proc* start_init();

/**
 * Entry point for WINIX.
 **/
void main() {
    struct proc *p;
    int i;

    init_bitmap();
    init_mem_table();
    init_proc();
    init_holes();
    init_sched();

    for(i = 0; i < ARRAY_SIZE(boot_table); i++){
        struct boot_image* task = &boot_table[i];
        p = start_kernel_proc(task->entry, task->proc_nr, task->name, task->quantum);
        ASSERT(p != NULL);
    }
    p = start_init();
    ASSERT(p != NULL);

    p = start_user_proc(shell_code,shell_code_length,shell_pc, shell_offset,"shell");
    p->parent = INIT;//hack 
    add_free_mem(shell_code,shell_code_length);
    
    
    //Initialise exceptions
    init_exceptions();
    //Kick off first task. Note: never returns
    sched();
}

struct proc* start_init(){
    struct proc* init = get_proc(INIT);
    proc_set_default(init);
    init->i_flags |= IN_USE | RUNNABLE;
    init->pid = INIT;
    if(exec_proc(init,init_code,init_code_length,init_pc,init_offset,"init"))
        PANIC("");
    add_free_mem(init_code, init_code_length);
    return init;
}
