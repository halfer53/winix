#include "../winix.h"

/**
 * implementation of brk
 * @param  who who of this sys call
 * @param  addr   set the end of data segment specified by the addr, note that this is virtual memory
 * @return        status of result, 0 if success
 */
int do_brk(struct proc *who, struct message *m){
	int i,paddr;
	ptr_t* addr;
	addr = get_physical_addr(m->p1,who);
	if(is_addr_in_same_page(who->heap_break, addr)){
		who->heap_break = addr;
		return OK;
	}
	if(addr < who->heap_break){
		i = get_page_index(who->heap_break);
		paddr = get_page_index(addr);
		for(; i> paddr; i--){
			bitmap_clear_bit(mem_map,MEM_MAP_LEN,i);
			bitmap_clear_bit(who->ptable,MEM_MAP_LEN,i);
		}
		who->heap_break = addr;
		return OK;
	}
	return EINVAL;
}
