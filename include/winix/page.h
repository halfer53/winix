/**
 * 
 * Page definition and utilities
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:22:22
 * 
*/
#ifndef _W_PAGE_H_
#define _W_PAGE_H_ 1

int align_page(int len);

#define align4(x)                       (((((x)-1)>>2)<<2)+4)
#define ALIGN1K_LB(x)                   ((((((int)x)-1)>>10)<<10))
#define ALIGN1K(x) 	                    (((((x)-1)>>10)<<10)+1023)
#define ALIGN1K_HB(x) 	                (((((x)-1)>>10)<<10)+PAGE_LEN)

#define get_physical_addr(va,proc)      (((ptr_t*)(va))+(unsigned int)(proc)->ctx.rbase)
#define get_virtual_addr(pa,proc)       (((ptr_t*)(pa))-(unsigned int)(proc)->ctx.rbase)

#define PAGE_TO_PADDR(_index)           ((ptr_t *)((_index) * PAGE_LEN))
#define PADDR_TO_PAGED(addr)            ((int)(addr) / PAGE_LEN)
#define PADDR_TO_NUM_PAGES(addr)        ((int)align_page(addr) / PAGE_LEN)

#define PAGE_TO_VADDR(_index,who)       (get_virtual_addr(PAGE_TO_PADDR(_index),who))
#define VADDR_TO_PAGE(addr,who)         (get_virtual_addr(PADDR_TO_PAGED(addr),who))
#define VADDR_TO_NUM_PAGES(addr,who)    (get_virtual_addr(PADDR_TO_NUM_PAGEs(addr),who))

#define clear_page(page)                memset((page), 0xffffffff, PAGE_LEN)
#define copy_page(to,from)              memcpy((to), (from), PAGE_LEN)



#endif
