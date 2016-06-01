/*
** $Id: redcvar.c,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: redcvar.c,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 1.5  1997/02/07  19:45:27  can
 * Make fileName FILENAME_LEN instead of hard 256
 *
 * Revision 1.4  1997/02/05  04:16:30  can
 * Several changes and bug fixes.
 * These changes only affect the printing of a reduced model.
 *
 * Revision 1.3  1997/01/17  18:56:09  can
 * Changed hncRulePase to hncRuleParse (duh!)
 *
 * Revision 1.2  1997/01/17  17:22:36  can
 * Updates for latest release
 *
**
*/

/* System include files */
#include <stdio.h>
#include <ctype.h>
#include <string.h>

/* HNC Rule Engine includes */
#include "mymall.h"
#include "engine.h"
#include "typechk.h"

/* HNC common includes */ 
#include "model.h"
#include "varcalc.h"
#include "except.h"
#include "hncrtyp.h"
#include "config.h"
#include "rulpars.h"

long ReduceModel(tModel *model);

long AddVarToReduceList
(char *varName, tEngVar *varList, tEngVar **usedVars, tModel *model);
long GetAllDependantVars
(char *varName, tEngVar *varList, tEngVar **usedVars, tModel *model);

long GetFuncCalcsFromVars(tEngVar *varList, tModel *model);
long GetRuleCalcsFromVars(tEngVar *varList, tModel *model);


long OutputCONSect(FILE *redFile, tModel *model, tTabSection *sect);
long OutputPRMSect(FILE *redFile, tModel *model, tTabSection *sect);
long OutputPIRSect(FILE *redFile, tModel *model, tTabSection *sect);
long OutputVARSect(FILE *redFile, tModel *model, tTabSection *sect);
long OutputXPLSect(FILE *redFile, tModel *model, tTabSection *sect);
long OutputEXTSect(FILE *redFile, tModel *model, tTabSection *sect);
long OutputRULSect(FILE *redFile, tModel *model, tTabSection *sect);
long OutputTYPSect(FILE *redFile, tModel *model, tTabSection *sect, char *usedTypes);
long OutputTABSect(FILE *redFile, tModel *model, tTabSection *sect, char *usedTypes);
long OutputTabFile(tModel *model);

#define MAX_USED_TYPES  10240
long CheckTypeUsed(char *type, char *usedTypes);
long AddTypeNameToList(char *name, char *usedTypes);
long GetLocalTypes(char *ruleText, char *usedTypes);
long GetUsedTypes(tModel *model, char *usedTypes);
long GetFilePtr(char *fileName, FILE **fp);


long AddVarToReduceList
(char *varName, tEngVar *varList, tEngVar **usedVars, tModel *model)
{
   tEngVar *newVar, *tmpVar, *prevVar;

   /* If there is no varName, just return */
   if(!varName)
      return 0;
   else if(varName[0] == '\0')
      return 0;


   /* See if the variable is already on the list */
   for(tmpVar=prevVar=*usedVars; tmpVar; tmpVar=tmpVar->next) 
   {  if(VarMatch(tmpVar, varName))
         break;
      prevVar=tmpVar;
   }
   if(tmpVar) return 0;

   /* Find the variable in the global list */
   for(tmpVar=varList; tmpVar; tmpVar=tmpVar->next)
      if(VarMatch(tmpVar, varName)) 
         break;
   if(!tmpVar) return 0;


   /* The variable is not on the list, so add it */
   if((newVar = (tEngVar*)MyMalloc(sizeof(tEngVar)))==NULL)
      throw(MALLOC_EXCEPTION);

   *newVar = *tmpVar;
   newVar->next = NULL;

   /* Link in the new variable */
   if(prevVar) prevVar->next = newVar;
   else *usedVars = newVar;

   /* Recursively get all dependant variables */
   GetAllDependantVars(varName, varList, usedVars, model);

   return(0);
}


long GetAllDependantVars
(char *varName, tEngVar *varList, tEngVar **usedVars, tModel *model)
{
   tEngVar     *var;
   tCalcVar    *argVar;
   char        errMsg[256], varNames[4096], name[100];
   char        maxName[100];
   tProfileMap *PRM;
   tModelVar   *modVar;
   int         i, j;

   /* Find the variable */
   for(var=varList; var; var=var->next)
   {  if(VarMatch(var, varName))
         break;
   }

   if(!var)
   {  sprintf(errMsg, "Variable '%s' is not in the variable list for '%s.tab'",
                                                             varName, model->modelName);
      SetConfigErrorMessage(errMsg);
      throw(CONFIGURATION_EXCEPTION);
   }


   /*****************************************************/
   /* If the variable is a function calc var, then make */
   /* sure all args are added to the varList            */
   /*****************************************************/
   if(var->user)
   {  modVar = (tModelVar*)var->user;
      if(modVar->args)
      {  for(argVar=modVar->args; argVar; argVar=argVar->next)
         {  if(argVar->var)
            {  char *field;

               /* if argument is a record field, add the record variable */
               if((field=strchr(argVar->var->varName, '.')))
               {  tDataTemplate *tmplt;
                  char          *varName;

                  varName = argVar->var->varName;
                  field[0] = '\0';

                  /* Only add if it is not a template */
                  for(tmplt=model->modelRBData->templateList; tmplt; tmplt=tmplt->next)
                  {  if(!strcmp(tmplt->templateName, varName))
                        break;
                  }
 
                  if(!tmplt)
                     AddVarToReduceList(varName, varList, usedVars, model);

                  field[0] = '.';
               }

               /* If have a MAXM variable, don't add MAXB and MAXP here */
/*
               else if(!memcmp(modVar->cName, "MAXM", 4) && argVar->var->user) 
               {  if(!memcmp(((tModelVar*)argVar->var->user)->cName, "MAXB", 4)) 
                     continue;
                  if(!memcmp(((tModelVar*)argVar->var->user)->cName, "MAXP", 4)) 
                     continue;

                  AddVarToReduceList(argVar->var->varName, varList, usedVars, model);
               }
*/
               else
                  AddVarToReduceList(argVar->var->varName, varList, usedVars, model);
            }
         }
      }

      /* If have MAXP or MAXB, add the related variables */
      if(!memcmp(modVar->cName, "MAXB", 4))
      {  sprintf(maxName, "MAXM%s", modVar->cName+4);
         AddVarToReduceList(maxName, varList, usedVars, model);
      }
      else if(!memcmp(modVar->cName, "MAXP", 4))
      {  sprintf(maxName, "MAXB%s", modVar->cName+4);
         AddVarToReduceList(maxName, varList, usedVars, model);

         sprintf(maxName, "MAXM%s", modVar->cName+4);
         AddVarToReduceList(maxName, varList, usedVars, model);
      }
   }

   /**********************************************************/
   /* If the variable has a calc rule, scan the rule to find */
   /* all used variables                                     */
   /**********************************************************/
   if(var->calcRule)
   {  GetGlobalVarsInRule(varNames, var->calcRule->code);

      i=0;
      while(varNames[i]!='\0')
      {  j=0;
         while(varNames[i]!=',' && varNames[i]!='\0')
         {  name[j] = varNames[i];
            i++;
            j++;
         }
         name[j] = '\0';

         AddVarToReduceList(name, varList, usedVars, model);

         if(varNames[i] != '\0') i++;
         else break;
      }
   }

   /***********************************************************/
   /* Check to see if the variable is a profile variable.  If */
   /* so, then make sure its init variable is added.          */
   /***********************************************************/
   for(PRM=model->PRMList; PRM; PRM=PRM->next)
      if(!NoCaseStrcmp(PRM->varName, varName))
      {  AddVarToReduceList(PRM->defValue->varName, varList, usedVars, model);
         break;
      }

   return(0);
}


