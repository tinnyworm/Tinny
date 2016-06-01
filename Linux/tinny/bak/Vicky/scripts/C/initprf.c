/*
** $Id: initprf.c,v 1.2 1999/08/26 00:23:15 xxs Exp $ 
** $Author: xxs $
** $Date: 1999/08/26 00:23:15 $
** $Locker:  $
**
*/

#include <string.h>


#include "maxerr.h"
#include "falcon.h"
#include "maxstrc.h"
#include "engine.h"
#include "rulproc.h"
#include "mgmodel.h"

// XXS: change profile from (REAL*) to (char*).
// profile is (char *) everywhere else.
// so keep it consistent.
void INITPROF(char *profile, BYTE *tran, void *handle, long *errCode)
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

   /***************************************************************/
   /* Copy the profile and tran areas into the registered buffers */
   /***************************************************************/
   memcpy(max->tranData, (char*)tran, max->tranLen);
   memcpy(max->profData, profile, max->profLen);

   /***************************/
   /* Initialize the profile  */
   /***************************/
   status = InitProfiles(max->ruleMg->modelHandle);
   if(status)
   {  *errCode = status | MAX;

      if(notWarning(status))
      {  memcpy(profile, max->profData, max->profLen);
         return;
      }
   }

   /*****************************************************/
   /* Copy the new profile back to the passed in buffer */
   /*****************************************************/
   memcpy(profile, max->profData, max->profLen);
}
