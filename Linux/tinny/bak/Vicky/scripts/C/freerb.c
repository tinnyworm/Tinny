/*
** $Id: freerb.c,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: freerb.c,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.2  1997/01/16  22:44:06  can
 * Lots of changes for 2.1 release
 *
 * Revision 2.1  1996/04/22  17:55:42  can
 * Several fixes and changes for 2.1
 *
 * Revision 1.8  1995/12/12  18:59:53  can
 * Made sure string parameters are NOT free'd
 *
 * Revision 1.7  1995/08/31  00:05:52  can
 * Fixed more compiler warnings.
 *
 * Revision 1.6  1995/08/16  17:59:31  can
 * Freeing the error message string.
 *
 * Revision 1.5  1995/08/11  18:28:31  can
 * Freeing error string here now.
 *
 * Revision 1.4  1995/08/02  00:08:37  can
 * Added calling of system termination rules before quit
 *
 * Revision 1.3  1995/07/21  15:35:44  can
 * Modified schemem for exception handling
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

/* HNC include files */
#include "mymall.h"
#include "exptree.h"
#include "prsrule.h"
#include "engerr.h"
#include "parstrc.h"
#include "engine.h"
#include "calctab.h"
#include "runfunc.h"
#include "freect.h"
#include "typechk.h"
#include "runct.h"

/* HNC common include files */
#include "except.h"

#ifdef DEVELOP
#include "rbdebug.h"
#endif



long FreeIfStatement(tIfData* ifData);
long FreeWhileStatement(tWhileData* whileData);
long FreeForStatement(tForData* forData);
long FreeCaseStatement(tCaseData* caseData);
long FreeStatementList(tStatement *statementList);
long FreeParseResults(tParseResults *parseResults, int freeVars);



