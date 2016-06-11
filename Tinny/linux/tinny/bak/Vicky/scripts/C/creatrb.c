/*
** $Id: creatrb.c,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: creatrb.c,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.3  1997/02/18  21:48:47  can
 * Fix bug that was clobbering the warning return status when SYS_INIT rules are present
 *
 * Revision 2.2  1997/01/16  22:44:06  can
 * Lots of changes for 2.1 release
 *
 * Revision 2.1  1996/04/22  18:01:49  can
 * Took out call to OutputCalcTable
 *
 * Revision 2.0  1996/03/18  18:45:44  gak
 * GUTS 2.0.1 alpha release
 *
 * Revision 1.10  1995/12/12  19:33:21  can
 * Took out extraneous call to FreeParseResults (not sure why it
 * was there at all!)
 *
 * Revision 1.9  1995/08/31  00:05:52  can
 * Fixed more compiler warnings.
 *
 * Revision 1.8  1995/08/16  17:59:31  can
 * Made changes to ensure that parseResults get free'd properly.
 *
 * Revision 1.7  1995/08/11  18:25:24  can
 * Added status errStr malloc here.  Also, calling FreeParseResults
 * before throwing an exception.
 *
 * Revision 1.6  1995/08/02  00:12:15  can
 * Added code to execute the system initialization rules from this
 * function.
 *
 * Revision 1.4  1995/07/21  21:46:14  can
 * Took out addition of severity and class from return of CreateCalcTable
 *
 * Revision 1.3  1995/07/21  15:35:44  can
 * Modified scheme for exception handling.
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

/* HNC Rule engine include files */
#include "mymall.h"
#include "exptree.h"
#include "prsrule.h"
#include "engerr.h"
#include "creatct.h"
#include "runfunc.h"
#include "engine.h"
#include "runct.h"

/* HNC common include files */
#include "except.h"

/*
#ifdef DEVELOP
#include "rbdebug.h"
void OutputParseResults(tParseResults *parseResults);
#endif
*/


long FreeParseResults(tParseResults *parseResults, int freeVars);



/*--------------------------------------------------------------------------*/
/* CreateRuleBase()                                                         */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long CreateRuleBase(tRuleBaseData *ruleBaseData, void **ruleBaseHandle)
{
   long           retStatus=OK, status;
   tParseResults  *parseResults=NULL;
   tRuleCalc      *ruleCalc;
   tCalcTable     *calcTab, *mainCalcTab;

   /*********************************************************/
   /* Allocate the calc table, and set up the return handle */         
   /* if one is required.                                   */
   /*********************************************************/
   if(ruleBaseHandle)
   {  if(AllocateCalcTable(&mainCalcTab))
         throw(MALLOC_EXCEPTION);
      *ruleBaseHandle = (void*)mainCalcTab; 
   }

   /***************************************/
   /* Allocate space for the error string */
   /***************************************/
   if((ruleBaseData->status.errStr = (char*)MyMalloc(MAX_ERR_STR))==NULL)
      throw(MALLOC_EXCEPTION);

   /***********************************************************************/
   /* Parse all the rules in the rule base.  This will do all syntax and  */
   /* type checking.  In addition, if all rules are error free and it is  */
   /* required, it will set up a "compiled" list of statements and        */
   /* expressions that can be converted into the run-time table format.   */ 
   /***********************************************************************/
   if(ruleBaseHandle) status=ParseRuleBase(ruleBaseData, (void**)&parseResults);
   else status=ParseRuleBase(ruleBaseData, NULL);

   if(status)
   {  status = MakeParseError(PARSE_RB|status, ruleBaseData, TRUE); 
      if(notWarning(status)) 
      {  /***************************************************/
         /* Free up memory from the parse results structure */
         /***************************************************/
         if(parseResults) FreeParseResults(parseResults, TRUE);
         throw(ENGINE_EXCEPTION);
      }
      else retStatus = status;
   }


   /*************************************************************************/
   /* Return here if no rule base handle is requested, which indicates that */
   /* only parsing is required                                              */
   /*************************************************************************/
   if(!ruleBaseHandle) 
   {  if(retStatus) throw(ENGINE_EXCEPTION);

      MyFree((char*)ruleBaseData->status.errStr);
      return(OK);
   }
  

   /*************************************************************/
   /* Take the parse results and create the run-time calc table */         
   /*************************************************************/
   if((status=CreateCalcTable(ruleBaseData, parseResults, mainCalcTab)))
   {  status = MakeEngineError(status, mainCalcTab); 
     
      if(notWarning(status)) 
      {  if(parseResults) FreeParseResults(parseResults, TRUE);
         throw(ENGINE_EXCEPTION);
      }
      else retStatus = status;
   }


   /***************************************************/
   /* Free up memory from the parse results structure */
   /***************************************************/
   if(parseResults) FreeParseResults(parseResults, FALSE);


   /*********************************************************/
   /* Loop through all the calc tables to rule all sys init */
   /* rules and SYSTEM_INIT functions.                      */
   /*********************************************************/
   for(calcTab=mainCalcTab; calcTab; calcTab=calcTab->next)
   {  for(ruleCalc=calcTab->sysInitList; ruleCalc; ruleCalc=ruleCalc->next)
      {  mainCalcTab->ruleBaseData->status.errCode = OK;
         calcTab->currentRuleCalc = ruleCalc;
         CalcRuleStatements(ruleCalc, &mainCalcTab->ruleBaseData->status, calcTab);
      }

      if(calcTab->haveSysInitFuncs)
      {  if((status=RunSupportFunctions(calcTab, SYSTEM_INIT)))
         {  status = MakeEngineError(FATAL_ENGINE|RUN_SUPPORT_FUNC|status, mainCalcTab); 
            throw(ENGINE_EXCEPTION);
         }
      }
   }


   /*********************************************/
   /* Now throw exception if was only a warning */
   /*********************************************/
   if(retStatus) 
   {  mainCalcTab->ruleBaseData->status.errCode = retStatus;
      throw(ENGINE_EXCEPTION);
   }

   return(retStatus);
}
