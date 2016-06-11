/*
** $Id: rulproc.c,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
**
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mymall.h"
#include "config.h"
#include "rulproc.h"
#include "rulpars.h"
#include "hncdef.h"
#include "rulfunc.h"
#include "maxerr.h"
#include "except.h"
#include "safopen.h"
#include "model.h"
#include "typechk.h"


extern long FreeRBDataStruct(tRuleBaseData *delRB);


long GetTemplates
(tDataTemplate **templateList, tTmpltMap **templateMaps, char *tmpltFile)
{
   FILE          *tmpltFp;
   long          i, k, status=OK, numTmplts, numFlds;
   char          buf[1024], templateName[hncTEMPLATENAMELEN+1];
   tDataTemplate *tmplt=NULL;

   /* Open the template file */
   if((tmpltFp=SafeFopen(tmpltFile))==NULL)
      return(FATAL_MAX|GET_TMPLTS|CANT_OPEN_FILE);

   /* Read the number of templates */ 
   fgets(buf, 1024, tmpltFp); 
   if(sscanf(buf, "TEMPLATES: %ld\n", &numTmplts)!=1)
      return(FATAL_MAX|GET_TMPLTS|CANT_READ_TEMPLATES);

   /* Read the template maps, if any */
   try
      ReadTemplateMaps(templateMaps, tmpltFp);
   catch(MALLOC_EXCEPTION)
   {  fprintf(stderr, "Out of memory in ReadTemplateMaps\n");
      return(FATAL_MAX|GET_TMPLTS|OUT_OF_MEMORY);
   }
   catch(CONFIGURATION_EXCEPTION)
   {  fprintf(stderr, "%s\n", GetConfigException());
      return(FATAL_MAX|GET_TMPLTS|MODEL_ERR);
   }
   endcatches;

   /* Loop through and read all the templates */
   for(i=0; i<numTmplts; i++)
   {  
      /* Skip blanks lines, if any */
      fgets(buf, 1024, tmpltFp);

      k = 0;
      while(isspace(buf[k]) && buf[k]!='\n') k++;
      if(buf[k] == '\n') 
      {  i--;
         continue;
      }

      /* Allocate a new template structure */ 
      if(tmplt)
      {  if((tmplt->next = (tDataTemplate*)MyMalloc(sizeof(tDataTemplate)))==NULL)  
            return(FATAL_MAX|GET_TMPLTS|OUT_OF_MEMORY);
         tmplt = tmplt->next;
      }
      else
      {  if((tmplt = (tDataTemplate*)MyMalloc(sizeof(tDataTemplate)))==NULL)  
            return(FATAL_MAX|GET_TMPLTS|OUT_OF_MEMORY);
         *templateList = tmplt;
      }
      memset((char*)tmplt, 0, sizeof(tDataTemplate));

      /* Read the template name and number of fields */
      sscanf(buf, "%s . %ld", templateName, &numFlds); 
      if((tmplt->templateName = (char*)MyMalloc(strlen(templateName)+1))==NULL)
         return(FATAL_MAX|GET_TMPLTS|OUT_OF_MEMORY);
      strcpy(tmplt->templateName, templateName);

      /* Read in all the template fields */
      try
      GetTemplateFields(tmpltFp, tmplt, numFlds, tmpltFile, 0);
      catch(MALLOC_EXCEPTION)
      {  fprintf(stderr, "Out of memory in GetTemplateFields\n");
         return(FATAL_MAX|GET_TMPLTS|OUT_OF_MEMORY);
      }
      catch(CONFIGURATION_EXCEPTION)
      {  fprintf(stderr, "%s\n", GetConfigException());
         return(FATAL_MAX|GET_TMPLTS|MODEL_ERR);
      }
      endcatches;
   }

   /* Combine template data according to the map, if any */ 
   try
   CombineTemplates(*templateMaps, *templateList);
   catch(MALLOC_EXCEPTION)
   {  fprintf(stderr, "Out of memory in CombineTemplates\n");
      return(FATAL_MAX|GET_TMPLTS|OUT_OF_MEMORY);
   }
   catch(CONFIGURATION_EXCEPTION)
   {  fprintf(stderr, "%s\n", GetConfigException());
      return(FATAL_MAX|GET_TMPLTS|MODEL_ERR);
   }
   endcatches;

   fclose(tmpltFp);
   return(status);
}


