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

#define BUFFER_SIZ  (64)
#define PADDING_BUFFER_SIZ  (20)

int tty_non_init_write(struct filp *file, char *data, size_t len, off_t offset){
    return tty_write_rex(RexSp2, data, len);
}

void add_to_buffer(char* buf, int *buf_len, char c){
    int len = *buf_len;
    buf[len++] = c;
    *buf_len = len;
}


/**
 * virtual memory printf, this function is used by both kernel and user process
 * @param  format    
 * @param  arg       
 * @param  who_rbase rbase of the calling process
 * @return           number of bytes being printed
 */
int kprintf_vm( struct filp* file, const char *orignal_format, void *arg, ptr_t *who_rbase){
    static char buffer[BUFFER_SIZ];
    char *buf = buffer;
    int padding_len;
    int padding_direction;
    int buf_len = 0;
    int count = 0;
    int (*filp_write)(struct filp *, char *, size_t, off_t );
    char* format = (char*)orignal_format;
    char prev;
    char token = SPACE;
    int format_len;
    
    filp_write = file ? file->filp_dev->fops->write : tty_non_init_write;

    while(*format) {

        if(*format == '%') {
            format++;
            padding_len = 0;
            format_len = 0;
            padding_direction = LEFT_PADDING;
            buf = buffer;

            // flush the buffer
            if(buf_len > 0){
                count += filp_write(file, buffer, buf_len, count);
                buf_len = 0;
            }
            
            // decode padding options
            if(*format == '-'){
                format++;
                padding_direction = RIGHT_PADDING;
            }
            
            if(*format >= '0' && *format <= '9'){
                char buf2[3];
                // todo limit it to 20
                strncpy(buf2, format, 2);
                padding_len = atoi(buf2);
                if(padding_len > PADDING_BUFFER_SIZ){
                    padding_len = PADDING_BUFFER_SIZ;
                }
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
                    break;

                case 'x':
                case 'p':
                    buf_len = kputx_buf(*((int*)arg),buf);
                    break;

                case 's':
                    buf = *(char **)arg + (int)who_rbase;
                    if(buf){
                        buf_len = strlen(buf);
                    }else{
                        buf = buffer;
                        *buf = '\0';
                    }
                    
                    break;

                case 'c':
                default:
                    *buf++ = *(int *)arg;
                    *buf = '\0';
                    buf_len = 1;
                    break;
            }
            arg = ((char *)arg) + 1;
            format++;

            padding_len -= buf_len;
            // count += buf_len;


            // left padding
            if(padding_len > 0){ 
                if(padding_direction == LEFT_PADDING ){
                    char padding_buffer[PADDING_BUFFER_SIZ];
                    char *padding_ptr = padding_buffer;
                    int len = padding_len;
                    if(prev == 'x' || prev == 'd')
                        token = ZERO;
                    while(len-- > 0){
                        *padding_ptr++ = token;
                        // count += filp_write(file, &token, 1 , 0);
                    }
                    count += filp_write(file, padding_buffer, padding_len , count);
                }else{
                    char* p = buf + buf_len;
                    buf_len += padding_len;
                    while(padding_len-- > 0){ 
                        *p++ = token;
                    }
                    *p = '\0';
                }
            }
            count += filp_write(file, buf, buf_len, count);
            buf_len = 0;

        }else if(*format == '\\'){
            char outchar;
            format++;

            switch (*format)
            {
            case 'a':
                outchar = 0x7;
                break;
            case 'b':
                outchar = 0x8;
                break;
            case 'e':
                outchar = 0x1b;
                break;
            case 'n':
                outchar = 0xa;
                break;
            case 'r':
                outchar = 0xd;
                break;
            case 't':
                outchar = 0x9;
                break;
            
            default:
                outchar = *format;
                break;
            }
            KDEBUG(("escape char %x\n", outchar));
            buffer[buf_len++] = outchar;
            format++;

        }else {

            char outchar = *format;

            // switch (*format)
            // {
            // case '\\':
            //     outchar = '!';
            //     break;
            // case '\a':
            //     outchar = 0x7;
            //     break;
            // case '\b':
            //     outchar = 0x8;
            //     break;
            // case 0x1b:
            //     outchar = '!';
            //     break;
            // case 'e':
            //     outchar = '!';
            //     break;
            // case '\n':
            //     outchar = 0xa;
            //     break;
            // case '\r':
            //     outchar = 0xd;
            //     break;
            // case '\t':
            //     outchar = 0x9;
            //     break;
            
            // default:
            //     outchar = *format;
            //     break;
            // }
            
            format++;
            buffer[buf_len++] = outchar;

            // if this is a normal character, simply print it to 
            // serial port 1
            // count += filp_write(file, format++, 1, 0);
            // buffer[buf_len++] = *format++;
            if(buf_len >= BUFFER_SIZ){
                count += filp_write(file, buffer, buf_len, count);
                buf_len = 0;
            }
        }
    }
    if(buf_len > 0){
        count += filp_write(file, buffer, buf_len, count);
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



