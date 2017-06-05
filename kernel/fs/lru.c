#include "fs.h"

static buf_t* hash_buf[HASH_BUF_LEN];

static buf_t buf_table[LRU_LEN];

static buf_t *lru_cache[2];

void init_fs(){

}

buf_t* dequeue_buf(){
    buf_t *rear = lru_cache[REAR];
    if(!rear)
        return NULL;

    rear->next->prev = NULL;
    lru_cache[REAR] = rear->next;
    return rear;
}

void enqueue_buf(size_t blocknr){
    register buf_t *tbuf;
    
    if(lru_cache[REAR] == NULL){
        tbuf = lru_cache[REAR] = lru_cache[FRONT] = &buf_table[0];
        tbuf->b_blocknr = blocknr;
        tbuf->next = tbuf->prev = NULL;
        return;
    }

    tbuf = dequeue_buf();
    if(tbuf && tbuf->b_dirt)
        // put_block(tbuf);

    tbuf->b_dirt = 0;
    tbuf->b_count = 0;
    tbuf->b_blocknr = blocknr;
    tbuf->next = tbuf->prev = NULL;

    lru_cache[FRONT]->next = tbuf;
    tbuf->prev = lru_cache[REAR];
    lru_cache[FRONT] = tbuf;

    hash_buf[blocknr] = tbuf;
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
    
}


