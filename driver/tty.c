//
// Created by bruce on 25/04/20.
//

#include <kernel/kernel.h>
#include <sys/tty.h>
#include <sys/fcntl.h>
#include <fs/const.h>
#include <fs/cache.h>
#include <fs/filp.h>
#include <winix/dev.h>
#include <fs/fs_methods.h>
#include <sys/ioctl.h>
#include <winix/rex.h>
#include <winix/list.h>
#include <string.h>
#include <ctype.h>
#include <winix/tty.h>
#include <winix/sigsend.h>


#define CTRL_A  (1)
#define CTRL_E  (5)
#define CTRL_B  (2)
#define CTRL_F  (6)
#define CTRL_P  (16)
#define CTRL_N  (14)
#define CTRL_C  (3)
#define CTRL_Z  (26)
#define BEEP    (7)
#define BACKSPACE   (8)
#define CTRL_U  (21)
#define CTRL_L  (12)
#define CTRL_D  (4)

#define CURSOR_LEFT     ('D')
#define CURSOR_RIGHT    ('C')

struct tty_command{
    char *command;
    int len;
    struct list_head list;
};

struct tty_state tty1_state, tty2_state;
struct device _tty_dev, _tty2_dev;
struct filp *tty1_filp = NULL, *tty2_filp = NULL;
struct list_head commands;
struct tty_command* prev_history_cmd = NULL;

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
    int count;
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

void save_command_history(struct tty_state* state){
    struct tty_command* cmd;
    int len;
    cmd = kmalloc(sizeof(struct tty_command));
    if(!cmd)
        return;
    len = strlen(state->read_ptr);
    cmd->command = kmalloc(len + 1);
    if(!cmd->command)
        goto err_str;
    strcpy(cmd->command, state->read_ptr);
    cmd->len = len;
    list_add(&cmd->list, &commands);
    // KDEBUG(("saving %s\n", state->read_ptr));
    return;

    err_str:
    kfree(cmd);
}

