/*
** $Id: maxmisc.c,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "engine.h"
#include "config.h"
#include "except.h"
#include "maxmisc.h"
#include "calctab.h"
#include "mymall.h"
#include "bltins.h"
#include "varcalc.h"
#include "rulfunc.h"
#include "modfunc.h"
#include "dates.h"
#include "dyntabl.h"
#include "typechk.h"
#include "evlarg.h"
#include "prdmfnc.h"
#include "prdrfnc.h"
#include "prdcalc.h"
#include "falcalc.h"
#include "falmfnc.h"


#ifdef ITM
extern long RegisterNetPointers(tOptiNet *net);

long RegisterTraceData(tTrace *trace);
long RegisterRuleCalcs(tRuleCalc *ruleList);
long RegisterStmtBlock(tStmtCalc *stmBlock);
long RegisterArgList(tArg* argList);
long RegisterArgument(tArg* arg);
long RegisterArgCalc(tArg* arg);
long RegisterExprCalc(tExprCalc* exprCalc);
long RegisterVarCalcs(tVarCalc *varCalcList);
long RegisterStr2NumCalcs(tStr2NumCalc *str2numList);
long RegisterISAMList(tISAMData *ISAMList);
long RegisterRulePointers(tRuleManage *ruleMg);
long RegisterMgModelPointers(void *modelHandle);
long RegisterModelPointers(tModel *model);
long RegisterTableList(tTableTemplate *tableList);
long RegisterRuleEngPointers(void *RBHandle);
long RegisterRecUse(tRecUse *recUse);
long RegisterRBDataPointers(tRuleBaseData *RBData);
long RegisterRuleList(tRuleData *ruleList);
long RegisterAllData(tDataPtr *allData);
long RegisterTypePointers(tType *type);
long RegisterRecordPointers(tEngRecord *record);
long RegisterArrayPointers(tEngArray *array);
long RegisterVarPointers(tEngVar *var);
long RegisterRecordData(tRecData *recData);
long RegisterArrayData(tArrayData *arrayData);
long RegisterFuncList(tFunc *funcList);
long RegisterTemplateList(tDataTemplate *templateList);
#endif



/***********************************************************************/
/* rtrim
**    removes spaces from the end of a string
************************************************************************/
char *rtrim(char *In)
{
   int len;

   len = strlen(In);
   len--;
   while (len >= 0
   && (*(In+len) == ' '|| *(In+len) == '\t' || *(In+len) == '\n'))
   {
      *(In+len) = 0;
      len--;
   }
   return (In);
}


/***********************************************************************/
/* ltrim
**    Removes whitespace from the front of a string
************************************************************************/
char *ltrim(char *In)
{
   char *pTemp;

   pTemp = In;
   while(*pTemp != '\0'
   && (*pTemp == ' ' || *pTemp == '\t' || *pTemp == '\n'))
      pTemp++;
   memcpy(In,pTemp,strlen(pTemp)+1);
   return(In);
}


/***********************************************************************/
/* rltrim(char *In)
**    Removes leading and trailing whitespace from a string
************************************************************************/
char *rltrim(char *In)
{
   rtrim(In);
   ltrim(In);
   return (In);
}


long GetTemplate(int dbHanlde, char *templateName, tDataTemplate **template)
{
   char  errMsg[1024];
   /*******************************************************/
   /* JUST AN EMPTY STUB TO MAKE THE CODE COMPILE WITHOUT */
   /* NEEDING TO COMPILE IN hncTemplate.c.  IF IT GETS    */
   /* HERE, THERE IS AN ERROR                             */
   /*******************************************************/
   sprintf(errMsg, "Cannot access database to read template '%s'", templateName);
   SetConfigErrorMessage(errMsg);
   throw(CONFIGURATION_EXCEPTION);
   
   return(0);
}


#ifdef ITM
long RegisterTemplateList(tDataTemplate *templateList)
{
   tDataTemplate *tmplt;
   tDataField    *fld;

   for(tmplt=templateList; tmplt; tmplt=tmplt->next)
   {  LogAddress((void*)&tmplt->templateName);
      LogAddress((void*)&tmplt->data);

      LogAddress((void*)&tmplt->fieldList);
      for(fld=tmplt->fieldList; fld; fld=fld->next)
      {  LogAddress((void*)&fld->fieldName);
         LogAddress((void*)&fld->subTmpltName);
         LogAddress((void*)&fld->nonFixed.fldID);
         LogAddress((void*)&fld->next);
      }

      LogAddress((void*)&tmplt->next);
   }

   return(0); 
}


long RegisterFuncList(tFunc *funcList)
{
   tFunc      *func;
   tTypeList  *type;
   tParamList *param;

   for(func=funcList; func; func=func->next)
   {  LogAddress((void*)&func->functionName);

      LogAddress((void*)&func->paramTypeList);
      for(param=func->paramTypeList; param; param=param->next)
      {  LogAddress((void*)&param->list);
         for(type=param->list; type; type=type->next)
            LogAddress((void*)&type->next);

         LogAddress((void*)&param->next);
      }

      LogAddress((void*)&func->funcData);
      LogAddress((void*)&func->inputData);
      LogAddress((void*)&func->next);
   }

   return(0);
}


long RegisterArrayData(tArrayData *arrayData)
{
   long i;
   LogAddress((void*)&arrayData->array);

   if(arrayData->data)
   {  LogAddress((void*)&arrayData->data);
      for(i=0; i<arrayData->array->numElems; i++) 
      {  LogAddress((void*)&arrayData->data[i]);
         if(arrayData->array->type == RECORD_TYPE)   
            RegisterRecordData((tRecData*)arrayData->data[i]);
      }
   }

   return(0);
}


