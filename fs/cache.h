#ifndef _FS_LRU_H_
#define _FS_LRU_H_ 1

typedef struct buf
{
    char block[BLOCK_SIZE];
    struct buf *prev, *next;
    block_t b_blocknr; // block number for this buffer
    dev_t b_dev;            /* major | minor device where block resides */
    int b_dirt; // clean or dirty
    int b_count; // number of users on this buffer
} buf_t;


#define LRU_LEN         4
#define HASH_BUF_LEN    496

#define FRONT   1
#define REAR    0

/* When a block is released, the type of usage is passed to put_block(). */
#define WRITE_IMMED   1 /* block should be written to disk now */
#define ONE_SHOT      2 /* set if block not likely to be needed soon */


buf_t *get_imap();
int put_imap();
buf_t *get_bmap();
int put_bmap();
int put_block(buf_t *buffer, mode_t mode);
buf_t *get_block(block_t blocknr);
buf_t* dequeue_buf();
void enqueue_buf(buf_t *tbuf);
void init_buf();

#endif
