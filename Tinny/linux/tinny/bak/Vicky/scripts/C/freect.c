/*
** $Id: freect.c,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: freect.c,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.4  1997/02/05  03:31:22  can
 * Added check for TYPE_SHARED before freeing a fieldList of a
 * record
 *
 * Revision 2.3  1997/01/16  22:44:06  can
 * Lots of changes for 2.1 release
 *
 * Revision 2.2  1996/07/30  19:02:38  can
 * Added FreeInternalGlobals to free the internally generated
 * global variables.
 *
 * Revision 2.1  1996/04/22  17:55:38  can
 * Several fixes and changes for 2.1
 *
 * Revision 1.6  1995/12/12  17:27:12  can
 * Made sure that string parameters are NOT free'd here because
 * they are no longer allocated, but rather just set from their
 * input data.
 *
 * Revision 1.5  1995/08/25  01:25:48  can
 * Fixed compile warning messages
 *
 * Revision 1.4  1995/08/09  23:56:00  can
 * Added free'ing of variables and string data.
 *
 * Revision 1.3  1995/07/21  15:35:44  can
 * Modified clean-up routines.  Added code to handle use of variables.
 *
 * Revision 1.2  1995/07/09  23:07:20  can
 * Fixed some bugs and updated the error handling
 *
 * Revision 1.1  1995/07/03  21:18:26  wle
 * Initial revision
 *
*/

/* System include files */
#include <stdio.h>
#include <stdlib.h>


/* HNC Rule Engine include files */
#include "mymall.h"
#include "exptree.h"
#include "prsrule.h"
#include "engerr.h"
#include "creatct.h"
#include "freect.h"
#include "typechk.h"
#include "bltins.h"

#ifdef DEVELOP
#include "rbdebug.h"
#endif


long FreeTraceData(tTrace* trace);
long FreeArgList(tArg* argList);
long FreeArgument(tArg* arg);
long FreeArgCalc(tArg* arg);
long FreeExprCalc(tExprCalc* exprCalc);
long FreeStmtBlock(tStmtCalc* stmtBlock);
long FreeVarCalcs(tVarCalc *varCalcList);
long FreeStr2NumCalcs(tStr2NumCalc *str2numList);


