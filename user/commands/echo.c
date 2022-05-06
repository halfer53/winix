#include <stdio.h>
#include <fcntl.h>

static char buffer[128];

int main(int argc, char *argv[]){
    char *p, *dest;
    char c;
    if(argc < 2)
        return 1;
    p = argv[1];
    dest = buffer;
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
    printf("%s", buffer);
    return 0;
}



