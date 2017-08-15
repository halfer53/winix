#include "../winix.h"

void* sys_sbrk(struct proc *who, int size){
	ptr_t* next_page;
	void* oheap;
	int residual;

	if(size == 0)
		return get_virtual_addr(who->heap_break, who);

	residual = who->heap_bottom - who->heap_break;
	if(residual >= size){
		who->heap_break += size;
		return get_virtual_addr(who->heap_break, who);
	}

	next_page = who->heap_bottom + 1;
	size -= residual;
	if(user_get_free_pages_from(who,next_page, size ) == ERR)
		return (void *)-1;

	// kmesg("extending heap size %d\n", size);
	
	who->heap_break += size;
	who->heap_bottom += align_page(size);
	return get_virtual_addr(who->heap_break,who);
}


int do_brk(struct proc *who, struct message *m){
	int size;
	ptr_t* new_brk;
	ptr_t* addr = get_physical_addr(m->p1, who);
	ptr_t* heap_prev_limit;

	m->p1 = get_virtual_addr(who->heap_break, who);
	if(addr < who->heap_break){
		heap_prev_limit = who->stack_top + GET_DEF_STACK_SIZE(who);
		if(addr < heap_prev_limit){
			return ERR;
		}
		who->heap_break = addr;
		return OK;
	}

	size = (int)addr - (int)who->heap_break;
	new_brk = sys_sbrk(who, size);
	if(new_brk == (void *)-1){
		return ENOMEM;
	}
	m->p1 = new_brk;
	return OK;
}
