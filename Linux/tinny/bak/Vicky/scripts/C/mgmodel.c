/*
** $Id: mgmodel.c,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: mgmodel.c,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.10  1997/04/19  00:51:19  can
 * Added check for numCalcs==0 before malloc in SetCalcDones
 *
** Revision 2.9  1997/02/19 01:46:48  can
** Modified the severity mask in GetModelErrorString from 0xFF000000 to
** 0xFFFF0000.  This is because the lastest models are setting the second
** byte as well for the severity.
**
** Revision 2.8  1997/01/25  07:15:31  can
** Fixed bug when parsing multiple MODEL_SET lines
**
 * Revision 2.7  1997/01/21  21:02:50  can
 * Changed creation of modelPath to include the '/'.  This was
 * done for the mainframe which doesn't have paths like that.
 *
 * Revision 2.6  1997/01/21  19:03:26  can
 * Changed _InitProfiles and _ResetModels back to previous names, without '_'.
 * Also, changed the rule wrapper function InitProfiles to InitProfilesFunc,
 * and did similarly with the ResetModels wrapper.
 *
 * Revision 2.5  1997/01/21  02:00:28  gak
 * corrected call to NoCaseMemcmp
 *
** Revision 2.4  1997/01/21 00:36:04  gak
** Created rule wrapper functions for ResetModels and InitProfiles
**
** Revision 2.3  1997/01/20 23:52:48  can
** Check memcmp to NoCaseMemcmp in FireModel
**
 * Revision 2.2  1997/01/17  17:22:36  can
 * Updates for latest release
 *
**
*/

/* System include files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* HNC Rule Engine includes */
#include "mymall.h"
#include "engine.h"
#include "engerr.h"
#include "error.h"
#include "typechk.h"

/* HNC common includes */
#include "except.h"
#include "config.h"
#include "mgmodel.h"
#include "dyntabl.h"
#include "rulfunc.h"
#include "modfunc.h"
#include "varcalc.h"



void FindMatchingLongConstant(long value, char *name, tEngVar *varList) 
{
   tEngVar *var;

   name[0] = '\0';
   for(var=varList; var; var=var->next)
   {  if(var->flags&VAR_CONSTANT && var->type==HNC_INT32)
      {  if(*((long*)var->data) == value)
         {  strcpy(name, var->varName);
            break;
         }
      }
   }
}


tAtom GetModelErrorString(tFuncCallData* funcCallData, tAtom *aParams, int numArgs)
{
   tAtom         retVal;
   char          errMsg[1024], severity[100], reason[100];
   tModelSet     *modelSet=NULL;
   tModel        *model=NULL;
   tModelManage  *modelManage = 
      (tModelManage*)((tFuncInit*)funcCallData->inputData)->modelHandle;

   if(!modelManage)
   {  retVal.type = NUMERIC_TYPE;
      ZERO(retVal.data.num)
#ifdef DEVELOP
      fprintf(stderr, "Failed in GetModelErrorString--No models are specified\n");
#endif
      return(retVal);
   }

   /* This will print a string for known FireModel error codes */
   if((NUM_GETLNG(aParams[1].data.num)&0x000000FF) == NO_MODELS) 
   {  sprintf(errMsg, "Error '%lx':  No models have been initialized, "
                      "so model '%s' failed to fire",
                        NUM_GETLNG(aParams[1].data.num), aParams[0].data.strVal.str);
   }
   else if((NUM_GETLNG(aParams[1].data.num)&0x000000FF) == MODEL_UNDEFINED) 
   {  sprintf(errMsg, "Error '%lx':  Model '%s' has not been initialized "
                      "so it could not be fired",
                        NUM_GETLNG(aParams[1].data.num), aParams[0].data.strVal.str);
   }

   /* Find the model and get the names of the constants that comprise the error code */
   else
   {  for(modelSet=modelManage->setList; modelSet!=NULL; modelSet=modelSet->next)
      {  for(model=modelSet->modelList; model!=NULL; model=model->next)
         {  if(!NoCaseMemcmp(model->modelName, aParams[0].data.strVal.str,
            	aParams[0].data.strVal.len))
               break;
         }
 
         if(model) break;
      }
      
      if(!model)
      {  sprintf(errMsg, "Can't create error message for model '%s' because "
                         "it hasn't been initialized", aParams[0].data.strVal.str);
      }
      else
      {  FindMatchingLongConstant(NUM_GETLNG(aParams[1].data.num)&0xFFFF0000, 
                                       severity, model->modelRBData->globalVarList);
         FindMatchingLongConstant(NUM_GETLNG(aParams[1].data.num)&0x000000FF, 
                                       reason, model->modelRBData->globalVarList);

         if(severity[0] == '\0') strcpy(severity, "UNKNOWN_SEVERITY");
         if(reason[0] == '\0') strcpy(reason, "UNKNOWN_REASON");

         sprintf(errMsg, "Model '%s' fired with return code '%lx' "
                         "-- Severity: '%s', Reason: '%s'", 
                         aParams[0].data.strVal.str, NUM_GETLNG(aParams[1].data.num),
                         severity, reason);
      }
   }

   memcpy(aParams[2].data.strVal.str, errMsg, aParams[2].data.strVal.len);
   
   return(retVal);
}


