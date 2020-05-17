/**
 * 
 * Winix kernel stdio
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2016-09-19
 * 
*/
#include <kernel/kernel.h>
#include <kernel/clock.h>
#include <winix/rex.h>
#include <ctype.h>
#include <winix/dev.h>

const char *errlist[_NERROR] = {
    0,			/* EGENERIC */    
    "EPERM",    /* EPERM */
    "ENOENT",    /* ENOENT */
    "ESRCH",    /* ESRCH */
    "EINTR",    /* EINTR */
    "EIO",			/* EIO */    
    "ENXIO",    /* ENXIO */
    "E2BIG",    /* E2BIG */
    "ENOEXEC",    /* ENOEXEC */
    "EBADF",    /* EBADF */
    "ECHILD",    /* ECHILD */
    "EAGAIN",    /* EAGAIN */
    "ENOMEM",    /* ENOMEM */
    "EACCES",    /* EACCES */
    "EFAULT",    /* EFAULT */
    "ENOTBLK",    /* ENOTBLK */
    "EBUSY",    /* EBUSY */
    "EEXIST",    /* EEXIST */
    "EXDEV",		/* EXDEV */    
    "ENODEV",    /* ENODEV */
    "ENOTDIR",    /* ENOTDIR */
    "EISDIR",    /* EISDIR */
    "EINVAL",    /* EINVAL */
    "ENFILE",    /* ENFILE */
    "EMFILE",    /* EMFILE */
    "ENOTTY",    /* ENOTTY */
    "ETXTBSY",    /* ETXTBSY */
    "EFBIG",    /* EFBIG */
    "ENOSPC",    /* ENOSPC */
    "ESPIPE",    /* ESPIPE */
    "EROFS",	/* EROFS */    
    "EMLINK",    /* EMLINK */
    "EPIPE",    /* EPIPE */
    "EDOM",    /* EDOM */
    "ERANGE",    /* ERANGE */
    "EDEADLK",    /* EDEADLK */
    "ENAMETOOLONG",    /* ENAMETOOLONG */
    "ENOLCK",    /* ENOLCK */
    "ENOSYS",    /* ENOSYS */
    "ENOTEMPTY",    /* ENOTEMPTY */
};

const char* kstr_error(int err){
    if(err < 0){
        err = -err;
    }
    if(err < 0 || err >= _NERROR)
        return (const char*)0;
    return errlist[err];
}



#define IS_SERIAL_CODE(c) (isprint(c) || c - 7 < 6)

/**
 * print value of n in hexadecimal string format
 * into the buffer
 * @param  n   
 * @param  buf 
 * @return     OK
 */
int kputx_buf(int n,char *buf) {
    int i;
    int v = 0, count = 0;

    if(n == 0){
        *buf++ = '0';
        *buf = '\0';
        return 1;
    }
    for(i = 28; i >= 0; i -= 4) {
        int d = (n >> i) & 0xf;
        if(d)
            v = 1;
        if(d < 10) {
            if(v){
                *buf++ = d + '0';
                count++;
            }    
        }
        else {
            // kputc(d - 10 + 'A');
            *buf++ = d - 10 + 'A';
            count++;
        }
        
    }
    *buf = '\0';
    return count;
}

/**
 * print the number in decimal string format to buf
 * @param  n   
 * @param  buf 
 * @return     
 */
int kputd_buf(int n, char *buf) {
    int place = 1000000000;
    int count = 0;
    // zero?
    if(n == 0) {
        *buf++ = '0';
        *buf = '\0';
        return 1;
    }

    // negative?
    if(n < 0) {
        *buf++ = '-';
        count++;
        n *= -1;
    }

    // find first digit of number
    while(place > n) {
        place /= 10;
    }

    // print the rest
    while(place) {
        int d = n / place;
        *buf++ = d % 10 + '0';
        place /= 10;
        count++;
    }
    *buf = '\0';
    return count;
}

int kputs_vm_buf(char *s, void *who_rbase, char *buf) {
    char *sp = s;
    int count = 0;
    sp += (int)who_rbase;
    while(*sp){
        *buf++ = *sp++;
        count++;
    }
        
    *buf = '\0';
    return count;
}

// int kput_token(char token, int len){
//     int count = 0;
//     while(len--){
//         if(kputc(token) != EOF)
//             count++;
//     }
//     return count;
// }