long GetRuleBases(tRuleManage *ruleManage, char *ruleBaseFile)
{
   FILE          *rbFp;
   long          i, status, numRBs, numRules;
   char          buf[1024], rbName[RULEBASENAMELEN+1];
   tRuleBaseData *ruleBase=NULL;

   /* Open the rule base file */
   if((rbFp=SafeFopen(ruleBaseFile))==NULL)
      return(FATAL_MAX|GET_RBS|CANT_OPEN_FILE);

   /* Read the number of rule bases */ 
   fgets(buf, 1024, rbFp); 
   if(sscanf(buf, "RULEBASES: %ld", &numRBs)!=1)
      return(FATAL_MAX|GET_RBS|CANT_READ_RULEBASES);

   /**************************************************************/
   /* Multiple rule bases can be in this file, but it is assumed */
   /* that the first one is the "root" rule base and it calls    */
   /* the others.                                                */
   /**************************************************************/
   /* Allocate the root rule base structure */
   if((ruleManage->ruleBase = (tRuleBase*)MyMalloc(sizeof(tRuleBase)))==NULL)
      return(FATAL_MAX|GET_RBS|OUT_OF_MEMORY);
   memset((char*)ruleManage->ruleBase, 0, sizeof(tRuleBase));

   /* Loop through and read all the rule bases */
   for(i=0; i<numRBs; i++)
   {  /* Allocate a new rule base structure */ 
      if(ruleBase)
      {  if((ruleBase->next = (tRuleBaseData*)MyMalloc(sizeof(tRuleBaseData)))==NULL)  
            return(FATAL_MAX|GET_RBS|OUT_OF_MEMORY);
         ruleBase = ruleBase->next;
      }
      else
      {  if((ruleBase = (tRuleBaseData*)MyMalloc(sizeof(tRuleBaseData)))==NULL)  
            return(FATAL_MAX|GET_RBS|OUT_OF_MEMORY);
         ruleManage->ruleBase->ruleBaseList = ruleBase;
      }
      memset((char*)ruleBase, 0, sizeof(tRuleBaseData));

      /* Read the rule base name and number of rules */
      fgets(buf, 1024, rbFp);
      sscanf(buf, "%s . %ld", rbName, &numRules);
      if((ruleBase->ruleBaseName = (char*)MyMalloc(sizeof(rbName)))==NULL)
         return(FATAL_MAX|GET_RBS|OUT_OF_MEMORY);
      strcpy(ruleBase->ruleBaseName, rbName);

      /* Read the rules */
      try
      {  if((status=ReadRuleBase(rbFp, ruleBase, numRules, ruleBaseFile, 0)))
            return(FATAL_MAX|GET_RBS|(0x00FF&status));
      }
      catch(MALLOC_EXCEPTION)
      {  fprintf(stderr, "Out of memory in CreateRuleBase\n");
         return(FATAL_MAX|GET_RBS|OUT_OF_MEMORY);
      }
      catch(CONFIGURATION_EXCEPTION)
      {  fprintf(stderr, "%s\n", GetConfigException());
         return(FATAL_MAX|GET_RBS|BAD_RULE_PARSE);
      }
      endcatches;

      /* Set the template and func lists */
      ruleBase->templateList = ruleManage->masterData.templateList;
      ruleBase->funcList = ruleManage->masterData.funcList;
   }

   /**********************************************/
   /* Call CreateRuleBase for the rule base list */
   /* It will parse all rule bases in the list   */
   /* and return a handle to the first one       */
   /**********************************************/
   try
      CreateRuleBase(ruleManage->ruleBase->ruleBaseList,
                              &ruleManage->ruleBase->rootHandle);
   catch(MALLOC_EXCEPTION)
   {  fprintf(stderr, "Out of memory in CreateRuleBase\n");
      return(FATAL_MAX|GET_RBS|OUT_OF_MEMORY);
   }
   catch(ENGINE_EXCEPTION)
   {  fprintf(stderr, "%s\n", 
         GetEngineExceptionString(ruleManage->ruleBase->ruleBaseList));
      return((0xF0000000&GetEngineExceptionNumber(ruleManage->ruleBase->ruleBaseList))|MAX|
             GET_RBS|BAD_RULE_PARSE);
   }
   endcatches;

   fclose(rbFp);
   return(status);
}


