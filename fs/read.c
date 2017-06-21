#include "fs.h"

int rw_file(int fd, char *buf, size_t count, int flag){
    // char *pbuf = get_physical_addr(buf);
    int ret, r;
    int open_slot, pos;
    filp_t *filp;
    int b,off, len;
    buf_t *buffer;
    block_t blocknr, endblocknr;
    inode_t *ino;
    char c;
    int j;

    filp = current_proc->fp_filp[fd];

    blocknr = filp->filp_pos / BLOCK_SIZE;
    off = filp->filp_pos % BLOCK_SIZE;
    endblocknr = (filp->filp_pos + count) / BLOCK_SIZE;
    

    for( ; blocknr <= endblocknr; blocknr++){
        buffer = get_block(blocknr);
        len = BLOCK_SIZE - off > count ? count : BLOCK_SIZE - off;
        c = buffer->block[off];
        /* Read or write 'chunk' bytes. */
		// r = rw_chunk(ino,  off, blocknr, endblocknr- blocknr, falgs);
        if(flag & READING){
            for( j=off; j< off+len; j++,buf++){
                c = buffer->block[j];
                *buf = buffer->block[j];
            }
        }else{
            for( j=off; j< off+len; j++,buf++){
                buffer->block[j] = *buf;
            }
        }
        filp->filp_pos = off + len;
//		if (r != OK) break;	/* EOF reached */
        off = 0;
    }
    return 0;
}

int sys_read(int fd, void *buf, int count){
    rw_file(fd, buf,count, READING);
}

int sys_write(int fd, void *buf, int count){
    rw_file(fd, buf, count, WRITING);
}


