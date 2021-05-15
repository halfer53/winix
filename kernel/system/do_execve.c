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
#include <kernel/kernel.h>
#include <winix/srec.h>
#include <kernel/table.h>
#include <winix/welf.h>
#include <sys/fcntl.h>
#include <fs/fs_methods.h>
#include <fs/cache.h>
#include <winix/mm.h>
#include <winix/bitmap.h>
#include <winix/page.h>
#include <winix/dev.h>

#define ASM_ADDUI_SP   (0x1ee10000)
#define ASM_ADDUI_SP_SP_2   (0x1ee10002)

struct exit_code{
    unsigned int i_code1;
    unsigned int i_code2;
};

struct initial_frame{
    int argc;
    char **argv;
    struct syscall_frame_comm i_base;
    struct exit_code code;
};

struct string_array{
    char **array;
    int size;
};

int do_execve(struct proc *who, struct message *m){
    char* path = m->m1_p1;
    char** argv = m->m1_p2;
    char** envp = m->m1_p3;

    if(is_vaddr_accessible(path, who) && is_vaddr_accessible(argv, who) && is_vaddr_accessible(envp, who)){
        path = (char *)get_physical_addr(path, who);
        argv = (char**)get_physical_addr(argv, who);
        envp = (char**)get_physical_addr(envp, who);
        return exec_welf(who, path, argv, envp, false);
    }
    return EFAULT;
}

int copy_stirng_array(struct string_array *arr, char* from[], struct proc* who, bool is_physical_addr){
    int limit = 32;
    int count = 32;
    int size = 0, i, strsize, j, ret;
    char* ptr, *physical;
    char **pptr = from;

    arr->size = 0;
    if(!from)
        return OK;
    while(*pptr++ && count-- > 0){
        size++;
    }
    size++;
    arr->array = (char**)kmalloc(size);
    if(!arr->array){
        return ENOMEM;
    }
    arr->size = size;
    pptr = from;
    count = limit;
    for(i = 0; i < size - 1; i++){
        physical = from[i];
        if(!is_physical_addr){
            physical = (char*)get_physical_addr(physical, who);
        }
        strsize = strlen(physical);
        strsize = strsize < limit ? strsize : limit;
        arr->array[i] = (char*)kmalloc(strsize + 1);
        if(!arr->array[i]){
            ret = ENOMEM;
            goto err_free_all;
        }
        
        strlcpy(arr->array[i], physical, limit);
    }
    arr->array[i] = NULL;
    ret = OK;
    
    goto final;

    err_free_all:
        for( j = 0; j < i; j++){
            kfree(arr->array[j]);
        }
    err_free_array:
        kfree(arr->array);
    final:
    return ret;
}

void kfree_string_array(struct string_array* arr){
    int j;
    char *p;
    for( j = 0; j < arr->size; j++){
        p = arr->array[j];
        if(p){
            kfree(p);
        }
    }
    kfree(arr->array);
}

void copy_sarray_to_heap(struct proc* who, struct string_array* arr, ptr_t* p){
    int i, len;
    for(i = 0; i < arr->size - 1; i++){
        len = strlen(arr->array[i]) + 1;
        *p++ = (unsigned int)copyto_user_heap(who, arr->array[i], len);
    }
    *p = 0;
}