long RulesInitialize(tRuleManage **pRuleManage, char *configFile)
{
   tRuleManage *ruleManage;
   char        ruleBaseFile[RBFILELEN+1];
   char        tmpltFile[TMPLTFILELEN+1];
   long        status;
   FILE        *configFp;
   tFuncInit   *funcInit;

   /* Make sure the config file exists */
   if((configFp=SafeFopen(configFile))==NULL) 
   {  fprintf(stderr, "File <%s> cannot be read\n", configFile);
      return(FATAL_MAX|RULES_INIT|CANT_OPEN_FILE);
   }
   else
      fclose(configFp);

   /* Allocate the structure */
   *pRuleManage = NULL;
   if((ruleManage = (tRuleManage*)MyMalloc(sizeof(tRuleManage)))==NULL)
      return(FATAL_MAX|RULES_INIT|OUT_OF_MEMORY);
   memset((char*)ruleManage, 0, sizeof(tRuleManage));

   /* Get the template file name */
   if(!GetParameter(configFile, TMPLT_FILE, tmpltFile, TMPLTFILELEN))
      return(FATAL_MAX|RULES_INIT|BAD_TMPLTFILE_PARM);

   /* Read all the templates into the template list */
   if((status=GetTemplates(&ruleManage->masterData.templateList, 
                            &ruleManage->templateMaps, tmpltFile)))
      return(status);

   /* Get all the functions for the rules */
   if((funcInit = (tFuncInit*)MyMalloc(sizeof(tFuncInit)))==NULL) 
   {  fprintf(stderr, "Out of memory for funcInit structure\n");
      return(FATAL_MAX|RULES_INIT|OUT_OF_MEMORY);
   }
   memset((char*)funcInit, 0, sizeof(tFuncInit));
   ruleManage->funcInit = ruleManage->masterData.funcInit = funcInit;
   GetGeneralRuleFuncs(funcInit, &ruleManage->masterData.funcList);

   /* Initialize the models */
   try
      status = InitModels(configFile, &ruleManage->masterData, &ruleManage->modelHandle);
   catch(MALLOC_EXCEPTION)
   {  fprintf(stderr, "Out of memory in InitModels\n");
      return(FATAL_MAX|RULES_INIT|OUT_OF_MEMORY);
   }
   catch(CONFIGURATION_EXCEPTION)
   {  fprintf(stderr, "%s\n", GetConfigException());
      return(FATAL_MAX|RULES_INIT|MODEL_ERR);
   }
   endcatches;

   /* Set the model handle in the tFuncInit structure */
   funcInit->modelHandle = ruleManage->modelHandle;
   
   if(status)
      return(FATAL_MAX|RULES_INIT|(0x00FF&status));

   /* Get the rule base file name */
   if(!GetParameter(configFile, RB_FILE, ruleBaseFile, RBFILELEN))
      return(FATAL_MAX|RULES_INIT|BAD_RBFILE_PARM);

    /* Read the rule base file and create the rule bases */
   if((status=GetRuleBases(ruleManage, ruleBaseFile)))
      return(status);

   *pRuleManage = ruleManage;

#ifdef MPAGE
   /* Free all the temporary init memory */
/*
   FreeTempMemory();
*/
#endif

   return(status);
}


