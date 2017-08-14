#include <kernel/kernel.h>

unsigned int mem_map[MEM_MAP_LEN];

ptr_t *get_free_pages(int length, int flags) {
	int nstart;
	int num = PADDR_TO_NUM_PAGES(length);
	if(flags & GFP_HIGH){
		nstart =  bitmap_search_reverse(mem_map, MEM_MAP_LEN, num);
	}else{
		nstart =  bitmap_search(mem_map, MEM_MAP_LEN, num);
	}
	if (nstart != 0)
	{
		bitmap_set_nbits(mem_map, MEM_MAP_LEN, nstart, num);
		return PAGE_TO_PADDR(nstart);
	}
	return NULL;
}

ptr_t* user_get_free_pages(struct proc* who, int length, int flags){
	int index;
	ptr_t* p;
	int page_num;

	p = get_free_pages(length,flags);
	if(p == NULL)
		return NULL;
	index = PADDR_TO_PAGED(p);
	page_num = PADDR_TO_NUM_PAGES(length);
	if(bitmap_set_nbits(who->ptable, PTABLE_LEN, index, page_num) == ERR)
		return NULL;
	return p;
}

int next_free_page_index(){
	return bitmap_search(mem_map, MEM_MAP_LEN, 1);
}

int free_pages(ptr_t* page, int len){
	int page_index;
	if((int)page % PAGE_LEN != 0)
		return ERR;
	page_index = PADDR_TO_PAGED(page);
	return bitmap_clear_nbits(mem_map, MEM_MAP_LEN, page_index, len);
}

int user_free_pages(struct proc* who, ptr_t* page, int len){
	int index;
	if(free_pages(page,len) != OK)
		return ERR;
	index = PADDR_TO_PAGED(page);
	return bitmap_clear_nbits(who->ptable, PTABLE_LEN, index, len);
}

bool is_addr_accessible(struct proc* who, ptr_t* addr){
	int page;

	addr = get_physical_addr(addr, who);
	page = PADDR_TO_PAGED(addr);
	return is_bit_on(who->ptable, MEM_MAP_LEN, page);
}

void release_proc_mem(struct proc *who){
    bitmap_xor(mem_map,who->ptable,MEM_MAP_LEN);
}

void print_ptable(struct proc* who){
	print_bitmap(who->ptable, MEM_MAP_LEN);
}

void print_sysmap(){
	print_bitmap(mem_map, MEM_MAP_LEN);
}


void init_mem_table() {
	int len, i;
	uint32_t free_mem_begin;

	free_mem_begin = (uint32_t)&BSS_END;
	free_mem_begin |= 0x03ff;
	free_mem_begin++;
	len = free_mem_begin / PAGE_LEN;

	bitmap_clear(mem_map, MEM_MAP_LEN);
	bitmap_set_nbits(mem_map, MEM_MAP_LEN, 0, len);
	bitmap_set_bit(mem_map, MEM_MAP_LEN, FREE_MEM_END / PAGE_LEN);
}




