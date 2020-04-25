#ifndef _FS_LRU_H_
#define _FS_LRU_H_ 1

typedef struct block_buffer
{
    disk_word_t block[BLOCK_SIZE];
    struct block_buffer *prev, *next;
    block_t b_blocknr; // block number for this buffer
    struct device* b_dev;            /* major | minor device where block resides */
    int b_dirt; // clean or dirty
    int b_count; // number of users on this buffer
} block_buffer_t;


#define LRU_LEN         4
#define HASH_BUF_LEN    496

#define FRONT   1
#define REAR    0

/* When a block is released, the type of usage is passed to put_block_buffer(). */
#define WRITE_IMMED   1 /* block should be written to DISK_RAW now */
#define ONE_SHOT      2 /* set if block not likely to be needed soon */


block_buffer_t *get_imap(int, struct device* id);
block_buffer_t *get_bmap(struct device* id);
block_buffer_t *get_inode_table(int num, struct device* id);
int put_block_buffer(block_buffer_t *tbuf);
int put_block_buffer_immed(block_buffer_t *tbuf, struct device* id);
block_buffer_t *get_block_buffer(block_t blocknr, struct device* id);
int put_block_buffer_dirt(block_buffer_t *tbuf);
block_buffer_t* dequeue_buf();
void enqueue_buf(block_buffer_t *tbuf);
void init_buf();
int flush_inode_zones(inode_t *ino);
int block_io(block_buffer_t* tbuf, struct device* dev, int flag);

#endif
