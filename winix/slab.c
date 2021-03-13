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
#include <winix/mm.h>
#include <stdbool.h>
/**
 * block structure
 */
struct mem_block {
    size_t size;
    struct mem_block *next;
    struct mem_block *prev;
    void *ra;
    int free;
    void *ptr; // a pointer to the allocated block for magic checking
    char data[1]; // the pointer where the real data is pointed at. b->data is what kmalloc returns
    // The reason we use data[1] is that c returns the address of array by default, 
    // whereas if we were to use ```char data;```, b-> returns the data instead of the address.
    // note that data is just put there, it lies directly in the memory block where kmalloc starts.
};

static void *base = NULL;

#define SLAB_HEADER_SIZE 	(sizeof(struct mem_block) - 1)
#define align4(x) 	(((((x)-1)>>2)<<2)+4)

void kprintblock(struct mem_block *b) {
    char *prev, *next;
    prev = (b->prev ? b->prev->data : (char *)b->prev);
    next = (b->next ? b->next->data : (char *)b->next);
    kprintf("0x%08x size %4d prev 0x%08x next 0x%08x ra %08x %s\n",
        b->data,
        b->size, 
        prev, 
        next, 
        b->ra,
        b->free ? "is free" : "in use");
}

void kprint_slab() {
    int frees = 0;
    int used = 0;
    struct mem_block *b = base;
    
    if(!b){
        kprintf("slab is empty\n");
        return;
    }
    while (b) {
        if(b->free)
            frees += b->size;
        else
            used += b->size;
        kprintblock(b);
        b = b->prev;
    }
    kprintf("Total free words: %d\nTotal words in use: %d\n", frees, used);
}


struct mem_block *find_block(struct mem_block **first , size_t size) {
    struct mem_block *b = base;
    while (b && !(b->free && b->size >= size)) {
        *first = b;
        b = b->prev;
    }
    return (b);
}


/* Split block according to size. */
/* The b block must exist. */
struct mem_block* split_block(struct mem_block *b, size_t s)
{
    struct mem_block *new;
    new = (struct mem_block *)(b->data + b->size - s - SLAB_HEADER_SIZE);
    new->size = s;
    new->next = b->next;
    new->prev = b;
    new->free = true;
    new->ptr = new->data;

    // KDEBUG(("size %d new 0x%08x size %d orisize %d\n", s, new->data, new->size, b->size));
    b->size = b->size - s - SLAB_HEADER_SIZE;
    
    b->next = new;
    if(b == base){
        base = new;
    }
    if (new->next)
        new->next->prev = new;
    // kprintblock(b);
    return new;
}


/* Add a new block at the of heap */
/* return NULL if things go wrong */
struct mem_block *extend_heap(struct mem_block *first , size_t s)
{
    int *sb;
    struct mem_block *b, *b2;
    ptr_t *ptr;
    size_t round_s;

    if(s < SLAB_HEADER_SIZE){
        s = SLAB_HEADER_SIZE + 1;
    }

    round_s = ALIGN1K_HB(s);
    
    ptr = get_free_pages(round_s / PAGE_LEN, GFP_HIGH);
    if(!ptr){
        kerror("no mem left %d %d \n", s, round_s);
        return NULL;
    }
    
    b = (struct mem_block *)ptr;
    b->size = round_s - SLAB_HEADER_SIZE;
    b->prev = NULL;
    b->next = first;
    b->ptr = b->data;
    b->free = true;
    if (first)
        first->prev = b;
    if(!base)
        base = b;

    if (round_s - s > SLAB_HEADER_SIZE * 2){
        b2 = split_block(b, s);
        return b2;
    }

    return b;
}

