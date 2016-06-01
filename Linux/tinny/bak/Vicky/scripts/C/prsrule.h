#ifndef PRSRULE_H
#define PRSRULE_H

/*
** $Id: prsrule.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: prsrule.h,v $
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
 * Revision 1.3  1995/07/28  00:15:46  can
 * Added ParseRuleBase prototype back to this file
 *
 * Revision 1.2  1995/07/21  16:20:39  can
 * Added functions for variable handling
 *
 * Revision 1.1  1995/07/03  21:18:26  wle
 * Initial revision
 *
*/

#include "exptree.h"
#include "parstrc.h"


#define RETURN_PARSE  -98
#define TERM_PARSE    -99

long ParseArrayIndex
(tParse *parse, char *arrayVar, char *arrayIndex, tArrayUse *arrayUse);

long ParseExprTree(tParse *parse, char *optionTerms, tExprTree *exprTree);
long ParseExpression(tParse *parse, char *optionTerms, tExprTree **pExprTree);

long ParseReturn(tParse *parse, tExprTree **pExprTree);
long ParseRuleCall(tParse *parse, tRuleCall **ruleCall);
long ParseFunction(tParse *parse, tFuncCall **funcCall);
long ParseElse(tParse *parse, tIfData *ifData);
long ParseIfStatement(tParse *parse, tIfData **pIfData);

long ParseVarCalcRule(tParse *parse, tEngVar *var);

long ParseStatementList
(tParse *parse, int inIfBlock, tStatement **statementList);
long ParseStatement(tParse *parse, char *optionTerms, tStatement *statement);

long ParseRule(tParse *parse, tStatement **statementList);

long ParseRuleBase(tRuleBaseData *parse, void **parseResults);

#endif
