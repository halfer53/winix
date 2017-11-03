/**
 * Dynamic memory allocation for the kernel
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:12:01
 * 
*/
#include <kernel/kernel.h>
#include <winix/mem_alloc.h>
#include <winix/mm.h>

#define INITIAL_SYSFREEMEM_LEN     64

PRIVATE struct hole hole_table[NUM_HOLES];

// Entries to the list of unallocated memory space in RAM
PRIVATE struct hole *unused_holes[2];

// ENtries in the holes that are not in use, but can be added to the holes list
PRIVATE struct hole *pending_holes[2];

PRIVATE struct hole *used_holes[2];
// Linked lists are defined by a head and tail pointer.

PRIVATE char initial_free_sysmem[INITIAL_SYSFREEMEM_LEN];

/**
 * Adds a hole to the tail of a list.
 *
 * Parameters:
 *   q        An array containing a head and tail pointer of a linked list.
 *   hole    The hole struct to add to the list.
 **/
 PRIVATE void hole_enqueue_tail(struct hole **q, struct hole *hole) {
    if (q[HEAD] == NULL) {
        q[HEAD] = q[TAIL] = hole;
    }
    else {
        q[TAIL]->next = hole;
        q[TAIL] = hole;
    }
    hole->next = NULL;
}

/**
 * Adds a hole to the head of a list.
 *
 * Parameters:
 *   q        An array containing a head and tail pointer of a linked list.
 *   hole    The hole struct to add to the list.
 **/
 PRIVATE void hole_enqueue_head(struct hole **q, struct hole *hole) {
    if (q[HEAD] == NULL) {
        hole->next = NULL;
        q[HEAD] = q[TAIL] = hole;
    }
    else {
        hole->next = q[HEAD];
        q[HEAD] = hole;
    }
}

/**
 * Removes the head of a list.
 *
 * Parameters:
 *   q        An array containing a head and tail pointer of a linked list.
 *
 * Returns:
 *   The hole struct that was removed from the head of the list
 *   NULL if the list is empty.
 **/
PRIVATE struct hole *hole_dequeue(struct hole **q) {
    struct hole *hole = q[HEAD];

    if (hole == NULL)
        return NULL;

    if (q[HEAD] == q[TAIL]) { // Last item
        q[HEAD] = q[TAIL] = NULL;
    }
    else { // At least one remaining item
        q[HEAD] = hole->next;
    }
    hole->next = NULL;
    return hole;
}

PRIVATE void hole_delete2(struct hole **q, struct hole *prev, struct hole *curr) {
    if (curr != NULL) {
        if (prev == NULL) {
            if (q[HEAD] == q[TAIL]) {
                q[HEAD] = q[TAIL] = NULL;
            } else {
                q[HEAD] = curr->next;
            }
        } else {
            prev->next = curr->next;
        }
    }
}

PRIVATE void hole_delete(struct hole **q, struct hole *h) {
    struct hole *curr = q[HEAD];
    struct hole *prev = NULL;

    if (curr == NULL) 
        return;

    while (curr != h && curr != NULL) {
        prev = curr;
        curr = curr->next;
    }
    hole_delete2(q, prev, curr);
}