void *_kmalloc(size_t size, void* ra) {

    struct mem_block *b, *first, *b2;
    size_t s = size;

    // s = align4(size);
    // KDEBUG(("kmalloc begin size %d by %x\n", size, ra));

    if (base) {
        // kprintf("finding heap\n");
        /* First find a block */
        first = base;
        b = find_block(&first , s);
        if (b) {
            /* can we split */
            if ((b->size - s) >= (SLAB_HEADER_SIZE * 2)){
                b2 = split_block(b, s);
                b = b2;
            } 
        } else {
            /* No fitting block , extend the heap */
            b = extend_heap(first , s);
            if (!b)
                return (NULL);
        }
    } else {
        b = extend_heap(NULL , s);
        if (!b)
            return (NULL);    
    }
    b->ra = ra;
    b->free = false;
    // KDEBUG(("kmalloc %x size %d to %x\n", b->data, size, ra));
    // printblock(b);
    return (b->data);
}

/* Get the block from and addr */
struct mem_block *get_block(void *p){
    return (void *)((int *)p - SLAB_HEADER_SIZE);
}

/* Valid addr for free */
bool valid_addr(void *p)
{
    struct mem_block *b;
    bool result = p && (get_block(p))->ptr == p;
    return result;
}

struct mem_block *fusion(struct mem_block *b) {
    struct mem_block* next = b->next;
    // KDEBUG(("before fusing %x ", b->data));
    // kprint_slab();
    if (next && next->free && ((int)(b->ptr) + b->size == (int)next)) {
        b->size += SLAB_HEADER_SIZE + next->size;
        b->next = next->next;
        if(next == base){
            base = b;
        }
        if (b->next)
            b->next->prev = b;
    }
    // KDEBUG(("after fusing %x ", b->data));
    // kprint_slab();
    return (b);
}

/* The free */
/* See free(3) */
void _kfree(void *p, void *ra)
{
    struct mem_block *b;
    if (valid_addr(p))
    {
        b = get_block(p);
        b->free = true;
        /* fusion with previous if possible */
        if (b->prev && b->prev->free){
            // kprintf("fuse prev %x %x\n", b->prev, b);
            b = fusion(b->prev);
        }
        /* then fusion with next */
        if (b->next && b->next->free){
            // kprintf("fuse next %x %x\n", b->next, b);
            fusion(b);
        }
        // KDEBUG(("kfree %x by %x\n", p, ra));
        // kprint_slab();
    }else{
        kwarn("invalid addr %x by %x\n", p, ra);
    }
    
    // putchar('\n');
}

void add_free_mem(void* addr, size_t size){
    
}

// void *kcalloc(size_t number , size_t size) {
//     size_t *ptr;
//     size_t s4, i;
//     ptr = malloc(number * size);
//     if(ptr)
//         memset(ptr, 0, size);
//     return ptr;
// }


// /* Copy data from block to block */
// void copy_block(struct mem_block *src, struct mem_block *dst)
// {
//     memcpy(dst->ptr, src->ptr, src->size);
// }

// /* The realloc */
// /* See realloc(3) */
// void *krealloc(void *p, size_t size)
// {
//     size_t s;
//     struct mem_block *b, *new, *next;
//     void *newp;
//     if (!p)
//         return (malloc(size));
//     if (valid_addr(p))
//     {
//         s = align4(size);
//         b = get_block(p);
//         if (b->size >= s)
//         {
//             if (b->size - s >= (SLAB_HEADER_SIZE + 4))
//                 split_block(b, s);
//         }
//         else
//         {
//             /* Try fusion with next if possible */
//             next = b->next;
//             if (next && next->free
//                     &&    ((int)(b->ptr) + b->size == (int)next)
//                     && (b->size + SLAB_HEADER_SIZE + next->size) >= s)
//             {
//                 fusion(b);
//                 if (b->size - s >= (SLAB_HEADER_SIZE + 4))
//                     split_block(b, s);
//             }
//             else
//             {
//                 /* good old realloc with a new block */
//                 newp = malloc(s);
//                 if (!newp)
//                     /* were doomed ! */
//                     return (NULL);
//                 /* I assume this work ! */
//                 new = get_block(newp);
//                 /* Copy data */
//                 copy_block(b, new);
//                 /* free the old one */
//                 free(p);
//                 return (newp);
//             }
//         }
//         return (p);
//     }
//     return NULL;
// }

void init_holes() {
    
}
