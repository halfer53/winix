#include "memblock.h"
#include <stdlib.h>


/* Copy data from block to block */
void copy_sblock(struct s_block *src, struct s_block *dst)
{
	int *sdata , *ddata;
	size_t i;
	sdata = src->ptr;
	ddata = dst->ptr;
	for (i = 0; i * 4 < src->size && i * 4 < dst->size; i++)
		ddata[i] = sdata[i];
}

/* The realloc */
/* See realloc(3) */
void *realloc(void *p, size_t size)
{
	size_t s;
	struct s_block *b, *new, *next;
	void *newp;
	if (!p)
		return (malloc(size));
	if (sblock_valid_addr(p))
	{
		s = align4(size);
		b = get_sblock(p);
		if (b->size >= s)
		{
			if (b->size - s >= (BLOCK_SIZE + 4))
				split_sblock(b, s);
		}
		else
		{
			/* Try fusion_sblock with next if possible */
			next = b->next;
			if (next && next->free
					&&	((long)(b->ptr) + b->size == (long)next)
			        && (b->size + BLOCK_SIZE + next->size) >= s)
			{
				fusion_sblock(b);
				if (b->size - s >= (BLOCK_SIZE + 4))
					split_sblock(b, s);
			}
			else
			{
				/* good old realloc with a new block */
				newp = malloc(s);
				if (!newp)
					/* were doomed ! */
					return (NULL);
				/* I assume this work ! */
				new = get_sblock(newp);
				/* Copy data */
				copy_sblock(b, new);
				/* free the old one */
				free(p);
				return (newp);
			}
		}
		return (p);
	}
	return NULL;
}