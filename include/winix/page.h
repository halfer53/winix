#ifndef _W_PAGE_H_
#define _W_PAGE_H_ 1

#define PAGE_LEN    1024

#define align4(x) (((((x)-1)>>2)<<2)+4)
#define ALIGN1K_LB(x) ((((((int)x)-1)>>10)<<10))

#define get_physical_addr(va,proc)	(((ptr_t*)(va))+(unsigned int)(proc)->rbase)
#define get_virtual_addr(pa,proc)	(((ptr_t*)(pa))-(unsigned int)(proc)->rbase)

#define PAGE_TO_PADDR(_index)   ((ptr_t *)((_index) * PAGE_LEN))
#define PADDR_TO_PAGED(addr)    ((int)(addr) / PAGE_LEN)
#define PADDR_TO_NUM_PAGES(addr)     ((int)align_page(addr) / PAGE_LEN)

#define PAGE_TO_VADDR(_index,who)   (get_virtual_addr(PAGE_TO_PADDR(_index),who))
#define VADDR_TO_PAGE(addr,who)    (get_virtual_addr(PADDR_TO_PAGED(addr),who))
#define VADDR_TO_NUM_PAGES(addr,who)     (get_virtual_addr(PADDR_TO_NUM_PAGEs(addr),who))

#define clear_page(page)	memset((page), 0xffffffff, PAGE_LEN)
#define copy_page(to,from)	memcpy((to), (from), PAGE_LEN)

#define is_addr_in_same_page(a,b)	((((int)a)/1024) == (((int)b)/1024))
#define is_addr_in_consecutive_page(a,b)	((((int)a)/1024) == ((((int)b)/1024)-1))


#endif
