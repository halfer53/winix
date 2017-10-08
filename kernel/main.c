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

/**
 * Entry point for WINIX.
 **/
void main() {
    struct proc *p = NULL;
    int i;

    init_bitmap();
    init_mem_table();
    init_proc();
    init_holes();
    init_sched();

    for(i = 0; i < sizeof(boot_table) / sizeof(struct boot_image); i++)
    {
        struct boot_image* pconfig = &boot_table[i];
        p = start_kernel_proc(pconfig->entry, pconfig->proc_nr, pconfig->name, pconfig->quantum);
        ASSERT(p != NULL);
    }

    p = start_user_proc(init_code, init_code_length, init_pc, init_offset, "init");
    add_free_mem(init_code, init_code_length);

    p = start_user_proc(shell_code,shell_code_length,shell_pc, shell_offset,"shell");
    p->parent = 1;//hack 
    add_free_mem(shell_code,shell_code_length);
    
    
    //Initialise exceptions
    init_exceptions();
    //Kick off first task. Note: never returns
    sched();
}
