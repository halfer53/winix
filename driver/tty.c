//
// Created by bruce on 25/04/20.
//

#include <kernel/kernel.h>
#include <sys/tty.h>
#include <sys/fcntl.h>
#include <fs/common.h>
#include <fs/cache.h>
#include <fs/filp.h>
#include <winix/dev.h>
#include <fs/fs_methods.h>
#include <sys/ioctl.h>
#include <winix/rex.h>
#include <winix/list.h>
#include <string.h>
#include <bsd/string.h>
#include <ctype.h>
#include <winix/ksignal.h>
#include <termios.h>

#define BACKSPACE   0x08
#define TTY_BUFFER_SIZ  (64)

struct tty_state{
    struct device* dev;
    RexSp_t* rex;
    char *bptr, *buffer_end, *read_ptr;
    struct proc* reader;
    struct filp* reader_filp;
    char *read_data;
    size_t read_count;
    struct termios termios;
    char buffer[TTY_BUFFER_SIZ];
    pid_t foreground_group;
    pid_t controlling_session;
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

int __kputs(RexSp_t* rex, const char *s){
    int count = 0;
    while(*s){
        if(__kputc(rex, *s++) != EOF)
            count++;
    }
    return count;
}

/**
 * Reads a character from serial port 1. (blocking)
 **/

#define TRIES   (32)

int kgetc_blocking(struct proc* who) {
    int try;
    if(RexSp1->Ctrl & (1 << 8)){
        return EOF;
    }
    do{
        try = TRIES;
        while(!(RexSp1->Stat & 1) && --try);
        
        if(is_sigpending(who))
            return -EINTR;

    }while(try == 0);
    return RexSp1->Rx;
}

void terminal_backspace(struct tty_state* state, bool echo_erase){
    if(state->bptr > state->buffer){
        if(state->bptr == state->read_ptr){
            state->read_ptr--;
        }
        state->bptr--;
        if (echo_erase)
            __kputc(state->rex, BACKSPACE);
    }
}

void clear_terminal_buffer(struct tty_state *state, bool echo_erase){
    while(state->bptr > state->buffer){
        terminal_backspace(state, echo_erase);
    }
}

void clear_screen(RexSp_t* rex){
    static char cls[] = {0x1b, 0x5b, 0x31, 0x3b, 0x31, 0x48, 0x1b, 0x5b, 0x32, 0x4a, 0};
    // static char cls[] = {0x1b, 0x5b, 0x32, 0x4a, 0};
    __kputs(rex, cls);
}

void move_cursor(RexSp_t* rex, int num, int direction){
    //\033[XD
    static char cmd_cursor[] = {0x5c, 0x30, 0x33, 0x33, 0x5b, 0x31, 0x44, 0};
    cmd_cursor[5] = '0' + num;
    cmd_cursor[6] = direction;
    __kputs(rex, cmd_cursor);
}

void clear_reader(struct tty_state* state){
    state->reader = NULL;
    state->reader_filp = NULL;
}

bool can_return_in_non_canonical(struct tty_state* state){
    return !(state->termios.c_lflag & ICANON) && ((state->bptr - state->read_ptr) >= state->termios.c_cc[VMIN]);
}

void tty_exception_handler( struct tty_state* state){
    int val, stat;
    bool is_new_line, send_response;
    struct message* msg = get_exception_m();
    RexSp_t *rex = state->rex;
    struct termios *termios = &state->termios;
    cc_t *cc = termios->c_cc;
    
    stat = rex->Stat;
    
    if(stat & 1){
        val = rex->Rx;
        if (termios->c_iflag & ICRNL && val == '\r')
            val = '\n';
        is_new_line = val == '\n';
        send_response = is_new_line;
        
        if (termios->c_lflag & ISIG){
            int signal = 0;
            if (val == cc[VINTR])
                signal = SIGINT;
            else if (val == cc[VSUSP])
                signal = SIGTSTP;
            else if (val == cc[VQUIT])
                signal = SIGQUIT;
            
            if(signal && state->foreground_group > 0){
                // kdebug("Send sig to foreground %d\n", state->foreground_group);
                (void)sys_kill(SYSTEM_TASK, -(state->foreground_group), signal);
                goto end;
            }
        }

        if (termios->c_lflag & ICANON) {
            if (val == cc[VERASE]) {
                terminal_backspace(state, termios->c_lflag & ECHOE);
                goto end;
            }
            else if(termios->c_lflag & ECHOK && val == cc[VKILL]){
                clear_terminal_buffer(state, termios->c_lflag & ECHOE);
                goto end;
            }
            else if(val == cc[VEOF]){
                send_response = true;
            }
        }

        if(state->bptr < state->buffer_end){
            if ( !(termios->c_lflag & ICANON) || (isprint(val) || is_new_line) ){
                *state->bptr++ = val;
                if(termios->c_lflag & (ICANON | ECHO))
                    __kputc(rex, val);
            }

            if (can_return_in_non_canonical(state)) 
                send_response = true;
        }
        else if(termios->c_iflag & IMAXBEL){
            __kputc(rex, '\a');
        }

        if (is_sigpending(state->reader))
        {   
            syscall_reply2(READ, -EINTR, state->reader->proc_nr, msg);
            clear_reader(state);
        }
        else if (state->reader && (send_response || state->bptr >= state->buffer_end || state->reader_filp->filp_flags & O_NONBLOCK))
        {
            int len = state->bptr - state->read_ptr;
            memcpy(state->read_data, state->read_ptr, len);
            syscall_reply2(READ, len, state->reader->proc_nr, msg);
            state->bptr = state->buffer;
            state->read_ptr = state->buffer;
            clear_reader(state);
        }
    }
    end:
    rex->Iack = 0;
}


void tty1_handler(){
    tty_exception_handler(&tty1_state);
}

void tty2_handler(){
    tty_exception_handler(&tty2_state);
}

int __tty_init(RexSp_t* rex, struct device* dev, struct tty_state* state){
    char* buf;
    dev->private = (void*)state;
    rex->Ctrl = (1 << 8) | 7; // 38400 bits per second, interrupt enabled for serial port
    buf = state->buffer;
    state->bptr = buf;
    state->buffer_end = buf + TTY_BUFFER_SIZ - 1;
    state->rex = rex;
    state->read_ptr = buf;
    return 0;
}

int __tty_read(struct tty_state* state, char* data, size_t len){
    size_t buffer_count, count, ret = 0;

    disable_interrupt();
    buffer_count = state->bptr - state->read_ptr;
    count = buffer_count < len ? buffer_count : len;
    
    if(count > 0 && state->bptr > state->read_ptr){
        *state->bptr = '\0';
        strlcpy(data, state->read_ptr, count);
        buffer_count -= count;
        state->read_ptr += count;
        // kdebug("count %d buffer %d, ptr %x %x\n", count, buffer_count, state->bptr, state->read_ptr);
        ret = count;
    }
    enable_interrupt();
    
    return ret;
}

char* get_buffer_data(char* data, size_t count);

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

