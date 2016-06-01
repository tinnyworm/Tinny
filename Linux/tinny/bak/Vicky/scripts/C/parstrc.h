#ifndef PARSTRC_H
#define PARSTRC_H

/*
** $Id: parstrc.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: parstrc.h,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.3  1997/01/16  22:44:06  can
 * Lots of changes for 2.1 release
 *
 * Revision 2.1  1996/04/22  17:56:15  can
 * Several fixes and changes for 2.1
 *
 * Revision 1.8  1995/11/15  00:06:10  can
 * Made MAX_TOKEN_LENGTH the same as MAX_RULE_LENGTH.  Also, changed to 4096.
 * Most tokens, obviously, will not be as large as that, but it should be a
 * catch-all.  4K of space is negligible.
 *
 * Revision 1.7  1995/08/25  01:25:48  can
 * Fixed compile warning messages
 *
 * Revision 1.6  1995/08/09  23:56:00  can
 * Added tVarPtr structure which is used to store a list of
 * all variables that will be free'd at shutdown
 *
 * Revision 1.5  1995/08/08  16:56:03  can
 * Added fields to make sure RETURN statements exist for
 * rules that have a return type
 *
 * Revision 1.4  1995/08/02  00:09:11  can
 * Added fields to support the rule base support rules.
 *
 * Revision 1.3  1995/07/21  16:20:39  can
 * Added variable lists to tParse structure
 *
 * Revision 1.2  1995/07/09  23:07:20  can
 * Fixed some bugs
 *
 * Revision 1.1  1995/07/03  21:18:26  wle
 * Initial revision
 *
*/

/* HNC Rules Engine include files */
#include "toktype.h"
#include "exptree.h"
#include "engine.h"

/* HNC Common includes */
#include "hncrtyp.h"

#define MAX_TOKEN_LENGTH   4096  


/**************************************************************/
/* Structure to hold parsed information about sttatement list */
/**************************************************************/
#define IF_STATEMENT       1
#define EXPR_STATEMENT     2
#define RETURN_STATEMENT   3
#define TERM_STATEMENT     4
#define WHILE_STATEMENT    5
#define FOR_STATEMENT      6
#define CASE_STATEMENT     7
typedef struct sStatement {
   int    traceOn;
   int    level;
   int    type;
   union {
      struct sIfData    *ifData;
      struct sWhileData *whileData;
      struct sForData   *forData;
      struct sCaseData  *caseData;
     tExprTree *exprTree;
   } data;

   struct sStatement *next;
} tStatement;

/************************************************************/
/* Structure to hold parsed information about an WHILE loop */
/************************************************************/
typedef struct sWhileData {
   tExprTree  *condExpr;
   tStatement *stmtList;
   long       limit;
} tWhileData;

/**********************************************************/
/* Structure to hold parsed information about an FOR loop */
/**********************************************************/
#define COUNT_UP     1
#define COUNT_DOWN   2
typedef struct sForData {
   tExprTree  *initExpr;
   tExprTree  *limitExpr;
   tExprTree  *stepExpr;
   tEngVar    *loopVar;
   long       direction;
   struct sStatement *stmtList;
} tForData;

/**************************************************************/
/* Structure to hold parsed information about an IF statement */
/**************************************************************/
#define NO_SECOND          0
#define ELSE_STATEMENT     1
#define ELSE_IF_STATEMENT  2
typedef struct sIfData {
   tExprTree  *condExpr;
   tStatement *primeSList;
   int secondaryType;
   union {
      struct sStatement *sList;
      struct sIfData    *ifData;
   } secondary;
} tIfData;

/****************************************************************/
/* Structures to hold parsed information about a CASE statement */
/****************************************************************/
typedef struct sCaseArg {
   tAtom atom;
   struct sCaseArg  *next;
} tCaseArg;

typedef struct sCase {
   tCaseArg   *valList;     
   tStatement *stmtList;

   struct sCase *next;
} tCase;

typedef struct sCaseData {
   tExprTree  *caseExpr;
   tCase      *caseList;
} tCaseData;


/********************************************************/
/* Structure to hold a list of all variables and types, */
/* to make them easily accessible for freeing.          */
/********************************************************/
typedef struct sDataNode {
   union {
      tEngVar *var;
      tType   *type;
   } val;
   struct sDataNode *next;
} tDataNode;

typedef struct sDataPtr {
   tDataNode *typeList;
   tDataNode *varList;
} tDataPtr;


/********************************************************/
/* Structure used to pass the parsed information out of */
/* ParseRuleBase.                                       */
/********************************************************/
#define SYS_INIT_RULE   1
#define SYS_TERM_RULE   2
#define ITER_INIT_RULE  3
#define ITER_TERM_RULE  4
#define EXEC_RULE       5
typedef struct sParseResults {
   int          ruleType;
   tRuleData    *ruleData;
   tStatement   *ruleStatementList;
   tEngVar      *paramVarList;
   tDataPtr     *allData;
   tEngRBData   *engRB;
   tRecInstance *tmpltRecList;
 
   struct sParseResults *next;
} tParseResults;

/***********************************************************/
/* Structure passed around to hold all current information */
/* for parsing a rule.                                     */
/***********************************************************/
typedef struct sParse {
   char           *ruleText;
   char           token[MAX_TOKEN_LENGTH];
   tTokenType     tokenType;
   tEngRBData     *RBList;
   tEngRBData     *curRB;

   union {
      tEngNum    num;
      tArrayUse  *arrayUse;
      tRecUse    *dataFld;
      tEngVar    *var;
      tEngRBData *engRB;
   } data;

   int            lineNumber;
   int            index;
   int            lastIndex;
   int            numLinesInLast;
   int            numBegins;
   int            allowGlobalDef;
   int            inExpr;
   int            traceOn;
   int            numSubs;
   int            numReturns;
   int            mainReturn;
   int            whileLimit;
   tDataTemplate  *templateList;
   tFunc          *funcList;
   tRuleData      *ruleList;
   tRuleErr       *curErr;
   tRuleData      *curRule;
   tEngVar        *globalVarList;
   tEngVar        *paramVarList;
   tEngVar        *localVarList;
   tType          *globalTypeList;
   tType          *localTypeList;
   tDataPtr       *allData;
   tParseResults  *parseResults;
   tRecInstance   *tmpltRecList;
   
} tParse;


#endif