tAtom FireModel(tFuncCallData* funcCallData, tAtom *aParams, int numArgs)
{
   tModelSet     *modelSet=NULL;
   tModel        *model=NULL;
   int           ran=FALSE;
   long          error=OK;
   tAtom         retVal;
   tFuncInit     *funcInit = (tFuncInit*)funcCallData->inputData;
   tModelManage  *modelManage = (tModelManage*)funcInit->modelHandle;

   if(!modelManage)
   {  retVal.type = HNC_STRING;
      NUM_SETINT32((*((tEngNum*)aParams[1].data.uVal)), 
                          (FAIL_ENGINE|FIREMODEL|NO_MODELS))
      retVal.data.strVal.str = "";
      retVal.data.strVal.len = 0;
#ifdef DEVELOP
      fprintf(stderr, "Failed in FireModel--No models are specified\n");
#endif
      return(retVal);
   }
 
   /******************************************************************/
   /* Loop through the list of all model sets and models to find the */
   /* requested model.                                               */
   /******************************************************************/
   for(modelSet=modelManage->setList; modelSet!=NULL && !ran; modelSet=modelSet->next)
   {  for(model=modelSet->modelList; model!=NULL; model=model->next)
      {  if(!NoCaseMemcmp(model->modelName, 
                  aParams[0].data.strVal.str, aParams[0].data.strVal.len))
         {  
#ifdef GETDATA
            /******************************/
            /* Get any required ISAM data */
            /******************************/
            GetModelData(model);
#endif

            /**********************************************************/
            /* Set the global current model for the scoring functions */
            /**********************************************************/
            modelManage->currModel = model;
            funcInit->dynTableList = (void*)model->tableList;

            /***************************************************/
            /* Execute the rule base that will score the model */
            /***************************************************/
            error = ExecuteRuleBase(model->modelRuleHandle);
            ran=TRUE;
  

            /********************************/
            /* Set the model ID for return */
            /********************************/
            retVal.data.strVal.str = model->modelName;
            retVal.data.strVal.len = strlen(model->modelName);
            break;
         }
      }
   }

   /*************************************/
   /* Check the error code from the run */
   /*************************************/
   if(!ran)
   {  NUM_SETINT32((*((tEngNum*)aParams[1].data.uVal)), 
                                (FAIL_ENGINE|FIREMODEL|MODEL_UNDEFINED))
      retVal.data.strVal.str = "";
      retVal.data.strVal.len = 0;
#ifdef DEVELOP
      fprintf(stderr, "Failed in FireModel--Model '%s' is not defined\n", 
                                      aParams[0].data.strVal.str);
#endif
   }
   else if(error!=OK)
   {  NUM_SETINT32((*((tEngNum*)aParams[1].data.uVal)), error)
#ifdef DEVELOP
      if(notWarning(error)) 
         fprintf(stderr, "Failed in FireModel with error code '%lx'\n", error);
      else
         fprintf(stderr, "Warning in FireModel with code '%lx'\n", error);
#endif
   }
   else
      ZERO((*((tEngNum*)aParams[1].data.uVal)))

   retVal.type = HNC_STRING;
   return(retVal);
}


