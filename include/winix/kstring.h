#ifndef _WINIX_KSTRING_H_
#define _WINIX_KSTRING_H_

#include <sys/types.h>

void *kmemcpy(void *s1, const void *s2, size_t n);
int kstrcmp(const char *s1, const char *s2);
int kstrlen(const char *s);
char *kstrcpy(char *dest,const char *src);
char *kstrcat(char *dest, const char *src);
char *kstrchr(char *s, int c);
void *kmemset(void *dst, int c, size_t n);
int ksubstring(char* buffer,char* original,int start_index,int length);

#define KSTRCPY(dest, src) \
do{\
    while ((*(dest)++ = *(src)++));\
    *(dest) = 0;\
}while(0)\

#endif
