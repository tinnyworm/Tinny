#ifndef CALCTAB_H
#define CALCTAB_H

/*
** $Id: calctab.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: calctab.h,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.3  1997/01/16  22:44:06  can
 * Lots of changes for 2.1 release
 *
 * Revision 2.2  1996/04/22  19:03:27  wle
 * Corrected include file name hncrtype.h to hncrtyp.h
 *
 * Revision 2.1  1996/04/22  17:56:00  can
 * Several fixes and changes for 2.1
 *
 * Revision 1.11  1995/08/25  01:25:48  can
 * Fixed compile warning messages
 *
 * Revision 1.10  1995/08/09  23:56:00  can
 * Added list of allVars to make it easy to free the variable structures
 *
 * Revision 1.9  1995/08/02  00:03:16  can
 * Added new lists for rule base support rules .
 *
 * Revision 1.8  1995/07/28  00:12:42  can
 * Took out ruleBaseName because it now lives in ruleBaseData
 *
 * Revision 1.7  1995/07/27  00:16:51  can
 * Added stuff to tCalcTable to make run-time exception handling
 * better.
 *
 * Revision 1.6  1995/07/26  20:53:26  can
 * Added support for new expression calculator
 *
 * Revision 1.4  1995/07/21  15:34:05  can
 * Moved flags from atom to argument.
 *
 * Revision 1.3  1995/07/13  22:31:52  can
 * Took out tREStatus structure and put it in engine.h
 *
 * Revision 1.2  1995/07/09  23:07:20  can
 * Fixed some bugs and updated the error handling
 *
 * Revision 1.1  1995/07/03  21:18:26  wle
 * Initial revision
 *
*/


/* HNC Rule Engine include files */
#include "parstrc.h"
#include "engine.h"

/* HNC Common includes */
#include "hncrtyp.h"


/***************************************************************/
/* Structure used to hold a list of function structures of the */
/* functions used by a rule base.  It is used to access the    */
/* support functions.                                          */
/***************************************************************/
typedef struct sUsedFunc {
   tFunc  *func;
   
   struct sUsedFunc *next;
} tUsedFunc;


/**************************************************/
/* Allow 10K of memory for run-time temporary use */
/**************************************************/
#define MEM_POOL_SIZE  10240L
typedef struct sMemPool {
   char   data[MEM_POOL_SIZE];
   int    index;
} tMemPool;


/****************************************************************/
/* Structures to hold data blocks for dynamically resolving non */
/* fixed templates and arrays.                                  */
/****************************************************************/
#define POOL_PAGE_SIZE  100L * 1024L 
typedef struct sPoolPage {
   char *data;
   struct sPoolPage *next;
} tPoolPage;

typedef struct sDynPool {
   long       offset;
   tPoolPage  *curPage;
   tPoolPage  *pageList;
   struct sCalcTable *mainCalcTab;
} tDynPool;


/************************************************************************/
/* Structure to hold all information necesary for rule base calculation */
/************************************************************************/
typedef struct sCalcTable {
   struct sRuleCalc    *sysInitList;
   struct sRuleCalc    *sysTermList;
   struct sRuleCalc    *iterInitList;
   struct sRuleCalc    *iterTermList;
   struct sRuleCalc    *ruleCalcList;
   struct sRuleCalc    *paramRuleList;
   struct sStr2NumCalc *str2numList;
   struct sVarCalc     *varCalcList;
   struct sRuleCalc    *currentRuleCalc;
   struct sVarCalc     *calcVar;
   int                 numArgFuncs;
   struct sArgFunc     *argFuncArray;
   tUsedFunc           *usedFuncList;
   char                haveSysInitFuncs;          
   char                haveSysTermFuncs;          
   char                haveIterInitFuncs;          
   char                haveIterTermFuncs;          
   tRuleBaseData       *ruleBaseData;
   tMemPool            *memPool;
   tDataPtr            *allData;
   tAtom               retValue;
   int                 level;
   int                 traceOn;
   void                *user;
   int                 noAutoResetShared;
   tDynPool            *dynPool;
   int                 numCalcDones;
   char                *calcDones;
   tRecInstance        *tmpltRecList;

   struct sCalcTable *next;
} tCalcTable;


