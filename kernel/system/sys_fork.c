#include "../winix.h"

/**
 * fork the calling process
 *
 * Returns:
 *   proc_index of the newly forked process
 *
 * Side Effects:
 *   a new process forked onto the a new memory space, but not yet added to the scheduling queue
 **/
proc_t* do_fork(proc_t *parent) {
	proc_t *p = NULL;
	void *ptr_base = NULL;
	int len = 0;
	int nstart = 0;
	int pbak,index,j;
	unsigned long *sp;
	uint32_t *src, *dest;
	pattern_t ptn;

	if (parent->length == 0 || (size_t)(parent->rbase) == 0) {
		//we can't fork p1 if it's a system task
		kprintf("%s can't be forked\n", parent->name );
		return NULL;
	}

	if (p = get_free_proc()) {
		pbak = p->proc_index;
		*p = *parent;
		p->proc_index = pbak;
		p->ptable = p->protection_table;

		if(extract_pattern(p->ptable, MEM_MAP_LEN, (int)p->heap_break, &ptn) != 0){
			kprintf("pat search failed");
		}

		index = bitmap_search_pattern(mem_map, MEM_MAP_LEN, BSS_END / 1024, ptn.pattern, ptn.size);
		bitmap_clear(p->ptable, PROTECTION_TABLE_LEN);

		bitmap_set_pattern(mem_map, 32, index, ptn.pattern, ptn.size);
		bitmap_set_pattern(p->ptable, 32, index, ptn.pattern, ptn.size);

		ptr_base = (void *)(index * 1024);
		p->rbase = ptr_base;

		sp = (size_t *)((size_t)(parent->sp) + (size_t)(parent->rbase));
		p->message = (message_t *)(*(sp+ 2) + (size_t)p->rbase);

		for( src = (uint32_t *)parent->rbase, dest = (uint32_t *)p->rbase, j=0; j < ptn.size ; src+=1024,dest+=1024, j++){
			if((0x80000000 >> j) & ptn.pattern){
				memcpy(dest,src,1024);
			}
		}
		p->parent_proc_index = parent->proc_index;
	}
	// process_overview();
	// printProceInfo(parent);
	// printProceInfo(p);
	assert(p != NULL, "Fork");
	return p;
}