tAtom InitProfilesFunc(tFuncCallData* funcCallData, tAtom *aParams, int numArgs)
{
   tAtom retVal;
   tModelManage  *modelManage =
      (tModelManage*)((tFuncInit*)funcCallData->inputData)->modelHandle;

   if(!modelManage)
   {  retVal.type = NUMERIC_TYPE;
      ZERO(retVal.data.num)
#ifdef DEVELOP
      fprintf(stderr, "Failed in GetModelErrorString--No models are specified\n");
#endif
      return(retVal);
   }

   NUM_SETINT32(retVal.data.num, InitProfiles((void*)modelManage));
   return(retVal);
}

tAtom ResetModelsFunc(tFuncCallData* funcCallData, tAtom *aParams, int numArgs)
{
   tAtom retVal;
   tModelManage  *modelManage =
      (tModelManage*)((tFuncInit*)funcCallData->inputData)->modelHandle;

   if(!modelManage)
   {  retVal.type = NUMERIC_TYPE;
      ZERO(retVal.data.num)
#ifdef DEVELOP
      fprintf(stderr, "Failed in GetModelErrorString--No models are specified\n");
#endif
      return(retVal);
   }

   NUM_SETINT32(retVal.data.num, ResetModels((void*)modelManage));
   return(retVal);
}


long ResetModels(void *modelHandle)
{
   tModelManage *modelManage = (tModelManage*)modelHandle;

   if(modelManage->numCalcDones)
      memset(modelManage->calcDones, 0, modelManage->numCalcDones);

   return(OK);
}


long InitProfiles(void *modelHandle)
{
   tModel       *model;
   tModelSet    *modelSet;
   tModelManage *modelManage = (tModelManage*)modelHandle;
   long         status;

   for(modelSet=modelManage->setList; modelSet; modelSet=modelSet->next)
   {  for(model=modelSet->modelList; model; model=model->next)
      {  modelManage->currModel = model;

         status=InitModelProfiles(model);
         if(status && notWarning(status))
            return(status);
      }
   }

   ResetModels(modelHandle);

   return(OK);
}


void LoadModelSet(tModelManage *modelManage, tModelSet *modelSet)
{
   tModel  *model;
   int     noShare;

   /* If there is only one model in the model set, */
   /* then don't need to set up shared variables   */
   noShare = (modelSet->modelList->next == NULL);
  
   for(model=modelSet->modelList; model; model=model->next)
   {  /***********************************************/
      /* Load all the information from the .tab file */
      /***********************************************/
      if(noShare)
         LoadModel(model, NULL, modelManage->masterData);
      else
         LoadModel(model, &modelSet->sharedVarList, modelManage->masterData);
   }
}


#ifdef MODELING

typedef struct sDepStrs {
   char inpDepend[4096];
   char varDepend[4096];
   char othDepend[4096];
} tDepStrs;


long GetInputsFromRule
(tDepStrs *depStrs, tEngVar *ruleVar, tModel *model, tEngVar *shareVars);


long gMakeScoreModels=0;
long WriteScoreModels(tModelManage *modManage)
{
   tModelSet *modSet;
   tModel    *model;

   /* Write all the individual models */
   for(modSet=modManage->setList; modSet; modSet=modSet->next)
      for(model=modSet->modelList; model; model=model->next)
         WriteScoreModel(model);

   /* Write the master score model */
   
   return(OK);
}



