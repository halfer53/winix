/**
 * Memory allocation module
 * This is an abstraction layer of the system bitmap. System memories
 * are allocated using bitmap mem_map records the the page information
 * of in the system wise. If one page is being used, 1 is set on the 
 * corresponding bit, and vice versa. 
 *
 * mm is an abstraction of the bitmap, its public interface takes physical
 * address of the page, and calculate the corresponding
 * page descriptor to do processing in the system bitmap. All mm takes 
 * physical address as the page, and calculate the length of page by words
 *
 * Internally mm uses bitmap utilities to search, set, unset bits and pages
 * in the system bitmap.
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:12:27
 * 
*/
#include <kernel/kernel.h>
#include <winix/mm.h>
#include <winix/bitmap.h>

PRIVATE unsigned int mem_map[MEM_MAP_LEN];
PRIVATE int bss_page_end;

/**
 * is the address accessible by the proc
 * @param  who  
 * @param  addr 
 * @return      
 */
bool is_vaddr_ok( vptr_t* addr, struct proc* who){
    int page;
    ptr_t* paddr;

    paddr = get_physical_addr(addr, who);
    page = PADDR_TO_PAGED(paddr);
    return is_bit_on(who->ctx.ptable, PTABLE_LEN, page);
}

/**
 * is the given page address free
 * @param  addr the starting address of the page
 * @return      
 */
bool is_page_free(ptr_t* addr){
    int paged = PADDR_TO_PAGED(addr);

    return !is_bit_on(mem_map, MEM_MAP_LEN, paged);
}

/**
 * is page free from the address given
 * for instance, given addr = 1024, size = 2048
 * it means are the pages from 1024 to 3072 free?
 * @param  addr physical address
 * @param  size 
 * @return      
 */
bool is_pages_free_from(ptr_t* addr, int size){
    int i;
    int paged = PADDR_TO_PAGED(addr);
    int page_num = PADDR_TO_NUM_PAGES(size);

    // kprintf("is page free from 0x%x %d with lenth %d\n", addr, paged, page_num);
    for(i = 0; i < page_num; i++){
        if(is_bit_on(mem_map, MEM_MAP_LEN, paged)){
            return false;
        }
        paged++;
    }
    return true;
}

/**
 * get free pages from the system
 * @param  length length in words
 * @param  flags  High mem or Normal mem
 * @return        pointer to the start of the page, or Null if failed
 */
ptr_t *get_free_pages(int length, int flags) {
    int nstart;
    int num = PADDR_TO_NUM_PAGES(length);
    if(flags & GFP_HIGH){
        nstart =  bitmap_search_reverse(mem_map, MEM_MAP_LEN, num);
    }else{
        nstart =  bitmap_search_from(mem_map, MEM_MAP_LEN, bss_page_end, num);
    }
    
    if (nstart >= 0){
        bitmap_set_nbits(mem_map, MEM_MAP_LEN, nstart, num);
        return PAGE_TO_PADDR(nstart);
    }
    return NULL;
}

/**
 * similar to get_free_pages(), the only diff is that this one sets bits on 
 * corresponding the pages it got from get_free_pages(), so that the user can access these
 * allocated pages
 * @param  who    
 * @param  length 
 * @param  flags  
 * @return        
 */
ptr_t* user_get_free_pages(struct proc* who, int length, int flags){
    int index;
    ptr_t* p;
    int page_num;

    p = get_free_pages(length,flags);
    if(p == NULL)
        return NULL;
    index = PADDR_TO_PAGED(p);
    page_num = PADDR_TO_NUM_PAGES(length);
    if(bitmap_set_nbits(who->ctx.ptable, PTABLE_LEN, index, page_num) == ERR)
        return NULL;
    return p;
}

/**
 * get free pages starting froma addr, with size 
 * @param  addr 
 * @param  size 
 * @return      
 */
int get_free_pages_from(ptr_t* addr, int size){
    int ret;
    int paged, page_num;

    // kprintf("extending from 0x%x with size %d\n", addr, size);
    // kreport_sysmap();
    if(!is_pages_free_from(addr, size))
        return ENOMEM;
    paged = PADDR_TO_PAGED(addr);
    page_num = PADDR_TO_NUM_PAGES(size);
    return bitmap_set_nbits(mem_map, MEM_MAP_LEN, paged, page_num);
}

