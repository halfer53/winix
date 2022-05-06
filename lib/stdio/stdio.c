#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>





FILE *fopen(const char *pathname, const char *mode){
    FILE* f = (FILE*)malloc(sizeof(FILE));
    if(!f)
        return f;
    f->_fd = open(pathname, O_RDWR | O_CREAT);
    if(f->_fd == -1)
        goto err;
    f->_buf = malloc(FILE_BUFFER_SIZ);
    if(!f->_buf)
        goto err_buffer;
    f->_bufsiz = FILE_BUFFER_SIZ;
    return f;
    
err_buffer:
    free(f->_buf);
err:
    free(f);
    return NULL;
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream){
    size_t len = size * nmemb;
    return read(stream->_fd, ptr, len);
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream){
    size_t len = size * nmemb;
    return write(stream->_fd, ptr, len);
}

int fclose(FILE *stream){
    int ret = close(stream->_fd);
    if(stream->_buf){
        free(stream->_buf);
    }
    if((stream->_flags & _IODEFAULT) ^ _IODEFAULT)
        free(stream);
    return ret;
}

// void debug_file(FILE* stream){
//     printf("FILE fd %d count %d flag %x bufsiz %d\n", stream->_fd, stream->_count, stream->_flags, stream->_bufsiz);
// }






