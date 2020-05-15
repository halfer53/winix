//
// Created by bruce on 25/04/20.
//
#include <sys/fcntl.h>
#include <fs/const.h>
#include <fs/cache.h>
#include <fs/filp.h>
#include <fs/dev.h>
#include <fs/fs_methods.h>
#include <kernel/kernel.h>
#include <winix/rex.h>
#include <winix/list.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZ  (128)
struct device tty_dev;
static struct filp_operations fops;
static struct device_operations dops;
static char* name = "tty";
static char buffer[BUFFER_SIZ];
char *bptr, *buffer_end;
struct proc* reader = NULL;
char *read_data;
size_t read_count;


int tty_read ( struct filp *filp, char *data, size_t count, off_t offset){
    if(reader){
        kerror("race condition, two proc are reading tty %d %d\n", reader->proc_nr, curr_user->proc_nr);
        return EIO;
    }
    read_data = data;
    read_count = count;
    reader = curr_user;
    return SUSPEND;
    // char ret = kgetc(curr_user);
    // *data = ret;
    // // kdebug("read %d, usr addr %p, phy %p\n", ret, data, get_physical_addr(data, curr_user));
    // return 1;
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

void tty1_handler(){
    int val, stat;
    struct message* msg;
    stat = RexSp1->Stat;
    
    if(stat & 1){
        val = RexSp1->Rx;

        if (val == 8) { // backspace
            if(bptr > buffer){
                bptr--;
                kputc(val);
                goto end;
            }
        }

        if(bptr < buffer_end){
            if(val == '\r')
                val = '\n';

            if(isprint(val) || val == '\n'){
                
                *bptr++ = val;
                kputc(val);
            }else{
                kputc(7);            // beep
            }
        }

        if((val == '\n' || bptr >= buffer_end ) && reader){
            *bptr = '\0';
            msg = get_exception_m();
            strncpy(read_data, buffer, read_count);
            syscall_reply(bptr - buffer, reader->proc_nr, msg);
            bptr = buffer;
            reader = NULL;
        }
    }
    end:
    RexSp1->Iack = 0;
}

void init_tty(){
    bptr = buffer;
    buffer_end = bptr + BUFFER_SIZ - 1;
    dops.dev_init = tty_dev_init;
    dops.dev_release = tty_dev_release;
    fops.open = tty_open;
    fops.read = tty_read;
    fops.write = tty_write;
    fops.close = tty_close;
    tty_dev.dops = &dops;
    tty_dev.fops = &fops;
    register_device(&tty_dev, name, TTY_DEV, S_IFCHR);
    register_irq(8, tty1_handler);
    RexSp1->Ctrl = (1 << 8) | 7; // 38400 bits per second, interrupt enabled for serial port 1
    
}