/*****************************************************/
/* Structure to hold a list of atomic type arguments */
/*****************************************************/
/* Flags used for interpreting data */
#define ATOM_STRING       0x000001   /* The data are a string */
#define ATOM_NOT          0x000002   /* The data should be NOT'd */
#define ATOM_NEGATE       0x000004   /* The data should be negated */
#define ATOM_ADDRESS      0x000008   /* The data are a data field address to be resolved */
#define ATOM_CONSTANT     0x000010   /* The argument is a constant */
#define ATOM_REFERENCE    0x000020   /* The data are a reference to a data field */
#define ATOM_PARAMETER    0x000040   /* The data are a parameter */
#define ATOM_FLDDATA      0x000080   /* The arg is a record field to be resolved */
#define ATOM_ARRAYPROC    0x000100   /* The arg is a record with an array to be resolved */
#define ATOM_ERROR        0x000200   /* The arg is most recent status error */
#define ATOM_CASE_STMT    0x000400   /* The arg is a statement for a case */
#define ATOM_VAR_RETURN   0x000800   /* The arg return statement for a calc var */
#define ATOM_STMT_CALC    0x001000   /* The data are a statement list */
#define ATOM_EXPR_CALC    0x002000   /* The data are an expression calc list */
#define ATOM_ARG_CALC     0x004000   /* The data are a argument calc node */
#define ATOM_FUNC_CALC    0x008000   /* The data are a function calc node */
#define ATOM_STR2NUM_CALC 0x010000   /* The data are a shared calc node */
#define ATOM_VAR_CALC     0x020000   /* The data are a shared calc node */
typedef struct sArg {
   struct sArgFunc *func;
   long            flags;
   tAtom           atom;

   struct sArg     *next;
} tArg;


/**************************************************/
/* Structrure to store all the argument functions */
/* This makes it easier to relocate the functions */
/**************************************************/
typedef void (*tfArgEval)(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);

typedef struct sArgFunc {
   char       *funcName;
   tfArgEval  EvalFunc;
} tArgFunc;


/*****************************************************************/
/* Structures to hold data for the various types of calculations */
/*****************************************************************/
/* Expression calc structure */
#define IS_ORED    0x01
#define IS_ANDED   0x02
#define LEFT_ARG   0x04
#define RIGHT_ARG  0x08

#define MAX_EVALS  128
typedef struct sExprCalc {
   short  func;
   char   flags;
   char   evalIdx; 
   void   *left;
   void   *right;

   struct sExprCalc *parent;
   struct sExprCalc *next;
} tExprCalc;


/* Argument calc structure */
typedef struct sArgCalc {
   tArg   *argList;
} tArgCalc;


/* Statement calc structure */
typedef struct sStmtCalc {
   tArg    *argList;
} tStmtCalc;


/* Function calc structure */
typedef struct sFuncCalc {
   tFunc     *funcData;
   tArg      *argList;
   tRuleData *ruleData;
} tFuncCalc;


/* String to numeric calc structure */
typedef struct sStr2NumCalc {
   char      *id;
   char      *calcDone;
   tArg      arg;
   tAtom     eval;

   struct sStr2NumCalc *next;
} tStr2NumCalc;


/* Variable calc structure */
typedef struct sVarCalc {
   char       *id;
   tEngVar    *var;
   tArg       *arg;

   struct sVarCalc *next;
} tVarCalc;


/* Rule calc structure */
typedef struct sRuleParam {
   tEngVar *param;
   struct sRuleParam *next;
} tRuleParam;

typedef struct sRuleCalc {
   char       *id;
   tStmtCalc  *stmtBlock;
   tRuleParam *paramList;

   struct sRuleCalc *next;
} tRuleCalc;


#endif