/*--------------------------------------------------------------------------*/
/* FreeArrayType()                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long FreeArrayType(tEngArray *array)
{
   return(OK);
}


/*--------------------------------------------------------------------------*/
/* FreeRecordType()                                                         */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long FreeRecordType(tEngRecord *record, int freeName)
{
   tRecField *fld, *delFld;

   fld=record->fieldList;
   while(fld)
   {  if(freeName) MyFree((void*)fld->fieldName);
      
      delFld = fld;
      fld = fld->next;
      MyFree((void*)delFld);
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* FreeType()                                                               */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long FreeType(tType *type)
{
   if(type->type == RECORD_TYPE && !(type->flags&TYPE_SHARED))
      FreeRecordType(&type->fmt.record, !(type->flags&TYPE_TEMPLATE));
   else if(type->type == ARRAY_TYPE)
      FreeArrayType(&type->fmt.array);

   MyFree((void*)type->typeName);
   MyFree((void*)type);

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* FreeRecordData()                                                         */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long FreeRecordData(tRecData *recData, int freeBuffer)
{
   tFldData *fld;
   long     f;

   /* Free the data buffer */
   if(freeBuffer)
      MyFree((void*)*recData->data);
   MyFree((void*)recData->data);
 
   /* Free the field data, recursively freeing sub records if necessary */
   for(f=0; f<recData->record->numFlds; f++)
   {  fld = recData->flds+f;

      /* If there is no field data, there is nothing to free */
      if(!fld->data) continue;

      if(fld->fld->type == RECORD_TYPE)
      {  if(fld->origData)
            FreeRecordData((tRecData*)fld->data, FALSE);
      }
      else if(fld->fld->type == ARRAY_TYPE)
      { if(fld->origData==1)
           FreeArrayData((tArrayData*)fld->data, FALSE);
        else if(fld->origData==2)
           MyFree((void*)fld->data);
      }
      else
      {  if(fld->origData)
            MyFree((void*)fld->data);
      }
   } 

   /* Free the field array */
   MyFree((void*)recData->flds);

   /* Free the structure itself */
   MyFree((void*)recData);
 
   return(OK);
}


/*--------------------------------------------------------------------------*/
/* FreeArrayData()                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long FreeArrayData(tArrayData *arrayData, int freeBuffer)
{
   long i;

   /* Free the element data, if necessary */ 

   for(i=0; i<arrayData->array->numElems; i++)
   {  if(arrayData->array->type == RECORD_TYPE)  
      {  if(freeBuffer && i==0)
            FreeRecordData((tRecData*)arrayData->data[i], TRUE);
         else
            FreeRecordData((tRecData*)arrayData->data[i], FALSE);
      }
      else if(i==0 && freeBuffer)
         MyFree((void*)arrayData->data[i]);
   }

   /* Free the pointers */
   MyFree((void*)arrayData->data);
  
   /* Free the structure itself */
   MyFree((void*)arrayData);

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* FreeVarStruct()                                                           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long FreeVarStruct(tEngVar *var)
{
   MyFree((void*)var->varName);

   if(!(var->flags&VAR_PARAMETER))
   {  if(var->type == RECORD_TYPE)
      {  if(!(var->flags&VAR_EXTDATA))
         {  if(var->flags&VAR_TEMPLATE)
               FreeRecordData(((tRecInstance*)var->data)->data, FALSE);
            else
            {  FreeRecordData(((tRecInstance*)var->data)->data, TRUE);
               MyFree((void*)var->data);
            }
         }
      }
      else if(var->type == ARRAY_TYPE)
         FreeArrayData((tArrayData*)var->data, TRUE);
      else if(!(var->flags&VAR_TEMPLATE))
         MyFree((void*)var->data);
   }
   else if(!(var->flags&VAR_REFERENCE) && !IsString(var->type))
      MyFree((void*)var->data);
   
   return(OK);
}


/*--------------------------------------------------------------------------*/
/* FreeInternalGlobals()                                                    */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long FreeInternalGlobals(tRuleBaseData *RBData)
{
   tEngVar *var, *delVar, *lastVar=NULL, *nonIntVar=NULL;
   tType   *type, *delType, *lastType=NULL, *nonIntType=NULL;

   /* Go through global variable list freeing all internal ones */
   var=RBData->globalVarList;
   while(var)
   {  if(var->flags&VAR_GLOBINT)  
      {  delVar = var;
         var = var->next;

         FreeVarStruct(delVar);
         MyFree((void*)delVar);
      }
      else 
      {  if(!nonIntVar)
            nonIntVar = var;
         if(lastVar) lastVar->next = var;
         lastVar = var;
         var = var->next;
         lastVar->next = NULL;
      }
   }

   /* Reset the first pointer, if necessary */
   if(nonIntVar != RBData->globalVarList)
      RBData->globalVarList = nonIntVar;


   /* Go through global type list freeing all internal ones */
   type=RBData->globalTypeList;
   while(type)
   {  if(type->flags&TYPE_GLOBINT)  
      {  delType = type;
         type = type->next;
         FreeType(delType);
      }
      else 
      {  if(!nonIntType)
            nonIntType = type;
         if(lastType) lastType->next = type;
         lastType = type;
         type = type->next;
         lastType->next = NULL;
      }
   }

   /* Reset the first pointer, if necessary */
   if(nonIntType != RBData->globalTypeList)
      RBData->globalTypeList = nonIntType;

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* FreeAllDataList()                                                        */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long FreeAllDataList(tDataPtr *allData)
{
   tDataNode   *data, *delData;

   data = allData->varList;
   while(data)
   {  FreeVarStruct(data->val.var);
      MyFree((void*)data->val.var);

      delData = data;
      data = data->next;
      MyFree((void*)delData);
   }

   data = allData->typeList;
   while(data)
   {  FreeType(data->val.type);

      delData = data;
      data = data->next;
      MyFree((void*)delData);
   }

   MyFree((void*)allData);

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* FreeRecordUse()                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long FreeRecordUse(tRecUse *recUse)
{
   tArrayProc *array, *delArray;

   array = recUse->arrayProc;
   while(array)
   {  delArray = array;
      array = array->next;
 
      if(delArray->arrayUse) 
      {  FreeArgument((tArg*)delArray->arrayUse->indexExpr);
         MyFree((void*)delArray->arrayUse);
      }

      MyFree((void*)delArray);
   }

   if(recUse->fldData->flags&FIELD_DUMMY)
      MyFree((void*)recUse->fldData);

   MyFree((void*)recUse->recordName);
   MyFree((void*)recUse->fieldName);
   MyFree((void*)recUse);
   
   return(0);
}


/*--------------------------------------------------------------------------*/
/* FreeExprCalc()                                                           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long FreeExprCalc(tExprCalc *exprList)
{
   tExprCalc *exprCalc, *delCalc; 

   exprCalc = exprList;
   while(exprCalc)
   {  /* Free left and right arguments, if any */ 
      if(exprCalc->flags&LEFT_ARG)
         FreeArgument((tArg*)exprCalc->left);
      if(exprCalc->flags&RIGHT_ARG)
         FreeArgument((tArg*)exprCalc->right);
       
      delCalc = exprCalc;
      exprCalc = exprCalc->next;
      MyFree((void*)delCalc);
   }

   return(0);
}


/*--------------------------------------------------------------------------*/
/* FreeArgCalc()                                                            */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long FreeArgCalc(tArg *arg)
{
   /* Note that string to numeric and var calcs are not free here */
   if(arg->flags&ATOM_ARG_CALC)
   {  tArgCalc *argCalc = (tArgCalc*)arg->atom.data.uVal;

      /* Do special things for each function that needs it */

      FreeArgList(argCalc->argList);
      MyFree((void*)argCalc);
   }
   else if(arg->flags&ATOM_FUNC_CALC)
   {  tFuncCalc *funcCalc = (tFuncCalc*)arg->atom.data.uVal;

      FreeArgList(funcCalc->argList);
      MyFree((void*)funcCalc);
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* FreeArgList()                                                            */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long FreeArgument(tArg *arg)
{
   /* Get the type of calc node the argument is, if any, and act accordingly */ 
   if(arg->flags&ATOM_STMT_CALC)
      FreeStmtBlock((tStmtCalc*)arg->atom.data.uVal);
   else if(arg->flags&ATOM_EXPR_CALC)
      FreeExprCalc((tExprCalc*)arg->atom.data.uVal);
   else if(arg->flags >= ATOM_ARG_CALC)
      FreeArgCalc(arg);
   else
   {  /* Free a string constant */
      if(arg->flags&ATOM_CONSTANT && arg->flags&ATOM_STRING)
         MyFree((void*)arg->atom.data.strVal.str);
  
      /* Free a record use structure */
      else if(arg->flags&ATOM_ARRAYPROC)
         FreeRecordUse((tRecUse*)arg->atom.data.uVal); 
   }

   MyFree((void*)arg);
   return(OK);
}


/*--------------------------------------------------------------------------*/
/* FreeArgList()                                                            */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long FreeArgList(tArg *argList)
{
   tArg      *arg, *delArg;

   arg = argList;
   while(arg)
   {  delArg = arg;
      arg = arg->next;
      FreeArgument(delArg);
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* FreeTraceData()                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long FreeTraceData(tTrace *trace)
{
   tStmtText *txt, *delTxt;

   if(trace)
   {  txt = trace->textList;
      while(txt)
      {  delTxt = txt;
         txt = txt->next;

         MyFree((void*)delTxt->text);
         MyFree((void*)delTxt);
      }
   }
 
   return(OK);
}


/*--------------------------------------------------------------------------*/
/* FreeStmtBlock()                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long FreeStmtBlock(tStmtCalc *stmtBlock)
{
   tArg      *stmtArg, *delArg;

   stmtArg = stmtBlock->argList;
   while(stmtArg)
   {  delArg = stmtArg;
      stmtArg = stmtArg->next;

      FreeArgument(delArg);
   }

   MyFree((void*)stmtBlock);

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* FreeStr2NumCalcs()                                                       */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long FreeStr2NumCalcs(tStr2NumCalc *str2numList)
{
   tStr2NumCalc *str2numCalc, *delCalc;

   str2numCalc = str2numList;
   while(str2numCalc)
   {  delCalc = str2numCalc;
      str2numCalc = str2numCalc->next;
    
      MyFree((void*)delCalc->id);
      MyFree((void*)delCalc);
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* FreeVarCalcs()                                                           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long FreeVarCalcs(tVarCalc *varCalcList)
{
   tVarCalc *varCalc, *delCalc;

   varCalc = varCalcList;
   while(varCalc)
   {  delCalc = varCalc;
      varCalc = varCalc->next;
    
      MyFree((void*)delCalc->id);
      FreeArgument(delCalc->arg);

      MyFree((void*)delCalc);
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* FreeRuleCalcs()                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long FreeRuleCalcs(tRuleCalc *ruleList)
{
   tRuleCalc  *ruleCalc, *delCalc;
   tRuleParam *param, *delParam;

   ruleCalc = ruleList;
   while(ruleCalc)
   {  delCalc = ruleCalc;
      ruleCalc = ruleCalc->next;
    
      param = delCalc->paramList;
      while(param)
      {  delParam = param;
         param = param->next;
         MyFree((void*)delParam);
      }

      MyFree((void*)delCalc->id);
      FreeStmtBlock(delCalc->stmtBlock);

      MyFree((void*)delCalc);
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* FreeCalcTables()                                                         */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long FreeCalcTables(tCalcTable *calcTab)
{
   tUsedFunc    *tmpUF, *delUF;
   tCalcTable   *delCalcTab;
   tPoolPage    *delPage, *dynPage;
   tRecInstance *tmpltRec, *delRec;
   int          i;
 
   /*******************************/
   /* Free the dynamic data pages */
   /*******************************/
   if(calcTab->dynPool)
   {  dynPage = calcTab->dynPool->pageList;
      while(dynPage)
      {  delPage = dynPage;
         dynPage = dynPage->next;

         MyFree((void*)delPage->data);
         MyFree((void*)delPage);
      }
      MyFree((void*)calcTab->dynPool);
   }

   /***********************************************/
   /* Free all variables and parameters allocated */
   /***********************************************/
   if(calcTab->allData)
      FreeAllDataList(calcTab->allData);

   /*****************************/
   /* Free internal global data */
   /*****************************/
   FreeInternalGlobals(calcTab->ruleBaseData);

   /******************************************************/
   /* Free the tRecInstance structures for the templates */
   /******************************************************/
   tmpltRec = calcTab->tmpltRecList;
   while(tmpltRec)
   {  delRec = tmpltRec;
      tmpltRec = tmpltRec->next;
      MyFree((void*)delRec);
   }

   /*****************************/
   /* Free the calc dones array */
   /*****************************/
   if(calcTab->calcDones) 
      MyFree((void*)calcTab->calcDones);

   /************************************/
   /* Free the argument function array */
   /************************************/
   for(i=0; i<calcTab->numArgFuncs; i++)
      MyFree((void*)calcTab->argFuncArray[i].funcName);
   MyFree((void*)calcTab->argFuncArray);


   /****************************************/
   /* Free all the calc tables in the list */
   /****************************************/
   while(calcTab)
   {  /***********************/
      /* Free the calc lists */
      /***********************/
      FreeRuleCalcs(calcTab->sysInitList);
      FreeRuleCalcs(calcTab->sysTermList);
      FreeRuleCalcs(calcTab->iterInitList);
      FreeRuleCalcs(calcTab->iterTermList);
      FreeRuleCalcs(calcTab->ruleCalcList);
      FreeRuleCalcs(calcTab->paramRuleList);
      FreeStr2NumCalcs(calcTab->str2numList);
      FreeVarCalcs(calcTab->varCalcList);

      /********************************/
      /* Free the used function lists */
      /********************************/
      tmpUF = calcTab->usedFuncList;
      while(tmpUF)
      {  delUF = tmpUF;
         tmpUF = tmpUF->next;
         MyFree((void*)delUF);
      }

      /***************************************/
      /* Finally, free the calc table itself */
      /***************************************/
      delCalcTab = calcTab;
      calcTab = calcTab->next;
      MyFree((void*)delCalcTab);
   }
 
   return(OK);
}
