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


/**
 * Writes a character to serial port 1.
 **/
int kputc(const int c) {
    while(!(RexSp1->Stat & 2));
    RexSp1->Tx = c;
    return c;
}


int kputc2(const int c) {
    while(!(RexSp2->Stat & 2));
    RexSp2->Tx = c;
    return c;
}

/**
 * Reads a character from serial port 1.
 **/
int kgetc() {
    //TODO: user interrupt-driven I/O
    //Use thread
    while(!(RexSp1->Stat & 1));
    return RexSp1->Rx;
}

/**
 * print value of n in hexadecimal string format
 * into the buffer
 * @param  n   
 * @param  buf 
 * @return     OK
 */
PRIVATE int kputx_buf(int n,char *buf) {
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
PRIVATE int kputd_buf(int n, char *buf) {
    int place = 1000000000;
    int count = 0;
    //zero?
    if(n == 0) {
        // kputc('0');
        *buf++ = '0';
        *buf = '\0';
        return 1;
    }

    //negative?
    if(n < 0) {
        // kputc('-');
        *buf++ = '-';
        count++;
        n *= -1;
    }

    //find first digit of number
    while(place > n) {
        place /= 10;
    }

    //print the rest
    while(place) {
        int d = n / place;
        // kputc(d % 10 + '0');
        *buf++ = d % 10 + '0';
        place /= 10;
        count++;
    }
    *buf = '\0';
    return count;
}

PRIVATE int kputs_vm_buf(char *s, void *who_rbase, char *buf) {
    char *sp = s;
    int count = 0;
    sp += (int)who_rbase;
    while(*sp){
        *buf++ = *sp++;
        count++;
    }
        
    *buf = '\0';
    return count;
        // kputc(*sp++);
}

/**
 * print the string to serial port 1
 * @param s 
 */
void kputs(const char *s) {
    while(*s)
        kputc(*s++);
}

#define SPACE    ' '
#define ZERO    '0'

#define PUT_PADDING(_padding_len,_token)\
    while(_padding_len--){    \
        kputc(_token);        \
    }                    \


/**
 * virtual memory printf, this function is used by both kernel and user process
 * @param  format    
 * @param  arg       
 * @param  who_rbase rbase of the calling process
 * @return           number of bytes being printed
 */
int kprintf_vm(const char *format, void *arg, ptr_t *who_rbase){
    char c = *format;
    static char buffer[64];
    char *buf = buffer;
    int padding_len = 0;
    bool right_padding_len;
    int buf_len;
    int count = 0;

    while(*format) {
        if(*format == '%') {
            char prev;
            char token = SPACE;
            format++;
            buf = buffer;
            
            //decode padding options
            if(*format == '-'){
                format++;
                right_padding_len = true;
            }else{
                right_padding_len = false;
            }
            
            if(*format == '*'){
                padding_len = atoi(*(int *)arg);
                arg = ((int*)arg) + 1;
            }else if(*format >= '0' && *format <= '9'){
                *buf++ = *format++;
                *buf++ = *format++;
                *buf = '\0';
                padding_len = atoi(buffer);
                buf = buffer;
            }

            prev = *format;
            switch(*format) {
                
                case 'd':
                    buf_len = kputd_buf(*((int*)arg),buf);
                    arg = ((int*)arg) + 1;
                    format++;
                    break;

                case 'x':
                    buf_len = kputx_buf(*((int*)arg),buf);
                    arg = ((int*)arg) + 1;
                    format++;
                    right_padding_len = false;
                    break;

                case 's':
                    // buf_len = kputs_vm_buf(*(char **)arg,who_rbase,buffer);
                    buf = (*(char **)arg)+ (int)who_rbase;
                    buf_len = strlen(buf);
                    arg = ((char *)arg) + 1;
                    format++;
                    break;

                case 'c':
                    // kputc(*(int *)arg);
                    buffer[0] = *(int *)arg;
                    buffer[1] = '\0';
                    buf_len = 1;
                    arg = ((char *)arg) + 1;
                    format++;
                    break;

                default:
                    kputc(*format++);
            }

            padding_len -= buf_len;
            count += buf_len;

            //left padding
            if(!right_padding_len && padding_len > 0){ 
                if(prev == 'x' || prev == 'd')
                    token = ZERO;
                PUT_PADDING(padding_len,token);
            }
            kputs(buf);
            //right padding
            if(right_padding_len && padding_len > 0){
                PUT_PADDING(padding_len,token);
            }
            padding_len = 0;    
        }
        else {
            //if this is a normal character, simply print it to 
            //serial port 1
            kputc(*format++);
            count++;
        }
    }
    return count;
}

int kprintf(const char *format, ...) {
    void *arg = &format;
    arg = ((char*)arg) + 1;

    return kprintf_vm(format,arg,0);
}

int kinfo(const char *format, ...){
    void *arg = &format;
    arg = ((char*)arg) + 1;

    kputs("[SYSTEM] ");
    return kprintf_vm(format,arg,0);
}

int kdebug(const char *format, ...){
    void *arg = &format;
    arg = ((char*)arg) + 1;

    kputs("[DEBUG ] ");
    return kprintf_vm(format,arg,0);
}


