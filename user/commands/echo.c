#include <stdio.h>
#include <fcntl.h>

static char buffer[512];

int main(int argc, char *argv[]){
    char *p, *dest;
    char c;
    int i = 1;
    if(argc < 2)
        return 1;
    dest = buffer;
    while (i < argc){
        p = argv[i];
        if (i > 1)
            *dest++ = ' ';
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
        i++;
    }
    *dest++ = '\n';
    printf("%s", buffer);
    return 0;
}



