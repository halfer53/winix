/* wc - count lines, words and characters	Author: david Messer */

#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/syscall.h>
#define isdigit(c) (c >= '0' && c <= '9')
#define isspace(c) (c == ' ' || c == '\t' || c == '\n')
/*
 *
 *	Usage:  wc [-lwc] [names]
 *
 *		Flags:
 *			l - count lines.
 *			w - count words.
 *			c - count characters.
 *
 *		Flags l, w, and c are default.  
 *		Words are delimited by any non-alphabetic character.
 *
 *  Released into the PUBLIC-dOMAIN 02/10/86
 *
 *	If you find this program to be of use to you, a donation of
 *	whatever you think it is worth will be cheerfully accepted.
 *
 *	Written by: david L. Messer
 *				P.O. Box 19130, Mpls, MN,  55119
 *      Program (heavily) modified by Andy Tanenbaum 
 */

int lflag = 0; /* Count lines */
int wflag = 0; /* Count words */
int cflag = 0; /* Count characters */

long lcount; /* Count of lines */
long wcount; /* Count of words */
long ccount; /* Count of characters */

long ltotal; /* Total count of lines */
long wtotal; /* Total count of words */
long ctotal; /* Total count of characters */

void count()
{
    register int c;
    register int word = 0;

    lcount = 0;
    wcount = 0;
    ccount = 0L;

    while ((c = getc(stdin)) > 0)
    {
        ccount++;

        if (isspace(c))
        {
            if (word)
                wcount++;
            word = 0;
        }
        else
        {
            word = 1;
        }

        if (c == '\n' || c == '\f')
            lcount++;
    }
    ltotal += lcount;
    wtotal += wcount;
    ctotal += ccount;
}

void usage()
{
    fprintf(stderr, "Usage: wc [-lwc] [name ...]\n");
    exit(1);
}

int main(int argc, char *argv[]) 
{
    int k;
    char *cp;
    int tflag, files;
    int i;
    int fd;

    /* Get flags. */
    files = argc - 1;
    k = 1;
    cp = argv[1];

    

    if (cp && *cp++ == '-')
    {
        files--;
        k++; /* points to first file */
        while (*cp != 0)
        {
            switch (*cp)
            {
            case 'l':
                lflag++;
                break;
            case 'w':
                wflag++;
                break;
            case 'c':
                cflag++;
                break;
            default:
                usage();
            }
            cp++;
        }
    }

    /* If no flags are set, treat as wc -lwc. */
    if (!lflag && !wflag && !cflag)
    {
        lflag = 1;
        wflag = 1;
        cflag = 1;
    }

    // printf("argc %6d flags %6d %6d %6d\n", argc, lflag, wflag, cflag);

    /* Process files. */
    tflag = files >= 2; /* set if # files > 1 */

    /* Check to see if input comes from std input. */
    if (k >= argc)
    {
        count();
        if (lflag)
            printf(" %6d", lcount);
        if (wflag)
            printf(" %6d", wcount);
        if (cflag)
            printf(" %6d", ccount);
        printf(" \n");
        exit(0);
    }

    /* There is an explicit list of files.  Loop on files. */
    while (k < argc)
    {
        // fclose(stdin);
        close(STDIN_FILENO);
        if ((fd = open(argv[k], O_RDONLY)) == -1)
        {
            fprintf(stderr, "wc: cannot open %s\n", argv[k]);
            k++;
            continue;
        }
        else
        {
            /* Next file has been opened as std input. */
            count();
            if (lflag)
                printf(" %6d", lcount);
            if (wflag)
                printf(" %6d", wcount);
            if (cflag)
                printf(" %6d", ccount);
            printf(" %s\n", argv[k]);
        }
        k++;
    }

    if (tflag)
    {
        if (lflag)
            printf(" %6d", ltotal);
        if (wflag)
            printf(" %6d", wtotal);
        if (cflag)
            printf(" %6d", ctotal);
        printf(" total\n");
    }

    return 0;
}
