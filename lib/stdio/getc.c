#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int fgetc(FILE* stream){
    int ret;
    unsigned char* buf_end;
    if(stream->_flags & _IOEOF)
        return EOF;

    if(!stream->_buf){
        stream->_buf = malloc(FILE_BUFFER_SIZ);
        stream->_bufsiz = FILE_BUFFER_SIZ;
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