/*
** $Id: creatct.c,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: creatct.c,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.10  1997/03/05  00:17:59  can
 * Fixed bug in determining expression function type (for long expressions,
 * float expressions, mixed numeric expressions, etc).
 *
 * Revision 2.9  1997/02/19  01:35:20  can
 * Fixed bug where rule base "chains" were not properly sharing
 * string to numeric conversions.
 *
 * Revision 2.8  1997/02/12  00:06:25  can
 * Modified to ensure cleanup of all memory when multiple rulebases
 * are used.
 *
 * Revision 2.7  1997/02/11  18:20:01  can
 * Fixed bug in evaluation of local array variables.
 *
 * Revision 2.6  1997/02/05  03:31:22  can
 * Changed a called to free to MyFree
 *
 * Revision 2.5  1997/01/18  00:30:16  can
 * Fixed bug in CreateResolveArg
 *
 * Revision 2.4  1997/01/16  22:44:06  can
 * Lots of changes for 2.1 release
 *
 * Revision 2.1  1996/04/22  17:55:03  can
 * Several fixes and changes for 2.1
 *
 * Revision 1.21  1995/12/14  17:51:46  can
 * Made sure that NOOP calc nodes get their functions set
 * properly to NOOP in CreateExprArgs
 *
 * Revision 1.20  1995/10/27  16:22:56  ramesh
 * Fixed bug where string parameters did not have ATOM_STRING
 * flag set.
 *
 * Revision 1.19  1995/10/14  01:38:51  can
 * Modified so that check for empty rule list doesn't happen until the
 * end of the Creation process.  This is to allow the support rules to
 * still be set up properly.
 *
 * Revision 1.18  1995/09/06  23:57:16  can
 * Fixed a bug dealing with superfluous parentheses and NOT'd or negated
 * parenthesized expressions.
 *
 * Revision 1.17  1995/09/01  00:24:41  wle
 * Made sure that the arg flag is 0 before going to free it.
 *
 * Revision 1.16  1995/08/31  00:05:52  can
 * Fixed more compiler warnings.
 *
 * Revision 1.15  1995/08/25  01:35:44  can
 * Fixed compile warning messages
 *
 * Revision 1.14  1995/08/23  17:23:53  can
 * Made all lines less than 100 bytes
 *
 * Revision 1.13  1995/08/09  23:56:00  can
 *  Added list of allData to make it easy to free the variable structures
 * Also, fixed some bugs, and set the rest of the code up to be free'd
 * properly.
 *
 * Revision 1.12  1995/08/08  22:45:15  can
 * Added special check for NOOP expressions
 *
 * Revision 1.11  1995/08/04  21:19:13  can
 * Made sure right operand of an expression is never a reference field.
 *
 * Revision 1.10  1995/08/02  00:47:50  can
 * Fixed bug in determining whether an expression is numeric or not.
 *
 * Revision 1.9  1995/08/02  00:19:45  can
 * Extensive changes.
 *
 * Revision 1.8  1995/07/25  16:05:02  can
 * Set the ruleBaseData field in the tCalcTable structure
 *
 * Revision 1.7  1995/07/21  21:46:14  can
 * Added full severity and class to return status
 *
 * Revision 1.6  1995/07/21  15:34:05  can
 * Fixed bugs.  Added stuff to enable use of local, global, and parameter
 * type variables.
 *
 * Revision 1.5  1995/07/16  22:32:01  can
 * Made sure that all allocated calc nodes call AllocateCalcNode.
 *
 * Revision 1.4  1995/07/15  00:10:56  can
 * Made sure that NOT and - work with single function or rule calls.
 *
 * Revision 1.3  1995/07/13  23:43:19  can
 * Added fix to make sure string expressions keep their string
 * status (ATOM_STRING)
 *
 * Revision 1.1  1995/07/10  16:14:48  can
 * Initial revision
 *
 * Revision 1.2  1995/07/09  23:07:20  can
 * Fixed some bugs and updated the error handling
 *
 * Revision 1.1  1995/07/03  21:18:26  wle
 * Initial revision
 *
*/


/* System include files */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* HNC rules engine include files */
#include "mymall.h"
#include "exptree.h"
#include "prsrule.h"
#include "engerr.h"
#include "creatct.h"
#include "freect.h"
#include "typechk.h"
#include "bltins.h"
#include "strstmt.h"
#include "evlarg.h"
#include "evlexpr.h"
#include "evlstmt.h"

/* HNC common include files */
#include "except.h"

#ifdef DEVELOP
#include "rbdebug.h"
#endif

static long GetCalcForRule
(tCalcTable *calcTab, tRuleData *ruleData, int ruleType, tRuleCalc **ruleCalc);
static long CreateRuleCalc
(tRuleCalc **pRuleCalc, tParseResults *parseResults, tCalcTable *calcTab);

static long CreateStatementBlock
(tStmtCalc **stmtBlock, tStatement *statementList, tCalcTable *calcTab);
static long CreateStatementCalc
(tArg *stmtArg, tStatement *statement, tCalcTable *calcTab);
static long CreateTermStmt
(tArg *stmtArg, tExprTree *termExpr, tCalcTable *calcTab);
static long CreateReturnStmt
(tArg *stmtArg, tExprTree *returnExpr, tCalcTable *calcTab);
static long CreateIfStmt
(tArg *stmtArg, tIfData *ifData, tCalcTable *calcTab);
static long CreateForStmt
(tArg *stmtArg, tForData *forData, tCalcTable *calcTab);
static long CreateWhileStmt
(tArg *stmtArg, tWhileData *whileData, tCalcTable *calcTab);
static long CreateCaseStmt
(tArg *stmtArg, tCaseData *caseData, tCalcTable *calcTab);


static int  CheckTreeType(tExprTree *exprTree);
static long ResetLeafConstants(tExprTree *exprTree, int type);
static long AllocateExprCalc(tExprCalc **pExprCalc, char evalIdx);
static long GetExprEvalFunc(tArg *exprArg, tExprTree *exprTree, tCalcTable *calcTab);
static long CreateExprList
(tExprCalc **exprList, tExprCalc *parent, 
  tExprTree *exprTree, tCalcTable *calcTab, char evalIdx);
static long CreateExprArg
(tArg *calcArg, tExprTree *exprTree, tCalcTable *calcTab);
static long CreateExprStmt
(tArg *stmtArg, tExprTree *exprTree, tCalcTable *calcTab);
static long CreateInPlaceCalc
(tArg *stmtArg, tExprTree *exprTree, tCalcTable *calcTab);
static long CreateOperatorFunc
(tExprCalc *exprCalc, tOp op, tCalcTable *calcTab);
static long CreateOperandArg
(tArg *arg, tExprBranch *exprBranch, tCalcTable *calcTab, int ref, int numeric);

static long SetAddrEval(int type, tArg *arg, tCalcTable *calcTab);
static long SetVarArg(tEngVar *var, tArg *arg, int reference, tCalcTable *calcTab);
static long SetParamArg(tEngVar *var, tArg *arg, int reference, tCalcTable *calcTab);
static long SetVarCalcArg(tEngVar *var, tArg *arg, tCalcTable *calcTab);
static long SetRecFldArg(tRecUse *dataFld, tArg *arg, int reference, tCalcTable *calcTab);
static long CreateRecFieldArg
(tArg *arg, tRecUse *dataFld, tCalcTable *calcTab, int ref, int numeric);
static long CreateStringToNumericArg
(tArg *arg, tRecUse *dataFld, tCalcTable *calcTab);
static long CreateVariableArg
(tArg *arg, tEngVar *var, tCalcTable *calcTab, int reference);
static long CreateArrayVarArg
(tArg *arg, tArrayUse *arrayUse, tCalcTable *calcTab, int reference);
static long CreateFuncCallArg
(tArg *arg, tFuncCall *funcCall, tCalcTable *calcTab);
static long CreateRBExecArg
(tArg *arg, tEngRBData *RBExec, tCalcTable *calcTab);
static long CreateRuleCallArg
(tArg *arg, tRuleCall *ruleCall, tCalcTable *calcTab);
static long CreateInCallArg
(tArg *arg, tInCall *inCall, tCalcTable *calcTab);
static long CreateArrayProcArg
(tArg *arg, tRecUse *dataFld, tCalcTable *calcTab);
static long CreateSizeofArg
(tArg *arg, tSizeOf *sizeOp, tCalcTable *calcTab);
static long CreateExistsArg
(tArg *arg, tRecUse *dataFld, tCalcTable *calcTab);
static long CreateErrstrArg
(tArg *arg, tCalcTable *calcTab);
static long CreateErrnoArg
(tArg *arg, tCalcTable *calcTab);
static long CreateResolveArg
(tArg *arg, tResolve *resolve, tCalcTable *calcTab);
static long CreateStatementArg
(tArg *arg, tStatement *statementList, tCalcTable *calcTab);

static long UpdatingValue(tExprTree *exprTree);
static long UpdatingVariable(tEngVar *var, tExprTree *exprTree);

extern long FreeStatementList(tStatement *stmtList);

static tCalcTable *gCalcTableList;


#define NEED_ADJUST(arg) (arg->flags&ATOM_NOT || arg->flags&ATOM_NEGATE)



