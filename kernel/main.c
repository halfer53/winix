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

#include <kernel/kernel.h>
#include <kernel/table.h>
#include <winix/bitmap.h>
#include <init_bin.c>
#include <shell_bin.c>

void init_kernel_tasks();
void start_init();
void start_bins();

/**
 * Entry point for WINIX.
 **/
void main() {
    init_bitmap();
    init_mem_table();
    init_proc();
    init_holes();
    init_sched();
    init_syscall_table();

    init_kernel_tasks();
    start_init();
    start_bins();

    init_exceptions();
    sched();
}

void init_kernel_tasks(){
    int i;
    struct proc* p;
    for(i = 0; i < ARRAY_SIZE(boot_table); i++){
        struct boot_image* task = &boot_table[i];
        p = start_kernel_proc(task->entry, task->proc_nr, task->name, task->quantum, task->priority);
        ASSERT(p != NULL);
    }
    add_free_mem(boot_table, sizeof(boot_table));
}

void start_init(){
    struct proc* init = get_proc(INIT);
    proc_set_default(init);
    init->state = STATE_RUNNING;
    init->flags = IN_USE;
    init->pid = 1;
    if(exec_proc(init,init_code,init_code_length,init_pc,init_offset,"init"))
        PANIC("init");
    add_free_mem(init_code, init_code_length);
}

void start_bins(){
    struct proc* p;
    p = start_user_proc(shell_code,shell_code_length, shell_pc, shell_offset,"shell");
    p->parent = INIT;// hack 
    add_free_mem(shell_code,shell_code_length);
}
