#include <sys/syscall.h>

#define ESTR_SIZ    (16)

char estr[ESTR_SIZ];

char* strerror(int usrerrno){
    __strerror(estr, ESTR_SIZ, usrerrno);
    return estr;
}