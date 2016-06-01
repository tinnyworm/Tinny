/*
** $Id: initnet.c,v 1.1 1998/06/08 22:55:28 jcb Exp $ 
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mymall.h"
#include "maxerr.h"
#include "falcon.h"
#include "maxstrc.h"
#include "engine.h"
#include "rulproc.h"
#include "typechk.h"

#ifdef DEVELOP
extern short gDoMem;
extern char  *gCreateMemFile;
/* extern char  *gReadMemFile; */
#endif


long gProfileSize;


void INITNET(char *configFile, void **handle, INT32 *errCode)
{
   long          status;
   tMax          *max;
   tDataTemplate *tplt;
#ifdef DEVELOP
   char          *block;
#endif

#ifdef DEVELOP
   /* if(gReadMemFile)
   {  FILE *fp;
      long size;

      if((fp = fopen(gReadMemFile, "r"))==NULL)
      {  fprintf(stderr, "Can't open memory load image "
                         "file '%s'\n", gReadMemFile);
         exit(0);
      }

      if(!fread((char*)&size, sizeof(long), 1, fp))
      {  fprintf(stderr, "Error reading size from memory load "
                         "file '%s'\n", gReadMemFile);
         exit(0);
      }

      if((block = MyMalloc(size)) == NULL)
      {  fprintf(stderr, "Can't malloc memory image of size %ld "
                         "bytes\n", size);
         exit(0);
      }

      if(!fread((char*)block, size, 1, fp))
      {  fprintf(stderr, "Error reading %ld data from memory load "
                         "file '%s'\n", size, gReadMemFile);
         exit(0);
      }
   
      ITM_SET_GLBL(block);
      ITM_OFFS_2_ABS(block);
      ITM_GET_PNET((void**)&max);

      ResetFunctionPointers((void*)max, errCode);

      max->flags |= USED_MEMFILE;
      *handle = (void*)max;

      return;
   } */
#endif

   /**********************************************************/
   /* Allocate the max structure, which is the return handle */
   /**********************************************************/
   *handle = NULL;
   *errCode = OK;
   if((max = (tMax*)MyMalloc(sizeof(tMax)))==NULL)
   {  *errCode = FATAL_MAX|MAX_INIT|OUT_OF_MEMORY;
      return;
   }
   memset((char*)max, 0, sizeof(tMax));

   /**********************************************/
   /* Load the rule bases, models, and templates */
   /**********************************************/
   status = RulesInitialize(&max->ruleMg, configFile);
   if(status && notWarning(status))
   {  *errCode = status;
      return;
   }
   else
      *errCode = status;

   /*****************************************************/
   /* Find the transaction and profile template buffers */
   /*****************************************************/
   if(max->ruleMg)
   {  for(tplt=max->ruleMg->masterData.templateList; tplt; tplt=tplt->next)
      {  if(!strcmp(tplt->templateName, TRAN_TEMPLATE_NAME))
         {  max->tranLen = tplt->dataSize;
            max->tranData = tplt->data;
         }
         else if(!strcmp(tplt->templateName, PROFILE_TEMPLATE_NAME))
         {  max->profLen = tplt->dataSize;
            max->profData = tplt->data;
            gProfileSize = tplt->dataSize;
         }
         else if(!strcmp(tplt->templateName, SCORE_TEMPLATE_NAME))
         {  max->scoreLen = tplt->dataSize;
            max->scoreData = tplt->data;
         }
         else if(!strcmp(tplt->templateName, EXPLAIN_TEMPLATE_NAME))
         {  max->explnLen = tplt->dataSize;
            max->explnData = tplt->data;
         }
      }
   }

   /**************************************************************/
   /* If there is no match on the template names, then error out */
   /**************************************************************/
   if(max->tranData == NULL)
   {  *errCode = (FATAL_MAX|MAX_INIT|NO_TRAN_TEMPLATE);
      return;
   }
   else if(max->profData == NULL)
   {  *errCode = (FATAL_MAX|MAX_INIT|NO_PROF_TEMPLATE);
      return;
   }
   else if(max->scoreData == NULL)
   {  *errCode = (FATAL_MAX|MAX_INIT|NO_SCORE_TEMPLATE);
      return;
   }
   else if(max->explnData == NULL)
   {  *errCode = (FATAL_MAX|MAX_INIT|NO_EXPLN_TEMPLATE);
      return;
   }

   /**********************************************************/
   /* Set a self pointer in the max structure for validation */
   /**********************************************************/
   max->self = max;

#ifdef DEVELOP
#ifdef ITM
   if(gCreateMemFile)
   {  FILE *fp;
      long size;
      char *start;

      RegisterPMaxPointers(max, errCode);

      ITM_SET_PNET(max);

      if((fp = fopen(gCreateMemFile, "w"))==NULL)
      {  fprintf(stderr, "Can't create memory file '%s'\n", gCreateMemFile);
         exit(1);
      }

      size = ITM_GETTOTAL();
      fwrite((char*)&size, sizeof(long), 1, fp);

      start = ITM_GETSTRTPTR();
      ITM_ABS_2_OFFS();
      fwrite((char*)start, size, 1, fp);

      fprintf(stderr, "Created memory file '%s'... exiting now\n", gCreateMemFile);

      exit(0);
   }
   else if(gDoMem)
   {  RegisterPMaxPointers(max, errCode);

      ITM_SET_PNET(max);
      ITM_ABS_2_OFFS();

      block = (char*)malloc(4096*1024*2);
      ITM_COPY_AREA(block);
   
      ITM_OFFS_2_ABS(block);
      ITM_GET_PNET((void**)&max);

      PrintLogCt();
   }
#endif
#elif ITM
   RegisterPMaxPointers(max, errCode);
#endif

   /*****************************/
   /* Set the handle and return */
   /*****************************/
   *handle = (void*)max;
}
