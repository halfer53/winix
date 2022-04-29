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

#include <fs/filp.h>
#include <winix/rex.h>

const char* kstr_error(int err);

int kputd_buf(int n, char *buf);
int kputx_buf(int n,char *buf);
int kprintf_vm( struct filp* dev, const char *orignal_format, void *arg, struct proc* who);
int kputc(const int c);
int kgetc_blocking(struct proc* who);

int filp_kprint(struct filp* dev, const char* format, ...) CHECK_EPRINTF;
int kprintf(const char *format, ...) CHECK_PRINTF;
int kprintf2(const char *format, ...) CHECK_PRINTF;
int kerror(const char *format, ...) CHECK_PRINTF;
int klog(const char *format, ...) CHECK_PRINTF;
int kwarn(const char *format, ...) CHECK_PRINTF;

int kputs(const char *s);
int tty_write_rex(RexSp_t* rex, char* data, size_t len);

#endif