long GetVarsFromRules(tEngVar **usedVars, tModel *model)
{
   tRuleData *rule;
   char      varNames[4096], name[100];
   int       i, j;

   for(rule=model->modelRBData->ruleList; rule; rule=rule->next)
   {  GetGlobalVarsInRule(varNames, rule->ruleText);
     
      i=0;
      while(varNames[i]!='\0')
      {  j=0;
         while(varNames[i]!=',' && varNames[i]!='\0')
         {  name[j] = varNames[i];
            i++; j++;
         }
         name[j] = '\0';

         AddVarToReduceList(name, model->modelRBData->globalVarList, usedVars, model);

         if(varNames[i] != '\0') i++;
         else break;
      }
   }
   for(rule=model->modelRBData->supportRules.sysInitRuleList; rule; rule=rule->next)
   {  GetGlobalVarsInRule(varNames, rule->ruleText);
     
      i=0;
      while(varNames[i]!='\0')
      {  j=0;
         while(varNames[i]!=',' && varNames[i]!='\0')
         {  name[j] = varNames[i];
            i++; j++;
         }
         name[j] = '\0';

         AddVarToReduceList(name, model->modelRBData->globalVarList, usedVars, model);

         if(varNames[i] != '\0') i++;
         else break;
      }
   }
   for(rule=model->modelRBData->supportRules.sysTermRuleList; rule; rule=rule->next)
   {  GetGlobalVarsInRule(varNames, rule->ruleText);
     
      i=0;
      while(varNames[i]!='\0')
      {  j=0;
         while(varNames[i]!=',' && varNames[i]!='\0')
         {  name[j] = varNames[i];
            i++; j++;
         }
         name[j] = '\0';

         AddVarToReduceList(name, model->modelRBData->globalVarList, usedVars, model);

         if(varNames[i] != '\0') i++;
         else break;
      }
   }
   for(rule=model->modelRBData->supportRules.iterInitRuleList; rule; rule=rule->next)
   {  GetGlobalVarsInRule(varNames, rule->ruleText);
     
      i=0;
      while(varNames[i]!='\0')
      {  j=0;
         while(varNames[i]!=',' && varNames[i]!='\0')
         {  name[j] = varNames[i];
            i++; j++;
         }
         name[j] = '\0';

         AddVarToReduceList(name, model->modelRBData->globalVarList, usedVars, model);

         if(varNames[i] != '\0') i++;
         else break;
      }
   }
   for(rule=model->modelRBData->supportRules.iterTermRuleList; rule; rule=rule->next)
   {  GetGlobalVarsInRule(varNames, rule->ruleText);
     
      i=0;
      while(varNames[i]!='\0')
      {  j=0;
         while(varNames[i]!=',' && varNames[i]!='\0')
         {  name[j] = varNames[i];
            i++; j++;
         }
         name[j] = '\0';

         AddVarToReduceList(name, model->modelRBData->globalVarList, usedVars, model);

         if(varNames[i] != '\0') i++;
         else break;
      }
   }


   if(model->modelPInitData)
   {  for(rule=model->modelPInitData->ruleList; rule; rule=rule->next)
      {  GetGlobalVarsInRule(varNames, rule->ruleText);
     
         i=0;
         while(varNames[i]!='\0')
         {  j=0;
            while(varNames[i]!=',' && varNames[i]!='\0')
            {  name[j] = varNames[i];
               i++; j++;
            }
            name[j] = '\0';
   
            AddVarToReduceList(name, model->modelRBData->globalVarList, usedVars, model);
   
            if(varNames[i] != '\0') i++;
            else break;
         }
      }
      for(rule=model->modelPInitData->supportRules.sysInitRuleList; rule; rule=rule->next)
      {  GetGlobalVarsInRule(varNames, rule->ruleText);
        
         i=0;
         while(varNames[i]!='\0')
         {  j=0;
            while(varNames[i]!=',' && varNames[i]!='\0')
            {  name[j] = varNames[i];
               i++; j++;
            }
            name[j] = '\0';
   
            AddVarToReduceList(name, model->modelRBData->globalVarList, usedVars, model);
   
            if(varNames[i] != '\0') i++;
            else break;
         }
      }
      for(rule=model->modelPInitData->supportRules.sysTermRuleList; rule; rule=rule->next)
      {  GetGlobalVarsInRule(varNames, rule->ruleText);
     
         i=0;
         while(varNames[i]!='\0')
         {  j=0;
            while(varNames[i]!=',' && varNames[i]!='\0')
            {  name[j] = varNames[i];
               i++; j++;
            }
            name[j] = '\0';
   
            AddVarToReduceList(name, model->modelRBData->globalVarList, usedVars, model);
   
            if(varNames[i] != '\0') i++;
            else break;
         }
      }
      for(rule=model->modelPInitData->supportRules.iterInitRuleList; rule; rule=rule->next)
      {  GetGlobalVarsInRule(varNames, rule->ruleText);
        
         i=0;
         while(varNames[i]!='\0')
         {  j=0;
            while(varNames[i]!=',' && varNames[i]!='\0')
            {  name[j] = varNames[i];
               i++; j++;
            }
            name[j] = '\0';
   
            AddVarToReduceList(name, model->modelRBData->globalVarList, usedVars, model);
   
            if(varNames[i] != '\0') i++;
            else break;
         }
      }
      for(rule=model->modelPInitData->supportRules.iterTermRuleList; rule; rule=rule->next)
      {  GetGlobalVarsInRule(varNames, rule->ruleText);
     
         i=0;
         while(varNames[i]!='\0')
         {  j=0;
            while(varNames[i]!=',' && varNames[i]!='\0')
            {  name[j] = varNames[i];
               i++; j++;
            }
            name[j] = '\0';
   
            AddVarToReduceList(name, model->modelRBData->globalVarList, usedVars, model);
   
            if(varNames[i] != '\0') i++;
            else break;
         }
      }
   }

   return(0);
}


long BuildCalcList(tCalcVar *calcList, tEngVar *varList, tCalcVar **pNewList)
{
   tEngVar  *var, *tmp;
   tCalcVar *oldCalc, *delCalc, *newCalc=NULL, *newList=NULL;
   tCalcVar *arg;
   int      used;

   oldCalc = calcList;
   while(oldCalc)
   {  used = FALSE;
      for(var=varList; var; var=var->next)
      {  if(var->user && VarMatch(var, oldCalc->var->varName))
         {  /* Add the variable to the new list */ 
            if(newCalc)
            {  newCalc->next = oldCalc;
               newCalc = newCalc->next;
            }
            else
            {  newCalc = oldCalc; 
               newList = newCalc;
            }

            /**************************************************/
            /* This is kind of a nasty kludge to make sure    */
            /* that all arguments get their addresses correct */
            /**************************************************/
            if(((tModelVar*)var->user)->args)
            {  for(arg=((tModelVar*)var->user)->args; arg; arg=arg->next)
               {  for(tmp=varList; tmp; tmp=tmp->next)
                  {  if(VarMatch(tmp, arg->var->varName))
                     {  if(!arg->var->user && !(arg->var->flags&VAR_CONSTANT))
                        {  MyFree((void*)arg->var->varName);
                           MyFree((void*)arg->var);
                        }

                        arg->var = tmp;
                        break;
                     }
                  }
               }
            }

            used = TRUE;
            break;
         }
      }

      if(used)
      {  oldCalc = oldCalc->next;
         newCalc->next = NULL;
         newCalc->var = var;
      }
      else
      {  delCalc = oldCalc;
         oldCalc = oldCalc->next;
         MyFree((void*)delCalc);
      }
   }

   *pNewList = newList;

   return(0);
}


long BuildNoCalcList(tCalcVar *noCalcList, tEngVar *varList, tCalcVar **pNewList)
{
   tEngVar  *var;
   tCalcVar *oldCalc, *delCalc, *newCalc=NULL, *newList=NULL;
   int      used;

   oldCalc = noCalcList;
   while(oldCalc)
   {  used = FALSE;
      for(var=varList; var; var=var->next)
      {  if(VarMatch(var, oldCalc->var->varName))
         {  if(newCalc)
               {  newCalc->next = oldCalc;
               newCalc = newCalc->next;
            }
            else
            {  newCalc = oldCalc; 
               newList = newCalc;
            }

            used = TRUE;
            break;
         }
      }

      if(used)
      {  oldCalc = oldCalc->next;
         newCalc->next = NULL;
         newCalc->var = var;
      }
      else
      {  delCalc = oldCalc;
         oldCalc = oldCalc->next;
         MyFree((void*)delCalc);
      }
   }

   *pNewList = newList;

   return(0);
}