long InsertDepend(tDepStrs *depStrs, tModel *model, char *name)
{
   int  i=0, j;
   char tmp[128], *dependStr, *ptr=name;

   if(strchr(name, '.'))
   {  tDataTemplate  *tmplt;

      while(name[i]!='.') {tmp[i] = name[i]; i++;}
      tmp[i] = '\0';

      /* See if is an input field */
      for(tmplt=model->modelRBData->templateList; tmplt; tmplt=tmplt->next)
         if(!NoCaseStrcmp(tmplt->templateName, tmp))
            break;
 
      if(tmplt)
         dependStr = depStrs->inpDepend;
      else
         dependStr = depStrs->othDepend;
   }
   else
   {  tProfileMap *PRM;
      tEngVar     *var;

      dependStr = depStrs->othDepend;

      /* Convert variable name to C name, if available */
      for(var=model->modelRBData->globalVarList; var; var=var->next)
         if(!NoCaseStrcmp(var->varName, name))
            break;

      if(var)
      {  if(!memcmp(var->varName, "VAR", 3) && var->user)
            if(((tModelVar*)var->user)->cName) 
               ptr = ((tModelVar*)var->user)->cName;
      }
 

      /* See if variable is a profile variable */
      for(PRM=model->PRMList; PRM; PRM=PRM->next)
      {  if(!NoCaseStrcmp(PRM->varName, name))
         {  dependStr = depStrs->varDepend;
            break;
         }
         else if(!NoCaseStrcmp(PRM->varName, ptr))
         {  dependStr = depStrs->varDepend;
            break;
         }
      }
   }

   i=0;
   while(1)
   {  j=0;
      while(dependStr[i]!=';' && dependStr[i]!='\0')
      {  tmp[j] = dependStr[i];
         i++; j++;
      }

      tmp[j] = '\0';
      if(!NoCaseStrcmp(tmp, ptr))
         return(0);

      if(dependStr[i] == '\0') break;
      else i++;
   }

   if(i==0) sprintf(dependStr, "%s", ptr);
   else sprintf(dependStr+i, ";%s", ptr);

   return(1);
}


long GetDependencies
(tDepStrs *depStrs, tEngVar *var, tModel *model, tEngVar *shareVars)
{
   tModelVar *modelVar;
   tCalcVar  *arg;

   /* Don't output constants */
   if(var->flags&VAR_CONSTANT)
      return(OK);

   /* If have a calc rule, search the text for dependencies */ 
   if(var->calcRule)
      GetInputsFromRule(depStrs, var, model, shareVars);
   else
   {  if(var->user)
      {  modelVar = (tModelVar*)var->user;
         if(!modelVar->args || modelVar->funcCode == DO_NOTHING)
         {  char    vname[100];
            tEngVar *tmpv;

            if(!memcmp(modelVar->cName, "MAX", 3))
            {  if(modelVar->cName[3]=='B' || modelVar->cName[3]=='P')
               {  sprintf(vname, "MAXM%s", modelVar->cName+4); 

                  for(tmpv=model->modelRBData->globalVarList; tmpv; tmpv=tmpv->next)
                  {  if(tmpv->user)
                        if(!strcmp(((tModelVar*)tmpv->user)->cName, vname))
                        {  if(InsertDepend(depStrs, model, tmpv->varName))
                              GetDependencies(depStrs, tmpv, model, shareVars);
                           break;
                        }
                  }
               }

               if(modelVar->cName[3]=='P')
               {  sprintf(vname, "MAXB%s", modelVar->cName+4); 

                  for(tmpv=model->modelRBData->globalVarList; tmpv; tmpv=tmpv->next)
                  {  if(tmpv->user)
                        if(!strcmp(((tModelVar*)tmpv->user)->cName, vname))
                        {  if(InsertDepend(depStrs, model, tmpv->varName))
                              GetDependencies(depStrs, tmpv, model, shareVars);
                           break;
                        }
                  }
               }
            }
            return(OK);
         }

         /* Get dependencies of argument variables */
         for(arg=modelVar->args; arg; arg=arg->next)
         {  if(!strcmp(arg->var->varName, var->varName))
               continue;
            else
            {  if(arg->var->flags&VAR_CONSTANT) continue;
               if(!memcmp(modelVar->cName, "MAXM", 4))
               {  tModelVar *tmpMV;
                  if(arg->var->user)
                  {  tmpMV = (tModelVar*)arg->var->user;
                     if(!memcmp(tmpMV->cName, "MAXP", 4) ||
                        !memcmp(tmpMV->cName, "MAXB", 4))
                        continue;
                  }
               }
               if(InsertDepend(depStrs, model, arg->var->varName))
                  GetDependencies(depStrs, arg->var, model, shareVars);
            }
         }
      }
      else 
         return(OK);
   }
   
   return(OK);
}