void terminal_backspace(RexSp_t* rex, struct tty_state* state){
    if(state->bptr > state->buffer){
        if(state->bptr == state->read_ptr){
            state->read_ptr--;
        }
        state->bptr--;
        __kputc(rex, BACKSPACE);
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

void tty_exception_handler(RexSp_t* rex, struct tty_state* state){
    int val, stat, ret;
    struct message* msg;
    
    stat = rex->Stat;
    
    if(stat & 1){
        val = rex->Rx;

        if (val == BACKSPACE) { // backspace
            terminal_backspace(rex, state);
            goto end;
        }
        else if(val == CTRL_C || val == CTRL_Z){ // Control C or Z
            int signal;
            switch (val)
            {
            case CTRL_C:
                signal = SIGINT;
                break;

            case CTRL_Z:
                signal = SIGTSTP;
                break;
            
            default:
                break;
            }
            if(state->controlling_session > 0 && state->foreground_group > 0){
                ret = sys_kill(SYSTEM_TASK, -(state->foreground_group), signal);
                // KDEBUG(("C ret %d curr %p %d state %x\n", ret,current_proc,  current_proc->proc_nr, current_proc->state));
            }
            goto end;
        }
        else if(val == CTRL_P || val == CTRL_N){ // control p or n, to navigate through history
            struct tty_command* t1;
            bool found = false;
            int len;

            
            if(prev_history_cmd){
                t1 = (val == CTRL_P) ? list_next_entry(struct tty_command, prev_history_cmd, list) : 
                                    list_prev_entry(struct tty_command, prev_history_cmd, list);
                if(&t1->list != (&commands)){
                    found = true;
                }
            }else{
                t1 = list_first_entry(&commands, struct tty_command, list);
                found = true;
            }
            
            if(found){
                prev_history_cmd = t1;
                while(state->bptr > state->buffer){
                    terminal_backspace(rex, state);
                }
                strncpy(state->bptr, t1->command, state->buffer_end - state->bptr);
                state->bptr += t1->len;
                __kputs(rex, t1->command);
            }
        }
        else if(val == CTRL_U){
            while(state->bptr > state->buffer){
                terminal_backspace(rex, state);
            }
        }
        else if(val == CTRL_L){
            clear_screen(rex);
            state->read_ptr = state->bptr = state->buffer;
            val = '\n';
        }
        else if(val == CTRL_D){
            ret = sys_kill(SYSTEM_TASK, -(state->foreground_group), SIGKILL);
        }

        // reset ring buffer if buffer end is reached
        // if(state->bptr >= state->buffer_end && state->read_ptr == state->bptr){
        //     state->read_ptr = state->bptr = state->buffer;
        // }

        if(state->bptr < state->buffer_end){
            if(val == '\r')
                val = '\n';

            // only save command history if it's a regular command typed by the user
            if(val == '\n'){
                // KDEBUG(("prev cmd %x\n", prev_history_cmd));
                if(prev_history_cmd == NULL){
                    *state->bptr = '\0';
                    save_command_history(state);
                }
                prev_history_cmd = NULL;
            }

            if(isprint(val) || val == '\n'){
                *state->bptr++ = val;
                if(state->is_echoing){
                    __kputc(rex, val);
                    // KDEBUG(("received %d\n", val));
                }
            }else{
                __kputc(rex, BEEP);            // beep
            }
        }

        if((val == '\n' || state->bptr >= state->buffer_end ) && state->reader){
            *state->bptr = '\0';
            msg = get_exception_m();
            strncpy(state->read_data, state->read_ptr, state->read_count);
            syscall_reply2(READ, state->bptr - state->buffer, state->reader->proc_nr, msg);
            state->bptr = state->buffer;
            state->read_ptr = state->buffer;
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
    state->buffer_end = buf + TTY_BUFFER_SIZ - 1;
    state->rex = rex;
    state->is_echoing = true;
    state->read_ptr = buf;
    return 0;
}

int __tty_read(struct tty_state* state, char* data, size_t len){
    size_t buffer_count, count;
    buffer_count = state->bptr - state->read_ptr;
    count = buffer_count < len ? buffer_count : len;
    
    if(count > 0 && state->bptr > state->read_ptr){
        *state->bptr = '\0';
        strncpy(data, state->read_ptr, count);
        buffer_count -= count;
        state->read_ptr += count;
        // KDEBUG(("count %d buffer %d, ptr %x %x\n", count, buffer_count, state->bptr, state->read_ptr));
        return count;
    }
    return 0;
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
    if(curr_syscall_caller->procgrp != state->foreground_group){
        send_sig(curr_syscall_caller, SIGINT);
        return DONTREPLY;
    }

    if(state->reader){
        if(IS_INUSE(state->reader)){
            return EBUSY;
        }
        state->reader = NULL;
    }
    if(filp->filp_flags & O_NONBLOCK){
        return __tty_read(state, data, count);
    }
    state->read_data = data;
    state->read_count = count;
    state->reader = curr_syscall_caller;
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

int tty_ioctl(struct filp* file, int request, ptr_t* ptr){
    struct proc* who = curr_syscall_caller;
    struct tty_state* tty_data;
    tty_data = (struct tty_state*)file->filp_dev->private;
    switch (request)
    {
    case TIOCGPGRP:
        *ptr = (pid_t)tty_data->foreground_group;
        break;
    case TIOCSPGRP:
        tty_data->foreground_group = (pid_t)*ptr;
        // KDEBUG(("foreground to %d for %s, vir %x phy %x\n", (pid_t)*ptr, dev->init_name, arg, ptr));
        break;
    case TIOCSCTTY:
        tty_data->controlling_session = who->session_id;
        break;
    case TIOCDISABLEECHO:
        tty_data->is_echoing = false;
        break;
    case TIOCENABLEECHO:
        tty_data->is_echoing = true;
        break;

    default:
        return EINVAL;
    }
    return OK;
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
    file->private = (void*)state;
    *_file = file;
}

void init_tty(){
    INIT_LIST_HEAD(&commands);
    init_tty_filp(&tty1_filp, &_tty_dev, &tty1_state);
    init_tty_filp(&tty2_filp, &_tty2_dev, &tty2_state);

    register_device(&_tty_dev, name, TTY1_DEV_NUM, S_IFCHR, &dops, &fops);
    register_device(&_tty2_dev, name2, TTY2_DEV_NUM, S_IFCHR, &dops2, &fops);
}
