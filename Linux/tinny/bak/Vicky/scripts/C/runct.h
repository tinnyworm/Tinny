#ifndef RUNCT_H
#define RUNCT_H

/*
** $Id: runct.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: runct.h,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.1  1997/01/16  22:44:06  can
 * Lots of changes for 2.1 release
 *
 * Revision 2.0  1996/03/18  18:45:44  gak
 * GUTS 2.0.1 alpha release
 *
 * Revision 1.2  1995/07/09  23:07:20  can
 * Fixed some bugs
 *
 * Revision 1.1  1995/07/03  21:18:26  wle
 * Initial revision
 *
*/

/**************************************************/
/* Status codes for node calculation flow control */
/**************************************************/
#define KEEP_GOING      0
#define RETURN_FOUND    1
#define TERMINATE_FOUND 2

#define DONE(code) (code == TERMINATE_FOUND || code == RETURN_FOUND)


char  *GetMemory(tCalcTable *calcTab, int size, tREStatus *status);
long  ResolveRecordAllLen
(tRecData *rec, char **data, long recLen, tREStatus *status, tDynPool *dynPool);
long  ResolveRecordNoLen
(tRecData *rec, char **data, tREStatus *status, tDynPool *dynPool);
 
void CalcRuleStatements(tRuleCalc *ruleCalc, tREStatus *status, tCalcTable *calcTab);

long RunCalcTable(tCalcTable *calcTab);

#endif
