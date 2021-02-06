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

PRIVATE unsigned int mask[BITMASK_NR];

void init_bitmap(){
    int i;
    mask[0] = 0x80000000;
    for(i=1; i < BITMASK_NR; i++){
        mask[i] = mask[i-1] >> 1;
    }
}

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
        return ERR;
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
    return OK;
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
    return OK;
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
        return ERR;

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
    return ERR;
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
        return ERR;

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
    return ERR;
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
        return ERR;
    map[ibit] = map[ibit] | mask[start%32];
    return OK;
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
        return ERR;
    for(i=0 ; i<len ; i++){
        inum = start + i;
        map[inum/32] = map[inum/32] | mask[inum%32];
    }
    return OK;
}

/**kprintf
 * @return         
 */
int bitmap_clear_nbits(unsigned int *map, int map_len,int start, int len){
    int i;
    int inum;
    if(start + len >= map_len * 32)    
        return ERR;
    for(i=0 ; i<len ; i++){
        inum = start + i;
        map[inum/32] = map[inum/32] & (~mask[inum%32]);
    }
    return OK;
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
        return ERR;
    map[ibit] = map[ibit] & (~mask[start%32]);
    return OK;
}

// /**
//  * create a mask given a, and b. e.g.
//  * a = 0, b = 10
//  * then 11111111110000000000000000000000 is returned as the and_mask
//  * @param  a starting position of the mask
//  * @param  b number of bits to be masked
//  * @return   the resulting mask
//  */
// unsigned int createMask(unsigned int a, unsigned int b)
// {
//    unsigned int r = 0;
//    unsigned int i;
//    for (i=a; i<=b; i++)
//        r |= (0x80000000 >> i);

//    return r;
// }

// /**
//  * This method is obsolete
//  * extract the bitpattern given the bitmap, This method was initially used in 
//  * fork() syscall to copy the bit pattern of the parent, and then search the bit pattern
//  * in the system map. This was done on the assumption that process image are not continuous,
//  * However, Process image are now arranged to be continous, thus all bit pattern related 
//  * methods are obsolete. 
//  *
//  * E.g. bitmap: 0000001111101000000000000
//  * bit pattern extracted: 1111101
//  * bit pattern length:    7                     
//  * @param  map        
//  * @param  map_len    
//  * @param  heap_break the process image heap_break, which is the end of the process image
//  * @param  p          
//  * @return            
//  */
// int bitmap_extract_pattern(unsigned int *map, int map_len, int heap_break, struct bit_pattern *p){
//     int i,j,start = 0;
//     unsigned int result = 0;
//     int end = (align_page(heap_break) / 1024);
//     int endi = end/32;
//     unsigned int tmask;
//     for(i=0;i < map_len;i++){
//         for(j=0; j< 32; j++){
//             if((map[i] & mask[j]) == mask[j]){
//                 start = i*32 + j;
//                 // kprintf(" i %d j %d\n",i,j);
//                 goto then;
//             }
//         }
//     }
    
//     then:
//     if(end - start > 32 || (i==31 && j==31)){
//         return ERR;
//     }
//     result = map[i] << (j);
//     if(i < map_len && i != endi){
//         tmask = createMask(0,end%32);
//         result |= (map[i+1] & tmask) >> (32 - j);
//     }
//     // kprintf(" start %d end %d\n",start, end);
//     p->pattern = result;
//     p->size = end - start +1;
//     return OK;
// }

// /**
//  * backtrace search of the bit_pattern, this methods searches the given
//  * bitmap, see if the bit_pattern could fit at position specified by j
//  * For instance, 
//  * bitmap : 101011100000000000000000000000000
//  * region_len: 1
//  * pattern: 1010001
//  * pattern_len: 7
//  * j : 0
//  * if we do an XOR of the bit pattern and bitmap at all positions, 
//  * we can find that both fits together at index = 1. so here we found
//  * a match at position 1
//  * @param  map         
//  * @param  region_len  number of bitmap elements to be searched
//  * @param  pattern     bit pattern
//  * @param  pattern_len bit pattern len
//  * @param  j           starting position to be searched at
//  * @return             OK on success, ERR on failure
//  */
// int search_backtrace(unsigned int *map, int region_len,unsigned int pattern, int pattern_len,int j){
//     int i=0;
//     int count = 1;
//     int j_bak = j;
//     unsigned int map_bit, pattern_bit;
//     int result;
//     unsigned int curr_pattern = pattern >> (j-1);
//     for(;i<region_len;i++){
//         for(;j<32;j++){
//             map_bit = map[i] & mask[j];
//             pattern_bit = curr_pattern & mask[j];
//             result = map_bit & pattern_bit;
//             // kprintf(" %d |",j);
//             if(result == 0){
//                 count++;
//                 if(count == pattern_len){
//                     return ERR;
//                 }
//             }else{
//                 return OK;
//             }
//         }
//         j=0; 
//         curr_pattern = pattern << count;
//     }
//     return OK;
// }

// /**
//  * search the bit_pattern in the given bitmap, it does a check of the first bit
//  * of bitpattern and bitmap position, if it matches, then backtrace searching is used
//  * to see if the bit pattern can match at current position, if not, the next position is 
//  * tried
//  * @param  map         
//  * @param  map_len     
//  * @param  pattern     bit pattern
//  * @param  pattern_len bit pattern length
//  * @return             OK on success, ERR on failure
//  */
// int bitmap_search_pattern(unsigned int *map, int map_len, unsigned int pattern, int pattern_len){
//     int i=0, j = 0;
//     unsigned int map_bit, pattern_bit;
//     int result;
//     for(i=0;i<map_len;i++){
//         for(j=0;j<32;j++){
//             map_bit = (map[i] & mask[j]);
//             pattern_bit = (pattern >> j) & mask[j];
//             result = map_bit & pattern_bit;
//             // kprintf("%d 0x%08x 0x%08x 0x%08x 0x%08x\n",j,(map[i]),map_bit, pattern_bit ,result);
//             if(result == 0){ 
//                 if(pattern_len == 1)
//                     return i*32+j;
//                 if(search_backtrace(map+i, map_len - i, pattern,pattern_len, j+1)){
//                     return i*32 + j;
//                 }
//             }
//         }
//     }
//     return OK;
// }


/**
 * set the bitmap pattern on the given bitmap
 * @param  map         
 * @param  map_len     
 * @param  index       starting position to be put
 * @param  pattern     bit pattern
 * @param  pattern_len bit pattern len
 * @return             OK
 */
int bitmap_set_pattern(unsigned int *map, int map_len, int index, unsigned int pattern, int pattern_len){
    int i= index/32, j=index%32;
    map[i] |= (pattern >> j);
    if(i < map_len -1 && 32 - j < pattern_len){
        map[i+1] |= (pattern << (32 - j));
    }
    return OK;
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
        return ERR;

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
    return OK;
}

/**
 * print the given bitmap
 * @param p   
 * @param len 
 */
void kreport_bitmap(unsigned int *p, int len){
    int i;
    for( i = 0; i < len; i++){
        kprintf("0x%08x ",*p++);
        if((i+1) % 8 == 0)
            kprintf("\n");
    }
    kprintf("\n");
}
