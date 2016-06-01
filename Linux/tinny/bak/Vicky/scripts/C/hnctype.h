#ifndef HNCTYPE_H
#define HNCTYPE_H

/* ----------------------------------------------------------------------
 *
 *  FILE: hnctype.h
 *
 * -------------------------------------------------------------------- */

#include <limits.h>

#define  INT16 short
#define UINT16 unsigned short
#define  INT32 long         
#define UINT32 unsigned long
#define   REAL float

#ifndef BOOL
#ifndef BOOLDEF      /* use this flag in third party software if they used BOOL */
#define BOOL  char
#endif
#endif

#ifndef BYTE
#define BYTE  char
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE  1
#endif

#define MISSINT   INT_MIN
#define MISSLNG   LONG_MIN

#define MISSFLT   -(float)INT_MIN 
#define MISSDBL   -(double)LONG_MIN

#endif
