/*
** $Id: freenet.c,v 1.1 1998/06/08 22:55:28 jcb Exp $ 
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
**
*/


#include "maxerr.h"
#include "falcon.h"
#include "maxstrc.h"
#include "rulproc.h"
#include "mymall.h"
#include "hnctype.h"


void FREENET(void *handle, INT32 *errCode)
{
   tMax *max = (tMax*)handle;
   long status;

   *errCode = OK;

   /***********************************************************/
   /* Check the self pointer the make sure the handle is good */
   /***********************************************************/
   if(max->self != max)
   {  *errCode = FATAL_MAX|MAX_SCORE|BAD_HANDLE;
      return;
   }

   /**************************************************/
   /* If USED_MEMFILE flag is set, don't do anything */
   /**************************************************/
   if(max->flags&USED_MEMFILE)
      return;

   /**********************************/
   /* Free the rule manage structure */
   /**********************************/
   status = FreeRules(max->ruleMg);
   if(status && notWarning(status))
   {  *errCode = status;
      return;
   }
   else
      *errCode = status;

   /**************************/
   /* Free the max structure */
   /**************************/
   MyFree((void*)max);
}