/*--------------------------------------------------------------------------*/
/* CreateResolveArg()                                                       */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long CreateResolveArg
(tArg *arg, tResolve *resolve, tCalcTable *calcTab)
{
   tArgCalc  *resCalc;
   tArg      *newArg;
   long      status;

   /* Set the argument's EvalFunc */
   if(NEED_ADJUST(arg)) arg->func = GetArgFunc(calcTab, EvalResolveAdj);
   else arg->func = GetArgFunc(calcTab, EvalResolve);

   /* Allocate a new arg calc struct */
   if((resCalc = (tArgCalc*)MyMalloc(sizeof(tArgCalc)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)resCalc, 0, sizeof(tArgCalc));

   /* First arg in the length of the data */
   if((newArg = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
      throw(MALLOC_EXCEPTION);
   resCalc->argList = newArg;
   memset((char*)newArg, 0, sizeof(tArg));
   if((status=CreateExprArg(newArg, resolve->lenExpr, calcTab)))
      return(status);
 

   /* Next arg is an the record data structure */
   if((newArg->next = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
      throw(MALLOC_EXCEPTION);
   newArg = newArg->next;
   memset((char*)newArg, 0, sizeof(tArg));
 
   newArg->atom.type = RECORD_TYPE;
   newArg->flags |= ATOM_ADDRESS;
   newArg->atom.data.uVal = (void*)resolve->recData;

   /* Set the calcNode in the arg */
   arg->atom.type = NUMERIC_TYPE;
   arg->flags |= ATOM_ARG_CALC;
   arg->atom.data.uVal = (void*)resCalc;


   /* Last arg, if any, is the expression to get the address of the data */
   if(resolve->addrExpr)
   {  if((newArg->next = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
         throw(MALLOC_EXCEPTION);
      newArg = newArg->next;
      memset((char*)newArg, 0, sizeof(tArg));
      if((status=CreateExprArg(newArg, resolve->addrExpr, calcTab)))
         return(status);
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CreateErrstrArg()                                                        */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long CreateErrstrArg(tArg *arg, tCalcTable *calcTab)
{
   /* Set the argument's EvalFunc */
   arg->func = GetArgFunc(calcTab, EvalErrorStr);

   arg->atom.type = HNC_STRING;
   arg->flags |= ATOM_STRING;
   arg->flags |= ATOM_ERROR;
   arg->flags |= ATOM_ADDRESS;

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CreateErrnoArg()                                                         */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long CreateErrnoArg(tArg *arg, tCalcTable *calcTab)
{
   /* Set the argument's EvalFunc */
   if(NEED_ADJUST(arg)) arg->func = GetArgFunc(calcTab, EvalErrorNumAdj);
   else arg->func = GetArgFunc(calcTab, EvalErrorNum);

   arg->atom.type = HNC_INT32;
   arg->flags |= ATOM_ERROR;
   arg->flags |= ATOM_ADDRESS;

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CreateExistsArg()                                                        */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long CreateExistsArg
(tArg *arg, tRecUse *dataFld, tCalcTable *calcTab)
{
   tArgCalc *argCalc;
   tArg     *newArg;
   long     status;

   /* Set the argument's EvalFunc */
   if(NEED_ADJUST(arg)) arg->func = GetArgFunc(calcTab, EvalExistsAdj);
   else arg->func = GetArgFunc(calcTab, EvalExists);

   /* Allocate a calc node that will either check the flags, or will */
   /* resolve the array to determine whether a data field exists or  */
   /* not.                                                           */ 
   if((argCalc = (tArgCalc*)MyMalloc(sizeof(tArgCalc)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)argCalc, 0, sizeof(tArgCalc));

   /* Create an argument */
   if((newArg = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
      throw(MALLOC_EXCEPTION);
   argCalc->argList = newArg;
   memset((char*)newArg, 0, sizeof(tArg));

   /*********************************************************************/
   /* If no array processing in data field, only argument is a pointer  */
   /* to the flags of the record field.  Otherwise, create an array     */
   /* proc argument                                                     */
   /*********************************************************************/
   if(!dataFld->arrayProc)
   {  newArg->flags |= ATOM_ADDRESS;
      newArg->atom.type = HNC_INT32;
      newArg->atom.data.uVal = (void*)&dataFld->fldData->flags;
      newArg->func = GetArgFunc(calcTab, EvalLongAddr);

      /* Free dataFld because it has no further use */
      MyFree((void*)dataFld->fieldName);
      MyFree((void*)dataFld->recordName);
      MyFree((void*)dataFld);
   }
   else
   {  /* Make it a reference so it doesn't create a value if the */
      /* field does not exist                                    */
      newArg->flags |= ATOM_REFERENCE;
      if((status=CreateArrayProcArg(newArg, dataFld, calcTab)))
         return(status);
   }

   /* Set the calcNode in the arg */
   arg->atom.type = NUMERIC_TYPE;
   arg->flags |= ATOM_ARG_CALC;
   arg->atom.data.uVal = (void*)argCalc;

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CreateSizeofArg()                                                       */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long CreateSizeofArg
(tArg *arg, tSizeOf *sizeOp, tCalcTable *calcTab)
{
   tArgCalc  *argCalc;
   tArg      *newArg;
   long      status;

   /*********************************************************************/
   /* If it is direct, then the size is put directly into the argument. */
   /* Otherwise, have to build a calc node to do run the array proc     */
   /* and get the field.                                                */
   /*********************************************************************/
   if(sizeOp->type == SIZE_DIRECT)
   {  /* Set the argument's EvalFunc */
      if(arg->flags&ATOM_NOT) arg->func = GetArgFunc(calcTab, EvalLongAddrNot);
      else if(arg->flags&ATOM_NEGATE) arg->func = GetArgFunc(calcTab, EvalLongAddrNeg);
      else arg->func = GetArgFunc(calcTab, EvalLongAddr);

      arg->flags |= ATOM_ADDRESS;
      arg->atom.type = HNC_INT32;
      arg->atom.data.uVal = (long*)sizeOp->size;
   }
   else
   {  /* Set the argument's EvalFunc */
      if(NEED_ADJUST(arg)) arg->func = GetArgFunc(calcTab, EvalSizeofAdj);
      else arg->func = GetArgFunc(calcTab, EvalSizeof);

      /* Allocate a new calc node struct */
      if((argCalc = (tArgCalc*)MyMalloc(sizeof(tArgCalc)))==NULL)
         throw(MALLOC_EXCEPTION);
      memset((char*)argCalc, 0, sizeof(tArgCalc));

      /* Create an array proc argument */
      if((newArg = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
         throw(MALLOC_EXCEPTION);
      argCalc->argList = newArg;
      memset((char*)newArg, 0, sizeof(tArg));

      if((status=CreateArrayProcArg(newArg, (tRecUse*)sizeOp->size, calcTab)))
         return(status);

      /* Set the calcNode in the arg */
      arg->atom.type = NUMERIC_TYPE;
      arg->flags |= ATOM_ARG_CALC;
      arg->atom.data.uVal = (void*)argCalc;
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CreateRBExecArg()                                                        */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long CreateRBExecArg
(tArg *arg, tEngRBData *RBExec, tCalcTable *calcTab)
{
   tArg       *newArg;
   tCalcTable *tmpCT;
   tArgCalc   *argCalc;
   
   /* Set the argument's EvalFunc */
   if(NEED_ADJUST(arg)) arg->func = GetArgFunc(calcTab, EvalRBExecAdj);
   else arg->func = GetArgFunc(calcTab, EvalRBExec);

   /**********************/
   /* Set up an arg calc */
   /***********************/
   if((argCalc = (tArgCalc*)MyMalloc(sizeof(tArgCalc)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)argCalc, 0, sizeof(tArgCalc));
   arg->atom.data.uVal = (void*)argCalc;
   arg->flags |= ATOM_ARG_CALC;

   /***************************************************/
   /* Set the RBExec handle with the right calc table */
   /***************************************************/
   for(tmpCT=gCalcTableList; tmpCT; tmpCT=tmpCT->next)
      if(tmpCT->ruleBaseData == RBExec->RBData)
      {  RBExec->handle = (void*)tmpCT;
         break;
      }

   /**********************************************/
   /* The first argument is the rule base handle */
   /**********************************************/
   /* Allocate the argument */
   if((newArg = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
      throw(MALLOC_EXCEPTION);
   argCalc->argList = newArg;
   memset((char*)newArg, 0, sizeof(tArg));
  
   newArg->flags |= ATOM_ADDRESS;
   newArg->atom.data.uVal = (void*)RBExec->handle;
   
   /*******************************************************/
   /* The second argument is the rule base data structure */
   /*******************************************************/
   /* Allocate the argument */
   if((newArg->next = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
      throw(MALLOC_EXCEPTION);
   newArg = newArg->next;
   memset((char*)newArg, 0, sizeof(tArg));
  
   newArg->flags |= ATOM_ADDRESS;
   newArg->atom.data.uVal = (void*)RBExec->RBData;

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CreateFuncCallArg()                                                      */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long CreateFuncCallArg
(tArg *arg, tFuncCall *funcCall, tCalcTable *calcTab)
{
   long         status;
   tUsedFunc    *usedFunc, *prevFunc=NULL;
   tParamExpr   *paramExpr;
   tArg         *newArg=NULL;
   tFuncCalc    *funcCalc;

   /* Set the argument's EvalFunc */
   if(NEED_ADJUST(arg)) arg->func = GetArgFunc(calcTab, EvalLinkedFuncAdj);
   else arg->func = GetArgFunc(calcTab, EvalLinkedFunc);
   
   /**********************************************************/
   /* Check if the function is in the list of used functions */
   /**********************************************************/
   usedFunc = calcTab->usedFuncList;
   while(usedFunc) 
   {  if(usedFunc->func == funcCall->funcData)
         break;
      
      prevFunc = usedFunc;
      usedFunc = usedFunc->next;
   }

   /**************************************************************/
   /* Function not found so add it to the list of used functions */
   /**************************************************************/
   if(!usedFunc) 
   {  if((usedFunc = (tUsedFunc*)MyMalloc(sizeof(tUsedFunc)))==NULL)
         throw(MALLOC_EXCEPTION);
      memset((char*)usedFunc, 0, sizeof(tUsedFunc));
      
      if(prevFunc) prevFunc->next = usedFunc;
      else calcTab->usedFuncList = usedFunc;
      
      usedFunc->func = funcCall->funcData;

      /* Set flags if have support functions */
      if(usedFunc->func->SysInitFuncPtr)
         calcTab->haveSysInitFuncs = 1;
      if(usedFunc->func->SysTermFuncPtr)
         calcTab->haveSysTermFuncs = 1;
      if(usedFunc->func->IterInitFuncPtr)
         calcTab->haveIterInitFuncs = 1;
      if(usedFunc->func->IterTermFuncPtr)
         calcTab->haveIterTermFuncs = 1;
   }
   

   /**********************/
   /* Set up a func calc */
   /***********************/
   if((funcCalc = (tFuncCalc*)MyMalloc(sizeof(tFuncCalc)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)funcCalc, 0, sizeof(tFuncCalc));
   arg->atom.data.uVal = (void*)funcCalc;
   arg->flags |= ATOM_FUNC_CALC;

   /* Set up the calc node with the function pointer */
   funcCalc->funcData = funcCall->funcData;

   /* Set the current rule data in the structure */
   funcCalc->ruleData = funcCall->ruleData;
   

   /****************************************/
   /* Set up the arguments to the function */
   /****************************************/
   paramExpr = funcCall->paramList;
   while(paramExpr)
   {  /*****************************/
      /* Allocate the new argument */
      /*****************************/
      if(funcCalc->argList)
      {  if((newArg->next = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
            throw(MALLOC_EXCEPTION);
         newArg = newArg->next;
      }
      else
      {  if((newArg = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
            throw(MALLOC_EXCEPTION);
         funcCalc->argList = newArg;
      }
      memset((char*)newArg, 0, sizeof(tArg));
      
      /**********************************/
      /* Create an expression calc node */
      /**********************************/
      if((status=CreateExprArg(newArg, paramExpr->exprTree, calcTab)))
         return(status);
      
      paramExpr = paramExpr->next;
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CreateRuleCallArg()                                                      */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long CreateRuleCallArg
(tArg *arg, tRuleCall *ruleCall, tCalcTable *calcTab)
{
   long         status;
   tParamExpr   *paramExpr;
   tRuleCalc    *ruleCalc;
   tArg         *newArg;
   tArgCalc     *argCalc;

   /* Set the argument's EvalFunc */
   if(NEED_ADJUST(arg)) arg->func = GetArgFunc(calcTab, EvalRuleAdj);
   else arg->func = GetArgFunc(calcTab, EvalRule);
   
   /**************************************************************/
   /* Get the calc node for the rule.  If it doesn't exist, then */
   /* create it and add it to the front of the ruleCalcList.     */
   /**************************************************************/
   if((status=GetCalcForRule(calcTab, ruleCall->ruleData, EXEC_RULE, &ruleCalc)))
      return(status);

   /**********************/
   /* Set up an arg calc */
   /***********************/
   if((argCalc = (tArgCalc*)MyMalloc(sizeof(tArgCalc)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)argCalc, 0, sizeof(tArgCalc));
   arg->atom.data.uVal = (void*)argCalc;
   arg->flags |= ATOM_ARG_CALC;
   

   /****************************************/
   /* The first argument is rule calc node */
   /****************************************/
   if((newArg = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)newArg, 0, sizeof(tArg));
   newArg->atom.type = ruleCall->ruleData->returnType;
   newArg->atom.data.uVal = (void*)ruleCalc;
   newArg->flags |= ATOM_ADDRESS;
   argCalc->argList = newArg;


   /*********************************************************/
   /* The remaining arguments are the parameter expressions */
   /*********************************************************/
   paramExpr = ruleCall->paramList;
   while(paramExpr)
   {  /*****************************/
      /* Allocate the new argument */
      /*****************************/
      if(argCalc->argList)
      {  if((newArg->next = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
            throw(MALLOC_EXCEPTION);
         newArg = newArg->next;
      }
      else
      {  if((newArg = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
            throw(MALLOC_EXCEPTION);
         argCalc->argList = newArg;
      }
      memset((char*)newArg, 0, sizeof(tArg));
      
      /**********************************/
      /* Create an expression calc node */
      /**********************************/
      if((status=CreateExprArg(newArg, paramExpr->exprTree, calcTab)))
         return(status);
      
      paramExpr = paramExpr->next;
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CreateInCallArg()                                                        */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long CreateInCallArg
(tArg *arg, tInCall *inCall, tCalcTable *calcTab)
{
   long       status, haveString=0;
   tArg       *newArg;
   tParamExpr *paramExpr;
   tArgCalc   *argCalc;
 


   /**********************/
   /* Set up an arg calc */
   /***********************/
   if((argCalc = (tArgCalc*)MyMalloc(sizeof(tArgCalc)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)argCalc, 0, sizeof(tArgCalc));
   arg->atom.data.uVal = (void*)argCalc;
   arg->flags |= ATOM_ARG_CALC;
 
   /***************************************************************/
   /* The first argument is the key expression, and the following */
   /* arguments are the list that the key is to be compared with  */
   /***************************************************************/
   if((newArg = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)newArg, 0, sizeof(tArg));
   argCalc->argList = newArg;
      
   /**********************************/
   /* Create an expression calc node */
   /**********************************/
   if((status=CreateExprArg(newArg, inCall->keyExpr, calcTab)))
      return(status);
 
   /**********************/
   /* Set up the IN list */
   /**********************/
   paramExpr = inCall->paramList;
   while(paramExpr)
   {  /*****************************/
      /* Allocate the new argument */
      /*****************************/
      if(argCalc->argList)
      {  if((newArg->next = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
            throw(MALLOC_EXCEPTION);
         newArg = newArg->next;
      }
      else
      {  if((newArg = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
            throw(MALLOC_EXCEPTION);
         argCalc->argList = newArg;
      }
      memset((char*)newArg, 0, sizeof(tArg));

      if(paramExpr->exprTree->type == HNC_STRING ||
         paramExpr->exprTree->type == HNC_CHAR)
         haveString++;
      
      /**********************************/
      /* Create an expression calc node */
      /**********************************/
      if((status=CreateExprArg(newArg, paramExpr->exprTree, calcTab)))
         return(status);

      paramExpr = paramExpr->next;
   }

   /*******************************************************/
   /* Set the correct Eval function according to the type */
   /*******************************************************/
   if(inCall->keyExpr->type == HNC_STRING || haveString)
   {  argCalc->argList->flags |= ATOM_STRING;
      if(NEED_ADJUST(arg)) arg->func = GetArgFunc(calcTab, EvalStringInAdj);
      else arg->func = GetArgFunc(calcTab, EvalStringIn);
   }
   else
   {  if(NEED_ADJUST(arg)) arg->func = GetArgFunc(calcTab, EvalNumericInAdj);
      else arg->func = GetArgFunc(calcTab, EvalNumericIn);
   }
   
   return(OK); 
}


/*--------------------------------------------------------------------------*/
/* CreateVarCalc()                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long CreateVarCalc
(tVarCalc **pVarCalc, tEngVar *var, tCalcTable *calcTab)
{
   tVarCalc   *prevNode=NULL, *tmpCalc, *saveCalc;
   long       status;


   /**********************************************/
   /* Add the calc rule for this variable to the */
   /* shared calc list                           */
   /**********************************************/
   /* See if already exists */
   tmpCalc = calcTab->varCalcList;
   while(tmpCalc)
   {  if(!strcmp(var->varName, tmpCalc->id))
         break;

      prevNode = tmpCalc;
      tmpCalc = tmpCalc->next;
   }

   /********************************************************************/
   /* If the var calc node does not exist, and if the variable has not */
   /* already been converted, which indicates that it belongs to       */
   /* another shared list, create the calc node for the variable and   */
   /* add it to the list                                               */
   /********************************************************************/
   if(!tmpCalc && !var->calcRule->converted)
   {  /* Allocate a new calc node struct */
      if((tmpCalc = (tVarCalc*)MyMalloc(sizeof(tVarCalc)))==NULL)
         throw(MALLOC_EXCEPTION);
      memset((char*)tmpCalc, 0, sizeof(tVarCalc));

      /* Set the id */
      if((tmpCalc->id = (char*)MyMalloc(strlen(var->varName)+1))==NULL)
         throw(MALLOC_EXCEPTION);
      strcpy(tmpCalc->id, var->varName);

      /* Link the new calc node into the list of var calcs */
      if(prevNode) prevNode->next = tmpCalc;
      else calcTab->varCalcList = tmpCalc;
   
      /* Set the variable */
      tmpCalc->var = var;

      /* Save current varCalc and put in current varCalc */
      saveCalc = calcTab->calcVar;
      calcTab->calcVar = tmpCalc;

      /* Create an argument which is the method to calculate the variable */
      if((tmpCalc->arg = (tArg*)MyMalloc(sizeof(tArg)))==NULL) 
         throw(MALLOC_EXCEPTION);
      memset((char*)tmpCalc->arg, 0, sizeof(tArg));
      if((status=CreateStatementArg(tmpCalc->arg, 
                     (tStatement*)var->calcRule->data, calcTab)))
         return(status);

      /* Restore saved calc */
      calcTab->calcVar = saveCalc;

      /* Free the statement list, and set data to the calcNode */
      FreeStatementList((tStatement*)var->calcRule->data);

      var->calcRule->data = (void*)tmpCalc;
      var->calcRule->converted = TRUE;
   }
   else if(var->calcRule->converted)
      tmpCalc = (tVarCalc*)var->calcRule->data;
   
   /**************************************/
   /* Return the calc node, if necessary */
   /**************************************/
   if(pVarCalc) *pVarCalc = tmpCalc;

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CreateArrayVarArg()                                                      */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long CreateArrayVarArg
(tArg *arg, tArrayUse *arrayUse, tCalcTable *calcTab, int reference)
{
   tArg      *newArg;
   tArgCalc  *arrayCalc;
   long      status;

   /* Set the argument's EvalFunc */
   if(NEED_ADJUST(arg)) arg->func = GetArgFunc(calcTab, EvalArrayVarAdj);
   else arg->func = GetArgFunc(calcTab, EvalArrayVar);


   /*********************************/
   /* Set up the arg as a calc node */
   /*********************************/
   arg->atom.type = arrayUse->arrayData->array->type;
   arg->flags |= ATOM_ARG_CALC;
   arg->flags |= ATOM_ADDRESS;
   if(reference)
      arg->flags |= ATOM_REFERENCE;

   /* Set arg flag for string if necessary */
   if(IsString(arrayUse->arrayData->array->type)) 
      arg->flags |= ATOM_STRING;

   /* Allocate node */
   if((arrayCalc = (tArgCalc*)MyMalloc(sizeof(tArgCalc)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)arrayCalc, 0, sizeof(tArgCalc));
   arg->atom.data.uVal = (void*)arrayCalc;


   /***************************************************/
   /* First argument is the address of the array data */
   /***************************************************/
   if((newArg = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
      throw(MALLOC_EXCEPTION);
   arrayCalc->argList = newArg;
   memset((char*)newArg, 0, sizeof(tArg));
   newArg->flags |= ATOM_ADDRESS;
   newArg->atom.data.uVal = (void*)arrayUse->arrayData;


   /*******************************************/
   /* Second argument is the index expression */
   /*******************************************/
   if((newArg->next = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
      throw(MALLOC_EXCEPTION);
   newArg = newArg->next;
   memset((char*)newArg, 0, sizeof(tArg));
   if((status=CreateExprArg(newArg, (tExprTree*)arrayUse->indexExpr, calcTab)))
      return(status);


   /*********************************************/
   /* Third argument is a place holder for data */
   /*********************************************/
   if((newArg->next = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
      throw(MALLOC_EXCEPTION);
   newArg = newArg->next;
   memset((char*)newArg, 0, sizeof(tArg));

   newArg->atom.type = arrayUse->arrayData->array->type;
   newArg->flags |= ATOM_ADDRESS;
   if(reference)
      newArg->flags |= ATOM_REFERENCE;
 
   /* Set the appropriate EvalFunc */
   if(reference)
      newArg->func = GetArgFunc(calcTab, EvalAsIs);
   else
      SetAddrEval(arrayUse->arrayData->array->type, newArg, calcTab);
 
   /* Set arg flag for string if necessary */
   if(IsString(arrayUse->arrayData->array->type))
      newArg->flags |= ATOM_STRING;
 
   return(OK);
}


/*--------------------------------------------------------------------------*/
/* SetAddrEval()                                                             */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long SetAddrEval(int type, tArg *arg, tCalcTable *calcTab)
{
   switch(type)
   {  case NUMERIC_TYPE:
         if(arg->flags&ATOM_NOT) arg->func = GetArgFunc(calcTab, EvalNumericAddrNot);
         else if(arg->flags&ATOM_NEGATE) arg->func = GetArgFunc(calcTab, EvalNumericAddrNeg);
         else arg->func = GetArgFunc(calcTab, EvalNumericAddr);
         break;  
      case HNC_FLOAT:
         if(arg->flags&ATOM_NOT) arg->func = GetArgFunc(calcTab, EvalFloatAddrNot);
         else if(arg->flags&ATOM_NEGATE) arg->func = GetArgFunc(calcTab, EvalFloatAddrNeg);
         else arg->func = GetArgFunc(calcTab, EvalFloatAddr);
         break;  
      case HNC_DOUBLE:
         if(arg->flags&ATOM_NOT) arg->func = GetArgFunc(calcTab, EvalDoubleAddrNot);
         else if(arg->flags&ATOM_NEGATE) arg->func = GetArgFunc(calcTab, EvalDoubleAddrNeg);
         else arg->func = GetArgFunc(calcTab, EvalDoubleAddr);
         break;  
      case HNC_INT16:
         if(arg->flags&ATOM_NOT) arg->func = GetArgFunc(calcTab, EvalShortAddrNot);
         else if(arg->flags&ATOM_NEGATE) arg->func = GetArgFunc(calcTab, EvalShortAddrNeg);
         else arg->func = GetArgFunc(calcTab, EvalShortAddr);
         break;  
      case HNC_UINT16:
         if(arg->flags&ATOM_NOT) arg->func = GetArgFunc(calcTab, EvalUShortAddrNot);
         else if(arg->flags&ATOM_NEGATE) arg->func = GetArgFunc(calcTab, EvalUShortAddrNeg);
         else arg->func = GetArgFunc(calcTab, EvalUShortAddr);
         break;  
      case HNC_INT32:
         if(arg->flags&ATOM_NOT) arg->func = GetArgFunc(calcTab, EvalLongAddrNot);
         else if(arg->flags&ATOM_NEGATE) arg->func = GetArgFunc(calcTab, EvalLongAddrNeg);
         else arg->func = GetArgFunc(calcTab, EvalLongAddr);
         break;  
      case HNC_UINT32:
         if(arg->flags&ATOM_NOT) arg->func = GetArgFunc(calcTab, EvalULongAddrNot);
         else if(arg->flags&ATOM_NEGATE) arg->func = GetArgFunc(calcTab, EvalULongAddrNeg);
         else arg->func = GetArgFunc(calcTab, EvalULongAddr);
         break;  
      default:
         arg->func = GetArgFunc(calcTab, EvalAsIs);
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* SetVarArg()                                                              */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long SetVarArg(tEngVar *var, tArg *arg, int reference, tCalcTable *calcTab)
{
   /* Set the argument type */
   arg->atom.type = var->type;

   /* Set the argument flags */
   if(reference) arg->flags |= ATOM_REFERENCE;
   arg->flags |= ATOM_ADDRESS;

   /* Set the argument data */
   if(IsString(var->type))
   {  arg->flags |= ATOM_STRING;
      arg->atom.data.strVal.str = (char*)var->data;
      arg->atom.data.strVal.len = var->dataSize;
   }
   else
   {  if(var->type == RECORD_TYPE)
         arg->atom.data.uVal = (void*)((tRecInstance*)var->data)->data;
      else
         arg->atom.data.uVal = (void*)var->data;
   }

   /* Set the appropriate EvalFunc */
   if(reference)
      arg->func = GetArgFunc(calcTab, EvalAsIs);   
   else
      SetAddrEval(var->type, arg, calcTab);

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* SetParamArg()                                                            */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long SetParamArg(tEngVar *var, tArg *arg, int reference, tCalcTable *calcTab)
{
   /* Set the type */
   arg->atom.type = var->type;

   /* Set the flags */
   arg->flags |= ATOM_PARAMETER;
   if(reference) arg->flags |= ATOM_REFERENCE;

   /* Set the data, which is a pointer to the argument */
   arg->atom.data.uVal = (void*)var;

   /* Set the appropriate EvalFunc */
   if(IsString(var->type))
   {  arg->flags |= ATOM_STRING;
      arg->func = GetArgFunc(calcTab, EvalStringParam);
   }
   else if(reference) 
      arg->func = GetArgFunc(calcTab, EvalParamData);
   else
   {  switch(var->type)
      {  case NUMERIC_TYPE:
            if(arg->flags&ATOM_NOT) 
               arg->func = GetArgFunc(calcTab, EvalNumericParamNot);
            else if(arg->flags&ATOM_NEGATE) 
               arg->func = GetArgFunc(calcTab, EvalNumericParamNeg);
            else 
               arg->func = GetArgFunc(calcTab, EvalNumericParam);
            break;  
         case HNC_FLOAT:
            if(arg->flags&ATOM_NOT) 
               arg->func = GetArgFunc(calcTab, EvalFloatParamNot);
            else if(arg->flags&ATOM_NEGATE) 
               arg->func = GetArgFunc(calcTab, EvalFloatParamNeg);
            else 
               arg->func = GetArgFunc(calcTab, EvalFloatParam);
            break;  
         case HNC_DOUBLE:
            if(arg->flags&ATOM_NOT) 
               arg->func = GetArgFunc(calcTab, EvalDoubleParamNot);
            else if(arg->flags&ATOM_NEGATE) 
               arg->func = GetArgFunc(calcTab, EvalDoubleParamNeg);
            else 
               arg->func = GetArgFunc(calcTab, EvalDoubleParam);
            break;  
         case HNC_INT16:
            if(arg->flags&ATOM_NOT) 
               arg->func = GetArgFunc(calcTab, EvalShortParamNot);
            else if(arg->flags&ATOM_NEGATE) 
               arg->func = GetArgFunc(calcTab, EvalShortParamNeg);
            else 
               arg->func = GetArgFunc(calcTab, EvalShortParam);
            break;  
         case HNC_UINT16:
            if(arg->flags&ATOM_NOT) 
               arg->func = GetArgFunc(calcTab, EvalUShortParamNot);
            else if(arg->flags&ATOM_NEGATE) 
               arg->func = GetArgFunc(calcTab, EvalUShortParamNeg);
            else 
               arg->func = GetArgFunc(calcTab, EvalUShortParam);
            break;  
         case HNC_INT32:
            if(arg->flags&ATOM_NOT) 
               arg->func = GetArgFunc(calcTab, EvalLongParamNot);
            else if(arg->flags&ATOM_NEGATE) 
               arg->func = GetArgFunc(calcTab, EvalLongParamNeg);
            else 
               arg->func = GetArgFunc(calcTab, EvalLongParam);
            break;  
         case HNC_UINT32:
            if(arg->flags&ATOM_NOT) 
               arg->func = GetArgFunc(calcTab, EvalULongParamNot);
            else if(arg->flags&ATOM_NEGATE) 
               arg->func = GetArgFunc(calcTab, EvalULongParamNeg);
            else 
               arg->func = GetArgFunc(calcTab, EvalULongParam);
            break;  
         default:
            arg->func = GetArgFunc(calcTab, EvalParamData);
            break;  
      }
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* SetVarCalcArg()                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long SetVarCalcArg(tEngVar *var, tArg *arg, tCalcTable *calcTab)
{
   long     status;
   tVarCalc *varCalc;

   /* Create a tVarCalc structure for the calc */
   if((status=CreateVarCalc(&varCalc, var, calcTab)))
      return(status);
   arg->atom.data.uVal = (void*)varCalc;

   /* Set the flags */
   arg->flags |= ATOM_VAR_CALC;

   /* Set the appropriate EvalFunc */
   if(IsString(var->type))
      arg->func = GetArgFunc(calcTab, EvalStringVarCalc);
   else
   {  switch(var->type)
      {  case NUMERIC_TYPE:
            if(NEED_ADJUST(arg)) arg->func = GetArgFunc(calcTab, EvalNumericVarCalcAdj);
            else arg->func = GetArgFunc(calcTab, EvalNumericVarCalc);
            break;  
         case HNC_FLOAT:
            if(NEED_ADJUST(arg)) arg->func = GetArgFunc(calcTab, EvalFloatVarCalcAdj);
            else arg->func = GetArgFunc(calcTab, EvalFloatVarCalc);
            break;  
         case HNC_DOUBLE:
            if(NEED_ADJUST(arg)) arg->func = GetArgFunc(calcTab, EvalDoubleVarCalcAdj);
            else arg->func = GetArgFunc(calcTab, EvalDoubleVarCalc);
            break;  
         case HNC_INT16:
            if(NEED_ADJUST(arg)) arg->func = GetArgFunc(calcTab, EvalShortVarCalcAdj);
            else arg->func = GetArgFunc(calcTab, EvalShortVarCalc);
            break;  
         case HNC_UINT16:
            if(NEED_ADJUST(arg)) arg->func = GetArgFunc(calcTab, EvalUShortVarCalcAdj);
            else arg->func = GetArgFunc(calcTab, EvalUShortVarCalc);
            break;  
         case HNC_INT32:
            if(NEED_ADJUST(arg)) arg->func = GetArgFunc(calcTab, EvalLongVarCalcAdj);
            else arg->func = GetArgFunc(calcTab, EvalLongVarCalc);
            break;  
         case HNC_UINT32:
            if(NEED_ADJUST(arg)) arg->func = GetArgFunc(calcTab, EvalULongVarCalcAdj);
            else arg->func = GetArgFunc(calcTab, EvalULongVarCalc);
            break;  
         default:
            arg->func = GetArgFunc(calcTab, EvalVarCalcData);
            break;  
      }
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* SetRecFldArg()                                                           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long SetRecFldArg(tRecUse *dataFld, tArg *arg, int reference, tCalcTable *calcTab)
{
   int fixed=1;

   /* Set the fixed flag to 0 for non-fixed records */
   if(dataFld->fldData->data)
   {   /* If there is data for the field and it is not a record */
       /* or array type, then check the tFlatData               */
       if(dataFld->type != RECORD_TYPE && dataFld->type != ARRAY_TYPE)
       {   if(!((tFlatData*)dataFld->fldData->data)->data)
              fixed = 0;
       }
   }
   else
      fixed = 0;

   /* Set type and flags */
   arg->atom.type = dataFld->type;

   /* Set up flags */
   if(reference) arg->flags |= ATOM_REFERENCE;
   arg->flags |= ATOM_ADDRESS;
 
   /* Set the field data */
   if(!fixed) 
   {  if(dataFld->type == RECORD_TYPE || dataFld->type == ARRAY_TYPE)
         arg->atom.data.uVal = (void*)dataFld->fldData;
      else 
         arg->atom.data.uVal = (void*)dataFld->fldData->data;
   }
   else 
   {  if(IsString(dataFld->type))
      {  arg->atom.data.strVal.str = (char*)((tFlatData*)dataFld->fldData->data)->data;
         arg->atom.data.strVal.len = ((tFlatData*)dataFld->fldData->data)->length;
      }
      else if(dataFld->type == RECORD_TYPE || dataFld->type == ARRAY_TYPE)
         arg->atom.data.uVal = (void*)dataFld->fldData->data;
      else 
         arg->atom.data.uVal = (void*)((tFlatData*)dataFld->fldData->data)->data;
   }

   /* Set the appropriate EvalFunc */
   if(IsString(dataFld->type)) 
   {  arg->flags |= ATOM_STRING;
      if(!fixed) arg->func = GetArgFunc(calcTab, EvalFldString);
      else arg->func = GetArgFunc(calcTab, EvalAsIs);
   }
   else if(reference) 
   {  if(!fixed) arg->func = GetArgFunc(calcTab, EvalFldData);
      else arg->func = GetArgFunc(calcTab, EvalAsIs);
   }
   else
   {  if(!fixed)
      {  switch(dataFld->type)
         {  case NUMERIC_TYPE:
               if(arg->flags&ATOM_NOT) 
                  arg->func = GetArgFunc(calcTab, EvalNumericFldNot);
               else if(arg->flags&ATOM_NEGATE) 
                  arg->func = GetArgFunc(calcTab, EvalNumericFldNeg);
               else 
                  arg->func = GetArgFunc(calcTab, EvalNumericFld);
               break;  
            case HNC_FLOAT:
               if(arg->flags&ATOM_NOT) 
                  arg->func = GetArgFunc(calcTab, EvalFloatFldNot);
               else if(arg->flags&ATOM_NEGATE) 
                  arg->func = GetArgFunc(calcTab, EvalFloatFldNeg);
               else 
                  arg->func = GetArgFunc(calcTab, EvalFloatFld);
               break;  
            case HNC_DOUBLE:
               if(arg->flags&ATOM_NOT) 
                  arg->func = GetArgFunc(calcTab, EvalDoubleFldNot);
               else if(arg->flags&ATOM_NEGATE) 
                  arg->func = GetArgFunc(calcTab, EvalDoubleFldNeg);
               else 
                  arg->func = GetArgFunc(calcTab, EvalDoubleFld);
               break;  
            case HNC_INT16:
               if(arg->flags&ATOM_NOT) 
                  arg->func = GetArgFunc(calcTab, EvalShortFldNot);
               else if(arg->flags&ATOM_NEGATE) 
                  arg->func = GetArgFunc(calcTab, EvalShortFldNeg);
               else 
                  arg->func = GetArgFunc(calcTab, EvalShortFld);
               break;  
            case HNC_UINT16:
               if(arg->flags&ATOM_NOT) 
                  arg->func = GetArgFunc(calcTab, EvalUShortFldNot);
               else if(arg->flags&ATOM_NEGATE) 
                  arg->func = GetArgFunc(calcTab, EvalUShortFldNeg);
               else 
                  arg->func = GetArgFunc(calcTab, EvalUShortFld);
               break;  
            case HNC_INT32:
               if(arg->flags&ATOM_NOT) 
                  arg->func = GetArgFunc(calcTab, EvalLongFldNot);
               else if(arg->flags&ATOM_NEGATE) 
                  arg->func = GetArgFunc(calcTab, EvalLongFldNeg);
               else 
                  arg->func = GetArgFunc(calcTab, EvalLongFld);
               break;  
            case HNC_UINT32:
               if(arg->flags&ATOM_NOT) 
                  arg->func = GetArgFunc(calcTab, EvalULongFldNot);
               else if(arg->flags&ATOM_NEGATE) 
                  arg->func = GetArgFunc(calcTab, EvalULongFldNeg);
               else 
                  arg->func = GetArgFunc(calcTab, EvalULongFld);
               break;  
            default:
               arg->func = GetArgFunc(calcTab, EvalFldData);
         }
      }
      else
         SetAddrEval(dataFld->type, arg, calcTab);
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CreateVariableArg()                                                      */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long CreateVariableArg
(tArg *arg, tEngVar *var, tCalcTable *calcTab, int reference)
{
   long     status;

   if(var->flags&VAR_PARAMETER)
   {  if((status=SetParamArg(var, arg, reference, calcTab)))
         return(status);
   }
   else
   {  if(var->calcRule && !var->calcRule->noAutoCalc && !reference)
      {  if((status=SetVarCalcArg(var, arg, calcTab)))
            return(status);
      }
      else
      {  if((status=SetVarArg(var, arg, reference, calcTab)))
            return(status);
      }
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CreateRecFieldArg()                                                      */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long CreateRecFieldArg
(tArg *arg, tRecUse *dataFld, tCalcTable *calcTab, int reference, int numeric)
{
   long  status;
   
   /* Record field may be a numeric string that needs to be converted */
   if(!reference && numeric  &&
        (dataFld->type == HNC_INTSTR ||  
         dataFld->type == HNC_FLOATSTR))
   {  /*********************************************************************/
      /* If it is a string numeric and not a reference parameter, then set */
      /* it up to be converted to an actual numeric variable.              */
      /*********************************************************************/
      if((status=CreateStringToNumericArg(arg, dataFld, calcTab)))
         return(status);
   }
   else
   {  /* If have an array, set up array proc arg */
      /* Otherwise, set up as appropriate        */
      if(dataFld->arrayProc) 
      {  if(reference) arg->flags |= ATOM_REFERENCE;
         if((status=CreateArrayProcArg(arg, dataFld, calcTab))) 
            return(status);
      }
      else
      {  if((status=SetRecFldArg(dataFld, arg, reference, calcTab)))
            return(status);
      }
   }
   
   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CreateStringToNumericArg()                                                */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long CreateStringToNumericArg
(tArg *arg, tRecUse *dataFld, tCalcTable *calcTab)
{
   tStr2NumCalc *tmpCalc, *prevNode=NULL;
   char         fldName[1024];
   long         status;
   
   /****************************************************************/
   /* First check to see if the field has already been set up as a */
   /* shared calc node.                                            */
   /****************************************************************/
   sprintf(fldName, "%s.%s", dataFld->recordName,
                             dataFld->fieldName);
   tmpCalc = calcTab->str2numList;
   while(tmpCalc)
   {  if(!strcmp(fldName, tmpCalc->id))
         break;
          
      prevNode = tmpCalc;
      tmpCalc = tmpCalc->next;
   }

   /******************************************************************/
   /* If the calc node for the field conversion has not already been */
   /* set up, then create a new calc node and add it to the list of  */
   /* shared calc nodes.                                             */
   /******************************************************************/
   if(!tmpCalc)
   {  /* Allocate the structure */
      if((tmpCalc = (tStr2NumCalc*)MyMalloc(sizeof(tStr2NumCalc)))==NULL)
         throw(MALLOC_EXCEPTION);
      memset((char*)tmpCalc, 0, sizeof(tStr2NumCalc));

      /* Set the id */
      if((tmpCalc->id = (char*)MyMalloc(strlen(fldName)+1))==NULL)
         throw(MALLOC_EXCEPTION);
      strcpy(tmpCalc->id, fldName);
      
      /* Link the new calc node into the list of shared calcs */
      if(prevNode) prevNode->next = tmpCalc;
      else calcTab->str2numList = tmpCalc;
      
      /* Set the data field arg */
      tmpCalc->arg.atom.type = dataFld->type;
      tmpCalc->arg.flags |= ATOM_STRING;
      tmpCalc->arg.flags |= ATOM_ADDRESS;

      if(dataFld->arrayProc)
      {  if((status=CreateArrayProcArg(&tmpCalc->arg, dataFld, calcTab)))
            return(status);
      }
      else
      {  tmpCalc->arg.flags |= ATOM_FLDDATA;
         tmpCalc->arg.atom.data.uVal = (void*)dataFld->fldData->data;
         tmpCalc->arg.func = GetArgFunc(calcTab, EvalFldString);
      }
   } 

   /********************************************************/
   /* At this point, we have a calc node that converts the */
   /* string to a numeric value.                           */
   /********************************************************/
   arg->atom.type = dataFld->type;
   arg->atom.data.uVal = (void*)tmpCalc;
   arg->flags |= ATOM_STR2NUM_CALC;

   if(NEED_ADJUST(arg)) arg->func = GetArgFunc(calcTab, EvalStr2NumAdj);
   else arg->func = GetArgFunc(calcTab, EvalStr2Num);
   
   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CreateArrayProcArg()                                                     */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long CreateArrayProcArg
(tArg *arg, tRecUse *dataFld, tCalcTable *calcTab)
{
   long       status;
   tArrayProc *arrayProc;
   tArg       *tmpArg;
   
   /******************************************************/
   /* The argument uVal gets the tRecUse structure, but  */
   /* all expressions need to be converted to calc nodes */
   /******************************************************/
   /* Set up arg */
   arg->atom.type = dataFld->type;
   arg->flags |= ATOM_ARRAYPROC;
   arg->flags |= ATOM_ADDRESS;
   if(IsString(arg->atom.type))
      arg->flags |= ATOM_STRING;
   arg->atom.data.uVal = (void*)dataFld; 

   if(NEED_ADJUST(arg)) arg->func = GetArgFunc(calcTab, EvalArrayProcAdj);
   else arg->func = GetArgFunc(calcTab, EvalArrayProc);

   /* Convert all index expressions to tExprCalc arguments */
   for(arrayProc=dataFld->arrayProc; arrayProc; arrayProc=arrayProc->next)
   {  if(arrayProc->arrayUse)
      {  /* Create a tArg structure to hold the index expression */
         if((tmpArg = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
            throw(MALLOC_EXCEPTION);
         memset((char*)tmpArg, 0, sizeof(tArg));
         if((status=CreateExprArg(tmpArg, 
                     (tExprTree*)arrayProc->arrayUse->indexExpr, calcTab)))
            return(status);

         /* Free the old index expression and set it with the calc node */
         FreeExprTree((tExprTree*)arrayProc->arrayUse->indexExpr); 
         arrayProc->arrayUse->indexExpr = (void*)tmpArg;
      }      
   }

   /* Indicate that the data field is not to be freed with the expression */
   /* because it is used at run-time to resolve the data                  */
   dataFld->noFree = 1;

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CreateOperandArg()                                                       */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long CreateOperandArg
(tArg *arg, tExprBranch *exprBranch, tCalcTable *calcTab, int reference, int numeric)
{
   long status;

   /******************************************************/
   /* Make sure the NOT and NEGATE flags are transferred */
   /******************************************************/
   if(exprBranch->flags&EXPR_NOT) arg->flags |= ATOM_NOT;
   if(exprBranch->flags&EXPR_NEGATE) arg->flags |= ATOM_NEGATE;


   /*****************************************************************/
   /* If the expression is not numeric, then make sure the arg atom */
   /* gets its flag set properly with ATOM_STRING.                  */
   /*****************************************************************/
   if(!numeric) arg->flags |= ATOM_STRING;

   
   /************************************************/
   /* Set up the arg according to the operand type */
   /************************************************/
   if(exprBranch->flags&EXPR_FUNCTION)
   {  /********************************************/
      /* Create a new calc node for function call */
      /********************************************/
      arg->atom.type = exprBranch->val.funcCall->funcData->returnType;
      if((status=CreateFuncCallArg(arg, exprBranch->val.funcCall, calcTab)))
         return(status);
   }
   else if(exprBranch->flags&EXPR_RULE)
   {  /****************************************/
      /* Create a new calc node for rule call */
      /****************************************/
      arg->atom.type = exprBranch->val.ruleCall->ruleData->returnType;
      if((status=CreateRuleCallArg(arg, exprBranch->val.ruleCall, calcTab)))
         return(status);
   }
   else if(exprBranch->flags&EXPR_IN)
   {  /**************************************/
      /* Create a new calc node for in call */
      /**************************************/
      arg->atom.type = NUMERIC_TYPE;
      if((status=CreateInCallArg(arg, exprBranch->val.inCall, calcTab)))
         return(status);
   }
   else if(exprBranch->flags&EXPR_RBEXEC)
   {  /***************************************************/
      /* Create a new calc node for rule bease execution */
      /***************************************************/
      arg->atom.type = NUMERIC_TYPE;
      if((status=CreateRBExecArg(arg, exprBranch->val.RBExec, calcTab)))
         return(status);
   }
   else if(exprBranch->flags&EXPR_FIELD)
   {  if((status=CreateRecFieldArg(arg, exprBranch->val.dataFld, 
                                                calcTab, reference, numeric)))
         return(status);
   }
   else if(exprBranch->flags&EXPR_VARBLE)
   {  if((status=CreateVariableArg(arg, exprBranch->val.var, 
                                                    calcTab, reference)))
         return(status);
   }
   else if(exprBranch->flags&EXPR_ARRAYVAR)
   {  if((status=CreateArrayVarArg(arg, exprBranch->val.arrayUse, 
                                                    calcTab, reference)))
         return(status);
   }
   else if(exprBranch->flags&EXPR_SIZEOF)
   {  if((status=CreateSizeofArg(arg, exprBranch->val.sizeOp, calcTab)))
         return(status);
   }
   else if(exprBranch->flags&EXPR_EXISTS)
   {  if((status=CreateExistsArg(arg, exprBranch->val.dataFld, calcTab)))
         return(status);
   }
   else if(exprBranch->flags&EXPR_ERRSTR)
   {  if((status=CreateErrstrArg(arg, calcTab)))
         return(status);
   }
   else if(exprBranch->flags&EXPR_ERRNO)
   {  if((status=CreateErrnoArg(arg, calcTab)))
         return(status);
   }
   else if(exprBranch->flags&EXPR_RESOLVE)
   {  if((status=CreateResolveArg(arg, exprBranch->val.resolve, calcTab)))
         return(status);
   }
   else if(IsNumeric(exprBranch->type))
   {  arg->atom.type = exprBranch->type;
      arg->flags |= ATOM_CONSTANT;
      arg->func = GetArgFunc(calcTab, EvalAsIs);

      if(arg->atom.type == HNC_FLOAT)
         arg->atom.data.lVal = exprBranch->val.lVal;
      else if(arg->atom.type == HNC_INT32)
         arg->atom.data.fVal = exprBranch->val.fVal;
      else 
         arg->atom.data.num = exprBranch->val.num;
   }
   else if(IsString(exprBranch->type)) 
   {  arg->atom.type = exprBranch->type;
      arg->flags |= ATOM_STRING;
      arg->flags |= ATOM_CONSTANT;

      if((arg->atom.data.strVal.str = 
                   (char*)MyMalloc(strlen(exprBranch->val.str)+1))==NULL)
         throw(MALLOC_EXCEPTION);
      strcpy(arg->atom.data.strVal.str, exprBranch->val.str);
      arg->atom.data.strVal.len = strlen(exprBranch->val.str);
      arg->func = GetArgFunc(calcTab, EvalAsIs);
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CreateOperatorFunc()                                                     */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long CreateOperatorFunc
(tExprCalc *exprCalc, tOp op, tCalcTable *calcTab)
{
   /************************************************/
   /* Next get the built in type for the operation */
   /************************************************/
   switch(op)
   {  case SET:
         exprCalc->func = EVAL_SET;
         break;
      case ANDBOOL:
         exprCalc->func = EVAL_ANDBOOL;
         break;
      case ORBOOL:
         exprCalc->func = EVAL_ORBOOL;
         break;
      case EQUAL:
         exprCalc->func = EVAL_EQUAL;
         break;
      case NOTEQL:
         exprCalc->func = EVAL_NOTEQL;
         break;
      case GRTREQL:
         exprCalc->func = EVAL_GRTREQL;
         break;
      case LESSEQL:
         exprCalc->func = EVAL_LESSEQL;
         break;
      case GRTR:
         exprCalc->func = EVAL_GRTR;
         break;
      case LESS:
         exprCalc->func = EVAL_LESS;
         break;
      case MINUS:
         exprCalc->func = EVAL_MINUS;
         break;
      case PLUS:
         exprCalc->func = EVAL_PLUS;
         break;
      case MULTIPLY:
         exprCalc->func = EVAL_MULTIPLY;
         break;
      case DIVIDE:
         exprCalc->func = EVAL_DIVIDE;
         break;
      case POWER:
         exprCalc->func = EVAL_POWER;
         break;
      default:
         return(UNKNOWN_OPERATOR);
   }
   
   return(OK);
}


/*--------------------------------------------------------------------------*/
/* AllocateExprCalc()                                                       */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long AllocateExprCalc(tExprCalc **pExprCalc, char evalIdx)
{
   tExprCalc *exprCalc;

   /* Allocate the structure */
   if((exprCalc = (tExprCalc*)MyMalloc(sizeof(tExprCalc)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)exprCalc, 0, sizeof(tExprCalc));

   /* Set the evaluate index */
   exprCalc->evalIdx = evalIdx;

   /* Set the return pointer */
   *pExprCalc = exprCalc;

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CreateExprList()                                                         */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long CreateExprList
(tExprCalc **exprList, tExprCalc *parent, 
    tExprTree *exprTree, tCalcTable *calcTab, char evalIdx)
{
   long      status;
   tExprCalc *leftChild=NULL, *rightChild=NULL, *tmpExpr;
   tExprCalc *newCalc, *noopCalc;
   long      numeric, result;
   
   /*********************************************************/
   /* Allocate a calc node for this piece of the expression */
   /*********************************************************/
   if((status=AllocateExprCalc(&newCalc, evalIdx)))
      return(status);

   /*************************************************************/
   /* Create 'numeric' flag, which indicates if the branches of */
   /* the expression should be treated as numerics              */
   /*************************************************************/
   if(exprTree->op != NOOP)
      numeric = (IsNumeric(exprTree->left.type) && IsNumeric(exprTree->right.type));
   else
      numeric = !IsString(exprTree->type);

   /***************************************************************/
   /* Recursively search the left side of the tree for leaf nodes */
   /* and add nodes to the list from the bottom-up.  If left is   */
   /* not a tree, the create the operand.                         */
   /***************************************************************/
   if(exprTree->left.flags&EXPR_TREE)
   {  if((status=CreateExprList(exprList, newCalc, 
                                exprTree->left.val.tree, calcTab, evalIdx)))
         return(status);

      /* If tree is not or negated, then create a NOOP calc to negate it */
      if(exprTree->left.flags&EXPR_NOT || exprTree->left.flags&EXPR_NEGATE)
      {  /* Allocate a EVAL_NOT or an EVAL_NEGATE expression */
         if((result=AllocateExprCalc(&noopCalc, evalIdx)))
            return(result);

         /* Set the function, NOT takes precedence of NEGATE if both */
         if(exprTree->left.flags&EXPR_NOT) noopCalc->func = EVAL_NOT;
         else if(exprTree->left.flags&EXPR_NEGATE) noopCalc->func = EVAL_NEGATE;

         /* Put this arg in as the last expression in the list */
         for(tmpExpr=*exprList; tmpExpr->next; tmpExpr=tmpExpr->next);
         tmpExpr->next = noopCalc;
      }

      /* Set the left child */
      for(tmpExpr=*exprList; tmpExpr->next; tmpExpr=tmpExpr->next);
      leftChild = tmpExpr;
   }
   else
   {  /* Allocate a left side argument */
      newCalc->flags |= LEFT_ARG;
      if((newCalc->left = (void*)MyMalloc(sizeof(tArg)))==NULL)
         throw(MALLOC_EXCEPTION);
      memset((char*)newCalc->left, 0, sizeof(tArg));

      if((status=CreateOperandArg((tArg*)newCalc->left, &exprTree->left, calcTab,
                  (exprTree->op==SET || exprTree->reference), numeric)))
         return(status);
   }


   /*****************************************************/
   /* If have a NOOP, then this calc node is not needed */
   /*****************************************************/
   if(exprTree->op == NOOP)
   {  /* Free the newCalc */
      MyFree((void*)newCalc);

      return(OK);
   }
   

   /**************************************/
   /* Next, set up the operator function */
   /**************************************/
   if((status=CreateOperatorFunc(newCalc, exprTree->op, calcTab)))
      return(status);

   /****************************************************************/
   /* Recursively search the right side of the tree for leaf nodes */
   /* and add nodes to the list from the bottom-up.  If right is   */
   /* not a tree, the create the operand.                          */
   /****************************************************************/
   if(exprTree->right.flags&EXPR_TREE)
   {  /* Create the right branch expression */
      if((status=CreateExprList(exprList, newCalc, 
                                     exprTree->right.val.tree, calcTab, (char)(evalIdx+1))))
         return(status);

      /* If tree is not or negated, then create a NOOP calc to negate it */
      if(exprTree->right.flags&EXPR_NOT || exprTree->right.flags&EXPR_NEGATE)
      {  /* Allocate a EVAL_NOT or an EVAL_NEGATE expression */
         if((result=AllocateExprCalc(&noopCalc, (char)(evalIdx+1))))
            return(result);

         /* Set the function, NOT takes precedence of NEGATE if both */
         if(exprTree->right.flags&EXPR_NOT) noopCalc->func = EVAL_NOT;
         else if(exprTree->right.flags&EXPR_NEGATE) noopCalc->func = EVAL_NEGATE;

         /* Put this arg in as the last expression in the list */
         for(tmpExpr=*exprList; tmpExpr->next; tmpExpr=tmpExpr->next);
         tmpExpr->next = noopCalc;
      }

      /* Set the right child */
      for(tmpExpr=*exprList; tmpExpr->next; tmpExpr=tmpExpr->next);
      rightChild = tmpExpr;
   }
   else 
   {  /* Allocate a right side argument */
      newCalc->flags |= RIGHT_ARG;
      if((newCalc->right = (void*)MyMalloc(sizeof(tArg)))==NULL)
         throw(MALLOC_EXCEPTION);
      memset((char*)newCalc->right, 0, sizeof(tArg));

      if((status=CreateOperandArg((tArg*)newCalc->right, 
                                   &exprTree->right, calcTab, FALSE, numeric)))
         return(status);
   }

  
   /*****************************************************/
   /* Set this node as the last in the expression list, */
   /* or the first if the list is empty                 */
   /*****************************************************/
   if(*exprList)
   {  for(tmpExpr=*exprList; tmpExpr->next; tmpExpr=tmpExpr->next);
      tmpExpr->next = newCalc;
   }
   else
      *exprList = newCalc;


   /********************************************************************/
   /* Set the child-parent link and set the IS_ORED and IS_ANDED flags */
   /*******************************************************************/
   if(leftChild)
   {  leftChild->parent = newCalc;
      newCalc->left = (void*)((int)leftChild->evalIdx);

      /* Set up AND or OR flags for optimization */
      if(newCalc->func == EVAL_ANDBOOL)
         leftChild->flags |= IS_ANDED;
      else if(newCalc->func == EVAL_ORBOOL)
         leftChild->flags |= IS_ORED;
   }

   if(rightChild)
   {  rightChild->parent = newCalc;
      newCalc->right = (void*)((int)rightChild->evalIdx);
   }
   
   return(OK);
}


/*--------------------------------------------------------------------------*/
/* GetExprEvalFunc()                                                        */
/*                                                                          */
/*--------------------------------------------------------------------------*/
#define MIXED_NUMERIC   20 
#define MIXED_FLOAT     21 
#define MIXED_LONG      22 
#define NUM_STRING      23 
static long GetExprEvalFunc
(tArg *exprArg, tExprTree *exprTree, tCalcTable *calcTab)
{
   int type;

   /* Traverse the exprTree tree to see if all leaves are the same type */
   type = CheckTreeType(exprTree);

   /* Reset leaf constants, if necessary, and set the function */
   if(type == HNC_FLOAT)
   {  ResetLeafConstants(exprTree, HNC_FLOAT);
      exprArg->func = GetArgFunc(calcTab, EvalQuickFloatExpr);
   }
   else if(type == MIXED_FLOAT)
      exprArg->func = GetArgFunc(calcTab, EvalMixedFloatExpr);
   else if(type == HNC_INT32)
   {  ResetLeafConstants(exprTree, HNC_INT32);
      exprArg->func = GetArgFunc(calcTab, EvalQuickLongExpr);
   }
   else if(type == MIXED_LONG)
      exprArg->func = GetArgFunc(calcTab, EvalMixedLongExpr);
   else if(type == NUMERIC_TYPE)
      exprArg->func = GetArgFunc(calcTab, EvalQuickNumericExpr);
   else if(type == MIXED_NUMERIC)
      exprArg->func = GetArgFunc(calcTab, EvalMixedNumericExpr);
   else if(type == NUM_STRING)
      exprArg->func = GetArgFunc(calcTab, EvalStringNumExpr);
   else if(type == HNC_STRING)
      exprArg->func = GetArgFunc(calcTab, EvalStringExpr);
   else
      exprArg->func = GetArgFunc(calcTab, EvalMixedExpr);
   
   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CheckTreeType()                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
#define IsFloat(type) (type == HNC_FLOAT || type == MIXED_FLOAT) 
#define IsLong(type) (type == HNC_INT32 || type == MIXED_LONG) 
#define IsEngNum(type) (type == NUMERIC_TYPE || type == MIXED_NUMERIC)

#define IsLFN(type) \
(IsFloat(type) || IsLong(type) || IsEngNum(type))

#define IsNum(type) \
(IsLFN(type) || IsNumeric(type))

int IsNotQuick(tExprBranch branch)
{
   /* No argument calculations */
   if(branch.flags >= EXPR_FUNCTION)
      return(1);

   /* No NOT or NEGATE arguments */
   if(branch.flags&EXPR_NOT || branch.flags&EXPR_NEGATE)
      return(1);

   /* No autoCalc rule calc variables or parameters */
   if(branch.flags&EXPR_VARBLE)
   {  if(branch.val.var->flags&VAR_PARAMETER)
         return(1);
      if(branch.val.var->calcRule)
         if(!branch.val.var->calcRule->noAutoCalc)
            return(1);
   }

   /* No array calculations */
   if(branch.flags&EXPR_ARRAYVAR)
      return(1);

   /* No non-fixed data fields */
   if(branch.flags&EXPR_FIELD)
   {  if(branch.val.dataFld->fldData->data)
      {  if(!((tFlatData*)branch.val.dataFld->fldData->data)->data)
            return(1);

         if(branch.val.dataFld->fldData->fld->type == ARRAY_TYPE)
            return(1);
      }
      else
         return(1);
   } 

   return(0);
}

int CheckTreeType(tExprTree *exprTree)
{
   int leftType=exprTree->left.type;
   int rightType=exprTree->right.type;


   /* If left side is a tree, check it */
   if(exprTree->left.flags&EXPR_TREE)
   {  if((leftType = CheckTreeType(exprTree->left.val.tree))==0)
         return(leftType);

      /* Not or negate goes into mixed category */
      if(exprTree->left.flags&EXPR_NOT || exprTree->left.flags&EXPR_NEGATE)
      {  if(leftType == HNC_FLOAT)
            leftType = MIXED_FLOAT;
         else if(leftType == HNC_INT32)
            leftType = MIXED_LONG;
         else if(leftType == NUMERIC_TYPE)
            leftType = MIXED_NUMERIC;
         else if(IsNum(leftType))
            leftType = MIXED_NUMERIC;
         else 
            return(0);
      }

      /* Boolean string sub-expressions are full mix */ 
      if(leftType == NUM_STRING)
         return(0);
   }
   else
   {  if(leftType == RECORD_TYPE || leftType == ARRAY_TYPE)
         return(0);
      else if(IsNotQuick(exprTree->left))
      {  if(leftType == HNC_FLOAT)
            leftType = MIXED_FLOAT;
         else if(leftType == HNC_INT32)
            leftType = MIXED_LONG;
         else if(leftType == NUMERIC_TYPE)
            leftType = MIXED_NUMERIC;
         else if(IsNum(leftType))
            leftType = MIXED_NUMERIC;
         else if(!IsString(leftType))
            return(0);
      }
      else if(IsString(leftType)) 
         leftType = HNC_STRING;
   }
      
   /* If right side is a tree, check it */
   if(exprTree->right.flags&EXPR_TREE)
   {  if((rightType = CheckTreeType(exprTree->right.val.tree))==0)
         return(rightType);

      /* Not or negate goes into mixed category */
      if(exprTree->right.flags&EXPR_NOT || exprTree->right.flags&EXPR_NEGATE)
      {  if(rightType == HNC_FLOAT)
            rightType = MIXED_FLOAT;
         else if(rightType == HNC_INT32)
            rightType = MIXED_LONG;
         else if(leftType == NUMERIC_TYPE)
            rightType = MIXED_NUMERIC;
         else if(IsNum(leftType))
            rightType = MIXED_NUMERIC;
         else 
            return(0);
      }

      /* Boolean string sub-expressions are full mix */ 
      if(rightType == NUM_STRING)
         return(0);
   }
   else
   {  if(rightType == RECORD_TYPE || rightType == ARRAY_TYPE)
         return(0);
      else if(IsNotQuick(exprTree->right))
      {  if(rightType == HNC_FLOAT)
            rightType = MIXED_FLOAT;
         else if(rightType == HNC_INT32)
            rightType = MIXED_LONG;
         else if(rightType == NUMERIC_TYPE)
            rightType = MIXED_NUMERIC;
         else if(IsNum(rightType))
            rightType = MIXED_NUMERIC;
         else if(!IsString(rightType))
            return(0);
      }
      else if(IsString(rightType)) 
         rightType = HNC_STRING;
   }
      

   /* See how left and right types are compatible, if at all */
   if(leftType == rightType)
   {  /* Check for NUM_STRING */ 
      if(leftType == HNC_STRING && !(exprTree->op == SET || exprTree->op == PLUS))
         return(NUM_STRING);

      return(leftType);
   }
   else if(IsString(leftType) && IsString(rightType))
      return(leftType);
   else if(IsFloat(leftType) && IsFloat(rightType))
      return(MIXED_FLOAT);
   else if(IsLong(leftType) && IsLong(rightType))
      return(MIXED_LONG);
   else if(IsEngNum(leftType) && IsEngNum(rightType))
      return(MIXED_NUMERIC);
   else if(IsLFN(leftType) && 
         (exprTree->right.flags&EXPR_CONSTANT && exprTree->right.type==NUMERIC_TYPE))
   {  if((leftType == HNC_INT32 || leftType == MIXED_LONG)
                       && !IS_INTEGER(exprTree->right.val.num))
         return(MIXED_NUMERIC);
      else
         return(leftType);
   }
   else if(IsLFN(rightType) &&
         (exprTree->left.flags&EXPR_CONSTANT && exprTree->left.type==NUMERIC_TYPE))
   {  if((rightType == HNC_INT32 || rightType == MIXED_LONG)
                       && !IS_INTEGER(exprTree->left.val.num))
         return(MIXED_NUMERIC);
      else
         return(rightType);
   }
   else if(IsNum(leftType) && IsNum(rightType))
      return(MIXED_NUMERIC);

   return(0);
}


/*--------------------------------------------------------------------------*/
/* ResetLeafConstants()                                                     */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long ResetLeafConstants(tExprTree *exprTree, int type)
{
   /* If left side is a tree, follow it */
   if(exprTree->left.flags&EXPR_TREE)
      ResetLeafConstants(exprTree->left.val.tree, type);
   else
   {  if(exprTree->left.flags&EXPR_CONSTANT && exprTree->left.type == NUMERIC_TYPE)
      {  if(type == HNC_INT32)
            exprTree->left.val.lVal = NUM_GETLNG(exprTree->left.val.num);
         else if(type == HNC_FLOAT)
            exprTree->left.val.fVal = NUM_GETFLT(exprTree->left.val.num);
      }
   }


   /* If right side is a tree, follow it */
   if(exprTree->right.flags&EXPR_TREE)
      ResetLeafConstants(exprTree->right.val.tree, type);
   else
   {  if(exprTree->right.flags&EXPR_CONSTANT && exprTree->right.type == NUMERIC_TYPE)
      {  if(type == HNC_INT32)
            exprTree->right.val.lVal = NUM_GETLNG(exprTree->right.val.num);
         else if(type == HNC_FLOAT)
            exprTree->right.val.fVal = NUM_GETFLT(exprTree->right.val.num);
      }
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CreateExprArg()                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long CreateExprArg
(tArg *exprArg, tExprTree *exprTree, tCalcTable *calcTab)
{
   long       status;
   tExprCalc  *exprCalc=NULL;

   /*********************************************************************/  
   /* First, check to see if the operator is NOOP and the expression is */
   /* a single argument.  If so, then set up the argument only.         */
   /*********************************************************************/
   exprArg->atom.type = exprTree->type;
   if(exprTree->op == NOOP && !(exprTree->left.flags&EXPR_TREE))
   {  if((status=CreateOperandArg(exprArg, &exprTree->left, 
                               calcTab, exprTree->reference, !IsString(exprTree->type))))
         return(status);
      return(OK);
   }


   /*********************************************/
   /* Create the expression calc and set up the */
   /* proper function for the evaluation        */
   /*********************************************/
   /* Get the evaluation function and reset the exprTree accordingly */
   if((status=GetExprEvalFunc(exprArg, exprTree, calcTab)))
      return(status);
   
   /* Create the list of expression atoms */
   if((status=CreateExprList(&exprCalc, NULL, exprTree, calcTab, 0)))
      return(status);

   exprArg->atom.data.uVal = (void*)exprCalc;
   exprArg->flags |= ATOM_EXPR_CALC;

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* UpdatingValue()                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long UpdatingValue(tExprTree *exprTree)
{
   /* Must be a SET operation */
   if(exprTree->op!=SET)
      return(0);

   /* Must be long, float, or numeric data */
   if(exprTree->left.type != HNC_INT32 && 
      exprTree->left.type != HNC_FLOAT && 
      exprTree->left.type != NUMERIC_TYPE)
      return(0);

   /* Must not be an auto calc rule variable */
   if(exprTree->left.flags&EXPR_VARBLE)
   {  if(exprTree->left.val.var->calcRule) 
         if(!exprTree->left.val.var->calcRule->noAutoCalc)
            return(0);
   }

   /* Right side must be a tree with a PLUS, MINUS, DIVIDE, or MULTIPLY */
   if(!(exprTree->right.flags&EXPR_TREE))
      return(0);
   
   if(exprTree->right.val.tree->op != PLUS &&
      exprTree->right.val.tree->op != MINUS &&
      exprTree->right.val.tree->op != DIVIDE &&
      exprTree->right.val.tree->op != MULTIPLY)
      return(0);

   /* Check for right side being a NOT or NEGATE expression */ 
   if(exprTree->right.flags&EXPR_NOT || exprTree->right.flags&EXPR_NEGATE)
      return(0);

   /* Check for right side being a NOT or NEGATE left side of the right expression */ 
   if(exprTree->right.val.tree->left.flags&EXPR_NOT || 
      exprTree->right.val.tree->left.flags&EXPR_NEGATE)
      return(0);

   /* Cannot have NOT or NEGATE remain expression, e.g. X := X + -(Y + 2) */
   if(exprTree->right.val.tree->right.flags&EXPR_TREE && 
        (exprTree->right.val.tree->right.flags&EXPR_NOT ||
         exprTree->right.val.tree->right.flags&EXPR_NEGATE))
      return(0);

   /* First check for updating a data field */
   if(exprTree->left.flags&EXPR_FIELD && exprTree->right.val.tree->left.flags&EXPR_FIELD)
   {  if(NoCaseStrcmp(exprTree->left.val.dataFld->recordName, 
                             exprTree->right.val.tree->left.val.dataFld->recordName))
         return(0);

      if(NoCaseStrcmp(exprTree->left.val.dataFld->fieldName, 
                             exprTree->right.val.tree->left.val.dataFld->fieldName))
         return(0);

      return(1);
   }
   else if(exprTree->left.flags&EXPR_VARBLE && 
                         exprTree->right.val.tree->left.flags&EXPR_VARBLE)
   {  if(exprTree->left.val.var != exprTree->right.val.tree->left.val.var)
         return(0);

      return(1);
   }

   return(0);
}


/*--------------------------------------------------------------------------*/
/* SetIPFunc()                                                             */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long SetIPFunc(tArg *stmtArg, tExprTree *exprTree, tCalcTable *calcTab)
{
   /********************************/
   /* Set the appropriate function */
   /********************************/
   switch(exprTree->op)
   {  case PLUS:
         if(exprTree->left.type == NUMERIC_TYPE)
            stmtArg->func = GetArgFunc(calcTab, EvalNumIPADD);
         else if(exprTree->left.type == HNC_FLOAT)
            stmtArg->func = GetArgFunc(calcTab, EvalFloatIPADD);
         else if(exprTree->left.type == HNC_INT32)
            stmtArg->func = GetArgFunc(calcTab, EvalLongIPADD);
         break;

      case MINUS:
         if(exprTree->left.type == NUMERIC_TYPE)
            stmtArg->func = GetArgFunc(calcTab, EvalNumIPSUB);
         else if(exprTree->left.type == HNC_FLOAT)
            stmtArg->func = GetArgFunc(calcTab, EvalFloatIPSUB);
         else if(exprTree->left.type == HNC_INT32)
            stmtArg->func = GetArgFunc(calcTab, EvalLongIPSUB);
         break;

      case MULTIPLY:
         if(exprTree->left.type == NUMERIC_TYPE)
            stmtArg->func = GetArgFunc(calcTab, EvalNumIPMUL);
         else if(exprTree->left.type == HNC_FLOAT)
            stmtArg->func = GetArgFunc(calcTab, EvalFloatIPMUL);
         else if(exprTree->left.type == HNC_INT32)
            stmtArg->func = GetArgFunc(calcTab, EvalLongIPMUL);
         break;

      case DIVIDE:
         if(exprTree->left.type == NUMERIC_TYPE)
            stmtArg->func = GetArgFunc(calcTab, EvalNumIPDIV);
         else if(exprTree->left.type == HNC_FLOAT)
            stmtArg->func = GetArgFunc(calcTab, EvalFloatIPDIV);
         else if(exprTree->left.type == HNC_INT32)
            stmtArg->func = GetArgFunc(calcTab, EvalLongIPDIV);
         break;

      default:
         break;
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* SetIncrFunc()                                                            */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long SetIncrFunc(tArg *stmtArg, tExprTree *exprTree, tCalcTable *calcTab)
{
   /********************************/
   /* Set the appropriate function */
   /********************************/
   if(exprTree->left.type == NUMERIC_TYPE)
      stmtArg->func = GetArgFunc(calcTab, EvalNumIncrement);
   else if(exprTree->left.type == HNC_FLOAT)
      stmtArg->func = GetArgFunc(calcTab, EvalFloatIncrement);
   else if(exprTree->left.type == HNC_INT32)
      stmtArg->func = GetArgFunc(calcTab, EvalLongIncrement);

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CreateInPlaceCalc()                                                      */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long CreateInPlaceCalc(tArg *stmtArg, tExprTree *exprTree, tCalcTable *calcTab)
{  
   long     status;
   tArgCalc *calcArg = (tArgCalc*)stmtArg->atom.data.uVal;


   /*********************************************/
   /* Indicate that left side of tree is unused */
   /*********************************************/
   exprTree->left.flags |= EXPR_UNUSED;

   /*****************************************************/
   /* Check if the right operand is a 1.  If so, set up */
   /* the EVAL_INCREMENT function.  If not, set up the  */
   /* appropriate in place function.                    */
   /*****************************************************/
   if((IS_VAL(exprTree->right.val.num, 1.0)) &&
      exprTree->op == PLUS)
   {  SetIncrFunc(stmtArg, exprTree, calcTab);
      return(OK);
   }
   else
   {  SetIPFunc(stmtArg, exprTree, calcTab);
     
      /**********************************************************************/ 
      /* Set up the next argument, which is the remainder of the expression */ 
      /**********************************************************************/ 
      if((calcArg->argList->next = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
         throw(MALLOC_EXCEPTION);
      memset((char*)calcArg->argList->next, 0, sizeof(tArg));

      /******************************************************************/
      /* Check to see if remainder is a single operand or an expression */
      /******************************************************************/
      if(exprTree->right.flags&EXPR_TREE)
      {  /* Temporary make the parent of the right tree NULL */
         /* so that no unecessary arguments will be created  */
         exprTree->right.val.tree->parent = NULL;

         /* Create an expression calc for the remainder */
         if((status=CreateExprArg(calcArg->argList->next,
                         exprTree->right.val.tree, calcTab)))
            return(status);

         /* Put the parent pointer back */
         exprTree->right.val.tree->parent = exprTree->right.val.tree;
      }
      else
      {  if((status=CreateOperandArg(calcArg->argList->next,
                        &exprTree->right, calcTab, FALSE, TRUE)))
            return(status);
      }

      return(OK);
   }
}


/*--------------------------------------------------------------------------*/
/* CreateExprStmt()                                                    */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long CreateExprStmt
(tArg *stmtArg, tExprTree *exprTree, tCalcTable *calcTab)
{
   long      status;

   /******************************************************/
   /* Check to see if the expression is of the format    */ 
   /* x := x op y, or even the special case x := x + 1.  */
   /* These will be very common operations and will be   */
   /* optimized with their own built-in functions.       */
   /******************************************************/
   if(UpdatingValue(exprTree))
   {  tArgCalc  *calcArg;

      /*********************************/
      /* Allocate a tArgCalc structure */
      /*********************************/
      if((calcArg=(tArgCalc*)MyMalloc(sizeof(tArgCalc)))==NULL)
         throw(MALLOC_EXCEPTION);
      memset((char*)calcArg, 0, sizeof(tArgCalc));
      stmtArg->atom.data.uVal = (void*)calcArg;
      stmtArg->flags |= ATOM_ARG_CALC;

      /*********************************************/ 
      /* Set up the variable as the first argument */ 
      /*********************************************/ 
      if((calcArg->argList = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
         throw(MALLOC_EXCEPTION);
      memset((char*)calcArg->argList, 0, sizeof(tArg));

      if((status=CreateOperandArg(calcArg->argList, 
                 &exprTree->left, calcTab, TRUE, TRUE)))
         return(status);

      /*************************************************************/
      /* Call CreateInPlaceCalc to set up proper in-place function */
      /*************************************************************/
      if((status=CreateInPlaceCalc(stmtArg, exprTree->right.val.tree, calcTab)))
         return(status);

      return(OK);
   }


   /**********************************/
   /* The statement is an expression */
   /**********************************/
   if((status=CreateExprArg(stmtArg, exprTree, calcTab)))
      return(status);

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CreateCaseStmt()                                                         */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long CreateCaseStmt
(tArg *stmtArg, tCaseData *caseData, tCalcTable *calcTab)
{
   long      status;
   tCaseArg  *caseArg;
   tArg      *newArg;
   tCase     *caseNode;
   tArgCalc  *calcArg;

   /*********************************/
   /* Allocate a tArgCalc structure */
   /*********************************/
   if((calcArg=(tArgCalc*)MyMalloc(sizeof(tArgCalc)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)calcArg, 0, sizeof(tArgCalc));
   stmtArg->atom.data.uVal = (void*)calcArg;
   stmtArg->flags |= ATOM_ARG_CALC;

   
   /*********************************************/
   /* The first argument is the case expression */
   /*********************************************/
   /* Allocate the argument */
   if((newArg = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)newArg, 0, sizeof(tArg));
   calcArg->argList = newArg;
   
   /* Set up the case expression */
   if((status=CreateExprArg(newArg, caseData->caseExpr, calcTab)))
      return(status);
   

   /*********************************************************/
   /* The rest of the arguments are case nodes values, with */
   /* their corresponding statement lists in between        */
   /*********************************************************/
   for(caseNode=caseData->caseList; caseNode; caseNode=caseNode->next)
   {  for(caseArg=caseNode->valList; caseArg; caseArg=caseArg->next)
      {  /* Allocate the argument */
         if((newArg->next = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
            throw(MALLOC_EXCEPTION);
         newArg = newArg->next;
         memset((char*)newArg, 0, sizeof(tArg));
         newArg->atom = caseArg->atom;
      }

      /* Allocate the argument for the statement list */
      if((newArg->next = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
         throw(MALLOC_EXCEPTION);
      newArg = newArg->next;
      memset((char*)newArg, 0, sizeof(tArg));
      
      /* Set up the statements */
      if((status=CreateStatementArg(newArg, caseNode->stmtList, calcTab)))
         return(status);

      /* Set the flag to indicate that this is a statement block */
      newArg->flags |= ATOM_CASE_STMT;
   }
   

   /***********************************/
   /* Set up the appropriate function */
   /***********************************/
   if(!IsString(caseData->caseExpr->type))
      stmtArg->func = GetArgFunc(calcTab, EvalNumericCase);
   else
      stmtArg->func = GetArgFunc(calcTab, EvalStringCase);

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CreateForStmt()                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long CreateForStmt
(tArg *stmtArg, tForData *forData, tCalcTable *calcTab)
{
   long      status;
   tArg      *newArg;
   tArgCalc   *calcArg;

   /*********************************/
   /* Allocate a tArgCalc structure */
   /*********************************/
   if((calcArg=(tArgCalc*)MyMalloc(sizeof(tArgCalc)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)calcArg, 0, sizeof(tArgCalc));
   stmtArg->atom.data.uVal = (void*)calcArg;
   stmtArg->flags |= ATOM_ARG_CALC;

   
   /**************************************/
   /* Set up to use the EvalFor function */
   /**************************************/
   stmtArg->func = GetArgFunc(calcTab, EvalFor);
   
   /*********************************************/
   /* The first argument is the init expression */
   /*********************************************/
   /* Allocate the argument */
   if((newArg = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)newArg, 0, sizeof(tArg));
   calcArg->argList = newArg;
   
   /* Set up the init expression */
   if((status=CreateExprArg(newArg, forData->initExpr, calcTab)))
      return(status);

  
   /***********************************************/
   /* The second argument is the limit expression */
   /***********************************************/
   /* Allocate the argument */
   if((newArg->next = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
      throw(MALLOC_EXCEPTION);
   newArg = newArg->next;
   memset((char*)newArg, 0, sizeof(tArg));
   
   /* Set up the limit expression */
   if((status=CreateExprArg(newArg, forData->limitExpr, calcTab)))
      return(status);
   

   /*********************************************/
   /* The thrid argument is the step expression */
   /*********************************************/
   /* Allocate the argument */
   if((newArg->next = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
      throw(MALLOC_EXCEPTION);
   newArg = newArg->next;
   memset((char*)newArg, 0, sizeof(tArg));

   /* If direction is COUNT_DOWN, then negate the step expression */
   if(forData->direction == COUNT_DOWN)
      newArg->flags |= ATOM_NEGATE;
   
   /* Set up the set expression calc */
   if((status=CreateExprArg(newArg, forData->stepExpr, calcTab)))
      return(status);
   
   
   /****************************************************/
   /* The fourth argument is the loop control variable */
   /****************************************************/
   /* Allocate the argument */
   if((newArg->next = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
      throw(MALLOC_EXCEPTION);
   newArg = newArg->next;
   memset((char*)newArg, 0, sizeof(tArg));
   newArg->flags |= ATOM_ADDRESS;
   newArg->atom.data.uVal = (void*)forData->loopVar->data;


   /********************************************/
   /* The fifth argument is the statement list */
   /********************************************/
   /* Allocate the argument */
   if((newArg->next = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
      throw(MALLOC_EXCEPTION);
   newArg = newArg->next;
   memset((char*)newArg, 0, sizeof(tArg));
   
   /* Set up the statement list */
   if((status=CreateStatementArg(newArg, forData->stmtList, calcTab)))
      return(status);
 
   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CreateWhileStmt()                                                        */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long CreateWhileStmt
(tArg *stmtArg, tWhileData *whileData, tCalcTable *calcTab)
{
   long      status;
   tArg      *newArg;
   tArgCalc  *calcArg;

   /*********************************/
   /* Allocate a tArgCalc structure */
   /*********************************/
   if((calcArg=(tArgCalc*)MyMalloc(sizeof(tArgCalc)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)calcArg, 0, sizeof(tArgCalc));
   stmtArg->atom.data.uVal = (void*)calcArg;
   stmtArg->flags |= ATOM_ARG_CALC;

   /****************************************/
   /* Set up to use the EvalWhile function */
   /****************************************/
   stmtArg->func = GetArgFunc(calcTab, EvalWhile);
   
   /****************************************************/
   /* The first argument is the conditional expression */
   /****************************************************/
   /* Allocate the argument */
   if((newArg = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)newArg, 0, sizeof(tArg));
   calcArg->argList = newArg;
   
   /* Set up the conditional expression */
   if((status=CreateExprArg(newArg, whileData->condExpr, calcTab)))
      return(status);
   

   /*********************************************/
   /* The second argument is the statement list */
   /*********************************************/
   /* Allocate the argument */
   if((newArg->next = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
      throw(MALLOC_EXCEPTION);
   newArg = newArg->next;
   memset((char*)newArg, 0, sizeof(tArg));
   
   /* Set it up as a statement list */
   if((status=CreateStatementArg(newArg, whileData->stmtList, calcTab)))
      return(status);
   

   /****************************************/
   /* The third argument is the loop limit */
   /****************************************/
   /* Allocate the argument */
   if((newArg->next = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
      throw(MALLOC_EXCEPTION);
   newArg = newArg->next;
   memset((char*)newArg, 0, sizeof(tArg));
   NUM_SETINT32(newArg->atom.data.num, whileData->limit)
 
   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CreateIfStmt()                                                           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long CreateIfStmt
(tArg *stmtArg, tIfData *ifData, tCalcTable *calcTab)
{
   long       status;
   tArg       *newArg;
   tArgCalc   *calcArg;

   /*********************************/
   /* Allocate a tArgCalc structure */
   /*********************************/
   if((calcArg=(tArgCalc*)MyMalloc(sizeof(tArgCalc)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)calcArg, 0, sizeof(tArgCalc));
   stmtArg->atom.data.uVal = (void*)calcArg;
   stmtArg->flags |= ATOM_ARG_CALC;
   
   /*************************************/
   /* Set up to use the EvalIf function */
   /*************************************/
   if(calcTab->calcVar)
      stmtArg->func = GetArgFunc(calcTab, EvalVarIf);
   else
      stmtArg->func = GetArgFunc(calcTab, EvalIf);

   /****************************************************/
   /* The first argument is the conditional expression */
   /****************************************************/
   /* Allocate the argument */
   if((newArg = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)newArg, 0, sizeof(tArg));
   calcArg->argList = newArg;
   
   /* Set up the conditional expression */
   if((status=CreateExprArg(newArg, ifData->condExpr, calcTab)))
      return(status);
   

   /*****************************************************/
   /* The second argument is the primary statement list */
   /*****************************************************/
   /* Allocate the argument */
   if((newArg->next = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
      throw(MALLOC_EXCEPTION);
   newArg = newArg->next;
   memset((char*)newArg, 0, sizeof(tArg));
   
   /* Set up the statement argument */
   if((status=CreateStatementArg(newArg, ifData->primeSList, calcTab)))
      return(status);


   /**************************************************************/
   /* The third argument is the secondary statement list, if any */
   /**************************************************************/
   if(ifData->secondaryType != NO_SECOND)
   {  /* Allocate the argument */
      if((newArg->next = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
         throw(MALLOC_EXCEPTION);
      newArg = newArg->next;
      memset((char*)newArg, 0, sizeof(tArg));
  
      /*************************************************************/
      /* Determine the secondary type.  If it is an ELSE statement */
      /* then setup the statement list.  If it is an ELSE-IF, then */
      /* set up a new if statement.                                */
      /*************************************************************/
      if(ifData->secondaryType == ELSE_STATEMENT)
      {  /* Set it a statement block argument */
         if((status=CreateStatementArg(newArg, ifData->secondary.sList, calcTab)))
            return(status);
      }
      else
      {  /* Set up a new IF argument calc */
         if((status=CreateIfStmt(newArg, ifData->secondary.ifData, calcTab)))
            return(status);
      }
   }  

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* UpdatingVariable()                                                       */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long UpdatingVariable(tEngVar *var, tExprTree *exprTree)
{
   /* See if left side of expression is a variable */
   if(!(exprTree->left.flags&EXPR_VARBLE))
      return(0);

   /* Expression must be numeric */
   if(!IsBinNumeric(exprTree->type))
      return(0);
   
   /* Left side of expression cannot be NOT'd or NEGATE'd */ 
   if(exprTree->left.flags&EXPR_NOT || exprTree->left.flags&EXPR_NEGATE)
      return(0);

   /* Cannot have NOT or NEGATE remain expression, e.g. X := X + -(Y + 2) */
   if(exprTree->right.flags&EXPR_TREE &&
        (exprTree->right.flags&EXPR_NOT ||
         exprTree->right.flags&EXPR_NEGATE))
      return(0);

   /* Must have PLUS, MINUS, DIVIDE, or MULTIPLY */
   if(exprTree->op != PLUS && exprTree->op != MINUS &&
      exprTree->op != DIVIDE && exprTree->op != MULTIPLY)
      return(0);

   /* See if variable on left matches this variable */
   if(exprTree->left.val.var->data == var->data)
      return(1);

   return(0);
}


/*--------------------------------------------------------------------------*/
/* GetVarCalcStmt()                                                         */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long GetVarCalcStmt(tArg *stmtArg, tExprTree *returnExpr, tCalcTable *calcTab)
{
   tVarCalc  *calcVar = calcTab->calcVar;
   tArgCalc  *calcArg;
   tExprTree *varExpr;
   long      status;

   calcVar = calcTab->calcVar;

   /***************************************************************/
   /* First check for something like RETURN(X + 1) for variable X */
   /***************************************************************/
   if(UpdatingVariable(calcVar->var, returnExpr))
   {  /* Allocate a tArgCalc structure to hold the arguments */ 
      if((calcArg = (tArgCalc*)MyMalloc(sizeof(tArgCalc)))==NULL)
         throw(MALLOC_EXCEPTION);
      memset((char*)calcArg, 0, sizeof(tArgCalc));
      stmtArg->atom.data.uVal = (void*)calcArg;
      stmtArg->flags |= ATOM_ARG_CALC;
      stmtArg->atom.type = calcVar->var->type;

      /* Allocate the first argument */
      if((calcArg->argList = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
         throw(MALLOC_EXCEPTION);
      memset((char*)calcArg->argList, 0, sizeof(tArg));
      stmtArg->atom.data.uVal = (void*)calcArg;

      /* Set the data, flags, and EvalFunc for the argument */
      SetVarArg(calcVar->var, calcArg->argList , 1, calcTab);

      /* Call CreateInPlaceCalc to set up proper in-place function */
      if((status=CreateInPlaceCalc(stmtArg, returnExpr, calcTab)))
         return(status);

      return(OK);
   }


   /*********************************************************/
   /* If the return expression is a single argument, set up */
   /* a special function to set the variable data           */
   /*********************************************************/
   if(returnExpr->op == NOOP && !(returnExpr->left.flags&EXPR_TREE))
   {  stmtArg->atom.type = returnExpr->type;

      /* Allocate a tArgCalc structure to hold the arguments */ 
      if((calcArg = (tArgCalc*)MyMalloc(sizeof(tArgCalc)))==NULL)
         throw(MALLOC_EXCEPTION);
      memset((char*)calcArg, 0, sizeof(tArgCalc));
      stmtArg->atom.data.uVal = (void*)calcArg;
      stmtArg->flags |= ATOM_ARG_CALC;
      stmtArg->atom.type = calcVar->var->type;

      /* Allocate the first argument */
      if((calcArg->argList = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
         throw(MALLOC_EXCEPTION);
      memset((char*)calcArg->argList, 0, sizeof(tArg));
      stmtArg->atom.data.uVal = (void*)calcArg;
         
      /* Create the calc for the single operand in the first argument */
      if((status=CreateOperandArg(calcArg->argList, &returnExpr->left,
                               calcTab, returnExpr->reference, !IsString(returnExpr->type))))
         return(status);

      /* Set up the appropriate function to access the single operand */
      if(calcVar->var->type == HNC_FLOAT)
         stmtArg->func = GetArgFunc(calcTab, EvalSgl_FltVarCalc);
      else if(calcVar->var->type == HNC_INT32)
         stmtArg->func = GetArgFunc(calcTab, EvalSgl_LngVarCalc);
      else if(calcVar->var->type == NUMERIC_TYPE)
         stmtArg->func = GetArgFunc(calcTab, EvalSgl_NumVarCalc);
      else if(IsString(calcVar->var->type))
         stmtArg->func = GetArgFunc(calcTab, EvalSgl_StrVarCalc);
      else 
         stmtArg->func = GetArgFunc(calcTab, EvalSgl_OthVarCalc);

      /* Put the tVarCalc structure in the second argument */
      if((calcArg->argList->next = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
         throw(MALLOC_EXCEPTION);
      memset((char*)calcArg->argList->next, 0, sizeof(tArg));
      calcArg->argList->next->atom.data.uVal = (void*)calcVar;
      calcArg->argList->next->flags |= ATOM_ADDRESS;

      return(OK);
   }


   /********************************************************/
   /* Create a temporaray set expression with the variable */
   /* as the left-most operand                             */
   /********************************************************/
   if((status=AllocateExprTree(&varExpr)))
      return(status);

   /* Set right branch */
   varExpr->right.flags |= EXPR_TREE;
   varExpr->right.val.tree = returnExpr;
   varExpr->right.type = returnExpr->type;

   /* Set left branch */
   varExpr->left.flags |= EXPR_VARBLE;
   varExpr->left.val.var = calcVar->var;
   varExpr->left.type = calcVar->var->type;

   varExpr->type = returnExpr->type;
   varExpr->op = SET;
   

   /****************************************************/
   /* Create the expression calc for the statement arg */
   /****************************************************/
   if((status=CreateExprArg(stmtArg, varExpr, calcTab)))
      return(status);

   /***************************/
   /* Free the temporary tree */
   /***************************/
   MyFree((void*)varExpr);

   return(OK);
}
   

/*--------------------------------------------------------------------------*/
/* CreateReturnStmt()                                                       */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long CreateReturnStmt
(tArg *stmtArg, tExprTree *returnExpr, tCalcTable *calcTab)
{
   long     status;
   tArg     *newArg;
   tArgCalc *calcArg;

   /*****************************************************************/
   /* If have a calcVar, then set up a specific expression function */
   /* to calculate and set the variable.                            */
   /*****************************************************************/
   if(calcTab->calcVar) 
   {  if((status=GetVarCalcStmt(stmtArg, returnExpr, calcTab)))
         return(status);

      stmtArg->flags |= ATOM_VAR_RETURN;
      return(OK);
   }

   /*********************************/
   /* Allocate a tArgCalc structure */
   /*********************************/
   if((calcArg=(tArgCalc*)MyMalloc(sizeof(tArgCalc)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)calcArg, 0, sizeof(tArgCalc));
   stmtArg->atom.data.uVal = (void*)calcArg;
   stmtArg->flags |= ATOM_ARG_CALC;

   /*******************************/
   /* Set the EvalReturn function */
   /*******************************/
   stmtArg->func = GetArgFunc(calcTab, EvalReturn);
   
   /*************************************************/
   /* The argument is the return expression, if any */
   /*************************************************/
   if(returnExpr)
   {  /* Allocate the argument */
      if((newArg = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
         throw(MALLOC_EXCEPTION);
      calcArg->argList = newArg;
      memset((char*)newArg, 0, sizeof(tArg));
   
      /* Set up the return expression */
      if((status=CreateExprArg(newArg, returnExpr, calcTab)))
         return(status);
   }
   
   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CreateTermStmt()                                                         */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long 
CreateTermStmt(tArg *stmtArg, tExprTree *termExpr, tCalcTable *calcTab)
{
   long      status;
   tArg      *newArg;
   tArgCalc  *calcArg;

   /*********************************/
   /* Allocate a tArgCalc structure */
   /*********************************/
   if((calcArg=(tArgCalc*)MyMalloc(sizeof(tArgCalc)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)calcArg, 0, sizeof(tArgCalc));
   stmtArg->atom.data.uVal = (void*)calcArg;
   stmtArg->flags |= ATOM_ARG_CALC;

   
   /*********************************************/
   /* Set up to use the EVAL_TERMINATE function */
   /*********************************************/
   stmtArg->func = GetArgFunc(calcTab, EvalTerminate);
   
   /****************************************************/
   /* The argument is the terminate expression, if any */
   /****************************************************/
   if(termExpr)
   {  /* Allocate the argument */
      if((newArg = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
         throw(MALLOC_EXCEPTION);
      calcArg->argList = newArg;
      memset((char*)newArg, 0, sizeof(tArg));
   
      /* Set up the terminate expression */
      if((status=CreateExprArg(newArg, termExpr, calcTab)))
         return(status);
   }
   
   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CreateStatementCalc()                                                    */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long CreateStatementCalc
(tArg *stmtArg, tStatement *statement, tCalcTable *calcTab) 
{
   long  status=OK;


   /**************************************************************/
   /* Find out what kind of statement it is, and act accordingly */
   /**************************************************************/
   switch(statement->type)
   {  case IF_STATEMENT:
        status = CreateIfStmt(stmtArg, statement->data.ifData, calcTab);  
        break;
      case RETURN_STATEMENT:
        status = CreateReturnStmt(stmtArg, statement->data.exprTree, calcTab);  
        break;
      case WHILE_STATEMENT:
        status = CreateWhileStmt(stmtArg, statement->data.whileData, calcTab);  
        break;
      case FOR_STATEMENT:
        status = CreateForStmt(stmtArg, statement->data.forData, calcTab);  
        break;
      case CASE_STATEMENT:
        status = CreateCaseStmt(stmtArg, statement->data.caseData, calcTab);  
        break;
      case TERM_STATEMENT:
        status = CreateTermStmt(stmtArg, statement->data.exprTree, calcTab);  
        break;
      case EXPR_STATEMENT:
        status = CreateExprStmt(stmtArg, statement->data.exprTree, calcTab);  
        break;
      default:
        return(BAD_STATEMENT_TYPE);
   }

   return(status);
}


/*--------------------------------------------------------------------------*/
/* CreateStatementArg()                                                     */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long CreateStatementArg
(tArg *arg, tStatement *statementList, tCalcTable *calcTab)
{
   tStmtCalc *stmtBlock;
   long      status;

   /* Create the list of statement calcs */
   if((status=CreateStatementBlock(&stmtBlock, statementList, calcTab)))
      return(status);

   /******************************************************/
   /* If there is only one statement, set up the arg as  */
   /* directly as that statement.  Otherwise, set up the */
   /* CalcStatements function.                           */
   /******************************************************/
   if(!stmtBlock->argList->next)
   {  *arg = *stmtBlock->argList;
      MyFree((void*)stmtBlock->argList);
      MyFree((void*)stmtBlock);
   }
   else
   {  if(calcTab->calcVar) arg->func = GetArgFunc(calcTab, CalcVarStatements);
      else arg->func = GetArgFunc(calcTab, CalcStatements);

      arg->flags |= ATOM_STMT_CALC;
      arg->atom.data.uVal = (void*)stmtBlock;
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CreateStatementBlock()                                                   */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long CreateStatementBlock
(tStmtCalc **stmtBlock, tStatement *statementList, tCalcTable *calcTab)
{
   long        status;
   tStatement  *tmpStmnt;
   tArg        *stmtArg=NULL;

   /************************************************/
   /* If there are no statements, then just return */ 
   /************************************************/
   if(statementList==NULL)
      return(OK);

   /********************************************/
   /* Start at the front of the statement list */
   /********************************************/
   tmpStmnt = statementList;

   /**********************************/
   /* Allocate a tStmtCalc structure */
   /**********************************/
   if((*stmtBlock = (tStmtCalc*)MyMalloc(sizeof(tStmtCalc)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)*stmtBlock, 0, sizeof(tStmtCalc));

   /*********************************************************************/
   /* Loop through the statements, creating arguments of calc nodes for */
   /* each of the statements                                            */
   /*********************************************************************/
   while(tmpStmnt)
   {  /******************************/
      /* Watch for empty statements */
      /******************************/
      if(tmpStmnt->type==0)
      {  tmpStmnt = tmpStmnt->next;
         continue;
      }

      /*********************************/
      /* Allocate a new statement calc */  
      /*********************************/
      if(stmtArg)
      {  if((stmtArg->next = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
            throw(MALLOC_EXCEPTION);
         stmtArg = stmtArg->next;
      }
      else
      {  if((stmtArg = (tArg*)MyMalloc(sizeof(tArg)))==NULL)
            throw(MALLOC_EXCEPTION);
         (*stmtBlock)->argList = stmtArg;
      }
      memset((char*)stmtArg, 0, sizeof(tArg));

      /************************************************************/
      /* Setup the argument as a pointer to a calc node and setup */
      /* that the statement for that calc node                    */
      /************************************************************/
      if((status=CreateStatementCalc(stmtArg, tmpStmnt, calcTab))) 
         return(status);

      tmpStmnt = tmpStmnt->next;
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* GetCalcForRule()                                                         */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long 
GetCalcForRule(tCalcTable *calcTab, tRuleData *ruleData, int ruleType, tRuleCalc **pRuleCalc)
{
   tRuleCalc *ruleCalc;
   
   /***********************************/
   /* Get the right list for the rule */
   /***********************************/
   switch(ruleType)
   {  case SYS_INIT_RULE:
         ruleCalc = calcTab->sysInitList;
         break;
      case SYS_TERM_RULE:
         ruleCalc = calcTab->sysTermList;
         break;
      case ITER_INIT_RULE:
         ruleCalc = calcTab->iterInitList;
         break;
      case ITER_TERM_RULE:
         ruleCalc = calcTab->iterTermList;
         break;
      case EXEC_RULE:
         if(ruleData->argList==NULL && ruleData->returnType==0)
            ruleCalc = calcTab->ruleCalcList;
         else
            ruleCalc = calcTab->paramRuleList;
         break;
      default:
         return(RULE_NOT_FOUND);
   }

   for(; ruleCalc; ruleCalc=ruleCalc->next)
   {  if(!strcmp(ruleCalc->id, ruleData->ruleName))
      {  *pRuleCalc = ruleCalc;
         return(OK);
      }
   }

   return(RULE_NOT_FOUND);
}


/*--------------------------------------------------------------------------*/
/* CreateRuleCalc()                                                         */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long CreateRuleCalc
(tRuleCalc **pRuleCalc, tParseResults *parseResults, tCalcTable *calcTab)
{
   tRuleCalc *ruleCalc, *tmpCalc, *prevCalc=NULL;
   tRuleData *ruleData = parseResults->ruleData;
   int       ruleType = parseResults->ruleType;

   /**************************************/
   /* Allocate a new rule calc structure */ 
   /**************************************/
   if((ruleCalc = (tRuleCalc*)MyMalloc(sizeof(tRuleCalc)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)ruleCalc, 0, sizeof(tRuleCalc));
   
   /*************************************************/
   /* Set the ID for the calc node as the rule name */
   /*************************************************/
   if((ruleCalc->id = (char*)MyMalloc(strlen(ruleData->ruleName)+1))==NULL)
      throw(MALLOC_EXCEPTION);
   strcpy(ruleCalc->id, ruleData->ruleName);

   /***********************************************/
   /* Put the new rule calc into the proper list  */
   /* Put support rules into their proper place   */
   /* Rules with parameters go into paramRuleList */
   /* and those without go into ruleCalcList.     */
   /***********************************************/
   switch(ruleType)
   {  case SYS_INIT_RULE:
         tmpCalc = calcTab->sysInitList;
         while(tmpCalc) 
         {  prevCalc = tmpCalc;
            tmpCalc = tmpCalc->next;
         }
         if(prevCalc) prevCalc->next = ruleCalc;
         else calcTab->sysInitList = ruleCalc;
         break;
      case SYS_TERM_RULE:
         tmpCalc = calcTab->sysTermList;
         while(tmpCalc) 
         {  prevCalc = tmpCalc;
            tmpCalc = tmpCalc->next;
         }
         if(prevCalc) prevCalc->next = ruleCalc;
         else calcTab->sysTermList = ruleCalc;
         break;
      case ITER_INIT_RULE:
         tmpCalc = calcTab->iterInitList;
         while(tmpCalc) 
         {  prevCalc = tmpCalc;
            tmpCalc = tmpCalc->next;
         }
         if(prevCalc) prevCalc->next = ruleCalc;
         else calcTab->iterInitList = ruleCalc;
         break;
      case ITER_TERM_RULE:
         tmpCalc = calcTab->iterTermList;
         while(tmpCalc) 
         {  prevCalc = tmpCalc;
            tmpCalc = tmpCalc->next;
         }
         if(prevCalc) prevCalc->next = ruleCalc;
         else calcTab->iterTermList = ruleCalc;
         break;
      case EXEC_RULE:
         if(ruleData->argList || ruleData->returnType != 0)
         {  tmpCalc = calcTab->paramRuleList;
            while(tmpCalc) 
            {  prevCalc = tmpCalc;
               tmpCalc = tmpCalc->next;
            }
            if(prevCalc) prevCalc->next = ruleCalc;
            else calcTab->paramRuleList = ruleCalc;
         }
         else
         {  tmpCalc = calcTab->ruleCalcList;
               while(tmpCalc) 
            {  prevCalc = tmpCalc;
               tmpCalc = tmpCalc->next;
            }
            if(prevCalc) prevCalc->next = ruleCalc;
            else calcTab->ruleCalcList = ruleCalc;
         }
         break;
   }

   *pRuleCalc = ruleCalc;
   return(OK);  
}


/*--------------------------------------------------------------------------*/
/* AllocateCalcTable()                                                      */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long AllocateCalcTable(tCalcTable **calcTab)
{
   /***********************************************/
   /* Allocate space for the calc table structure */
   /***********************************************/
   if((*calcTab = (tCalcTable*)MyMalloc(sizeof(tCalcTable)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)*calcTab, 0, sizeof(tCalcTable));

   return(OK);
}

 
/*--------------------------------------------------------------------------*/
/* CreateCalcTable()                                                        */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long CreateCalcTable
(tRuleBaseData *ruleBaseData, tParseResults *parseResults, tCalcTable *calcTab)
{
   long           status=OK;
   tParseResults  *tmpPR, *curPR;
   tEngRBData     *engRB;
   tRuleCalc      *ruleCalc;
   tCalcTable     *RBCalcTab;
   tEngVar        *tmpVar;
   tRuleParam     *param=NULL;
   tDynPool       *dynPool=NULL;
   tRecInstance   *tmpltRec;
   tStr2NumCalc   *str2numList=NULL, *s2nCalc;

   /**************************************/
   /* Make sure table has been allocated */
   /**************************************/
   if(!calcTab)
      return(FATAL_ENGINE|CREATE_TABLE|NO_CALC_TABLE);

   /***********************************************************/
   /* If no parseResults, then there are no rules to evaluate */
   /***********************************************************/
   if(!parseResults)
   {  calcTab->ruleBaseData = ruleBaseData;
      return(OK);
   }


   /*********************************************************************/
   /* Allocate the dynamic data pool structure if necessary.  This will */
   /* be done only when there are templates with dynamic arrays in them */
   /*********************************************************************/
   for(tmpltRec=parseResults->tmpltRecList; tmpltRec; tmpltRec=tmpltRec->next)
   {  if(tmpltRec->flags&NON_FIXED) 
      {  /* Allocate the pool structure */
         if((dynPool = (tDynPool*)MyMalloc(sizeof(tDynPool)))==NULL)
            throw(MALLOC_EXCEPTION);
         memset((char*)dynPool, 0, sizeof(tDynPool));

         /* Allocate a single page for the dynamic pool */
         if((dynPool->curPage = (tPoolPage*)MyMalloc(sizeof(tPoolPage)))==NULL)
            throw(MALLOC_EXCEPTION);
         dynPool->curPage->next = NULL;
         dynPool->pageList = dynPool->curPage;
         dynPool->mainCalcTab = calcTab;

         if((dynPool->curPage->data = (char*)MyMalloc(POOL_PAGE_SIZE))==NULL)
            throw(MALLOC_EXCEPTION);
         memset((char*)dynPool->curPage->data, 0, POOL_PAGE_SIZE);

         break;
      }
   }
    

   /****************************************/
   /* Load the array of argument functions */
   /****************************************/
   CreateArgFuncArray(calcTab);
   

   /******************************************************************/
   /* Loop through all the parse results and allocate all calc table */
   /******************************************************************/
   gCalcTableList = RBCalcTab = calcTab; 
   RBCalcTab->ruleBaseData = parseResults->engRB->RBData;
   RBCalcTab->tmpltRecList = parseResults->tmpltRecList;
   RBCalcTab->dynPool = dynPool;
   engRB = parseResults->engRB;
   tmpPR = parseResults;
   RBCalcTab->allData = tmpPR->allData;
   while(tmpPR)
   {  /*****************************************/
      /* Get the calc tab for this parseResult */ 
      /*****************************************/
      if(tmpPR->engRB != engRB) 
      {  /* Allocate a new calcTab */
         if(AllocateCalcTable(&RBCalcTab->next)) 
            throw(MALLOC_EXCEPTION);

         RBCalcTab = RBCalcTab->next;
         engRB = tmpPR->engRB;
         RBCalcTab->ruleBaseData = engRB->RBData;
         RBCalcTab->dynPool =  dynPool;
         RBCalcTab->numArgFuncs =  calcTab->numArgFuncs;
         RBCalcTab->argFuncArray =  calcTab->argFuncArray;
         RBCalcTab->tmpltRecList = calcTab->tmpltRecList;
      }
 
      tmpPR=tmpPR->next;
   }


   /********************************************************************/
   /* Loop through all the parse results, and get the right calc table */
   /********************************************************************/
   RBCalcTab = calcTab; 
   engRB = parseResults->engRB;
   tmpPR = parseResults;
   while(tmpPR)
   {  /****************************/
      /* Get the right calc table */ 
      /****************************/
      if(tmpPR->engRB != engRB) 
      {  engRB = tmpPR->engRB;
         
         /* If any string to numeric calcs have been created, use them */
         /* as the staring point for the next calc table, thus all     */
         /* rule bases share the same calculations                     */
         if(RBCalcTab->str2numList)
           str2numList = RBCalcTab->str2numList;
             
         RBCalcTab = RBCalcTab->next;
         RBCalcTab->str2numList = str2numList;
      }

      /**********************************************************/
      /* First loop through the parse results to make sure that */
      /* the rules will be executed in the order that they are  */
      /* presented to the engine.                               */
      /**********************************************************/
      curPR = tmpPR;
      while(tmpPR->engRB == engRB)
      {  /*********************************************************/
         /* Create the rule calc, and put it into the proper list */
         /*********************************************************/
         if((status=CreateRuleCalc(&ruleCalc, tmpPR, RBCalcTab)))
            return(FATAL_ENGINE|CREATE_TABLE|status);

         tmpPR = tmpPR->next;
         if(!tmpPR) break;
      }

      /*********************************************************/
      /* Main loop through the rule parsing results to get the */
      /* rule's statement lists                                */
      /*********************************************************/
      tmpPR = curPR;
      while(tmpPR->engRB == engRB)
      {  /***********************************************/
         /* Get the rule calc node from the proper list */
         /***********************************************/
         if((status=GetCalcForRule(RBCalcTab, tmpPR->ruleData, 
                                         tmpPR->ruleType, &ruleCalc)))
            return(FATAL_ENGINE|CREATE_TABLE|status);
         
         /********************************************/
         /* Set up the statement list for this rule */
         /********************************************/
         if((status=CreateStatementBlock(&ruleCalc->stmtBlock, 
                                  tmpPR->ruleStatementList, RBCalcTab)))
            return(FATAL_ENGINE|CREATE_TABLE|status);

         /******************************************************/
         /* If there are parameters, set up the parameter list */
         /******************************************************/
         for(tmpVar=tmpPR->paramVarList; tmpVar; tmpVar=tmpVar->next)
         {  if(param)
            {  if((param->next = (tRuleParam*)MyMalloc(sizeof(tRuleParam)))==NULL)
                  throw(MALLOC_EXCEPTION);
               param = param->next;
            }
            else
            {  if((param = (tRuleParam*)MyMalloc(sizeof(tRuleParam)))==NULL)
                  throw(MALLOC_EXCEPTION);
               ruleCalc->paramList = param;
            }
            memset((char*)param, 0, sizeof(tRuleParam));
            param->param = tmpVar; 
         }

         tmpPR = tmpPR->next;
         if(!tmpPR) break;
      }
   }

   /***************************************************************/
   /* Set all the calc done flags for the string to numeric calcs */
   /***************************************************************/
   if(!str2numList)
   {  for(RBCalcTab=calcTab; RBCalcTab; RBCalcTab=RBCalcTab->next)
      {  if(RBCalcTab->str2numList)
         {  str2numList = RBCalcTab->str2numList;
            break;
         }
      }
   }

   if(str2numList)
   {  int numCalcs=0, i;

      /* Count the number of string to numeric calcs */
      for(s2nCalc=str2numList; s2nCalc; s2nCalc=s2nCalc->next, numCalcs++);

      /* Allocate a buffer for the calcs */
      if((calcTab->calcDones = (char*)MyMalloc(numCalcs))==NULL)
         throw(MALLOC_EXCEPTION);
      memset((char*)calcTab->calcDones, 0, numCalcs);

      /* Loop through the string to numeric calcs again, setting the pointers */ 
      for(i=0, s2nCalc=str2numList; s2nCalc; s2nCalc=s2nCalc->next, i++)
         s2nCalc->calcDone = calcTab->calcDones + i;

      /* Set the number of calcs in the calc table */
      calcTab->numCalcDones = numCalcs;
   }
   
   return(OK);
}
