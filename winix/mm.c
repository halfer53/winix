#include <kernel/kernel.h>

PRIVATE unsigned int mem_map[MEM_MAP_LEN];

bool is_addr_accessible(struct proc* who, ptr_t* addr){
	int page;

	addr = get_physical_addr(addr, who);
	page = PADDR_TO_PAGED(addr);
	return is_bit_on(who->ptable, PTABLE_LEN, page);
}

bool is_page_free(ptr_t* addr){
	int paged = PADDR_TO_PAGED(addr);

	return is_bit_on(mem_map, MEM_MAP_LEN, paged);
}

bool is_pages_free_from(ptr_t* addr, int size){
	int i;
	int paged = PADDR_TO_PAGED(addr);
	int page_num = PADDR_TO_NUM_PAGES(size);

	for(i = 0; i < page_num; i++){
		if(!is_bit_on(mem_map, MEM_MAP_LEN, paged++))
			return false;
	}
	return true;
}

ptr_t *get_free_pages(int length, int flags) {
	int nstart;
	int num = PADDR_TO_NUM_PAGES(length);
	if(flags & GFP_HIGH){
		nstart =  bitmap_search_reverse(mem_map, MEM_MAP_LEN, num);
	}else{
		nstart =  bitmap_search(mem_map, MEM_MAP_LEN, num);
	}
	if (nstart != 0){
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

int get_free_pages_from(ptr_t* addr, int size){
	int paged, page_num;

	if(!is_pages_free_from(addr, size))
		return ERR;
	
	paged = PADDR_TO_PAGED(addr);
	page_num = PADDR_TO_NUM_PAGES(size);
	if(bitmap_set_nbits(mem_map, MEM_MAP_LEN, paged, page_num) == ERR)
		return ERR;

	return OK;
}

int user_get_free_pages_from(struct proc* who, ptr_t* addr, int size){
	int index;
	int ret;
	int page_num;

	ret = get_free_pages_from(addr,size);
	if(ret == ERR)
		return ERR;
	index = PADDR_TO_PAGED(addr);
	page_num = PADDR_TO_NUM_PAGES(size);
	if(bitmap_set_nbits(who->ptable, PTABLE_LEN, index, page_num) == ERR)
		return ERR;
	return OK;
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


void* dup_vm(struct proc* parent, struct proc* child, struct bit_pattern* ptn){
	int index;
	if(bitmap_extract_pattern(parent->ptable, MEM_MAP_LEN, (int)child->heap_break, ptn) == ERR)
		return NULL;
	
	index = bitmap_search_pattern(mem_map, MEM_MAP_LEN, ptn->pattern, ptn->size);
	if(index == ERR)
		return NULL;

	bitmap_set_pattern(mem_map, MEM_MAP_LEN, index, ptn->pattern, ptn->size);
	bitmap_set_pattern(child->ptable, PTABLE_LEN, index, ptn->pattern, ptn->size);

	return PAGE_TO_PADDR(index);
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




