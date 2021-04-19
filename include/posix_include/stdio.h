/**
 * 
 * A rather limited version of <stdio.h>
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

#ifndef _STDIO_H_
#define _STDIO_H_ 1

typedef struct __iobuf {
	int		_count;
	int		_fd;
	int		_flags;
	int		_bufsiz;
	unsigned char	*_buf;
	unsigned char	*_ptr;
} FILE;

#define	_IOFBF		0x000   // file buffered
#define	_IOREAD		0x001   // can read
#define	_IOWRITE	0x002   // can write
#define _IORW       0x003   // can read and write
#define	_IONBF		0x004   // not buffered
#define	_IOMYBUF	0x008   // custised buffer
#define	_IOEOF		0x010   // end of file is reached
#define	_IOERR		0x020   // IO error
#define	_IOLBF		0x040   // line buffered
#define	_IOREADING	0x080   // reading
#define	_IOWRITING	0x100   // writing
#define	_IOAPPEND	0x200   // can append
#define _IOFIFO		0x400   // not used
#define _IODEFAULT	0x800	// defualt stdin, stdout and stderr


#ifndef SEEK_SET
#define	SEEK_SET	0
#endif
#ifndef SEEK_CUR
#define	SEEK_CUR	1
#endif
#ifndef SEEK_END
#define	SEEK_END	2
#endif


#define	FOPEN_MAX	20

// C89/C99 requires them to be macros, so satisfy them
#define	stdin		stdin
#define	stdout		stdout
#define	stderr		stderr

#define STDIN_FILENO	(0)
#define STDOUT_FILENO	(1)
#define STDERR_FILENO	(2)

#define	BUFSIZ		1024
#ifndef NULL
#define	NULL		((void *)0)
#endif
#define	EOF		(-1)


#include <sys/dirent.h>
#define	FILENAME_MAX	DIRSIZ

#define	TMP_MAX		999
#define	L_tmpnam	(sizeof("/tmp/") + FILENAME_MAX)
#define __STDIO_VA_LIST__	void *

typedef long int	fpos_t;

#ifndef _SIZE_T
#define	_SIZE_T
typedef unsigned int	size_t;		/* type returned by sizeof */
#endif /* _SIZE_T */

int printf(const char *format, ...);
int dprintf(int fd, const char *format, ...);
int fprintf(FILE *stream, const char *format, ...);
int putchar(const char c);

FILE *fopen(const char *pathname, const char *mode);
int fclose(FILE *stream);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
int getline(char *buf, int size);
#define getchar()	(getc(stdin))


extern FILE	*__iotab[FOPEN_MAX];
extern FILE	*stdin, *stdout, *stderr;

#include <sys/syscall.h>

#endif
