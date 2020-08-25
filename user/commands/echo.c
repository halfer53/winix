#include <stdio.h>
#include <sys/syscall.h>
#include <sys/fcntl.h>
#include <string.h>

int main(int argc, char *argv[]){
    char *p, *dest, *dest_bak;
    int len;
    char c;
    if(argc < 2)
        return -1;
    p = argv[1];
    len = strlen(p);
    dest_bak = sbrk(len);
    dest = dest_bak;
    while(*p){
        c = *p;
        if(c == '\\'){
            c = *++p;
            switch (c)
            {
            case 'n':
                c = '\n';
                break;
            case 't':
                c = '\t';
                break;
            
            default:
                break;
            }
            
        }
        *dest++ = c;
        p++;
    }
    *dest = '\0';
    printf("%s", dest_bak);
    return 0;
}