long RegisterRecordData(tRecData *recData)
{
   long     f;
   tFldData *fld;
   
   LogAddress((void*)&recData->record);
   LogAddress((void*)&recData->data);
   LogAddress((void*)&*recData->data);

   LogAddress((void*)&recData->flds);
   for(f=0; f<recData->record->numFlds; f++)
   {  fld = recData->flds+f;
      LogAddress((void*)&fld->fld);
      if(fld->data)
      {  LogAddress((void*)&fld->data);
         if(fld->fld->type == RECORD_TYPE)
            RegisterRecordData((tRecData*)fld->data);
         else if(fld->fld->type == ARRAY_TYPE) 
            RegisterArrayData((tArrayData*)fld->data);
         else
            LogAddress((void*)&((tFlatData*)fld->data)->data);
      }
   }
   
   return(0);
}


long RegisterVarPointers(tEngVar *var)
{
   /* Register the variable name */
   LogAddress((void*)&var->varName);

   /* Register the calc rule, if any */
   if(var->calcRule)
   {  LogAddress((void*)&var->calcRule);
      LogAddress((void*)&var->calcRule->data);
      LogAddress((void*)&var->calcRule->calcDone);
   }
     
   /* Register the data area */
   LogAddress((void*)&var->data);
   if(var->type == RECORD_TYPE)
   {  tRecInstance *rec = (tRecInstance*)var->data; 
      LogAddress((void*)&rec->data);
      LogAddress((void*)&rec->next);
      RegisterRecordData(rec->data);
   }
   else if(var->type == ARRAY_TYPE)
      RegisterArrayData((tArrayData*)var->data);

   /* Assume user data, if any, is a tModelVar structure */
   if(var->user)
   {  tModelVar *modVar = (tModelVar*)var->user;
      tCalcVar  *arg;

      LogAddress((void*)&var->user);
      LogAddress((void*)&modVar->cfgName);
      LogAddress((void*)&modVar->calcDone);

      LogAddress((void*)&modVar->args);
      for(arg=modVar->args; arg; arg=arg->next)
      {  LogAddress((void*)&arg->var);
         LogAddress((void*)&arg->var->varName);
         LogAddress((void*)&arg->next);
      }

      LogAddress((void*)&modVar->expBins.bin);
   }

   LogAddress((void*)&var->next);

   return(0);
}


long RegisterArrayPointers(tEngArray *array)
{
   LogAddress((void*)&array->name);

   if(array->type == ARRAY_TYPE)
   {  LogAddress((void*)&array->typeFmt);
      LogAddress((void*)&array->typeFmt->array);
   }
   else if(array->type == RECORD_TYPE)
   {  LogAddress((void*)&array->typeFmt);
      LogAddress((void*)&array->typeFmt->record);
   }

   return(0);
}


long RegisterRecordPointers(tEngRecord *record)
{
   tRecField *fld;

   LogAddress((void*)&record->recName);
   LogAddress((void*)&record->fieldList);

   for(fld=record->fieldList; fld; fld=fld->next)
   {  LogAddress((void*)&fld->fieldName);
      if(fld->nonFixed)
      {  LogAddress((void*)&fld->nonFixed->fldID);
         LogAddress((void*)&fld->nonFixed);
      }

      if(fld->type == ARRAY_TYPE)
      {  LogAddress((void*)&fld->typeFmt);
         LogAddress((void*)&fld->typeFmt->array);
      }
      else if(fld->type == RECORD_TYPE)
      {  LogAddress((void*)&fld->typeFmt);
         LogAddress((void*)&fld->typeFmt->record);
      }

      LogAddress((void*)&fld->next);
   }

   if(record->user)
      LogAddress((void*)&record->user);

   return(0);
}


long RegisterTypePointers(tType *type)
{
   LogAddress((void*)&type->typeName);
   if(type->type == RECORD_TYPE)
      RegisterRecordPointers(&type->fmt.record);
   else if(type->type == ARRAY_TYPE)
      RegisterArrayPointers(&type->fmt.array);

   LogAddress((void*)&type->next);

   return(0);
}


long RegisterAllData(tDataPtr *allData)
{
   tDataNode *node;

   LogAddress((void*)&allData->typeList);
   for(node=allData->typeList; node; node=node->next)
   {  LogAddress((void*)&node->val.type);
      RegisterTypePointers(node->val.type);
      LogAddress((void*)&node->next);
   }

   LogAddress((void*)&allData->varList);
   for(node=allData->varList; node; node=node->next)
   {  LogAddress((void*)&node->val.var);
      RegisterVarPointers(node->val.var);
      LogAddress((void*)&node->next);
   }
   
   return(0);
}


long RegisterRuleList(tRuleData *ruleList)
{ 
   tRuleData *rule; 
   tRuleErr  *ruleErr;
   tRuleArg  *ruleArg;

   for(rule=ruleList; rule; rule=rule->next)
   {
      LogAddress((void*)&rule->ruleName);
/*
      LogAddress((void*)&rule->ruleText);
*/
      rule->ruleText = NULL;

      LogAddress((void*)&rule->errList);
      for(ruleErr=rule->errList; ruleErr; ruleErr=ruleErr->next)
      {  LogAddress((void*)&ruleErr->errToken);
         LogAddress((void*)&ruleErr->next);
      }

      LogAddress((void*)&rule->argList);
      for(ruleArg=rule->argList; ruleArg; ruleArg=ruleArg->next)
      {  LogAddress((void*)&ruleArg->argName);
         LogAddress((void*)&ruleArg->next);
      }

      LogAddress((void*)&rule->next);
   }

   return(0);
}


