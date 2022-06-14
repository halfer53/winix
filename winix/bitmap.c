/**
 * 
 * Winix bitmap module
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:11:36
 * 
*/
#include <kernel/kernel.h>
#include <winix/bitmap.h>

unsigned int mask[BITMASK_NR] = {
    0x80000000,
    0x40000000,
    0x20000000,
    0x10000000,
    0x08000000,
    0x04000000,
    0x02000000,
    0x01000000,
    0x00800000,
    0x00400000,
    0x00200000,
    0x00100000,
    0x00080000,
    0x00040000,
    0x00020000,
    0x00010000,
    0x00008000,
    0x00004000,
    0x00002000,
    0x00001000,
    0x00000800,
    0x00000400,
    0x00000200,
    0x00000100,
    0x00000080,
    0x00000040,
    0x00000020,
    0x00000010,
    0x00000008,
    0x00000004,
    0x00000002,
    0x00000001,
};

/**
 * The mask is simply bit mask
 * E.g. mask[0] = 10000000000000000000000000000000
 *      mask[1] = 01000000000000000000000000000000
 *      mask[2] = 00100000000000000000000000000000
 *      ...
 * @param  i 
 * @return   
 */
int get_mask(int i){
    if(i < 0 || i >= 32)
        return -EINVAL;
    return mask[i];
}

/**
 * clear the bitmap, setting all elements to 0
 * @param  map     
 * @param  map_len 
 * @return         
 */
int bitmap_clear(unsigned int *map, int map_len){
    int i;
    for (i=0; i < map_len; ++i)
    {
        map[i] = 0;
    }
    return 0;
}

/**
 * set all elements to 1
 * @param  map     
 * @param  map_len 
 * @return         
 */
int bitmap_fill(unsigned int *map, int map_len){
    int i;
    for (i=0; i < map_len; ++i)
    {
        map[i] = 0xffffffff;
    }
    return 0;
}
/**
 * search the number of 0 bits from the position specified
 * @param  map     memory map
 * @param  map_len memory map length
 * @param  start   starting bit to search from memory map, 0 <= start < map_len * 32
 * @param  num     number of bits to search
 * @return         bit found
 */
int bitmap_search_from(unsigned int *map, int map_len, int start, int num){
    int i,j;
    int count = 0;

    if(num >= map_len * 32 || start >= map_len * 32)
        return -EINVAL;

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
    return -EINVAL;
}

// int bitmap_search(unsigned int *map, int map_len, int num){
//     return bitmap_search_from(map,map_len, (BSS_END) / PAGE_LEN , num);
// }

/**
 * similar to bitmap_search_from, but this one starts from the reverse
 * @param  map     
 * @param  map_len 
 * @param  num     number of 0 bits to be found
 * @return         the starting index of the position found
 *                 or -1 if failed
 */
int bitmap_search_reverse(unsigned int *map, int map_len, int num){
    int i,j;
    int count = 0;

    if(num >= map_len * 32 )
        return -EINVAL;

    for (i = map_len -1; i >= 0; i--){
        for (j = BITMASK_NR -1; j >= 0; j--) {
            if ((map[i] & mask[j]) == 0) {
                count++;
                if (count == num) {
                    return (i*32 + j);
                }
            }else{
                count = 0;
            }
        }
    }
    return -EINVAL;
}

/**
 * set one bit in the bitmap to 1 at position specified
 * @param  map     
 * @param  map_len 
 * @param  start   bit to be set to 1
 * @return         
 */
int bitmap_set_bit(unsigned int *map, int map_len,int start){
    int ibit = start/32;
    if(start >= map_len * 32)    
        return -EINVAL;
    map[ibit] = map[ibit] | mask[start%32];
    return 0;
}

/**
 * Set multiple bits in the bitmap to 1, starting at position, with
 * len  
 * @param  map     
 * @param  map_len 
 * @param  start    starting position to be set to 1  
 * @param  len      number of bits to set to 1
 * @return         
 */
int bitmap_set_nbits(unsigned int *map, int map_len,int start, int len){
    int i;
    int inum;
    if(start + len >= map_len * 32)    
        return -EINVAL;
    for(i=0 ; i<len ; i++){
        inum = start + i;
        map[inum/32] = map[inum/32] | mask[inum%32];
    }
    return 0;
}

/**kprintf
 * @return         
 */
int bitmap_clear_nbits(unsigned int *map, int map_len,int start, int len){
    int i;
    int inum;
    if(start + len >= map_len * 32)    
        return -EINVAL;
    for(i=0 ; i<len ; i++){
        inum = start + i;
        map[inum/32] = map[inum/32] & (~mask[inum%32]);
    }
    return 0;
}

/**
 * similar to bitmap_clear_bit, but just set bit to 0
 * @param  map     
 * @param  map_len 
 * @param  start   
 * @return         
 */
int bitmap_clear_bit(unsigned int *map, int map_len,int start){
    int ibit = start/32;
    if(start >= map_len * 32)    
        return -EINVAL;
    map[ibit] = map[ibit] & (~mask[start%32]);
    return 0;
}

/**
 * test if the given position's bit is 1
 * @param  map     
 * @param  map_len 
 * @param  bit     position to be tested
 * @return         1 if the bit is 1, or 0 otherwise
 */
bool is_bit_on(unsigned int *map, int map_len, int bit){
    int i= bit/32, j=bit%32;
    if(bit > map_len * 32)
        return false;
    
    return (map[i] & mask[j]);
}

/**
 * count the number of bits, either 0, or 1, in the given
 * if ONE_BITS is set in flags, it returns the number of 1s
 * in the bitmap, vice versa.
 * bitmap
 * @param  map     
 * @param  map_len 
 * @param  flags   ONE_BITS or ZERO_BITS
 * @return         number of bits found
 */
int count_bits(unsigned int *map, int map_len, int flags){
    int i,j,curr_map_unit, count = 0;
    bool and_mask;

    if(flags == ONE_BITS)
        and_mask = true;
    else if(flags == ZERO_BITS)
        and_mask = false;
    else
        return -EINVAL;

    for(i = 0; i < map_len; i++){
        curr_map_unit = map[i];
        for(j = 0; j < 32; j++){
            if(and_mask && (mask[j] & curr_map_unit)){
                count++;
            }else if((!and_mask) && ((mask[j] & curr_map_unit) == 0)){
                count++;
            }
        }
    }
    return count;
}

/**
 * Xor the second bitmap to the first one
 * @param  map1    
 * @param  map2    
 * @param  map_len 
 * @return         
 */
int bitmap_xor(unsigned int *map1, unsigned int *map2, int map_len){
    int i=0;
    for(i = 0; i< map_len; i++){
        map1[i] ^= map2[i];
    }
    return 0;
}

/**
 * print the given bitmap
 * @param p   
 * @param len 
 */
void _kreport_bitmap(unsigned int *p, int len, int (*func) (const char *, ...) ){
    int i;
    for( i = 0; i < len; i++){
        func("0x%08x ",*p++);
        if((i+1) % 8 == 0)
            kprintf("\n");
    }
    func("\n");
}
