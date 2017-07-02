#ifndef	_W_MEM_MAP_H_
#define _W_MEM_MAP_H_

void *get_free_pages(int num);
void free_page(void* ptr);
void print_mem_map(int i);

#define MEM_MAP_LEN	32
void init_mem_table();

extern unsigned long mem_map[MEM_MAP_LEN];

#define align4(x) (((((x)-1)>>2)<<2)+4)
#define align1k(x) (((((x)-1)>>10)<<10)+1024)
#define i_align1k_lb(x) ((((((int)x)-1)>>10)<<10))

#define is_addr_in_same_page(a,b)	((((int)a)/1024) == (((int)b)/1024))
#define is_addr_in_consecutive_page(a,b)	((((int)a)/1024) == ((((int)b)/1024)-1))

#define get_physical_addr(va,proc)	(((unsigned long *)va)+(int)proc->rbase)
#define get_virtual_addr(pa,proc)	(((unsigned long *)pa)-(int)proc->rbase)

#define get_page_index(pa)	( ((int)pa) / 1024 )
#define physical_len_to_page_len(pa_len)	( (align1k((int)pa_len)) / 1024 )
#define page_len_to_physical_len(p_len)	(p_len * 1024)

#endif