long RegisterRBDataPointers(tRuleBaseData *RBData)
{
   tEngVar *var;
   tType   *type;

   /* Register the name */
   LogAddress((void*)&RBData->ruleBaseName);
   
   /* Register the main rule list */
   LogAddress((void*)&RBData->ruleList);
   RegisterRuleList(RBData->ruleList);

   /* Register the template list */
   LogAddress((void*)&RBData->templateList);
   RegisterTemplateList(RBData->templateList);
   
   /* Register the function list */
   LogAddress((void*)&RBData->funcList);
   RegisterFuncList(RBData->funcList);
   
   /* Register the function list */
   LogAddress((void*)&RBData->funcList);
   RegisterFuncList(RBData->funcList);
   
   /* Register the variable list */
   LogAddress((void*)&RBData->globalVarList);
   for(var=RBData->globalVarList; var; var=var->next)
      RegisterVarPointers(var);
      
   /* Register the type list */
   LogAddress((void*)&RBData->globalTypeList);
   for(type=RBData->globalTypeList; type; type=type->next)
      RegisterTypePointers(type);
   
   /* Register support rules */
   LogAddress((void*)&RBData->supportRules.sysInitRuleList);
   RegisterRuleList(RBData->supportRules.sysInitRuleList);
   LogAddress((void*)&RBData->supportRules.sysTermRuleList);
   RegisterRuleList(RBData->supportRules.sysTermRuleList);
   LogAddress((void*)&RBData->supportRules.iterInitRuleList);
   RegisterRuleList(RBData->supportRules.iterInitRuleList);
   LogAddress((void*)&RBData->supportRules.iterTermRuleList);
   RegisterRuleList(RBData->supportRules.iterTermRuleList);
   
   /* Register remaining crap */
   LogAddress((void*)&RBData->status.errStr);
   LogAddress((void*)&RBData->traceFile);
   LogAddress((void*)&RBData->next);
   
   return(0);
}


long RegisterRecUse(tRecUse *recUse)
{
   tArrayProc *array;

   LogAddress((void*)&recUse->fldData);
   if(recUse->fldData->flags&FIELD_DUMMY)
      LogAddress((void*)&recUse->fldData->fld);

   LogAddress((void*)&recUse->fieldName);
   LogAddress((void*)&recUse->recordName);

   LogAddress((void*)&recUse->arrayProc);
   for(array = recUse->arrayProc; array; array=array->next)
   {  if(array->arrayUse)
      {  LogAddress((void*)&array->arrayUse);
         LogAddress((void*)&array->arrayUse->indexExpr);
         RegisterArgument((tArg*)array->arrayUse->indexExpr);

         LogAddress((void*)&array->arrayUse->arrayData);
      }

      LogAddress((void*)&array->next);
   }

   return(0);
}


long RegisterExprCalc(tExprCalc *exprList)
{
   tExprCalc *exprCalc;

   for(exprCalc=exprList; exprCalc; exprCalc=exprCalc->next)
   {  /* Register left and right arguments, if any */ 
      if(exprCalc->flags&LEFT_ARG)
      {  LogAddress((void*)&exprCalc->left);
         RegisterArgument((tArg*)exprCalc->left);
      }
      if(exprCalc->flags&RIGHT_ARG)
      {  LogAddress((void*)&exprCalc->right);
         RegisterArgument((tArg*)exprCalc->right);
      }
       
      LogAddress((void*)&exprCalc->parent);
      LogAddress((void*)&exprCalc->next);
   }

   return(0);
}


long RegisterArgCalc(tArg *arg)
{
   /* Note that string to numeric and var calcs are not registered here */
   if(arg->flags&ATOM_ARG_CALC)
   {  tArgCalc *argCalc = (tArgCalc*)arg->atom.data.uVal;

      LogAddress((void*)&argCalc->argList);
      RegisterArgList(argCalc->argList);
   }
   else if(arg->flags&ATOM_FUNC_CALC)
   {  tFuncCalc *funcCalc = (tFuncCalc*)arg->atom.data.uVal;

      LogAddress((void*)&funcCalc->argList);
      RegisterArgList(funcCalc->argList);

      LogAddress((void*)&funcCalc->funcData);

/* Address may be bogus.  Not usually needed anyway 
      LogAddress((void*)&funcCalc->ruleData);
*/
   }

   return(0);
}


long RegisterArgument(tArg *arg)
{
   /* Get the type of calc node the argument is, if any, and act accordingly */ 
   if(arg->flags&ATOM_STMT_CALC)
   {  LogAddress((void*)&arg->atom.data.uVal);
      RegisterStmtBlock((tStmtCalc*)arg->atom.data.uVal);
   }
   else if(arg->flags&ATOM_EXPR_CALC)
   {  LogAddress((void*)&arg->atom.data.uVal);
      RegisterExprCalc((tExprCalc*)arg->atom.data.uVal);
   }
   else if(arg->flags >= ATOM_ARG_CALC)
   {  LogAddress((void*)&arg->atom.data.uVal);
      RegisterArgCalc(arg);
   }
   else
   {  if(arg->flags&ATOM_ARRAYPROC)
      {  LogAddress((void*)&arg->atom.data.uVal);
         RegisterRecUse((tRecUse*)arg->atom.data.uVal); 
      }
      if(arg->flags&ATOM_ADDRESS || 
            arg->flags&ATOM_STRING || 
            arg->flags&ATOM_PARAMETER || 
            arg->flags&ATOM_REFERENCE)
         LogAddress((void*)&arg->atom.data.uVal);
   }

   if(arg->func)
   {  LogAddress((void*)&arg->func);
      LogAddress((void*)&arg->func->funcName);
   }

   return(0);
}


long RegisterArgList(tArg *argList)
{
   tArg *arg;

   for(arg=argList; arg; arg=arg->next)
   {  RegisterArgument(arg);
      LogAddress((void*)&arg->next);
   }

   return(0);
}


long RegisterTraceData(tTrace *trace)
{
   tStmtText *txt;

   if(trace)
   {  for(txt=trace->textList; txt; txt=txt->next)
      {  LogAddress((void*)&txt->text);
         LogAddress((void*)&txt->next);
      }
   }
 
   return(0);
}


