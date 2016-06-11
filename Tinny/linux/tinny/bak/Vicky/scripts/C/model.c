/*
** $Id: model.c,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: model.c,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.9  1997/02/24  20:31:04  can
 * Took out fclose of config file in LoadNetworks because the file
 * is already closed at that point.
 *
 * Revision 2.8  1997/02/07  19:42:46  can
 * Made sure all files are FILENAME_LEN or MODLEPATHLEN.
 *
 * Revision 2.7  1997/01/21  21:02:50  can
 * Took out '/' in model file name creation.  The modelPath
 * now includes the '/'.  This was done for the mainframe.
 *
 * Revision 2.6  1997/01/21  00:38:01  gak
 * changed #ifdef DECRYPT to #ifdef MODEL_DECRYPT
 *
** Revision 2.5  1997/01/18 03:34:52  can
** Made symbolic BINARY inputs string variable types.
**
 * Revision 2.4  1997/01/17  18:56:09  can
 * Changed hncRulePase to hncRuleParse (duh!)
 *
 * Revision 2.3  1997/01/17  17:22:36  can
 * Updates for latest release
 *
**
*/

/* System includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* HNC Rule Engine includes */
#include "mymall.h"
#include "engine.h"
#include "freect.h"
#include "typechk.h"
#include "error.h"

/* HNC Common includes */
#include "model.h"
#include "modtab.h"
#include "except.h"
#include "hncrtyp.h"
#include "config.h"
#include "safopen.h"
#include "varcalc.h"
#include "rulpars.h"



extern long FreeRBDataStruct(tRuleBaseData *RBData);

long FreeRuleList(tRuleData *ruleList);
long FreeVarList(tEngVar *varList);
long FreeRBDataStruct(tRuleBaseData *RBData);
long ReduceModel(tModel *model);

#ifdef MODEL_DECRYPT
int decrypt=TRUE;
#else
int decrypt=FALSE;
#endif

#ifdef REDUCE
extern long gReduce;
extern long FixRuleVars(tModel *model);
#endif

int VarMatch(tEngVar *var, char *varName)
{
   tModelVar *modVar = (tModelVar*)var->user;

   if(modVar)
   {  if(modVar->cfgName)
         if(!NoCaseStrcmp(modVar->cfgName, varName)) return(1);

      if(modVar->cName)
         if(!NoCaseStrcmp(modVar->cName, varName)) return(1);

      if(modVar->sasName)
         if(!NoCaseStrcmp(modVar->sasName, varName)) return(1);

      return(0);
   }

   return((int)(NoCaseStrcmp(varName, var->varName)==0));
}


#ifdef GETDATA
long SaveModelData(tModel *model)
{
   tISAMData *ISAM; 

   for(ISAM=model->ISAMList; ISAM; ISAM = ISAM->next)
   {  if(ISAM->modelOnly)
         SaveISAMData(ISAM); 
   }

   return(0);
}


long GetModelData(tModel *model)
{
   tISAMData *ISAM; 

   for(ISAM=model->ISAMList; ISAM; ISAM = ISAM->next)
   {  if(*ISAM->retrieved)
      {  ISAM->modelOnly = FALSE;
         continue;
      }
      else
      {  ISAM->modelOnly = TRUE;
         GetISAMData(ISAM); 
      }
   }

   return(0);
}
#endif


void LoadNetByPass(FILE *varFp, tNetInput **NetInputList)
{
   tNetInput  *input=NULL;
   char       buf[256], name[256];
   int        i, j, inComment=0;

   while(GetRcrd(buf, 256, varFp, decrypt))
   {  if(!memcmp(buf, "/*", 2))
      {  inComment++;
         continue;
      }
      else if(!memcmp(buf, "*/", 2))
      {  if(inComment) inComment--;
         continue;
      }
      else if(inComment) continue;

      i=0;
      while(isspace(buf[i])) i++; 
      j=0; 
      while(!isspace(buf[i]) && buf[i]!='\0' && buf[i]!='\n')
      {  name[j] = buf[i];
         i++; j++;
      }
      name[j] = '\0';
      
      if(input)
      {  if((input->next = (tNetInput*)MyMalloc(sizeof(tNetInput)))==NULL) 
            throw(MALLOC_EXCEPTION);
         input = input->next;
      }
      else
      {  if((input = (tNetInput*)MyMalloc(sizeof(tNetInput)))==NULL) 
            throw(MALLOC_EXCEPTION);
         *NetInputList = input;
      }
      memset((char*)input, 0, sizeof(tNetInput));

      /*****************************/
      /* Get the name of the input */
      /*****************************/
      input->name = (char*)MyMalloc(strlen(name)+1);
      strcpy(input->name, name);
   }
}


