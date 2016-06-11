/*
** $Id: evlarg.c,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: evlarg.c,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 1.2  1997/02/12  00:06:25  can
 * Made sure EXEC rule base call evaluates to a NUMERIC type.
 *
 * Revision 1.1  1997/01/16  22:44:06  can
 * Initial revision
 *
*/

/* System include files */
#include <stdio.h>
#include <string.h>

/* HNC Rule Engine includes */
#include "evlarg.h"
#include "evlstmt.h"
#include "evlexpr.h"
#include "bltins.h"
#include "typechk.h"
#include "engerr.h"
#include "runct.h"
#include "mymall.h"

/* HNC common includes */
#include "except.h"



void EvalFldData(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{
   if(arg->atom.type == RECORD_TYPE || arg->atom.type == ARRAY_TYPE)
      eval->data.uVal = (void*)((tFldData*)arg->atom.data.uVal)->data;
   else
      eval->data.uVal = (void*)((tFlatData*)arg->atom.data.uVal)->data;
   
   eval->type = arg->atom.type;
}

void EvalFldString(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{ 
   eval->data.strVal.str = (char*)((tFlatData*)arg->atom.data.uVal)->data;
   eval->data.strVal.len = ((tFlatData*)arg->atom.data.uVal)->length;
   eval->type = arg->atom.type;
}

void EvalAsIs(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   *eval = arg->atom;
}

void EvalParamData(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   eval->data.uVal = (void*)((tEngVar*)arg->atom.data.uVal)->data;
   eval->type = arg->atom.type;
}

void EvalStringParam(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   eval->data.strVal.str = (char*)((tEngVar*)arg->atom.data.uVal)->data;
   eval->data.strVal.len = ((tEngVar*)arg->atom.data.uVal)->dataSize;
   eval->type = arg->atom.type;
}

void EvalVarCalcData(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   tVarCalc  *varCalc = (tVarCalc*)arg->atom.data.uVal;

   if(*varCalc->var->calcRule->calcDone)
   {  eval->data.uVal = (void*)varCalc->var->data;
      eval->type = varCalc->var->type;
   }
   else
   {  *varCalc->var->calcRule->calcDone = 1;
      varCalc->arg->func->EvalFunc(eval, varCalc->arg, status, calcTab);
   }

   if(status->errCode == RETURN_FOUND) status->errCode = 0;
}

void EvalStringVarCalc(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{ 
   tVarCalc  *varCalc = (tVarCalc*)arg->atom.data.uVal;

   if(*varCalc->var->calcRule->calcDone)
   {  eval->data.strVal.str = (char*)varCalc->var->data;
      eval->data.strVal.len = varCalc->var->dataSize;
      eval->type = varCalc->var->type;
   }
   else
   {  *varCalc->var->calcRule->calcDone = 1;
    
      varCalc->arg->func->EvalFunc(eval, varCalc->arg, status, calcTab);
   }

   if(status->errCode == RETURN_FOUND) status->errCode = 0;
}

void EvalErrorStr(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   eval->data.strVal.str = status->errStr;
   eval->data.strVal.len = strlen(status->errStr);
   eval->type = HNC_STRING;
}

void EvalErrorNum(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETINT32(eval->data.num, status->errCode);
   eval->type = NUMERIC_TYPE; 
}



void EvalArrayProc(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   tRecUse    *recUse = (tRecUse*)arg->atom.data.uVal;
   tRecData   *recData=NULL;
   tFldData   *fldData;
   tArrayData *arrayData;
   tArrayProc *arrayProc;
   tAtom      idx;
   long       index, dataLen=0;
   void       *address=NULL;

   arrayData = recUse->arrayProc->arrayUse->arrayData;
   for(arrayProc=recUse->arrayProc; arrayProc; arrayProc=arrayProc->next)  
   {  if(arrayData)
      {  ((tArg*)arrayProc->arrayUse->indexExpr)->func->EvalFunc(&idx, 
                     (tArg*)arrayProc->arrayUse->indexExpr, status, calcTab);
         index = NUM_GETLNG(idx.data.num);

         /* Make sure index is between start and end */
         if(arrayData->endIndex < arrayData->array->startIndex)
         {  
#ifdef DEVELOP
            fprintf(stderr, "There is no data for array '%s'.\n",
                               arrayData->array->name); 
#endif
            address = NULL;
            break;
         }
         else if(index > arrayData->endIndex)
         {
#ifdef DEVELOP
            fprintf(stderr, "Index '%ld' to array '%s' is greater "
                            "than defined end '%ld'.  Using index '%ld'\n", 
                               index,
                               arrayData->array->name, 
                               arrayData->endIndex, 
                               arrayData->endIndex); 
#endif

            index = arrayData->endIndex;
         }
         else if(index < arrayData->array->startIndex)
         {  
#ifdef DEVELOP
            fprintf(stderr, "Index '%ld' to array '%s' is less "
                            "than defined string '%ld'.  Using index '%ld'\n", 
                               index,
                               arrayData->array->name, 
                               arrayData->array->startIndex, 
                               arrayData->array->startIndex); 
#endif
 
            index = arrayData->array->startIndex;
         }

         index -= arrayData->array->startIndex;
        
         if(arrayData->array->type == RECORD_TYPE)
            recData = (tRecData*)arrayData->data[index];
         else 
         {  recData = NULL;
            if(arrayData->nonFixed)
            {  address = (void*)((tFlatData*)arrayData->data[index])->data;
               dataLen = ((tFlatData*)arrayData->data[index])->length;
            }
            else
            {  address = (void*)arrayData->data[index];
               dataLen = arrayData->array->elemSize;
            }
         }
      }
    
      if(recData && arrayProc->fldIndex != -1)
      {  fldData = recData->flds+arrayProc->fldIndex;
         if(fldData->flags&FIELD_NOT_HIT)
         {  address = NULL;
            break;
         }
         else if(fldData->fld->type == ARRAY_TYPE)
         {  arrayData = (tArrayData*)fldData->data;
            address = (void*)arrayData;
            recData = NULL;
         } 
         else if(fldData->fld->type == RECORD_TYPE)
         {  recData = (tRecData*)fldData->data;
            address = (void*)recData;
            arrayData = NULL;
         }
         else 
            address = (void*)((tFlatData*)fldData->data)->data;
      }
   }

   if(address==NULL)
   {  if(arg->flags&ATOM_STRING) 
      {  eval->data.strVal.str = NULL;
         eval->data.strVal.len = 0;
         eval->type = arg->atom.type;
      }
      else if(arg->flags&ATOM_REFERENCE)
      {  eval->data.uVal = NULL;
         eval->type = arg->atom.type;
      }
      else
      {  ZERO(eval->data.num);
         eval->type = NUMERIC_TYPE; 
      }
   } 
   else
   {  if(arg->flags&ATOM_STRING) 
      {  eval->data.strVal.str = (char*)address;
         eval->data.strVal.len = dataLen;
         eval->type = arg->atom.type;
      }
      else if(arg->flags&ATOM_REFERENCE)
      {  eval->data.uVal = address;
         eval->type = arg->atom.type;
      }
      else
      {  switch(arg->atom.type)
         {  case NUMERIC_TYPE:
               eval->data.num = *((tEngNum*)address);
               eval->type = NUMERIC_TYPE; 
               break;
            case HNC_FLOAT:
               NUM_SETFLT(eval->data.num, (*((float*)address)))
               eval->type = NUMERIC_TYPE; 
               break;
            case HNC_DOUBLE:
               NUM_SETDBL(eval->data.num, (*((double*)address)))
               eval->type = NUMERIC_TYPE; 
               break;
            case HNC_INT16:
               NUM_SETINT16(eval->data.num, (*((short*)address)))
               eval->type = NUMERIC_TYPE; 
               break;
            case HNC_UINT16:
               NUM_SETUINT16(eval->data.num, (*((unsigned short*)address)))
               eval->type = NUMERIC_TYPE; 
               break;
            case HNC_INT32:
               NUM_SETINT32(eval->data.num, (*((long*)address)))
               eval->type = NUMERIC_TYPE; 
               break;
            case HNC_UINT32:
               NUM_SETUINT32(eval->data.num, (*((unsigned long*)address)))
               eval->type = NUMERIC_TYPE; 
               break;
            default:
               eval->data.uVal = address;
               eval->type = arg->atom.type; 
         }
      }
   } 
}

void EvalResolve(tAtom *eval, tArg *calcArg, tREStatus *status, tCalcTable *calcTab)
{  
   long   resStat;
   tAtom  temp;
   char   *startAddr;
   tArg   *arg = ((tArgCalc*)calcArg->atom.data.uVal)->argList;

   /* Get the data address, if any */
   if(arg->next->next)
   {  arg->next->next->func->EvalFunc(&temp, arg->next->next, status, calcTab);
      *(((tRecData*)arg->next->atom.data.uVal)->data) =
                              (char*)NUM_GETLNG(temp.data.num);
   }
   
   /* Get the length */
   arg->func->EvalFunc(&temp, arg, status, calcTab);

   /* Save the starting address */
   startAddr = *((tRecData*)arg->next->atom.data.uVal)->data;

   /* Resolve the data */
   if((resStat=ResolveRecordAllLen((tRecData*)arg->next->atom.data.uVal, 
                                  ((tRecData*)arg->next->atom.data.uVal)->data,
                                  NUM_GETLNG(temp.data.num), status,
                                  calcTab->dynPool)))
   {  status->errCode = resStat;
      NUM_SETINT32(eval->data.num, resStat)
   }
   else
      ZERO(eval->data.num)
   
   /* Restore the starting address */
   *((tRecData*)arg->next->atom.data.uVal)->data = startAddr;

   eval->type = NUMERIC_TYPE;
}

void EvalExists(tAtom *eval, tArg *calcArg, tREStatus *status, tCalcTable *calcTab)
{  
   tAtom  temp;
   tArg   *arg = ((tArgCalc*)calcArg->atom.data.uVal)->argList;

   /* If the argument is ATOM_ARRAYPROC, it must be resolved to a field */
   /* If not, it already points to the flags, so just check to see if   */
   /* the FIELD_NOT_HIT flag is set                                     */
   if(arg->flags&ATOM_ARRAYPROC)
   {  EvalArrayProc(&temp, arg, status, calcTab);
      if(arg->flags&ATOM_STRING)
      {  if(temp.data.strVal.str)
            NUM_SETINT32(eval->data.num, 1L)
         else
            NUM_SETINT32(eval->data.num, 0L) 
      }
      else if(temp.data.uVal)
         NUM_SETINT32(eval->data.num, 1L) 
      else
         NUM_SETINT32(eval->data.num, 0L) 
   }
   else
      NUM_SETINT32(eval->data.num, (!(*((long*)arg->atom.data.uVal)&FIELD_NOT_HIT)))

   eval->type = NUMERIC_TYPE;
}

void EvalSizeof(tAtom *eval, tArg *calcArg, tREStatus *status, tCalcTable *calcTab)
{  
   tAtom  temp;
   tArg   *arg = ((tArgCalc*)calcArg->atom.data.uVal)->argList;

   /* Evaluate the argument */
   arg->func->EvalFunc(&temp, arg, status, calcTab);

   if(((tRecUse*)arg->atom.data.uVal)->type == ARRAY_TYPE)
   {  if(temp.data.uVal) 
         NUM_SETINT32(eval->data.num, ((tArrayData*)temp.data.uVal)->endIndex)
      else
         ZERO(eval->data.num)
   }
   else
      NUM_SETINT32(eval->data.num, temp.data.strVal.len)

   eval->type = NUMERIC_TYPE;
}


void EvalArrayVar(tAtom *eval, tArg *calcArg, tREStatus *status, tCalcTable *calcTab)
{  
   tAtom      temp;
   long       index;
   tArrayData *arrayData;
   tArg       *arg = ((tArgCalc*)calcArg->atom.data.uVal)->argList;


   /* Get the index */
   arg->next->func->EvalFunc(&temp, arg->next, status, calcTab);
   index = NUM_GETLNG(temp.data.num);

   /* Make sure index is between start and end */
   arrayData = (tArrayData*)arg->atom.data.uVal;
   if(arrayData->endIndex < arrayData->array->startIndex)
   {  
#ifdef DEVELOP
      fprintf(stderr, "There is no data for array '%s'.\n",
                         arrayData->array->name);
#endif
      index = -1;
   }
   else if(index < arrayData->array->startIndex) 
   {  
#ifdef DEVELOP
      fprintf(stderr, "Index '%ld' to array '%s' is less "
                      "than defined start '%ld'.  Using index '%ld'\n", 
                         index,
                         arrayData->array->name, 
                         arrayData->array->startIndex, 
                         arrayData->array->startIndex); 
#endif

      index = arrayData->array->startIndex;
   }
   else if(index > arrayData->endIndex)
   {  
#ifdef DEVELOP
      fprintf(stderr, "Index '%ld' to array '%s' is greater "
                      "than defined end '%ld'.  Using index '%ld'\n", 
                         index,
                         arrayData->array->name, 
                         arrayData->endIndex, 
                         arrayData->endIndex); 
#endif

      index = arrayData->endIndex;
   }
     
   /* Set the address in the third arg and evaluate it */
   if(index >= 0)
   {  arg->next->next->atom.data.uVal = 
                      (void*)arrayData->data[index - arrayData->array->startIndex];
      arg->next->next->func->EvalFunc(eval, arg->next->next, status, calcTab);
   }
}

void EvalStringIn(tAtom *eval, tArg *calcArg, tREStatus *status, tCalcTable *calcTab)
{  
   tAtom    left, right;
   int      len;
   tArg     *arg = ((tArgCalc*)calcArg->atom.data.uVal)->argList;

   /****************************************************/
   /* First argument is the key.  Subsequent arguments */
   /* are compared for matched to the key.  A match    */
   /* results in an eval of 1, otherwise, 0            */
   /****************************************************/
   arg->func->EvalFunc(&left, arg, status, calcTab);

   ZERO(eval->data.num);  /* Assume initial mismatch */
   eval->type = NUMERIC_TYPE;

   arg = arg->next;
   while(arg)
   {  arg->func->EvalFunc(&right, arg, status, calcTab);

      len = left.data.strVal.len < right.data.strVal.len ?
            left.data.strVal.len : right.data.strVal.len;
      if(!memcmp(left.data.strVal.str, right.data.strVal.str, len))
      {  ONE(eval->data.num);
         break;
      }

      arg = arg->next;
   }
}

void EvalNumericIn(tAtom *eval, tArg *calcArg, tREStatus *status, tCalcTable *calcTab)
{  
   tAtom    left, right;
   tArg     *arg = ((tArgCalc*)calcArg->atom.data.uVal)->argList;

   /****************************************************/
   /* First argument is the key.  Subsequent arguments */
   /* are compared for matched to the key.  A match    */
   /* results in an eval of 1, otherwise, 0            */
   /****************************************************/
   arg->func->EvalFunc(&left, arg, status, calcTab);

   ZERO(eval->data.num);  /* Assume initial mismatch */
   eval->type = NUMERIC_TYPE;

   arg = arg->next;
   while(arg)
   {  arg->func->EvalFunc(&right, arg, status, calcTab);

      if(IS_EQUAL(left.data.num, right.data.num))
      {  ONE(eval->data.num);
         break;
      }

      arg = arg->next;
   }
}

void EvalRule(tAtom *eval, tArg *calcArg, tREStatus *status, tCalcTable *calcTab)
{ 
   tRuleCalc   *ruleCalc;
   tRuleParam  *ruleParam;
   tAtom       temp;
   tArg        *arg = ((tArgCalc*)calcArg->atom.data.uVal)->argList;

   /**************************************************/
   /* The first argument is the rule statment calc.  */
   /* Save it until the parameters are evaluated and */
   /* put into place.  First, use it to set the arg  */
   /* list for the rule.                             */
   /**************************************************/
   ruleCalc = (tRuleCalc*)arg->atom.data.uVal;
   
   /*******************************************************/
   /* Evaluate the arguments putting the results into the */
   /* parameter variable data areas.                      */
   /*******************************************************/
   arg = arg->next;
   for(ruleParam=ruleCalc->paramList; ruleParam && arg; 
                  ruleParam=ruleParam->next, arg=arg->next)
   {  arg->func->EvalFunc(&temp, arg, status, calcTab);

      if(IsString(ruleParam->param->type))
      {  /* Just set the address of the string... no copy */
         ruleParam->param->data = temp.data.strVal.str;
         ruleParam->param->dataSize = temp.data.strVal.len;
      }
      else if(ruleParam->param->flags&VAR_REFERENCE)
         ruleParam->param->data = temp.data.uVal;
      else if(ruleParam->param->type == NUMERIC_TYPE)
         *((tEngNum*)ruleParam->param->data) = temp.data.num;
      else
         ruleParam->param->data = temp.data.uVal;
   }
   
   if(arg || ruleParam)
   {  status->errCode = FAIL_ENGINE|CALC_NODE|RULE_PARAM_MISMTCH;
      sprintf(status->errStr, "Rule parameter list doesn't match for "
                              "call to rule '%s'.  " 
                              "Failed for rule '%s' in rule base '%s'", 
                                    ruleCalc->id,
                                    calcTab->currentRuleCalc->id, 
                                    calcTab->ruleBaseData->ruleBaseName);
      throw(ENGINE_EXCEPTION);
   }

   /*************************************************/
   /* Now, evaluate the statement list for the rule */
   /*************************************************/
   CalcRuleStatements(ruleCalc, status, calcTab);
   *eval = calcTab->retValue;
 
   /*******************************************************/
   /* If found RETURN, then keep going because RETURN was */
   /* for rule that was called, not thisArg rule             */
   /*******************************************************/
   if(status->errCode==RETURN_FOUND) status->errCode = OK;
}

void EvalRBExec(tAtom *eval, tArg *calcArg, tREStatus *status, tCalcTable *calcTab)
{  
   tArg     *arg = ((tArgCalc*)calcArg->atom.data.uVal)->argList;

   /************************************************************************/
   /* Only argument is a tEngRBData structure, which has the handle to the */
   /* rule base that is to be executed.                                    */
   /************************************************************************/
   try
      NUM_SETINT32(eval->data.num,
         ExecuteRuleBase((void*)arg->atom.data.uVal))
   catch(ENGINE_EXCEPTION)
   {  if(GetEngineExceptionType((tRuleBaseData*)arg->next->atom.data.uVal)==1)
         throw(ENGINE_EXCEPTION);
   }
   endcatches;

   eval->type = NUMERIC_TYPE;
}
   
void EvalLinkedFunc(tAtom *eval, tArg *calcArg, tREStatus *status, tCalcTable *calcTab)
{  
   tArg           *arg;
   int            numArgs=0;
   tAtom          *temp, argList[MAX_ARGS];
   tFuncCalc      *funcCalc = (tFuncCalc*)calcArg->atom.data.uVal;
   tFuncCallData  funcCallData;

   arg = funcCalc->argList;
   while(arg)
   {  temp = argList+numArgs;
      arg->func->EvalFunc(temp, arg, status, calcTab);
      arg = arg->next;
      numArgs++;
   }

   funcCallData.funcData = funcCalc->funcData->funcData; 
   funcCallData.inputData = funcCalc->funcData->inputData; 
   funcCallData.errStatus.errCode = OK;
   funcCallData.errStatus.errStr = status->errStr;
   funcCallData.ruleData = funcCalc->ruleData;
   funcCallData.ruleBaseData = calcTab->ruleBaseData;
   *eval = funcCalc->funcData->FuncPtr(&funcCallData, argList, numArgs);
   
   if(funcCallData.errStatus.errCode != OK)
   {  char *chPtr;
  
      status->errCode = funcCallData.errStatus.errCode;
      status->errStr = funcCallData.errStatus.errStr;
      chPtr = status->errStr+strlen(status->errStr);
      sprintf(chPtr, "  Failed for rule '%s' in rule base '%s'",
                     calcTab->currentRuleCalc->id, calcTab->ruleBaseData->ruleBaseName);
      if(notWarning(status->errCode))
         throw(ENGINE_EXCEPTION);
   }
}

void EvalNumericVarCalc(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   tVarCalc  *varCalc = (tVarCalc*)arg->atom.data.uVal;

   if(*varCalc->var->calcRule->calcDone)
   {  eval->data.num = *((tEngNum*)varCalc->var->data);
      eval->type = NUMERIC_TYPE;
   }
   else
   {  *varCalc->var->calcRule->calcDone = 1;
      varCalc->arg->func->EvalFunc(eval, varCalc->arg, status, calcTab);
   }

   if(status->errCode == RETURN_FOUND) status->errCode = 0;
}

void EvalFloatVarCalc(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   tVarCalc  *varCalc = (tVarCalc*)arg->atom.data.uVal;

   if(*varCalc->var->calcRule->calcDone)
   {  NUM_SETFLT(eval->data.num, (*((float*)varCalc->var->data)))
      eval->type = NUMERIC_TYPE;
   }
   else
   {  *varCalc->var->calcRule->calcDone = 1;
      varCalc->arg->func->EvalFunc(eval, varCalc->arg, status, calcTab);
   }

   if(status->errCode == RETURN_FOUND) status->errCode = 0;
}

void EvalDoubleVarCalc(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   tVarCalc  *varCalc = (tVarCalc*)arg->atom.data.uVal;

   if(*varCalc->var->calcRule->calcDone)
   {  NUM_SETDBL(eval->data.num, (*((double*)varCalc->var->data)))
      eval->type = NUMERIC_TYPE;
   }
   else
   {  *varCalc->var->calcRule->calcDone = 1;
      varCalc->arg->func->EvalFunc(eval, varCalc->arg, status, calcTab);
   }

   if(status->errCode == RETURN_FOUND) status->errCode = 0;
}

void EvalShortVarCalc(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   tVarCalc  *varCalc = (tVarCalc*)arg->atom.data.uVal;

   if(*varCalc->var->calcRule->calcDone)
   {  NUM_SETINT16(eval->data.num, (*((short*)varCalc->var->data)))
      eval->type = NUMERIC_TYPE;
   }
   else
   {  *varCalc->var->calcRule->calcDone = 1;
      varCalc->arg->func->EvalFunc(eval, varCalc->arg, status, calcTab);
   }

   if(status->errCode == RETURN_FOUND) status->errCode = 0;
}

void EvalUShortVarCalc(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   tVarCalc  *varCalc = (tVarCalc*)arg->atom.data.uVal;

   if(*varCalc->var->calcRule->calcDone)
   {  NUM_SETUINT16(eval->data.num, (*((unsigned short*)varCalc->var->data)))
      eval->type = NUMERIC_TYPE;
   }
   else
   {  *varCalc->var->calcRule->calcDone = 1;
      varCalc->arg->func->EvalFunc(eval, varCalc->arg, status, calcTab);
   }

   if(status->errCode == RETURN_FOUND) status->errCode = 0;
}

void EvalLongVarCalc(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   tVarCalc  *varCalc = (tVarCalc*)arg->atom.data.uVal;

   if(*varCalc->var->calcRule->calcDone)
   {  NUM_SETINT32(eval->data.num, (*((long*)varCalc->var->data)))
      eval->type = NUMERIC_TYPE;
   }
   else
   {  *varCalc->var->calcRule->calcDone = 1;
      varCalc->arg->func->EvalFunc(eval, varCalc->arg, status, calcTab);
   }

   if(status->errCode == RETURN_FOUND) status->errCode = 0;
}

void EvalULongVarCalc(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   tVarCalc  *varCalc = (tVarCalc*)arg->atom.data.uVal;

   if(*varCalc->var->calcRule->calcDone)
   {  NUM_SETUINT32(eval->data.num, (*((unsigned long*)varCalc->var->data)))
      eval->type = NUMERIC_TYPE;
   }
   else
   {  *varCalc->var->calcRule->calcDone = 1;
      varCalc->arg->func->EvalFunc(eval, varCalc->arg, status, calcTab);
   }

   if(status->errCode == RETURN_FOUND) status->errCode = 0;
}

void EvalStr2Num(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   tAtom         temp;
   tStr2NumCalc  *str2numCalc = (tStr2NumCalc*)arg->atom.data.uVal;

   if(!(*str2numCalc->calcDone))
   {  str2numCalc->arg.func->EvalFunc(&temp, &str2numCalc->arg, status, calcTab);  
 
      NUM_SETSTR(&str2numCalc->eval.data.num, 
                      temp.data.strVal.str, temp.data.strVal.len);

      str2numCalc->eval.type = NUMERIC_TYPE;
      *str2numCalc->calcDone = 1;
   }

   *eval = str2numCalc->eval;
}




void EvalArrayProcAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{
   EvalArrayProc(eval, arg, status, calcTab);
  
   if(arg->flags&ATOM_NOT)
      NUM_NOT(eval->data.num)
   else if(arg->flags&ATOM_NEGATE)
      NUM_NEG(eval->data.num)
}

void EvalArrayVarAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{
   EvalArrayVar(eval, arg, status, calcTab);

   if(arg->flags&ATOM_NOT)
      NUM_NOT(eval->data.num)
   else if(arg->flags&ATOM_NEGATE)
      NUM_NEG(eval->data.num)
}

void EvalResolveAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{
   EvalResolve(eval, arg, status, calcTab);
  
   if(arg->flags&ATOM_NOT)
      NUM_NOT(eval->data.num)
   else if(arg->flags&ATOM_NEGATE)
      NUM_NEG(eval->data.num)
}

void EvalErrorNumAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{
   EvalErrorNum(eval, arg, status, calcTab);
  
   if(arg->flags&ATOM_NOT)
      NUM_NOT(eval->data.num)
   else if(arg->flags&ATOM_NEGATE)
      NUM_NEG(eval->data.num)
}

void EvalExistsAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{
   EvalExists(eval, arg, status, calcTab);
  
   if(arg->flags&ATOM_NOT)
      NUM_NOT(eval->data.num)
   else if(arg->flags&ATOM_NEGATE)
      NUM_NEG(eval->data.num)
}

void EvalSizeofAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{
   EvalSizeof(eval, arg, status, calcTab);
  
   if(arg->flags&ATOM_NOT)
      NUM_NOT(eval->data.num)
   else if(arg->flags&ATOM_NEGATE)
      NUM_NEG(eval->data.num)
}

void EvalRBExecAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{
   EvalRBExec(eval, arg, status, calcTab);
  
   if(arg->flags&ATOM_NOT)
      NUM_NOT(eval->data.num)
   else if(arg->flags&ATOM_NEGATE)
      NUM_NEG(eval->data.num)
}

void EvalLinkedFuncAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{
   EvalLinkedFunc(eval, arg, status, calcTab);
  
   if(arg->flags&ATOM_NOT)
      NUM_NOT(eval->data.num)
   else if(arg->flags&ATOM_NEGATE)
      NUM_NEG(eval->data.num)
}

void EvalRuleAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{
   EvalRule(eval, arg, status, calcTab);
  
   if(arg->flags&ATOM_NOT)
      NUM_NOT(eval->data.num)
   else if(arg->flags&ATOM_NEGATE)
      NUM_NEG(eval->data.num)
}

void EvalStringInAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{
   EvalStringIn(eval, arg, status, calcTab);
  
   if(arg->flags&ATOM_NOT)
      NUM_NOT(eval->data.num)
   else if(arg->flags&ATOM_NEGATE)
      NUM_NEG(eval->data.num)
}

void EvalNumericInAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{
   EvalNumericIn(eval, arg, status, calcTab);
  
   if(arg->flags&ATOM_NOT)
      NUM_NOT(eval->data.num)
   else if(arg->flags&ATOM_NEGATE)
      NUM_NEG(eval->data.num)
}

void EvalStr2NumAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{
   EvalStr2Num(eval, arg, status, calcTab); 

   if(arg->flags&ATOM_NOT)
      NUM_NOT(eval->data.num)
   else if(arg->flags&ATOM_NEGATE)
      NUM_NEG(eval->data.num)
}

void EvalNumericVarCalcAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{
   EvalNumericVarCalc(eval, arg, status, calcTab);
  
   if(arg->flags&ATOM_NOT)
      NUM_NOT(eval->data.num)
   else if(arg->flags&ATOM_NEGATE)
      NUM_NEG(eval->data.num)
}

void EvalFloatVarCalcAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{
   EvalFloatVarCalc(eval, arg, status, calcTab);
  
   if(arg->flags&ATOM_NOT)
      NUM_NOT(eval->data.num)
   else if(arg->flags&ATOM_NEGATE)
      NUM_NEG(eval->data.num)
}

void EvalDoubleVarCalcAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{
   EvalDoubleVarCalc(eval, arg, status, calcTab);
  
   if(arg->flags&ATOM_NOT)
      NUM_NOT(eval->data.num)
   else if(arg->flags&ATOM_NEGATE)
      NUM_NEG(eval->data.num)
}

void EvalShortVarCalcAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{
   EvalShortVarCalc(eval, arg, status, calcTab);
  
   if(arg->flags&ATOM_NOT)
      NUM_NOT(eval->data.num)
   else if(arg->flags&ATOM_NEGATE)
      NUM_NEG(eval->data.num)
}

void EvalUShortVarCalcAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{
   EvalUShortVarCalc(eval, arg, status, calcTab);
  
   if(arg->flags&ATOM_NOT)
      NUM_NOT(eval->data.num)
   else if(arg->flags&ATOM_NEGATE)
      NUM_NEG(eval->data.num)
}

void EvalLongVarCalcAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{
   EvalLongVarCalc(eval, arg, status, calcTab);
  
   if(arg->flags&ATOM_NOT)
      NUM_NOT(eval->data.num)
   else if(arg->flags&ATOM_NEGATE)
      NUM_NEG(eval->data.num)
}

void EvalULongVarCalcAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{
   EvalULongVarCalc(eval, arg, status, calcTab);
  
   if(arg->flags&ATOM_NOT)
      NUM_NOT(eval->data.num)
   else if(arg->flags&ATOM_NEGATE)
      NUM_NEG(eval->data.num)
}





void EvalNumericParamNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   tEngNum num = *((tEngNum*)((tEngVar*)arg->atom.data.uVal)->data);

   NUM_NEG(num);

   eval->data.num = num;
   eval->type = NUMERIC_TYPE;
}

void EvalFloatParamNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETFLT(eval->data.num, (-(*((float*)((tEngVar*)arg->atom.data.uVal)->data))))
   eval->type = NUMERIC_TYPE; 
}

void EvalDoubleParamNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETDBL(eval->data.num, (-(*((double*)((tEngVar*)arg->atom.data.uVal)->data))))
   eval->type = NUMERIC_TYPE; 
}

void EvalShortParamNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETINT16(eval->data.num, (-(*((short*)((tEngVar*)arg->atom.data.uVal)->data))))
   eval->type = NUMERIC_TYPE; 
}

void EvalUShortParamNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETUINT16(eval->data.num, 
            (-(*((unsigned short*)((tEngVar*)arg->atom.data.uVal)->data))))
   eval->type = NUMERIC_TYPE; 
}

void EvalLongParamNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETINT32(eval->data.num, 
                  (-(*((long*)((tEngVar*)arg->atom.data.uVal)->data))))
   eval->type = NUMERIC_TYPE; 
}

void EvalULongParamNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETUINT32(eval->data.num, 
            (-(*((unsigned long*)((tEngVar*)arg->atom.data.uVal)->data))))
   eval->type = NUMERIC_TYPE; 
}

void EvalNumericAddrNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   tEngNum num = *((tEngNum*)arg->atom.data.uVal);

   NUM_NEG(num);
   eval->data.num = num;

   eval->type = NUMERIC_TYPE;
}

void EvalFloatAddrNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETFLT(eval->data.num, (-(*((float*)arg->atom.data.uVal))))
   eval->type = NUMERIC_TYPE; 
}

void EvalDoubleAddrNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETDBL(eval->data.num, (-(*((double*)arg->atom.data.uVal))))
   eval->type = NUMERIC_TYPE; 
}

void EvalShortAddrNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETINT16(eval->data.num, (-(*((short*)arg->atom.data.uVal))))
   eval->type = NUMERIC_TYPE; 
}

void EvalUShortAddrNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETUINT16(eval->data.num, (-(*((unsigned short*)arg->atom.data.uVal))))
   eval->type = NUMERIC_TYPE; 
}

void EvalLongAddrNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETINT32(eval->data.num, (-(*((long*)arg->atom.data.uVal))))
   eval->type = NUMERIC_TYPE; 
}

void EvalULongAddrNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETUINT32(eval->data.num, (-(*((unsigned long*)arg->atom.data.uVal))))
   eval->type = NUMERIC_TYPE; 
}

void EvalNumericFldNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   tEngNum num = *((tEngNum*)((tFlatData*)arg->atom.data.uVal)->data);

   NUM_NEG(num);

   eval->data.num = num;
   eval->type = NUMERIC_TYPE;
}

void EvalFloatFldNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETFLT(eval->data.num, (-(*((float*)((tFlatData*)arg->atom.data.uVal)->data))))
   eval->type = NUMERIC_TYPE; 
}

void EvalDoubleFldNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETDBL(eval->data.num, (-(*((double*)((tFlatData*)arg->atom.data.uVal)->data))))
   eval->type = NUMERIC_TYPE; 
}

void EvalShortFldNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETINT16(eval->data.num, (-(*((short*)((tFlatData*)arg->atom.data.uVal)->data))))
   eval->type = NUMERIC_TYPE; 
}

void EvalUShortFldNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{
   NUM_SETUINT16(eval->data.num, 
              (-(*((unsigned short*)((tFlatData*)arg->atom.data.uVal)->data))))
   eval->type = NUMERIC_TYPE; 
}

void EvalLongFldNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{
   NUM_SETINT32(eval->data.num, (-(*((long*)((tFlatData*)arg->atom.data.uVal)->data))))
   eval->type = NUMERIC_TYPE; 
}

void EvalULongFldNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{
   NUM_SETUINT32(eval->data.num, 
                   (-(*((unsigned long*)((tFlatData*)arg->atom.data.uVal)->data))))
   eval->type = NUMERIC_TYPE; 
}




void EvalNumericParamNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   tEngNum num = *((tEngNum*)((tEngVar*)arg->atom.data.uVal)->data);

   NUM_NOT(num);

   eval->data.num = num;
   eval->type = NUMERIC_TYPE;
}

void EvalFloatParamNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETFLT(eval->data.num, (!(*((float*)((tEngVar*)arg->atom.data.uVal)->data))))
   eval->type = NUMERIC_TYPE; 
}

void EvalDoubleParamNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETDBL(eval->data.num, (!(*((double*)((tEngVar*)arg->atom.data.uVal)->data))))
   eval->type = NUMERIC_TYPE; 
}

void EvalShortParamNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETINT16(eval->data.num, (!(*((short*)((tEngVar*)arg->atom.data.uVal)->data))))
   eval->type = NUMERIC_TYPE; 
}

void EvalUShortParamNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETUINT16(eval->data.num, 
              (!(*((unsigned short*)((tEngVar*)arg->atom.data.uVal)->data))))
   eval->type = NUMERIC_TYPE; 
}

void EvalLongParamNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETINT32(eval->data.num, (!(*((long*)((tEngVar*)arg->atom.data.uVal)->data))))
   eval->type = NUMERIC_TYPE; 
}

void EvalULongParamNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETUINT32(eval->data.num, 
               (!(*((unsigned long*)((tEngVar*)arg->atom.data.uVal)->data))))
   eval->type = NUMERIC_TYPE; 
}

void EvalNumericAddrNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   tEngNum num = *((tEngNum*)arg->atom.data.uVal);

   NUM_NOT(num);

   eval->data.num = num;
   eval->type = NUMERIC_TYPE;
}

void EvalFloatAddrNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETFLT(eval->data.num, (!(*((float*)arg->atom.data.uVal))))
   eval->type = NUMERIC_TYPE; 
}

void EvalDoubleAddrNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETDBL(eval->data.num, (!(*((double*)arg->atom.data.uVal))))
   eval->type = NUMERIC_TYPE; 
}

void EvalShortAddrNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETINT16(eval->data.num, (!(*((short*)arg->atom.data.uVal))))
   eval->type = NUMERIC_TYPE; 
}

void EvalUShortAddrNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETUINT16(eval->data.num, (!(*((unsigned short*)arg->atom.data.uVal))))
   eval->type = NUMERIC_TYPE; 
}

void EvalLongAddrNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETINT32(eval->data.num, (!(*((long*)arg->atom.data.uVal))))
   eval->type = NUMERIC_TYPE; 
}

void EvalULongAddrNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETUINT32(eval->data.num, (!(*((unsigned long*)arg->atom.data.uVal))))
   eval->type = NUMERIC_TYPE; 
}

void EvalNumericFldNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   tEngNum num = *((tEngNum*)((tFlatData*)arg->atom.data.uVal)->data);

   NUM_NOT(num);

   eval->data.num = num;
   eval->type = NUMERIC_TYPE;
}

void EvalFloatFldNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETFLT(eval->data.num, (!(*((float*)((tFlatData*)arg->atom.data.uVal)->data))))
   eval->type = NUMERIC_TYPE; 
}

void EvalDoubleFldNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETDBL(eval->data.num, (!(*((double*)((tFlatData*)arg->atom.data.uVal)->data))))
   eval->type = NUMERIC_TYPE; 
}

void EvalShortFldNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETINT16(eval->data.num, (!(*((short*)((tFlatData*)arg->atom.data.uVal)->data))))
   eval->type = NUMERIC_TYPE; 
}

void EvalUShortFldNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{
   NUM_SETUINT16(eval->data.num, 
                  (!(*((unsigned short*)((tFlatData*)arg->atom.data.uVal)->data))))
   eval->type = NUMERIC_TYPE; 
}

void EvalLongFldNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{
   NUM_SETINT32(eval->data.num, (!(*((long*)((tFlatData*)arg->atom.data.uVal)->data))))
   eval->type = NUMERIC_TYPE; 
}

void EvalULongFldNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{
   NUM_SETUINT32(eval->data.num, 
                 (!(*((unsigned long*)((tFlatData*)arg->atom.data.uVal)->data))))
   eval->type = NUMERIC_TYPE; 
}



void EvalNumericParam(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   eval->data.num = *((tEngNum*)((tEngVar*)arg->atom.data.uVal)->data);
   eval->type = NUMERIC_TYPE;
}

void EvalFloatParam(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETFLT(eval->data.num, (*((float*)((tEngVar*)arg->atom.data.uVal)->data)))
   eval->type = NUMERIC_TYPE; 
}

void EvalDoubleParam(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETDBL(eval->data.num, (*((double*)((tEngVar*)arg->atom.data.uVal)->data)))
   eval->type = NUMERIC_TYPE; 
}

void EvalShortParam(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETINT16(eval->data.num, (*((short*)((tEngVar*)arg->atom.data.uVal)->data)))
   eval->type = NUMERIC_TYPE; 
}

void EvalUShortParam(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETUINT16(eval->data.num, (*((unsigned short*)((tEngVar*)arg->atom.data.uVal)->data)))
   eval->type = NUMERIC_TYPE; 
}

void EvalLongParam(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETINT32(eval->data.num, (*((long*)((tEngVar*)arg->atom.data.uVal)->data)))
   eval->type = NUMERIC_TYPE; 
}

void EvalULongParam(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETUINT32(eval->data.num, (*((unsigned long*)((tEngVar*)arg->atom.data.uVal)->data)))
   eval->type = NUMERIC_TYPE; 
}

void EvalNumericAddr(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   eval->data.num = *((tEngNum*)arg->atom.data.uVal);
   eval->type = NUMERIC_TYPE;
}

void EvalFloatAddr(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETFLT(eval->data.num, (*((float*)arg->atom.data.uVal)))
   eval->type = NUMERIC_TYPE; 
}

void EvalDoubleAddr(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETDBL(eval->data.num, (*((double*)arg->atom.data.uVal)))
   eval->type = NUMERIC_TYPE; 
}

void EvalShortAddr(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETINT16(eval->data.num, (*((short*)arg->atom.data.uVal)))
   eval->type = NUMERIC_TYPE; 
}

