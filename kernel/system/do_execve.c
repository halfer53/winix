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
#include <limits.h>
#include <fs/super.h>

#define ASM_ADDUI_SP   (0x1ee10000)
#define ASM_ADDUI_SP_SP_2   (0x1ee10002)

struct exit_code{
    unsigned long i_code1;
    unsigned long i_code2;
};

struct initial_frame{
    int argc;
    char **argv;
    int syscall_num;
    int exit_code;
    int signum;
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
    char buffer[PATH_MAX];
    int ret;

    if(is_vaddr_accessible(path, who) && is_vaddr_accessible(argv, who) && is_vaddr_accessible(envp, who)){
        argv = (char**)get_physical_addr(argv, who);
        envp = (char**)get_physical_addr(envp, who);
        if((ret = copy_from_user(who, (ptr_t *)buffer, (vptr_t *)path, PATH_MAX)) < 0)
            return ret;
        return exec_welf(who, buffer, argv, envp, false);
    }
    return -EFAULT;
}

int copy_stirng_array(struct string_array *arr, char* from[], struct proc* who, bool is_physical_addr){
    int limit_per_arg = PAGE_LEN * sizeof(char);
    int limit_arg_num = ARG_MAX / limit_per_arg;
    int count = limit_arg_num;
    int size = 0, i, strsize, j, ret;
    char* physical;
    char **pptr = from;

    arr->size = 0;
    if(!from)
        return 0;
    while(*pptr++ && count-- > 0){
        size++;
    }
    size++;
    arr->array = (char**)kmalloc(size, sizeof(char *));
    if(!arr->array){
        return -ENOMEM;
    }
    arr->size = size;
    pptr = from;
    count = limit_per_arg;
    for(i = 0; i < size - 1; i++){
        physical = from[i];
        if(!is_physical_addr){
            physical = (char*)get_physical_addr(physical, who);
        }
        strsize = strlen(physical);
        strsize = strsize < limit_per_arg ? strsize : limit_per_arg;
        arr->array[i] = (char*)kmalloc(strsize + 1, sizeof(char));
        if(!arr->array[i]){
            ret = -ENOMEM;
            goto err_free_all;
        }
        
        strlcpy(arr->array[i], physical, limit_per_arg);
    }
    arr->array[i] = NULL;
    ret = 0;
    
    goto final;

    err_free_all:
        for( j = 0; j < i; j++){
            kfree(arr->array[j]);
        }
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
        *p++ = (unsigned int)(unsigned long)copyto_user_heap(who, arr->array[i], len);
    }
    *p = 0;
}

int build_user_stack(struct proc* who, struct string_array* argv, struct string_array* env){
    struct initial_frame init_stack;
    unsigned long* sp_btm;
    ptr_t *env_ptr = NULL, *argv_ptr = NULL;

    sp_btm = (unsigned long*)get_physical_addr((unsigned long)align_page((int)(unsigned long)who->ctx.m.sp) - 1,who);

    memset(&init_stack, 0, sizeof(init_stack));

    // malloc the pointer for each environment and argv variable
    if(argv->size > 0){
        argv_ptr = (ptr_t*)kmalloc(argv->size, sizeof(ptr_t));
        if(argv_ptr){
            copy_sarray_to_heap(who, argv, argv_ptr);
            init_stack.argc = argv->size - 1;
            init_stack.argv = (char**)copyto_user_heap(who, argv_ptr, argv->size);
            // kdebug("%d argc %d argv %p\n", who->pid, init_stack.argc, init_stack.argv);
            kfree(argv_ptr);
        }
    }
    if(env->size > 0){
        env_ptr = (ptr_t*)kmalloc(env->size, sizeof(ptr_t));
        if(env_ptr){
            copy_sarray_to_heap(who, env, env_ptr);
            // store the pointer to environment variable at the bottom of stack
            // this can be retrieved by get_environ() in lib/ansi/env.c
            *sp_btm = (unsigned long)copyto_user_heap(who, env_ptr, env->size);
            // kdebug("build stack env %p physical %p\n", *sp_btm , get_physical_addr(*sp_btm, who));
            kfree(env_ptr);
        }
    }

    // setup exit if main is returned
    who->ctx.m.ra = who->ctx.m.sp - sizeof(struct exit_code);
    

    init_stack.syscall_num = EXIT;
    init_stack.exit_code = EXIT_MAGIC;
    init_stack.signum = 0;
    init_stack.code.i_code1 = ASM_ADDUI_SP_SP_2;
    init_stack.code.i_code2 = ASM_SYSCALL;

    copyto_user_stack(who, &init_stack, sizeof(struct initial_frame));

    return 0;
}