void LoadNetwork(tModel *model)
{
   tNetInput  *input=NULL;
   tOptiNet   *net;
   long       status, i;
   FILE       *varFp, *cfgFp;
   char       fileName[MODELPATHLEN+1], errMsg[256];

   /*****************************************************/
   /* Look for a .cfg file or a .var file (or neither!) */
   /*****************************************************/
   sprintf(fileName, "%s%s.cfg", model->modelPath, model->modelName);
   if((cfgFp=SafeFopen(fileName)))
   {  
      /* Load the neural network structure */
      sprintf(fileName, "%s%s", model->modelPath, model->modelName);
      if((status=InitOptiNet(fileName, &net)))
      { 
         /* Look for a .var file */
         sprintf(fileName, "%s%s.var", model->modelPath, model->modelName);
         if((varFp=SafeFopen(fileName)))
         {  fprintf(stderr, "\nRunning with the '%s.var' file and "
                            "not a model\n", model->modelName); 

            model->byPassNet = TRUE;
            LoadNetByPass(varFp, &model->inputList);
            fclose(varFp);
            return;
         }
         else
         {  sprintf(errMsg, "Failed to load model '%s' with %lX", fileName, status);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }
      }
      model->pNet = net;
      fclose(cfgFp);
   }
   else 
   {    
      /* Look for a .var file */
      sprintf(fileName, "%s%s.var", model->modelPath, model->modelName);
      if((varFp=SafeFopen(fileName)))
      {  model->byPassNet = TRUE;
         LoadNetByPass(varFp, &model->inputList);
         fclose(varFp);
      }
      else
         fprintf(stderr, "No model or %s.var file specified!\n", model->modelName);
      
      return;
   }
      

   /******************************************************************/
   /* Create a list of data atoms that parallels the net inputs file */
   /* The calculated net input variables will put their evaluated    */
   /* data into this table so it can be used when iterating the net  */
   /******************************************************************/
   /* Loop through cfg nodes */
   for(i=0; i<net->numCfgNodes; i++)
   {  /* Allocate a new input strucure */
      if(input)
      {  if((input->next = (tNetInput*)MyMalloc(sizeof(tNetInput)))==NULL)
            throw(MALLOC_EXCEPTION);
         input = input->next;
      }
      else
      {  if((input = (tNetInput*)MyMalloc(sizeof(tNetInput)))==NULL)
            throw(MALLOC_EXCEPTION);
         model->inputList = input;
      }
      memset((char*)input, 0, sizeof(tNetInput));

      /* Get the name of the input */
      input->name = net->aCfgNodes[i].name;

      /* Get the type and length */
      if(net->aCfgNodes[i].varType == VAR_GROUP ||
         (net->aCfgNodes[i].varType == VAR_BINARY && net->aCfgNodes[i].numSymbols))
      {  input->type = HNC_STRING;
         input->dataSize = net->aCfgNodes[i].length;
      }
      else
      {  input->type = HNC_FLOAT;
         input->dataSize = sizeof(float);
      }
   }
}


long CheckTemplateExists(tDataTemplate *templateList, char *templateName)
{
   tDataTemplate *temp;

   for(temp=templateList; temp; temp=temp->next)
      if(!NoCaseStrcmp(templateName, temp->templateName))
         return(TRUE);

   return(FALSE);
}


