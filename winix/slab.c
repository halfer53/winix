#include <kernel/kernel.h>
#include <winix/slab.h>
/**
 * block structure
 */
typedef struct s_block {
	size_t size;
	struct s_block *next;
	struct s_block *prev;
	int free;
	void *ptr; //a pointer to the allocated block
	char data[1]; //the pointer where the real data is pointed at. b->data is what kmalloc returns
	//The reason we use data[1] is that c returns the address of array by default, 
	//whereas if we were to use ```char data;```, b-> returns the data instead of the address.
	//note that data is just put there, it lies directly in the memory block where kmalloc starts.
}block_t;



static block_t *base = NULL;
static block_t *end = NULL;

#define SLAB_BLOCK_SIZE 5

void init_slab(void *addr,int size){
	base = (block_t *)addr;
	base->size = size;
	base->free = 1;
	base->ptr = base->data;
	base->next = base->prev = NULL;
}

PRIVATE void kprintblock(block_t *b) {
	kprintf("0x%08x size %d next 0x%08x prev 0x%08x free %d data 0x%08x\n",b, b->size, b->next , b->prev, b->free, b->data);
}

void kblock_overview() {
	int kfrees = 0;
	block_t *b = base;
	
	if(!b){
		kprintf("no heap mems\n");
		return;
	}
	while (b) {
		if(b->free)
			kfrees += b->size;
		kprintblock(b);
		b = b->next;
	}
	kprintf("total frees %d\n", kfrees);
}


PRIVATE block_t *find_block(block_t **last , size_t size) {
	block_t *b = base;
	while (b && !(b->free && b->size >= size)) {
		*last = b;
		b = b->next;
	}
	return (b);
}


/* Split block according to size. */
/* The b block must exist. */
PRIVATE void split_block(block_t *b, size_t s)
{
	block_t *new;
	// kprintf("data 0x%08x new 0x%08x\n", b->data,new);
	new = (block_t *)(b->data + s);
	new->size = b->size - s - SLAB_BLOCK_SIZE;
	new->next = b->next;
	new->prev = b;
	new->free = 1;
	new->ptr = new->data;
	b->size = s;
	// kprintf("new 0x%08x size %d orisize %d\n", new,new->size,b->size);
	b->next = new;
	if (new->next)
		new->next ->prev = new;
}


PRIVATE void alloc_page_rest(block_t *last, int end){
	block_t *b = (block_t*)((int)(last->ptr) + last->size);
	b->size = (end - (int)b - SLAB_BLOCK_SIZE);
	b->prev = last;
	b->next = last->next;
	b->free = 1;
	b->ptr = b->data;

	last->next = b;
}

/* Add a new block at the of heap */
/* return NULL if things go wrong */
PRIVATE block_t *alloc_page(block_t *last , size_t s)
{
	int pg_idx;
	block_t *p;
	//if last is null, get new free page, pre allocate
	//else if last is not null
	//	if last is at the end of the heap
	//		check if it can extend to the next pages
	//			do staff
	//		else
	//			free last
	//			get new pages and return the new pointer
	//	else
	//		get new pages and return
	
	p = (block_t *)get_free_pages(align_page(s),GFP_NORM);
	p->size = s;
	p->prev = last;
	p->free = 0;
	p->ptr = p->data;
	alloc_page_rest(p,align_page((int)p));

	if(last)
		last->next = p;

	return p;
}

void *kmalloc(size_t size) {

	block_t *b, *last;
	size_t s;

	// kprintf("got size %d\n", s);
	s = align4(size);

	if (base) {
		// kprintf("finding heap\n");
		/* First find a block */
		last = base;
		b = find_block(&last , s);
		if (b) {
			/* can we split */
			if ((b->size - s) >= (SLAB_BLOCK_SIZE + 4))
				split_block(b, s);
			b->free = 0;
		} else {
			/* No fitting block , extend the heap */
			// kprintf(" Extend Heap 0x%08x\n",b);
			b = alloc_page(last , s);
			
			if (!b)
				return (NULL);
		}
	} else {
		b = alloc_page(NULL , s);
		if (!b)
			return (NULL);
		base = b;
	}
	// kprintblock(b);
	return (b->data);
}


/* Copy data from block to block */
PRIVATE void copy_block(block_t *src, block_t *dst)
{
	int *sdata , *ddata;
	size_t i;
	sdata = src->ptr;
	ddata = dst->ptr;
	for (i = 0; i * 4 < src->size && i * 4 < dst->size; i++)
		ddata[i] = sdata[i];
}



PRIVATE block_t *fusion(block_t *b) {
	block_t* next = b->next;
	if (next && next->free && ((int)(b->ptr) + b->size == (int)next)) {
		b->size += SLAB_BLOCK_SIZE + next->size;
		b->next = next->next;
		if (next)
			next->prev = b;
	}
	return (b);
}



/* Get the block from and addr */
PRIVATE block_t *get_block(void *p){
	return (void *)((int *)p - SLAB_BLOCK_SIZE);
}

/* Valid addr for kfree */
PRIVATE int valid_addr(void *p)
{
	block_t *b;
	if (base)
	{
		if ( p > (void *)base )
		{
			return (p == (get_block(p))->ptr);
		}
	}
	return (0);
}

/* The kfree */
/* See kfree(3) */
void kfree(void *p)
{
	block_t *b;
	if (valid_addr(p))
	{
		b = get_block(p);
		b->free = 1;
		/* fusion with previous if possible */
		if (b->prev && b->prev->free)
			b = fusion(b->prev);
		/* then fusion with next */
		if (b->next){
			fusion(b);
		}else{

			/* kfree the end of the heap */
			if (b->prev)
				b->prev ->next = NULL;
			else
				/* No more block !*/
				base = NULL;
		}
	}
}

void *kcalloc(size_t number , size_t size) {
	size_t *new;
	size_t s4, i;
	new = kmalloc(number * size);
	if (new) {
		s4 = number * size;
		for (i = 0; i < s4 ; i++)
			new[i] = 0;
	}
	return (new);
}


void *krealloc(void *p, size_t size)
{
	size_t s;
	block_t *b, *new, *next;
	void *newp;
	if (!p)
		return (kmalloc(size));
	if (valid_addr(p))
	{
		s = align4(size);
		b = get_block(p);
		if (b->size >= s)
		{
			if (b->size - s >= (SLAB_BLOCK_SIZE + 4))
				split_block(b, s);
		}
		else
		{
			/* Try fusion with next if possible */
			next = b->next;
			if (next && next->free
					&&	((int)(b->ptr) + b->size == (int)next)
			        && (b->size + SLAB_BLOCK_SIZE + next->size) >= s)
			{
				fusion(b);
				if (b->size - s >= (SLAB_BLOCK_SIZE + 4))
					split_block(b, s);
			}
			else
			{
				/* good old realloc with a new block */
				newp = kmalloc(s);
				if (!newp)
					/* were doomed ! */
					return (NULL);
				/* I assume this work ! */
				new = get_block(newp);
				/* Copy data */
				copy_block(b, new);
				/* kfree the old one */
				kfree(p);
				return (newp);
			}
		}
		return (p);
	}
	return NULL;
}