int build_user_stack(struct proc* who, struct string_array* argv, struct string_array* env){
    struct initial_frame init_stack;
    ptr_t* sp_btm;
    ptr_t *env_ptr = NULL, *p, *argv_ptr = NULL;

    sp_btm = get_physical_addr(align_page((int)who->ctx.m.sp) - 1,who);

    memset(&init_stack, 0, sizeof(init_stack));

    // malloc the pointer for each environment and argv variable
    if(argv->size > 0){
        argv_ptr = (ptr_t*)kmalloc(argv->size);
        if(argv_ptr){
            copy_sarray_to_heap(who, argv, argv_ptr);
            init_stack.argc = argv->size - 1;
            init_stack.argv = (char**)copyto_user_heap(who, argv_ptr, argv->size);
            // KDEBUG(("%d argc %d argv %p\n", who->pid, init_stack.argc, init_stack.argv));
            kfree(argv_ptr);
        }
    }
    if(env->size > 0){
        env_ptr = (ptr_t*)kmalloc(env->size);
        if(env_ptr){
            copy_sarray_to_heap(who, env, env_ptr);
            *sp_btm = (vptr_t)copyto_user_heap(who, env_ptr, env->size);
            // KDEBUG(("build stack env %p physical %p\n", *sp_btm , get_physical_addr(*sp_btm, who)));
            kfree(env_ptr);
        }
    }

    // setup exit if main is returned
    who->ctx.m.ra = who->ctx.m.sp - sizeof(struct exit_code);

    init_stack.i_base.operation = WINIX_SENDREC;
    init_stack.i_base.dest = SYSTEM;
    init_stack.i_base.pm = (struct message*)(who->ctx.m.sp - sizeof(struct exit_code) - sizeof(struct message));
    init_stack.i_base.m.type = EXIT;
    init_stack.i_base.m.m1_i1 = EXIT_MAGIC;
    init_stack.i_base.m.m1_i2 = 0;
    init_stack.code.i_code1 = ASM_ADDUI_SP_SP_2;
    init_stack.code.i_code2 = ASM_SYSCALL;
    
    copyto_user_stack(who, &init_stack, sizeof(struct initial_frame));

    return OK;
}

int exec_welf(struct proc* who, char* path, char *argv[], char *envp[], bool is_new){
    int ret, fd, i, remaining, curr, bss_start;
    struct winix_elf elf;
    struct inode* ino;
    block_t bnr;
    struct block_buffer* buffer;
    struct string_array argv_copy, envp_copy;
    struct proc* parent = get_proc(who->parent);
    struct message m;

    memset(&m, 0, sizeof(m));
    copy_stirng_array(&argv_copy, argv, who, is_new);
    // KDEBUG(("copy argv string %d\n", argv_copy.size));
    copy_stirng_array(&envp_copy, envp, who, is_new);
    // KDEBUG(("copy env string %d\n", envp_copy.size));

    if(!is_new){
        who->sig_pending = 0;
        // if(parent->state & STATE_VFORKING){
        //     bitmap_clear(who->ctx.ptable, PTABLE_LEN);
        // }else{
        //     release_proc_mem(who);
        // }
        if(who->parent > 0 && !(parent->state & STATE_VFORKING)){
            release_proc_mem(who);
        }
        bitmap_clear(who->ctx.ptable, PTABLE_LEN);
    }

    who->thread_parent = 0;

    fd = sys_open(who, path, O_RDONLY | O_DIRECT, 0);
    if(fd < 0)
        return fd;
    ret = sys_read(who, fd, &elf, sizeof(elf));

    ret = alloc_mem_welf(who, &elf, USER_STACK_SIZE, USER_HEAP_SIZE);

    if(ret)
        goto final;

    ino = who->fp_filp[fd]->filp_ino;
    // KDEBUG(("elf %s %x %x size: %d %d %d %d\n", path, elf.binary_offset, elf.binary_pc,
        // elf.binary_size, elf.text_size, elf.data_size, elf.bss_size));
    // KDEBUG(("inode size %d %d \n", ino->i_size, ino->i_total_size ));

    ret = sys_read(who, fd, who->ctx.rbase + elf.binary_offset, elf.binary_size);
    if(ret != elf.binary_size){
        ret = EIO;
        release_proc_mem(who);
        goto final;
    }
    build_user_stack(who, &argv_copy, &envp_copy);
    proc_memctl(who, (void *)0, PROC_NO_ACCESS);

    if(trace_syscall){
        klog("%s[%d] calls execve() to excute %s\n", who->name, who->pid, path);
    }
    set_proc(who, (void (*)())elf.binary_pc, path);
    ret = OK;

    
    final:
    sys_close(who, fd);
    who->state = STATE_RUNNABLE;
    enqueue_schedule(who);
    // KDEBUG(("freeing argv\n"));
    kfree_string_array(&argv_copy);
    // KDEBUG(("freeing envp\n"));
    kfree_string_array(&envp_copy);
    if(!is_new){
        if(parent->state & STATE_VFORKING){
            parent->state &= ~STATE_VFORKING;
            m.type = VFORK;
            syscall_reply2(VFORK, who->pid, parent->proc_nr, &m);
        }
    }
    return DONTREPLY;
}

