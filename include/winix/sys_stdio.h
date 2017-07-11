#ifndef _W_SYS_STDIO_H_
#define _W_SYS_STDIO_H_ 1

#ifndef _GCC_DEBUG
int kputc(const int c);
int kgetc();
void kprintf(const char *format, ...);
#endif

#endif
