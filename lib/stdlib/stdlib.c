#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "memblock.h"


static void *base = NULL;

// void printsblock(struct s_block *b) {
//     printf("0x%04x size %04d prev 0x%04x next 0x%04x %s\n",
//         b, 
//         b->size, 
//         b->prev, 
//         b->next, 
//         b->free ? "is free" : "in use");
// }

// void print_mallinfo() {
//     int frees = 0;
//     int used = 0;
//     struct s_block *b = base;
    
//     if(!b){
//         printf("Heap is empty\n");
//         return;
//     }
//     while (b) {
//         if(b->free)
//             frees += b->size;
//         else
//             used += b->size;
//         printsblock(b);
//         b = b->next;
//     }
//     printf("Total free words: %d\nTotal words in use: %d\n", frees, used);
// }

struct s_block *find_sblock(struct s_block **last , size_t size) {
	struct s_block *b = base;
	while (b && !(b->free && b->size >= size)) {
		*last = b;
		b = b->next;
	}
    // printf("find_sblock base 0x%08x, b %p\n", base, b);
	return (b);
}


/* Split block according to size. */
/* The b block must exist. */
void split_sblock(struct s_block *b, size_t s)
{
	struct s_block *new;
	// printf("data 0x%08x new 0x%08x\n", b->data,new);
	new = (struct s_block *)(b->data + s);
	new->size = b->size - s - BLOCK_SIZE;
	new->next = b->next;
	new->prev = b;
	new->free = 1;
	new->ptr = new->data;
	b->size = s;
	// printf("new 0x%08x size %d orisize %d\n", new,new->size,b->size);
	b->next = new;
	if (new->next)
		new->next ->prev = new;
}

/* Add a new block at the of heap */
/* return NULL if things go wrong */
struct s_block *extend_sblock_heap(struct s_block *last , size_t s)
{
	int *sb;
    struct s_block *b;
    
    b = sbrk (0);
    sb = sbrk(BLOCK_SIZE+s);

    if ((long)sb < 0)
        return (NULL);
        
    b->size = s;
    b->next = NULL;
    b->prev = last;
    b->ptr = b->data;
    if (last)
        last->next = b;
    b->free = 0;
    return b;
}

void *malloc(size_t size) {

	struct s_block *b, *last;
	size_t s;

	// printf("got size %d\n", s);
	s = align4(size);

	if (base) {
		// printf("finding heap\n");
		/* First find a block */
		last = base;
		b = find_sblock(&last , s);
		if (b) {
			/* can we split */
			if ((b->size - s) >= (BLOCK_SIZE + 4))
				split_sblock(b, s);
			b->free = 0;
		} else {
			/* No fitting block , extend the heap */
			// printf(" Extend Heap 0x%x %d\n", size, size);
			b = extend_sblock_heap(last , s);
			
			if (!b)
				return (NULL);
		}
	} else {
		b = extend_sblock_heap(NULL , s);
		if (!b)
			return (NULL);
        // printf("setting base %p\n",(void*)b);
		base = b;
	}
	return (b->data);
}



struct s_block *fusion_sblock(struct s_block *b) {
	struct s_block* next = b->next;
	if (next && next->free && ((long)(b->ptr) + b->size == (long)next)) {
		b->size += BLOCK_SIZE + next->size;
		b->next = next->next;
		if (b->next)
			b->next->prev = b;
	}
	return (b);
}



/* Get the block from and addr */
struct s_block *get_sblock(void *p){
	return (void *)((int *)p - BLOCK_SIZE);
}

/* Valid addr for free */
int sblock_valid_addr(void *p)
{
	if (base)
	{
		if ( p > base && p < (void *)sbrk (0))
		{
			return (p == (get_sblock(p))->ptr);
		}
	}
	return (0);
}

/* The free */
/* See free(3) */
void free(void *p)
{
	struct s_block *b;
    // printf("free %p | ", p);
	if (sblock_valid_addr(p))
	{
		b = get_sblock(p);
		b->free = 1;
		/* fusion_sblock with previous if possible */
		if (b->prev && b->prev->free)
			b = fusion_sblock(b->prev);

		/* then fusion with next */
        if (b->next && b->next->free){
            // printf("fuse next %x %x\n", b->next, b);
            fusion_sblock(b);
        }
	}
}








