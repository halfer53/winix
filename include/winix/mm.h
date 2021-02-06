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

void init_mem_table();

ptr_t* get_free_pages(int length, int flags);
int release_pages(ptr_t* page, int num);
int user_release_pages(struct proc* who, ptr_t* page, int len);
ptr_t* user_get_free_pages(struct proc* who, int length, int flags);
bool is_vaddr_ok(vptr_t* addr,struct proc* who);


#define is_vaddr_accessible(addr, who) is_vaddr_ok((vptr_t*)addr, who)
#define free_page(page)         (release_pages((page),1))
#define get_free_page(flags)    (get_free_pages(1,(flags)))
#define user_get_free_page(who,flags)   (user_get_free_pages((who),1,(flags)))
#define user_free_page(who,page)        (user_release_pages((who),(page),1))

int next_free_page_index();

void* dup_vm(struct proc* parent, struct proc* child);
void kreport_ptable(struct proc* who);
void kreport_sysmap();

#endif
