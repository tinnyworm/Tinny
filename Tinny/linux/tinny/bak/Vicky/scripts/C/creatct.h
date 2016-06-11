#ifndef CREATCT_H
#define CREATCT_H

/*
** $Id: creatct.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: creatct.h,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.2  1997/01/16  22:44:06  can
 * Lots of changes for 2.1 release
 *
 * Revision 2.1  1996/04/22  17:56:02  can
 * Several fixes and changes for 2.1
 *
 * Revision 1.2  1995/07/09  23:07:20  can
 * Fixed some bugs and updated the error handling
 *
 * Revision 1.1  1995/07/03  21:18:26  wle
 * Initial revision
 *
*/

#include "calctab.h"
#include "parstrc.h"
#include "engine.h"


long CreateCalcTable
(tRuleBaseData *ruleBaseData, tParseResults *parseResults, tCalcTable *ruleCalcTable);
long AllocateCalcTable
(tCalcTable **ruleCalcTable);
long CreateVarCalc
(tVarCalc **varCalc, tEngVar *var, tCalcTable *calcTab);


#endif
