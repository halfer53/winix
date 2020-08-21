#include <lib.h>

#define BUFFER_SIZ  (16)

FILE _stdin = {0, 0, 0, 0 , 0, 0};
FILE _stdout = {0, 1, 0, 0 , 0, 0};
FILE _stderr = {0, 2, 0, 0 , 0, 0};
FILE *stdin = &_stdin, *stdout = &_stdout, *stderr = &_stderr;

int putchar(const char c){
    if(printf("%c",c))
        return c;
    return EOF;
}

// void debug_file(FILE* stream){
//     printf("FILE fd %d count %d flag %x bufsiz %d\n", stream->_fd, stream->_count, stream->_flags, stream->_bufsiz);
// }

int getc(FILE* stream){
    int ret;
    unsigned char* buf_end, *buf_count_end;
    if(stream->_flags & _IOEOF)
        return EOF;

    if(!stream->_buf){
        stream->_buf = sbrk(BUFFER_SIZ);
        stream->_bufsiz = BUFFER_SIZ;
    }

    buf_end = stream->_buf + stream->_count;
    if(!(stream->_ptr) || stream->_ptr >= buf_end){

        ret = read(stream->_fd, stream->_buf, stream->_bufsiz);
        if(ret <= 0){
            stream->_flags |= _IOEOF;
            return EOF;
        }
        stream->_count = ret;
        stream->_ptr = stream->_buf;
    }

    return *stream->_ptr++;
}