void EvalUShortAddr(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETUINT16(eval->data.num, (*((unsigned short*)arg->atom.data.uVal)))
   eval->type = NUMERIC_TYPE; 
}

void EvalLongAddr(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETINT32(eval->data.num, (*((long*)arg->atom.data.uVal)))
   eval->type = NUMERIC_TYPE; 
}

void EvalULongAddr(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETUINT32(eval->data.num, (*((unsigned long*)arg->atom.data.uVal)))
   eval->type = NUMERIC_TYPE; 
}

void EvalNumericFld(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   eval->data.num = *((tEngNum*)((tFlatData*)arg->atom.data.uVal)->data);
   eval->type = NUMERIC_TYPE;
}

void EvalFloatFld(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETFLT(eval->data.num, (*((float*)((tFlatData*)arg->atom.data.uVal)->data)))
   eval->type = NUMERIC_TYPE; 
}

void EvalDoubleFld(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETDBL(eval->data.num, (*((double*)((tFlatData*)arg->atom.data.uVal)->data)))
   eval->type = NUMERIC_TYPE; 
}

void EvalShortFld(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   NUM_SETINT16(eval->data.num, (*((short*)((tFlatData*)arg->atom.data.uVal)->data)))
   eval->type = NUMERIC_TYPE; 
}

