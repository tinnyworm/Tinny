/*
** $Id: execrb.c,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: execrb.c,v $
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
 * Revision 1.4  1995/08/31  00:05:52  can
 * Fixed more compiler warnings.
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

/* HNC Rules engine include files */
#include "exptree.h"
#include "prsrule.h"
#include "engerr.h"
#include "calctab.h"
#include "runfunc.h"
#include "runct.h"

/* HNC Common include files */
#include "except.h"

#ifdef DEVELOP
#include "rbdebug.h"
#endif


/*--------------------------------------------------------------------------*/
/* ExecutateRuleBase()                                                      */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long ExecuteRuleBase(void *ruleBaseHandle)
{
   long           status=OK;
   tCalcTable     *calcTab = (tCalcTable*)ruleBaseHandle;


   /**********************************************/
   /* Run all iteration initialization functions */
   /**********************************************/
   if(calcTab->haveIterInitFuncs)
   {  if((status=RunSupportFunctions(calcTab, ITER_INIT)))
      {  status = MakeEngineError(status, calcTab);
         throw(ENGINE_EXCEPTION);
         if(isFatal(status)) return(status);
      }
   }

    
   /***************************************/
   /* Reset the dynamic data pool, if any */
   /***************************************/
   if(calcTab->dynPool)
      ResetDynamicPool(ruleBaseHandle);


   /*************************************************************/
   /* All the work is done in this function which loops through */
   /* all the calc nodes in the table and performs the          */
   /* calculations.                                             */ 
   /*************************************************************/
   if((status=RunCalcTable(calcTab)))
   {  status = MakeEngineError(status, calcTab);
      throw(ENGINE_EXCEPTION);
      if(isFatal(status)) return(status);
   }
  

   /****************************************************/
   /* Finally, run all iteration termination functions */
   /****************************************************/
   if(calcTab->haveIterTermFuncs)
   {  if((status=RunSupportFunctions(calcTab, ITER_TERM)))
      {  status = MakeEngineError(status, calcTab);
         throw(ENGINE_EXCEPTION);
      }
   }

   return(NUM_GETLNG(calcTab->retValue.data.num));
}
