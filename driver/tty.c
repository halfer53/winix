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

#define BUFFER_SIZ  (64)

struct tty_state{
    struct device* dev;
    RexSp_t* rex;
    char *bptr, *buffer_end;
    struct proc* reader;
    char *read_data;
    size_t read_count;
    char buffer[BUFFER_SIZ];
};

struct tty_state state1, state2;
struct device _tty_dev, _tty2_dev;
struct device *tty_dev = NULL, *tty2_dev = NULL;
static struct filp_operations fops;
static struct device_operations dops, dops2;
static const char* name = "tty";
static const char* name2 = "tty2";
// static char buffer[BUFFER_SIZ];
// char *bptr, *buffer_end;
// struct proc* reader = NULL;
// char *read_data;
// size_t read_count;


#define IS_SERIAL_CODE(c) (isprint(c) || c - 7 < 6)

int __kputc(RexSp_t* rex, const int c) {
    if(IS_SERIAL_CODE(c)){
        while(!(rex->Stat & 2));
        rex->Tx = c;
        return c;
    }
    return EOF;
}
/**
 * Writes a character to serial port 1.
 **/
int kputc( const int c) {
    if(IS_SERIAL_CODE(c)){
        while(!(RexSp1->Stat & 2));
        RexSp1->Tx = c;
        return c;
    }
    return EOF;
}

int kputs(const char *s) {
    int count = 0;
    while(*s){
        if(kputc(*s++) != EOF)
            count++;
    }
    return count;    
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

void tty_exception_handler(RexSp_t* rex, struct tty_state* state){
    int val, stat;
    struct message* msg;
    stat = rex->Stat;
    
    if(stat & 1){
        val = rex->Rx;

        if (val == 8) { // backspace
            if(state->bptr > state->buffer){
                state->bptr--;
                __kputc(rex, val);
                goto end;
            }
        }

        if(state->bptr < state->buffer_end){
            if(val == '\r')
                val = '\n';

            if(IS_SERIAL_CODE(val) || val == '\n'){
                *state->bptr++ = val;
                __kputc(rex, val);
            }else{
                __kputc(rex, 7);            // beep
            }
        }

        if((val == '\n' || state->bptr >= state->buffer_end ) && state->reader){
            *state->bptr = '\0';
            msg = get_exception_m();
            strncpy(state->read_data, state->buffer, state->read_count);
            syscall_reply2(READ, state->bptr - state->buffer, state->reader->proc_nr, msg);
            state->bptr = state->buffer;
            state->reader = NULL;
        }
    }
    end:
    rex->Iack = 0;
}


void tty1_handler(){
    tty_exception_handler(RexSp1, &state1);
}

void tty2_handler(){
    tty_exception_handler(RexSp2, &state2);
}

int __tty_init(RexSp_t* rex, struct device* dev, struct tty_state* state){
    char* buf;
    dev->private = (void*)state;
    rex->Ctrl = (1 << 8) | 7; // 38400 bits per second, interrupt enabled for serial port 1
    buf = state->buffer;
    state->bptr = buf;
    state->buffer_end = buf + BUFFER_SIZ - 1;
    state->rex = rex;
    return 0;
}

int __tty_read(struct tty_state* state, char* data, size_t len){
    size_t buffer_count, count;
    *state->bptr = '\0';
    buffer_count = state->bptr - state->buffer;
    count = buffer_count < len ? buffer_count : len;
    strncpy(data, state->buffer, count);
    state->bptr -= count;
    return count;
}

int __tty_write(struct tty_state* state, char* data, size_t len){
    RexSp_t* rex = state->rex;
    char *p = data;
    while(len-- > 0){
        if(IS_SERIAL_CODE(*p)){
            while(!(rex->Stat & 2));
            rex->Tx = *p++;
        }
    }
    return p - data;
}

int tty_read ( struct filp *filp, char *data, size_t count, off_t offset){
    struct tty_state* state = (struct tty_state*)filp->private;
    if(state->reader){
        return EBUSY;
    }
    state->read_data = data;
    state->read_count = count;
    state->reader = curr_user;
    return SUSPEND;
}

int tty_write ( struct filp *filp, char *data, size_t count, off_t offset){
    return __tty_write((struct tty_state*)filp->private, data, count);
}

int tty_open ( struct device* dev, struct filp *file){
    file->private = dev->private;
    return 0;
}

int tty_close ( struct device* dev, struct filp *file){
    return 0;
}



int tty_dev_init(){
    register_irq(8, tty1_handler);
    return __tty_init(RexSp1, &_tty_dev, &state1);
}

int tty_dev_io_read(char *buf, off_t off, size_t len){
    return __tty_read(&state1, buf, len);
}

int tty_dev_io_write(char *buf, off_t off, size_t len){
    return __tty_write(&state1, buf, len);
}

int tty_dev_release(){
    return 0;
}

/* TTY2: Serial Port 2 handler */

int tty2_dev_init(){
    register_irq(9, tty2_handler);
    return __tty_init(RexSp2, &_tty2_dev, &state2);
}

int tty2_dev_io_read(char *buf, off_t off, size_t len){
    return __tty_read(&state2, buf, len);
}

int tty2_dev_io_write(char *buf, off_t off, size_t len){
    return __tty_write(&state2, buf, len);
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
