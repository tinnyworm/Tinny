static char rcsid[] = "$Header: /work/price/falcon/dvl/ccpp/drv_falcon30/RCS/flib.c,v 1.1 1998/06/08 22:55:28 jcb Exp $";
static char copyright[] = "(c) Copyright 1992, HNC Inc. All Rights Reserved";
/*========================================================================*/
/*                                                                        */
/*                                  flib.c                                */
/*                                                                        */
/*  DESCRIPTION: Contains replacement function for C-standard-library 
 *    function atoi();  *  and atoi() versions for handling non-null-
 *    terminated strings
 *
 *  NOTES: low level utility called from translation layer
/*                                                                        */
/*========================================================================*/
/*                                                                        */
/*
$Source: /work/price/falcon/dvl/ccpp/drv_falcon30/RCS/flib.c,v $
 *
$Log: flib.c,v $
Revision 1.1  1998/06/08 22:55:28  jcb
Initial revision

 * Revision 1.2  1998/04/22  21:09:27  jcb
 * changed xatofp to replace blank with zero once numbers have started in a field
 *
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 1.3  1995/08/28  17:23:42  can
 * Moved checksum to end of the profile array.
 *
 * Revision 1.2  1995/07/17  19:19:13  mjk
 * Changed some char * declaration to const char *, mainly to shut up som
 * C++ errors/warnings from other code which uses this.  No change in
 * functionality, but technically more "correct."
 *
 * Revision 1.1  1995/03/22  19:55:32  can
 * Initial revision
 *
/*                                                                        */
/*========================================================================*/
#include <ctype.h>

#include "hnctype.h"
#include "flib.h"

#define BAD_COMP3 LONG_MAX


/* -----------------------------------------------------------------------
 *
 *  null-terminated strict version of atoi returns MISSINT on error
 *
 *  does not accept things like "1bob"
 *  does accept "  1  "
 *  only returns 0 for strings like "0", " 0", "0 ", and " 0 "
 *
 *  INPUT: a null terminated string
 *
 *  EXAMPLE: if((x=xatoi(string))==MISSINT) err();
 *
 * --------------------------------------------------------------------- */
int xatoi(const char * str)
{ int x, sign=1;
  while(isspace(*str)) str++;
  if(*str=='+' || *str=='-') sign=((*str++)=='+')?1:-1;
  for(x=0; *str>='0' && *str<='9'; str++)
    x = 10*x + *str - '0';
  while(isspace(*str)) str++;
  if(*str=='\0') return(sign*x);
  else return(MISSINT);
}

/* -----------------------------------------------------------------------
 *  xfatolp: floating-point-alpha to long (ptr version)
 *
 *  ptr-terminated version parses -9(nn).v99 (ft:) returns MISSINT on error
 *
 *  INPUT: pointer to string area + pointer to where \0 would have been
 *
 *  EXAMPLE: if((x=xfatolp(string,string+fieldLength))==MISSINT) err();
 *
 * --------------------------------------------------------------------- */
long xfatolp(const char * str, const char * ptr)
{ long x, sign=1L;
  while(isspace(*str))
    if(++str==ptr) return(MISSLNG);
  if(*str=='+' || *str=='-') {
    sign=(*str=='+')?1L:-1L;
    if(++str==ptr) return(MISSLNG);
  }
  if(*str>='0' && *str<='9')
  {
    for(x=0L; *str>='0' && *str<='9';) {
      x = 10L*x + (long)(*str - '0');
      if(++str==ptr) return(sign*x);
    }
    if(*str=='.')
    { if(++str==ptr) return(sign*x);
      for(; *str>='0' && *str<='9';) {
  x = 10L*x + (long)(*str - '0');
  if(++str==ptr) return(sign*x);
      }
    }
  }
  else return(MISSLNG);
  while(isspace(*str)) if(++str==ptr) return(sign*x);

  return(MISSLNG);
}

