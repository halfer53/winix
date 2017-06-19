#include <sys/types.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/syscall.h>

/**
 * block structure
 */
typedef struct s_block {
	size_t size;
	struct s_block *next;
	struct s_block *prev;
	int free;
	void *ptr; //a pointer to the allocated block
	char data[1]; //the pointer where the real data is pointed at. b->data is what malloc returns
	//The reason we use data[1] is that c returns the address of array by default, 
	//whereas if we were to use ```char data;```, b-> returns the data instead of the address.
	//note that data is just put there, it lies directly in the memory block where malloc starts.
}block_t;



static void *base = NULL;
static void *end = NULL;

#define BLOCK_SIZE 5
#define align4(x) (((((x)-1)>>2)<<2)+4)
#define align1k(x) (((((x)-1)>>10)<<10)+1024)

void printblock(block_t *b) {
	int i = (int)b / 1024;
	printf("%d %x size %d next %x prev %x free %d data %x\n",i, b, b->size, b->next , b->prev, b->free, b->data);
}

void block_overview() {
	int frees = 0;
	block_t *b = base;
	
	if(!b){
		printf("no heap mems\n");
		return;
	}
	while (b) {
		if(b->free)
			frees += b->size;
		printblock(b);
		b = b->next;
	}
	printf("total frees %d\n", frees);
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

/* Add a new block at the of heap */
/* return NULL if things go wrong */
block_t *extend_heap(block_t *last , size_t s)
{
	int *sb;
	int page_end;
	block_t *b, *b2;
	int size_til_endof_page;
	
	b = sbrk (0);
	if (b != NULL)
	{
		page_end = align1k((int)b);
		size_til_endof_page = page_end - (int)b - BLOCK_SIZE;

		// printf("b %x siend %d\n",b,size_til_endof_page);

		if(size_til_endof_page < s){
			if(size_til_endof_page > 0){
				sb = sbrk(BLOCK_SIZE + size_til_endof_page);
				// printf("b %x sb %x siend %d\n",b,sb,size_til_endof_page);
				if ((int)sb < 0)
					return (NULL);
				if(size_til_endof_page < 4){
					last->size += size_til_endof_page + BLOCK_SIZE;
				}else{
					b->size = size_til_endof_page;
					b->next = NULL;
					b->prev = last;
					b->ptr = b->data;
					if (last)
						last->next = b;
					b->free = 1;
					last = b;
				}
			}

			b2 = sbrk(BLOCK_SIZE+s);
			// printf("nextpage b %x b2 %x \n",b,b2);
			if ((int)b2 < 0)
				return (NULL);
			b2->size = s;
			b2->next = NULL;
			b2->prev = last;
			b2->ptr = b2->data;
			if (last)
				last->next = b2;
			b2->free = 0;
			// printf("size %d\n", b2->size);
			return b2;
		}
	}
	
	sb = sbrk(BLOCK_SIZE+s);
	// printf("extendingheap b %x sb %x \n",b,sb);

	if ((int)sb < 0)
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
			b = extend_heap(last , s);
			
			if (!b)
				return (NULL);
		}
	} else {
		b = extend_heap(NULL , s);
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

/* Valid addr for free */
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

/* The free */
/* See free(3) */
void free(void *p)
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

			/* free the end of the heap */
			if (b->prev)
				b->prev ->next = NULL;
			else
				/* No more block !*/
				base = NULL;
			brk(b);
		}
	}
}

void *calloc(size_t number , size_t size) {
	size_t *new;
	size_t s4, i;
	new = malloc(number * size);
	if (new) {
		s4 = number * size;
		for (i = 0; i < s4 ; i++)
			new[i] = 0;
	}
	return (new);
}




/* The realloc */
/* See realloc(3) */
void *realloc(void *p, size_t size)
{
	size_t s;
	block_t *b, *new, *next;
	void *newp;
	if (!p)
		return (malloc(size));
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
				newp = malloc(s);
				if (!newp)
					/* were doomed ! */
					return (NULL);
				/* I assume this work ! */
				new = get_block(newp);
				/* Copy data */
				copy_block(b, new);
				/* free the old one */
				free(p);
				return (newp);
			}
		}
		return (p);
	}
	return NULL;
}


