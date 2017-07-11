#include "fs.h"

int rw_chunk(inode_t *ino, int off, int len, int curr_fp_index, char *buf, int flag) {
	int j;
	buf_t *buffer = get_block(curr_fp_index);
	char c;
	if (flag & READING) {
		for (j = off; j< off + len; j++) {
			*buf++ = buffer->block[j];
		}
	}
	else {
		for (j = off; j< off + len; j++) {
			buffer->block[j] = *buf++;
		}
	}
}

int rw_file(filp_t *filp, char *buf, size_t count, int flag){
    // char *pbuf = get_physical_addr(buf);
    int ret, r;
    int open_slot, pos;
    
    int b,off, len;
    buf_t *buffer;
    block_t curr_fp_index, curr_fp_index;
    inode_t *ino = NULL;
    char c;
    int j;

    curr_fp_index = filp->filp_pos / BLOCK_SIZE;
    off = filp->filp_pos % BLOCK_SIZE;
    curr_fp_index = (filp->filp_pos + count - 1) / BLOCK_SIZE;
	ino = filp->filp_ino;

    for( ; curr_fp_index <= curr_fp_index; curr_fp_index++){
        len = BLOCK_SIZE - off > count ? count : BLOCK_SIZE - off;

        /* Read or write 'chunk' bytes. */
		r = rw_chunk(ino,  off, len, curr_fp_index, buf, flag);
		if (r != OK) 
			break;	/* EOF reached */

		count -= len;
        filp->filp_pos = off + len;

        off = 0;
    }
    return OK;
}

int sys_read(proc_t *who,int fd, void *buf, int count){
    rw_file(who->fp_filp[fd], buf,count, READING);
}

int sys_write(proc_t *who,int fd, void *buf, int count){
    rw_file(who->fp_filp[fd], buf, count, WRITING);
}


