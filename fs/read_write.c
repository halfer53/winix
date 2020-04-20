#include "fs.h"

int rw_chunk(inode_t *ino, int off, int len, int curr_fp_index, char *buf, int flag) {
    int j;
    block_buffer_t *buffer = get_block_buffer(curr_fp_index, 1);
    char c;
    if (flag & READING) {
        for (j = off; j< off + len; j++) {
            *buf++ = buffer->block[j];
        }
        XDEBUG(("read for block %d, ptr %08x\n", curr_fp_index, buffer));
        put_block_buffer(buffer);
    }
    else {
        for (j = off; j< off + len; j++) {
            buffer->block[j] = *buf++;
        }
        XDEBUG(("write for block %d, ptr %08x\n", curr_fp_index, buffer));
        put_block_buffer_dirt(buffer);
    }
}

int rw_file(filp_t *filp, char *buf, size_t count, int flag){
    // char *pbuf = get_physical_addr(block_buffer);
    int ret, r;
    int open_slot, pos;
    
    int b,off, len;
    block_buffer_t *buffer;
    int curr_fp_index, fp_limit;
    block_t bnr;
    inode_t *ino = NULL;
    char c;
    int j;

    curr_fp_index = filp->filp_pos / BLOCK_SIZE;
    off = filp->filp_pos % BLOCK_SIZE;
    fp_limit = (filp->filp_pos + count ) / BLOCK_SIZE;
    ino = filp->filp_ino;

    for( ; curr_fp_index <= fp_limit; curr_fp_index++){
        len = BLOCK_SIZE - off > count ? count : BLOCK_SIZE - off;
        bnr = filp->filp_ino->i_zone[curr_fp_index];
        if(bnr == 0){
            break;
        }

        /* Read or write 'chunk' bytes. */
        r = rw_chunk(ino,  off, len, bnr, buf, flag);
        if (r != OK) 
            break;    /* EOF reached */

        count -= len;
        filp->filp_pos = off + len;
        off = 0;
    }
    return OK;
}

int sys_read(struct proc *who,int fd, void *buf, size_t count){
    if(!is_fd_opened_and_valid(who, fd))
        return EINVAL;
    rw_file(who->fp_filp[fd], buf,count, READING);
}

int sys_write(struct proc *who,int fd, void *buf, size_t count){
    if(!is_fd_opened_and_valid(who, fd))
        return EINVAL;
    rw_file(who->fp_filp[fd], buf, count, WRITING);
}


