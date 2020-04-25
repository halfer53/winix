//
// Created by bruce on 25/04/20.
//

#include "fs.h"

struct device pipe_dev;
static struct filp_operations fops;
static struct device_operations dops;
static char* name = "PIPE";

int pipe_read (struct filp *filp, char *data, size_t count, off_t offset){
    return 0;
}

int pipe_write (struct filp *filp, const char *data, size_t count, off_t offset){
    return 0;
}

int pipe_open (struct inode* ino, struct filp *file){
    return 0;
}

int pipe_close (struct inode* ino, struct filp *file){
    return 0;
}


int pipe_dev_io_read(disk_word_t *buf, off_t off, size_t len){
    return 0;
}

int pipe_dev_io_write(disk_word_t *buf, off_t off, size_t len){
    return 0;
}

int pipe_dev_init(){
    return 0;
}

int pipe_dev_destroy(){
    return 0;
}


void init_pipe(){
    dops.dev_init = pipe_dev_init;
    dops.dev_read = pipe_dev_io_read;
    dops.dev_write = pipe_dev_io_write;
    dops.dev_destroy = pipe_dev_destroy;
    fops.open = pipe_open;
    fops.read = pipe_read;
    fops.write = pipe_write;
    fops.close = pipe_close;
    register_device(&pipe_dev, name, MAKEDEV(2, 1), &dops, &fops);
}