void AddTemplateToMaster
(tDataTemplate *tmplt, tDataTemplate **masterList)
{
   tDataTemplate *temp;

   /********************************************/
   /* Check if template already in master list */
   /* If so, then return without adding it.    */
   /********************************************/
   if(CheckTemplateExists(*masterList, tmplt->templateName))
      return;

   /****************************************************/
   /* Add template to end of master list, if it exists */
   /****************************************************/
   if(*masterList)
   {  for(temp=*masterList; temp->next; temp=temp->next);
      temp->next = tmplt;
   }
   else
      *masterList = tmplt;
}


void GetTemplateFromDB
(tDataTemplate **templateList, char *templateName, tMasterData *masterData)
{
   tDataTemplate *temp, *newTmplt=NULL;
   int           inMaster = FALSE;

   /**********************************************/
   /* Check the existing template list for match */
   /**********************************************/
   for(temp=masterData->templateList; temp; temp=temp->next)
   {  if(!NoCaseStrcmp(templateName, temp->templateName))
         break;
   }

   /*******************************************************************/
   /* If there is a match, then allocate a new template structure and */
   /* copy the data from the matched template.                        */
   /*******************************************************************/
   if(temp)
   {  if((newTmplt = (tDataTemplate*)MyMalloc(sizeof(tDataTemplate)))==NULL)
         throw(MALLOC_EXCEPTION);
      *newTmplt = *temp;
      inMaster = TRUE;
   }   
   else
   {  /***********************************************************/
      /* If template doesn't exist yet, get it from the database */
      /***********************************************************/
/*
      GetTemplate(masterData->DbHandle, templateName, &newTmplt);
*/
   }

   /*****************************************/
   /* Add the template to the template list */
   /*****************************************/
   newTmplt->next = NULL;
   if(*templateList)
   {  for(temp=*templateList; temp->next; temp=temp->next);
      temp->next = newTmplt;
   }
   else
      *templateList = newTmplt;

   /***************************************/
   /* Add the template to the master list */
   /***************************************/
   if(!inMaster) AddTemplateToMaster(newTmplt, &masterData->templateList);
}


long InitModelProfiles(tModel *model)
{
   long status=OK;

   /************************************/ 
   /* Set the global model to this one */
   /* and execute the prof init rule   */
   /* base for this model              */
   /************************************/ 
   if(!model->modelPInitHandle)
      return(OK);

   try
      status = ExecuteRuleBase(model->modelPInitHandle);
   catch(MALLOC_EXCEPTION)
   {  fprintf(stderr, "Out of memory in InitProf ExecutreRuleBase for model %s\n", 
                                                                 model->modelName);
      throw(MALLOC_EXCEPTION);
   }
   catch(ENGINE_EXCEPTION)
      return(GetEngineExceptionNumber(model->modelPInitData));
   endcatches;

   return(status);
}


