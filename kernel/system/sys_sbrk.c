#include "../winix.h"


void* _sbrk(struct proc *who, int size){
	ptr_t* next_page;
	void* oheap;

	KDEBUG(("size %d \n",size));
	if(size == 0)
		return get_virtual_addr(who->heap_break, who);

	if(who->heap_break + size < who->heap_bottom){
		oheap = who->heap_break;
		who->heap_break += size;
		return get_virtual_addr(who->heap_break, who);
	}

	next_page = who->heap_bottom + 1;
	if(user_get_free_pages_from(who,next_page, size) == ERR){
		kprintf("page not free \n");
		return NULL;
	}
	
	oheap = who->heap_break;
	who->heap_break += size;
	who->heap_bottom += align_page(size) - 1;
	return oheap;
}

// void *_sbrk( struct proc *who, size_t size) {

// 	unsigned int *ptable;
// 	int nstart,len;
// 	ptr_t* heap_break_bak;
// 	ptr_t* ptr_addr;
// 	if (size == 0)
// 		return get_virtual_addr(who->heap_break,who);
	
// 	heap_break_bak = who->heap_break;

// 	if (is_addr_in_same_page( heap_break_bak, ( heap_break_bak + size)) ) {
// 		who->heap_break = heap_break_bak + size;
// 		// kprintf("ptr 0x%08x ptr+size 0x%08x heap_break 0x%08x 0x%08x\n", heap_break_bak,heap_break_bak + size, who->heap_break,mem_map[0]);
// 		return get_virtual_addr(heap_break_bak,who);
// 	}
	

// 	ptable = who->protection_table;
// 	len = PADDR_TO_NUM_PAGES(size);
// 	nstart = bitmap_search(mem_map, MEM_MAP_LEN, len);
// 	if (nstart != -1) {
// 		//set mem_map and who's ptable's corresponding bits to 1
// 		bitmap_set_nbits(mem_map, MEM_MAP_LEN, nstart, len);
// 		bitmap_set_nbits(ptable, PTABLE_LEN, nstart, len);
// 		ptr_addr = (void *)(nstart * 1024);

// 		if (is_addr_in_consecutive_page( heap_break_bak, ptr_addr)){
			
// 			who->heap_break = heap_break_bak + size ;
// 			// kprintf("ptr 0x%08x pt 0x%08x obrk 0x%08x brk 0x%08x\n", heap_break_bak, mem_map[0], heap_break_bak,who->heap_break);
// 			return get_virtual_addr(heap_break_bak,who);
// 		}
// 		if(ptr_addr > heap_break_bak)
// 			who->heap_break = ptr_addr + size;

// 		// kprintf("sbrk: optr 0x%08x ", ptr_addr);
// 		ptr_addr = get_virtual_addr(ptr_addr,who);
// 		// kprintf("ptr 0x%08x pt 0x%08x obrk 0x%08x brk 0x%08x\n", ptr_addr, mem_map[0],  heap_break_bak, who->heap_break);
// 		return ptr_addr;
// 	}else{
// 		kprintf("System out of memory\n");
// 		for (nstart = 0; nstart < 32; ++nstart)
// 		{
// 			kprintf(" 0x%08x |",mem_map[nstart]);
// 		}
// 		kprintf("\n");
// 	}
// 	return NULL;
// }

int do_sbrk(struct proc *who, struct message *m){
    void* p = _sbrk(who,m->i1);
	return p == NULL ? EINVAL : OK;
}