long GetCalcsFromVars(tEngVar *varList, tModel *model)
{
   tVarBlock *varBlock;
   tCalcVar  *calcList;

   /* Reset the main variable calc blocks */
   for(varBlock=model->mainBlockList; varBlock; varBlock=varBlock->next)
   {  BuildCalcList(varBlock->calcList, varList, &calcList);
      varBlock->calcList = calcList;

      BuildNoCalcList(varBlock->noCalcList, varList, &calcList);
      varBlock->noCalcList = calcList;
   }

   /* Reset the sub variable calc blocks */
   for(varBlock=model->subBlockList; varBlock; varBlock=varBlock->next)
   {  BuildCalcList(varBlock->calcList, varList, &calcList);
      varBlock->calcList = calcList;

      BuildNoCalcList(varBlock->noCalcList, varList, &calcList);
      varBlock->noCalcList = calcList;
   }


   return(0);
}


char* TypeToString(int type)
{
   switch(type)
   {  case HNC_INTSTR:
         return("HNC_INTSTR");
      case HNC_FLOATSTR:
         return("HNC_FLOATSTR");
      case HNC_STRING:
         return("STRING");
      case HNC_DATESTR:
         return("HNC_DATESTR");
      case HNC_TIMESTR:
         return("HNC_TIMESTR");
      case HNC_DTSTR:
         return("HNC_DTSTR");
      case HNC_INT16:
         return("HNC_INT16");
      case HNC_UINT16:
         return("HNC_UINT16");
      case HNC_INT32:
         return("HNC_UINT32");
      case HNC_UINT32:
         return("HNC_UINT32");
      case HNC_FLOAT:
         return("HNC_FLOAT");
      case HNC_DOUBLE:
         return("HNC_DOUBLE");
      case HNC_CHAR:
         return("HNC_CHAR");
      case NUMERIC_TYPE:
         return("NUMERIC");
   }

   return("");
}


long OutputRule(tRuleData *rule, FILE *fp)
{
   tRuleArg  *ruleArg;
   int       i;

   fprintf(fp, "&%s", rule->ruleName);  

   /* Output arguments, if any */
   for(ruleArg=rule->argList; ruleArg; ruleArg=ruleArg->next)
   {  if(ruleArg==rule->argList) 
         fprintf(fp, "(%s%s : %s", 
            ruleArg->reference?"VAR " : "", 
            ruleArg->argName, 
            TypeToString(ruleArg->type));
      else
         fprintf(fp, ",%s%s : %s",
            ruleArg->reference?"VAR " : "", 
            ruleArg->argName, 
            TypeToString(ruleArg->type));
   }

   if(rule->argList) fprintf(fp, ")");

   /* Output return type, if any */
   if(rule->returnType)
      fprintf(fp, " : %s", TypeToString(rule->returnType));

   /* Output rule text */
   i = strlen(rule->ruleText)-1;
   while(rule->ruleText[i] == '\n')
   {  rule->ruleText[i] = '\0';
      i--;
   }

   fprintf(fp, "%sRULE\n\n", rule->ruleText);
 
   return(0);
}


long gReduce=0;

long OutputTabVar(tEngVar *var, FILE *fp)
{
   tModelVar *modVar = (tModelVar*)var->user;
   tCalcVar  *arg;
   int       i;

   if(!modVar) return(1);

   /* Output names, type, and shared flag */
   if(var->type == RECORD_TYPE)
   {  fprintf(fp, "%s . %s . %s . %s . %d\n",
          modVar->cfgName, modVar->cName, modVar->sasName, 
          ((tRecInstance*)var->data)->data->record->recName, 
          modVar->isShared?1:0);
   }
   else if(var->type == HNC_STRING)
   {  fprintf(fp, "%s . %s . %s . %d(%d) . %d\n",
          modVar->cfgName, modVar->cName, modVar->sasName, 
          var->type, var->dataSize, modVar->isShared?1:0);
   }
   else
   {  fprintf(fp, "%s . %s . %s . %d . %d\n",
          modVar->cfgName, modVar->cName, modVar->sasName, 
          var->type, modVar->isShared?1:0);
   }

   /* Output explanation data */
   if(modVar->explainGrp != modVar->explainVal)
      fprintf(fp, "%d(%d) . %d\n", 
             modVar->explainVal, modVar->explainGrp, modVar->expBins.numBins);
   else
      fprintf(fp, "%d . %d\n", modVar->explainVal, modVar->expBins.numBins);

   for(i=0; i<modVar->expBins.numBins; i++)
   {  fprintf(fp, "%g %g\n", 
           modVar->expBins.bin[i].range, modVar->expBins.bin[i].expect);
   }

   /* Output calc data, if any*/
   if(var->calcRule)
   {  if(var->calcRule->noAutoCalc)
         fprintf(fp, "%sVAR_NOAUTO\n", var->calcRule->code);
      else
         fprintf(fp, "%sVAR\n", var->calcRule->code);
   }
   else if(modVar->func)
   {  fprintf(fp, "%d", modVar->funcCode);
      for(arg=modVar->args; arg; arg=arg->next)
          fprintf(fp, ", %s", arg->var->varName);
      fprintf(fp, "\n");
   }
   else
      fprintf(fp, "0\n");

   return(0);
}


long OutputRuleData(FILE *redFile, tRuleData *ruleList, long *ct)
{
   tRuleData *rule;

   for(rule=ruleList; rule; rule=rule->next)
   {  OutputRule(rule, redFile); 
      (*ct)++;
   }

   return(0);
}


long OutputTableRecord(FILE *redFile, tTableTemplate *table, char *data)
{
   tRecField *fld;

   for(fld=table->fieldList; fld; fld=fld->next)
   {  if(fld!=table->fieldList) fprintf(redFile, ",");

      switch(fld->type)
      {  case HNC_INTSTR:
         case HNC_FLOATSTR:
         case HNC_STRING:
         case HNC_DATESTR:
         case HNC_TIMESTR:
         case HNC_DTSTR:
         case HNC_CHAR:
            fprintf(redFile, "\"%*.*s\"", 
                   (int)fld->length, (int)fld->length,
                   (char*)(data+fld->offset)); 
            break;
         case HNC_INT16:
            fprintf(redFile, "%d", 
                   *((short*)(data+fld->offset))); 
            break;
         case HNC_UINT16:
            fprintf(redFile, "%u", 
                   *((unsigned short*)(data+fld->offset))); 
            break;
         case HNC_INT32:
            fprintf(redFile, "%ld", 
                   *((long*)(data+fld->offset))); 
            break;
         case HNC_UINT32:
            fprintf(redFile, "%lu", 
                   *((unsigned long*)(data+fld->offset))); 
            break;
         case HNC_FLOAT:
            fprintf(redFile, "%f", 
                   *((float*)(data+fld->offset))); 
            break;
         case HNC_DOUBLE:
            fprintf(redFile, "%f", 
                   *((double*)(data+fld->offset))); 
            break;
         case NUMERIC_TYPE:
            fprintf(redFile, "%g", 
                 NUM_GETFLT((*((tEngNum*)(data+fld->offset))))); 
            break;
         case RECORD_TYPE:
            fprintf(redFile, "%s", 
                 (*((tTableTemplate**)(data+fld->offset)))->tableName);
            break;
      }
   }

   fprintf(redFile, "\n");

   return(0);
}