long LoadModel(tModel *model, tEngVar **sharedVars, tMasterData *masterData)
{
   FILE        *fp;
   char        buf[1024], fileName[MODELNAMELEN+1], errMsg[256];
   tNetInput   *netInput;
   tEngVar     *var;
   int         found, loaded=0;


   /****************************************/
   /* Read <modelName>.tab file                */
   /****************************************/
   sprintf(fileName,"%s%s.tab", model->modelPath, model->modelName);
   if((fp = SafeFopen(fileName))==NULL)
   {  sprintf(errMsg, "Failed to open file '%s'",fileName);
      SetConfigErrorMessage(errMsg);
      throw(CONFIGURATION_EXCEPTION);
   }

   /*********************************************************/
   /* Load the neural network structure and data atom table */
   /*********************************************************/
   LoadNetwork(model);

   /*************************************/
   /* Allocate main processing rulebase */
   /*************************************/
   if((model->modelRBData = (tRuleBaseData*)MyMalloc(sizeof(tRuleBaseData)))==NULL)
   {  fprintf(stderr, "\nFailed to allocate modelRBData\n\n");
      throw(MALLOC_EXCEPTION);
   }
   memset((char*)model->modelRBData, 0, sizeof(tRuleBaseData));

   /*********************************************/
   /* Set the master template list in the model */
   /*********************************************/
   model->modelRBData->templateList = masterData->templateList;

   /**************************/
   /* Set the rule base name */
   /**************************/
   sprintf(buf, "%s_CALC_RB", model->modelName);
   if((model->modelRBData->ruleBaseName = (char*)MyMalloc(strlen(buf)+1))==NULL)
      throw(MALLOC_EXCEPTION);
   strcpy(model->modelRBData->ruleBaseName, buf);
  
   /****************************************/
   /* scan .tab file by segments and call  */
   /* appropriate modules for loading info */
   /****************************************/
   ScanFile(fileName, fp, model, &loaded, sharedVars, masterData); 
   fclose(fp);

   /************************************/
   /* If .ext file exists, scan it too */
   /************************************/
   sprintf(fileName, "%s%s.ext", model->modelPath, model->modelName);
   if((fp=SafeFopen(fileName))!=NULL)
   {  ScanFile(fileName, fp, model, &loaded, sharedVars, masterData); 
      fclose(fp);
   }

   /*********************************/
   /* Check table load requirements */
   /*********************************/
   if(loaded&MODEL_PROFMAP && !(loaded&MODEL_PINITRB))
   {  sprintf(errMsg, "Must specify a profile init rule base for model '%s'", 
                                                               model->modelName);
      SetConfigErrorMessage(errMsg);
      throw(CONFIGURATION_EXCEPTION);
   }

   /**************************************************/
   /* Reduce the lists to include only the data that */
   /* are required by the net inputs and profile     */
   /**************************************************/
#ifdef REDUCE
   if(gReduce)
   {  if(ReduceModel(model) != 0)
      {  sprintf(errMsg, "ReduceModel failed for model '%s'", model->modelName);
         SetConfigErrorMessage(errMsg);
         throw(CONFIGURATION_EXCEPTION);
      }
   }
   else
      FixRuleVars(model);
#endif


   
   /************************************************************/
   /* Loop through global variables to find matches in the net */
   /* input list.  If a match, make sure they share the same   */
   /* data address.  If not, skip it.                          */
   /************************************************************/
   for(netInput = model->inputList; netInput; netInput=netInput->next)
   {  found = 0;
      for(var = model->modelRBData->globalVarList; var; var=var->next)
      {  if(VarMatch(var, netInput->name))
         {  netInput->data = var->data;

            if(netInput->type==0)
            {  netInput->type = var->type;
               netInput->dataSize = var->dataSize;
            }
            else if(netInput->type != var->type)
            {  sprintf(errMsg, "Error: input variable '%s' is of type '%d' "
                               "but variable '%s' is of type '%d' for model '%s'",
                               netInput->name, netInput->type, 
                               var->varName, var->type, model->modelName);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }
            else if(netInput->type==HNC_STRING && netInput->dataSize!=var->dataSize)
            {  sprintf(errMsg, "Input variable '%s' is a GROUP varible with "
                               "length '%d' but variable '%s' has a "
                               "length of '%d' for model '%s'",
                               netInput->name, netInput->dataSize, 
                               var->varName, var->dataSize, model->modelName);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }
              
            netInput->modelVar = (tModelVar*)var->user;
            found = 1;
            break;
         }
      }      

      /* Look through the shared variables, if any */
      if(sharedVars)
      {  for(var=*sharedVars; var && !found; var=var->next)
         {  if(VarMatch(var, netInput->name))
            {  netInput->data = var->data;

               if(netInput->type==0)
               {  netInput->type = var->type;
                  netInput->dataSize = var->dataSize;
               }
               else if(netInput->type != var->type)
               {  sprintf(errMsg, "Error: input variable '%s' is of type '%d' "
                                  "but variable '%s' is of type '%d' for model '%s'",
                                  netInput->name, netInput->type, 
                                  var->varName, var->type, model->modelName);
                  SetConfigErrorMessage(errMsg);
                  throw(CONFIGURATION_EXCEPTION);
               }
               else if(netInput->type==HNC_STRING && netInput->dataSize!=var->dataSize)
               {  sprintf(errMsg, "Input variable '%s' is a GROUP varible with "
                                  "length '%d' but variable '%s' has a "
                                  "length of '%d' for model '%s'",
                                  netInput->name, netInput->dataSize, 
                                  var->varName, var->dataSize, model->modelName);
                  SetConfigErrorMessage(errMsg);
                  throw(CONFIGURATION_EXCEPTION);
               }
              
               netInput->modelVar = (tModelVar*)var->user;
               found = 1;
               break;
            }
         } 
      }

      if(!found)
      {  sprintf(errMsg, "Input variable '%s' is not "
                         "found in variable list for model '%s'",
                         netInput->name,
                         model->modelName);
         SetConfigErrorMessage(errMsg);
         throw(CONFIGURATION_EXCEPTION);
      }
   }

   /*************************/
   /* Set the function list */
   /*************************/
   model->modelRBData->funcList = masterData->funcList;

   /*******************************************************************************/
   /* Set the model's table list in the funcInit structure for any SYS_INIT rules */
   /*******************************************************************************/
   masterData->funcInit->dynTableList = (void*)model->tableList;

   /*********************************************************/
   /* Call CreateRuleBase to parse the rules and create the */
   /* run-time handle                                       */ 
   /*********************************************************/
   /* Parse the main rule base */
   try
      CreateRuleBase(model->modelRBData, &model->modelRuleHandle);
   catch(MALLOC_EXCEPTION)
   {  fprintf(stderr, "Out of memory in CreateRuleBase for model %s\n", model->modelName);
      throw(MALLOC_EXCEPTION);
   }
   catch(ENGINE_EXCEPTION)
   {  SetConfigErrorMessage(GetEngineExceptionString(model->modelRBData));
      throw(CONFIGURATION_EXCEPTION);
   }
   endcatches;
   SetNoAutoReset(model->modelRuleHandle);

   /*************************************/
   /* Parse all the variable calc rules */
   /*************************************/
   for(var=model->modelRBData->globalVarList; var; var=var->next)
   {  if(var->calcRule)
      {  if(!var->calcRule->converted)
         {  try
               ParseVarRule(var, model->modelRuleHandle);
            catch(MALLOC_EXCEPTION)
            {  fprintf(stderr, "Out of memory in CreateRuleBase for model %s\n", 
                                                                 model->modelName);
               throw(MALLOC_EXCEPTION);
            }
            catch(ENGINE_EXCEPTION)
            {  SetConfigErrorMessage(GetEngineExceptionString(model->modelRBData));
               throw(CONFIGURATION_EXCEPTION);
            }
            endcatches;
         }
      }
   }


   /******************************************************/
   /* Parse the profile initialization rule base, if any */
   /******************************************************/
   if(model->modelPInitData)
   {  model->modelPInitData->globalVarList = model->modelRBData->globalVarList;
      model->modelPInitData->globalTypeList = model->modelRBData->globalTypeList;

      try
         CreateRuleBase(model->modelPInitData, &model->modelPInitHandle);
      catch(MALLOC_EXCEPTION)
      {  fprintf(stderr, "Out of memory in ProfInit CreateRuleBase "
                         "for model %s\n", model->modelName);
         throw(MALLOC_EXCEPTION);
      }
      catch(ENGINE_EXCEPTION)
      {  SetConfigErrorMessage(GetEngineExceptionString(model->modelPInitData));
         throw(CONFIGURATION_EXCEPTION);
      }
      endcatches;
   }

   return(0);
}


