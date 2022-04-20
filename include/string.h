/**
 * 
 * A rather limited version of <string.h>
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2016-09-19
 * 
*/
#ifndef _STRING_H_
#define _STRING_H_ 1

void *memcpy(void *dst, const void *src, size_t n);
int strcmp(const char *s1, const char *s2);
int strlen(const char *s);
char *strlcpy(char *dest, const char *src, size_t n);
char *strlcat(char *dest, const char *src, size_t n);
int strncmp(const char *s1, const char *s2, size_t size);
void *memset(void *dst, int c, size_t n);
int substring(char* buffer,char* original,int start_index,int length);
char *index(const char *string, int c);
char *strstr(const char *haystack, const char *needle);
size_t strcspn(const char *s1, const char *s2);
size_t strspn(const char *s1, const char *s2);
void *memmove(void *dest, const void *src, size_t n);

#define strchr(s, c)    index(s, c)

#endif