long ProcessData(tRuleManage *ruleManage)
{
   long status;

   /* Execute the main rule base */
   try
      status = ExecuteRuleBase(ruleManage->ruleBase->rootHandle);
   catch(MALLOC_EXCEPTION)
   {  fprintf(stderr, "Out of memory in ExecuteRuleBase\n");
      return(FATAL_MAX|PROC_DATA|OUT_OF_MEMORY);
   }
   catch(ENGINE_EXCEPTION)
   {  
#ifdef DEVELOP
      fprintf(stderr, "%s\n", 
         GetEngineExceptionString(ruleManage->ruleBase->ruleBaseList));
#endif

      /* The function will reset any shared model stuff */
      ResetModels(ruleManage->modelHandle);
      return(GetEngineExceptionNumber(ruleManage->ruleBase->ruleBaseList)|MAX);
   }
   endcatches;

   /* The function will reset any shared model stuff */
   ResetModels(ruleManage->modelHandle);

   if(status) return(status|MAX);
   else return(OK);
}


long FreeRules(tRuleManage *ruleManage)
{
   tRuleBaseData *RB, *delRB;
   tDataTemplate *tmplt, *delTmplt;
   tDataField    *fld, *delFld;
   tFunc         *func, *delFunc;
   tISAMData     *ISAM, *delISAM;
   tTmpltMap     *map, *delMap;
   tTmpltNode    *node, *delNode;
   tSiteValue    *siteVal, *delSite;
   long          status;

#ifdef MPAGE
   /*************************************************/
   /* If MPAGE used, just free all the memory pages */
   /*************************************************/
   FreeAllPages();
   return(0);
#endif

   /***************************/
   /* Free the main rule base */
   /***************************/
   try
      status = FreeRuleBase(ruleManage->ruleBase->rootHandle);
   catch(MALLOC_EXCEPTION)
   {  fprintf(stderr, "Out of memory in FreeRuleBase\n");
      return(FATAL_MAX|FREE_RULES|OUT_OF_MEMORY);
   }
   catch(ENGINE_EXCEPTION)
   {  fprintf(stderr, "%s\n", 
         GetEngineExceptionString(ruleManage->ruleBase->ruleBaseList));
      return(GetEngineExceptionNumber(ruleManage->ruleBase->ruleBaseList));
   }
   endcatches;

   /****************************************/
   /* Free all the ruleBaseData structures */
   /****************************************/
   RB = ruleManage->ruleBase->ruleBaseList;
   while(RB)
   {  delRB = RB;
      RB = RB->next;
      FreeRBDataStruct(delRB);
   }
   MyFree((void*)ruleManage->ruleBase);

   /***********************/
   /* Free all the models */
   /***********************/
   FreeModels(ruleManage->modelHandle);

   /**************************/
   /* Free the template maps */ 
   /**************************/
   map = ruleManage->templateMaps;
   while(map)
   {  node = map->nodeList;

      /* If the template for the node has a dataSize, free the data */
      if(node->tmplt->dataSize)
         MyFree((void*)node->tmplt->data);

      while(node)
      {  delNode = node;
         node->tmplt->data = NULL;
         node = node->next;
         MyFree((void*)delNode);
      }

      delMap = map;
      map = map->next;
      MyFree((void*)delMap);
   }


   /**************************/
   /* Free the template list */
   /**************************/
   tmplt = ruleManage->masterData.templateList;
   while(tmplt)
   {  fld=tmplt->fieldList;
      while(fld)
      {  MyFree((void*)fld->fieldName);

         if(fld->nonFixed.fldID) 
            MyFree((void*)fld->nonFixed.fldID);

         if(fld->subTmpltName) 
            MyFree((void*)fld->subTmpltName);
         
         delFld = fld;
         fld = fld->next;
         MyFree((void*)delFld);
      }
 
      MyFree((void*)tmplt->templateName);
      if(tmplt->data)
         MyFree((void*)tmplt->data);

      delTmplt=tmplt;
      tmplt=tmplt->next;
      MyFree((void*)delTmplt);
   }
 
   /**************************/
   /* Free the function list */
   /**************************/
   func = ruleManage->masterData.funcList;

   while(func)
   {  /* Assume these lists were MyCalloc'ed */
      if(func->paramTypeList)
      {  MyFree((void*)func->paramTypeList->list);
         MyFree((void*)func->paramTypeList);
      }

      delFunc = func;
      func = func->next;

      MyFree((void*)delFunc->functionName);
      MyFree((void*)delFunc);
   }

   /**********************/
   /* Free the ISAM list */
   /**********************/
   ISAM = ruleManage->masterData.ISAMList;
   while(ISAM)
   {  delISAM = ISAM;
      ISAM = ISAM->next;
      MyFree((void*)delISAM);
   }

   /*********************************/
   /* Free the site specific values */
   /*********************************/
   siteVal = ruleManage->masterData.siteValues;
   while(siteVal)
   {  delSite = siteVal;
      siteVal = siteVal->next;
      MyFree((void*)delSite);
   }
 
   /**********************/
   /* Free the structure */
   /**********************/
   MyFree((void*)ruleManage->funcInit);
   MyFree((void*)ruleManage);
 
   return(status);
}


