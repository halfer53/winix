#ifndef _MEMBLOCK
#define _MEMBLOCK

#include <stdlib.h>

/**
 * block structure
 */
struct s_block {
	size_t size;
	struct s_block *next;
	struct s_block *prev;
	int free;
	void *ptr; //a pointer to the allocated block
	char data[1]; //the pointer where the real data is pointed at. b->data is what malloc returns
	//The reason we use data[1] is that c returns the address of array by default, 
	//whereas if we were to use ```char data;```, b-> returns the data instead of the address.
	//note that data is just put there, it lies directly in the memory block where malloc starts.
};

#define BLOCK_SIZE 5
#define align4(x) (((((x)-1)>>2)<<2)+4)
#define align1k(x) (((((x)-1)>>10)<<10)+1024)

int sblock_valid_addr(void *p);
struct s_block *get_sblock(void *p);
void split_sblock(struct s_block *b, size_t s);
struct s_block *fusion_sblock(struct s_block *b);
void copy_sblock(struct s_block *src, struct s_block *dst);

#endif
