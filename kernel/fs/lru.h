#ifndef _FS_LRU_H_
#define _FS_LRU_H_

#include "const.h"

typedef struct _buf
{
    
    unsigned int block[BLOCK_SIZE];

    struct _buf *prev, *next;
    block_t b_blocknr; //block number for this buffer
    int b_dirt; //clean or dirty
    int b_count; //number of users on this buffer
} buf_t;

#define LRU_LEN         4
#define HASH_BUF_LEN    496

#define FRONT   1
#define REAR    0

#endif
