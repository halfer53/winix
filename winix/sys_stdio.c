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
    int v = 0, offset = 0;

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
                offset++;
            }    
        }
        else {
            // kputc(d - 10 + 'A');
            *buf++ = d - 10 + 'A';
            offset++;
        }
        
    }
    *buf = '\0';
    return offset;
}

/**
 * print the number in decimal string format to buf
 * @param  n   
 * @param  buf 
 * @return     
 */
int kputd_buf(int n, char *buf) {
    int place = 1000000000;
    int offset = 0;
    // zero?
    if(n == 0) {
        *buf++ = '0';
        *buf = '\0';
        return 1;
    }

    // negative?
    if(n < 0) {
        *buf++ = '-';
        offset++;
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
        offset++;
    }
    *buf = '\0';
    return offset;
}

int kputs_vm_buf(char *s, void *who_rbase, char *buf) {
    char *sp = s;
    int offset = 0;
    sp += (int)who_rbase;
    while(*sp){
        *buf++ = *sp++;
        offset++;
    }
        
    *buf = '\0';
    return offset;
}

// int kput_token(char token, int len){
//     int offset = 0;
//     while(len--){
//         if(kputc(token) != EOF)
//             offset++;
//     }
//     return offset;
// }

#define SPACE    ' '
#define ZERO    '0'

#define LEFT_PADDING    1
#define RIGHT_PADDING   2



int tty_non_init_write(struct filp *file, char *data, size_t len, off_t offset){
    return tty_write_rex(RexSp2, data, len);
}


#define BUFFER_SIZ  (101)   // Column size in WRAMP is 100
#define FORMAT_BUF_SIZ  (11)
#define PADDING_BUFFER_SIZ (20)
#define PADDING_NUM_BUF_SIZ (8)
// static char buffer[BUFFER_SIZ];


struct printf_buffer{
    char buffer[BUFFER_SIZ];
    int limit;
    int pos;
    int (*filp_write)(struct filp *, char *, size_t, off_t );
    int total_count;
    struct filp* filp;
};

static void flush_buffer_data(struct printf_buffer* tbuf, char* data, int len){
    int ret;
    ret = tbuf->filp_write(tbuf->filp, data, len, tbuf->filp->filp_pos);
    tbuf->total_count += ret;
}   

static void flush_buffer(struct printf_buffer* tbuf){
    if(tbuf->pos > 0){
        flush_buffer_data(tbuf, tbuf->buffer, tbuf->pos);
        tbuf->pos = 0;
    }
}

static void add_char_to_buffer(struct printf_buffer* tbuf, char c){
    if(tbuf->pos >= tbuf->limit){
        flush_buffer(tbuf);
    }
    tbuf->buffer[tbuf->pos++] = c;
}

static void append_buffer(struct printf_buffer* tbuf, char* sbuf, int sbuf_len){
    char *p;
    if(sbuf_len > tbuf->limit){
        flush_buffer(tbuf);
        flush_buffer_data(tbuf, sbuf, sbuf_len);
        return;
    }

    if(sbuf_len + tbuf->pos >= tbuf->limit){
        flush_buffer(tbuf);
    }
    p = &tbuf->buffer[tbuf->pos];
    tbuf->pos += sbuf_len;
    while(sbuf_len--){
        *p++ = *sbuf++;
    }
}

static void fill_padding(struct printf_buffer* tbuf, char token, int len){
    if(len + tbuf->pos >= tbuf->limit){
        flush_buffer(tbuf);
    }
    while(len--){
        add_char_to_buffer(tbuf, token);
    }
}

static int pass_number(char **s_format){
    char *format = *s_format;
    int ret = 0;
    char padding_num_buffer[PADDING_NUM_BUF_SIZ];
    int padding_num_buffer_count = 0;

    while(isdigit(*format) && (padding_num_buffer_count < (PADDING_NUM_BUF_SIZ - 1))){
        padding_num_buffer[padding_num_buffer_count++] = *format++;
    }

    if(padding_num_buffer_count > 0 ){
        padding_num_buffer[padding_num_buffer_count] = '\0';
        ret = atoi(padding_num_buffer);
    }
    *s_format = format;
    return ret;
}

/**
 * virtual memory printf, this function is used by both kernel and user process
 * @param  format    
 * @param  arg       
 * @param  who_rbase rbase of the calling process
 * @return           number of bytes being printed
 */
int kprintf_vm( struct filp* file, const char *orignal_format, void *arg, ptr_t *who_rbase){
    char format_buffer[FORMAT_BUF_SIZ];
    struct printf_buffer _this_buffer;
    struct printf_buffer* this_buffer = &_this_buffer;
    char *format_ptr;
    int padding_len;
    int padding_direction;
    int format_buf_len;
    int (*filp_write)(struct filp *, char *, size_t, off_t );
    char* format = (char*)orignal_format;
    char prev;
    char token;
    
    memset(this_buffer, 0, sizeof(struct printf_buffer));
    this_buffer->limit = BUFFER_SIZ;
    filp_write = file ? file->filp_dev->fops->write : tty_non_init_write;
    this_buffer->filp_write = filp_write;
    this_buffer->filp = file;

    while(*format) {

        if(*format == '%') {
            format++;
            padding_direction = LEFT_PADDING;
            format_ptr = format_buffer;
            token = SPACE;
            format_buf_len = 0;

            // decode padding options
            if(*format == '-'){
                format++;
                padding_direction = RIGHT_PADDING;
            }
            if(*format == '0'){
                token = ZERO;
                format++;
            }

            padding_len = pass_number(&format);

            if(*format == 'l'){
                format++;
                //ignore
            }

            prev = *format;
            switch(*format) {
                
                case 'd':
                    format_buf_len = kputd_buf(*((int*)arg),format_ptr);
                    break;

                case 'x':
                case 'p':
                    format_buf_len = kputx_buf(*((int*)arg),format_ptr);
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
            // offset += this_buffer->pos;


            // left padding
            if(padding_len > 0){ 

                if(padding_direction == LEFT_PADDING ){
                    fill_padding(this_buffer, token, padding_len);
                    append_buffer(this_buffer, format_ptr, format_buf_len);

                }else{
                    append_buffer(this_buffer, format_ptr, format_buf_len);
                    fill_padding(this_buffer, token, padding_len);
                }
            }else{
                append_buffer(this_buffer, format_ptr, format_buf_len);

            }

        }else {

            add_char_to_buffer(this_buffer, *format++);
        }
    }
    flush_buffer(this_buffer);
    return this_buffer->total_count;
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

    kprintf2("[%d] ERROR: ", get_uptime());
    return kprintf_vm(tty2_filp, format, arg, 0);
}


int kwarn(const char *format, ...){
    void *arg = &format;
    arg = ((char*)arg) + 1;

    kprintf2("[%d] WARN: ", get_uptime());
    return kprintf_vm(tty2_filp, format, arg, 0);
}



