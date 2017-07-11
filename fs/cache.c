#include "fs.h"

static buf_t* hash_buf[HASH_BUF_LEN];

static buf_t buf_table[LRU_LEN];

static buf_t *lru_cache[2];

static buf_t imap;//inode map is assumed to be 1 block in length

static buf_t bmap; //block map is also assumed to be 1 block in length

// The lru is illustrated as below
// REAR -> next -> .... -> next -> FRONT
// With the most recently used cache at the front, and least recently used block at the rear

buf_t *get_imap(){
    return &imap;
}

buf_t *get_bmap(){
    return &bmap;
}

void rm_lru(buf_t *buffer){
    if(buffer->prev)
        buffer->prev->next = buffer->next;

    if(buffer->next)
        buffer->next->prev = buffer->prev;
        
    if(lru_cache[REAR] == buffer)
        lru_cache[REAR] = buffer->next;
    
    if(lru_cache[FRONT] == buffer)
        lru_cache[FRONT] = buffer->prev;
}

PRIVATE void buf_move_to_front(buf_t *buffer){
    if(lru_cache[FRONT] == buffer)
        return;
    
	rm_lru(buffer);
    enqueue_buf(buffer);
}

buf_t* dequeue_buf() {
	buf_t *rear = lru_cache[REAR];
	if (!rear)
		return NULL;

	rear->next->prev = NULL;
	lru_cache[REAR] = rear->next;

	hash_buf[rear->b_blocknr] = NULL;

	return rear;
}

void enqueue_buf(register buf_t *tbuf) {

	if (lru_cache[FRONT] == NULL) {
		tbuf = lru_cache[REAR] = lru_cache[FRONT] = &buf_table[0];
		tbuf->next = tbuf->prev = NULL;
		return;
	}

	lru_cache[FRONT]->next = tbuf;
	tbuf->prev = lru_cache[REAR];
	lru_cache[FRONT] = tbuf;

	hash_buf[(tbuf->b_blocknr)] = tbuf;
}

PRIVATE void buf_move_to_rear(buf_t *buffer){
    if(lru_cache[REAR] == buffer)
        return;

	rm_lru(buffer);
    buffer->next = lru_cache[REAR];
    lru_cache[REAR] = buffer;
}


int put_block(buf_t *tbuf, mode_t mode) {
	if (mode & WRITE_IMMED && tbuf->b_dirt) {
		tbuf->b_dirt = 0;
		buf_move_to_front(tbuf);
		return dev_io(tbuf->block, tbuf->b_blocknr, DEV_WRITE);
	}
	else { //mode = ONE_SHOT
		buf_move_to_rear(tbuf);
	}
}

buf_t *get_block(block_t blocknr){
    register buf_t *tbuf;
    int ret;
    if(hash_buf[blocknr] != NULL){
        tbuf = hash_buf[blocknr];
        // tbuf->b_count += 1;
        buf_move_to_front(tbuf);
        return tbuf;
    }

    //not in memory
    tbuf = dequeue_buf();
    if(tbuf && tbuf->b_dirt)
        put_block(tbuf,WRITE_IMMED);

    tbuf->b_blocknr = blocknr;
    tbuf->next = tbuf->prev = NULL;
    tbuf->b_dirt = 0;
    tbuf->b_count = 1;

	if (!dev_io(tbuf->block, blocknr, DEV_READ)) {
		return NULL;
	}
		

    enqueue_buf(tbuf);
    return tbuf;
}


int put_imap(){
//    return dev_io(imap.block,imap.b_blocknr,DEV_WRITE);
}

int put_bmap(){
//    return dev_io(bmap.block,bmap.b_blocknr,DEV_WRITE);
}

void init_buf(){
    int i=0;
    register buf_t *tbuf = NULL, *prevbuf = NULL;
    register char *val;
    for(tbuf = &buf_table[0];tbuf< &buf_table[LRU_LEN];tbuf++){
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
    imap.b_blocknr = sb->s_inodemapnr;
    imap.b_dirt = 0;
    dev_io(imap.block,imap.b_blocknr,DEV_READ);
    for(val = &imap.block[0]; val < &imap.block[BLOCK_SIZE]; val++){
        *val = hexstr2char(*val);
    }
    // printf("imap %d\n",imap.block[0]);

    bmap.b_blocknr = sb->s_blockmapnr;
    bmap.b_dirt = 0;
    dev_io(bmap.block,bmap.b_blocknr,DEV_READ);
    for(val = &bmap.block[0]; val < &bmap.block[BLOCK_SIZE]; val++){
        *val = hexstr2char(*val);
    }
}


