#include "../winix.h"

/**
 * Syscall in this file: brk
 * Input:	p1: new user heap break
 *
 * Return: 	p1: current user heap break
 * 			on success, it returns the new break set by user
 * 			on failure, it returns the previous heap break
 * 
 */


/**
 * For each process, the heap would look something like this
 * 
 * stack_top
 * stack bottom
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
	

//sbrk function
//NB sbrk() is implemented as a user wrapper function, that internally uses brk() syscall
//This function is just an internal kernel function for extending heaps
void* sys_sbrk(struct proc *who, int size){
	ptr_t* next_page;
	void* oheap;
	int residual, request_size;

	if(size == 0)
		return get_virtual_addr(who->heap_break, who);

	
	//residual is the remaining unused heap by the user
	residual = who->heap_bottom - who->heap_break;
	if(residual >= size){
		who->heap_break += size;
		return get_virtual_addr(who->heap_break, who);
	}

	//extend the heap bottom if needed
	next_page = who->heap_bottom + 1;
	request_size = size - residual; 
	if(user_get_free_pages_from(who,next_page, request_size ) == ERR)
		return (void *)-1;
	// kinfo("extending heap size %d oheap %x newheap %x\n", size, who->heap_break, 
	// 														(who->heap_break + size));
	
	who->heap_break += size;
	who->heap_bottom += align_page(request_size);
	return get_virtual_addr(who->heap_break,who);
}

//syscall for brk()
//in contrast to the user space sbrk(), system call brk() returns the new heap break 
//to the user space, and then user space sbrk() will return the saved previous break. 
//same applies to brk(), which checks the syscall return is valid, and return 0 or 1
int do_brk(struct proc *who, struct message *m){
	int size;
	ptr_t* new_brk;
	ptr_t* addr = get_physical_addr(m->p1, who);
	ptr_t* heap_top;

	m->p1 = get_virtual_addr(who->heap_break, who);

	if(addr < who->heap_break){
		heap_top = GET_HEAP_TOP(who);
		if(addr < heap_top)
			return EINVAL;
		
		who->heap_break = addr;
		return OK;
	}

	size = (int)addr - (int)who->heap_break;
	new_brk = sys_sbrk(who, size);
	if(new_brk == (void *)-1)
		return ENOMEM;
	
	m->p1 = new_brk;
	return OK;
}