long FreeRuleList(tRuleData *ruleList)
{
   tRuleData *rule, *delRule;
   tRuleArg  *arg, *delArg;

   rule = ruleList;
   while(rule)
   {  MyFree((void*)rule->ruleName); 
      MyFree((void*)rule->ruleText); 
   
      arg = rule->argList;
      while(arg)
      {  /*free((void*)arg->argName);*/
         delArg = arg;
         arg = arg->next;
         MyFree((void*)delArg);
      }

      delRule = rule;
      rule = rule->next;
      MyFree((void*)delRule);
   }   

   return(0);
}


long FreeVarList(tEngVar *varList)
{
   tEngVar   *var, *delVar;
   tCalcVar  *arg, *delArg;
   tModelVar *modVar;

   var = varList;
   while(var)
   {  /* This is for all model defined variables */
      if(var->user)
      {  modVar = (tModelVar*)var->user;
   
         /* If this is an ORIGINAL or NOT_SHARED variable, free the data, */
         /* the calc rule, if any, and the variable names.                */
         if(modVar->isShared == ORIGINAL || modVar->isShared == NOT_SHARED)
         {  if(var->data && !(var->flags&VAR_NOFREE))
            {  if(var->type == RECORD_TYPE) 
               {  FreeRecordData(((tRecInstance*)var->data)->data, TRUE);
                  MyFree((void*)var->data);
               }
               else if(var->type == ARRAY_TYPE) 
                  FreeArrayData(((tArrayData*)var->data), TRUE);
               else
                  MyFree((void*)var->data);
               
               var->data = NULL;
            }

            /* Free calc rule, if any */
            if(var->calcRule)
               FreeCalcRule(var->calcRule);
         
            /* Free names */
            MyFree((void*)modVar->cName);
            MyFree((void*)modVar->cfgName);
            MyFree((void*)modVar->sasName);

            /* Free function arguments */
            arg = modVar->args;
            while(arg)
            {  delArg = arg;
   
#ifndef REDUCE
               if(modVar->funcCode == DO_NOTHING)
               {  MyFree((void*)arg->var->varName);
                  MyFree((void*)arg->var);
               }
#endif

               arg = arg->next;
               MyFree((void*)delArg);
            }

            modVar->args=NULL;
         }

         /* Free explanation data */
         if(modVar->expBins.numBins)
            MyFree((void*)modVar->expBins.bin);

         /* Free model var structure */
         MyFree((void*)modVar);
      }
   
      /* This is for constants and field variables */
      else
      {  if(var->data && !(var->flags&VAR_NOFREE)) 
            MyFree((void*)var->data);
         MyFree((void*)var->varName);
      }

      delVar = var;
      var = var->next;
      MyFree((void*)delVar);
   }

   return(OK);
}


