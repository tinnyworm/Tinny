#ifndef LEXRULE_H
#define LEXRULE_H

/*
** $Id: lexrule.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: lexrule.h,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.0  1996/03/18  18:45:44  gak
 * GUTS 2.0.1 alpha release
 *
 * Revision 1.1  1995/07/03  21:18:26  wle
 * Initial revision
 *
*/

#include "parstrc.h"

#define END_OF_RULE   -99

long LexRule(tParse *parse);
long PutBackToken(tParse *parse);

#endif