long GetInputsFromRule
(tDepStrs *depStrs, tEngVar *ruleVar, tModel *model, tEngVar *shareVars)
{
   char          varNames[10240], name[128];
   int           i, j;
   tEngVar       *var;

   /* First get all the variables from the rule and get their dependencies */
   GetGlobalVarsInRule(varNames, ruleVar->calcRule->code);

   i=0;
   while(varNames[i]!='\0')
   {  j=0;
      while(varNames[i]!=',' && varNames[i]!='\0')
      {  name[j] = varNames[i];
         i++;
         j++;
      }
      name[j] = '\0';

      if(VarMatch(ruleVar, name)) 
      {  if(varNames[i] != '\0') 
         {  i++;
            continue;
         }
         else break;
      }

      for(var=model->modelRBData->globalVarList; var; var=var->next)
         if(VarMatch(var, name))
            break;

      if(var)
      {  if(InsertDepend(depStrs, model, name))
           GetDependencies(depStrs, var, model, shareVars);
      }

      if(varNames[i] != '\0') i++;
      else break;
   }

   
   /* Now get all the record usages in the rule */
   GetRecordsInRule(varNames, ruleVar->calcRule->code);

   i=0;
   while(varNames[i]!='\0')
   {  j=0;
      while(varNames[i]!=',' && varNames[i]!='\0')
      {  name[j] = varNames[i];
         i++;
         j++;
      }
      name[j] = '\0';

      InsertDepend(depStrs, model, name);

      if(varNames[i] != '\0') i++;
      else break;
   }

   return(OK);
}


extern FILE *gOutDepends;
long OutputVarDependencies(tEngVar *var, tModel *model, tEngVar *sharedVars)
{
   char        si, sv, so, *pi, *pv, *po;
   char        *name;
   tDepStrs    dp, *depStrs=&dp;
   int         ii=0, iv=0, io=0, indent, first=1, isProf=0;
   tProfileMap *PRM;

   memset(depStrs, 0, sizeof(tDepStrs));

   GetDependencies(depStrs, var, model, sharedVars);

   if(depStrs->inpDepend[0] == '\0')
      strcpy(depStrs->inpDepend, "*** NONE ***");
   if(depStrs->varDepend[0] == '\0')
      strcpy(depStrs->varDepend, "*** NONE ***");
   if(depStrs->othDepend[0] == '\0')
      strcpy(depStrs->othDepend, "*** NONE ***");

   name = var->varName;
   if(!memcmp(var->varName, "VAR", 3))
   {  if(var->user)
         name = ((tModelVar*)var->user)->cName;
   }

   /* See if variable is a profile variable */
   for(PRM=model->PRMList; PRM; PRM=PRM->next)
   {  if(!NoCaseStrcmp(PRM->varName, name))
      {  isProf=1;
         break;
      }
   }
   
   if(isProf) fprintf(gOutDepends, "* %-30.30s ", name);
   else fprintf(gOutDepends, "%-32.32s ", name);

   while(1)
   {  pi = depStrs->inpDepend+ii;
      pv = depStrs->varDepend+iv;
      po = depStrs->othDepend+io;

      while(depStrs->inpDepend[ii] != ';' && depStrs->inpDepend[ii] != '\0') ii++; 
      while(depStrs->varDepend[iv] != ';' && depStrs->varDepend[iv] != '\0') iv++; 
      while(depStrs->othDepend[io] != ';' && depStrs->othDepend[io] != '\0') io++; 

      si = depStrs->inpDepend[ii]; depStrs->inpDepend[ii] = '\0'; 
      sv = depStrs->varDepend[iv]; depStrs->varDepend[iv] = '\0'; 
      so = depStrs->othDepend[io]; depStrs->othDepend[io] = '\0';

      if(first)
      {  indent = 0;  
         first = 0;
      }
      else
         indent = 33;

      fprintf(gOutDepends, "%*.*s %-35.35s %-32.32s %-35.35s\n", 
                                 indent, indent, "", pi, pv, po); 

      if(si=='\0' && sv=='\0' && so=='\0')
         break;
      
      if(si==';') ii++;
      if(sv==';') iv++;
      if(so==';') io++;
   }

   fprintf(gOutDepends, "\n");

   return(OK);
}


