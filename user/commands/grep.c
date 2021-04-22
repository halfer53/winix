/* grep - search for a pattern 		Author: Martin C. Atkins */

/*
 *	Search files for a regular expression
 *
 *<-xtx-*>cc -o grep grep.c -lregexp
 */

/*
 *	This program was written by:
 *		Martin C. Atkins,
 *		University of York,
 *		Heslington,
 *		York. Y01 5DD
 *		England
 *	and is released into the public domain, on the condition
 *	that this comment is always included without alteration.
 *
 *	The program was modified by Andy Tanenbaum.
 */

#include <regexp.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <string.h>

void match(char *name, regexp *exp);
void pline(char *name, int lineno, char buf[]);
int getline(char *buf, int size);
void usage();

void regdump(regexp *s);

#define MAXLINE (101)
int status = 1;
char *progname;
int pmflag = 1;	 /* print lines which match */
int pnmflag = 0; /* print lines which don't match */
int nflag = 0;	 /* number the lines printed */
int args;

int main(argc, argv) int argc;
char *argv[];
{
	regexp *exp;
	char **argp = &argv[1];
	int fd;

	args = argc;
	progname = argv[0];
	while (*argp != 0 && argp[0][0] == '-')
	{
		args--; /* flags don't count */
		switch (argp[0][1])
		{
		case 'v':
			pmflag = 0;
			pnmflag = 1;
			break;
		case 'n':
			nflag++;
			break;
		case 's':
			pmflag = pnmflag = 0;
			break;
		case 'e':
			argp++;
			goto out;
		default:
			usage();
		}
		argp++;
	}
out:
	if (*argp == 0)
		usage();

	// printf("flags pm %d pnm %d n %d \n", pmflag, pnmflag, nflag);

	if ((exp = regcomp(*argp++)) == NULL)
	{
		fprintf(stderr, "grep: regcomp failed\n");
		exit(2);
	}

	if (*argp == 0){
		match((char *)0, exp);
	}else{
		while (*argp)
		{
			int infd;

			if (strcmp(*argp, "-") == 0)
				match("-", exp);
			else
			{
				close(STDIN_FILENO);
				if ((fd = open(*argp, O_RDONLY)) == -1)
				{
					fprintf(stderr, "Can't open %s\n", *argp);
					status = 2;
				}
				else
				{
					// printf("opened %s in %d\n", *argp, fd);
					match(*argp, exp);
					close(infd);
				}
			}
			argp++;
		}
	}
	return status;
}

/*
 *	This routine actually matches the file
 */
void match(char *name, regexp *exp) 
{
	char buf[MAXLINE];
	int lineno = 0, ret, lineret;

	// printf("match %s\n", name);
	// enable_syscall_tracing();
	while ((lineret = getline(buf, MAXLINE)) != -1)
	{
		char *cr = index(buf, '\n');
		lineno++;
		if (cr == 0)
		{
			fprintf(stderr, "Line too long in ");
			fprintf(stderr, name == 0 ? "stdin" : name);
		}
		else{
			*cr = '\0';
		}
			
		ret = regexec(exp, buf);
		if (ret)
		{
			if (pmflag)
				pline(name, lineno, buf);
			if (status != 2)
				status = 0;
		}
		else if (pnmflag){
			pline(name, lineno, buf);
		}
		// printf("regexec %s ret %d\n", buf, ret);
	}
	// printf("line ret %d\n", lineret);
}

void pline(char *name, int lineno, char buf[])
{
	if (name && args > 3)
		printf("%s:", name);
	if (nflag)
		printf("%d:", lineno);
	printf("%s\n", buf);
}

void usage()
{
	fprintf(stderr, "Usage: grep [-v] [-n] [-s] [-e expr] expression [file ...]\n");
	exit(2);
}

int getline(char *buf, int size)
{
	char *initbuf = buf, c;

	while (1)
	{
		c = getc(stdin);
		// printf("read %d %c\n", c, c);
		*buf++ = c;
		if (c < 0)
			return (-1);
		if (((buf - initbuf) == size - 1) || c == '\n'){
			*buf = '\0';
			return buf - initbuf;
		}
	}
	return -1;
}

