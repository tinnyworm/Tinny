#ifndef RUNFUNC_H
#define RUNFUNC_H

/*
** $Id: runfunc.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: runfunc.h,v $
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


#include "calctab.h"


/**************************/
/* Support function types */
/**************************/
#define SYSTEM_INIT   1
#define ITER_INIT     2
#define SYSTEM_TERM   3
#define ITER_TERM     4


/*************************************************************************/
/* Function to call a set of support functions required by the functions */
/* called by the rule base.                                              */
/*                                                                       */
/*   - ruleCalcTab must be allocated and initialized via a call to       */
/*     CreateCalcTable.                                                  */
/*   - funcType must be one of the above support function types          */
/*************************************************************************/
long RunSupportFunctions(tCalcTable *ruleCalcTable, int funcType);


#endif

