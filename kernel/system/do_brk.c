/**
 * Syscall in this file: brk
 * Input:    m1_p1: new user heap break
 *
 * Return:     m1_p1: current user heap break
 *             on success, it returns the new break set by user
 *             on failure, it returns the previous heap break
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

/**
 * For each process, the heap would look something like this
 * 
 * stack_top
 * stack bottom
 * text
 * bss
 * data
 * heap_top
 * heap_break
 * heap_bottom 
 * 
 * where heap_bottom points to the end of the user's heap region
 * between stack bottom and heap bottom are the preallocated heap 
 * region for each process heap_break can be increased or 
 * decreased freely between heap_bottom and stack bottom
 * but if heap_break were to go beyond heap_bottom, heap_bottom 
 * is first extended by syscall brk() 
 * NB that heap_bottom always point at the end of the page
 */
    

// sbrk function
// NB sbrk() is implemented as a user wrapper function, that internally uses brk() syscall
// This function is just an internal kernel function for extending heaps
ptr_t* sys_sbrk(struct proc *who, int size){
    ptr_t* next_page;
    int err;
    void* oheap;
    int residual, request_size;

    if(size == 0)
        goto ret_result;

    if(size < 0){
        if(who->heap_break + size >= who->heap_top){
            who->heap_break += size;
            goto ret_result;
        }
        return NULL;
    }
    
    // residual is the remaining unused heap by the user
    residual = who->heap_bottom - who->heap_break;
    if(residual >= size){
        who->heap_break += size;
        goto ret_result;
    }

    // extend the heap bottom if needed
    next_page = who->heap_bottom + 1;
    request_size = size - residual; 
    err = user_get_free_pages_from(who,next_page, request_size );
    if(err)
        return NULL;        
    
    // klog("extending heap size %d oheap %x newheap %x btm %x\n", size, who->heap_break, 
    //                                                         (who->heap_break + size), who->heap_bottom);                                  
    who->heap_break += size;
    who->heap_bottom += align_page(request_size);

ret_result:
    return who->heap_break;
}

int do_sbrk(struct proc* who, struct message *m){
    ptr_t* ret = sys_sbrk(who, m->m1_i1);
    return (int)get_virtual_addr(ret, who);
}

// syscall for brk()
// in contrast to the user space sbrk(), system call brk() returns the new heap break 
// to the user space, and then user space sbrk() will return the saved previous break. 
// same applies to brk(), which checks the syscall return is valid, and return 0 or 1
int do_brk(struct proc *who, struct message *m){
    int size;
    ptr_t* new_brk;
    vptr_t* vaddr = m->m1_p1, *vheapbrk;
    ptr_t* addr = get_physical_addr(m->m1_p1, who);
    ptr_t* heap_top;

    
    m->m1_p1 = get_virtual_addr(who->heap_break, who);
    // KDEBUG(("proc %d req brk %x curr brk %x \n", who->proc_nr, vaddr, m->m1_p1  ));
    if(addr < who->heap_break){
        if(addr < who->heap_top){
            return EINVAL;
        }
        
        who->heap_break = addr;
        m->m1_p1 = vaddr;
        return OK;
    }

    size = (int)addr - (int)who->heap_break;
    new_brk = sys_sbrk(who, size);
    if(new_brk == NULL)
        return ENOMEM;
    new_brk = get_virtual_addr(new_brk, who);
    m->m1_p1 = new_brk;
    return OK;
}