#define SPACE    ' '
#define ZERO    '0'

#define LEFT_PADDING    1
#define RIGHT_PADDING   2

int tty_non_init_write(struct filp *file, char *data, size_t len, off_t offset){
    return tty_write_rex(RexSp2, data, len);
}


/**
 * virtual memory printf, this function is used by both kernel and user process
 * @param  format    
 * @param  arg       
 * @param  who_rbase rbase of the calling process
 * @return           number of bytes being printed
 */
int kprintf_vm( struct filp* file, const char *orignal_format, void *arg, ptr_t *who_rbase){
    static char buffer[64];
    char *buf = buffer;
    int padding_len;
    int padding_direction;
    int buf_len;
    int count = 0;
    int (*filp_write)(struct filp *, char *, size_t, off_t );
    char* format = (char*)orignal_format;
    
    filp_write = file ? file->filp_dev->fops->write : tty_non_init_write;

    while(*format) {
        if(*format == '%') {
            char prev;
            char token = SPACE;
            format++;
            padding_len = 0;
            padding_direction = LEFT_PADDING;
            buf = buffer;
            
            // decode padding options
            if(*format == '-'){
                format++;
                padding_direction = RIGHT_PADDING;
            }
            
            if(*format >= '0' && *format <= '9'){
                strncpy(buffer, format, 2);
                padding_len = atoi(buffer);
                format += 2;
            }

            if(*format == 'l'){
                format++;
                //ignore
            }

            prev = *format;
            switch(*format) {
                
                case 'd':
                    buf_len = kputd_buf(*((int*)arg),buf);
                    goto arg_end;

                case 'x':
                case 'p':
                    buf_len = kputx_buf(*((int*)arg),buf);
                    padding_direction = LEFT_PADDING;
                    goto arg_end;

                case 's':
                    buf = *(char **)arg+ (int)who_rbase;
                    buf_len = strlen(buf);
                    goto arg_end;

                case 'c':
                    buffer[0] = *(int *)arg;
                    buffer[1] = '\0';
                    buf_len = 1;
                    goto arg_end;

                default:
                    // if(kputc(*format++) != EOF)
                    //     count++;
                    count += filp_write(file, format++, 1, 0);
                    break;
                
                arg_end:
                    arg = ((char *)arg) + 1;
                    format++;
            }
            

            padding_len -= buf_len;
            count += buf_len;


            // left padding
            if(padding_len > 0){ 
                if(padding_direction == LEFT_PADDING ){
                    if(prev == 'x' || prev == 'd')
                        token = ZERO;
                    // count += kput_token(token, padding_len);
                    while(padding_len-- > 0){
                        count += filp_write(file, &token, 1, 0);
                    }
                }else{
                    char* p = buf + buf_len;
                    buf_len += padding_len;
                    while(padding_len-- > 0){
                        *p++ = token;
                    }
                    *p = '\0';
                }
                
            }

            // count += kputs(buf);
            count += filp_write(file, buf, buf_len, 0);
            
            // right padding
            // if(padding_direction == RIGHT_PADDING && padding_len > 0)
            //     count += kput_token(token, padding_len);
            
        }else {
            // if this is a normal character, simply print it to 
            // serial port 1
            // if(kputc(*format++)!= EOF)
            //     count++;
            count += filp_write(file, format++, 1, 0);
        }
    }
    return count;
}

int filp_kprint(struct filp* file, const char* format, ...){
    void *arg = &format;
    arg = ((char*)arg) + 1;

    return kprintf_vm(file, format, arg, 0);
}

int kprintf(const char *format, ...) {
    void *arg = &format;
    arg = ((char*)arg) + 1;

    return kprintf_vm(tty1_filp, format, arg, 0);
}

int kprintf2(const char *format, ...) {
    void *arg = &format;
    arg = ((char*)arg) + 1;

    return kprintf_vm(tty2_filp, format, arg, 0);
}

int klog(const char *format, ...){
    void *arg = &format;
    arg = ((char*)arg) + 1;

    kprintf2("[%d] ", get_uptime());
    return kprintf_vm(tty2_filp,format, arg, 0);
}

int kerror(const char *format, ...){
    void *arg = &format;
    arg = ((char*)arg) + 1;

    kprintf("[%d] ERROR: ", get_uptime());
    return kprintf_vm(tty2_filp, format, arg, 0);
}