void arch_elf(struct winix_elf* elf, struct superblock* sb){
    ARCH_CHAR_SIZE(elf->binary_size, sb);
    ARCH_CHAR_SIZE(elf->bss_size, sb);
    ARCH_CHAR_SIZE(elf->data_size, sb);
    ARCH_CHAR_SIZE(elf->text_size, sb);
    ARCH_CHAR_SIZE(elf->binary_offset, sb);
}

int exec_welf(struct proc* who, const char* path, char *argv[], char *envp[], bool is_new){
    int ret;
    struct filp* filp;
    bool has_enough_ram;
    struct winix_elf elf;
    struct string_array argv_copy, envp_copy;
    struct proc* parent = get_proc(who->parent);
    struct message m;

    memset(&m, 0, sizeof(m));
    memset(&argv_copy, 0, sizeof(struct string_array));
    memset(&envp_copy, 0, sizeof(struct string_array));
    if (argv && (ret = copy_stirng_array(&argv_copy, argv, who, is_new)))
        return ret;
    // kdebug("copy argv string %d\n", argv_copy.size);
    if (envp && (ret = copy_stirng_array(&envp_copy, envp, who, is_new)))
        goto err_env;

    ret = filp_open(who, &filp, path, O_RDONLY | O_DIRECT, 0);
    if(ret)
        goto err_open;
    if (filp->filp_ino->i_mode & S_IFDIR){
        ret = -EISDIR;
        goto err_open;
    }
        
    ret = filp_read(who, filp, &elf, sizeof(elf));
    if (ret != sizeof(elf)){
        kwarn("welf %s read fail %d\n", path, ret);
        ret = -EIO;
        goto final;
    }
    arch_elf(&elf, filp->filp_ino->i_sb);

    has_enough_ram = peek_mem_welf(&elf, USER_STACK_SIZE, USER_HEAP_SIZE);
    if (!has_enough_ram){
        ret = -ENOMEM;
        goto final;
    }

    if (!validate_welf(&elf)){
        ret = -ENOEXEC;
        goto final;
    }

    if ((ret = release_proc_mem(who)))
        goto final;
    bitmap_clear((unsigned int *)who->ctx.ptable, PTABLE_LEN);
    
    ret = alloc_mem_welf(who, &elf, USER_STACK_SIZE, USER_HEAP_SIZE);
    if(ret)
        goto final;

    // kdebug("elf %s %x %x size: %d %d %d %d\n", path, elf.binary_offset, elf.binary_pc,
        // elf.binary_size, elf.text_size, elf.data_size, elf.bss_size);

    ret = filp_read(who, filp, who->ctx.rbase + elf.binary_offset, elf.binary_size);
    if(ret != elf.binary_size){
        if (ret >= 0){
            kwarn("exp %d read %d\n", elf.binary_size, ret);
            ret = -EAGAIN;
        }
            
        goto final;
    }

    who->thread_parent = 0;
    build_user_stack(who, &argv_copy, &envp_copy);
    proc_memctl(who, (void *)0, false);
    ret = 0;
    goto final;
    
final:
    filp_close(filp);
err_open:
    // kdebug("freeing envp\n");
    if (envp)
        kfree_string_array(&envp_copy);
err_env:
    // kdebug("freeing argv\n");
    if (argv)
        kfree_string_array(&argv_copy);

    if (trace_syscall || ret != 0){
        klog("%s[%d] execve() %s, return %s\n", who->name, who->pid, path, kstr_error(ret));
    }
    if (ret == 0){
        who->state = STATE_RUNNABLE;
        enqueue_schedule(who);
        set_proc(who, (void (*)())(unsigned long)elf.binary_pc, path);
    }
    
    if(!is_new){
        if(parent->state & STATE_VFORKING){
            parent->state &= ~STATE_VFORKING;
            m.type = VFORK;
            syscall_reply2(VFORK, who->pid, parent->proc_nr, &m);
        }
    }

    if (ret == 0)
        return DONTREPLY;
    return ret;
}