long OutputTYPSect(FILE *redFile, tModel *model, tTabSection *sect, char *usedTypes)
{
   tSectItem *item;
   tType     *type;
   long      comMark, ct=0, sectionEnd, numSpot;


   /**********************************************/
   /* Print the comments, but mark the beginning */
   /**********************************************/
   comMark = ftell(redFile);
   if(sect->comments[0] != '\0')
      fprintf(redFile, "%s", sect->comments);

   /****************/
   /* Output types */ 
   /****************/
   fprintf(redFile, "TYP . ");
   numSpot = ftell(redFile);
   fprintf(redFile, "     \n");
   for(item=sect->itemList; item; item=item->next)
   {  type = (tType*)item->data;

      /* Skip type if not on usedType list */
      if(!CheckTypeUsed(type->typeName, usedTypes))
         continue;
 
      /* Output RECORD or ARRAY type */
      if(type->type == RECORD_TYPE)
      {  tRecField *fld;

         fprintf(redFile, "RECORD . %s . %ld\n", type->typeName, type->fmt.record.numFlds);
         for(fld=type->fmt.record.fieldList; fld; fld=fld->next)
         {  if(IsString(fld->type))
               fprintf(redFile, "%s . %ld(%ld)\n", fld->fieldName, fld->type, fld->length);
            else if(fld->type == RECORD_TYPE)
               fprintf(redFile, "%s . %s\n", fld->fieldName, fld->typeFmt->record.recName);
            else if(fld->type == ARRAY_TYPE)
               fprintf(redFile, "%s . %s\n", fld->fieldName, fld->typeFmt->array.name);
            else
               fprintf(redFile, "%s . %ld\n", fld->fieldName, fld->type);
         }
      }
      else
      {  if(IsString(type->fmt.array.type))
            fprintf(redFile, "ARRAY . %s . %ld . %ld(%ld)\n",  
                               type->typeName, type->fmt.array.numElems,
                               type->fmt.array.type, type->fmt.array.elemSize);
         else
            fprintf(redFile, "ARRAY . %s . %ld . %ld\n",  
                               type->typeName, type->fmt.array.numElems,
                               type->fmt.array.type);
      }

      ct++;
   }

   /**************************************************************/
   /* If no types were output, move file back to before comments */ 
   /**************************************************************/
   if(ct == 0)
   {  fseek(redFile, comMark, 0);
      return(0);
   }

   /*******************/
   /* Print the count */
   /*******************/
   sectionEnd = ftell(redFile);
   fseek(redFile, numSpot, 0);
   fprintf(redFile, "%-5ld", ct);
   fseek(redFile, sectionEnd, 0);
   fprintf(redFile, "\n");

   return(0);
}


long OutputVARSect(FILE *redFile, tModel *model, tTabSection *sect)
{
   tCalcVar  *var;
   tVarBlock *varBlock = (tVarBlock*)sect->itemList->data;
   long      comMark, ct=0, sectionEnd, numSpot;


   /**********************************************/
   /* Print the comments, but mark the beginning */
   /**********************************************/
   comMark = ftell(redFile);
   if(sect->comments[0] != '\0')
      fprintf(redFile, "%s", sect->comments);

   /********************/
   /* Output variables */
   /********************/
   fprintf(redFile, "VAR . ");
   numSpot = ftell(redFile);
   if(varBlock->name)
      fprintf(redFile, "     . %s\n", varBlock->name);
   else
      fprintf(redFile, "     \n");
 
 
   /* Output all no calc variables */
   for(var=varBlock->noCalcList; var; var=var->next)
   {  OutputTabVar(var->var, redFile);
      ct++;
   }

   /* Output all calc variables */
   for(var=varBlock->calcList; var; var=var->next)
   {  OutputTabVar(var->var, redFile);
      ct++;
   }
 

   /******************************************************************/
   /* If no variables were output, move file back to before comments */ 
   /******************************************************************/
   if(ct == 0)
   {  fseek(redFile, comMark, 0);
      return(0);
   }

   /********************/
   /* Output the count */
   /********************/
   sectionEnd = ftell(redFile);
   fseek(redFile, numSpot, 0);
   fprintf(redFile, "%-5ld", ct);
   fseek(redFile, sectionEnd, 0);
   fprintf(redFile, "\n");

   return(0);
}


long OutputCONSect(FILE *redFile, tModel *model, tTabSection *sect)
{
   tSectItem *item;
   tEngVar   *usedVar, *var;
   long      comMark, ct=0, sectionEnd, numSpot;


   /**********************************************/
   /* Print the comments, but mark the beginning */
   /**********************************************/
   comMark = ftell(redFile);
   if(sect->comments[0] != '\0')
      fprintf(redFile, "%s", sect->comments);

   /********************/
   /* Output constants */ 
   /********************/
   fprintf(redFile, "CON . ");
   numSpot = ftell(redFile);
   fprintf(redFile, "     \n");
   for(item=sect->itemList; item; item=item->next)
   {  var = (tEngVar*)item->data; 

      for(usedVar=model->modelRBData->globalVarList; usedVar; usedVar=usedVar->next)
      {  if(!(usedVar->flags&VAR_CONSTANT)) continue;

         if(!NoCaseStrcmp(var->varName, usedVar->varName))
         {  if(usedVar->type == HNC_STRING)
               fprintf(redFile, "%-35.35s \"%s\"\n", usedVar->varName, (char*)usedVar->data);
            else if(usedVar->type == HNC_FLOAT)
               fprintf(redFile, "%-35.35s %g\n", usedVar->varName, *((float*)usedVar->data));
            else if(usedVar->type == HNC_INT32)
               fprintf(redFile, "%-35.35s 0x%lx\n", usedVar->varName, *((long*)usedVar->data));
            ct++;
         }
      }
   }

   /******************************************************************/
   /* If no constants were output, move file back to before comments */ 
   /******************************************************************/
   if(ct == 0)
   {  fseek(redFile, comMark, 0);
      return(0);
   }

   /********************/
   /* Output the count */
   /********************/
   sectionEnd = ftell(redFile);
   fseek(redFile, numSpot, 0);
   fprintf(redFile, "%-5ld", ct);
   fseek(redFile, sectionEnd, 0);
   fprintf(redFile, "\n");

   return(0);
}


long OutputXPLSect(FILE *redFile, tModel *model, tTabSection *sect)
{
   tSectItem *item;
   tEngVar   *usedVar, *var;
   tModelVar *modVar;
   long      i, comMark, ct=0, sectionEnd, numSpot;


   /**********************************************/
   /* Print the comments, but mark the beginning */
   /**********************************************/
   comMark = ftell(redFile);
   if(sect->comments[0] != '\0')
      fprintf(redFile, "%s", sect->comments);

   /********************/
   /* Output variables */ 
   /********************/
   fprintf(redFile, "XPL . ");
   numSpot = ftell(redFile);
   fprintf(redFile, "     \n");
   for(item=sect->itemList; item; item=item->next)
   {  var = (tEngVar*)item->data; 

      for(usedVar=model->modelRBData->globalVarList; usedVar; usedVar=usedVar->next)
      {  if(var->data == usedVar->data)
         {  /* Output names */
            modVar = (tModelVar*)usedVar->user;
            fprintf(redFile, "%s . %s . %s\n",
                   modVar->cfgName, modVar->cName, modVar->sasName);
 
            /* Output explanation data */
            if(modVar->explainGrp != modVar->explainVal)
               fprintf(redFile, "%d(%d) . %d\n", 
                      modVar->explainVal, modVar->explainGrp, modVar->expBins.numBins);
            else
               fprintf(redFile, "%d . %d\n", modVar->explainVal, modVar->expBins.numBins);

            for(i=0; i<modVar->expBins.numBins; i++)
            {  fprintf(redFile, "%g %g\n",
                    modVar->expBins.bin[i].range, modVar->expBins.bin[i].expect);
            }

            ct++;
         }
      }
   }

   /*****************************************************************/
   /* If no XPL vars were output, move file back to before comments */ 
   /*****************************************************************/
   if(ct == 0)
   {  fseek(redFile, comMark, 0);
      return(0);
   }

   /********************/
   /* Output the count */
   /********************/
   sectionEnd = ftell(redFile);
   fseek(redFile, numSpot, 0);
   fprintf(redFile, "%-5ld", ct);
   fseek(redFile, sectionEnd, 0);
   fprintf(redFile, "\n");

   return(0);
}