long RegisterStmtBlock(tStmtCalc *stmtBlock)
{
   tArg  *stmtArg = stmtBlock->argList;

   LogAddress((void*)&stmtBlock->argList);
   for(; stmtArg; stmtArg=stmtArg->next)
   {  RegisterArgument(stmtArg);
      LogAddress((void*)&stmtArg->next);
   }

   return(0);
}


long RegisterStr2NumCalcs(tStr2NumCalc *str2numList)
{
   tStr2NumCalc *s2nCalc;

   for(s2nCalc=str2numList; s2nCalc; s2nCalc=s2nCalc->next)
   {  LogAddress((void*)&s2nCalc->id);
      LogAddress((void*)&s2nCalc->calcDone);
      LogAddress((void*)&s2nCalc->next);

      RegisterArgument(&s2nCalc->arg);
   }

   return(0);
}


long RegisterVarCalcs(tVarCalc *varCalcList)
{
   tVarCalc *varCalc;

   for(varCalc=varCalcList; varCalc; varCalc=varCalc->next)
   {  LogAddress((void*)&varCalc->id);
      LogAddress((void*)&varCalc->var);

      LogAddress((void*)&varCalc->arg);
      RegisterArgument(varCalc->arg);

      LogAddress((void*)&varCalc->next);
   }

   return(0);
}


long RegisterRuleCalcs(tRuleCalc *ruleList)
{
   tRuleCalc  *ruleCalc; 
   tRuleParam *param;

   for(ruleCalc=ruleList; ruleCalc; ruleCalc=ruleCalc->next)
   {  LogAddress((void*)&ruleCalc->paramList);
      for(param=ruleCalc->paramList; param; param=param->next)
      {  LogAddress((void*)&param->param);
         LogAddress((void*)&param->next);
      }


      LogAddress((void*)&ruleCalc->stmtBlock);
      RegisterStmtBlock(ruleCalc->stmtBlock);

      LogAddress((void*)&ruleCalc->id);
      LogAddress((void*)&ruleCalc->next);
   }

   return(0);
}


long RegisterRuleEngPointers(void *RBHandle)
{
   tCalcTable   *calcTab, *start = (tCalcTable*)RBHandle;
   tPoolPage    *page;
   tUsedFunc    *usedFunc;
   tRecInstance *tmpltRec;

   for(calcTab=start; calcTab; calcTab=calcTab->next)
   {  /*****************************************/
      /* Register all the rule calc node lists */
      /*****************************************/
      LogAddress((void*)&calcTab->sysInitList);
      RegisterRuleCalcs(calcTab->sysInitList);

      LogAddress((void*)&calcTab->sysTermList);
      RegisterRuleCalcs(calcTab->sysTermList);

      LogAddress((void*)&calcTab->iterInitList);
      RegisterRuleCalcs(calcTab->iterInitList);

      LogAddress((void*)&calcTab->iterTermList);
      RegisterRuleCalcs(calcTab->iterTermList);

      LogAddress((void*)&calcTab->ruleCalcList);
      RegisterRuleCalcs(calcTab->ruleCalcList);
   
      LogAddress((void*)&calcTab->paramRuleList);
      RegisterRuleCalcs(calcTab->paramRuleList);
   
      LogAddress((void*)&calcTab->str2numList);
      RegisterStr2NumCalcs(calcTab->str2numList);

      LogAddress((void*)&calcTab->varCalcList);
      RegisterVarCalcs(calcTab->varCalcList);
   

      /*************************************************/
      /* Register the tRuleBaseData structure pointers */
      /*************************************************/
      LogAddress((void*)&calcTab->ruleBaseData);
      RegisterRBDataPointers(calcTab->ruleBaseData);

      /******************************/
      /* Register the allData lists */
      /******************************/
      LogAddress((void*)&calcTab->allData);
      if(calcTab==start) 
         RegisterAllData(calcTab->allData);

      /****************************************/
      /* Register the dynamic data structures */
      /****************************************/
      if(calcTab->dynPool)
      {  LogAddress((void*)&calcTab->dynPool);
         if(calcTab==start) 
         {  LogAddress((void*)&calcTab->dynPool->curPage);
            LogAddress((void*)&calcTab->dynPool->pageList);
            LogAddress((void*)&calcTab->dynPool->mainCalcTab);

            for(page=calcTab->dynPool->pageList; page; page=page->next)
            {  LogAddress((void*)&page->data);
               LogAddress((void*)&page->next);
            }
         }
      }

      /**********************************************/
      /* Register the template record instance list */
      /**********************************************/
      LogAddress((void*)&calcTab->tmpltRecList);
      for(tmpltRec=calcTab->tmpltRecList; tmpltRec; tmpltRec=tmpltRec->next)
      {  LogAddress((void*)&calcTab->tmpltRecList->data);
         LogAddress((void*)&calcTab->tmpltRecList->next);
      }

      /*******************************/
      /* Register used function list */
      /*******************************/
      LogAddress((void*)&calcTab->usedFuncList);
      for(usedFunc=calcTab->usedFuncList; usedFunc; usedFunc=usedFunc->next)
      {  LogAddress((void*)&usedFunc->func);
         LogAddress((void*)&usedFunc->next);
      }

      /***********************************/
      /* Register the argument functions */
      /***********************************/
      LogAddress((void*)&calcTab->argFuncArray);
      if(calcTab==start)
      {  int a;
         for(a=0; a<calcTab->numArgFuncs; a++)
            LogAddress((void*)&calcTab->argFuncArray[a].funcName);
      }

      /************************/
      /* Register other stuff */
      /************************/
      LogAddress((void*)&calcTab->calcDones);
      LogAddress((void*)&calcTab->user);
      LogAddress((void*)&calcTab->memPool);
      LogAddress((void*)&calcTab->next);
   }

   return(0);
}


long RegisterISAMList(tISAMData *ISAMList)
{
   /* This isn't currently being used */
   return(0);   
}