long ResolveData(char *data, char *tmpltName, tRuleManage *ruleManage)
{
   tDataTemplate *tmplt;
   tTmpltMap     *tmpltMap;
   tTmpltNode    *node;
 
   /* Find the template that matches that name */
   for(tmplt=ruleManage->masterData.templateList; tmplt; tmplt=tmplt->next)
      if(!NoCaseStrcmp(tmpltName, tmplt->templateName))
         break;

   /* Error out if template not found */
   if(!tmplt)
      return(TMPLT_NOT_FOUND);

   /**************************************************************/
   /* Find if there is an INPUT MAP that matches this template   */
   /* If so, resolve all linked templates, up to any OR'd values */
   /* for non-fixed templates.                                   */
   /**************************************************************/
   for(tmpltMap=ruleManage->templateMaps; tmpltMap; tmpltMap=tmpltMap->next)
   {  if(!strcmp(tmpltMap->nodeList->name, tmpltName)) 
         break;
   }

   /* First check if have fixed data */
   if(tmpltMap && tmpltMap->nodeList->tmplt->data)
      memcpy(tmpltMap->nodeList->tmplt->data, data, tmpltMap->nodeList->tmplt->dataSize);
   else if(tmpltMap)
   {  /* Have a non-fixed template map.  Resolve each template */
      /* record until hit an optional list.                    */
      for(node=tmpltMap->nodeList; node; node=node->next)
      {  if(node->optionCt)
            break;
         else if((ResolveTemplateRecord(&data, node->tmplt, ruleManage->ruleBase->rootHandle)))
         {  fprintf(stderr, "%s\n", 
               GetEngineExceptionString(ruleManage->ruleBase->ruleBaseList));
            break;
         }
      }

      /* Set the starting address for each of the optional nodes, if any */
      if(node)
      {  if(node->optionCt)
         {  for(; node; node=node->next)
               SetTemplateRecordData(data, node->tmplt, ruleManage->ruleBase->rootHandle);
         }
      }
   }
   else if(tmplt->data)
      memcpy(tmplt->data, data, tmplt->dataSize);
   else
      if((ResolveTemplateRecord(&data, tmplt, ruleManage->ruleBase->rootHandle)))
         fprintf(stderr, "%s\n", GetEngineExceptionString(ruleManage->ruleBase->ruleBaseList));
      
   return(OK);
}
