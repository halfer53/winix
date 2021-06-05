#include "fs.h"

static struct block_buffer buf_table[LRU_LEN];

static struct block_buffer *lru_cache[2];
// The lru is illustrated as below
// REAR -> next -> .... -> next -> FRONT
// With the most recently used cache at the front, and least recently used block at the rear

#define TBUF_NR(tbr)    (tbr - buf_table)

void visualise_lru(){
    int limit = LRU_LEN;
    struct block_buffer* buf = lru_cache[FRONT];
    while(buf != NULL && limit > 0){
        unsigned int val = TBUF_NR(buf);
        kprintf("%ld -> ", val);
        buf = buf->prev;
        limit--;
    }
    kprintf("\n");
}


struct block_buffer *get_imap(struct device* id){
    struct superblock* sb = get_sb(id);
    struct block_buffer* buf = get_block_buffer(sb->s_inodemapnr, id);
    return buf;
}

struct block_buffer *get_inode_table(int num, struct device* id){
    struct superblock* sb = get_sb(id);
    struct block_buffer* buf;
    block_t bnr = (num * sb->s_inode_size) / BLOCK_SIZE;
    if(bnr * BLOCK_SIZE > sb->s_inode_table_size){
        return NULL;
    }
    buf = get_block_buffer(sb->s_inode_tablenr + bnr, id);
    return buf;
}

struct block_buffer *get_bmap(struct device* id){
    struct superblock* sb = get_sb(id);
    struct block_buffer* buf = get_block_buffer(sb->s_blockmapnr, id);
    return buf;
}

void rm_lru(struct block_buffer *buffer){
    if(buffer->prev)
        buffer->prev->next = buffer->next;

    if(buffer->next)
        buffer->next->prev = buffer->prev;
        
    if(lru_cache[REAR] == buffer)
        lru_cache[REAR] = buffer->next;
    
    if(lru_cache[FRONT] == buffer)
        lru_cache[FRONT] = buffer->prev;
}

struct block_buffer* dequeue_buf() {
    struct block_buffer *rear = lru_cache[REAR];
    if (!rear)
        return NULL;
    if(rear->next)
        rear->next->prev = NULL;
    lru_cache[REAR] = rear->next;
    rear->next = NULL;
    rear->prev = NULL;
    return rear;
}

void enqueue_buf(struct block_buffer *tbuf) {
    struct block_buffer *front = lru_cache[FRONT];
    tbuf->next = NULL;
    tbuf->prev = front;
    front->next = tbuf;
    lru_cache[FRONT] = tbuf;
}

int flush_all_buffer(){
    int j, ret;
    struct block_buffer* tbuf;
    for(j = 0; j < LRU_LEN; j++){
        tbuf = &buf_table[j];
        if(tbuf->b_dirt){
            tbuf->b_dev->bops->flush_block(tbuf);
            tbuf->b_dirt = false;
        }
    }
    return OK;
}

int flush_inode_zones(struct inode *ino){
    int i, j;
    block_t zid;
    struct block_buffer* tbuf;
    for(i = 0; i < NR_TZONES; i++){
        zid = ino->i_zone[i];
        if(zid > 0){
            for(j = 0; j < LRU_LEN; j++){
                tbuf = &buf_table[j];
                if(tbuf->b_blocknr == zid && tbuf->b_dirt){
                    tbuf->b_dev->bops->flush_block(tbuf);
                    tbuf->b_dirt = false;
                }
            }
        }
    }
    return OK;
}

int put_block_buffer_immed(struct block_buffer* tbuf, struct device* dev){
    if(tbuf->b_dev->bops->flush_block(tbuf) == 0)
        return EIO;
    tbuf->b_dirt = false;
    return put_block_buffer(tbuf);
}

int put_block_buffer_dirt(struct block_buffer *tbuf) {
    tbuf->b_dirt = true;
    return put_block_buffer(tbuf);
}

int put_block_buffer(struct block_buffer *tbuf) {
//    KDEBUG(("Buffer %d is dirty %d put\n", tbuf->b_blocknr, tbuf->b_dirt));

    enqueue_buf(tbuf);
    tbuf->b_count -= 1;
    if(tbuf->b_count <= 0){
        tbuf->b_count = 0;
    }
//    visualise_lru();
    return OK;
}

struct block_buffer *get_block_buffer(block_t blocknr, struct device* dev){
    struct block_buffer *tbuf;
    int ret;
    int i;
    for(i = 0; i < LRU_LEN; i++){
        tbuf = &buf_table[i];
        if(tbuf->b_blocknr == blocknr){
            rm_lru(tbuf);
            tbuf->b_count += 1;
//            KDEBUG(("Buffer %d cache returned\n", blocknr));
            return tbuf;
        }
    }

    // not in memory
    tbuf = dequeue_buf();
    if(!tbuf){
        return NULL;
    }

    if(tbuf->b_dirt){
        ret = tbuf->b_dev->bops->flush_block(tbuf);
        tbuf->b_dirt = false;
        // KDEBUG(("Sync block %d count %d before returning %d\n", tbuf->b_blocknr, tbuf->b_count, blocknr));
    }

    
    if(tbuf->b_dev && tbuf->b_dev != dev){
        tbuf->b_dev->bops->release_block(tbuf);
        tbuf->initialised = false;
    }

    if(!tbuf->initialised){
        dev->bops->init_block(tbuf);
        tbuf->initialised = true;
    }

    ret = dev->bops->retrieve_block(tbuf, dev, blocknr);
    // printf("ret blk %d %d\n", blocknr, ret);

    if (ret != BLOCK_SIZE) {
        // KDEBUG(("dev io return %d for %d\n", ret, tbuf->b_blocknr));
        dev->bops->release_block(tbuf);
        tbuf->initialised = false;
        enqueue_buf(tbuf);
        return NULL;
    }

    tbuf->b_blocknr = blocknr;
    tbuf->b_dev = dev;
    tbuf->next = tbuf->prev = NULL;
    tbuf->b_dirt = false;
    tbuf->b_count = 1;
    return tbuf;
}

void flush_super_block(struct device* dev){
//    struct superblock sb1, sb2;
    struct superblock* sb = get_sb(dev);
    dearch_superblock(sb);
    dev->dops->dev_write((char*)sb, 0, sizeof(struct superblock));

//    dev->dops->dev_read((char*)&sb1, 0, sizeof(struct superblock));
//    memcpy(&sb2, DISK_RAW, sizeof(struct superblock));
//    KDEBUG(("sb %d %d %d \n", sb1.s_inode_per_block, sb2.s_inode_per_block, sb->s_inode_per_block));
}

void init_buf(){
    int i=0;
    struct block_buffer *tbuf = NULL, *prevbuf = NULL;
    char *val;
    for(i = 0; i < LRU_LEN; i++){
        tbuf = &buf_table[i];
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