long OutputRULSect(FILE *redFile, tModel *model, tTabSection *sect)
{
   tRuleData *rule;
   tSectItem *item;
   long      comMark, ct=0, sectionEnd, numSpot;


   /**********************************************/
   /* Print the comments, but mark the beginning */
   /**********************************************/
   comMark = ftell(redFile);
   if(sect->comments[0] != '\0')
      fprintf(redFile, "%s", sect->comments);

   /****************/
   /* Output rules */
   /****************/
   fprintf(redFile, "RUL . ");
   numSpot = ftell(redFile);
   fprintf(redFile, "     \n");
   for(item=sect->itemList; item; item=item->next)
   {  rule = (tRuleData*)item->data;
      OutputRule(rule, redFile);
      ct++;
   }

   /**************************************************************/
   /* If no rules were output, move file back to before comments */ 
   /**************************************************************/
   if(ct == 0)
   {  fseek(redFile, comMark, 0);
      return(0);
   }

   /********************/
   /* Output the count */
   /********************/
   sectionEnd = ftell(redFile);
   fseek(redFile, numSpot, 0);
   fprintf(redFile, "%-5ld", ct);
   fseek(redFile, sectionEnd, 0);
   fprintf(redFile, "\n");

   return(0);
}
   

long OutputPIRSect(FILE *redFile, tModel *model, tTabSection *sect)
{
   tRuleBaseData *initRB = (tRuleBaseData*)sect->itemList->data;
   long          comMark, ct=0, sectionEnd, numSpot;


   /**********************************************/
   /* Print the comments, but mark the beginning */
   /**********************************************/
   comMark = ftell(redFile);
   if(sect->comments[0] != '\0')
      fprintf(redFile, "%s", sect->comments);

   /****************/
   /* Output rules */
   /****************/
   fprintf(redFile, "PIR . ");
   numSpot = ftell(redFile);
   fprintf(redFile, "     \n");
   OutputRuleData(redFile, initRB->ruleList, &ct);
   OutputRuleData(redFile, initRB->supportRules.sysInitRuleList, &ct);
   OutputRuleData(redFile, initRB->supportRules.sysTermRuleList, &ct);
   OutputRuleData(redFile, initRB->supportRules.iterInitRuleList, &ct);
   OutputRuleData(redFile, initRB->supportRules.iterTermRuleList, &ct);

   /**************************************************************/
   /* If no rules were output, move file back to before comments */ 
   /**************************************************************/
   if(ct == 0)
   {  fseek(redFile, comMark, 0);
      return(0);
   }

   /********************/
   /* Output the count */
   /********************/
   sectionEnd = ftell(redFile);
   fseek(redFile, numSpot, 0);
   fprintf(redFile, "%-5ld", ct);
   fseek(redFile, sectionEnd, 0);
   fprintf(redFile, "\n");

   return(0);
}
   

long OutputPRMSect(FILE *redFile, tModel *model, tTabSection *sect)
{
   tProfileMap *PRM, *PRMList = (tProfileMap*)sect->itemList->data;
   long        comMark, ct=0, sectionEnd, numSpot;


   /**********************************************/
   /* Print the comments, but mark the beginning */
   /**********************************************/
   comMark = ftell(redFile);
   if(sect->comments[0] != '\0')
      fprintf(redFile, "%s", sect->comments);

   /******************/
   /* Output PRM map */
   /******************/
   fprintf(redFile, "PRM . ");
   numSpot = ftell(redFile);
   fprintf(redFile, "     \n");
   for(PRM=PRMList; PRM; PRM=PRM->next)
   {  fprintf(redFile, "%s.%s %s %s\n", 
             PRM->prof, PRM->field, PRM->varName, PRM->defValue->varName);  
      ct++;
   }

   /*************************************************************/
   /* If no maps were output, move file back to before comments */ 
   /*************************************************************/
   if(ct == 0)
   {  fseek(redFile, comMark, 0);
      return(0);
   }

   /********************/
   /* Output the count */
   /********************/
   sectionEnd = ftell(redFile);
   fseek(redFile, numSpot, 0);
   fprintf(redFile, "%-5ld", ct);
   fseek(redFile, sectionEnd, 0);
   fprintf(redFile, "\n");

   return(0);
}


long OutputTABSect(FILE *redFile, tModel *model, tTabSection *sect, char *usedTypes)
{
   tSectItem       *item;
   tTableTemplate  *table;
   tRecField       *fld;
   long            i, comMark, ct=0, sectionEnd, numSpot;


   /**********************************************/
   /* Print the comments, but mark the beginning */
   /**********************************************/
   comMark = ftell(redFile);
   if(sect->comments[0] != '\0')
      fprintf(redFile, "%s", sect->comments);

   /*****************/
   /* Output tables */ 
   /*****************/
   fprintf(redFile, "TBL . ");
   numSpot = ftell(redFile);
   fprintf(redFile, "     \n");
   for(item=sect->itemList; item; item=item->next)
   {  table = (tTableTemplate*)item->data;

      /* If table is not of used type, skip it */
      if(table->typeName)
      {  if(!CheckTypeUsed(table->typeName, usedTypes)) 
            continue;
      }
      else if(!CheckTypeUsed(table->tableName, usedTypes)) 
         continue;
      
      /* Output table name */
      fprintf(redFile, "%s\n", table->tableName);

      /* Output the columns, or the type */
      if(table->typeName)
         fprintf(redFile, "%s\n", table->typeName);
      else
      {  fprintf(redFile, "COL . %d\n", table->fieldCount); 
         for(fld=table->fieldList; fld; fld=fld->next)
         {  fprintf(redFile, "%s\n", fld->fieldName);
            if(fld->type == HNC_STRING)
               fprintf(redFile, "%ld . %ld\n", fld->type, fld->length);
            else
               fprintf(redFile, "%ld\n", fld->type);
         }
      }

      /* Output the default data, if any */
      if(table->defaultData)
      {  fprintf(redFile, "DEFAULT_DATA\n");
         OutputTableRecord(redFile, table, table->defaultData);
      }

      /* Output the data */
      fprintf(redFile, "DAT . %d\n", table->numRows);
      for(i=0; i<table->numRows; i++)
         OutputTableRecord(redFile, table, table->dataList[i]); 

      ct++;
   }


   /***************************************************************/
   /* If no tables were output, move file back to before comments */ 
   /***************************************************************/
   if(ct == 0)
   {  fseek(redFile, comMark, 0);
      return(0);
   }

   /********************/
   /* Output the count */
   /********************/
   sectionEnd = ftell(redFile);
   fseek(redFile, numSpot, 0);
   fprintf(redFile, "%-5ld", ct);
   fseek(redFile, sectionEnd, 0);
   fprintf(redFile, "\n");

   return(0);
}


long OutputEXTSect(FILE *redFile, tModel *model, tTabSection *sect)
{
   char    *ptr = (char*)sect->itemList->data;
   long    comMark, ct=0, sectionEnd, numSpot;


   /**********************************************/
   /* Print the comments, but mark the beginning */
   /**********************************************/
   comMark = ftell(redFile);
   if(sect->comments[0] != '\0')
      fprintf(redFile, "%s", sect->comments);

   /*********************/
   /* Output EXT names */
   /*********************/
   fprintf(redFile, "EXT . ");
   numSpot = ftell(redFile);
   fprintf(redFile, "     \n");
   for(; *ptr != '\0'; ptr++)
   {  if(*ptr == ' ')
         continue;

      if(*ptr == ',')
      {  fprintf(redFile, "\n"); 
         ct++;
      }

      fprintf(redFile, "%c", *ptr);
   }

   /**************************************************************/
   /* If no names were output, move file back to before comments */ 
   /**************************************************************/
   if(ct == 0)
   {  fseek(redFile, comMark, 0);
      return(0);
   }

   /********************/
   /* Output the count */
   /********************/
   sectionEnd = ftell(redFile);
   fseek(redFile, numSpot, 0);
   fprintf(redFile, "%-5ld", ct);
   fseek(redFile, sectionEnd, 0);
   fprintf(redFile, "\n");

   return(0);
}

