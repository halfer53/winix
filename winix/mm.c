#include <kernel/kernel.h>

unsigned int mem_map[MEM_MAP_LEN];

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

int get_free_page(int flags) {
	return get_free_pages(1,flags);
}

int get_free_pages(int num, int flags) {
	int nstart;
	if(flags && __GFP_HIGH)
		nstart =  bitmap_search_reverse(mem_map, MEM_MAP_LEN, num);
	else
		nstart =  bitmap_search(mem_map, MEM_MAP_LEN, num);
	if (nstart != 0)
	{
		bitmap_set_nbits(mem_map, MEM_MAP_LEN, nstart, num);
		return nstart;
	}
	return ERR;
}

ptr_t* get_free_page_addr(int flags){
	return get_free_pages_addr(1,flags);
}

ptr_t* get_free_pages_addr(int num, int flags){
	int index;
	if((index = get_free_pages(num,flags)) != ERR){
		return (ptr_t *)(index * PAGE_LEN);
	}
	return NULL;
}

int next_free_page_index(){
	return bitmap_search(mem_map, MEM_MAP_LEN, 1);
}

void free_page(ptr_t* ptr) {
	int nstart = (int)((long)ptr /1024);
	bitmap_reset_bit(mem_map, MEM_MAP_LEN, nstart);
}

void print_ptable(unsigned int *p, int len){
	int i;
	for( i = 0; i < len; i++){
		kprintf("0x%08x ",*p++);
		if((i+1) % 8 == 0)
			kprintf("\n");
	}
	kprintf("\n");
}

void print_sysmap(){
	kprintf("sysmap: ");
	print_ptable(mem_map, MEM_MAP_LEN);
}


