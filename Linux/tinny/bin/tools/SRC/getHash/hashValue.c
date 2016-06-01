static char rcsId[] = "$Id: hashValue.c,v 2.3 2003/06/03 15:27:10 pcd Exp $";
/*
 * hashvalue:
 *   A subroutine that returns the hash value for an account number.
 *   Value is based upon one of four possible hashing functions
 *   currently in use in Falcon.
 *
 * Author: Paul C. Dulany (pcd) 12/8/99
 *
 * Algorithms:
 *   Taken out of the sampstats and drv_falcon source code (and
 *   rearranged for readability).
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "hashValue.h"

#define KEYLENMAX   19
enum {
    MAX_HYPHEN_WARNINGS = 1
};

static int coprime_drv_falcon[] = {11,13,17,19,23,31};
static int coprime_sampstats[] =  {33,47,53,17,27,41};

/******************************************************************************/

char * rcsId_hashValue()
{
   return rcsId;
}

/******************************************************************************/

int hashValue(char* acctNum, int hashType, int hashDirection)
{
   int i, j, k, sum=0;
   int tmp;
   int *coprime;
   int pivot;
   static int warnedAbout12CharAcct = 0;
   static int warnedAboutHyphen = 0;

   if (hashType == testTrain) 
      coprime = coprime_drv_falcon;
   else
      coprime = coprime_sampstats;

   for (i = KEYLENMAX - 1; !isalnum((int) acctNum[i]) && acctNum[i] != '-'; i--)
      ;

   /* 
      Code to handle different account number lengths gracefully
      13 or larger are the default case
      12 is a special case for a special client
      11 and under is an error, cannot process account 
      Note i is one shorter than the length.
   */
   if(i>11) {
       pivot = 12;
   } else if(i==11) {
       pivot = 11;
       if (!warnedAbout12CharAcct) {
           warnedAbout12CharAcct = 1;
           fprintf(stderr,"Account number found of length only %d"
                   " in hashValue!\n", i+1);
           fprintf(stderr,"        Only warning once...\n");
       }
   } else  {
      /* if(i<12) { */
      fprintf(stderr,"Account number found of length only %d"
              " in hashValue!\n", i+1);
      return(-1);
   }


   tmp = i;
   for (k=0, j=tmp-pivot; k<6; tmp--, j++, k++) {
      if (!isalnum((int) acctNum[tmp])) {
         if (acctNum[tmp] == '-') {
            if (warnedAboutHyphen < MAX_HYPHEN_WARNINGS) {
               warnedAboutHyphen++;
               fprintf (stderr, "Warning: hyphen characters in acct number: only warning %d time(s)\n",
                        MAX_HYPHEN_WARNINGS);
            }
         } else {
            fprintf (stderr, "Warning: character %c at %d is not alpha-numeric\n",
                     acctNum[tmp], tmp+1);
         }
      }
      if (!isalnum((int) acctNum[j])) {
         if (acctNum[j] == '-') {
            if (warnedAboutHyphen < MAX_HYPHEN_WARNINGS) {
               warnedAboutHyphen++;
               fprintf (stderr, "Warning: hyphen characters in acct number: only warning %d time(s)\n",
                        MAX_HYPHEN_WARNINGS);
            }
         } else {
            fprintf (stderr, "Warning: character %c at %d is not alpha-numeric\n",
                     acctNum[j], j+1);
         }
      }
   }


   if (hashDirection == forward) {
      for(k=0, j=i-pivot; k<6; i--, j++, k++)
        sum += (((int) (acctNum[j] - '0') * 10) +
                 (int) (acctNum[i] - '0')
               ) * coprime[k];
   } else {
      for(k=0, j=i-pivot; k<6; i--, j++, k++)
        sum += (((int) (acctNum[i] - '0') * 10) +
                 (int) (acctNum[j] - '0')
               ) * coprime[k];
   }

   return(sum%100);
}


/******************************************************************************/