    if(curr_syscall_caller->session_id != state->controlling_session || state->foreground_group != curr_syscall_caller->procgrp){

        struct sigaction *action = &curr_syscall_caller->sig_table[SIGINT];
        kwarn("tty_read: invalid reader %d session %d pgrp %d, tty %x session %d foreground grp %d\n", 
            curr_syscall_caller->pid, curr_syscall_caller->session_id, curr_syscall_caller->procgrp,
            filp->filp_dev->dev_id, state->controlling_session, state->foreground_group);

        if (action->sa_handler == SIG_IGN || sigismember(&curr_syscall_caller->sig_mask, SIGINT))
            return -EIO;
        send_sig(curr_syscall_caller, SIGINT);
        return DONTREPLY;
    }

    if (state->reader){
        if(IS_INUSE(state->reader)){
            return -EBUSY;
        }
        clear_reader(state);
    }

    if (filp->filp_flags & O_NONBLOCK || can_return_in_non_canonical(state)){
        return __tty_read(state, data, count);
    }

    state->read_data = data;
    state->read_count = count;
    state->reader = curr_syscall_caller;
    state->reader_filp = filp;
    return SUSPEND;
}

int tty_write ( struct filp *filp, char *data, size_t count, off_t offset){
    struct tty_state* state = (struct tty_state*)filp->private;
    int ret;
    ret = tty_write_rex(state->rex, data, count);

    return ret;
}