long PrintDependencies(tModelManage *modManage)
{
   tModelSet *modSet;
   tModel    *model;
   tNetInput *input;
   tEngVar   *var, *outList=NULL, *outVar=NULL, *out, *delOut;
   
   /* For each model, print all the input data */
   /* dependencies, variable by variable       */ 
   for(modSet=modManage->setList; modSet; modSet=modSet->next)
   {  
      /* Output all the model specific variables */
      for(model=modSet->modelList; model; model=model->next)
      {  
         /* Put a header line in the file */
         fprintf(gOutDepends, "MODEL DEPENDENCIES: %s\n(note that inputs marked with a "
                              "'*' are profile variables)\n\n", model->modelName);
         fprintf(gOutDepends, "%-33.33s %-35.35s %-32.32s %-35.35s \n", 
             "INPUT","API FIELD","PROFILE VARIABLES","OTHER DEPENDENCIES");
         fprintf(gOutDepends, "%-33.33s %-35.35s %-32.32s %-35.35s \n", 
             "=====","=========","=================","==================");

         /* Go through all the net inputs for the model */
         for(input=model->inputList; input; input=input->next)
         {  
            /* Check for var on allready output list */
            for(out=outList; out; out=out->next)
            {  if(input->data == out->data)
                  break;
            }
            if(out) continue;

            for(var=model->modelRBData->globalVarList; var; var=var->next)
            {  if(input->data == var->data)  
                  break;
            }

            if(!var)
            {  for(var=modSet->sharedVarList; var; var=var->next)
               {  if(input->data == var->data)  
                     break;
               }

               if(!var)
               {  fprintf(stderr, "Can't find variable for "
                                  "input '%s'\n", input->name);
                  continue;
               }
            }
     
            OutputVarDependencies(var, model, modSet->sharedVarList);
   
            /* Add the variable to the list of already output variables */
            if(outList)
            {  outVar->next = (tEngVar*)MyMalloc(sizeof(tEngVar));
               outVar = outVar->next;
            }
            else
            {  outVar = (tEngVar*)MyMalloc(sizeof(tEngVar));
               outList = outVar;
            }
            *outVar = *var;
            outVar->next = NULL;
         }
      }

      /* Reset the output variable list for the next model set */
      out = outList;
      while(out)
      {  delOut = out;
         out = out->next;
         MyFree((void*)delOut);
      }
      outList = NULL;
   }

   return(OK);
}

#endif


long GetPathAndFile(char *fullName, char *path, char* file)
{
   int  i, j=0;
   char tmp[256];
  
   /* Check if any '/' characters exist */
   if(!strchr(fullName, '/'))
   {  path[0] = '\0';
      strcpy(file, fullName);
      return(0);
   }

   /* Copy the fileName back to the last '/' */ 
   strcpy(path, fullName);
   for(i=strlen(path)-1; i>=0; i--, j++) 
   {  if(path[i] == '/') 
      {  i++;
         break;
      }

      tmp[j] = path[i];
   }
   path[i] = '\0';
   tmp[j] = '\0';

   /* Reverse the file name */
   for(j=0, i=strlen(tmp)-1; i>=0; i--, j++)
      file[i] = tmp[j];
   file[i] = '\0';

   return(0);
}


