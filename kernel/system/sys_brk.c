#include "../winix.h"

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
