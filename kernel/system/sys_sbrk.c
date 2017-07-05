#include "../winix.h"


void *do_sbrk( proc_t *who, size_t size) {

	unsigned long *ptable;
	int nstart,len;
	int *heap_break_bak;
	void *ptr_addr;
	if (size == 0)
		return get_virtual_addr(who->heap_break,who);
	
	heap_break_bak = who->heap_break;

	if (is_addr_in_same_page( heap_break_bak, ( heap_break_bak + size)) ) {
		who->heap_break = heap_break_bak + size;
		// kprintf("ptr %x ptr+size %x heap_break %x %x\n", heap_break_bak,heap_break_bak + size, who->heap_break,mem_map[0]);
		return get_virtual_addr(heap_break_bak,who);
	}
	

	ptable = who->protection_table;
	len = physical_len_to_page_len(size);
	nstart = bitmap_search(mem_map, MEM_MAP_LEN, len);
	if (nstart != -1) {
		//set mem_map and who's ptable's corresponding bits to 1
		bitmap_set_nbits(mem_map, MEM_MAP_LEN, nstart, len);
		bitmap_set_nbits(ptable, PROTECTION_TABLE_LEN, nstart, len);
		ptr_addr = (void *)(nstart * 1024);

		if (is_addr_in_consecutive_page( heap_break_bak, ptr_addr)){
			
			who->heap_break = heap_break_bak + size ;
			// kprintf("ptr %x pt %x obrk %x brk %x\n", heap_break_bak, mem_map[0], heap_break_bak,who->heap_break);
			return get_virtual_addr(heap_break_bak,who);
		}
		if((int *)ptr_addr > heap_break_bak)
			who->heap_break = (int *)ptr_addr + size;

		// kprintf("sbrk: optr %x ", ptr_addr);
		ptr_addr = get_virtual_addr(ptr_addr,who);
		// kprintf("ptr %x pt %x obrk %x brk %x\n", ptr_addr, mem_map[0],  heap_break_bak, who->heap_break);
		return ptr_addr;
	}else{
		kprintf("System out of memory\n");
		for (nstart = 0; nstart < 32; ++nstart)
		{
			kprintf(" %x |",mem_map[nstart]);
		}
		kprintf("\n");
	}
	return NULL;

}

void syscall_sbrk(proc_t *who, message_t *m){
    m->p1 = do_sbrk(who,m->i1);
	m->i1 = m->p1 == NULL ? -1 : 0;
	winix_send(who->pid, m);
}
