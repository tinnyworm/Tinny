

#include <string.h>
#include <stdlib.h>

#include "hncrtyp.h"
#include "typechk.h"
#include "config.h"
#include "except.h"

#include "maxerr.h"
#include "falcon.h"
#include "maxstrc.h"
#include "rulproc.h"
#include "engdata.h"

/* the following info MIGHT just be helpful in interpreting this code */
/*
   max :  Passed to function
               defined in maxstrc.h
               the struct containing pointers to data templates and a rules mgr.
   max->ruleMg
               the tRuleManage struct in tMax struct
               struct defined in rulemgt.h
	       contains pointers to template map, the rules db, master (?) data,
	         and a (void *), for generality, model handle.
   max->ruleMg->modelHandle      
               a void * struct in tRuleManage struct -
		 must type cast to tModelManage struct for FALCON
	       tModelManage defined in mgmodel.h
	       contains pointers to master data (?), model sets, and the last model
	         fired (?).  also contains count of models and model sets.
   max->ruleMg->modelHandle->setList
               a tModelSet struct in tModelManage struct
               struct defined in mgmodel.h
	       contains a list of all model sets and a list of shared variables
   max->ruleMg->modelHandle->setList->modelList
               a tModel struct in tModelSet struct
               defined in model.h
                 contains a list of models in a particular set
	       the tModel struct contains 
   max->ruleMg->modelHandle->setList->modelList->modelRBData
	       a tRuleBaseData struct in the tModel struct
               defined in engine.h
	       an element in a null terminated linked list of different rule bases
	         for each model (?)
	       contains lists of rules, functions, and variables
   max->ruleMg->modelHandle->setList->modelList->modelRBData->globalVarList
	       a tEngVar struct in the tRuleBaseData struct
               defined in engdata.h
	       contains an actual variable name, type (float, int, etc),
	         flags (CON, VAR, TBL, etc), and the data value and size.
	       SIX Levels DEEP your data values lie.
*/

/* note the varName passed in below must match the name given in *.tab file */

void setModelVar(void* handle, char *varName, char *value, INT32 *errCode)
{

   tMax *max = (tMax*)handle;
   tModelManage  *modelMngr=NULL;
   tModelSet     *modelSet=NULL;
   tModel        *model=NULL;
   tRuleBaseData *modelRBD = NULL;
   tEngVar       *modelVar = NULL;

   int  found  =FALSE;
   char      errMsg[1024];

   *errCode = OK;

   /***********************************************************/
   /* Check the self pointer the make sure the handle is good */
   /***********************************************************/
   if(max->self != max)
   {  *errCode = FATAL_MAX|MAX_SCORE|BAD_HANDLE;
      return;
   }

   /*********************************************/
   /*  Search lists to find the model varName   */

   modelMngr = (tModelManage *) max->ruleMg->modelHandle;

   for(modelSet=modelMngr->setList; modelSet!=NULL; modelSet=modelSet->next) {
     found = FALSE;
     for(model=modelSet->modelList; model!=NULL; model=model->next) {
       found = FALSE;
       for(modelRBD=model->modelRBData;
            modelRBD!=NULL && !found; modelRBD=modelRBD->next) {
         for(modelVar=modelRBD->globalVarList;
              modelVar!=NULL && !found; modelVar=modelVar->next) {
           /*  FINALLY, have a variable list to search through */
           if(NoCaseStrcmp(varName, modelVar->varName)) {
               continue;
           } else {
               found = TRUE;
           }
           /* if found var name, check that is constant */
	   if (modelVar->flags & VAR_CONSTANT) {
             /* if var is constant, convert the char string to proper type */
	     if (modelVar->type == HNC_STRING) {
                strcpy((char*)modelVar->data, value); 
                modelVar->dataSize = strlen(value);
                fprintf(stderr, "\nThe variable %s was reset to \"%s\" in model %s.\n",
	            modelVar->varName, *((char*)modelVar->data), model->modelName );
	     } else
	     if (modelVar->type == HNC_FLOAT)  {
                modelVar->dataSize = sizeof(float);
                *((float*)modelVar->data) = (float)atof(value);
                fprintf(stderr, "\nThe variable %s was reset to \"%f\" in model %s.\n",
	            modelVar->varName, *((float*)modelVar->data), model->modelName );
	     } else
	     if (modelVar->type == HNC_INT32)  {
                modelVar->dataSize = sizeof(long);
                *((long*)modelVar->data) = strtol(value, NULL, 16);
                fprintf(stderr, "\nThe variable %s was reset to \"%ld\" in model %s.\n",
	            modelVar->varName, *((long*)modelVar->data), model->modelName );
	     } else {
                sprintf(errMsg, "In setModelVar, bad constant value '%s' for "
                                "constant '%s' in model %s\n",
		    value, modelVar->varName, model->modelName);
                SetConfigErrorMessage(errMsg);
                throw(CONFIGURATION_EXCEPTION);
	     }  /* end of if conversion type */
	   }  /* end of if constant  */           
         }  /* end of modelVar for loop */
       }  /* end of modelRBD for loop */
     }  /* end of model    for loop */
   }  /* end of modelSet for loop */
}
