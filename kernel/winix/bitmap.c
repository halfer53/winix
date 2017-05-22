#include "bitmap.h"

static unsigned long mask[BITMASK_NR];

void init_bitmap(){
	register int i;
	mask[BITMASK_NR-1] = 1;
    for(i=BITMASK_NR-2;i>=0;i--){
        mask[i] = mask[i+1] << 1;
    }
}

void bitmap_reset_all(unsigned long *map, int map_len){
	register int i;
	for (i=0; i < map_len; ++i)
	{
		map[i] = 0;
	}
}

void bitmap_set_all(unsigned long *map, int map_len){
	register int i;
	for (i=0; i < map_len; ++i)
	{
		map[i] = 0xffffffff;
	}
}
/**
 * search from start
 * @param  map     memory map
 * @param  map_len memory map length
 * @param  start   starting bit to search from memory map, 0 <= start < 1024
 * @param  num     number of bits to search
 * @return         bit found
 */
int bitmap_search_from(unsigned long *map, int map_len, int start, int num){
	register int i,j;
	int count = 0;

	if(num >= map_len * 32 || start >= map_len * 32)	return -1;

	i = start / 32;
	j = start % 32;

	for (; i < map_len; ++i){
		for (; j< BITMASK_NR; j++) {
            if ((map[i] & mask[j]) == 0) {
                count++;
                if (count == num) {
                    return (i*32 + j - count+1);
                }
            }else{
                count = 0;
            }
        }
        j=0;
	}
	return -1;
}

int bitmap_search(unsigned long *map, int map_len, int num){
	return bitmap_search_from(map,map_len,0,num);
}

void bitmap_set_bit(unsigned long *map, int map_len,int start){
	int ibit = start/32;
	if(start >= map_len * 32)	return;
    map[ibit] = map[ibit] | mask[start%32];
}


void bitmap_set_nbits(unsigned long *map, int map_len,int start, int len){
	register int i;
	int inum;
	if(start + len >= map_len * 32)	return;
	for(i=0 ; i<len ; i++){
		inum = start + i;
		map[inum/32] = map[inum/32] | mask[inum%32];
	}
}

void bitmap_reset_nbits(unsigned long *map, int map_len,int start, int len){
	register int i;
	int inum;
	if(start + len >= map_len * 32)	return;
	for(i=0 ; i<len ; i++){
		inum = start + i;
		map[inum/32] = map[inum/32] & (~mask[inum%32]);
	}
}


void bitmap_reset_bit(unsigned long *map, int map_len,int start){
	int ibit = start/32;
	if(start >= map_len * 32)	return;
    map[ibit] = map[ibit] & (~mask[start%32]);
}
