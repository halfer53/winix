#include <lib.h>

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
static int count = 0;

#define BLOCK_SIZE (sizeof(struct s_block) - 1)
#define align4(x) (((((x)-1)>>2)<<2)+4)
#define ALIGN1K(x) (((((x)-1)>>10)<<10)+1024)

void printblock(block_t *b) {
	printf("0x%04x size %04d prev 0x%04x next 0x%04x %s\n",
		b, 
		b->size, 
		b->prev, 
		b->next, 
		b->free ? "is free" : "in use");
}

void print_mallinfo() {
	int frees = 0;
	int used = 0;
	block_t *b = base;
	
	if(!b){
		printf("Heap is empty\n");
		return;
	}
	while (b) {
		if(b->free)
			frees += b->size;
		else
			used += b->size;
		printblock(b);
		b = b->next;
	}
	printf("Total free words: %d\nTotal words in use: %d\n", frees, used);
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
	// printf("data 0x%08x new 0x%08x\n", b->data,new);
	new = (block_t *)(b->data + s);
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
block_t *extend_heap(block_t *last , size_t s)
{
	int *sb;
	block_t *b, *b2;
	
	b = sbrk (0);
	if (b != (void *)-1)
	{
		sb = sbrk(BLOCK_SIZE+s);

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
	return NULL;
}

void *malloc(size_t size) {

	block_t *b, *last;
	size_t s;

	count++;
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
	// printf("invalid addr %x base %x sbrk %x\n",p, base, sbrk(0));
	// printblock(p);
	return (0);
}

block_t *fusion(block_t *b) {
	block_t* next = b->next;
	if (next && next->free && ((int)(b->ptr) + b->size == (int)next)) {
		b->size += BLOCK_SIZE + next->size;
		b->next = next->next;
		if (b->next)
			b->next->prev = b;
	}
	return (b);
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
		if (b->prev && b->prev->free){
			// printf("fuse prev %x %x\n", b->prev, b);
			b = fusion(b->prev);
		}
			
		/* then fusion with next */
		if (b->next && b->next->free){
			// printf("fuse next %x %x\n", b->next, b);
			fusion(b);
		}
	}
	// putchar('\n');
}

void *calloc(size_t number , size_t size) {
	size_t *ptr;
	size_t s4, i;
	ptr = malloc(number * size);
	if(ptr)
		memset(ptr, 0, size);
	return ptr;
}


/* Copy data from block to block */
void copy_block(block_t *src, block_t *dst)
{
	memcpy(dst->ptr, src->ptr, src->size);
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


int exit(int status){
	_exit(status);
	//should never get here
	return EXIT_FAILURE;
}


