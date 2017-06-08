#ifndef _FS_LRU_H_
#define _FS_LRU_H_

typedef struct _buf
{
    
    char block[BLOCK_SIZE];

    struct _buf *prev, *next;
    block_t b_blocknr; //block number for this buffer
    int b_dirt; //clean or dirty
    int b_count; //number of users on this buffer
} buf_t;

void enqueue_buf(register buf_t *tbuf);
void init_buf();
buf_t* dequeue_buf();
static void buf_move_to_front(buf_t *buffer);

#define LRU_LEN         4
#define HASH_BUF_LEN    496

#define FRONT   1
#define REAR    0

/* When a block is released, the type of usage is passed to put_block(). */
#define WRITE_IMMED   1 /* block should be written to disk now */
#define ONE_SHOT      2 /* set if block not likely to be needed soon */

#endif
