//
// Created by bruce on 25/04/20.
//
#include <sys/fcntl.h>
#include <fs/const.h>
#include <fs/cache.h>
#include <fs/filp.h>
#include <winix/dev.h>
#include <fs/fs_methods.h>
#include <kernel/kernel.h>
#include <winix/rex.h>
#include <winix/list.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZ  (128)

struct tty_state{
    struct device* dev;
    char* name;
    char *bptr, *buffer_end;
    struct proc* reader;
    char *read_data;
    size_t read_count;
    char buffer[BUFFER_SIZ];
};


struct device _tty_dev, _tty2_dev;
struct device *tty_dev = NULL, *tty2_dev = NULL;
static struct filp_operations fops;
static struct device_operations dops, dops2;
static const char* name = "tty";
static const char* name2 = "tty2";
static char buffer[BUFFER_SIZ];
char *bptr, *buffer_end;
struct proc* reader = NULL;
char *read_data;
size_t read_count;


#define IS_SERIAL_CODE(c) (isprint(c) || c - 7 < 6)
/**
 * Writes a character to serial port 1.
 **/
int kputc(const int c) {
    if(IS_SERIAL_CODE(c)){
        while(!(RexSp1->Stat & 2));
        RexSp1->Tx = c;
        return c;
    }
    return EOF;
}

/**
 * Reads a character from serial port 1. (blocking)
 **/

#define TRIES   (32)

int kgetc(struct proc* who) {
    int try;
    if(RexSp1->Ctrl & (1 << 8)){
        return EOF;
    }
    do{
        try = TRIES;
        while(!(RexSp1->Stat & 1) && --try);
        
        if(is_sigpending(who))
            return EINTR;

    }while(try == 0);
    return RexSp1->Rx;
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
            syscall_reply2(READ, bptr - buffer, reader->proc_nr, msg);
            bptr = buffer;
            reader = NULL;
        }
    }
    end:
    RexSp1->Iack = 0;
}

int tty_read ( struct filp *filp, char *data, size_t count, off_t offset){
    if(reader){
        return EBUSY;
    }
    read_data = data;
    read_count = count;
    reader = curr_user;
    return SUSPEND;
}

int tty_write ( struct filp *filp, char *data, size_t count, off_t offset){
    size_t len = count;
    while(count-- > 0){
        kputc(*data++);
    }
    return len;
}

int tty_open ( struct inode* ino, struct filp *file){
    return 0;
}

int tty_close ( struct inode* ino, struct filp *file){
    return 0;
}

int tty_dev_init(){
    register_irq(8, tty1_handler);
    RexSp1->Ctrl = (1 << 8) | 7; // 38400 bits per second, interrupt enabled for serial port 1
    bptr = buffer;
    buffer_end = bptr + BUFFER_SIZ - 1;
    return 0;
}

int tty_dev_io_read(char *buf, off_t off, size_t len){
    size_t buffer_count, count;
    *bptr = '\0';
    buffer_count = bptr - buffer;
    count = buffer_count < len ? buffer_count : len;
    strncpy(buf, buffer, count);
    bptr -= count;
    if(bptr < buffer)
        bptr = buffer;
    return count;
}

int tty_dev_io_write(char *buf, off_t off, size_t len){
    char *p = buf;
    while(len-- > 0){
        if(IS_SERIAL_CODE(*p)){
            while(!(RexSp1->Stat & 2));
            RexSp1->Tx = *p++;
        }
    }
    return p - buf;
}

int tty_dev_release(){
    return 0;
}


/* TTY2: Serial Port 2 handler */

int tty2_dev_init(){
    RexSp2->Ctrl = 7; // 38400 bits per second
    return 0;
}

int tty2_dev_io_read(char *buf, off_t off, size_t len){
    size_t count = len;
    while(count-- > 0){
        while(!(RexSp2->Stat & 1));
        *buf++ = RexSp2->Rx;
    }
    return len;
}

int tty2_dev_io_write(char *buf, off_t off, size_t len){
    char *p = buf;
    while(len-- > 0){
        if(IS_SERIAL_CODE(*p)){
            while(!(RexSp2->Stat & 2));
            RexSp2->Tx = *p++;
        }
    }
    return p - buf;
}

int tty2_dev_release(){
    return 0;
}


void init_tty(){
    dops.dev_init = tty_dev_init;
    dops.dev_read = tty_dev_io_read;
    dops.dev_write = tty_dev_io_write;
    dops.dev_release = tty_dev_release;

    dops2.dev_init = tty2_dev_init;
    dops2.dev_read = tty2_dev_io_read;
    dops2.dev_write = tty2_dev_io_write;
    dops2.dev_release = tty2_dev_release;

    fops.open = tty_open;
    fops.read = tty_read;
    fops.write = tty_write;
    fops.close = tty_close;
    
    tty_dev = &_tty_dev;
    tty2_dev = &_tty2_dev;
    _tty_dev.dops = &dops;
    _tty_dev.fops = &fops;
    _tty2_dev.fops = &fops;
    _tty2_dev.dops = &dops2;
    register_device(&_tty_dev, name, TTY_DEV_NUM, S_IFCHR);
    register_device(&_tty2_dev, name2, TTY2_DEV_NUM, S_IFCHR);
}
