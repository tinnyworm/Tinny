#ifndef DATACALC_H
#define DATACALC_H

/*
** $Id: datcalc.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: datcalc.h,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.3  1997/01/16  22:44:06  can
 * Lots of changes for 2.1 release
 *
 * Revision 2.2  1996/05/01  01:18:36  can
 * Changed numeric type to double for better precision
 *
 *
*/

/* System includes */
#include <ctype.h>
#include <math.h>

/*********************************************/
/* Structure for handling numeric data.  To  */
/* maximize precision, both a  float portion */
/* and an integer portion are stored.        */
/*********************************************/
typedef struct sEngNum {
   double fp;
} tEngNum;

/********************************************/
/* Macros used to calculate with tEngNum types */
/********************************************/
#define NUM_GETFLT(x) ((float)(x.fp))
#define NUM_GETDBL(x) ((double)(x.fp))
#define NUM_GETSHRT(x) ((short)(x.fp))
#define UNUM_GETSHRT(x) ((unsigned short)(x.fp))
#define NUM_GETLNG(x) ((long)(x.fp))
#define UNUM_GETLNG(x) ((unsigned long)(x.fp))

#define NUM_MUL(r, x, y) \
{ r.fp = (x.fp) * (y.fp); \
} 
#define NUM_DIV(r, x, y) \
{ if(y.fp==0) { r.fp = 0.0; } \
  else r.fp = (x.fp) / (y.fp); \
} 
#define NUM_SUB(r, x, y) \
  r.fp = x.fp - y.fp;
#define NUM_ADD(r, x, y) \
  r.fp = x.fp + y.fp;

#define NUM_IPADD(x, y) \
   ((tEngNum*)x)->fp += y.fp; 
#define NUM_IPSUB(x, y) \
   ((tEngNum*)x)->fp -= y.fp; 
#define NUM_IPMUL(x, y) \
   ((tEngNum*)x)->fp *= y.fp; 
#define NUM_IPDIV(x, y) \
   if(y.fp==0.0) \
      ((tEngNum*)x)->fp = (float)0.0; \
   else \
      ((tEngNum*)x)->fp /= y.fp;
#define NUM_INCR(x) \
   ( (((tEngNum*)x)->fp) )++; 

#define NUM_EQUAL(r, x, y) \
{ r.fp = ((x.fp) == (y.fp)); \
} 
#define NUM_NOTEQL(r, x, y) \
{ r.fp = ((x.fp) != (y.fp)); \
} 
#define NUM_GRTREQL(r, x, y) \
{ r.fp = ((x.fp) >= (y.fp)); \
}
#define NUM_LESSEQL(r, x, y) \
{ r.fp = ((x.fp) <= (y.fp)); \
}
#define NUM_GRTR(r, x, y) \
{ r.fp = ((x.fp) > (y.fp)); \
}
#define NUM_LESS(r, x, y) \
{ r.fp = ((x.fp) < (y.fp)); \
}


#define NUM_NEG(x) \
{ x.fp = -(x.fp); } 
#define NUM_NOT(x) \
{ if(x.fp) \
  {  x.fp = 0.0; } \
  else \
  {  x.fp = 1.0; } \
}

#define ZERO(x) \
  x.fp = 0.0; 
#define ONE(x) \
  x.fp = 1.0;

#define TRUNC(x) \
  x.fp = (double)((long)x.fp)
#define ABS(x) \
  x.fp = (double)(fabs((double)x.fp))


#define IS_EQUAL(x, y) \
((x.fp) == (y.fp))
#define IS_LESSEQL(x, y) \
((x.fp) <= (y.fp))
#define IS_GRTREQL(x, y) \
((x.fp) >= (y.fp))
#define IS_LESS(x, y) \
((x.fp) < (y.fp))
#define IS_GRTR(x, y) \
((x.fp) > (y.fp))
#define NOT_EQUAL(x, y) \
((x.fp) != (y.fp))
#define IS_POS(x) \
(x.fp  > 0.0)
#define IS_NEG(x) \
(x.fp  < 0.0)
#define IS_ZERO(x) \
(x.fp  == 0.0)
#define IS_FALSE(x) \
(x.fp == 0.0)
#define IS_TRUE(x) \
(x.fp != 0.0)
#define IS_VAL(x, y) \
((double)(x.fp) == (double)y)
#define IS_INTEGER(x) \
((x.fp - (long)x.fp)==0.0)


#define NUM_AND(r, x, y) \
 r.fp = (IS_TRUE(x) && IS_TRUE(y)); 

#define NUM_OR(r, x, y) \
 r.fp = (IS_TRUE(x) || IS_TRUE(y)); 

#define NUM_SETVAL(x, y) \
  x.fp = y; 
#define NUM_SET(x, y) \
  x.fp = y.fp; 
#define NUM_SETFLT(x, y) \
  x.fp = y; 
#define NUM_SETCHR(x, y) \
  x.fp = (double)y; 
#define NUM_SETDBL(x, y) \
  x.fp = (double)y; 
#define NUM_SETINT16(x, y) \
  x.fp = (double)y; 
#define NUM_SETUINT16(x, y) \
  x.fp = (double)y;
#define NUM_SETINT32(x, y) \
  x.fp = (double)y;
#define NUM_SETUINT32(x, y) \
  x.fp = (double)y; 

long NUM_SETSTR(tEngNum *val, char *str, int n);
void NUMSET(tEngNum *val1, tEngNum *val2);

#endif
