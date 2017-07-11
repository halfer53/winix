#include <kernel/kernel.h>

unsigned int mem_map[MEM_MAP_LEN];

void init_mem_table(int free_mem_begin) {
	int len = free_mem_begin / 1024;
	int i;
	for (i = 0; i < MEM_MAP_LEN; i++) {
		mem_map[i] = 0;
	}
	bitmap_set_nbits(mem_map, MEM_MAP_LEN, 0, len);
}

int get_free_pages(int num, int flags) {
	int nstart = bitmap_search(mem_map, MEM_MAP_LEN, num);
	if (nstart != 0)
	{
		bitmap_set_nbits(mem_map, MEM_MAP_LEN, nstart, num);
		return nstart;
	}
	return ERR;
}

int get_free_page(int flags) {
	int nstart = bitmap_search(mem_map, MEM_MAP_LEN, 1);
	if (nstart != 0)
	{
		bitmap_set_bit(mem_map, MEM_MAP_LEN, nstart);
		return nstart;
	}
	return ERR;
}

void free_page(void* ptr) {
	int nstart = (int)((long)ptr /1024);
	bitmap_reset_bit(mem_map, MEM_MAP_LEN, nstart);
}


