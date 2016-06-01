static char rcsId_getHashValues[] = "$Id: getHashValues.c,v 2.1 1999/12/08 18:09:44 pcd Exp $";
/*
 * hashvalue:
 *   Print the hash value for an account number.  Value is based upon
 *   one of four possible hashing functions currently in use in Falcon.
 *
 * Author: Paul C. Dulany (12/8/99)
 *
 * Input: (via stdin)
 *   A series of lines (max length = BUFLEN), with the account number in
 *   columns 1-19 (1-KEYLENMAX).
 *
 * Output: (via stdout)
 *   A number between 0 and 99.  One output line per input line.
 *
 * Algorithms:
 *   Taken out of the sampstats and drv_falcon source code (and
 *   rearranged for readability).  In files hashValue.c/hashValue.h.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "hashValue.h"
extern int getopt(int, char *const *, const char *);

#define KEYLENMAX   19
#define BUFLEN    1024

typedef struct {
   int hashType;
   int hashDirection;
} options_t;


/******************************************************************************/


static void usage (char *exename)
{
   fprintf (stderr,
            "Usage:  %s [-t | -s] [-b]\n"
            "\n"
            "    Read from stdin a series of lines.  Each line begins\n"
            "    with an account number in columns 1-19.\n"
            "\n"
            "    Output a number between 0 and 99.\n"
            "\n"
            "    The number is based upon a hashing function: by default\n"
            "    the function used by sampstats to determine the\n"
            "    sampling rate.\n"
            "\n"
            "        -t  Calculate based upon the drv_falcon hash\n"
            "            function for the TEST/TRAIN/VALIDATE split.\n"
            "\n"
            "        -s  Calculate based upon the sampstats hash\n"
            "            function for reducing the number of accouts\n"
            "            in a sample (** default **).\n"
            "\n"
            "        -b  Perform a backwards hash.  (Modifies the above\n"
            "            algorithms.  See dkg.)\n"
            "\n"
            "rcsId_getHashValues = %s\n"
            "rcsId_HashValue = %s\n"
            "rcsId_HashValue_h = %s\n"
            "\n",
      exename,
      rcsId_getHashValues,
      rcsId_hashValue(),
      rcsId_hashValue_h);

   exit (EXIT_FAILURE);
}


/******************************************************************************/


static options_t getOptions (int argc, char ** argv)
{
   int c;
   options_t returnVal;

   returnVal.hashType = sampstats;
   returnVal.hashDirection = forward;

   while ((c = getopt (argc, argv, "stbh")) != EOF) {
      switch (c) {
         case 's':
            returnVal.hashType = sampstats;
            break;
         case 't':
            returnVal.hashType = testTrain;
            break;
         case 'b':
            returnVal.hashDirection = backward;
            break;
         case 'h':
         default:
            fprintf (stderr, "ERROR: unknown options %c\n", (char) c);
            usage (argv[0]);
            break;
      }
   }

   return returnVal;
}
            


/******************************************************************************/


int
main (int argc, char **argv)
{
   char acct[KEYLENMAX+1];
   char buffer[BUFLEN];
   int  i;
   options_t options;

   /* 
   while ( fscanf (stdin, "%s", acct) != EOF )
      printf ("%s = %d, %d, %d, %d\n", acct,
              hashValue(acct, testTrain, forward),
              hashValue(acct, testTrain, backward),
              hashValue(acct, sampstats, forward),
              hashValue(acct, sampstats, backward));
   */

   options = getOptions (argc, argv);

   for (i=0; i<KEYLENMAX-1; i++) {
      acct[KEYLENMAX] = ' ';
   }
   acct[KEYLENMAX] = '\0';

   while (fgets (buffer, BUFLEN, stdin) != NULL) {
      strncpy (acct, buffer, KEYLENMAX);
      fprintf (stdout, "%4d\n",
               hashValue(acct, options.hashType, options.hashDirection));
   }

   return (0);

}