long RegisterTableList(tTableTemplate *tableList)
{
   tTableTemplate *table;
   tRecField      *fld;
   long           i, subRecOffset=0;
   
   for(table=tableList; table; table=table->next)
   {  LogAddress((void*)&table->tableName);

      LogAddress((void*)&table->fieldList);
      for(fld=table->fieldList; fld; fld=fld->next)
      {  /* Set flag if there is a sub-record or not */ 
         if(fld->type == RECORD_TYPE)
            subRecOffset = fld->offset;
         else
            subRecOffset = 0;
      }

      LogAddress((void*)&table->dataList);
      for(i=0; i<table->numRows; i++)
      {  LogAddress((void*)&table->dataList[i]);

         /* If there is a sub record, must log the table pointer */
         /* which is the last bytes of the data buffer           */ 
         if(subRecOffset)
            LogAddress((void*)(table->dataList[i] + subRecOffset));
      }

      LogAddress((void*)&table->defaultData);
      LogAddress((void*)&table->next);
   }

   return(0);
}


long RegisterNetPointers(tOptiNet *net)
{
   int i, j;

   /* Register the cfg nodes */
   LogAddress((void*)&net->aCfgNodes);
   for(i=0; i<net->numCfgNodes; i++)
   {   LogAddress((void*)&net->aCfgNodes[i].name);
       if(net->aCfgNodes[i].numSymbols)
       {  LogAddress((void*)&net->aCfgNodes[i].aSymbols);
          for(j=0; j<net->aCfgNodes[i].numSymbols; j++)
             LogAddress((void*)&net->aCfgNodes[i].aSymbols[j]);
       }
   }

   /* Register the input data structure */
   LogAddress((void*)&net->aInputData);

   /* Register the scaling data structure */
   LogAddress((void*)&net->aScaled);

   /* Register the hidden structure */
   LogAddress((void*)&net->aHidden);
   for(i=0; i<net->numHidden; i++)
      LogAddress((void*)&net->aHidden[i].aWeights);

   return(0);
}


long RegisterVarBlock(tVarBlock *blockList)
{
   tVarBlock *block;
   tCalcVar  *var;

   for(block=blockList; block; block=block->next)
   {  LogAddress((void*)&block->name);

      LogAddress((void*)&block->calcList);
      for(var=block->calcList; var; var=var->next)
      {  LogAddress((void*)&var->var);
         LogAddress((void*)&var->next);
      }

      LogAddress((void*)&block->noCalcList);
      for(var=block->noCalcList; var; var=var->next)
      {  LogAddress((void*)&var->var);
         LogAddress((void*)&var->next);
      }

      LogAddress((void*)&block->next);
   }

   return(0);
}


long RegisterModelPointers(tModel *model)
{
   tNetInput   *input;
   tEngVar     *constant, *fldVar;
   tProfileMap *PRM;

   /* Register the deployNet structure */
   if(model->pNet)
   {  LogAddress((void*)&model->pNet);
      RegisterNetPointers(model->pNet);
   }

   /* Register the base rules string */
   LogAddress((void*)&model->baseRules);

   /* Register the net inputs */
   LogAddress((void*)&model->inputList);
   for(input=model->inputList; input; input=input->next)
   {  LogAddress((void*)&input->name);
      LogAddress((void*)&input->data);
      LogAddress((void*)&input->modelVar);
      LogAddress((void*)&input->next);
   }

   /* Register the ISAM data */
   LogAddress((void*)&model->ISAMList);
   RegisterISAMList(model->ISAMList);

   /* Register the dynamic table structures */
   LogAddress((void*)&model->tableList);
   RegisterTableList(model->tableList);

   /* Register the profile map */
   LogAddress((void*)&model->PRMList);
   for(PRM=model->PRMList; PRM; PRM=PRM->next)
   {  LogAddress((void*)&PRM->prof);
      LogAddress((void*)&PRM->field);
      LogAddress((void*)&PRM->varName);
      LogAddress((void*)&PRM->defValue);
      RegisterVarPointers(PRM->defValue);
      LogAddress((void*)&PRM->data);
      LogAddress((void*)&PRM->next);
   }

   /* Register the constants */
   LogAddress((void*)&model->constantList);
   for(constant=model->constantList; constant; constant=constant->next)
      RegisterVarPointers(constant);

   /* Register the field variables */
   LogAddress((void*)&model->fieldVars);
   for(fldVar=model->fieldVars; fldVar; fldVar=fldVar->next)
      RegisterVarPointers(fldVar);
   
   /* Register variable block pointers */
   if(model->mainBlockList)
   {  LogAddress((void*)&model->mainBlockList);
      RegisterVarBlock(model->mainBlockList);
   }

   if(model->subBlockList)
   {  LogAddress((void*)&model->subBlockList);
      RegisterVarBlock(model->subBlockList);
   }


   /* Register main rule base data */
   LogAddress((void*)&model->modelRBData);
   RegisterRBDataPointers(model->modelRBData);
   LogAddress((void*)&model->modelRuleHandle);
   RegisterRuleEngPointers(model->modelRuleHandle);

   /* Register profile init rule base data */
   if(model->modelPInitHandle)
   {  LogAddress((void*)&model->modelPInitData);
      RegisterRBDataPointers(model->modelPInitData);
      LogAddress((void*)&model->modelPInitHandle);
      RegisterRuleEngPointers(model->modelPInitHandle);
   }   

   LogAddress((void*)&model->next);

   return(0);
}


long RegisterMgModelPointers(void *modelHandle)
{
   tModelManage  *mgModel = (tModelManage*)modelHandle;
   tModelSet     *modSet;
   tModel        *model;
   tEngVar       *sharedVar;


   /* Register all the model sets */
   LogAddress((void*)&mgModel->setList);
   for(modSet=mgModel->setList; modSet; modSet=modSet->next)
   {  LogAddress((void*)&modSet->modelList);
      for(model=modSet->modelList; model; model=model->next)
         RegisterModelPointers(model);

      LogAddress((void*)&modSet->sharedVarList);
      for(sharedVar=modSet->sharedVarList; sharedVar; sharedVar=sharedVar->next)
         RegisterVarPointers(sharedVar);

      LogAddress((void*)&modSet->next);
   }

   LogAddress((void*)&mgModel->masterData);
   LogAddress((void*)&mgModel->calcDones);
   LogAddress((void*)&mgModel->currModel);
   
   return(0);
}


