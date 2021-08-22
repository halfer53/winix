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
#include <fs/fs_methods.h>

void init_kernel_tasks();
void start_init();
void start_bins();

/**
 * Entry point for WINIX.
 **/
void main() {
    int bss_len = &BSS_END - &BSS_BEGIN;
    memset(&BSS_BEGIN, 0, bss_len);
    
    init_bitmap();
    init_dev();
    init_fs();
    init_tty();
    init_drivers();
    
    init_mem_table();
    init_proc();
    init_sched();
    init_syscall_table();
    

    init_kernel_tasks();
    start_init();

    init_exceptions();
    sched();
}

void init_kernel_tasks(){
    int i;
    struct proc* p;
    for(i = 0; i < ARRAY_SIZE(boot_table); i++){
        struct boot_image* task = &boot_table[i];
        p = start_kernel_proc(task);
        ASSERT(p != NULL);
    }
    add_free_mem(boot_table, sizeof(boot_table));
}

void start_init(){
    int ret;
    struct proc* init = proc_table + INIT;
    proc_set_default(init);
    init->state = STATE_RUNNABLE;
    init->flags = IN_USE;
    init->pid = INIT;
    ret = exec_welf(init, INIT_PATH, init_argv, initial_env, true);
    if(ret != OK && ret != DONTREPLY){
        kerror("%d\n", ret);
        PANIC("init");
    }
}