long OutputTabFile(tModel *model)
{
   tTabSection   *sect;
   FILE          *redFile=NULL;
   char          *namePtr=NULL, fileName[FILENAME_LEN];
   char          usedTypes[MAX_USED_TYPES];

   /* If not gReduce, just return */
   if(!gReduce) 
      return(0);

   /* Get the list of used types */
   GetUsedTypes(model, usedTypes);

   /* Open the first file */
   namePtr = model->sectionList->fileName;
   sprintf(fileName, "%s%s.REDUCE", model->modelPath, namePtr);
   if(GetFilePtr(fileName, &redFile))
      return(-1);

   /* Loop through the sections, outputting the data */
   for(sect=model->sectionList; sect; sect=sect->next)
   { 
      /* Open the file for the section, if necessary */ 
      if(strcmp(namePtr, sect->fileName))
      {  if(redFile) fclose(redFile);
      
         sprintf(fileName, "%s%s.REDUCE", model->modelPath, sect->fileName);
         if(GetFilePtr(fileName, &redFile))
            return(-1);

         namePtr = sect->fileName;
      }

      /* Print the data in the section */
      switch(sect->sectionType)
      {  case CON_SECTION:
            OutputCONSect(redFile, model, sect);
            break;
         case TYP_SECTION:
            OutputTYPSect(redFile, model, sect, usedTypes);
            break;
         case VAR_SECTION:
            OutputVARSect(redFile, model, sect);
            break;
         case XPL_SECTION:
            OutputXPLSect(redFile, model, sect);
            break;
         case RUL_SECTION:
            OutputRULSect(redFile, model, sect);
            break;
         case PRM_SECTION:
            OutputPRMSect(redFile, model, sect);
            break;
         case PIR_SECTION:
            OutputPIRSect(redFile, model, sect);
            break;
         case TAB_SECTION:
            OutputTABSect(redFile, model, sect, usedTypes);
            break;
         case EXT_SECTION:
            OutputEXTSect(redFile, model, sect);
            break;
      }
   }

   return(0);
}


#define NUM_RED_FPS  256
long GetFilePtr(char *fileName, FILE **fp)
{
   long i;

   typedef struct {
      char fileName[FILENAME_LEN];
      FILE *fp;
   } tFileList;
   
   static tFileList *file=NULL;
   static long      fileIdx=0;


   if(file==NULL)
      file = (tFileList*)MyTmpMalloc(NUM_RED_FPS * sizeof(tFileList));
 
   for(i=0; i<fileIdx; i++)
   {  if(!strcmp(fileName, file[i].fileName))
      {  *fp = file[i].fp;
         return(0);
      }
   }

   if(i==NUM_RED_FPS)
   {  fprintf(stderr, "Maximum number of reduce files "
                      "reached (%d)\n", NUM_RED_FPS);
      return(-1);
   }

   strcpy(file[i].fileName, fileName);
   fileIdx = i;
   if((file[i].fp = fopen(fileName, "w"))==NULL)
   {  fprintf(stderr, "File '%s' can't be created\n", fileName);
      return(-1);
   }

   *fp = file[i].fp; 
   return(0);
}


long CheckTypeUsed(char *name, char *usedTypes)
{
   int   i=0, j=0;
   char  tmp[50];
 
   while(1)
   {  j=0;
      while(usedTypes[i]!=',' && usedTypes[i]!='\0')
      {  tmp[j] = usedTypes[i];
         i++; j++;
      }
      tmp[j] = '\0';
 
      if(!NoCaseStrcmp(tmp, name))
         return(TRUE);
 
      if(usedTypes[i]=='\0') break;
      i++;
   }
 
   return(FALSE);
}


long AddTypeNameToList(char *name, char *usedTypes)
{
   int   i=0, j=0;
   char  tmp[50];
 
   while(1)
   {  j=0;
      while(usedTypes[i]!=',' && usedTypes[i]!='\0')
      {  tmp[j] = usedTypes[i];
         i++; j++;
      }
      tmp[j] = '\0';
 
      if(!NoCaseStrcmp(tmp, name))
         return(0);
 
      if(usedTypes[i]=='\0') break;
      i++;
   }
 
   if(i==0)
      sprintf(tmp, "%s", name);
   else
      sprintf(tmp, ",%s", name);
 
   strcat(usedTypes, tmp);
 
   return(0);
}


#include "parstrc.h"
#include "lexrule.h"
long GetLocalTypes(char *ruleText, char *usedTypes)
{
   tParse     p, *parse=&p;
   tRuleData  rd, *tmpRule=&rd;
   long       status;
 
   memset((char*)parse, 0, sizeof(tParse));
 
   parse->ruleText = ruleText;
   parse->curRule = tmpRule;
 
   while((status=LexRule(parse))!=END_OF_RULE)
   {  if(status!=0) continue;
      else if(!strcmp(parse->token, ":"))
      {  /* Next token is the type */
         LexRule(parse);
         if(!strcmp(parse->token, "STRING") || !strcmp(parse->token, "NUMERIC"))
            continue;
         
         /* Add type to usedTypes */
         AddTypeNameToList(parse->token, usedTypes);
      }
      else if(!strcmp(parse->token, "BEGIN"))
         break;
      else
         continue;
   }
 
   return(0);
}


long GetUsedTypes(tModel *model, char *usedTypes)
{
   char      *name;
   tEngVar   *var;
   tRuleData *rule;

   /* Any record or array type for global variables is used */
   for(var=model->modelRBData->globalVarList; var; var=var->next)
   {  if(var->type == RECORD_TYPE || var->type == ARRAY_TYPE)
      {  /* Get type name */
         if(var->type == ARRAY_TYPE)
            name = ((tArrayData*)var->data)->array->name;
         else
            name = ((tRecInstance*)var->data)->data->record->recName;

         AddTypeNameToList(name, usedTypes);
      }    

      if(var->calcRule)
         GetLocalTypes(var->calcRule->code, usedTypes);
   }

   /* Get all local types from main rule base */
   for(rule=model->modelRBData->ruleList; rule; rule=rule->next)
      GetLocalTypes(rule->ruleText, usedTypes);
   for(rule=model->modelRBData->supportRules.sysInitRuleList; rule; rule=rule->next)
      GetLocalTypes(rule->ruleText, usedTypes);
   for(rule=model->modelRBData->supportRules.sysTermRuleList; rule; rule=rule->next)
      GetLocalTypes(rule->ruleText, usedTypes);
   for(rule=model->modelRBData->supportRules.iterInitRuleList; rule; rule=rule->next)
      GetLocalTypes(rule->ruleText, usedTypes);
   for(rule=model->modelRBData->supportRules.iterTermRuleList; rule; rule=rule->next)
      GetLocalTypes(rule->ruleText, usedTypes);

   /* Fix rules in profile init base */
   if(model->modelPInitData)
   {  for(rule=model->modelPInitData->ruleList; rule; rule=rule->next)
         GetLocalTypes(rule->ruleText, usedTypes);
      for(rule=model->modelPInitData->supportRules.sysInitRuleList; rule; rule=rule->next)
         GetLocalTypes(rule->ruleText, usedTypes);
      for(rule=model->modelPInitData->supportRules.sysTermRuleList; rule; rule=rule->next)
         GetLocalTypes(rule->ruleText, usedTypes);
      for(rule=model->modelPInitData->supportRules.iterInitRuleList; rule; rule=rule->next)
         GetLocalTypes(rule->ruleText, usedTypes);
      for(rule=model->modelPInitData->supportRules.iterTermRuleList; rule; rule=rule->next)
         GetLocalTypes(rule->ruleText, usedTypes);
   }

   /* For Falcon, add CNTRY_RISK_DATA */
   AddTypeNameToList("CNTRY_RISK_DATA", usedTypes);

   return(0);
}



