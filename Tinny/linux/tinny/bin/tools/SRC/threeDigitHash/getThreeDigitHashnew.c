static char rcsId_getHashValues[] = "$Id: getThreeDigitHash.c,v 1.1 2003/10/20 17:28:11 pcd Exp $";
/*
 * hashvalue:
 *   Print the hash value for an account number.  Co-primes determined
 *   specifically for Visa EU.
 *
 * Author: Paul C. Dulany (12/8/99)
 *
 * Input: (via stdin)
 *   A series of lines (max length = BUFLEN), with the account number in
 *   columns 1-19 (1-KEYLENMAX).
 *
 * Output: (via stdout)
 *   A number between 0 and 999.  One output line per input line.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

extern int getopt(int, char *const *, const char *);
extern int isAccountInSample (char* acctNum, float rate);
extern int threeDigitHashValue(char* acctNum);

#define KEYLENMAX   19
#define BUFLEN    (4*1024)

/******************************************************************************/


static void usage (char *exename)
{
    fprintf (stderr,
             "Usage:  %s [-h] [-l]\n"
             "\n"
             "    Read from stdin a series of lines.  Each line begins\n"
             "    with an account number in columns 1-19.\n"
             "\n"
             "    Output:\n"
             "       a number between 0 and 999 in columns 1-4 (the Visa hash number)\n"
             "       the original line in columns 11 onward (if -l option)\n"
             "\n"
             "    The number is based upon a hashing function for Visa EU.\n"
             "    Now extended to non-numeric encryption.\n"
             "\n"
             "    -l  Include original line in output\n"
             "\n"
             "rcsId_getHashValues = %s\n"
             "\n",
             exename,
             rcsId_getHashValues);

    exit (EXIT_FAILURE);
}


/******************************************************************************/


    int
main (int argc, char **argv)
{
    char acct[KEYLENMAX+1];
    char buffer[BUFLEN];
    char *line;
    int  c;
    int  i;
    int hash;
    int outputLine = 0;
    unsigned long numLines = 0;

    /* perse options */
    while ((c = getopt (argc, argv, "lh")) != EOF) {
        switch (c) {
            case 'l':
                outputLine = 1;
                break;
            case 'h':
            default:
                fprintf (stderr, "ERROR: unknown options %c\n", (char) c);
                usage (argv[0]);
                break;
        }
    }
    if (outputLine)
        line = buffer;
    else
        line = "\n";


    /* do the work! */
    for (i=0; i<=KEYLENMAX-1; i++) {
        acct[i] = ' ';
    }
    acct[KEYLENMAX] = '\0';

    while (fgets (buffer, BUFLEN, stdin) != NULL) {
        numLines ++;
        strncpy (acct, buffer, KEYLENMAX);
        hash = threeDigitHashValue(acct);
        if (hash > 1000) {
            fprintf (stderr, "Error %d on line %lu: %s\n", hash/1000, numLines, buffer);
            hash %= 1000;
        }
        fprintf (stdout, "%4d      %s", hash, line);
    }

    return (0);

}


