#include <sys/types.h>

#define MEM_SIZE (5 * 1024 * 1024)
char mem[MEM_SIZE];
int curr;

void* kmalloc(unsigned int size){
    void* ret = &mem[curr];
    curr += size;
    return ret;
}
void kfree(void *ptr){

}

// note hexstr must use capital letters
int hexstr2int(char *a, int len)
{
    int i;
    int val = 0;

    for(i=0;i<len;i++){
        if(a[i] <= 57)
             val += (a[i]-48)*(1<<(4*(len-1-i)));
            else
             val += (a[i]-55)*(1<<(4*(len-1-i)));
        }

    return val;
}

char hexstr2char(char A){
    return (char)(A > '9')? (A &~ 0x20) - 'A' + 10: (A - '0');
}

// bytenr number of bytes in n
void int2hexstr(char *buffer,int n, int bytenr) {
    int i;
    for(i = bytenr * 4 - 4; i >= 0; i -= 4) {
        int d = (n >> i) & 0xf;
        if(d < 10) {
            *buffer++ = d + '0';
        }
        else {
            *buffer++ = d - 10 + 'A';
        }
    }
}
//
//void *memcpy(void *s1, const void *s2, size_t n)
//{
//    char *p1 = s1;
//    const char *p2 = s2;
//
//    if (n) {
//        n++;
//        while (--n > 0) {
//            *p1++ = *p2++;
//        }
//    }
//    return s1;
//}
//
///**
// * Compares two strings.
// **/
//int strcmp(const char *s1, const char *s2) {
//    while (*s1 && *s2) {
//        if (*s1 != *s2) {
//            break;
//        }
//        s1++;
//        s2++;
//    }
//
//    return *s1 - *s2;
//}
//
///**
// * Calculates the length of a string.
// **/
//int strlen(const char *s) {
//    const char* bak = s;
//    while (*s++);
//    return s - bak;
//}
//
///**
// * copy the src into the destination string, the destination should be large enough to hold the src
// **/
//char *strcpy(char *dest, const char *src) {
//    char *saved = dest;
//    while (*dest++ = *src++);
//    *dest = 0;
//    return saved;
//}
//
//char *strncpy(char *dest, const char *src, size_t n){
//    char *saved = dest;
//    while (*src && n--) {
//        *dest++ = *src++;
//    }
//    *dest = 0;
//    return saved;
//}
//
//char *strcat(char *dest, const char *src) {
//    char* saved = dest;
//    while(*dest++);
//    dest--;
//    while(*dest++ = *src++);
//    *dest = '\0';
//    return saved;
//}
//
//char *strchr(char *s, int c){
//    while(*s && *s != c)
//        s++;
//    return s;
//}
//
//void *memset(void *dst, int c, size_t n)
//{
//    if (n) {
//        char *d = dst;
//
//        do {
//            *d++ = c;
//        } while (--n);
//    }
//    return dst;
//}
//
///**
// * return the length of substring of the original string, starting at start_index, and has specified length
// **/
//int substring(char* buffer, char* original, int start_index, int length) {
//    int i = 0;
//    int count = 0;
//    for (i = start_index; i < length + start_index; i++)
//    {
//        buffer[count] = original[i];
//        count++;
//    }
//    buffer[count] = '\0';
//    return count;
//}

