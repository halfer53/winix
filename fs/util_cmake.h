//
// Created by bruce on 19/04/20.
//

#ifndef FS_UTIL_CMAKE_H
#define FS_UTIL_CMAKE_H

#include <sys/types.h>

void* kmalloc(unsigned int size);
void kfree(void *ptr);

void *memcpy(void *s1, const void *s2, size_t n);
int strcmp(const char *s1, const char *s2);
int strlen(const char *s);
char *strcpy(char *dest,const char *src);
char *strcat(char *dest, const char *src);
char *strchr(char *s, int c);
void *memset(void *dst, int c, size_t n);
int substring(char* buffer,char* original,int start_index,int length);

#endif //FS_UTIL_CMAKE_H
