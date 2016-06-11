#ifndef HASHVALUE_H
#define HASHVALUE_H

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

#define rcsId_hashValue_h "$Id: hashValue.h,v 2.1 1999/12/08 18:09:44 pcd Exp $"

enum { testTrain = 0, sampstats};    /* hash type */
enum { forward = 0, backward};       /* hash direction */

int hashValue(char* acctNum, int hashType, int hashDirection);
char * rcsId_hashValue();

#endif