long FreeRBDataStruct(tRuleBaseData *RBData)
{
   tType     *type, *delType;

   /******************/
   /* Free all rules */
   /******************/
   FreeRuleList(RBData->ruleList);
   FreeRuleList(RBData->supportRules.sysInitRuleList);
   FreeRuleList(RBData->supportRules.sysTermRuleList);
   FreeRuleList(RBData->supportRules.iterInitRuleList);
   FreeRuleList(RBData->supportRules.iterTermRuleList);

   /*****************************/
   /* Free global variable list */
   /*****************************/
   FreeVarList(RBData->globalVarList);

   /*************************/
   /* Free global type list */
   /*************************/
   type = RBData->globalTypeList;
   while(type)
   {  delType = type;
      type = type->next; 
      FreeType(delType);
   }

   /***********************/
   /* Free rule base name */
   /***********************/
   MyFree((void*)RBData->ruleBaseName);

   /********************************/
   /* Close the trace file, if any */
   /********************************/
   if(RBData->traceFile) fclose(RBData->traceFile);

   /****************************/
   /* Free the strucure itself */
   /****************************/
   MyFree((void*)RBData);

   return(0);
}


long FreeVarBlockList(tVarBlock *blockList)
{
   tVarBlock  *varBlock, *delVarBlock;
   tCalcVar   *calcVar, *delCalcVar;
   
   varBlock = blockList;
   while(varBlock)
   {  delVarBlock = varBlock;

      calcVar = varBlock->calcList;
      while(calcVar)
      {  delCalcVar = calcVar;
         calcVar = calcVar->next;
         MyFree((void*)delCalcVar);
      }

      calcVar = varBlock->noCalcList;
      while(calcVar)
      {  delCalcVar = calcVar;
         calcVar = calcVar->next;
         MyFree((void*)delCalcVar);
      }

      varBlock = varBlock->next;
      if(delVarBlock->name) MyFree((void*)delVarBlock->name);
      MyFree((void*)delVarBlock);
   }

   return(0);
}
   

