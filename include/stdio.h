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

#include <stddef.h>
#include <sys/syscall.h>
#include <sys/compiler.h>

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

#define FILE_BUFFER_SIZ  (500)

#ifndef SEEK_SET
#define	SEEK_SET	0
#endif
#ifndef SEEK_CUR
#define	SEEK_CUR	1
#endif
#ifndef SEEK_END
#define	SEEK_END	2
#endif


#define	FOPEN_MAX	8

extern FILE	_stdin, _stdout, _stderr;
#define	stdin		(&_stdin)
#define	stdout		(&_stdout)
#define	stderr		(&_stderr)

#define STDIN_FILENO	(0)
#define STDOUT_FILENO	(1)
#define STDERR_FILENO	(2)

#define	BUFSIZ		1024
#define	EOF		(-1)


#include <sys/dirent.h>
#define	FILENAME_MAX	DIRSIZ

#define	TMP_MAX		999
#define	L_tmpnam	(sizeof("/tmp/") + FILENAME_MAX)
#define __STDIO_VA_LIST__	void *

typedef long int	fpos_t;

int putchar(int c);

FILE *fopen(const char *pathname, const char *mode);
int fclose(FILE *stream);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
int getline(char *buf, int size);
char *fgets(char *str, int n, FILE *stream);
int dprintf(int fd, const char *format, ...) CHECK_EPRINTF;
int fprintf(FILE *stream, const char *format, ...) CHECK_EPRINTF;
int printf(const char *format, ...) CHECK_PRINTF;
void perror();


ssize_t getdelim(char **lineptr, size_t *n, int delimiter, FILE *stream);
#define getline(lineptr, n, stream)		getdelim(lineptr, n, '\n', stream)

int fgetc(FILE* stream);
#define getc(stream)	fgetc(stream)
#define getchar()		fgetc(stdin)


extern FILE	*__iotab[FOPEN_MAX];

#if defined(__wramp__) & !defined(LINTING) && !defined(_SYSTEM)

#define fprintf(stream, format, ...)        dprintf(stream->_fd, format, ##__VA_ARGS__)
#define printf(format, ...)                 dprintf(STDOUT_FILENO, format, ##__VA_ARGS__)
#endif

#endif
