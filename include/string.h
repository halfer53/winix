/**
 * 
 * A rather limited version of <string.h>
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * @create date 2016-09-19
 * @modify date 2017-08-23 06:04:27
*/
#ifndef _STRING_H_
#define _STRING_H_ 1

void *memcpy(void *s1, const void *s2, register size_t n);
int strcmp(const char *s1, const char *s2);
int strlen(const char *s);
char *strcpy(char *dest,const char *src);
char *strcat(char *dest, const char *src);
void *memset(void *dst, int c, size_t n);
int Substring(char* buffer,char* original,int start_index,int length);

#endif
