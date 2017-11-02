/**
 * 
 * kernel stdio
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:22:37
 * 
*/
#ifndef _W_SYS_STDIO_H_
#define _W_SYS_STDIO_H_ 1

#ifndef _GCC_DEBUG
int kputc(const int c);
int kgetc(struct proc* who);
int kprintf(const char *format, ...);
int kerror(const char *format, ...);
int kinfo(const char *format, ...);
int kdebug(const char *format, ...);
#endif

#endif
