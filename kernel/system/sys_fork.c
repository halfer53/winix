#include "../winix.h"

int copy_pcb(struct proc* parent, struct proc* child){
	int pbak;
	pbak = child->proc_nr;
	*child = *parent;
	child->proc_nr = pbak;
	child->ptable = child->protection_table;
	bitmap_clear(child->ptable, PTABLE_LEN);
	return OK;
}

int copy_mm(struct proc* parent, struct proc* child){
	uint32_t *src, *dest;
	int j, index;
	pattern_t ptn;

	if(bitmap_extract_pattern(parent->ptable, MEM_MAP_LEN, (int)child->heap_break, &ptn) != OK)
		return ERR;
	
	index = bitmap_search_pattern(mem_map, MEM_MAP_LEN, ptn.pattern, ptn.size);
	if(index == ERR)
		return ERR;

	bitmap_set_pattern(mem_map, MEM_MAP_LEN, index, ptn.pattern, ptn.size);
	bitmap_set_pattern(child->ptable, PTABLE_LEN, index, ptn.pattern, ptn.size);

	child->rbase = PAGE_TO_PADDR(index);

	src = (ptr_t *)parent->rbase;
	dest = (ptr_t *)child->rbase;
	for( j=0; j < ptn.size ;j++ ){
		if(mask[j] & ptn.pattern){
			copy_page(dest, src);
		}
		src += PAGE_LEN;
		dest += PAGE_LEN;
	}
	return OK;
}

int copy_pregs(struct proc* parent, struct proc* child){
	ptr_t *sp;
	sp = get_physical_addr(parent->sp,parent);
	child->message = (struct message *)(*(sp+ 2) + (reg_t)child->rbase);
	child->heap_break = parent->heap_break - (unsigned int)parent->rbase + (unsigned int)child->rbase;
	child->stack_top = parent->stack_top - (unsigned int)parent->rbase + (unsigned int) child->rbase;
	return OK;
}




/**
 * fork the calling process
 *
 * Returns:
 *   proc of the child process
 *
 * Side Effects:
 *   a new process forked onto the a new memory space, but not yet added to the scheduling queue
 **/
int _fork(struct proc *parent) {
	struct proc *child;
	// int tdb_page_len, sp_heap_page_len, page;

	if (child = get_free_proc_slot()) {
		copy_pcb(parent,child);
		if(copy_mm(parent,child) == ERR)
			return ERR;

		copy_pregs(parent,child);
		
		//Divide the quantum size between the parent and child
		//if quantum size is 1, quantum size is not changed
		if(parent->quantum != 1){
			child->quantum = (child->quantum + 1) / 2;
			parent->quantum /= 2;
		}

		child->parent = parent->proc_nr;
		return child->proc_nr;
	}
	return ERR;
}

int do_fork(struct proc *who, struct message *m){
	int child_pr;
	child_pr = _fork(who);
	
	if(child_pr == ERR)
		return EINVAL;
	
	m->i1 = 0;
	winix_send(child_pr,m);

	return child_pr;
}