long FreeModel(tModel *model, tMasterData *masterData)
{
   tNetInput      *input, *delInput;
   tISAMData      *ISAM, *delISAM;
   tTableTemplate *table, *delTable;
   tDataTemplate  *tmpl, *delTmpl;
   tProfileMap    *PRM, *delPRM;
   tTabSection    *sect, *delSect;
   tSectItem      *item, *delItem;

   /*********************************************/
   /* Free the templates specific to this model */
   /*********************************************/
   tmpl = model->modelRBData->templateList;
   while(tmpl && tmpl != masterData->templateList)
   {  delTmpl = tmpl;
      tmpl = tmpl->next;
      MyFree((void*)delTmpl);
   }

   /***********************************************/
   /* Free the rule engine for the main rule base */
   /***********************************************/
   try
      FreeRuleBase(model->modelRuleHandle);
   catch(MALLOC_EXCEPTION)
   {  fprintf(stderr, "Out of memory in FreeRuleBase for model %s\n", model->modelName);
      throw(MALLOC_EXCEPTION);
   }
   catch(ENGINE_EXCEPTION)
   {  SetConfigErrorMessage(GetEngineExceptionString(model->modelRBData));
      throw(CONFIGURATION_EXCEPTION);
   }
   endcatches;

   /*********************/
   /* Free the sections */
   /*********************/
   sect = model->sectionList;
   while(sect)
   {  delSect = sect;
      sect = sect->next;
      
      item = delSect->itemList;
      while(item)
      {  delItem = item;
         item = item->next;

         if(delSect->sectionType == EXT_SECTION)
            MyFree((void*)delItem->data);
           
         MyFree((void*)delItem);
      }

      MyFree((void*)delSect);
   }

   /***************************/
   /* Free the constants list */
   /***************************/
   FreeVarList(model->constantList);

   /*****************************************/
   /* Free the list of data field variables */
   /*****************************************/
   FreeVarList(model->fieldVars);

   /********************************/
   /* Free the rule base structure */
   /********************************/
   FreeRBDataStruct(model->modelRBData);

   /*********************************/
   /* Free the variable block lists */
   /*********************************/
   FreeVarBlockList(model->mainBlockList);
   FreeVarBlockList(model->subBlockList);

   /*******************************************************/
   /* Free the rule engine for the profile init rule base */
   /*******************************************************/
   if(model->modelPInitHandle)
   {  model->modelPInitData->globalVarList = NULL;
      model->modelPInitData->globalTypeList = NULL;

      try
         FreeRuleBase(model->modelPInitHandle);
      catch(MALLOC_EXCEPTION)
      {  fprintf(stderr, "Out of memory in FreeRuleBase for model %s\n", model->modelName);
         throw(MALLOC_EXCEPTION);
      }
      catch(ENGINE_EXCEPTION)
      {  SetConfigErrorMessage(GetEngineExceptionString(model->modelRBData));
         throw(CONFIGURATION_EXCEPTION);
      }
      endcatches;

      /******************************************/
      /* Free the prof init rule base structure */
      /******************************************/
      FreeRBDataStruct(model->modelPInitData);
   }

   /********************************/
   /* Free the deployNet structure */
   /********************************/
   if(model->pNet) FreeOptiNet(model->pNet);

   /******************/
   /* Free the lists */
   /******************/
   ISAM = model->ISAMList;
   while(ISAM)
   {  delISAM = ISAM;
      ISAM = ISAM->next;
      MyFree((void*)delISAM);
   }

   input = model->inputList;
   while(input)
   {  if(model->byPassNet)
         MyFree((void*)input->name);

      delInput = input;
      input = input->next;
      MyFree((void*)delInput);
   }

   table = model->tableList;
   while(table)
   {  delTable = table;
      table = table->next;
      FreeTable(delTable);
   }

   PRM = model->PRMList;
   while(PRM)
   {  MyFree((void*)PRM->prof);
      MyFree((void*)PRM->field);
      MyFree((void*)PRM->varName);
#ifndef REDUCE
      MyFree((void*)PRM->defValue);
#endif

      delPRM = PRM;
      PRM = PRM->next;
      MyFree((void*)delPRM);
   }

   if(model->baseRules) 
      MyFree((void*)model->baseRules);

   /****************************/
   /* Free the model structure */
   /****************************/
   MyFree((void*)model);

   return(0);
}
