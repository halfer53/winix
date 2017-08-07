#include "../winix.h"

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
	struct proc *p = NULL;
	void *ptr_base = NULL;
	int len = 0;
	int nstart = 0;
	int pbak,index,j;
	unsigned long *sp;
	uint32_t *src, *dest;
	pattern_t ptn;

	if (p = get_free_proc_slot()) {
		pbak = p->proc_nr;
		*p = *parent;
		p->proc_nr = pbak;
		p->proc_nr = p->proc_nr;
		p->ptable = p->protection_table;
		// kprintf("%d heap 0x%08x | start extract |",p->proc_nr,p->heap_break);
		if(bitmap_extract_pattern(parent->ptable, MEM_MAP_LEN, (int)p->heap_break, &ptn) != 0){
			kprintf("pat search failed");
		}
		// kprintf("start searching |");
		index = bitmap_search_pattern(mem_map, MEM_MAP_LEN, BSS_END / 1024, ptn.pattern, ptn.size);
		bitmap_clear(p->ptable, PTABLE_LEN);
		// kprintf("start set |");
		bitmap_set_pattern(mem_map, 32, index, ptn.pattern, ptn.size);
		bitmap_set_pattern(p->ptable, 32, index, ptn.pattern, ptn.size);

		ptr_base = (void *)(index * 1024);
		p->rbase = ptr_base;

		sp = (size_t *)((size_t)(parent->sp) + (size_t)(parent->rbase));
		p->message = (struct message *)(*(sp+ 2) + (size_t)p->rbase);

		for( src = (uint32_t *)parent->rbase, dest = (uint32_t *)p->rbase, j=0; j < ptn.size ; src+= 1024,dest+=1024, j++){
			if((0x80000000 >> j) & ptn.pattern){
				memcpy(dest,src,1024);
			}
		}
		p->parent = parent->proc_nr;
		p->heap_break = parent->heap_break - (unsigned int)parent->rbase + (unsigned int)p->rbase;
		// process_overview();
		return p->proc_nr;
	}
	return ERR;
}

int do_fork(struct proc *who, struct message *m){
	int child_pr;
	child_pr = _fork(who);
	
	if(child_pr < 0)
		return EINVAL;
	
	m->i1 = 0;
	winix_send(child_pr,m);

	return child_pr;
}


