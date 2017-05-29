#include "winix.h"


//scan the FREE_MEM_BEGIN
void Scan_FREE_MEM_BEGIN() {
	//TODO start kmalloc here
	FREE_MEM_BEGIN = (size_t)&BSS_END;

	//Round up to the next 1k boundary
	FREE_MEM_BEGIN |= 0x03ff;
	FREE_MEM_BEGIN++;

	// kprintf("\r\nfree memory begin %x\r\n", FREE_MEM_BEGIN );
}


void *expand_mem(size_t size) {
	size_t temp = FREE_MEM_BEGIN;
	if (FREE_MEM_END != 0) {
		//if FREE_MEM_END is not null, then that means the OS is running
		//otherwise it's initialising, thus FREE_MEM_END is not set yet
		//we just assume there is enough memory during the start up
		//since calculating FREE_MEM_END during the start up is gonna crash the system for some unknown reason
		if (size + FREE_MEM_BEGIN > FREE_MEM_END) {
			return NULL;
		}
	}

	FREE_MEM_BEGIN += size;
	//kprintf("free mem %x\n",FREE_MEM_BEGIN );
	return (void *)temp;
}

/**
 * implementation of brk
 * @param  caller caller of this sys call
 * @param  addr   set the end of data segment specified by the addr, note that this is virtual memory
 * @return        status of result, 0 if success
 */
int do_brk(proc_t *caller, void *addr){
	int i,paddr;
	addr = get_physical_addr(addr,caller);
	if(is_addr_in_same_page(caller->heap_break, addr)){
		caller->heap_break = addr;
		return 0;
	}
	if(addr < caller->heap_break){
		i = get_page_index(caller->heap_break);
		paddr = get_page_index(addr);
		for(; i> paddr; i--){
			bitmap_reset_bit(mem_map,MEM_MAP_LEN,i);
			bitmap_reset_bit(caller->ptable,MEM_MAP_LEN,i);
		}
		caller->heap_break = addr;
		return 0;
	}
	return 1;
}

void *do_sbrk( proc_t *caller, size_t size) {

	unsigned long *ptable;
	int nstart,len;
	int *heap_break_bak;
	void *ptr_addr;
	if (size == 0)
		return get_virtual_addr(caller->heap_break,caller);
	
	heap_break_bak = caller->heap_break;

	if (is_addr_in_same_page( heap_break_bak, ( heap_break_bak + size)) ) {
		caller->heap_break = heap_break_bak + size;
		// kprintf("ptr %x ptr+size %x heap_break %x %x\n", heap_break_bak,heap_break_bak + size, caller->heap_break,mem_map[0]);
		return get_virtual_addr(heap_break_bak,caller);
	}
	

	ptable = caller->protection_table;
	len = physical_len_to_page_len(size);
	nstart = bitmap_search(mem_map, MEM_MAP_LEN, len);
	if (nstart != -1) {
		//set mem_map and caller's ptable's corresponding bits to 1
		bitmap_set_nbits(mem_map, MEM_MAP_LEN, nstart, len);
		bitmap_set_nbits(ptable, PROTECTION_TABLE_LEN, nstart, len);
		ptr_addr = (void *)(nstart * 1024);

		if (is_addr_in_consecutive_page( heap_break_bak, ptr_addr)){
			
			caller->heap_break = heap_break_bak + size ;
			// kprintf("ptr %x pt %x obrk %x brk %x\n", heap_break_bak, mem_map[0], heap_break_bak,caller->heap_break);
			return get_virtual_addr(heap_break_bak,caller);
		}
		if((int *)ptr_addr > heap_break_bak)
			caller->heap_break = (int *)ptr_addr + size;

		// kprintf("sbrk: optr %x ", ptr_addr);
		ptr_addr = get_virtual_addr(ptr_addr,caller);
		// kprintf("ptr %x pt %x obrk %x brk %x\n", ptr_addr, mem_map[0],  heap_break_bak, caller->heap_break);
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






