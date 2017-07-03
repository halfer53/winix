#ifndef _W_SYS_MEMORY_H_
#define _W_SYS_MEMORY_H_



void Scan_FREE_MEM_BEGIN();
int do_brk(proc_t *who, void *addr);
void *do_sbrk(proc_t *who,size_t size);
void *expand_mem(size_t size);

#endif
