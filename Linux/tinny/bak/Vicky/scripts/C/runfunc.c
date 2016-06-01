/*
** $Id: runfunc.c,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: runfunc.c,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.1  1996/05/01  01:19:14  can
 * Added setting of inputData in the funcCallData structure
 *
 * Revision 1.7  1995/09/29  21:29:28  can
 * Fixed bug where the properly allocated error structure (error string)
 * was not getting passed to the functions.
 *
 * Revision 1.6  1995/07/28  00:15:46  can
 * Added ruleBaseName to ruleBaseData, so access to it comes from
 * ruleBaseData instead of calcTab
 *
 * Revision 1.5  1995/07/25  16:01:10  can
 * Added ruleBaseData to tFuncCallData
 *
 * Revision 1.4  1995/07/21  15:35:44  can
 * Modified to use tFuncCallData structure
 *
 * Revision 1.3  1995/07/13  22:36:30  can
 * Added inputData parameter to system initialization function call
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

/* HNC Rules Engine include files */
#include "exptree.h"
#include "prsrule.h"
#include "engerr.h"
#include "calctab.h"
#include "runfunc.h"

/* HNC Common include files */
#include "except.h"

#ifdef DEVELOP
#include "rbdebug.h"
#endif


/*--------------------------------------------------------------------------*/
/* RunSupportFunctions()                                                    */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long RunSupportFunctions(tCalcTable* ruleCalcTab, int funcType)
{
   long          status=OK;
   tUsedFunc     *usedFunc = ruleCalcTab->usedFuncList;
   tFuncCallData funcCallData;

   /**************************************/
   /* Make sure table has been allocated */
   /**************************************/
   if(!ruleCalcTab)
      return(FATAL_ENGINE|RUN_SUPPORT_FUNC|NO_CALC_TABLE);


   /***************************************************************/
   /* Loop through all the used functions and run those functions */
   /* of the specified function type.                             */
   /***************************************************************/
   while(usedFunc)
   {  funcCallData.errStatus.errCode = OK; 
      funcCallData.errStatus.errStr = ruleCalcTab->ruleBaseData->status.errStr;
      funcCallData.funcData = usedFunc->func->funcData;
      funcCallData.inputData = usedFunc->func->inputData;
      funcCallData.ruleBaseData = ruleCalcTab->ruleBaseData;
      switch(funcType)
      {  case SYSTEM_INIT: 
            if(usedFunc->func->SysInitFuncPtr)
               usedFunc->func->SysInitFuncPtr(&funcCallData,
                                              usedFunc->func->inputData);
            usedFunc->func->funcData = funcCallData.funcData;
            break;
         case ITER_INIT: 
            if(usedFunc->func->IterInitFuncPtr)
               usedFunc->func->IterInitFuncPtr(&funcCallData);
            break;
         case SYSTEM_TERM: 
            if(usedFunc->func->SysTermFuncPtr)
               usedFunc->func->SysTermFuncPtr(&funcCallData);
            break;
         case ITER_TERM: 
            if(usedFunc->func->IterTermFuncPtr)
               usedFunc->func->IterTermFuncPtr(&funcCallData);
            break;
      }

      if(funcCallData.errStatus.errCode != OK)
      {  ruleCalcTab->ruleBaseData->status.errCode = funcCallData.errStatus.errCode;
         ruleCalcTab->ruleBaseData->status.errStr = funcCallData.errStatus.errStr;
         throw(ENGINE_EXCEPTION);

         /************************************************/
         /* Really, this is unecessary because the throw */
         /* will never come back for a fatal error       */
         /************************************************/
         if(isFatal(funcCallData.errStatus.errCode))
            break;

         status = FATAL_ENGINE|RUN_SUPPORT_FUNC|FUNCTION_FAILED;
      }

      usedFunc = usedFunc->next;
   }
   
   return(status);
}
