#include "fs.h"

static block_buffer_t buf_table[LRU_LEN];

static block_buffer_t *lru_cache[2];

static block_buffer_t imap;// inode map is assumed to be 1 block in length

static block_buffer_t bmap; // block map is also assumed to be 1 block in length

// The lru is illustrated as below
// REAR -> next -> .... -> next -> FRONT
// With the most recently used cache at the front, and least recently used block at the rear

block_buffer_t *get_imap(int num, struct device* id){
    struct superblock* sb = get_sb(id);
    block_buffer_t* buf = get_block_buffer(sb->s_inodemapnr, id);
    return buf;
}

block_buffer_t *get_inode_table(int num, struct device* id){
    struct superblock* sb = get_sb(id);
    block_t bnr = (num * sb->s_inode_size) / BLOCK_SIZE;
    if(bnr * BLOCK_SIZE > sb->s_inode_table_size){
        return NULL;
    }
    block_buffer_t* buf = get_block_buffer(sb->s_inode_tablenr + bnr, id);
    return buf;
}

block_buffer_t *get_bmap(struct device* id){
    struct superblock* sb = get_sb(id);
    block_buffer_t* buf = get_block_buffer(sb->s_blockmapnr, id);
    return buf;
}

void rm_lru(block_buffer_t *buffer){
    if(buffer->prev)
        buffer->prev->next = buffer->next;

    if(buffer->next)
        buffer->next->prev = buffer->prev;
        
    if(lru_cache[REAR] == buffer)
        lru_cache[REAR] = buffer->next;
    
    if(lru_cache[FRONT] == buffer)
        lru_cache[FRONT] = buffer->prev;
}

PRIVATE void buf_move_to_front(block_buffer_t *buffer){
    if(lru_cache[FRONT] == buffer)
        return;
    
    rm_lru(buffer);
    enqueue_buf(buffer);
}

block_buffer_t* dequeue_buf() {
    block_buffer_t *rear = lru_cache[REAR];
    if (!rear)
        return NULL;

    rear->next->prev = NULL;
    lru_cache[REAR] = rear->next;

    return rear;
}

void enqueue_buf(block_buffer_t *tbuf) {

    if (lru_cache[FRONT] == NULL) {
        tbuf = lru_cache[REAR] = lru_cache[FRONT] = &buf_table[0];
        tbuf->next = tbuf->prev = NULL;
        return;
    }

    lru_cache[FRONT]->next = tbuf;
    tbuf->prev = lru_cache[REAR];
    lru_cache[FRONT] = tbuf;

}

PRIVATE void buf_move_to_rear(block_buffer_t *buffer){
    if(lru_cache[REAR] == buffer)
        return;

    rm_lru(buffer);
    buffer->next = lru_cache[REAR];
    lru_cache[REAR] = buffer;
}

int flush_inode_zones(inode_t *ino){
    int i = 0;
    struct block_buffer* tbuf;
    for(i = 0; i < NR_TZONES; i++){
        if(ino->i_zone[i] > 0){
            tbuf = get_block_buffer(ino->i_zone[i], ino->i_dev);
            if(tbuf->b_dirt){
                put_block_buffer_immed(tbuf, ino->i_dev);
            }
        }
    }
    return OK;
}

int block_io(block_buffer_t* tbuf, struct device* dev, int flag){
    off_t off = tbuf->b_blocknr * BLOCK_SIZE;

    if(flag == READING){
        return dev->dev_read(tbuf->block, off, BLOCK_SIZE);
    }else if(flag == WRITING){
        return dev->dev_write(tbuf->block, off, BLOCK_SIZE);
    }
    return 0;
}

int put_block_buffer_immed(block_buffer_t* tbuf, struct device* dev){
    //XDEBUG(("Buffer %08x %d put immed\n",tbuf, tbuf->b_blocknr));
    enqueue_buf(tbuf);
    if(block_io(tbuf, dev, WRITING) == 0)
        return -1;
    tbuf->b_dirt = false;
    tbuf->b_count -= 1;
    if(tbuf->b_count <= 0){
        tbuf->b_count = 0;
    }
    return OK;
}

int put_block_buffer_dirt(block_buffer_t *tbuf) {
    tbuf->b_dirt = true;
    return put_block_buffer(tbuf);
}

int put_block_buffer(block_buffer_t *tbuf) {
    //XDEBUG(("Buffer %08x %d put\n",tbuf, tbuf->b_blocknr));

    enqueue_buf(tbuf);
    tbuf->b_count -= 1;
    if(tbuf->b_count <= 0){
        tbuf->b_count = 0;
    }
    return 0;
}

block_buffer_t *get_block_buffer(block_t blocknr, struct device* dev){
    block_buffer_t *tbuf;
    int ret;
    for(tbuf = &buf_table[0];tbuf< &buf_table[LRU_LEN];tbuf++){
        if(tbuf->b_blocknr == blocknr){
            rm_lru(tbuf);
            tbuf->b_count += 1;
            //XDEBUG(("Buffer %08x %d returned\n",tbuf, blocknr));
            return tbuf;
        }
    }

    // not in memory
    tbuf = dequeue_buf();
    if(tbuf && tbuf->b_dirt){
        ret = block_io(tbuf, dev, WRITING);
        tbuf->b_dirt = false;
        XDEBUG(("Sync block %d count %d before returning %d\n", tbuf->b_blocknr, tbuf->b_count, blocknr));
    }
    tbuf->b_blocknr = blocknr;
    if (block_io(tbuf, dev, READING) == 0) {
        XDEBUG(("ERR: flush block, dev io return %d\n", ret));
        enqueue_buf(tbuf);
        return NULL;
    }

    tbuf->b_blocknr = blocknr;
    tbuf->next = tbuf->prev = NULL;
    tbuf->b_dirt = 0;
    tbuf->b_dev = dev;
    tbuf->b_count = 1;
    enqueue_buf(tbuf);
    //XDEBUG(("Buffer %08x %d returned\n",tbuf, blocknr));
    return tbuf;
}

void init_buf(){
    int i=0;
    block_buffer_t *tbuf = NULL, *prevbuf = NULL;
    char *val;
    for(tbuf = &buf_table[0];tbuf< &buf_table[LRU_LEN];tbuf++){
        memset(tbuf, 0, sizeof(struct block_buffer));
        if(prevbuf == NULL){
            lru_cache[FRONT] = lru_cache[REAR] = tbuf;
            tbuf->next = tbuf->prev = NULL;
        }else{
            prevbuf->next = tbuf;
            tbuf->prev = prevbuf;
            lru_cache[FRONT] = tbuf;
        }
        prevbuf = tbuf;
    }

}