long RegisterRulePointers(tRuleManage *ruleMg)
{
   tTmpltMap     *tmpltMap;
   tTmpltNode    *node;
 
   /**************************************/
   /* Register rule base data structures */
   /**************************************/
   LogAddress((void*)&ruleMg->ruleBase);
   LogAddress((void*)&ruleMg->ruleBase->rootHandle);
   LogAddress((void*)&ruleMg->ruleBase->ruleBaseList);
   RegisterRuleEngPointers(ruleMg->ruleBase->rootHandle);


   /******************************/
   /* Register the template maps */
   /******************************/
   LogAddress((void*)&ruleMg->templateMaps);
   for(tmpltMap=ruleMg->templateMaps; tmpltMap; tmpltMap=tmpltMap->next)
   {   LogAddress((void*)&tmpltMap->nodeList);
       for(node=tmpltMap->nodeList; node; node=node->next)
       {  LogAddress((void*)&node->tmplt);
          LogAddress((void*)&node->next);
       }

       LogAddress((void*)&tmpltMap->next);
   }


   /**************************************/
   /* Register the master data structure */
   /**************************************/
   LogAddress((void*)&ruleMg->masterData.templateList);
   RegisterTemplateList(ruleMg->masterData.templateList);

   LogAddress((void*)&ruleMg->masterData.funcList);
   RegisterFuncList(ruleMg->masterData.funcList);

   LogAddress((void*)&ruleMg->masterData.DbUserId);
   LogAddress((void*)&ruleMg->masterData.DbPassword);

   LogAddress((void*)&ruleMg->masterData.ISAMList);
   RegisterISAMList(ruleMg->masterData.ISAMList);


   /*********************************/
   /* Register the model structures */
   /*********************************/
   if(ruleMg->modelHandle)
   {  LogAddress((void*)&ruleMg->modelHandle);
      RegisterMgModelPointers(ruleMg->modelHandle);
   }


   /***********************************/
   /* Register the funcInit structure */
   /***********************************/
   LogAddress((void*)&ruleMg->funcInit);
   LogAddress((void*)&ruleMg->funcInit->modelHandle);
   LogAddress((void*)&ruleMg->funcInit->dynTableList);

   return(0);
}


void RegisterPMaxPointers(tMax *max, long *errCode)
{
   /* Clear out the address log */
   ClearLog();

   /* Register the tMax structure */
   LogAddress((void*)&max->self);
   LogAddress((void*)&max->ruleMg);

   LogAddress((void*)&max->tranData);
   LogAddress((void*)&max->profData);
   LogAddress((void*)&max->scoreData);
   LogAddress((void*)&max->explnData);
   
   /* Register the tRuleManage structure */
   *errCode = RegisterRulePointers(max->ruleMg);
}

#endif


long ResetFalconRuleFunc(tFunc *func)
{
   return(0);
}

long ResetFalconModelFunc(tFunc *func)
{
   if(!NoCaseStrcmp(func->functionName, "Unpack_Zip"))
      func->FuncPtr = UnpackZip;
   else if(!NoCaseStrcmp(func->functionName, "Pack_Zip"))
      func->FuncPtr = PackZip;
   else if(!NoCaseStrcmp(func->functionName, "Old_Unpack_Zip"))
      func->FuncPtr = OldUnpackZip;
   else if(!NoCaseStrcmp(func->functionName, "Old_Pack_Zip"))
      func->FuncPtr = OldPackZip;
   else
      fprintf(stderr, "Can't find function '%s' to reset its "
                      "address\n", func->functionName);
   return(0);
}

long ResetProductFunc(tFunc *func)
{
   ResetFalconRuleFunc(func);
   ResetFalconModelFunc(func);

   return(0);
}

long ResetModelFunc(tFunc *func)
{
   if(!NoCaseStrcmp(func->functionName, "Score_Model"))
      func->FuncPtr = ScoreModel;
   else if(!NoCaseStrcmp(func->functionName, "Get_Reasons"))
      func->FuncPtr = GetReasons;
   else if(!NoCaseStrcmp(func->functionName, "Get_Reasons_Value"))
      func->FuncPtr = GetReasonsValue;
   else if(!NoCaseStrcmp(func->functionName, "Calc_Vars"))
      func->FuncPtr = CalcVars;
   else if(!NoCaseStrcmp(func->functionName, "Calc_Sub_List"))
      func->FuncPtr = CalcSubList;
   else if(!NoCaseStrcmp(func->functionName, "Prof_Init"))
      func->FuncPtr = ProfInit;
   else if(!NoCaseStrcmp(func->functionName, "Calc_Risk"))
      func->FuncPtr = CalcRisk;
   else if(!NoCaseStrcmp(func->functionName, "Calc_Z"))
      func->FuncPtr = CalcZ;
   else if(!NoCaseStrcmp(func->functionName, "Prob_Norm"))
      func->FuncPtr = ProbNorm;
   else if(!NoCaseStrcmp(func->functionName, "Get_GMT_Offset"))
      func->FuncPtr = GetGMTOffset;
   else if(!NoCaseStrcmp(func->functionName, "Dump_Data"))
      func->FuncPtr = DumpData;
   else 
      ResetProductFunc(func);
 
   return(0);
}