/* -----------------------------------------------------------------------
 *  xfatoln: floating-point-alpha to long (length version)
 *
 *  ptr-terminated version parses -9(nn).v99 (ft:) returns MISSINT on error
 *
 *  INPUT: pointer to string area + pointer to where \0 would have been
 *
 *  EXAMPLE: if((x=xfatolp(string,string+fieldLength))==MISSINT) err();
 *
 * --------------------------------------------------------------------- */
long xfatoln(const char * offSet, int fieldLen)
{ const char * ptr = offSet+fieldLen;
  return(xfatolp(offSet,ptr));
}


/* -----------------------------------------------------------------------
 *
 *  ptr-terminated version of xatoi returns MISSINT on error
 *
 *  INPUT: pointer to string area + pointer to where \0 would have been
 *
 *  EXAMPLE: if((x=xatoip(string,string+fieldLength))==MISSINT) err();
 *
 * --------------------------------------------------------------------- */
long   xatolzip5p(const char * str, const char * ptr)
{ long x; int i;
  for(x=0L, i=0; i<5; i++, str++)
  { if(str==ptr) return(MISSLNG);
    if(*str<'0' || *str>'9') return(MISSLNG);
    else x = 10L*x + *str - '0';
  }
  return(x);
}

long xatolp(const char * str, const char * ptr)
{ long x, sign=1L;
  while(isspace(*str))
    if(++str==ptr) return(MISSLNG);
  if(*str=='+' || *str=='-') {
    sign=(*str=='+')?1L:-1L;
    if(++str==ptr) return(MISSLNG);
  }
  if(*str>='0' && *str<='9')
    for(x=0L; *str>='0' && *str<='9'; ) {
      x = 10L*x + *str - '0';
      if(++str==ptr) return(sign*x);
    }
  else return(MISSLNG);
  while(isspace(*str)) if(++str==ptr) return(sign*x);

  return(MISSLNG);
}

int xatosic(const char * str)
{ int x,count;
  while(isspace(*str)) str++;
  for(count=0,x=0; *str>='0' && *str<='9'; str++,count++)
    x = 10*x + *str - '0';
  while(isspace(*str)) str++;
  if(*str=='\0' && count==4) return(x);
  else return(MISSINT);
}

int xatoip(const char * str, const char * ptr)
{ int x, sign=1;
  while(isspace(*str))
    if(++str==ptr) return(MISSINT);
  if(*str=='+' || *str=='-') {
    sign=(*str=='+')?1:-1;
    if(++str==ptr) return(MISSINT);
  }
  if(*str>='0' && *str<='9')
    for(x=0; *str>='0' && *str<='9'; ) {
      x = 10*x + *str - '0';
      if(++str==ptr) return(sign*x);
    }
  else return(MISSINT);
  while(isspace(*str)) if(++str==ptr) return(sign*x);

  return(MISSINT);
}

/* -----------------------------------------------------------------------
 *
 *  fixed-field version of strict atoi
 *
 *  INPUT: pointer to string area + number of bytes to look at
 *
 *  EXAMPLE: if((x=xatoin(string,fieldSize))==MISSINT) err();
 *
 * --------------------------------------------------------------------- */
long   xatolzip5n(const char * str)
{ const char * ptr = str+5;
  return(xatolzip5p(str,ptr));
}
long xatoln(const char * str, int n)
{ const char * ptr = str+n;
  return(xatolp(str,ptr));
}
int xatoin(const char * str, int n)
{ const char * ptr = str+n;
  return(xatoip(str,ptr));
}

/* -----------------------------------------------------------------------
 *
 *  null-terminated strict version of atof returns MISSDBL on error
 *
 *  does not accept things like "1.0bob"
 *  does accept "  1.0  "
 *  only returns 0.0 for strings like "0.0", " 0.0", "0.0 ", and " 0.0 "
 *
 *  INPUT: a null terminated string
 *
 *  EXAMPLE: if((x=xatof(string))==MISSDBL) err();
 *
 * --------------------------------------------------------------------- */
