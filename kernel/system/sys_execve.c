/**
 * Syscall in this file: execve
 * Input:   
 *
 * Return:  
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:08:30
 * 
*/
#include "../winix.h"
#include <winix/srec.h>

PRIVATE unsigned int syscall_code = 0x200d0000;

int do_exec(struct proc *who, struct message *m){
    return OK;
}


/**
* malloc a new memory and write the values of lines into that address
* the process is updated
**/
int exec_proc(struct proc *p,size_t *lines, size_t length, size_t entry, int quantum, const char *name){

    set_proc(p, (void (*)())entry, quantum, name);
    if(alloc_proc_mem(p, length, USER_STACK_SIZE , USER_HEAP_SIZE, 
                         PROC_SET_SP | PROC_SET_HEAP) != OK){
        return ERR;
    }
    build_initial_stack(p, 0, NULL, initial_env, get_proc(SYSTEM));
    memcpy(p->rbase, lines , length);

    p->length = length;
    enqueue_schedule(p);
    return OK;
}


int build_initial_stack(struct proc* who, int argc, char** argv, char** env, struct proc* srcproc){
    struct initial_frame init_stack;
    struct initial_frame* pstack = &init_stack;
    ptr_t* sp_btm = get_physical_addr(who->sp,who);
    int env_len = 0;
    char **env_ptr;
    char *v;
    unsigned int *env_ptr_list, *p;

    env = (char **)get_physical_addr(env, srcproc);
    env_ptr = env;

    //get the length of environment variables
    while(*env_ptr++)   env_len++;
    env_len++; //for the last null terminator

    //malloc the pointer for each environment variable
    env_ptr_list = kmalloc(env_len);
    p = env_ptr_list;

    //copy each of the environment to the user stack
    env_ptr = env;
    while((v = *env_ptr) != NULL){
        v = (char *)get_physical_addr(v, srcproc);
        *p++ = (unsigned int)copyto_user_heap(who, v, strlen(v)+1);
        //save the pointer of the environment as well
        env_ptr++;
    }
    *p = 0;

    //copy the pointers of environment to the user stack
    copyto_user_stack(who, env_ptr_list, env_len);

    *sp_btm = (unsigned int)who->sp;
    //setup argc and argv before
    who->ra = who->sp -1;

    pstack->operation = WINIX_SENDREC;
    pstack->dest = SYSTEM;
    pstack->pm = (struct message*)(who->sp - sizeof(syscall_code) - sizeof(struct message));
    pstack->m.type = SYSCALL_EXIT;
    pstack->m.m1_i1 = EXIT_MAGIC;
    pstack->syscall_code = syscall_code;
    
    copyto_user_stack(who, pstack, sizeof(struct initial_frame));

    kfree(env_ptr_list);
    return OK;
}

