#include "fs.h"

static buf_t* hash_buf[HASH_BUF_LEN];

static buf_t buf_table[LRU_LEN];

static buf_t *lru_cache[2];


int put_block(buf_t *buffer){
    return dev_io(buffer->block,buffer->b_blocknr,DEV_WRITE);
}

buf_t *get_block(block_t blocknr){
    register buf_t *tbuf;
    int ret;
    if(hash_buf[blocknr] != NULL){
        tbuf = hash_buf[blocknr];
        buf_move_to_front(tbuf);
        return tbuf;
    }

    //not in memory
    tbuf = dequeue_buf();
    if(tbuf && tbuf->b_dirt)
        put_block(tbuf);

    tbuf->b_blocknr = blocknr;
    tbuf->next = tbuf->prev = NULL;
    tbuf->b_dirt = 0;
    tbuf->b_count = 0;

    ret = dev_io(tbuf->block,blocknr,DEV_READ);

    enqueue_buf(tbuf);
}


static void buf_move_to_front(buf_t *buffer){
    if(buffer->prev)
        buffer->prev->next = buffer->next;

    if(buffer->next)
        buffer->next->prev = buffer->prev;
}

buf_t* dequeue_buf(){
    buf_t *rear = lru_cache[REAR];
    if(!rear)
        return NULL;

    rear->next->prev = NULL;
    lru_cache[REAR] = rear->next;

    hash_buf[rear->b_blocknr] = NULL;
    
    return rear;
}

void enqueue_buf(register buf_t *tbuf){
    
    if(lru_cache[FRONT] == NULL){
        tbuf = lru_cache[REAR] = lru_cache[FRONT] = &buf_table[0];
        tbuf->next = tbuf->prev = NULL;
        return;
    }

    // tbuf = dequeue_buf();
    // if(tbuf && tbuf->b_dirt)
    //     // put_block(tbuf);

    // tbuf->b_dirt = 0;
    // tbuf->b_count = 0;
    // tbuf->b_blocknr = blocknr;
    // tbuf->next = tbuf->prev = NULL;

    lru_cache[FRONT]->next = tbuf;
    tbuf->prev = lru_cache[REAR];
    lru_cache[FRONT] = tbuf;

    hash_buf[(tbuf->b_blocknr)] = tbuf;
}

void init_buf(){
    int i=1;
    register buf_t *tbuf = NULL, *prevbuf = NULL;
    for(;i<LRU_LEN;i++){
        tbuf = &buf_table[i];
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

    for(i=0;i<HASH_BUF_LEN;i++){
        hash_buf[i] = NULL;
    }
    
}

void init_fs(){

}


