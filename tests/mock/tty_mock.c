//
// Created by bruce on 25/04/20.
//

#include <fs/fs.h>
#include "../unit_test.h"

struct device tty_dev_static;
static struct filp_operations fops;
static struct device_operations dops;
static const char* name = "tty";

bool TTY_OPEN_CALLED = false;

int tty_read ( struct filp *filp, char *data, size_t count, off_t offset){
    return TTY_RETURN;
}

int tty_write ( struct filp *filp, char *data, size_t count, off_t offset){
    return TTY_RETURN;
}

int tty_open ( struct device* dev, struct filp *file){
    TTY_OPEN_CALLED = true;
    return 0;
}

int tty_close ( struct device* dev, struct filp *file){
    return TTY_RETURN;
}


int tty_dev_io_read(char *buf, off_t off, size_t len){
    return 0;
}

int tty_dev_io_write(char *buf, off_t off, size_t len){
    return 0;
}

int tty_dev_init(){
    return 0;
}

int tty_dev_release(){
    return 0;
}


void init_tty(){
    dops.dev_init = tty_dev_init;
    dops.dev_read = tty_dev_io_read;
    dops.dev_write = tty_dev_io_write;
    dops.dev_release = tty_dev_release;
    fops.open = tty_open;
    fops.read = tty_read;
    fops.write = tty_write;
    fops.close = tty_close;
    register_device(&tty_dev_static, name, TTY1_DEV_NUM, S_IFCHR, &dops, &fops);
}