int tty_open ( struct device* dev, struct filp *file){
    file->private = dev->private;
    return 0;
}

int tty_close ( struct device* dev, struct filp *file){
    return 0;
}

int _tty_tiocspgrp ( struct tty_state* tty_data, struct proc* who, ptr_t* ptr){
    pid_t pgrp;
    struct proc* p;
    bool found = false;
    int ret;

    if(tty_data->controlling_session != who->session_id){
        return -ENOTTY;
    }
    if ((ret = copy_from_user(who, &pgrp, (vptr_t *)*(pid_t **)ptr, sizeof(pid_t))) <= 0)
        return ret;
    foreach_proc(p){
        if(p->procgrp == pgrp && p->session_id == who->session_id){
            found = true;
        }
    }
    
    if (!found){
        return -EINVAL;
    }
    tty_data->foreground_group = pgrp;
    // kdebug("setting tty foreground to %d in %x\n", pgrp, tty_data->dev->dev_id);
    return 0;
}

int tty_ioctl(struct filp* file, int request, ptr_t* stack_ptr){
    int ret = 0;
    int result;
    struct proc* who = curr_syscall_caller;
    struct tty_state* tty_data;
    tty_data = (struct tty_state*)file->filp_dev->private;
    
    switch (request)
    {

    case TCGETS:
        result = copy_to_user(who, (vptr_t *)(*(struct termios**)stack_ptr), &tty_data->termios, sizeof(struct termios));
        if (result < 0)
            return result;
        break;

    case TCSETS:
        result = copy_from_user(who, &tty_data->termios, (vptr_t *)(*(struct termios**)stack_ptr), sizeof(struct termios));
        if (result < 0)
            return result;
        break;

    case TIOCGPGRP:
        *stack_ptr = (pid_t)tty_data->foreground_group;
        break;

    case TIOCSPGRP:
        return _tty_tiocspgrp(tty_data, who, stack_ptr);

    case TIOCSCTTY:
        if(!IS_SESSION_LEADER(who)){
            return -EPERM;
        }
        if(tty_data->controlling_session && !IS_ROOT(who)){
            return -EPERM;
        }
        tty_data->controlling_session = who->session_id;
        break;

    case TIOCNOTTY:
        if (!IS_SESSION_LEADER(who)){
            return -EPERM;
        }
        if(tty_data->foreground_group){
            sys_kill(SYSTEM_TASK, -(tty_data->foreground_group), SIGHUP);
        }
        tty_data->foreground_group = 0;
        tty_data->controlling_session = 0;
        break;

    default:
        return -EINVAL;

    }
    return ret;
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
static struct filp_operations fops = {tty_open, tty_read, tty_write, tty_close, tty_ioctl};

void init_tty_filp(struct filp** _file, struct device* dev, struct tty_state* state){
    struct filp* file;
    file = get_free_filp();
    file->filp_count = 100;
    file->filp_dev = dev;
    state->dev = dev;
    file->private = (void*)state;
    *_file = file;
}

void init_tty(){
    init_tty_filp(&tty1_filp, &_tty_dev, &tty1_state);
    init_tty_filp(&tty2_filp, &_tty2_dev, &tty2_state);

    register_device(&_tty_dev, name, TTY1_DEV_NUM, S_IFCHR, &dops, &fops);
    register_device(&_tty2_dev, name2, TTY2_DEV_NUM, S_IFCHR, &dops2, &fops);
}
