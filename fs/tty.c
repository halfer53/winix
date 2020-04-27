//
// Created by bruce on 25/04/20.
//

#include "fs.h"

struct device tty_dev;
static struct filp_operations fops;
static struct device_operations dops;
static char* name = "tty";

int tty_read ( struct filp *filp, char *data, size_t count, off_t offset){
    return 0;
}

int tty_write ( struct filp *filp, char *data, size_t count, off_t offset){
    return 0;
}

int tty_open ( struct inode* ino, struct filp *file){
    return 0;
}

int tty_close ( struct inode* ino, struct filp *file){
    return 0;
}


int tty_dev_io_read(disk_word_t *buf, off_t off, size_t len){
    return 0;
}

int tty_dev_io_write(disk_word_t *buf, off_t off, size_t len){
    return 0;
}

int tty_dev_init(){
    return 0;
}

int tty_dev_destroy(){
    return 0;
}


void init_tty(){
    dops.dev_init = tty_dev_init;
    dops.dev_read = tty_dev_io_read;
    dops.dev_write = tty_dev_io_write;
    dops.dev_destroy = tty_dev_destroy;
    fops.open = tty_open;
    fops.read = tty_read;
    fops.write = tty_write;
    fops.close = tty_close;
    register_device(&tty_dev, name, MAKEDEV(3, 1), &dops, &fops);
}
