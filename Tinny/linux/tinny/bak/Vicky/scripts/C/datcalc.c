/*
** $Id: datcalc.c,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: datcalc.c,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.3  1997/01/16  22:44:06  can
 * Lots of changes for 2.1 release
 *
 * Revision 2.2  1996/07/02  20:34:21  can
 * Fixed bug in processing negative HNC_FLOATSTR values.
 *
 * Revision 2.1  1996/05/01  01:18:11  can
 * Changed numeric type to double for better precision
 *
 *
*/

/* HNC Rule Engine includes */
#include "datcalc.h"

long NUM_SETSTR(tEngNum *val, char *str, int n)
{ 
  double  frac=0, ex=0, power=1;
  int    i, sign=1, exsign=1, num=0;
  char   *ptr = str+n;

  ZERO((*val))

  while(isspace(*str))
     if(++str==ptr) return(-1);

  if(*str=='+' || *str=='-') 
  {  sign=(*str=='+')?1:-1;
     if(++str==ptr) return(-1);
  }

  if(*str>='0' && *str<='9')
  {  num=1;
     for(; *str>='0' && *str<='9';)  
     {  val->fp = 10*val->fp + *str - '0';
        if(++str==ptr) 
        {  val->fp *= sign;
           return(0);
        }
     }
  }

  val->fp *= sign;

  if(*str=='.')
  {  if(++str==ptr)
     {  if(num) return(0);
        else return(-1);
     }

     for(; *str>='0' && *str<='9';) 
     {  frac = frac*10 + *str-'0';
        power *= 10;
        if(++str==ptr)
        {  frac /= power;
           val->fp += sign*frac;
           return(0);
        }
     }
  }

  frac /= power;
  val->fp += sign*frac;

  if(*str=='e' || *str=='E')
  {  if(++str==ptr) return(-1);
     
     if(*str=='+' || *str=='-') 
     {  exsign=(*str=='+')?1:-1;
        if(++str==ptr) return(-1);
     }
    
     for(; *str>='0' && *str<='9';)  
     {  ex = 10*ex + *str-'0';
        if(++str==ptr)
        {  for(i=0; i<ex; i++)
           {  if(exsign==1) val->fp *= 10;  
              else val->fp /= 10;
           }
           return(0);
        }
     }
  }

  return(0);
}