long ResetMasterFuncList(tFunc *funcList)
{
   tFunc *func;

   for(func=funcList; func; func=func->next)
   {  if(!NoCaseStrcmp(func->functionName, "Fire_Model"))
         func->FuncPtr = FireModel;
      else if(!NoCaseStrcmp(func->functionName, "Get_Model_Error_String"))
         func->FuncPtr = GetModelErrorString;
      else if(!NoCaseStrcmp(func->functionName, "InitProfiles"))
         func->FuncPtr = InitProfilesFunc;
      else if(!NoCaseStrcmp(func->functionName, "ResetModels"))
         func->FuncPtr = ResetModelsFunc;
      else if(!NoCaseStrcmp(func->functionName, "Print"))
         func->FuncPtr = Print;
      else if(!NoCaseStrcmp(func->functionName, "Write"))
         func->FuncPtr = Write;
      else if(!NoCaseStrcmp(func->functionName, "Writeln"))
         func->FuncPtr = Writeln;
      else if(!NoCaseStrcmp(func->functionName, "Is_Missing"))
         func->FuncPtr = IsMissing;
      else if(!NoCaseStrcmp(func->functionName, "Is_Int_String"))
         func->FuncPtr = IsIntString;
      else if(!NoCaseStrcmp(func->functionName, "Is_Digit"))
         func->FuncPtr = IsDigit;
      else if(!NoCaseStrcmp(func->functionName, "Is_Alpha"))
         func->FuncPtr = IsAlpha;
      else if(!NoCaseStrcmp(func->functionName, "Is_Alnum"))
         func->FuncPtr = IsAlnum;
      else if(!NoCaseStrcmp(func->functionName, "String_To_Num"))
         func->FuncPtr = StringToNum;
      else if(!NoCaseStrcmp(func->functionName, "Max"))
         func->FuncPtr = Max;
      else if(!NoCaseStrcmp(func->functionName, "Min"))
         func->FuncPtr = Min;
      else if(!NoCaseStrcmp(func->functionName, "Exp"))
         func->FuncPtr = Exp;
      else if(!NoCaseStrcmp(func->functionName, "Trunc"))
         func->FuncPtr = Trunc;
      else if(!NoCaseStrcmp(func->functionName, "Abs"))
         func->FuncPtr = Abs;
      else if(!NoCaseStrcmp(func->functionName, "Power"))
         func->FuncPtr = Power;
      else if(!NoCaseStrcmp(func->functionName, "Round"))
         func->FuncPtr = Round;
      else if(!NoCaseStrcmp(func->functionName, "Sqrt"))
         func->FuncPtr = Sqrt;
      else if(!NoCaseStrcmp(func->functionName, "Log"))
         func->FuncPtr = Log;
      else if(!NoCaseStrcmp(func->functionName, "Log10"))
         func->FuncPtr = Log10;
      else if(!NoCaseStrcmp(func->functionName, "Mod"))
         func->FuncPtr = Mod;
      else if(!NoCaseStrcmp(func->functionName, "Div"))
         func->FuncPtr = Div;
      else if(!NoCaseStrcmp(func->functionName, "Get_Fraction"))
         func->FuncPtr = GetFraction;
      else if(!NoCaseStrcmp(func->functionName, "Left"))
         func->FuncPtr = Left;
      else if(!NoCaseStrcmp(func->functionName, "Right"))
         func->FuncPtr = Right;
      else if(!NoCaseStrcmp(func->functionName, "Mid"))
         func->FuncPtr = Mid;
      else if(!NoCaseStrcmp(func->functionName, "Valid_Date"))
         func->FuncPtr = ValidDateFunc;
      else if(!NoCaseStrcmp(func->functionName, "Valid_Time"))
         func->FuncPtr = ValidTime;
      else if(!NoCaseStrcmp(func->functionName, "Valid_Amount"))
         func->FuncPtr = ValidAmount;
      else if(!NoCaseStrcmp(func->functionName, "Time_Convert"))
         func->FuncPtr = TimeConvert;
      else if(!NoCaseStrcmp(func->functionName, "Date_Convert"))
         func->FuncPtr = DateConvert;
      else if(!NoCaseStrcmp(func->functionName, "Date_Time_Convert"))
         func->FuncPtr = DateTimeConvert;
      else if(!NoCaseStrcmp(func->functionName, "Is_Week_Day"))
         func->FuncPtr = IsWeekDay;
      else if(!NoCaseStrcmp(func->functionName, "Day_Of_Week"))
         func->FuncPtr = DayOfWeek;
      else if(!NoCaseStrcmp(func->functionName, "Day90_To_String"))
         func->FuncPtr = Day90ToString;
      else if(!NoCaseStrcmp(func->functionName, "Date_Diff"))
         func->FuncPtr = DateDiff;
      else if(!NoCaseStrcmp(func->functionName, "Month_Diff"))
         func->FuncPtr = MonthDiff;
      else if(!NoCaseStrcmp(func->functionName, "Table_Lookup"))
         func->FuncPtr = TableLookup;
      else if(!NoCaseStrcmp(func->functionName, "Table_Range_Lookup"))
         func->FuncPtr = TableRangeLookup;
      else if(!NoCaseStrcmp(func->functionName, "Table_Direct_Index"))
         func->FuncPtr = TableDirectIndex;
      else if(!NoCaseStrcmp(func->functionName, "Table_Get_Defaults"))
         func->FuncPtr = TableGetDefaults;
      else if(!NoCaseStrcmp(func->functionName, "Table_Multi_Lookup"))
         func->FuncPtr = TableMultiLookup;
      else
         ResetModelFunc(func);
   }

   return(0);
}


