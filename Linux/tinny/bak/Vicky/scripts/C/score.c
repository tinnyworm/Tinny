/*
** $Id: score.c,v 1.1 1998/06/08 22:55:28 jcb Exp $ 
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
**
*/

#include <string.h>
#include <math.h>


#include "maxerr.h"
#include "falcon.h"
#include "maxstrc.h"
#include "engine.h"
#include "rulproc.h"
#include "scorstr.h"
#include "flib.h"
#include "std.h"

#define MAXcomp(x,y) ((x<y)?(y):(x))
#define MINcomp(x,y) ((x<y)?(x):(y))

extern BOOL  scoreMod;

void CSCORE(BYTE* tran, char* profile, SCORESTRUCT* score, void* handle, int explain, INT32 *errCode)
{
   tMax *max = (tMax*)handle;
   long status;

   float  amount, amtmax, amtfac, credline, crdmax;

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
   memcpy(max->profData, (char*)profile, max->profLen);
   memcpy(max->explnData, (char*)&explain, max->explnLen);

   /********************************/
   /* Start the score data at zero */
   /********************************/
   memset(max->scoreData, 0, max->scoreLen);

   /***************************/
   /* Process the transaction */
   /***************************/
   status = ProcessData(max->ruleMg);
   if(status && notWarning(status))
   {  *errCode = (INT32)status;
      memcpy((char*)profile, max->profData, max->profLen);
      if(score) memcpy((char*)score, max->scoreData, max->scoreLen);
      return;
   }
   else
      *errCode = status;

   /*****************************************************/
   /* Copy the new profile back to the passed in buffer */
   /*****************************************************/
   memcpy((char*)profile, max->profData, max->profLen);

   /****************************/
   /* Copy the score area back */
   /****************************/
   if (score) memcpy((char*)score, max->scoreData, max->scoreLen);

   if ( scoreMod ) {
     if( (void *)score != (void *)NULL) {
       amount = (float) xatofn(tran+off(TRAN_AMOUNT), len(TRAN_AMOUNT));
       credline = (float) xatofn(tran+off(CREDIT_LINE), len(CREDIT_LINE));
       amtmax = (float) MAXcomp(amount,1000.);
       if ( credline > 10. )
         crdmax = 2.*amtmax/credline;
       else
         crdmax = 1.;
       amtfac = (float) log10((double)(amtmax/1000.)) * (float) MAXcomp(crdmax,1.);
       score->pFraudScore = (int) ( (amtfac*1000. + (float) score->pFraudScore)
				  / (amtfac+1.) );
      }
   }
}
