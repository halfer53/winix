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

struct tty_state tty1_state, tty2_state;
struct device _tty_dev, _tty2_dev;
struct filp *tty1_filp = NULL, *tty2_filp = NULL;

static const char* name = "tty";
static const char* name2 = "tty2";


#define IS_SERIAL_CODE(c) (c >= 0 && c <= 127)

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
            }
            goto end;
        }

        if(state->bptr < state->buffer_end){
            if(val == '\r')
                val = '\n';

            if(isprint(val) || val == '\n'){
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
    tty_exception_handler(RexSp1, &tty1_state);
}

void tty2_handler(){
    tty_exception_handler(RexSp2, &tty2_state);
}

int __tty_init(RexSp_t* rex, struct device* dev, struct tty_state* state){
    char* buf;
    dev->private = (void*)state;
    rex->Ctrl = (1 << 8) | 7; // 38400 bits per second, interrupt enabled for serial port
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
    if(count > 0){
        strncpy(data, state->buffer, count);
        state->bptr -= count;
    }
    return count;
}

int tty_write_rex(RexSp_t* rex, char* data, size_t len){
    char *p = data;
    while(len-- > 0){
        if(IS_SERIAL_CODE(*p)){
            while(!(rex->Stat & 2));
            rex->Tx = *p;
        }
        p++;
    }
    return p - data;
}

int tty_read ( struct filp *filp, char *data, size_t count, off_t offset){
    struct tty_state* state = (struct tty_state*)filp->private;
    if(state->reader){
        return EBUSY;
    }
    if(filp->filp_flags & O_NONBLOCK){
        return __tty_read(state, data, count);
    }
    state->read_data = data;
    state->read_count = count;
    state->reader = curr_user;
    return SUSPEND;
}

int tty_write ( struct filp *filp, char *data, size_t count, off_t offset){
    struct tty_state* state = (struct tty_state*)filp->private;
    return tty_write_rex(state->rex, data, count);
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
    return __tty_init(RexSp1, &_tty_dev, &tty1_state);
}

int tty_dev_io_read(char *buf, off_t off, size_t len){
    return __tty_read(&tty1_state, buf, len);
}

int tty_dev_io_write(char *buf, off_t off, size_t len){
    return tty_write_rex(RexSp1, buf, len);
}

int tty_dev_release(){
    return 0;
}

/* TTY2: Serial Port 2 handler */

int tty2_dev_init(){
    register_irq(9, tty2_handler);
    return __tty_init(RexSp2, &_tty2_dev, &tty2_state);
}

int tty2_dev_io_read(char *buf, off_t off, size_t len){
    return __tty_read(&tty2_state, buf, len);
}

int tty2_dev_io_write(char *buf, off_t off, size_t len){
    return tty_write_rex(RexSp2, buf, len);
}

int tty2_dev_release(){
    return 0;
}

static struct device_operations dops = {tty_dev_init, tty_dev_io_read, tty_dev_io_write, tty_dev_release};
static struct device_operations dops2  = {tty2_dev_init, tty2_dev_io_read, tty2_dev_io_write, tty2_dev_release};
static struct filp_operations fops = {tty_open, tty_read, tty_write, tty_close};

void init_tty_filp(struct filp** _file, struct device* dev, struct tty_state* state){
    struct filp* file;
    file = get_free_filp();
    file->filp_count = 100;
    file->filp_dev = dev;
    file->private = (void*)state;
    *_file = file;
}

void init_tty(){

    init_tty_filp(&tty1_filp, &_tty_dev, &tty1_state);
    init_tty_filp(&tty2_filp, &_tty2_dev, &tty2_state);

    register_device(&_tty_dev, name, TTY_DEV_NUM, S_IFCHR, &dops, &fops);
    register_device(&_tty2_dev, name2, TTY2_DEV_NUM, S_IFCHR, &dops2, &fops);
}