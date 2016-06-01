static char rcsId[] = "$Id: hashValue_filter.c,v 2.1 1999/12/08 18:09:44 pcd Exp $";
/*
 * hashValue_filter:
 *   Read in a series of lines, each beginning with an account number in
 *   columns 1-19.  Print out those lines whose account numbers are less
 *   than (or equal-to-or-greater-then) a given threshold. Use one of
 *   four hashing algorithms.
 *
 *                       *****  WARNING  *****
 *          This program pays no attention to whether or not an
 *          account is fraud.  This is very different from
 *          sampstats, and is not a replacement for that program.
 *
 *
 * Author: Paul C. Dulany (pcd) 12/8/99
 *
 * Input: (via stdin)
 *   A series of lines (max length = BUFLEN), with the account number in
 *   columns 1-19 (1-KEYLENMAX).
 *
 * Output: (via stdout)
 *   Those input lines whose account numbers satisfy the creieria
 *   from the command line.
 *
 * Algorithms:
 *   Taken out of the sampstats and drv_falcon source code (and
 *   rearranged for readability).
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "hashValue.h"

extern int getopt(int, char *const *, const char *);

#define KEYLENMAX   19
#define BUFLEN    2048

typedef struct {
   int hashType;
   int hashDirection;
   int upperLimit;
   int lowerLimit;
   int quiet;
} options_t;


/******************************************************************************/


static void usage (char *exename)
{
   fprintf (stderr,
            "Usage:  %s [-u <uval>] [-l <lval>] [-t | -s] [-b] [-q]\n"
            "\n"
            "    Read from stdin a series of lines.  Each line begins\n"
            "    with an account number in columns 1-19.\n"
            "\n"
            "    Print out those lines whose account numbers are less\n"
            "    than (or equal-to-or-greater-then) a given threshold.\n"
            "    Use one of four hashing algorithms.\n"
            "\n"
            "    I.e., print out if:         lval  <=  hash  <  uval \n"
            "    Default:                      0   <=  hash  <  100\n"
            "\n"
            "    By default the hashing function is the one used by\n"
            "    sampstats to determine the sampling rate.\n"
            "\n"
            "        -u <uval>  Use <lval> as the upper limit.\n"
            "\n"
            "        -l <lval>  Use <lval> as the lower limit.\n"
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
            "        -q  QUIET!  (Bloody noisy utilities, always telling"
            "            me things I don't want to know.....)\n"
            "\n"
            "\n"
            "                *****  WARNING  *****\n"
            "\n"
            "   This program pays no attention as to whether or not an\n"
            "   account is fraud.  This is very different from\n"
            "   sampstats, and is not a replacement for that program.\n"
            "\n"
            "\n"
            "rcsId = %s\n"
            "rcsId_hashValue = %s\n"
            "rcsId_hashValue_h = %s\n"
            "\n"
            , exename,
            rcsId,
            rcsId_hashValue(),
            rcsId_hashValue_h);
   exit (EXIT_FAILURE);
}


/******************************************************************************/


static options_t getOptions (int argc, char ** argv)
{
   int c;
   options_t returnVal;
   extern char *optarg;

   returnVal.hashType = sampstats;
   returnVal.hashDirection = forward;
   returnVal.upperLimit = 100;
   returnVal.lowerLimit = 0;
   returnVal.quiet = 0;

   while ((c = getopt (argc, argv, "qstbhl:u:")) != EOF) {
      switch (c) {
         case 'u':
            returnVal.upperLimit = (int) strtol (optarg, NULL, 10);
            break;
         case 'l':
            returnVal.lowerLimit = (int) strtol (optarg, NULL, 10);
            break;
         case 's':
            returnVal.hashType = sampstats;
            break;
         case 't':
            returnVal.hashType = testTrain;
            break;
         case 'b':
            returnVal.hashDirection = backward;
            break;
         case 'q':
            returnVal.quiet = 1;
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
   int  hv;
   int  i;
   options_t options;
   size_t inCount;


   if (BUFLEN <= KEYLENMAX) {
      fprintf (stderr, "You compiled me wrong!\n");
      fprintf (stderr, "BUFLEN (%d) <= KEYLENMAX (%d)\n", BUFLEN, KEYLENMAX);
      exit (EXIT_FAILURE);
   }


   /*  Parse and Report Options  */
   options = getOptions (argc, argv);

   if (!options.quiet) {
      if (options.hashType == testTrain)
         fprintf (stderr, "Using TEST/TRAIN hashing function");
      else
         fprintf (stderr, "Using SAMPSTATS hashing function");
      if (options.hashDirection == backward)
         fprintf (stderr, ", backwards");
      fprintf (stderr, "\n");
      fprintf (stderr, "upper Limit = %d\n", options.upperLimit);
      fprintf (stderr, "lower Limit = %d\n", options.lowerLimit);
      if (options.upperLimit <= options.lowerLimit)
         fprintf (stderr, "Warning: The limits need to differ by at least"
                  " one in order to get output!\n");
   }


   /*  Initialize the acct number array  */


   for (i=0; i<KEYLENMAX-1; i++) {
      acct[KEYLENMAX] = ' ';
   }
   acct[KEYLENMAX] = '\0';


   /*  Do the work!  */


   while (fgets (buffer, BUFLEN, stdin) != NULL) {
      strncpy (acct, buffer, KEYLENMAX);

      hv = hashValue(acct, options.hashType, options.hashDirection);
      if (options.lowerLimit <= hv   &&   hv < options.upperLimit)
         fprintf (stdout, "%s", buffer);

      /* Deal with lines longer than BUFLEN */

      inCount = strlen (buffer);
      while (inCount == BUFLEN - 1 && buffer[BUFLEN-1] != '\n') {
         if (fgets (buffer, BUFLEN, stdin) == NULL)
            break;
         if (options.lowerLimit <= hv   &&   hv < options.upperLimit)
            fprintf (stdout, "%s", buffer);
         inCount = strlen (buffer);
      }
   }

   return (0);

}