/**
 * user bits are on, so that user can access these
 * @param  who  
 * @param  addr 
 * @param  size 
 * @return      
 */
int user_get_free_pages_from(struct proc* who, ptr_t* addr, int size){
    int index;
    int error;
    int page_num;

    error = get_free_pages_from(addr,size);
    if(error)
        return error;

    index = PADDR_TO_PAGED(addr);
    page_num = PADDR_TO_NUM_PAGES(size);
    return bitmap_set_nbits(who->ctx.ptable, PTABLE_LEN, index, page_num);
}

/**
 * returns the next free page in the system
 * @return 
 */
int peek_next_free_page(){
    return bitmap_search_from(mem_map, MEM_MAP_LEN, bss_page_end,  1);
}

int peek_last_free_page(){
    return bitmap_search_reverse(mem_map, MEM_MAP_LEN, 1);
}

/**
 * free the pages given, similar to free() you see in the user space
 * @param  page 
 * @param  len  
 * @return      
 */
int release_pages(ptr_t* page, int len){
    int page_index;
    if((int)page % PAGE_LEN != 0)
        return ERR;
    page_index = PADDR_TO_PAGED(page);
    return bitmap_clear_nbits(mem_map, MEM_MAP_LEN, page_index, len);
}

int user_release_pages(struct proc* who, ptr_t* page, int len){
    int index;
    if(release_pages(page,len) != OK)
        return ERR;
    index = PADDR_TO_PAGED(page);
    return bitmap_clear_nbits(who->ctx.ptable, PTABLE_LEN, index, len);
}

/**
 * duplicate the virtual address from parent to child
 * The process image are not copied though
 * @param parent 
 * @param child  
 * @param ptn    
 * return the new rbase of the child
 */
void* dup_vm(struct proc* parent, struct proc* child){
    int len;

    len = parent->heap_bottom + 1 - parent->mem_start;
    
    return user_get_free_pages(child, len, GFP_NORM);
    // int index;
    // if(bitmap_extract_pattern(parent->ctx.ptable, MEM_MAP_LEN, (int)child->heap_break, ptn) == ERR)
    //     return NULL;
    
    // index = bitmap_search_pattern(mem_map, MEM_MAP_LEN, ptn->pattern, ptn->size);
    // if(index == ERR)
    //     return NULL;

    // bitmap_set_pattern(mem_map, MEM_MAP_LEN, index, ptn->pattern, ptn->size);
    // bitmap_set_pattern(child->ctx.ptable, PTABLE_LEN, index, ptn->pattern, ptn->size);

    // return PAGE_TO_PADDR(index);
}

/**
 * release process memory
 * @param who 
 */
void release_proc_mem(struct proc *who){
    // struct proc* parent = get_proc(who->parent);
    // if(parent->state & STATE_VFORKING){
    //     return;
    // }
    // KDEBUG(("release proc mem %d %d\n", who->proc_nr, who->thread_parent));
    if(who->thread_parent > 0){ // thread, in this case, we only release the stack
        user_release_pages(who, who->stack_top, 1);
    }else{
        int page_len = (int)(who->heap_bottom + 1 - who->mem_start) / PAGE_LEN;
        int start_page = (int)who->mem_start / PAGE_LEN;
        bitmap_clear_nbits(mem_map, MEM_MAP_LEN, start_page, page_len );
        // bitmap_xor(mem_map, who->ctx.ptable, MEM_MAP_LEN);
        bitmap_clear(who->ctx.ptable, PTABLE_LEN);
    }
    
}

void kreport_ptable(struct proc* who){
    kreport_bitmap(who->ctx.ptable, MEM_MAP_LEN);
}

void kreport_sysmap(){
    static char free_str[] = "Free";
    static char used_str[] = "Used";
    int flags, pages, i;
    char* str;
    kprintf("Sys Mem bitmap: ");
    kreport_bitmap(mem_map, MEM_MAP_LEN);

    for(i = 0; i < 2; i++){
        flags = i == 0 ? ZERO_BITS : ONE_BITS;
        pages = count_bits(mem_map, MEM_MAP_LEN, flags);
        str = i == 0 ? free_str : used_str;
        kprintf(" %s pages: %03d\t",str, pages, pages);
    }
    kprintf("\n");
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
    bss_page_end = PADDR_TO_PAGED(free_mem_begin);
}




