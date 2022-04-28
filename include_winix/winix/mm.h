/**
 * 
 * Memory management module
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:22:17
 * 
*/
#ifndef    _W_MEM_MAP_H_
#define _W_MEM_MAP_H_

#include <kernel/proc.h>
#include <winix/page.h>
#include <winix/gfp.h>
#include <winix/bitmap.h>

// Predefined, wramp board has 1024 pages in total,
// which equals to 4 * 32
#define MEM_MAP_LEN    4

void init_mem_table();

int peek_free_pages(int length, int flags);
ptr_t* get_free_pages(int length, int flags);
int release_pages(ptr_t* page, int num);
int user_release_pages(struct proc* who, ptr_t* page, int len);
ptr_t* user_get_free_pages(struct proc* who, int length, int flags);
bool is_vaddr_ok(struct proc* who, vptr_t* addr, size_t len);
void add_free_mem(void* addr, size_t size);
void kprint_slab();
int user_get_free_pages_from(struct proc* who, ptr_t* addr, int size);

#define is_vaddr_accessible(addr, who) is_vaddr_ok(who, (vptr_t*)addr, 1)
#define free_page(page)         (release_pages((page),PAGE_LEN))
#define get_free_page(flags)    (get_free_pages(PAGE_LEN,(flags)))
#define user_get_free_page(who,flags)   (user_get_free_pages((who), PAGE_LEN ,(flags)))
#define user_free_page(who,page)        (user_release_pages((who),(page), PAGE_LEN))

int next_free_page_index();
int peek_next_free_page();
int peek_last_free_page();

void* dup_vm(struct proc* parent, struct proc* child);
void kreport_ptable(struct proc* who);
void _kreport_sysmap(int (*func) (const char*, ...));
void _kreport_memtable(int (*func) (const char*, ...));

#define _kreport_memtable(func) _kreport_bitmap(mem_map, MEM_MAP_LEN, func)
#define kreport_sysmap()    _kreport_sysmap(kprintf)
#define kreport_ptable(who) kreport_bitmap(who->ctx.ptable, MEM_MAP_LEN);

#endif
