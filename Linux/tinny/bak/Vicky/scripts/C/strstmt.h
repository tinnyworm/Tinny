#ifndef STRSTMT_H
#define STRSTMT_H

/*
** $Id: strstmt.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: strstmt.h,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.0  1996/03/18  18:45:44  gak
 * GUTS 2.0.1 alpha release
 *
 *
*/

/* HNC Rule Engine includes */
#include "exptree.h"
#include "prsrule.h"
#include "typechk.h"


char* GetOpStr(tOp op);
void  MakeExpression(char *buf, tExprTree *exprTree); 
void  MakeWhile(char *buf, tWhileData *whileData);
void  MakeFor(char *buf, tForData *forData);
void  MakeTerminate(char *buf, tExprTree *termExpr);
void  MakeReturn(char *buf, tExprTree *retExpr);
void  MakeCase(tStmtText **textList, tCaseData *caseData);
void  MakeIf(tStmtText **textList, tIfData *ifData, int isElse);
long  MakeStatementText(tStmtText **textList, tStatement *stmt);

#endif