void ResetFalconVarCalcFunc(tModelVar *modelVar)
{
   switch(modelVar->funcCode)
   {  case FAL_EVENT_AVG_COEF1:
         modelVar->func = FalEventAvgCoef1;
         break;
      case FAL_EVENT_AVG_COEF2:
         modelVar->func = FalEventAvgCoef2;
         break;
      case FAL_DLY_COEF1:
         modelVar->func = FalDlyCoef1;
         break;
      case FAL_CONSEC:
         modelVar->func = FalConsec;
         break;
      case FAL_NORM_TIME:
         modelVar->func = FalNormTime;
         break;
      case FAL_NORM_DAY:
         modelVar->func = FalNormDay;
         break;
      case FAL_AGE_DECAY:
         modelVar->func = FalAgeDecay;
         break;
      case FAL_RISK_RATE:
         modelVar->func = FalRiskRate;
         break;
      case FAL_RISK_Z:
         modelVar->func = FalRiskZ;
         break;
      case FAL_CODE_CONV:
         modelVar->func = FalCodeConv;
         break;
      case FAL_DIST_HOME:
         modelVar->func = FalDistHome;
         break;
      case FAL_DIST_PREV:
         modelVar->func = FalDistPrev;
         break;
      case FAL_GEO_RISK:
         modelVar->func = FalGeoRisk;
         break;
      default:
         fprintf(stderr, "Can't find function for code %d for variable '%s'\n",
                         modelVar->funcCode, modelVar->cfgName);
   }
}


void ResetProductVarCalcFunc(tModelVar *modelVar)
{
   ResetFalconVarCalcFunc(modelVar);
}


void ResetVarCalcFunc(tModelVar *modelVar)
{
   /* Make sure variable has a function */
   if(!modelVar)
      return;
   else if(!modelVar->args)
      return;

   /* Set the function for the function code */
   switch(modelVar->funcCode)
   {  case EVENT_AVG:
         modelVar->func = EventAvg;
         break;
      case EVENT_COEF1_CALC:
         modelVar->func = EventAvgCoef1Calc;
         break;
      case DAILY_RATE:
         modelVar->func = DailyRate;
         break;
      case RATE_COEF1_CALC:
         modelVar->func = DailyRateCoef1Calc;
         break;
      case RATE_COEF2_CALC:
         modelVar->func = DailyRateCoef2Calc;
         break;
      case MAX_DECAY:
         modelVar->func = MaxDecay;
         break;
      case MAX_COEF_CALC:
         modelVar->func = MaxDecayCoefCalc;
         break;
      case MAX_SHIFT:
         modelVar->func = MaxShift;
         break;
      case NORMALIZE:
         modelVar->func = Normalize;
         break;
      case DO_NOTHING:
         modelVar->func = DoNothing;
         break;
      case DO_ARITHMETIC:
         modelVar->func = DoArithmetic;
         break;
      case GET_DECIMAL:
         modelVar->func = GetDecimal;
         break;
      case LOG10:
         modelVar->func = CalcLog10;
         break;
      case AGE_DECAY:
         modelVar->func = AgeDecay;
         break;
      case VARIANCE:
         modelVar->func = Variance;
         break;
      case WHICH_BIN:
         modelVar->func = WhichBin;
         break;
      case DO_COND_ARITHMETIC:
         modelVar->func = DoCondArithmetic;
         break;
      case RATE_DECAY:
         modelVar->func = RateDecay;
         break;
      case EXP_DECAY:
         modelVar->func = ExpDecay;
         break;
      case MONTH_DIFF:
         modelVar->func = CalcMonthDiff;
         break;
      case LOOKUP_TAB:
         modelVar->func = CalcTableLookup;
         break;
      case RANGE_TAB:
         modelVar->func = CalcTableRange;
         break;
      case DIRECT_TAB:
         modelVar->func = CalcTableDirect;
         break;
      case STR_EQL:
         modelVar->func = StrEql;
         break;
      case FLOAT_SET:
         modelVar->func = FloatSet;
         break;
      case FLOAT_ADD:
         modelVar->func = FloatAdd;
         break;
      case FLOAT_SUB:
         modelVar->func = FloatSub;
         break;
      case FLOAT_MUL:
         modelVar->func = FloatMul;
         break;
      case FLOAT_DIV:
         modelVar->func = FloatDiv;
         break;
      case FLOAT_EQL:
         modelVar->func = FloatEql;
         break;
      case FLOAT_NOTEQL:
         modelVar->func = FloatNotEql;
         break;
      case FLOAT_GRTR:
         modelVar->func = FloatGrtr;
         break;
      case FLOAT_GRTREQL:
         modelVar->func = FloatGrtrEql;
         break;
      case FLOAT_LESS:
         modelVar->func = FloatLess;
         break;
      case FLOAT_LESSEQL:
         modelVar->func = FloatLessEql;
         break;
      case FLOAT_TRUNC:
         modelVar->func = FloatTrunc;
         break;
      case FLOAT_MAX:
         modelVar->func = FloatMax;
         break;
      case FLOAT_MIN:
         modelVar->func = FloatMin;
         break;
      default:
         ResetProductVarCalcFunc(modelVar);
   }
}


void ResetModelFuncs(void *modelHandle)
{
   tModelManage  *mgModel = (tModelManage*)modelHandle;
   tModelSet     *modSet;
   tModel        *model;
   tEngVar       *var;

   for(modSet=mgModel->setList; modSet; modSet=modSet->next)
   {  for(model=modSet->modelList; model; model=model->next)
      {  /* Reset the variable calc functions */
         for(var=model->modelRBData->globalVarList; var; var=var->next)
            if(var->user) 
               ResetVarCalcFunc((tModelVar*)var->user);

         /* Reset the argument functions */
         ResetArgFuncs(model->modelRuleHandle);

         if(model->modelPInitHandle)
            ResetArgFuncs(model->modelPInitHandle);
      }
   }
}


void ResetFunctionPointers(void *handle, long *errCode)
{
   tMax *max = (tMax*)handle;

   /* Reset the master function pointer list */
   ResetMasterFuncList(max->ruleMg->masterData.funcList);

   /* Reset all function pointers for the model */
   ResetModelFuncs(max->ruleMg->modelHandle);

   /* Reset the argument functions for the main rule base */
   ResetArgFuncs(max->ruleMg->ruleBase->rootHandle);
}