/*--------------------------------------------------------------------------*/
/* FreeStatementList()                                                      */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long FreeStatementList(tStatement *statementList)
{
   tStatement    *tmpStmnt, *delStmnt;

   tmpStmnt = statementList;
   while(tmpStmnt)
   {  if(tmpStmnt->type==IF_STATEMENT)
         FreeIfStatement(tmpStmnt->data.ifData);
      else if(tmpStmnt->type==WHILE_STATEMENT)
         FreeWhileStatement(tmpStmnt->data.whileData);
      else if(tmpStmnt->type==FOR_STATEMENT)
         FreeForStatement(tmpStmnt->data.forData);
      else if(tmpStmnt->type==CASE_STATEMENT)
         FreeCaseStatement(tmpStmnt->data.caseData);
      else if(tmpStmnt->data.exprTree) 
         FreeExprTree(tmpStmnt->data.exprTree);

      delStmnt = tmpStmnt;
      tmpStmnt = tmpStmnt->next;
      MyFree(delStmnt);
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* FreeWhileStatement()                                                     */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long FreeWhileStatement(tWhileData *whileData)
{
   FreeExprTree(whileData->condExpr);
   FreeStatementList(whileData->stmtList);

   MyFree((void*)whileData);
   return(OK);
}


/*--------------------------------------------------------------------------*/
/* FreeForStatement()                                                       */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long FreeForStatement(tForData *forData)
{
   FreeExprTree(forData->initExpr);
   FreeExprTree(forData->limitExpr);
   FreeExprTree(forData->stepExpr);
   FreeStatementList(forData->stmtList);

   MyFree((void*)forData);
   return(OK);
}

/*--------------------------------------------------------------------------*/
/* FreeCaseStatement()                                                      */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long FreeCaseStatement(tCaseData *caseData)
{
   tCase    *tmpCase, *delCase;
   tCaseArg *arg, *delArg;

   FreeExprTree(caseData->caseExpr);

   tmpCase = caseData->caseList;
   while(tmpCase)
   {  FreeStatementList(tmpCase->stmtList);
      arg = tmpCase->valList;
      while(arg)
      {  delArg = arg;
         arg = arg->next;
         MyFree((void*)delArg);
      }

      delCase = tmpCase;
      tmpCase = tmpCase->next;
      MyFree((void*)delCase);
   }

   MyFree((void*)caseData);

   return(OK);
}

/*--------------------------------------------------------------------------*/
/* FreeIfStatement()                                                        */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long FreeIfStatement(tIfData *ifData)
{
   FreeExprTree(ifData->condExpr);
   FreeStatementList(ifData->primeSList);

   if(ifData->secondaryType==ELSE_STATEMENT)
      FreeStatementList(ifData->secondary.sList);
   else if(ifData->secondaryType==ELSE_IF_STATEMENT)
      FreeIfStatement(ifData->secondary.ifData);

   MyFree((void*)ifData);
   return(OK);
}


/*--------------------------------------------------------------------------*/
/* FreeParseResults()                                                       */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long FreeParseResults(tParseResults *parseResults, int freeVars)
{
   tParseResults *tmpPR, *delPR;
   tEngRBData    *engRB;
   tRecInstance  *tmpltRec, *delRec;

   /************************************************************/
   /* Free all variables and parameters allocated, if required */
   /************************************************************/
   if(freeVars)
   {  FreeAllDataList(parseResults->allData);
      FreeInternalGlobals(parseResults->engRB->RBData);

      /* Free the list of template record instances */
      tmpltRec = parseResults->tmpltRecList;
      while(tmpltRec)
      while(tmpltRec)
      {  delRec = tmpltRec;
         tmpltRec = tmpltRec->next;
         MyFree((void*)delRec);
      }
   }

   /************************************/
   /* Free all parseResults structures */
   /************************************/
   tmpPR = parseResults;
   engRB = parseResults->engRB;
   while(tmpPR)
   {  FreeStatementList(tmpPR->ruleStatementList);

      if(engRB != tmpPR->engRB)
      {  MyFree((void*)engRB);
         engRB = tmpPR->engRB;
      }

      delPR = tmpPR;
      tmpPR = tmpPR->next;
      MyFree(delPR);
   }

   MyFree((void*)engRB);

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* FreeRuleBase()                                                           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long FreeRuleBase(void *ruleBaseHandle)
{
   long       status=OK;
   tCalcTable *calcTab, *mainCalcTab = (tCalcTable*)ruleBaseHandle;
   tRuleCalc  *ruleCalc;

   /********************************************/
   /* Execute all the system termination rules */
   /********************************************/
   for(calcTab=mainCalcTab; calcTab; calcTab=calcTab->next)
   {  for(ruleCalc=calcTab->sysTermList; ruleCalc; ruleCalc=ruleCalc->next)
      {  mainCalcTab->ruleBaseData->status.errCode = OK;
         calcTab->currentRuleCalc = ruleCalc;
         CalcRuleStatements(ruleCalc, &mainCalcTab->ruleBaseData->status, calcTab);
      }

      if(calcTab->haveSysTermFuncs)
      {  if((status=RunSupportFunctions(calcTab, SYSTEM_TERM)))
         {  status = MakeEngineError(FATAL_ENGINE|RUN_SUPPORT_FUNC|status, mainCalcTab);
            throw(ENGINE_EXCEPTION);
         }
      }
   }


   /*********************/
   /* Free error string */
   /*********************/
   MyFree((void*)mainCalcTab->ruleBaseData->status.errStr);

   /*************************/
   /* Call FreeCalcTables() */
   /*************************/
   FreeCalcTables(mainCalcTab);

   return(status);
}


/*--------------------------------------------------------------------------*/
/* FreeCalcRule()                                                           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long FreeCalcRule(tCalcRule *calcRule)
{
   if(!calcRule)
      return(OK);

   if(calcRule->code)
      MyFree((void*)calcRule->code);

   MyFree((void*)calcRule);

   return(OK);
}