long SetCalcDones(tModelManage *modelManage)
{
   tModelSet *modSet;
   tModel    *model;
   tModelVar *modelVar;
   tEngVar   *var, *shareVar;
   char      errMsg[1024];
   int       numCalcs=0, idx=0;

   /****************************************************************/
   /* Add up number of calculations that required a calc done flag */
   /****************************************************************/
   for(modSet=modelManage->setList; modSet; modSet=modSet->next)
   {  /* Count all the shared vars */ 
      for(var=modSet->sharedVarList; var; var=var->next)
      {  if(var->user)
         {  modelVar = (tModelVar*)var->user;

            if(var->calcRule) numCalcs++;
            else if(modelVar->args)
               numCalcs++;
         }
      }

      for(model=modSet->modelList; model; model=model->next)
      {  /* Count all the non-shared model vars */ 
         for(var=model->modelRBData->globalVarList; var; var=var->next)
         {  if(var->user)
            {  modelVar = (tModelVar*)var->user;
               if(modelVar->isShared) continue;

               if(var->calcRule) numCalcs++;
               else if(modelVar->args)
                  numCalcs++;
            }
         }
 
         /* Get number of string to numeric calcs in the model */
         numCalcs += GetCountStr2NumCalcs(model->modelRuleHandle); 
      }
   }
   

   /*********************************************/
   /* Allocate a buffer for the calc done flags */
   /*********************************************/
   if(numCalcs)
   {  if((modelManage->calcDones = (char*)MyMalloc(numCalcs))==NULL)
         throw(MALLOC_EXCEPTION);
      memset((char*)modelManage->calcDones, 0, numCalcs);
      modelManage->numCalcDones = numCalcs;
   }
   else
      return(OK);


   /*********************/
   /* Set all the flags */
   /*********************/
   for(modSet=modelManage->setList; modSet; modSet=modSet->next)
   {  /* Set calc dones for all the shared vars */
      for(var=modSet->sharedVarList; var; var=var->next)
      {  if(var->user)
         {  modelVar = (tModelVar*)var->user;

            modelVar->calcDone = modelManage->calcDones + idx;

            if(modelVar->args)
               idx++;
            else if(var->calcRule)
            {  var->calcRule->calcDone = modelVar->calcDone;
               idx++;
            }
         }
      }

      
      for(model=modSet->modelList; model; model=model->next)
      {  /* Set the calc dones for all model variables */
         for(var=model->modelRBData->globalVarList; var; var=var->next)
         {  if(var->user)
            {  modelVar = (tModelVar*)var->user;
               if(modelVar->isShared)
               {  
                  /* Get the shared variable */
                  for(shareVar=modSet->sharedVarList; shareVar; shareVar=shareVar->next)
                     if(!NoCaseStrcmp(shareVar->varName, var->varName))
                        break;

                  /* If variable not found, error out */
                  if(!shareVar)
                  {  sprintf(errMsg, "Failed to find shared variable for variable '%s' "
                                     "in model '%s' while setting calc done flags.",
                                      var->varName, model->modelName);
                     SetConfigErrorMessage(errMsg);
                     throw(CONFIGURATION_EXCEPTION);
                  }

                  /* Set calc done flags from shared variable */
                  modelVar->calcDone = ((tModelVar*)shareVar->user)->calcDone;

                  if(var->calcRule)
                     var->calcRule->calcDone = shareVar->calcRule->calcDone;

                  continue;
               }
               else
                  modelVar->calcDone = modelManage->calcDones + idx;

               if(modelVar->args)
                  idx++;
               else if(var->calcRule)
               {  var->calcRule->calcDone = modelVar->calcDone;
                  idx++;
               }
            }
         }
 
         /* Move the string to numeric calcs from the model */
         MoveStr2NumCalcDones(model->modelRuleHandle, modelManage->calcDones, &idx);
      }
   }
   
/*
   if(idx != modelManage->numCalcDones)
      fprintf(stderr, "CALC DONE MISMATCH: %d to %d\n", idx, modelManage->numCalcDones);
*/

   return(OK);
}


