#include <lib.h>

extern const char *_errlist[_NERROR];
void perror(const char *s){
    if(s)
        printf("%s",s);
    
    if(errno > 0 && errno < _NERROR)
        printf(" %s\n",_errlist[errno]);
}

int putchar(const char c){
    if(printf("%c",c))
        return c;
    return EOF;
}
