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



int tty_non_init_write(struct filp *file, char *data, size_t len, off_t offset){
    return tty_write_rex(RexSp2, data, len);
}

void add_to_buffer(char* buf, int *buf_len, char c){
    int len = *buf_len;
    buf[len++] = c;
    *buf_len = len;
}

#define BUFFER_SIZ  (64)
#define FORMAT_BUF_SIZ  (11)
#define PADDING_NUM_BUF_SIZ (3)
/**
 * virtual memory printf, this function is used by both kernel and user process
 * @param  format    
 * @param  arg       
 * @param  who_rbase rbase of the calling process
 * @return           number of bytes being printed
 */
int kprintf_vm( struct filp* file, const char *orignal_format, void *arg, ptr_t *who_rbase){
    static char buffer[BUFFER_SIZ];
    static char format_buffer[FORMAT_BUF_SIZ];
    char *ptr, *p, *format_ptr, *second_ptr;
    int padding_len;
    int padding_direction;
    int buf_len = 0, format_buf_len, second_buf_len;
    int count = 0;
    int (*filp_write)(struct filp *, char *, size_t, off_t );
    char* format = (char*)orignal_format;
    char prev;
    char token;
    int format_len;
    char padding_num_buffer[PADDING_NUM_BUF_SIZ];
    int padding_num_buffer_count;
    
    filp_write = file ? file->filp_dev->fops->write : tty_non_init_write;

    while(*format) {

        if(*format == '%') {
            format++;
            padding_len = 0;
            format_len = 0;
            padding_num_buffer_count = 0;
            padding_direction = LEFT_PADDING;
            format_ptr = format_buffer;
            token = SPACE;
            format_buf_len = 0;

            // flush the buffer
            // if(buf_len > 0){
            //     count += filp_write(file, buffer, buf_len, count);
            //     buf_len = 0;
            // }
            
            // decode padding options
            if(*format == '-'){
                format++;
                padding_direction = RIGHT_PADDING;
            }

            if(*format == '0'){
                token = ZERO;
                format++;
            }
            
            while(isdigit(*format) && (padding_num_buffer_count < (PADDING_NUM_BUF_SIZ - 1))){
                padding_num_buffer[padding_num_buffer_count++] = *format++;
            }

            if(padding_num_buffer_count > 0 ){
                padding_num_buffer[padding_num_buffer_count] = '\0';
                padding_len = atoi(padding_num_buffer);
                // kputs(padding_num_buffer);
            }
            

            if(*format == 'l'){
                format++;
                //ignore
            }

            prev = *format;
            switch(*format) {
                
                case 'd':
                    format_buf_len = kputd_buf(*((int*)arg),format_buffer);
                    break;

                case 'x':
                case 'p':
                    format_buf_len = kputx_buf(*((int*)arg),format_buffer);
                    break;

                case 's':
                    format_ptr = *(char **)arg + (int)who_rbase;
                    if(format_ptr){
                        format_buf_len = strlen(format_ptr);
                    }
                    break;

                case 'c':
                default:
                    *format_ptr = *((int *)arg);
                    format_buf_len = 1;
                    break;
            }
            arg = ((char *)arg) + 1;
            format++;

            padding_len -= format_buf_len;
            // count += buf_len;


            // left padding
            if(padding_len > 0){ 
                int padding_total;
                int len = padding_len;
                second_ptr = format_buffer;

                if(padding_direction == LEFT_PADDING ){
                    second_buf_len = format_buf_len;
                    padding_total = padding_len + buf_len;
                    p = &buffer[buf_len];
                    if(padding_total >= BUFFER_SIZ){
                        len = BUFFER_SIZ - buf_len;
                    }
                    buf_len += len;

                }else{
                    goto add_format_to_buffer;

                    continue_right_padding_processing:
                    if(len >= FORMAT_BUF_SIZ){
                        len = FORMAT_BUF_SIZ;
                    }
                    p = format_buffer;
                    second_buf_len = len;
                }
                
                while(len-- > 0){
                    *p++ = token;
                }
                count += filp_write(file, buffer , buf_len, count);
                count += filp_write(file, second_ptr , second_buf_len, count);
                buf_len = 0;
                continue;
            }

            add_format_to_buffer:
            ptr = buffer;
            if(format_buf_len + buf_len >= BUFFER_SIZ){
                count += filp_write(file, buffer , buf_len, count);
                buf_len = 0;
            }else{
                ptr += buf_len;
            }

            buf_len += format_buf_len;
            while(format_buf_len--){
                *ptr++ = *format_ptr++;
            }

            if(padding_len > 0 && padding_direction == RIGHT_PADDING){
                goto continue_right_padding_processing;
            }

        }else {
            char outchar = *format;
            format++;
            buffer[buf_len++] = outchar;

            // if this is a normal character, simply print it to 
            // serial port 1
            // count += filp_write(file, format++, 1, 0);
            // buffer[buf_len++] = *format++;
            if(buf_len >= (BUFFER_SIZ)){
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



