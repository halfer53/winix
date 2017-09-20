#include <lib.h>

extern const char *_sys_errlist[_NERROR];
void perror(const char *s){
    if(s)
        printf("%s",s);
    
    if(errno > 0 && errno < _NERROR)
        printf(" E%s",_sys_errlist[errno]);

    putchar('\n');
}

int putchar(const char c){
    if(printf("%c",c))
        return c;
    return EOF;
}