void EvalUShortFld(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{
   NUM_SETUINT16(eval->data.num, (*((unsigned short*)((tFlatData*)arg->atom.data.uVal)->data)))
   eval->type = NUMERIC_TYPE; 
}

void EvalLongFld(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{
   NUM_SETINT32(eval->data.num, (*((long*)((tFlatData*)arg->atom.data.uVal)->data)))
   eval->type = NUMERIC_TYPE; 
}

void EvalULongFld(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{
   NUM_SETUINT32(eval->data.num, (*((unsigned long*)((tFlatData*)arg->atom.data.uVal)->data)))
   eval->type = NUMERIC_TYPE; 
}


static tArgFunc argFuncs[] = { 
   { "EvalErrorStr", EvalErrorStr },
   { "EvalVarCalcData ", EvalVarCalcData },
   { "EvalStringVarCalc ", EvalStringVarCalc },
   { "EvalFldString", EvalFldString },
   { "EvalFldData", EvalFldData },
   { "EvalAsIs", EvalAsIs },
   { "EvalParamData", EvalParamData },
   { "EvalStringParam", EvalStringParam },
   { "EvalArrayProc", EvalArrayProc },
   { "EvalResolve", EvalResolve },
   { "EvalErrorNum", EvalErrorNum },
   { "EvalExists", EvalExists },
   { "EvalSizeof", EvalSizeof },
   { "EvalRBExec", EvalRBExec },
   { "EvalLinkedFunc", EvalLinkedFunc },
   { "EvalRule", EvalRule },
   { "EvalStringIn", EvalStringIn },
   { "EvalNumericIn", EvalNumericIn },
   { "EvalArrayVar", EvalArrayVar },
   { "EvalStr2Num", EvalStr2Num },
   { "EvalNumericVarCalc ", EvalNumericVarCalc },
   { "EvalFloatVarCalc ", EvalFloatVarCalc },
   { "EvalDoubleVarCalc ", EvalDoubleVarCalc },
   { "EvalShortVarCalc ", EvalShortVarCalc },
   { "EvalUShortVarCalc ", EvalUShortVarCalc },
   { "EvalLongVarCalc ", EvalLongVarCalc },
   { "EvalULongVarCalc ", EvalULongVarCalc },
   { "EvalNumericParam", EvalNumericParam },
   { "EvalFloatParam", EvalFloatParam },
   { "EvalDoubleParam", EvalDoubleParam },
   { "EvalShortParam", EvalShortParam },
   { "EvalUShortParam", EvalUShortParam },
   { "EvalLongParam", EvalLongParam },
   { "EvalULongParam", EvalULongParam },
   { "EvalNumericAddr", EvalNumericAddr },
   { "EvalFloatAddr", EvalFloatAddr },
   { "EvalDoubleAddr", EvalDoubleAddr },
   { "EvalShortAddr", EvalShortAddr },
   { "EvalUShortAddr", EvalUShortAddr },
   { "EvalLongAddr", EvalLongAddr },
   { "EvalULongAddr", EvalULongAddr },
   { "EvalNumericFld", EvalNumericFld },
   { "EvalFloatFld", EvalFloatFld },
   { "EvalDoubleFld", EvalDoubleFld },
   { "EvalShortFld", EvalShortFld },
   { "EvalUShortFld", EvalUShortFld },
   { "EvalLongFld", EvalLongFld },
   { "EvalULongFld", EvalULongFld },
   { "EvalArrayProcAdj", EvalArrayProcAdj },
   { "EvalResolveAdj", EvalResolveAdj },
   { "EvalErrorNumAdj", EvalErrorNumAdj },
   { "EvalExistsAdj", EvalExistsAdj },
   { "EvalSizeofAdj", EvalSizeofAdj },
   { "EvalRBExecAdj", EvalRBExecAdj },
   { "EvalLinkedFuncAdj", EvalLinkedFuncAdj },
   { "EvalRuleAdj", EvalRuleAdj },
   { "EvalStringInAdj", EvalStringInAdj },
   { "EvalNumericInAdj", EvalNumericInAdj },
   { "EvalArrayVarAdj", EvalArrayVarAdj },
   { "EvalStr2NumAdj", EvalStr2NumAdj },
   { "EvalNumericVarCalcAdj ", EvalNumericVarCalcAdj },
   { "EvalFloatVarCalcAdj ", EvalFloatVarCalcAdj },
   { "EvalDoubleVarCalcAdj ", EvalDoubleVarCalcAdj },
   { "EvalShortVarCalcAdj ", EvalShortVarCalcAdj },
   { "EvalUShortVarCalcAdj ", EvalUShortVarCalcAdj },
   { "EvalLongVarCalcAdj ", EvalLongVarCalcAdj },
   { "EvalULongVarCalcAdj ", EvalULongVarCalcAdj },
   { "EvalNumericParamNot", EvalNumericParamNot },
   { "EvalFloatParamNot", EvalFloatParamNot },
   { "EvalDoubleParamNot", EvalDoubleParamNot },
   { "EvalShortParamNot", EvalShortParamNot },
   { "EvalUShortParamNot", EvalUShortParamNot },
   { "EvalLongParamNot", EvalLongParamNot },
   { "EvalULongParamNot", EvalULongParamNot },
   { "EvalNumericAddrNot", EvalNumericAddrNot },
   { "EvalFloatAddrNot", EvalFloatAddrNot },
   { "EvalDoubleAddrNot", EvalDoubleAddrNot },
   { "EvalShortAddrNot", EvalShortAddrNot },
   { "EvalUShortAddrNot", EvalUShortAddrNot },
   { "EvalLongAddrNot", EvalLongAddrNot },
   { "EvalULongAddrNot", EvalULongAddrNot },
   { "EvalNumericFldNot", EvalNumericFldNot },
   { "EvalFloatFldNot", EvalFloatFldNot },
   { "EvalDoubleFldNot", EvalDoubleFldNot },
   { "EvalShortFldNot", EvalShortFldNot },
   { "EvalUShortFldNot", EvalUShortFldNot },
   { "EvalLongFldNot", EvalLongFldNot },
   { "EvalULongFldNot", EvalULongFldNot },
   { "EvalNumericParamNeg", EvalNumericParamNeg },
   { "EvalFloatParamNeg", EvalFloatParamNeg },
   { "EvalDoubleParamNeg", EvalDoubleParamNeg },
   { "EvalShortParamNeg", EvalShortParamNeg },
   { "EvalUShortParamNeg", EvalUShortParamNeg },
   { "EvalLongParamNeg", EvalLongParamNeg },
   { "EvalULongParamNeg", EvalULongParamNeg },
   { "EvalNumericAddrNeg", EvalNumericAddrNeg },
   { "EvalFloatAddrNeg", EvalFloatAddrNeg },
   { "EvalDoubleAddrNeg", EvalDoubleAddrNeg },
   { "EvalShortAddrNeg", EvalShortAddrNeg },
   { "EvalUShortAddrNeg", EvalUShortAddrNeg },
   { "EvalLongAddrNeg", EvalLongAddrNeg },
   { "EvalULongAddrNeg", EvalULongAddrNeg },
   { "EvalNumericFldNeg", EvalNumericFldNeg },
   { "EvalFloatFldNeg", EvalFloatFldNeg },
   { "EvalDoubleFldNeg", EvalDoubleFldNeg },
   { "EvalShortFldNeg", EvalShortFldNeg },
   { "EvalUShortFldNeg", EvalUShortFldNeg },
   { "EvalLongFldNeg", EvalLongFldNeg },
   { "EvalULongFldNeg", EvalULongFldNeg },
   { "EvalNumIncrement", EvalNumIncrement },
   { "EvalNumIPADD", EvalNumIPADD },
   { "EvalNumIPSUB", EvalNumIPSUB },
   { "EvalNumIPMUL", EvalNumIPMUL },
   { "EvalNumIPDIV", EvalNumIPDIV },
   { "EvalFloatIncrement", EvalFloatIncrement },
   { "EvalFloatIPADD", EvalFloatIPADD },
   { "EvalFloatIPSUB", EvalFloatIPSUB },
   { "EvalFloatIPMUL", EvalFloatIPMUL },
   { "EvalFloatIPDIV", EvalFloatIPDIV },
   { "EvalLongIncrement", EvalLongIncrement },
   { "EvalLongIPADD", EvalLongIPADD },
   { "EvalLongIPSUB", EvalLongIPSUB },
   { "EvalLongIPMUL", EvalLongIPMUL },
   { "EvalLongIPDIV", EvalLongIPDIV },
   { "EvalReturn", EvalReturn },
   { "EvalTerminate", EvalTerminate },
   { "EvalIf", EvalIf },
   { "EvalVarIf", EvalVarIf },
   { "EvalStringCase", EvalStringCase },
   { "EvalNumericCase", EvalNumericCase },
   { "EvalFor", EvalFor },
   { "EvalWhile", EvalWhile },
   { "CalcStatements", CalcStatements },
   { "CalcVarStatements", CalcVarStatements },
   { "EvalQuickFloatExpr", EvalQuickFloatExpr },
   { "EvalMixedFloatExpr", EvalMixedFloatExpr },
   { "EvalQuickLongExpr", EvalQuickLongExpr },
   { "EvalMixedLongExpr", EvalMixedLongExpr },
   { "EvalQuickNumericExpr", EvalQuickNumericExpr },
   { "EvalMixedNumericExpr", EvalMixedNumericExpr },
   { "EvalStringNumExpr", EvalStringNumExpr },
   { "EvalStringExpr", EvalStringExpr },
   { "EvalMixedExpr", EvalMixedExpr },
   { "EvalSgl_NumVarCalc", EvalSgl_NumVarCalc },
   { "EvalSgl_StrVarCalc", EvalSgl_StrVarCalc },
   { "EvalSgl_OthVarCalc", EvalSgl_OthVarCalc },
   { "EvalSgl_LngVarCalc", EvalSgl_LngVarCalc },
   { "EvalSgl_FltVarCalc", EvalSgl_FltVarCalc }
};


long CreateArgFuncArray(tCalcTable *calcTab)
{
   int i, numFuncs = sizeof(argFuncs) / sizeof(tArgFunc);

   /* Allocate the array of tArgFunc structures */
   if((calcTab->argFuncArray = 
               (tArgFunc*)MyMalloc(numFuncs * sizeof(tArgFunc)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)calcTab->argFuncArray, 0, numFuncs * sizeof(tArgFunc));

   for(i=0; i<numFuncs; i++)
   {  calcTab->argFuncArray[i].funcName = 
              (char*)MyMalloc(strlen(argFuncs[i].funcName)+1);
      strcpy(calcTab->argFuncArray[i].funcName, argFuncs[i].funcName);
      calcTab->argFuncArray[i].EvalFunc = argFuncs[i].EvalFunc;
   }

   calcTab->numArgFuncs = numFuncs;

   return(OK);
}

long ResetArgFuncs(void *handle)
{
   tCalcTable *calcTab = (tCalcTable*)handle;
   int        i;

   for(i=0; i<calcTab->numArgFuncs; i++)
      calcTab->argFuncArray[i].EvalFunc = argFuncs[i].EvalFunc;

   return(OK);
}

tArgFunc* GetArgFunc(tCalcTable *calcTab, tfArgEval EvalFunc)
{
   int i;
 
   /* Find the matching function */
   for(i=0; i<calcTab->numArgFuncs; i++)
      if(EvalFunc == calcTab->argFuncArray[i].EvalFunc)
         break;

   /* If function not found, print an error */
   if(i==calcTab->numArgFuncs)
   {  fprintf(stderr, "Can't find function\n");
      return(NULL);
   }

   return(calcTab->argFuncArray+i);
}

tArgFunc* GetArgFuncName(tCalcTable *calcTab, char *funcName)
{
   int i;
 
   /* Find the matching function */
   for(i=0; i<calcTab->numArgFuncs; i++)
      if(!strcmp(calcTab->argFuncArray[i].funcName, funcName))
         break;

   /* If function not found, print an error */
   if(i==calcTab->numArgFuncs)
   {  fprintf(stderr, "Can't find function '%s'\n", funcName);
      return(NULL);
   }

   return(calcTab->argFuncArray+i);
}