double xatof(const char * str)
{ double x,power=1;
  int sign=1;
  while(isspace(*str)) str++;
  if(*str=='+' || *str=='-') sign=((*str++)=='+')?1:-1;
  for(x=0; *str>='0' && *str<='9'; str++)
    x = 10*x + *str - '0';
  if(*str=='.') str++;
  for(power=1; *str>='0' && *str<='9'; str++) {
    x = 10*x + *str - '0';
    power *= 10;
  }
  while(isspace(*str)) str++;
  if(*str=='\0')
    return((double)((sign*x)/power));
  else return(MISSDBL);
}

/* -----------------------------------------------------------------------
 *
 *  ptr-terminated version of xatof returns MISSDBL on error
 *
 *  INPUT: pointer to string area + pointer to where \0 would have been
 *
 *  EXAMPLE: if((x=xatofp(string,string+fieldLength))==MISSDBL) err();
 *
 * --------------------------------------------------------------------- */
double xatofp(const char * str, const char * ptr)
{ double x=0.0,power=1.0;
  int sign=1, num=0;
  while(isspace(*str))
    if(++str==ptr) return(MISSDBL);
  if(*str=='+' || *str=='-') {
    sign=(*str=='+')?1:-1;
    if(++str==ptr) return(MISSDBL);
  }

  if(*str>='0' && *str<='9')
  { num=1;
    for(; (*str>='0' && *str<='9') || *str==' ';) {
      x = 10.0*x;
      if ( *str != ' ')
      x += (double)(*str - '0');
      if(++str==ptr) return((double)sign*x);
    }
  }

  if(*str=='.')
  { if(++str==ptr)
      if(num) return((double)sign*x);
      else return(MISSDBL);
    for(power=1.0; *str>='0' && *str<='9';) {
      num++;
      x = 10.0*x + (double)(*str - '0');
      power *= 10.0;
      if(++str==ptr) return((double)sign*x/power);
    }
  }

  while(isspace(*str))
    if(++str==ptr)
      if(num) return((double)sign*x/power);
    else return(MISSDBL);
  return(MISSDBL);
}

/* -----------------------------------------------------------------------
 *
 *  fixed-field version of strict xatof
 *
 *  INPUT: pointer to string area + number of bytes to look at
 *
 *  EXAMPLE: if((x=xatofn(string,fieldSize))==MISSDBL) err();
 *
 * --------------------------------------------------------------------- */
double xatofn(const char * str, int n)
{ const char * ptr = str+n;
  return(xatofp(str,ptr));
}

/* -----------------------------------------------------------------------
 *
 *  convert COMP3 COBOL format to long
 *
 *  COMP3 uses each nibble to represent one decimal digit
 *  ... the value in the nibble is the value of the digit
 *  ... the low nibble encodes the sign, D => negative
 *  ... ie. 12345 == BYTE array[3] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x0C}
 *
 *
 *  INPUT: pointer to byte buffer + number of bytes to look at
 *
 *  LIMITATIONS: will only parse 5 bytes
 *  ... limitation is size limit of a signed long 2,147,483,647
 *
 *  EXAMPLE: if((z=comp3tol(&comp3Num,5))==BAD_COMP3) err();
 *
 * --------------------------------------------------------------------- */
INT32 comp3tol(const BYTE * ptr, int n)
{ INT32 power; const BYTE * low = ptr+n-1;
  INT32 sign = (*low) & 0x000F;
  INT32 y,x = (long)((*low--) & 0x00F0)>>4;

  if(n>5) return(BAD_COMP3);

  sign = sign==0x0D ? -1L : 1L;
  for(power=10L; n>1; n--,low--,power*=100L)
    x += (power*(*low & 0x000F)) + ((long)(10*power*(*low & 0x00F0))>>4);

  return(sign*x);
}


float checkSum(const float* buffer, int numEntries)
{
   int   shift, value, i;
   float sum=0;

   /*****************************************/
   /* Assume last entry is check sum value, */
   /* so don't include it.                  */
   /*****************************************/
   for(i=0; i<numEntries-1; i++)
      sum += buffer[i];

   return(sum);
}