#include "parstrc.h"
#include "lexrule.h"
#include "engerr.h"
long FixRuleText(char **ruleText, tEngVar *varList)
{
   tParse     p, *parse=&p;
   tRuleData  rd, *tmpRule=&rd;
   tEngVar    *var;
   long       i, j, status, change=0;
   int        lastIndex=0; 
   char       *ptr, newText[MAX_RULE_LEN], recName[100];

   memset((char*)parse, 0, sizeof(tParse));

   parse->ruleText = *ruleText;
   parse->curRule = tmpRule;

   ptr = newText;
   while((status=LexRule(parse))!=END_OF_RULE)
   {  if(status!=0) 
      {  memcpy(ptr, parse->ruleText+lastIndex, (parse->index-lastIndex));
         ptr += parse->index - lastIndex;
      }
      else if(parse->tokenType == T_VARIABLE)
      {  for(var=varList; var; var=var->next) 
         {  if(VarMatch(var, parse->token) && NoCaseStrcmp(var->varName, parse->token))
            {  /* Copy all white space */
               i=lastIndex; j=0;
               while(isspace(parse->ruleText[i]))
               {  ptr[j] = parse->ruleText[i];
                  i++; j++;
               }
               ptr += j;

               sprintf(ptr, "%s ", var->varName);
               ptr += strlen(ptr);
               change = 1;
               break;
            }
            else if(!NoCaseStrcmp(var->varName, parse->token))
            {  var = NULL;
               break;
            }
         }
 
         if(!var)
         {  memcpy(ptr, parse->ruleText+lastIndex, (parse->index-lastIndex));
            ptr += parse->index - lastIndex;
         }
      }
      else if(parse->tokenType == T_RECFLD)
      {  i=0;
         while(parse->token[i]!='.' && parse->token[i]!='\0')
         {  recName[i] = parse->token[i];
            i++;
         }
         recName[i] = '\0';

         for(var=varList; var; var=var->next) 
         {  if(VarMatch(var, recName) && NoCaseStrcmp(var->varName, recName))
            {  sprintf(ptr, " %s ", var->varName);
               ptr += strlen(ptr);
               change = 1;
               break;
            }
            else if(!NoCaseStrcmp(var->varName, parse->token))
            {  var = NULL;
               break;
            }
         }
 
         if(!var)
         {  memcpy(ptr, parse->ruleText+lastIndex, (parse->index-lastIndex));
            ptr += parse->index - lastIndex;
         }
      }
      else
      {  memcpy(ptr, parse->ruleText+lastIndex, (parse->index-lastIndex));
         ptr += parse->index - lastIndex;
      }

      lastIndex = parse->index;
   }

   *ptr = '\0';
   
   if(change)
   {  if(strlen(newText) > strlen(*ruleText))
      {  MyFree((void*)*ruleText);
         *ruleText = (char*)MyMalloc(strlen(newText));
      }
      strcpy(*ruleText, newText);
   }

   return(0);
}


long FixRuleVars(tModel *model)
{
   tRuleData *rule;
   tEngVar   *var;

   /* Fix rules in main rule base */
   for(var=model->modelRBData->globalVarList; var; var=var->next)
   {  if(var->calcRule)
         FixRuleText(&var->calcRule->code, model->modelRBData->globalVarList);
   }

   for(rule=model->modelRBData->ruleList; rule; rule=rule->next)
      FixRuleText(&rule->ruleText, model->modelRBData->globalVarList);

   for(rule=model->modelRBData->supportRules.sysInitRuleList; rule; rule=rule->next)
      FixRuleText(&rule->ruleText, model->modelRBData->globalVarList);

   for(rule=model->modelRBData->supportRules.sysTermRuleList; rule; rule=rule->next)
      FixRuleText(&rule->ruleText, model->modelRBData->globalVarList);

   for(rule=model->modelRBData->supportRules.iterInitRuleList; rule; rule=rule->next)
      FixRuleText(&rule->ruleText, model->modelRBData->globalVarList);

   for(rule=model->modelRBData->supportRules.iterTermRuleList; rule; rule=rule->next)
      FixRuleText(&rule->ruleText, model->modelRBData->globalVarList);


   /* Fix rules in profile init base */
   if(model->modelPInitData)
   {  for(var=model->modelPInitData->globalVarList; var; var=var->next)
      {  if(var->calcRule)
            FixRuleText(&var->calcRule->code, model->modelRBData->globalVarList);
      }

      for(rule=model->modelPInitData->ruleList; rule; rule=rule->next)
         FixRuleText(&rule->ruleText, model->modelRBData->globalVarList);

      for(rule=model->modelPInitData->supportRules.sysInitRuleList; rule; rule=rule->next)
         FixRuleText(&rule->ruleText, model->modelRBData->globalVarList);

      for(rule=model->modelPInitData->supportRules.sysTermRuleList; rule; rule=rule->next)
         FixRuleText(&rule->ruleText, model->modelRBData->globalVarList);

      for(rule=model->modelPInitData->supportRules.iterInitRuleList; rule; rule=rule->next)
         FixRuleText(&rule->ruleText, model->modelRBData->globalVarList);

      for(rule=model->modelPInitData->supportRules.iterTermRuleList; rule; rule=rule->next)
         FixRuleText(&rule->ruleText, model->modelRBData->globalVarList);
   }

   return(0);
}

 
long WriteScoreModel(tModel *model)
{
   FILE           *fp;
   char           fileName[FILENAME_LEN+1], ruleName[256];
   tRuleData      *rule;
   tEngVar        *var, *inputVar;
   tModelVar      *modVar;
   long           i, j, sectionEnd, numSpot, ct;
   tNetInput      *input;
   tDataTemplate  *tmplt; 
 
   /*********************/
   /* Create a new file */
   /*********************/
   sprintf(fileName, "%s.REDUCED", model->modelName);
   if((fp = fopen(fileName, "w"))==NULL)
   {  fprintf(stderr, "Failed to open file '%s'\n", fileName);
      return(-1);
   }
 

   /******************************************/
   /* Write input variables into XPL section */
   /******************************************/
   fprintf(fp, "XPL . ");
   numSpot = ftell(fp);
   fprintf(fp, "     \n");
   ct=0; 
   for(input=model->inputList; input; input=input->next)
   {  inputVar=NULL;
      for(var=model->modelRBData->globalVarList; var; var=var->next) 
      {  if(var->data == input->data)
         {   inputVar = var;
             break;
         }
      }

      if(!inputVar)
      {  fprintf(stderr, "Bad match from input '%s' to variable\n", input->name);
         return(-1);
      }
      else if(!inputVar->user)
      {  fprintf(stderr, "Variable '%s' has no user section\n", input->name);
         return(-1);
      }

 
      /* Output names */
      modVar = (tModelVar*)inputVar->user;
      fprintf(fp, "%s . %s . %s\n",
             modVar->cfgName, modVar->cName, modVar->sasName);
 
      /* Output explanation data */
      if(modVar->explainGrp != modVar->explainVal)
         fprintf(fp, "%d(%d) . %d\n", 
                modVar->explainVal, modVar->explainGrp, modVar->expBins.numBins);
      else
         fprintf(fp, "%d . %d\n", modVar->explainVal, modVar->expBins.numBins);

      for(i=0; i<modVar->expBins.numBins; i++)
      {  fprintf(fp, "%g %g\n",
              modVar->expBins.bin[i].range, modVar->expBins.bin[i].expect);
      }

      ct++;
   }

   sectionEnd = ftell(fp);
   fseek(fp, numSpot, 0);
   fprintf(fp, "%-5ld", ct);
   fseek(fp, sectionEnd, 0);
   fprintf(fp, "\n");
   

   /********************/
   /* Output templates */
   /********************/
   if(model->modelRBData->templateList)
   {  fprintf(fp, "TPL . ");
      numSpot = ftell(fp);
      fprintf(fp, "     \n");
 
      ct=0;
      for(tmplt=model->modelRBData->templateList; tmplt; tmplt=tmplt->next)
      {  fprintf(fp, "%s . 0\n", tmplt->templateName);
         ct++;
      }
 
      sectionEnd = ftell(fp);
      fseek(fp, numSpot, 0);
      fprintf(fp, "%-5ld", ct);
      fseek(fp, sectionEnd, 0);
      fprintf(fp, "\n");
   }


   /*********************************/
   /* Output the base rules, if any */
   /*********************************/
   if(!model->baseRules)
      return(0);

   i=0;
   ct=0;
   fprintf(fp, "RUL . ");
   numSpot = ftell(fp);
   fprintf(fp, "     \n");
   while(model->baseRules[i]!='\0')
   {  j=0;
      while(model->baseRules[i]!=',' && model->baseRules[i]!='\0')
      {  ruleName[j] = model->baseRules[i];
         i++;
         j++;
      }
      ruleName[j] = '\0';

      for(rule=model->modelRBData->ruleList; rule; rule=rule->next)
      {  if(!NoCaseStrcmp(ruleName, rule->ruleName))
         {  OutputRule(rule, fp);
            ct++;
            break;
         }
      }

      for(rule=model->modelRBData->supportRules.sysInitRuleList; rule; rule=rule->next)
      {  if(!NoCaseStrcmp(ruleName, rule->ruleName))
         {  OutputRule(rule, fp);
            ct++;
            break;
         }
      }

      for(rule=model->modelRBData->supportRules.sysTermRuleList; rule; rule=rule->next)
      {  if(!NoCaseStrcmp(ruleName, rule->ruleName))
         {  OutputRule(rule, fp);
            ct++;
            break;
         }
      }

      for(rule=model->modelRBData->supportRules.iterInitRuleList; rule; rule=rule->next)
      {  if(!NoCaseStrcmp(ruleName, rule->ruleName))
         {  OutputRule(rule, fp);
            ct++;
            break;
         }
      }

      for(rule=model->modelRBData->supportRules.iterTermRuleList; rule; rule=rule->next)
      {  if(!NoCaseStrcmp(ruleName, rule->ruleName))
         {  OutputRule(rule, fp);
            ct++;
            break;
         }
      }

      if(model->baseRules[i] != '\0') i++;
      else break;
   }

   sectionEnd = ftell(fp);
   fseek(fp, numSpot, 0);
   fprintf(fp, "%-5ld", ct);
   fseek(fp, sectionEnd, 0);
   fprintf(fp, "\n");

   return(0);
}
 

