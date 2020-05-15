//
// Created by bruce on 25/04/20.
//
#include <sys/fcntl.h>
#include <fs/const.h>
#include <fs/cache.h>
#include <fs/filp.h>
#include <fs/dev.h>
#include <fs/fs_methods.h>
#include <winix/sys_stdio.h>
#include <winix/mm.h>

struct device tty_dev;
static struct filp_operations fops;
static struct device_operations dops;
static char* name = "tty";

int tty_read ( struct filp *filp, char *data, size_t count, off_t offset){
    char ret = kgetc(curr_user);
    *data = ret;
    // kdebug("read %d, usr addr %p, phy %p\n", ret, data, get_physical_addr(data, curr_user));
    return 1;
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

int tty_dev_init(){
    return 0;
}

int tty_dev_release(){
    return 0;
}


void init_tty(){
    dops.dev_init = tty_dev_init;
    dops.dev_release = tty_dev_release;
    fops.open = tty_open;
    fops.read = tty_read;
    fops.write = tty_write;
    fops.close = tty_close;
    tty_dev.dops = &dops;
    tty_dev.fops = &fops;
    register_device(&tty_dev, name, TTY_DEV, S_IFCHR);
}