// /**
// * malloc a new memory and write the values of lines into that address
// * the process is updated
// **/
// int exec_proc(struct proc *who,size_t *lines, size_t length, size_t entry, int offset, const char *name){
//     int err;
//     unsigned int* first_word;
//     set_proc(who, (void (*)())entry, name);
//     if(err = alloc_proc_mem(who, length, USER_STACK_SIZE , USER_HEAP_SIZE)){
//         return err;
//     }

//     // set the first page unaccessible if offset is set
//     // Normally, for each user address space, NULL pointer, which is a macro 
//     // for (void *)0, is set to return invalid value. For this reason, the
//     // first page of the user process is disabled, so that dereferencing
//     // NULL pointer will immediately trigger segfault.
//     if(offset){
//         proc_memctl(who, (void *)0, PROC_NO_ACCESS);
//         who->flags |= DISABLE_FIRST_PAGE;
//     }

//     build_initial_stack(who, NULL, initial_env, SYSTEM_TASK);

//     memcpy(who->ctx.rbase + offset, lines , length);
//     first_word = who->ctx.rbase + offset;

//     enqueue_schedule(who);
//     return OK;
// }


// int build_initial_stack(struct proc* who, char** argv, char** env, struct proc* srcproc){
//     struct initial_frame init_stack;
//     struct initial_frame* pstack = &init_stack;
//     ptr_t* sp_btm = get_physical_addr(who->ctx.m.sp,who);
//     int env_len = 0;
//     char **env_ptr;
//     char *v;
//     unsigned int *env_ptr_list, *p;

//     env = (char **)get_physical_addr(env, srcproc);
//     env_ptr = env;

//     // get the length of environment variables
//     while(*env_ptr++)   env_len++;
//     env_len++; // for the last null terminator

//     // malloc the pointer for each environment variable
//     env_ptr_list = (unsigned int *)kmalloc(env_len);
//     p = env_ptr_list;

//     // copy each of the environment to the user stack
//     env_ptr = env;
//     while((v = *env_ptr++) != NULL){
//         v = (char *)get_physical_addr(v, srcproc);
//         *p++ = (unsigned int)copyto_user_heap(who, v, strlen(v)+1);
//         // save the pointer of the environment as well
//     }
//     *p = 0;

//     // copy the pointers of environment to the user stack
//     copyto_user_stack(who, env_ptr_list, env_len);
//     // sp_btm points to the start of the environment
//     *sp_btm = (unsigned int)who->ctx.m.sp;
//     // setup argc and argv before
//     who->ctx.m.ra = who->ctx.m.sp - sizeof(struct exit_code);

//     pstack->i_base.operation = WINIX_SENDREC;
//     pstack->i_base.dest = SYSTEM;
//     pstack->i_base.pm = (struct message*)(who->ctx.m.sp - sizeof(struct exit_code) - sizeof(struct message));
//     pstack->i_base.m.type = EXIT;
//     pstack->i_base.m.m1_i1 = EXIT_MAGIC;
//     pstack->i_base.m.m1_i2 = 0;
//     pstack->code.i_code1 = ASM_ADDUI_SP_SP_2;
//     pstack->code.i_code2 = ASM_SYSCALL;
    
//     copyto_user_stack(who, pstack, sizeof(struct initial_frame));

//     kfree(env_ptr_list);
//     return OK;
// }

