/*
 *
 * Proprietary and Confidential
 *
 * (c) Copyright 2002 Fair, Isaac and Company, Inc.  All rights reserved.
 *
 * 1.	Grant of License.  The parties agree to add the following
 * license:
 * 
 * "Grant of License to C Code.  Subject to the terms and conditions of
 * the Agreement, Fair, Isaac hereby grants to Visa, a revocable,
 * nonexclusive, transferable, non-sublicenseable, limited copyright
 * license to perform and display the C Code, in source code form, only
 * on the Designated System identified in the Agreement; only for the
 * benefit of Visa and Visa Members; only during the Implementation
 * Services; only for the purpose of performing the Implementation
 * Services as described herein."
 * 
 * 2.	Warranty.  The parties agree that the conformity warranty as
 * described in Section 11.5 of the Agreement shall not apply to the C
 * Code and the following shall be applicable to the C Code license
 * grant:
 * 
 * "FAIR, ISAAC FURNISHES THIS CODE TO CUSTOMERS "AS-IS", WITHOUT
 * WARRANTY OF ANY KIND.  FAIR, ISAAC DISCLAIMS ALL WARRANTIES, EXPRESS
 * AND IMPLIED, INCLUDING ANY WARRANTY OF NON-INFRINGEMENT, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
 * THOSE ARISING FROM A COURSE OF PERFORMANCE, A COURSE OF DEALING, OR
 * TRADE USAGE.  FAIR, ISAAC WILL NOT HAVE ANY OBLIGATION OR BE LIABLE
 * FOR ANY ERROR, OMISSION, DEFECT, DEFICIENCY, OR NONCONFORMITY IN THE
 * CODE."
 * 
 */

/******************************************************************************/

/*
 * isAccountInSample (char *acctNum, float rate):
 *
 *   A subroutine which returns 0 if the account number is not in the
 *   sample and a 1 if it is in the sample.  Please note that this is
 *   only for non-fraud accounts --- all fraud accounts should be
 *   included regardless of what this function returns.  This function
 *   calls threeDigitHashValue (char *acctNum).
 *
 *   Input:
 *
 *      acctNum:
 *         A pointer to a char array holding the account number.  The
 *         array of char's must contain at least KEYLENMAX characters,
 *         with the account number left justified and padded to
 *         KEYLENMAX characters using blanks (spaces) on the right.  The
 *         account number must consist of graphic characters only.
 *
 *      rate:
 *         A float containing a number between 0 and 1, inclusive.
 *         Please note that the function only has precision to one part
 *         in 1000.  For example, to have a 12.5 percent sample, use
 *         0.125 for the rate.  (Also, there is no difference between
 *         0.1255 and 0.1256.)
 *
 *
 *
 * threeDigitHashValue (char *acctNum):
 *
 *   A subroutine that returns the hash value for an account number.
 *   The value is between 0 and 999, inclusive.
 *
 *   Input:
 *
 *      acctNum:
 *         A pointer to a char array holding the account number.  The
 *         array of char's must contain at least KEYLENMAX characters,
 *         with the account number left justified and padded to
 *         KEYLENMAX characters using blanks (spaces) on the right.  The
 *         account number must consist of graphic characters only.
 *
 *
 *
 * Note:
 *    In cases of discrepancies between the comments and the code, THE
 *    CODE TAKES PRECEDENCE.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#define KEYLENMAX   19
#define RESOLUTION  1000 /* DO NOT CHANGE */

int threeDigitHashValue(char* acctNum);

/******************************************************************************/

int isAccountInSample (char *acctNum, float rate)
{
    return (threeDigitHashValue (acctNum) < rate * RESOLUTION);
}

/******************************************************************************/

int threeDigitHashValue(char* acctNum)
{
    int i, j, k, sum=0;
    int coprime[] =  {139 * 2,191,967,271,881,587};  /* DO NOT CHANGE */
    int pivot;


    /* Find the last graphic character in the account number */
    for (i = KEYLENMAX - 1; !isgraph ((int) acctNum[i]); i--)
        ;

    /*
     * Ensure that the account number is at least 12 char long  (i is
     * now equal to one less than the length of the account number)
     */
    if(i==11) {
        pivot = 11;
    } else if(i>11) {
        pivot = 12;
    } else  {
        fprintf(stderr,"Error: account number found of length only %d"
                " in hashValue!\n", i+1);
        return(-1);
    }


    /* 
     * In the following loop we simply check that the characters at
     * the offsets of interest are in fact "graphic" characters
     * (printable but not space).
     */
    for (k = i - pivot; k < (i+1); k++) {
        if (!isgraph((int) acctNum[k])) {
            fprintf (stderr,
                     "Warning: character %c at %d in %s is not \"graphic\"\n",
                     acctNum[k], k+1, acctNum);
            return(-1);
        }
    }


    /* 
     * In the following loop, we range through a set of numbers:
     *    k = 0 ... 5
     *    i = (length of acctNum) - 1 - k
     *    j = (length of acctNum) - 1 - pivot + k
     *
     *    We convolute the characters at offets i and j with the coprimes k.
     */
    for(k=0, j=i-pivot; k<6; i--, j++, k++) {
        sum += abs (((int) (acctNum[j] - '0') * 10) +
                    (int) (acctNum[i] - '0')
                   ) * coprime[k];
    }


    /* Here we modulo the sum with the desired resolution */
    return (sum % RESOLUTION);
}


/******************************************************************************/
