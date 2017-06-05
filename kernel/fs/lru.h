#ifndef _FS_LRU_H_
#define _FS_LRU_H_

#include <type.h>

#define SECTOR_SIZE 512
#define BLOCK_SIZE  1024

typedef struct _buf
{
    
    char block[BLOCK_SIZE];

    struct _buf *prev, *next;
    size_t b_blocknr; //block number for this buffer
    char b_dirt; //clean or dirty
    char b_count; //number of users on this buffer
} buf_t;

#define LRU_LEN         4
#define HASH_BUF_LEN    496

#define FRONT   1
#define REAR    0

#endif