// allocate a new chunk of memory given the size in the memory
// if loops through the existing holes, and try to find a hole that can fit
// if it can't, sbrk is called to allocate a new memory space
// a new used hole of given size is added to the used hole list
//
void *kmalloc(size_t size) {
    struct hole *prev = NULL;
    struct hole *h = unused_holes[HEAD];
    size_t *p_start_addr = NULL;
    size_t *old_base = 0;

    if (size == 0) {
        return NULL;
    }
    while (h != NULL && h->length < size) {
        prev = h;
        h = h->next;
    }
    // if there is a hole that is big enough to fit
    if (h != NULL) {
        // if we've found a hole taht is big enough
        old_base = h->start;
        // if the hole simply equal to the size required,
        // simply move it from unused holes to used holes
        if (h->length == size) {
            hole_delete2(unused_holes, prev, h);
            hole_enqueue_head(used_holes, h);
        } else {
            h->start += size;
            h->length -= size;
            h = hole_dequeue(pending_holes);
            h->start = old_base;
            h->length = size;
            hole_enqueue_head(used_holes, h);
        }
        return (void *)old_base;
    } else {
        // if no hole size  that is big enough is found in the unused_holes list,
        // it's gonna call sbrk to allocate a new chunk of memory
        if (p_start_addr = get_free_pages(align_page(size), GFP_HIGH)) {
            if (h = hole_dequeue(pending_holes)) {
                h->start = p_start_addr;
                h->length = size;
                hole_enqueue_head(used_holes, h);
                return p_start_addr;
            }
        }// else if system out of memory
    }
    return NULL;
}

// TODO release big enough unused holes to free pages
PRIVATE int merge_holes(struct hole **merging_holes_list, struct hole *h) {
    
        struct hole *curr = merging_holes_list[HEAD];
    
        while (curr != NULL) {
            // if there is hole that is adjacent to the hole to be merged
            if (curr->start + curr->length == h->start) {
                curr->length += h->length;
                break;
            } else if (h->start + h->length == curr->start) {
                curr->start -= h->length;
                curr->length += h->length;
                break;
            }
            curr = curr->next;
        }
    
    // if curr is not null, that means it is merged with other holes, whose size is increased by the size of h
    // so we simply add h to the pending_holes list
        if (curr != NULL) {
            hole_enqueue_head(pending_holes, h);
            return OK;
        } else {
            // if curr is null, that means it can't merge with any holes
            hole_enqueue_head(unused_holes, h);
            return ERR;
        }
    }

// equivalent to free()
// it loops through the used holes list, and find any starting address of the
// used hole that matches the parameter.
// hole is deleted from used holes, and added to the unused holes, if it finds it
// if it can't find any matching holes, it does nothing
void kfree(void *ptr_parameter) {
    size_t *p = (size_t *)ptr_parameter;
    struct hole *h = used_holes[HEAD];
    int i = 0;
    size_t start = 0;
    size_t hole_length = 0;

    while (h != NULL && h->start != p) {
        h = h->next;
    }
    if (h != NULL) {
        // for ( i = 0; i < h->length; i++) {
        //     *p = DEFAULT_MEM_VALUE;
        //     p++;
        // }

        hole_delete(used_holes, h);
        // try to merge the newly deleted hole with exiting unused holes
        merge_holes(unused_holes, h);
    }
}

void print_holelist() {
    struct hole *curr = unused_holes[HEAD];
    if (curr == NULL) {
        kprintf("unused hole empty\n" );
    }
    while (curr != NULL) {
        kprintf("unused hole start 0x%08x, length %d\n", curr->start, curr->length );
        curr = curr->next;
    }

    curr = used_holes[HEAD];
    if (curr == NULL) {
        kprintf("used holes empty\n" );
    }
    while (curr != NULL) {
        kprintf("used hole start 0x%08x, length %d\n", curr->start, curr->length );
        curr = curr->next;
    }

}

void add_free_mem(void* addr, size_t size){
    struct hole *h;
    h = hole_dequeue(pending_holes);
    h->start = addr;
    h->length = size;
    hole_enqueue_head(unused_holes, h);
}


void init_holes() {
    struct hole *h = NULL;
    int i = 0;
    unused_holes[HEAD] = unused_holes[TAIL] = NULL;
    used_holes[HEAD] = used_holes[TAIL] = NULL;
    pending_holes[HEAD] = pending_holes[TAIL] = NULL;

    for ( i = 0; i < NUM_HOLES; i++) {
        h = &hole_table[i];
        h->start = 0;
        h->length = 0;
        h->next = NULL;
        hole_enqueue_head(pending_holes, h);
    }
    add_free_mem(initial_free_sysmem, INITIAL_SYSFREEMEM_LEN);
}