long InitModels(char *configFile, tMasterData *master, void **modelHandle)
{
   char         modSetStr[MODELSETLEN+1], 
                maxSetStr[MAXSETSLEN+1], 
                modStr[MODELNAMELEN+1];
   int          i=0, j=0;
   tModel       *model=NULL;
   tModelSet    *modelSet=NULL;
   tModelManage *modelManage;

   /*****************************************************/
   /* Allocate the model manage structure, if necessary */
   /*****************************************************/
   if((modelManage = (tModelManage*)MyMalloc(sizeof(tModelManage)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)modelManage, 0, sizeof(tModelManage));

   /**************************************************************/
   /* Get the maximum number of model sets that can be in loaded */
   /**************************************************************/
   if(!GetParameter(configFile, MAXMODSETS, maxSetStr, MAXSETSLEN))
      modelManage->maxSets = -1;
   else if(!NoCaseStrcmp(maxSetStr, "all")) modelManage->maxSets = -1;
   else modelManage->maxSets = atoi(maxSetStr);

   /*********************************/
   /* Set the master data structure */
   /*********************************/
   modelManage->masterData = master;

   /*********************************************************/
   /* Load in the functions available for processing models */
   /*********************************************************/
   master->funcInit->modelHandle = (void*)modelManage;
   LoadModelFunctions(master->funcInit, &master->funcList);

   /********************************************************/
   /* Loop through the config reading model sets.  Each    */
   /* model set will have its structures allocated, but    */
   /* only those up to maxSets will be loaded into  memory */     
   /********************************************************/
   while(GetNextConfigItem(configFile,
                     MODELSET, modSetStr, MODELSETLEN))
   {  /**********************************/
      /* Allocate a model set structure */ 
      /*********************************/
      if(modelSet)
      {  if((modelSet->next = (tModelSet*)MyMalloc(sizeof(tModelSet)))==NULL)
            throw(MALLOC_EXCEPTION);
         modelSet = modelSet->next;
      }
      else
      {  if((modelSet = (tModelSet*)MyMalloc(sizeof(tModelSet)))==NULL)
            throw(MALLOC_EXCEPTION);
         modelManage->setList = modelSet;
      }
      memset((char*)modelSet, 0, sizeof(tModelSet));
       
      /***********************************************/
      /* Model file names are a comma separated list */    
      /* Read the name and add it to the modelList   */
      /***********************************************/
      model = NULL;
      i=0;
      while(1)
      {  j=0;
         if(modSetStr[i]=='\0') break;

         while((modSetStr[i] == ',' || 
                isspace(modSetStr[i])) && 
                modSetStr[i]!='\0') i++;
   
         while(modSetStr[i] != ',' && modSetStr[i] != '\0')
            modStr[j++] = modSetStr[i++];
         modStr[j] = '\0';
   
         /****************************/
         /* Allocate model structure */
         /****************************/
         if(model)
         {  if((model->next = (tModel*)MyMalloc(sizeof(tModel)))==NULL)
               throw(MALLOC_EXCEPTION);
            model = model->next;
         }
         else
         {  if((model = (tModel*)MyMalloc(sizeof(tModel)))==NULL)
               throw(MALLOC_EXCEPTION);
            modelSet->modelList = model;;
         }
         memset((char*)model, 0, sizeof(tModel));

         GetPathAndFile(modStr, model->modelPath, model->modelName);
      }

      /************************************************/
      /* Check for maxSets OK, and load the model set */
      /************************************************/
      if(modelManage->maxSets == -1 || 
         modelManage->numLoaded < modelManage->maxSets)
      {
         LoadModelSet(modelManage, modelSet);
         modelManage->numLoaded++;
      }
   }

   /*************************************************************/
   /* Now need to allocate space for and set all calcDone flags */
   /*************************************************************/
   SetCalcDones(modelManage);

#ifdef MODELING
   if(gMakeScoreModels) WriteScoreModels(modelManage);   
   if(gOutDepends) PrintDependencies(modelManage);
#endif

   *modelHandle = (void*)modelManage;
   return(OK);
}


long FreeModelSet(tModelSet *modelSet, tModelManage *modelManage)
{
   tModel  *model, *delModel;
   tEngVar *var, *delVar;

   /***********************************/
   /* Loop through freeing all models */
   /***********************************/
   model = modelSet->modelList; 
   while(model)
   {  delModel = model;
      model = model->next;
      FreeModel(delModel, modelManage->masterData);
   }

   /****************************************************/
   /* Free the shared variable lists, but not the data */
   /****************************************************/
   var = modelSet->sharedVarList;
   while(var)
   {  delVar = var;
      var = var->next;
      MyFree((void*)delVar);
   }
 
   /**********************/
   /* Free the structure */
   /**********************/
   MyFree((void*)modelSet);

   return(OK);
}
 

long FreeModels(void *modelHandle)
{
   tModelSet    *modSet, *delModSet;
   tModelManage *modelManage = (tModelManage*)modelHandle;

   /***************************************/
   /* Loop through freeing all model sets */
   /***************************************/
   modSet = modelManage->setList;
   while(modSet)
   {  delModSet = modSet;
      modSet = modSet->next;
      FreeModelSet(delModSet, modelManage);
   }

   /*****************************/
   /* Free the manage structure */
   /*****************************/
   MyFree((void*)modelManage->calcDones);
   MyFree((void*)modelManage);

   return(OK);
}