long ReduceModel(tModel *model)
{
   tNetInput   *input=NULL;
   tEngVar     *usedVarList=NULL, *prevConst=NULL;
   tEngVar     *usedVar, *globVar, *delVar;
   tCalcVar    *argVar, *delArg;
   tProfileMap *PRM, *newPRM=NULL, *newPRMList=NULL, *delPRM;
   int         usedConst;
   char        errMsg[256];

   /***************************************/
   /* Make sure there is a net input list */
   /***************************************/
   if(model->inputList == NULL)
   {  sprintf(errMsg, "No net input list to check against");
      SetConfigErrorMessage(errMsg);
      throw(CONFIGURATION_EXCEPTION);
   }

   /*************************************************/
   /* Loop through the net inputs to make sure that */
   /* all inputs and their dependants are added to  */
   /* the final variable list.                      */
   /*************************************************/
   for(input=model->inputList; input; input=input->next)
   {  AddVarToReduceList(input->name, 
                           model->modelRBData->globalVarList,
                           &usedVarList,
                           model);
   }

   /************************************************************/
   /* Update the usedVarList with all variables from the rules */
   /* This is assuming that all rules will be included.  If    */
   /* any rules are to excluded, then it is a more complex     */
   /* problem.                                                 */ 
   /************************************************************/
   GetVarsFromRules(&usedVarList, model);

   /**************************************************************/
   /* There are two options when dealing with profile variables. */
   /*                                                            */
   /* 1) Assume that every variable in the PRM list is required, */
   /*    so loop through the list and add all the PRM variables  */
   /*    to the usedVarList.                                     */
   /*                                                            */
   /* 2) Only use those profile variables that are required by   */
   /*    the current usedVarList.  For this, it is assumed that  */
   /*    the PRM list is a master list of all possible variables */
   /*                                                            */
   /**************************************************************/
   if(!model->PRMReduce)
   {  for(PRM=model->PRMList; PRM; PRM=PRM->next)
      {  AddVarToReduceList(PRM->varName, 
                              model->modelRBData->globalVarList,
                              &usedVarList,
                              model);

         AddVarToReduceList(PRM->defValue->varName,
                              model->modelRBData->globalVarList,
                              &usedVarList,
                              model);

         /* Make sure that the init variable is set correctly */
         for(usedVar=usedVarList; usedVar; usedVar=usedVar->next)
            if(VarMatch(usedVar, PRM->defValue->varName))
            {  MyFree((void*)PRM->defValue);
               PRM->defValue = usedVar;
               break;
            }
      }
   }
   else
   {  PRM = model->PRMList;
      while(PRM)
      {  for(usedVar=usedVarList; usedVar; usedVar=usedVar->next)
         {  if(VarMatch(usedVar, PRM->varName))
               break;
         }

         if(usedVar)
         {  if(newPRM) newPRM->next = PRM;
            else newPRMList = PRM;
       
            newPRM = PRM;
            PRM = PRM->next;
            newPRM->next = NULL;
         }
         else
         {  delPRM = PRM;
            PRM = PRM->next;

            MyFree((void*)delPRM->prof);
            MyFree((void*)delPRM->field);
            MyFree((void*)delPRM->varName);
            MyFree((void*)delPRM->defValue);
            MyFree((void*)delPRM);
         }
      }

      model->PRMList = newPRMList;
   }

   /*********************************************/
   /* Update the calcList to only include those */
   /* variables that are in the usedVarList     */
   /*********************************************/
   GetCalcsFromVars(usedVarList, model);


   /**************************************************/
   /* Free all variables that are not used and reset */
   /* the global variable list for the model         */
   /**************************************************/
   globVar=model->modelRBData->globalVarList;
   while(globVar)
   {  for(usedVar=usedVarList; usedVar; usedVar=usedVar->next)
         if(!strcmp(usedVar->varName, globVar->varName))
            break;

      usedConst = 0;
      if(!usedVar)
      {  tModelVar* modVar;

         if(globVar->user)
         {  modVar = (tModelVar*)globVar->user;

            if(!modVar->isShared)
            {  /* Free function arguments */
               argVar = modVar->args;
               while(argVar)
               {  delArg = argVar;
                  if(!argVar->var->user && !(argVar->var->flags&VAR_CONSTANT))
                  {  MyFree((void*)argVar->var->varName);
                     MyFree((void*)argVar->var);
                  }

                  argVar = argVar->next;
                  MyFree((void*)delArg);
               }
   
               /* Free variable names */
               MyFree((void*)modVar->cName);
               MyFree((void*)modVar->cfgName);
               MyFree((void*)modVar->sasName);

               /* Free explanation data */
               if(modVar->expBins.numBins)
                  MyFree((void*)modVar->expBins.bin);

               /* Free structure */
               MyFree((void*)modVar);
   
               /* Free calc rule, if any */
               if(globVar->calcRule)
                  FreeCalcRule(globVar->calcRule);
   
               if(globVar->type == RECORD_TYPE)
                  MyFree((void*)*((tRecInstance*)globVar->data)->data->data);
   
               if(globVar->data && !(globVar->flags&VAR_NOFREE))
               {  MyFree((void*)globVar->data);
                  globVar->data = NULL;
               }
            } 
            else 
            {  /* Free explanation data */
               if(modVar->expBins.numBins)
                  MyFree((void*)modVar->expBins.bin);

               if(modVar->funcCode == DO_NOTHING)
               {  argVar=modVar->args;
                  while(argVar)
                  {  delArg = argVar;
                     argVar=argVar->next;
   
                     MyFree((void*)delArg->var->varName);
                     MyFree((void*)delArg->var);
                     MyFree((void*)delArg);
                  }

                  modVar->args = NULL;
               }
            }
         }
         else 
         {  if(globVar->flags&VAR_CONSTANT)
            {  if(prevConst)
                  prevConst->next = globVar;
               else
                  model->constantList = globVar;

               prevConst = globVar;
               usedConst = 1;
            }
         }
      }

      if(!usedConst)
      {  delVar = globVar;
         globVar = globVar->next;
         MyFree((void*)delVar);   
      }
      else 
      {  globVar = globVar->next;
         prevConst->next = NULL;
      }
   }

   model->modelRBData->globalVarList = usedVarList;

   /***************************************/
   /* Call FixRuleVars to change all rule */
   /* text to use the variable .cfg names */                               
   /***************************************/
   FixRuleVars(model);

   if(gReduce) OutputTabFile(model);   

   return(0);
}
