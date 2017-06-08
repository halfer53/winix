#include <sys/types.h>
#include <stddef.h>
#include <winix/slab.h>
#include <winix/mem_map.h>
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



static void *base = NULL;
static void *end = NULL;

#define BLOCK_SIZE 5

void printblock(block_t *b) {
	int i = (int)b / 1024;
	printf("%d %x size %d next %x prev %x kfree %d data %x\n",i, b, b->size, b->next , b->prev, b->free, b->data);
}

void block_overview() {
	int kfrees = 0;
	block_t *b = base;
	
	if(!b){
		printf("no heap mems\n");
		return;
	}
	while (b) {
		if(b->free)
			kfrees += b->size;
		printblock(b);
		b = b->next;
	}
	printf("total kfrees %d\n", kfrees);
}


block_t *find_block(block_t **last , size_t size) {
	block_t *b = base;
	while (b && !(b->free && b->size >= size)) {
		*last = b;
		b = b->next;
	}
	return (b);
}


/* Split block according to size. */
/* The b block must exist. */
void split_block(block_t *b, size_t s)
{
	block_t *new;
	// printf("data %x new %x\n", b->data,new);
	new = (block_t *)(b->data + s);
	new->size = b->size - s - BLOCK_SIZE;
	new->next = b->next;
	new->prev = b;
	new->free = 1;
	new->ptr = new->data;
	b->size = s;
	// printf("new %x size %d orisize %d\n", new,new->size,b->size);
	b->next = new;
	if (new->next)
		new->next ->prev = new;
}


void alloc_page_rest(block_t *last, int end){
	block_t *b = (block_t*)((int)(last->ptr) + last->size);
	b->size = (end - (int)b - BLOCK_SIZE);
	b->prev = last;
	b->next = last->next;
	b->free = 1;
	b->ptr = b->data;

	last->next = b;

}

/* Add a new block at the of heap */
/* return NULL if things go wrong */
block_t *alloc_page(block_t *last , size_t s)
{
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
	
	block_t *p = get_free_pages(physical_len_to_page_len(s));
	p->size = s;
	p->prev = last;
	p->free = 0;
	p->ptr = p->data;
	alloc_page_rest(p,align1k((int)p));

	if(last)
		last->next = p;

	return p;
}

void *kmalloc(size_t size) {

	block_t *b, *last;
	size_t s;

	// printf("got size %d\n", s);
	s = align4(size);

	if (base) {
		// printf("finding heap\n");
		/* First find a block */
		last = base;
		b = find_block(&last , s);
		if (b) {
			/* can we split */
			if ((b->size - s) >= (BLOCK_SIZE + 4))
				split_block(b, s);
			b->free = 0;
		} else {
			/* No fitting block , extend the heap */
			// printf(" Extend Heap %x\n",b);
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
	// printblock(b);
	return (b->data);
}


/* Copy data from block to block */
void copy_block(block_t *src, block_t *dst)
{
	int *sdata , *ddata;
	size_t i;
	sdata = src->ptr;
	ddata = dst->ptr;
	for (i = 0; i * 4 < src->size && i * 4 < dst->size; i++)
		ddata[i] = sdata[i];
}



block_t *fusion(block_t *b) {
	block_t* next = b->next;
	if (next && next->free && ((int)(b->ptr) + b->size == (int)next)) {
		b->size += BLOCK_SIZE + next->size;
		b->next = next->next;
		if (next)
			next->prev = b;
	}
	return (b);
}



/* Get the block from and addr */
block_t *get_block(void *p){
	return (void *)((int *)p - BLOCK_SIZE);
}

/* Valid addr for kfree */
int valid_addr(void *p)
{
	block_t *b;
	if (base)
	{
		if ( p > base && p < (void *)sbrk (0))
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
			brk(b);
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
			if (b->size - s >= (BLOCK_SIZE + 4))
				split_block(b, s);
		}
		else
		{
			/* Try fusion with next if possible */
			next = b->next;
			if (next && next->free
					&&	((int)(b->ptr) + b->size == (int)next)
			        && (b->size + BLOCK_SIZE + next->size) >= s)
			{
				fusion(b);
				if (b->size - s >= (BLOCK_SIZE + 4))
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



