/*
** $Id: typechk.c,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: typechk.c,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.7  1997/01/20  23:40:30  can
 * Added NoCaseMemcmp
 * ,.
 *
 * Revision 2.6  1997/01/16  22:44:06  can
 * Lots of changes for 2.1 release
 *
 * Revision 2.2  1996/05/01  01:19:27  can
 * Changed CalculateTree to use doubles instead of floats
 *
 * Revision 1.16  1995/09/26  01:15:22  can
 * Fixed bug in IsValidDateStr.  Also, fixed bug in CompareFunctionParamTypes.  There is
 * still a problem here in the parameters that take multiple types do not have good
 * error reporting.
 *
 * Revision 1.15  1995/09/06  23:52:25  can
 * Made sure strings are freed when they are rolled up.
 *
 * Revision 1.14  1995/08/31  00:05:52  can
 * Fixed more compiler warnings.
 *
 * Revision 1.13  1995/08/30  16:10:14  can
 * Took i++ and j++ out of the array indexes because it doesn't work on the
 * mainframe optimization level 2.
 *
 * Revision 1.12  1995/08/25  01:37:41  can
 * Fixed compile warning messages
 *
 * Revision 1.11  1995/08/23  17:23:53  can
 *  Made all lines less than 100 bytes
 *
 * Revision 1.10  1995/08/11  01:40:04  can
 * Added variable to list of allVars.
 *
 * Revision 1.9  1995/08/08  22:45:15  can
 * Added check for NOOP expressions.
 *
 * Revision 1.8  1995/08/08  17:22:34  can
 * Fixed bug that treated string comparison expressions as strings, not numerics
 *
 * Revision 1.7  1995/08/05  01:06:15  can
 * Fixed bug in CompareFuncParamList and CompareRuleArgs that assumed all
 * reference parameters were data fields
 *
 * Revision 1.6  1995/08/02  00:11:07  can
 * Added code to allow global variables to be added from within
 * a rule, but only if the allowGlobalDef flag is set.  This should
 * be set only for the initialization support rules.
 *
 * Revision 1.5  1995/07/26  20:53:26  can
 * Added IsBinNumeric and fixed a bug in type checking strings and numerics
 *
 * Revision 1.4  1995/07/21  15:35:44  can
 * Fixed bugs in type checking routines.  Add variable checking.
 *
 * Revision 1.3  1995/07/16  18:26:25  can
 * Made all string comparisons case insensitive.
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
#include <string.h>
#include <ctype.h>
#include <math.h>

/* HNC Rule Engine include files */
#include "mymall.h"
#include "typechk.h"
#include "engerr.h"
#include "lexrule.h"
#include "prsrule.h"

/* HNC Common include files */
#include "except.h"

#ifdef DEVELOP
#include "rbdebug.h"
#endif

static long CalculateTree(tExprTree *exprTree);


/*--------------------------------------------------------------------------*/
/* AddLoopVariable()                                                        */
/*                                                                          */
/*--------------------------------------------------------------------------*/
#define MAX_LOOP_VARS    500
static char loopVars[MAX_LOOP_VARS];
long AddLoopVariable(char *var)
{
   int   i=0, j=0;
   char  tmpv[50]; 

   while(1)
   {  j=0;
      while(loopVars[i]!=',' && loopVars[i]!='\0')
      {  tmpv[j] = loopVars[i]; 
         i++; j++; 
      }
      tmpv[j] = '\0';

      if(!NoCaseStrcmp(tmpv, var))
         return(OK);

      if(loopVars[i]=='\0') break;
      i++;
   }

   if(i==0)
      sprintf(tmpv, "%s", var);
   else
      sprintf(tmpv, ",%s", var);

   strcat(loopVars, tmpv);

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* RemoveLoopVariable()                                                     */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long RemoveLoopVariable(char *varName)
{
   int   i=0, j=0;
   char  *ptr, temp[MAX_LOOP_VARS], tmpv[50]; 

   ptr = temp;
   temp[0] = '\0';
   while(1)
   {  j=0;
      while(loopVars[i]!=',' && loopVars[i]!='\0')
      {  tmpv[j] = loopVars[i]; 
         i++; j++; 
      }
      tmpv[j] = '\0';

      if(NoCaseStrcmp(tmpv, varName))
      {  if(temp[0] == '\0') sprintf(ptr, "%s", tmpv);
         else sprintf(ptr, ",%s", tmpv);
         ptr += strlen(tmpv);
      }

      if(loopVars[i]=='\0') break;
      i++;
   }

   strcpy(loopVars, temp);
   return(OK);
}


/*--------------------------------------------------------------------------*/
/* IsLoopVariable()                                                         */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long IsLoopVariable(char *var)
{
   int   i=0, j=0;
   char  tmpv[50]; 

   while(1)
   {  j=0;
      while(loopVars[i]!=',' && loopVars[i]!='\0')
      {  tmpv[j] = loopVars[i]; 
         i++; j++; 
      }
      tmpv[j] = '\0';

      if(!NoCaseStrcmp(tmpv, var))
         return(TRUE);

      if(loopVars[i]=='\0') break;
      i++;
   }

   return(FALSE);
}


/*--------------------------------------------------------------------------*/
/* FindType()                                                               */
/*                                                                          */
/*--------------------------------------------------------------------------*/
tType* FindType(char *typeName, tParse *parse)
{
   tType *type;

   /* Check local types */
   for(type=parse->localTypeList; type; type=type->next)
   {  if(!NoCaseStrcmp(typeName, type->typeName))
         return(type);
   }

   /* Check global types */
   for(type=parse->globalTypeList; type; type=type->next)
   {  if(!NoCaseStrcmp(typeName, type->typeName))
         return(type);
   }

   return((tType*)NULL);
}


/*--------------------------------------------------------------------------*/
/* NoCaseStrcmp()                                                           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long NoCaseStrcmp(char* str1, char* str2)
{
   int i, len, res;

   len = (strlen(str1) < strlen(str2))?strlen(str1):strlen(str2);
  
   for(i=0; i<len; i++)
   {  res = (int)(toupper(str1[i]) - toupper(str2[i]));
      if(res < 0) return(res);
      else if(res > 0) return(res);
      else continue;
   }

   if(strlen(str1) < strlen(str2)) return(-1);  
   else if(strlen(str1) > strlen(str2)) return(1);  

   return(0);
}


/*--------------------------------------------------------------------------*/
/* NoCaseMemcmp()                                                           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long NoCaseMemcmp(char* str1, char* str2, int len)
{
   int i, res;

   for(i=0; i<len; i++)
   {  res = (int)(toupper(str1[i]) - toupper(str2[i]));
      if(res < 0) return(res);
      else if(res > 0) return(res);
      else continue;
   }

   return(0);
}


/*--------------------------------------------------------------------------*/
/* IsValidDateStr()                                                         */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static int monthDays[12] = {
 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};
int IsValidDateStr(char* string)
{
   int year=0, mon=0, day=0, i=0;

   for(i=0; i<4; i++)
   {  if(!isdigit(string[i])) return(FALSE);
      year = year*10 + (string[i]-'0');
   }
   for(i=4; i<6; i++)
   {  if(!isdigit(string[i])) return(FALSE);
      mon = mon*10 + (string[i]-'0');
   }
   for(i=6; i<8; i++)
   {  if(!isdigit(string[i])) return(FALSE);
      day = day*10 + (string[i]-'0');
   }

   /* Check for bogus values */
   if(mon < 1 || mon > 12) return(FALSE);
   if(day < 1) return(FALSE);
   if(year < 1800 || year > 2100) return(FALSE);

   /* Check upper date range */
   if((year%4)==0 && mon==2 && day > 29) return(FALSE);
   else if(day > monthDays[mon-1]) return(FALSE);

   return(TRUE);
}


/*--------------------------------------------------------------------------*/
/* IsValidTimeStr()                                                         */
/*                                                                          */
/*--------------------------------------------------------------------------*/
int IsValidTimeStr(char* string)
{
   int i, strt, hours=0, minutes=0, seconds=0;

   for(i=0; i<2; i++)
   {  if(!isdigit(string[i])) return(FALSE);
      hours = hours*10 + (string[i]-'0');
   }

   if(string[i] == ':') strt=i+1;
   else strt=i;
   for(i=strt; i<strt+2; i++)
   {  if(!isdigit(string[i])) return(FALSE);
      minutes = minutes*10 + (string[i]-'0');
   }

   if(string[i] == ':') strt=i+1;
   else strt=i;
   for(i=strt; i<strt+2; i++)
   {  if(!isdigit(string[i])) return(FALSE);
      seconds = seconds*10 + (string[i]-'0');
   }

   if(hours < 0 || hours > 23) return(FALSE);
   if(minutes < 0 || minutes > 59) return(FALSE);
   if(seconds < 0 || seconds > 59) return(FALSE);

   return(TRUE);
}


/*--------------------------------------------------------------------------*/
/* IsValidDateTimeStr()                                                     */
/*                                                                          */
/*--------------------------------------------------------------------------*/
int IsValidDateTimeStr(char* string)
{
   if(!IsValidDateStr(string)) return(FALSE);
   else if(!IsValidTimeStr(string+8)) return(FALSE);

   return(TRUE);
}


/*--------------------------------------------------------------------------*/
/* CompareType()                                                            */
/*                                                                         */
/*--------------------------------------------------------------------------*/
long CompareType(int expectedType, int usedType)
{
   if(expectedType == usedType)
      return(OK);
   if(IsDate(expectedType)) 
   {  if(IsDate(usedType)) return(OK);
      else if(IsTime(usedType)) return(TYPE_MISMATCH_DT);
      else if(IsNumeric(usedType)) return(TYPE_MISMATCH_DN);
      else if(IsString(usedType)) return(TYPE_MISMATCH_DS);
      else return(TYPE_MISMATCH_DNONE);
   }
   else if(IsTime(expectedType))
   {  if(IsTime(usedType)) return(OK);
      else if(IsDate(usedType)) return(TYPE_MISMATCH_TD);
      else if(IsNumeric(usedType)) return(TYPE_MISMATCH_TN);
      else if(IsString(usedType)) return(TYPE_MISMATCH_TS);
      else return(TYPE_MISMATCH_TNONE);
   }
   else if(expectedType==HNC_STRING || expectedType == HNC_CHAR)
   {  if(IsString(usedType)) return(OK);
      else if(IsNumeric(usedType)) return(TYPE_MISMATCH_SN);
      else return(TYPE_MISMATCH_SNONE);
   }
   else if(IsNumString(expectedType))
   {  if(IsString(usedType)) return(OK);
      else if(IsNumeric(usedType)) return(OK);
      else return(TYPE_MISMATCH_SNONE);
   }
   else if(IsNumeric(expectedType))
   {  if(IsNumeric(usedType)) return(OK);
      else if(IsDate(usedType)) return(TYPE_MISMATCH_ND);
      else if(IsTime(usedType)) return(TYPE_MISMATCH_NT);
      else if(IsString(usedType)) return(TYPE_MISMATCH_NS);
      else return(TYPE_MISMATCH_NNONE);
   }
   else
      return(TYPE_MISMATCH_NONE);
}


/*--------------------------------------------------------------------------*/
/* CompareRuleArgType()                                                     */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long CompareRuleArgType
(tExprTree *exprTree, tRuleArg *ruleArg, char* ruleName, int argNum, tParse *parse)
{
   char errToken[200];
   long status;

   if(ruleArg->reference)
   {  if(exprTree->op != NOOP)
      {  if((status=AddErrMsg(FAIL_ENGINE|PARSE_RULE_CALL|PARAM_NOT_REF, 
                                   ruleName, argNum, parse))==FATAL_ENGINE)
            return(FATAL_ENGINE);
         else return(status);
      }
      else if((!(exprTree->left.flags&EXPR_FIELD) && 
               !(exprTree->left.flags&EXPR_VARBLE)) ||
                 exprTree->left.flags&EXPR_NEGATE || 
                 exprTree->left.flags&EXPR_NOT)
      {  if((status=AddErrMsg(FAIL_ENGINE|PARSE_RULE_CALL|PARAM_NOT_REF, 
                                   ruleName, argNum, parse))==FATAL_ENGINE)
            return(FATAL_ENGINE);
         else return(status);
      }
      else if(exprTree->left.flags&EXPR_FIELD)
      {  if(exprTree->left.val.dataFld->recPermission == READ_ONLY)
         {  sprintf(errToken, "%s$%s$%s", ruleName,
                               exprTree->left.val.dataFld->recordName,
                               exprTree->left.val.dataFld->fieldName);
            if((status=AddErrMsg(FAIL_ENGINE|PARSE_RULE_CALL|REF_READ_ONLY_TMPLT,
                                  errToken, argNum, parse))==FATAL_ENGINE)
               return(FATAL_ENGINE);
            else return(status);
         }
         else if(exprTree->left.val.dataFld->fldPermission == READ_ONLY)
         {  sprintf(errToken, "%s$%s$%s", ruleName,
                               exprTree->left.val.dataFld->recordName,
                               exprTree->left.val.dataFld->fieldName);
            if((status=AddErrMsg(FAIL_ENGINE|PARSE_RULE_CALL|REF_READ_ONLY_FLD,
                                     errToken, argNum, parse))==FATAL_ENGINE)
               return(FATAL_ENGINE);
            else return(status);
         }
      }
   }

   if((status= CompareType(ruleArg->type, exprTree->type)))
   {  if((status=AddErrMsg(FAIL_ENGINE|PARSE_RULE_CALL|status, 
                                 ruleName, argNum, parse))==FATAL_ENGINE)
         return(FATAL_ENGINE);
      else return(status);
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CompareFuncParamType()                                                   */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long CompareFuncParamType
(tExprTree *exprTree, tParamList* param, char* funcName, int argNum, tParse *parse)
{
   tTypeList* tList;
   long       status=OK;
   char       errToken[200];

   if(param->reference)
   {  if(exprTree->op != NOOP)
      {  if((status=AddErrMsg(FAIL_ENGINE|PARSE_FUNC|PARAM_NOT_REF, 
                                   funcName, argNum, parse))==FATAL_ENGINE)
            return(FATAL_ENGINE);
         else return(status);
      }
      else if((!(exprTree->left.flags&EXPR_FIELD) && 
               !(exprTree->left.flags&EXPR_VARBLE)) ||
                 exprTree->left.flags&EXPR_NEGATE || 
                 exprTree->left.flags&EXPR_NOT)
      {  if((status=AddErrMsg(FAIL_ENGINE|PARSE_FUNC|PARAM_NOT_REF, 
                                   funcName, argNum, parse))==FATAL_ENGINE)
            return(FATAL_ENGINE);
         else return(status);
      }
      else if(exprTree->left.flags&EXPR_FIELD)
      {  if(exprTree->left.val.dataFld->recPermission == READ_ONLY)
         {  sprintf(errToken, "%s$%s$%s", funcName, 
                               exprTree->left.val.dataFld->recordName, 
                               exprTree->left.val.dataFld->fieldName);
            if((status=AddErrMsg(FAIL_ENGINE|PARSE_FUNC|REF_READ_ONLY_TMPLT, 
                                  errToken, argNum, parse))==FATAL_ENGINE)
               return(FATAL_ENGINE);
            else return(status);
         }
         else if(exprTree->left.val.dataFld->fldPermission == READ_ONLY)
         {  sprintf(errToken, "%s$%s$%s", funcName, 
                               exprTree->left.val.dataFld->recordName, 
                               exprTree->left.val.dataFld->fieldName);
            if((status=AddErrMsg(FAIL_ENGINE|PARSE_FUNC|REF_READ_ONLY_FLD, 
                                     errToken, argNum, parse))==FATAL_ENGINE)
               return(FATAL_ENGINE);
            else return(status);
         }
      }
   }

   tList = param->list;
   while(tList)
   {  status = CompareType(tList->type, exprTree->type);

      if(status==OK)
      {  if(IsBinNumeric(tList->type) && IsString(exprTree->type))
            ResetTreeType(exprTree, NUMERIC_TYPE, parse);
         break;
      }
  
      else tList = tList->next;
   }

   if(status)
   {  if((status=AddErrMsg(FAIL_ENGINE|PARSE_FUNC|status, 
                                funcName, argNum, parse))==FATAL_ENGINE)
         return(FATAL_ENGINE);
      else return(status);
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* AddVarToAllData()                                                        */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long AddVarToAllData(tDataPtr *dataList, tEngVar *var)
{
   tDataNode *data, *newData;

   if((newData = (tDataNode*)MyMalloc(sizeof(tDataNode)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)newData, 0, sizeof(tDataNode));
   newData->val.var = var;

   if(dataList->varList)
   {  for(data=dataList->varList; data->next; data=data->next);
      data->next = newData;
   }
   else
      dataList->varList = newData;

   return(OK);
}
 

/*--------------------------------------------------------------------------*/
/* AddVarToList()                                                           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long AddVarToList(tEngVar **varList, tEngVar *var)
{
   tEngVar *tmpVar;

   if(*varList)
   {  for(tmpVar=*varList; tmpVar->next; tmpVar=tmpVar->next);
      tmpVar->next = var;
   }
   else
      *varList = var;

   return(OK);
}
 

/*--------------------------------------------------------------------------*/
/* AddTypeToAllData()                                                       */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long AddTypeToAllData(tDataPtr *dataList, tType *type)
{
   tDataNode *data, *newData;

   if((newData = (tDataNode*)MyMalloc(sizeof(tDataNode)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)newData, 0, sizeof(tDataNode));
   newData->val.type = type;

   if(dataList->typeList)
   {  for(data=dataList->typeList; data->next; data=data->next);
      data->next = newData;
   }
   else
      dataList->typeList = newData;

   return(OK);
}
 

/*--------------------------------------------------------------------------*/
/* AddTypeToList()                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long AddTypeToList(tType **typeList, tType *type)
{
   tType *tmpType;

   if(*typeList)
   {  for(tmpType=*typeList; tmpType->next; tmpType=tmpType->next);
      tmpType->next = type;
   }
   else
      *typeList = type;

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CheckVariable()                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long CheckVariable(char* varExpr, tParse *parse)
{
   tEngVar    *var, *newVar=NULL;
   tEngRBData *tmpRB;
   tArrayUse  *arrayUse;
   char       varName[MAX_TOKEN_LENGTH];
   long       arrayIndex=0, found=FALSE, i, status;


   /*************************/
   /* Check for array index */
   /*************************/
   if(strchr(varExpr, '{') || strchr(varExpr, '['))
   {  for(i=0; i<strlen(varExpr); i++)
      {  if(varExpr[i] == '{' || varExpr[i] == '[')
         {  arrayIndex = i;
            break;
         }

         varName[i] = varExpr[i];
      }
      varName[i] = '\0';
   }
   else
      strcpy(varName, varExpr);
   
   /************************************/
   /* Check in the rule parameter list */
   /************************************/
   for(var = parse->paramVarList; var; var=var->next)
      if(!NoCaseStrcmp(varName, var->varName))
      {  found = TRUE;
         break;
      }

   /*****************************/
   /* Check the local variables */
   /*****************************/
   if(!found)
   {  for(var = parse->localVarList; var; var=var->next)
         if(!NoCaseStrcmp(varName, var->varName))
         {  found = TRUE;
            var->flags |= VAR_LOCAL;
            break;
         }
   }

   /*************************************/
   /* Check in the global variable list */
   /*************************************/
   if(!found)
   {  for(var = parse->globalVarList; var; var=var->next)
         if(!NoCaseStrcmp(varName, var->varName))
         {  found = TRUE;
            break;
         }
   }

   if(found)
   {  /* If it is a constant, then set the value */
      if(var->flags&VAR_CONSTANT)
      {  switch(var->type)
         {  case HNC_STRING:
               memcpy(parse->token, (char*)var->data, var->dataSize);
               parse->token[var->dataSize] = '\0';
               parse->tokenType = T_STRING_CONSTANT;
               break;
            case HNC_CHAR:
               memcpy(parse->token, (char*)var->data, var->dataSize);
               parse->token[var->dataSize] = '\0';
               parse->tokenType = T_CHAR_CONSTANT;
               break;
            case HNC_FLOAT:
               NUM_SETFLT(parse->data.num, (*((float*)var->data)));
               parse->tokenType = T_FLOAT_CONSTANT;
               break;
            case HNC_INT32:
               NUM_SETINT32(parse->data.num, (*((long*)var->data)));
               parse->tokenType = T_INTEGER_CONSTANT;
               break;
            case NUMERIC_TYPE:
               NUM_SET(parse->data.num, (*((tEngNum*)var->data)));
               if(IS_INTEGER(parse->data.num)) 
                  parse->tokenType = T_INTEGER_CONSTANT;
               else
                  parse->tokenType = T_FLOAT_CONSTANT;
               break;
         }
         return(OK);
      }

      /* It is a variable, then set it in the parse structure */
      else
      {  /* If there is an array index, make sure variable is an */
         /* array type.  If so, parse the array index.           */ 
         if(arrayIndex!=0)
         {  /* Allocate arrayUse structure */
            parse->tokenType = T_ARRAYVAR;
            if((arrayUse = (tArrayUse*)MyMalloc(sizeof(tArrayUse)))==NULL)
               throw(MALLOC_EXCEPTION);
            memset((char*)arrayUse, 0, sizeof(tArrayUse));

            if(var->type != ARRAY_TYPE)
               return(VAR_NOT_ARRAY);
            else
               arrayUse->arrayData = (tArrayData*)var->data;

            /* Get the index expression */
            if((status=ParseArrayIndex(parse, varName, varExpr+arrayIndex, arrayUse)))
            {  MyFree((void*)arrayUse);
               parse->data.arrayUse = NULL;
               return(status);
            }

            /* Set the parse structure */ 
            parse->data.arrayUse = arrayUse;
         }
         else
         {  parse->data.var = var;
            if(var->calcRule)
            {  if(!var->calcRule->data)
                  return(ParseVarCalcRule(parse, var));
            }
         }

         return(OK);
      }
   }


   /*************************************************/
   /* If the variable has not been found, it may be */
   /* a new declaration.  Check if such a thing is  */
   /* currently allowed.                            */ 
   /*************************************************/
   if(parse->allowGlobalDef)
   {  char     name[50];
      int      index = parse->lastIndex;
      strcpy(name, varName);

      LexRule(parse);
      if(!strcmp(parse->token, ":="))
      {  /***************************************/
         /* Add the variable to the global list */
         /***************************************/
         if((newVar = (tEngVar*)MyMalloc(sizeof(tEngVar)))==NULL)
            throw(MALLOC_EXCEPTION); 
         memset((char*)newVar, 0, sizeof(tEngVar));
         AddVarToList(&parse->globalVarList, newVar); 
 
         /* Copy the name */
         newVar->varName = (char*)MyMalloc(strlen(name)+1);
         strcpy(newVar->varName, name);
       
         /* All new globals are numeric */
         newVar->type = NUMERIC_TYPE;  
         newVar->data = (tEngNum*)MyMalloc(sizeof(tEngNum));
         memset((char*)newVar->data, 0, sizeof(tEngNum));
         newVar->dataSize = sizeof(tEngNum);
         newVar->flags |= VAR_GLOBINT;

         parse->index = index;
         parse->data.var = newVar;

         LexRule(parse);

         return(OK);
      }
   }

   /* Finally, check if variable is a rule base name */
   for(tmpRB=parse->RBList; tmpRB; tmpRB=tmpRB->next)
   {  if(!strcmp(tmpRB->RBData->ruleBaseName, varName))
      {  parse->tokenType = T_RBNAME;
         parse->data.engRB = tmpRB;
         return(OK);
      }
   }

   return(VAR_NOT_FOUND);
}


/*--------------------------------------------------------------------------*/
/* GetNextFieldOffset()                                                     */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long GetNextFieldOffset(char* rec)
{
   long inArray=0;
   int  i=0;

   while(1)
   {  if(rec[i] == '[' || rec[i] == '{') 
         inArray++;
      else if(rec[i] == ']' || rec[i] == '}') 
         inArray--;

      if((rec[i] == '.' && !inArray) || rec[i] == '\0')
         break;
      i++;
   }

   if(rec[i] == '\0') return(0);
   else return(i+1);
}


/*--------------------------------------------------------------------------*/
/* CheckField()                                                             */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long CheckField
(tEngRecord *record, tRecData *recData, tRecUse *recUse, char *recTxt, tParse *parse) 
{
   tRecField   *fld;
   long        sepIndex=0, arrayIndex=0, recordIndex=0, fldOff, i=0;
   long        fldIndex=0, status;
   short       fldType=0;
   char        mark, *fldTxt;
   tArrayProc  *arrayProc=NULL;
   tArrayData  tmpAD, *tmpArrayData=&tmpAD;


   /* Get field text */
   fldOff = GetNextFieldOffset(recTxt);
   fldTxt = recTxt+fldOff;

   /* Get the next dot or array bracket, if any */
   while(1)
   {  if(fldTxt[i] == '{' || fldTxt[i] == '[')
      {  sepIndex = arrayIndex = i;
         recordIndex = GetNextFieldOffset(fldTxt);
         break;
      }
      else if(fldTxt[i] == '.')
      {  sepIndex = recordIndex = i;
         break;
      }
      else if(fldTxt[i] == '\0')
      {  sepIndex = i;
         break;
      }

      i++;
   }


   /* Check for the field within the record */
   mark = *(fldTxt+sepIndex); 
   *(fldTxt+sepIndex) = '\0';
   for(fld=record->fieldList; fld; fld=fld->next, fldIndex++)
   {  if(!NoCaseStrcmp(fldTxt, fld->fieldName))
      {  fldType = (short)fld->type;
         break;
      }
   }
   *(fldTxt+sepIndex) = mark;

   /* Return if field not found in record */
   if(fld==NULL)
   {  recTxt[fldOff-1] = '$';
      return(FLD_NOT_FOUND);
   }

   /* Check the field type against its use */
   if(arrayIndex && fld->type != ARRAY_TYPE)
   {  recTxt[fldOff-1] = '$';
      return(FLD_NOT_ARRAY);
   }
   else if(arrayIndex && recordIndex && fld->typeFmt->array.type != RECORD_TYPE)
   {  recTxt[fldOff-1] = '$';
      return(FLD_NOT_RECARRAY);
   }
   else if(!arrayIndex && recordIndex && fld->type != RECORD_TYPE)
   {  recTxt[fldOff-1] = '$';
      return(FLD_NOT_RECORD);
   }

   
   /* If is an array, or are currently tracking the fields, */
   /* add a new tArrayProc structure to the list            */ 
   if(arrayIndex || (recUse->arrayProc && recordIndex))
   {  if(recUse->arrayProc)
      {  for(arrayProc=recUse->arrayProc; arrayProc->next; arrayProc=arrayProc->next);
         arrayProc->fldIndex = fldIndex;
         if((arrayProc->next = (tArrayProc*)MyMalloc(sizeof(tArrayProc)))==NULL)
            throw(MALLOC_EXCEPTION);
         arrayProc = arrayProc->next;
      }
      else
      {  if((arrayProc = (tArrayProc*)MyMalloc(sizeof(tArrayProc)))==NULL)
            throw(MALLOC_EXCEPTION);
         recUse->arrayProc = arrayProc;
      }
      memset((char*)arrayProc, 0, sizeof(tArrayProc));
      arrayProc->fldIndex = -1;

      /* If have array index, add an array use structure */
      if(arrayIndex)
      {  /* Allocate a tArrayUse structure */
         if((arrayProc->arrayUse = (tArrayUse*)MyMalloc(sizeof(tArrayUse)))==NULL)
            throw(MALLOC_EXCEPTION);
         memset((char*)arrayProc->arrayUse, 0, sizeof(tArrayUse));

         /* Set the arrayData, if possible.  Otherwise, set up a bogus arrayData */
         if(recData)
            arrayProc->arrayUse->arrayData = (tArrayData*)recData->flds[fldIndex].data;
         else 
         {  arrayProc->arrayUse->arrayData = tmpArrayData;
            tmpArrayData->array = &fld->typeFmt->array;
         }
         recData = NULL;

         /* Set the array element type as the field type */
         fldType = (short)fld->typeFmt->array.type;

         /* Get index expression */
         if((status=ParseArrayIndex(parse, fld->fieldName, 
                                        fldTxt+arrayIndex, arrayProc->arrayUse)))
            return(status);
      }
   }
   else if(recUse->arrayProc && !recordIndex)
   {  for(arrayProc=recUse->arrayProc; arrayProc->next; arrayProc=arrayProc->next);
      arrayProc->fldIndex = fldIndex;
   }

   /* Get the data for the sub-record, if any */
   if(recData && fld->type == RECORD_TYPE && recordIndex)
      recData = (tRecData*)recData->flds[fldIndex].data; 

   /* If field is a record, recursively check the fields */
   if(recordIndex)
   {  if(arrayIndex)
         return(CheckField(&fld->typeFmt->array.typeFmt->record, 
                                    recData, recUse, fldTxt, parse));
      else
         return(CheckField(&fld->typeFmt->record, 
                                    recData, recUse, fldTxt, parse));
   }

   /* Set the data for the final field */
   if(recData) 
   {  recUse->fldData = recData->flds+fldIndex;
      recUse->type = fldType;
   }
   else
   {  /* Allocate a dummy tFldData structure */
      if((recUse->fldData = (tFldData*)MyMalloc(sizeof(tFldData)))==NULL)
         throw(MALLOC_EXCEPTION);
      memset((char*)recUse->fldData, 0, sizeof(tFldData));

      recUse->fldData->fld = fld;
      recUse->fldData->flags = FIELD_DUMMY;
      recUse->fldData->data = NULL;
      recUse->type = fldType;
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CheckRecordField()                                                       */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long CheckRecordField(char* recTxt, tParse *parse)
{
   char          recordName[50];
   long          inArray=0, found=FALSE, status, i=0, arrayIndex=0;
   tEngVar       *var;
   tEngRecord    *record;
   tRecData      *recData = NULL;
   tRecUse       *recUse = NULL;
   tDataTemplate *tmplt;
   tDataField    *fld;


   /****************************/
   /* Get the main record name */
   /****************************/
   while(recTxt[i]!='.')
   {  if(recTxt[i] == '{' || recTxt[i] == '[')
      {  arrayIndex = i;
         break;
      }

      recordName[i] = recTxt[i];
      i++;
   }
   recordName[i] = '\0';

   /*************************************************/
   /* Make sure that the record variable is defined */
   /*************************************************/
   /* Check parameters */
   for(var=parse->paramVarList; var && !found; var=var->next)
   {  if(!NoCaseStrcmp(recordName, var->varName))
      {  found = TRUE; 
         if(arrayIndex && var->type != ARRAY_TYPE)
            return(PARAM_NOT_ARRAY);
         else if(arrayIndex && ((tArrayData*)var->data)->array->type!=RECORD_TYPE) 
            return(PARAM_NOT_RECARRAY);
         else if(!arrayIndex && var->type != RECORD_TYPE)
            return(PARAM_NOT_RECORD);
         break;
      }
   }

   /* Check local variables */
   if(!found)
   {  for(var=parse->localVarList; var && !found; var=var->next)
      {  if(!NoCaseStrcmp(recordName, var->varName))
         {  found = TRUE; 
            if(arrayIndex && var->type != ARRAY_TYPE)
               return(VAR_NOT_ARRAY);
            else if(arrayIndex && ((tArrayData*)var->data)->array->type!=RECORD_TYPE) 
               return(VAR_NOT_RECARRAY);
            else if(!arrayIndex && var->type != RECORD_TYPE)
               return(VAR_NOT_RECORD);
            break;
         }
      }
   }

   /* Check global variables */
   if(!found)
   {  for(var=parse->globalVarList; var && !found; var=var->next)
      {  if(!NoCaseStrcmp(recordName, var->varName))
         {  found = TRUE; 
            if(arrayIndex && var->type != ARRAY_TYPE)
               return(VAR_NOT_ARRAY);
            else if(arrayIndex && ((tArrayData*)var->data)->array->type!=RECORD_TYPE) 
               return(VAR_NOT_RECARRAY);
            else if(!arrayIndex && var->type != RECORD_TYPE)
               return(VAR_NOT_RECORD);
            break;
         }
      }
   }

   if(!found)
      return(RECORD_NOT_DEFINED);


   /*************************************/
   /* Allocate up the tRecUse structure */
   /*************************************/
   if((recUse = (tRecUse*)MyMalloc(sizeof(tRecUse)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)recUse, 0, sizeof(tRecUse));

   /******************************************/
   /* Get the array index expression, if any */
   /******************************************/
   if(arrayIndex)
   {  if((recUse->arrayProc = (tArrayProc*)MyMalloc(sizeof(tArrayProc)))==NULL)
         throw(MALLOC_EXCEPTION);
      memset((char*)recUse->arrayProc, 0, sizeof(tArrayProc));
   
      if((recUse->arrayProc->arrayUse = (tArrayUse*)MyMalloc(sizeof(tArrayUse)))==NULL)
         throw(MALLOC_EXCEPTION);
      memset((char*)recUse->arrayProc->arrayUse, 0, sizeof(tArrayUse));

      recUse->arrayProc->arrayUse->arrayData = (tArrayData*)var->data;
      if((status=ParseArrayIndex(parse, recordName,
                            recTxt+arrayIndex, recUse->arrayProc->arrayUse))) 
         return(status);

      record = &((tArrayData*)var->data)->array->typeFmt->record;
   }
   else 
   {  record = ((tRecInstance*)var->data)->data->record;
      recData = ((tRecInstance*)var->data)->data;
   }

   /*************************************************/
   /* Make sure the field is defined for the record */
   /* This will recursively check if the field      */
   /* itself is a record.  Also, it will set up     */
   /* the processing path                           */
   /*************************************************/
   if((status=CheckField(record, recData, recUse, recTxt, parse)))
      return(status);

   /* Set the names */
   i=strlen(recTxt);
   while(1)
   {  if(recTxt[i]==']' || recTxt[i]=='}')
         inArray++;
      else if(recTxt[i]=='[' || recTxt[i]=='{')
         inArray--;

      if(recTxt[i]=='.' && !inArray)
         break;

      i--;
   }
   recTxt[i] = '\0';

   if((recUse->recordName = (char*)MyMalloc(strlen(recTxt)+1))==NULL)
      throw(MALLOC_EXCEPTION);
   strcpy(recUse->recordName, recTxt);

   if((recUse->fieldName = (char*)MyMalloc(strlen(recTxt+i+1)+1))==NULL)
      throw(MALLOC_EXCEPTION);
   strcpy(recUse->fieldName, recTxt+i+1);

   /******************************************************/
   /* Assume READ_WRITE fields and records, unless found */
   /* otherwise in the template list                     */
   /******************************************************/
   recUse->fldPermission = READ_WRITE;
   recUse->recPermission = READ_WRITE;
   for(tmplt = parse->templateList; tmplt; tmplt=tmplt->next)
      if(!NoCaseStrcmp(recUse->recordName, tmplt->templateName))
         break;

   if(tmplt)
   {  recUse->recPermission = tmplt->permission;
      for(fld=tmplt->fieldList; fld; fld=fld->next)   
      {  if(!NoCaseStrcmp(fld->fieldName, recUse->fieldName))
         {  recUse->fldPermission = fld->permission;
            break;
         }
      }
   }

   /* Set the dataFld value in the parse structure */
   parse->data.dataFld = recUse;

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CheckRule()                                                              */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long CheckRule(char* ruleName, tParse *parse, tRuleData **ruleData)
{
   tRuleData *temp;

   temp = parse->ruleList;
   while(temp)
   {  if(!NoCaseStrcmp(ruleName, temp->ruleName))
      {  *ruleData = temp; 
         return(OK);
      }
      temp = temp->next;
   }

   return(RULE_NOT_FOUND);
}


/*--------------------------------------------------------------------------*/
/* CheckFunction()                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long CheckFunction(char* function, tParse *parse, tFunc **funcStruct)
{
   tFunc *func;

   func = parse->funcList;
   while(func)
   {  if(!NoCaseStrcmp(function, func->functionName))
      {  *funcStruct = func;
         return(OK);
      }

      func = func->next;
   }

   return(FUNC_NOT_FOUND);
}


/*--------------------------------------------------------------------------*/
/* GetOpStr()                                                               */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static char* GetOpStr(tOp op)
{
   switch(op)
   {  case EQUAL: return "=";
      case LESS: return "<";
      case GRTR: return ">";
      case LESSEQL: return "<=";
      case GRTREQL: return ">=";
      case NOTEQL: return "<>";
      case ANDBOOL: return "AND";
      case ORBOOL: return "OR";
      case PLUS: return "+";
      case MINUS: return "-";
      case MULTIPLY: return "*";
      case DIVIDE: return "/";
      case SET: return ":=";
      case POWER: return "**";
      case NOOP: return "NOOP";
   }

   return(NULL);
}


/*--------------------------------------------------------------------------*/
/* SetOpType()                                                              */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void SetOpType(tOp *op, char *opStr)
{
   if(!strcmp(opStr, "="))
      *op = EQUAL;
   else if(!strcmp(opStr, "<"))
      *op = LESS;
   else if(!strcmp(opStr, ">"))
      *op = GRTR;
   else if(!strcmp(opStr, "<="))
      *op = LESSEQL;
   else if(!strcmp(opStr, ">="))
      *op = GRTREQL;
   else if(!strcmp(opStr, "<>"))
      *op = NOTEQL;
   else if(!strcmp(opStr, "AND"))
      *op = ANDBOOL;
   else if(!strcmp(opStr, "OR"))
      *op = ORBOOL;
   else if(!strcmp(opStr, "+"))
      *op = PLUS;
   else if(!strcmp(opStr, "-"))
      *op = MINUS;
   else if(!strcmp(opStr, "*"))
      *op = MULTIPLY;
   else if(!strcmp(opStr, "/"))
      *op = DIVIDE;
   else if(!strcmp(opStr, ":="))
      *op = SET;
   else if(!strcmp(opStr, "**"))
      *op = POWER;
   else
      *op = NOOP;
}


/*--------------------------------------------------------------------------*/
/* CalculateTree()                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long CalculateTree(tExprTree *exprTree)
{
   double  leftVal=(double)0.0, rightVal=(double)0.0, resVal=(double)0.0;
   char   *leftStr=NULL, *rightStr=NULL, *resStr=NULL;

   /*************************************************/
   /* First, make sure that they are both constants */
   /*************************************************/
   if(!(exprTree->left.flags&EXPR_CONSTANT) ||    
      !(exprTree->right.flags&EXPR_CONSTANT))     
      return(-1);

   /********************/
   /* Get the operands */
   /********************/
   if(IsNumeric(exprTree->left.type))
   {  leftVal = NUM_GETDBL(exprTree->left.val.num);
      rightVal = NUM_GETDBL(exprTree->right.val.num);

      if(exprTree->left.flags&EXPR_NEGATE) 
         leftVal = -(leftVal);
      else if(exprTree->right.flags&EXPR_NEGATE) 
         rightVal = -(rightVal);

      if(exprTree->left.flags&EXPR_NOT) 
         leftVal = (double)(!(leftVal));
      else if(exprTree->right.flags&EXPR_NOT) 
         rightVal = (double)(!(rightVal));
   }
   else
   {  leftStr = exprTree->left.val.str;
      rightStr = exprTree->right.val.str;
   }

   /***********************************/
   /* Perform the specified operation */
   /***********************************/
   switch(exprTree->op)
   {  case NOOP:
         if(IsNumeric(exprTree->left.type)) resVal = leftVal;
         else resStr = leftStr;
         break;
      case ANDBOOL:
         if(IsNumeric(exprTree->left.type)) 
            resVal = (double)(leftVal && rightVal);
         else resVal = (double)1.0;
         break;
      case ORBOOL:
         if(IsNumeric(exprTree->left.type)) 
            resVal = (double)(leftVal || rightVal);
         else resVal = (double)1.0;
         break;
      case EQUAL:
         if(IsNumeric(exprTree->left.type)) 
            resVal = (double)(leftVal == rightVal);
         else
         {  resVal = (double)(strcmp(leftStr, rightStr)==0);
            MyFree((char*)exprTree->left.val.str);
            MyFree((char*)exprTree->right.val.str);
         }   
         break;
      case NOTEQL:
         if(IsNumeric(exprTree->left.type)) 
            resVal = (double)(leftVal != rightVal);
         else
         {  resVal = (double)(strcmp(leftStr, rightStr)!=0);
            MyFree((char*)exprTree->left.val.str);
            MyFree((char*)exprTree->right.val.str);
         }   
         break;
      case GRTREQL:
         if(IsNumeric(exprTree->left.type)) 
            resVal = (double)(leftVal >= rightVal);
         else
         {  resVal = (double)(strcmp(leftStr, rightStr)>=0);
            MyFree((char*)exprTree->left.val.str);
            MyFree((char*)exprTree->right.val.str);
         }   
         break;
      case LESSEQL:
         if(IsNumeric(exprTree->left.type)) 
            resVal = (double)(leftVal <= rightVal);
         else
         {  resVal = (double)(strcmp(leftStr, rightStr)<=0);
            MyFree((char*)exprTree->left.val.str);
            MyFree((char*)exprTree->right.val.str);
         }   
         break;
      case GRTR:
         if(IsNumeric(exprTree->left.type)) 
            resVal = (double)(leftVal > rightVal);
         else
         {  resVal = (double)(strcmp(leftStr, rightStr)>0);
            MyFree((char*)exprTree->left.val.str);
            MyFree((char*)exprTree->right.val.str);
         }   
         break;
      case LESS:
         if(IsNumeric(exprTree->left.type)) 
            resVal = (double)(leftVal < rightVal);
         else
         {  resVal = (double)(strcmp(leftStr, rightStr)<0);
            MyFree((char*)exprTree->left.val.str);
            MyFree((char*)exprTree->right.val.str);
         }   
         break;
      case PLUS:
         if(IsNumeric(exprTree->left.type)) 
            resVal = (leftVal + rightVal);
         else
         {  resStr = (char*)MyMalloc(strlen(leftStr)+strlen(rightStr)+1);  
            sprintf(resStr, "%s%s", leftStr, rightStr);
            MyFree((char*)exprTree->left.val.str);
            MyFree((char*)exprTree->right.val.str);
         }
         break;
      case MINUS:
         resVal = leftVal - rightVal;
         break;
      case MULTIPLY:
         resVal = leftVal * rightVal;
         break;
      case DIVIDE:
         if(rightVal==0) resVal=0;
         else resVal = leftVal / rightVal;
         break;
      case POWER:
         resVal = pow(leftVal, rightVal);
         break;
      default:
         break;
   }

   /*********************************/
   /* Negate the value if necessary */
   /*********************************/
   if(exprTree->parent)
   {  if((exprTree->parent->left.flags & EXPR_TREE) &&
          exprTree->parent->left.val.tree == exprTree)
      {  if(exprTree->parent->left.flags & EXPR_NEGATE)
            resVal = -(resVal);
         else if(exprTree->parent->left.flags & EXPR_NOT)
            resVal = (double)(!(resVal));
      }
      else
      {  if(IsNumeric(exprTree->left.type))
         {  if(exprTree->parent->right.flags & EXPR_NEGATE)
               resVal = -(resVal);
            else if(exprTree->parent->right.flags & EXPR_NOT)
               resVal = (double)(!(resVal));
         }
      }
   }


   /******************************************************/
   /* Reset the current tree with just the calc'ed value */
   /******************************************************/
   if(IsNumeric(exprTree->type))
   {  NUM_SETDBL(exprTree->left.val.num, resVal)
      exprTree->left.type = NUMERIC_TYPE;
   }
   else
      exprTree->left.val.str = resStr;

   exprTree->op = NOOP;

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* ResetTreeType()                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long ResetTreeType(tExprTree *exprTree, int type, tParse *parse)
{
   long status;


   /*******************************/
   /* Reset the left tree, if any */
   /*******************************/
   if(exprTree->left.flags&EXPR_TREE)
   {  if((status=ResetTreeType(exprTree->left.val.tree, type, parse)))
         return(status);
   }

   /********************************/
   /* Reset the right tree, if any */
   /********************************/
   if(exprTree->right.flags&EXPR_TREE)
   {  if((status=ResetTreeType(exprTree->right.val.tree, type, parse)))
         return(status);
   }

   /**********************************************************/
   /* Make sure the new type is compatible with the old type */
   /**********************************************************/
   if((status=CompareType(type, exprTree->type)))
   {  status = AddErrMsg(FAIL_ENGINE|PARSE_EXPR|status, 
                                   GetOpStr(exprTree->op), 0, parse);
      return(status);
   }


   /************************************************/
   /* Check for illegal operations on string types */
   /* because may be going from INTSTR or FLOATSTR */
   /* to STRING.                                   */
   /************************************************/
   if(type==HNC_STRING)
   {  if((!IsNumeric(exprTree->left.type) && exprTree->left.flags&EXPR_NEGATE) ||
         (!IsNumeric(exprTree->right.type) && exprTree->right.flags&EXPR_NEGATE))
      {  status = AddErrMsg(FAIL_ENGINE|PARSE_EXPR|CANT_NEG_STR, 
                                                         NULL, 0, parse);
         return(status);
      }

      if(exprTree->op == MINUS ||
         exprTree->op == MULTIPLY ||
         exprTree->op == DIVIDE ||
         exprTree->left.flags&EXPR_NEGATE)
      {  status = AddErrMsg(FAIL_ENGINE|PARSE_EXPR|BAD_STRING_OP, 
                                        GetOpStr(exprTree->op), 0, parse);
         return(status);
      }

      exprTree->left.type = type;
      exprTree->right.type = type;
   }
    
   exprTree->type = type;

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* TypeCheckAndRollUpExpression()                                           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long TypeCheckAndRollUpExpression(tExprTree *exprTree, tParse *parse, int *type)
{
   int  leftType, rightType;
   long status;
   char errToken[200];
   
#ifdef DEVELOP
   debug_out("Type Checking expression:\n", "");
#endif

   /*************************************************************************/
   /* If the left branch is a sub-expression tree, recursively typecheck it */
   /*************************************************************************/
   if(exprTree->left.flags & EXPR_TREE)
   {  /*******************************************/
      /* Call TypeCheckExpression() for the tree */ 
      /*******************************************/
      if((status=TypeCheckAndRollUpExpression(exprTree->left.val.tree, parse, &leftType)))
         return(status);
      exprTree->left.type = leftType;
   }
   else
      leftType = exprTree->left.type;

   /****************************************/
   /* Stop now of the expression is a NOOP */
   /****************************************/
   if(exprTree->op == NOOP)
   {  if(type) *type = exprTree->left.type;
      exprTree->type = exprTree->left.type;
      return(OK);
   }
   
   /************************************************************************/
   /* If the right branch is sub-expression tree, recursively typecheck it */
   /************************************************************************/
   if(exprTree->right.flags & EXPR_TREE)
   {  /*********************************************/
      /* Call TypeCheckExpression() for the branch */ 
      /*********************************************/
      if((status=TypeCheckAndRollUpExpression(exprTree->right.val.tree, parse, &rightType)))
         return(status);
      exprTree->right.type = rightType;
   }
   else
      rightType = exprTree->right.type;
  


   /****************************************************************/
   /* If either of the operands is a NOT'd string, then the result */
   /* is a constant 0.                                             */ 
   /****************************************************************/
   if(!IsNumeric(exprTree->left.type) && exprTree->left.flags&EXPR_NOT)
   {  if(exprTree->left.flags&EXPR_CONSTANT)
         MyFree((char*)exprTree->left.val.str);

      exprTree->left.flags = EXPR_CONSTANT;
      leftType = exprTree->left.type = NUMERIC_TYPE;
      ZERO(exprTree->left.val.num)
      if(AddErrMsg(WARN_ENGINE|PARSE_EXPR|NOT_STRING, NULL, 0, parse)==FATAL_ENGINE)
         return(FATAL_ENGINE);
   }
   if(!IsNumeric(exprTree->right.type) && exprTree->right.flags&EXPR_NOT)
   {  if(exprTree->right.flags&EXPR_CONSTANT)
         MyFree((char*)exprTree->right.val.str);

      exprTree->right.flags = EXPR_CONSTANT;
      rightType = exprTree->right.type = NUMERIC_TYPE;
      ZERO(exprTree->right.val.num)
      if(AddErrMsg(WARN_ENGINE|PARSE_EXPR|NOT_STRING, NULL, 0, parse)==FATAL_ENGINE)
         return(FATAL_ENGINE);
   }


   /*****************************/
   /* Check for illegal lvalues */
   /*****************************/
   if(exprTree->op==SET)
   {  if((!(exprTree->left.flags&EXPR_FIELD) && 
          !(exprTree->left.flags&EXPR_VARBLE) &&
          !(exprTree->left.flags&EXPR_ARRAYVAR)) ||
            exprTree->left.flags&EXPR_NEGATE || 
            exprTree->left.flags&EXPR_NOT)
      {  status = AddErrMsg(FAIL_ENGINE|PARSE_EXPR|ILLEGAL_LVALUE, 
                                                         NULL, 0, parse);
         return(status);
      }

      /* The left side is a reference */
      exprTree->left.flags |= EXPR_REFERENCE;

      if(exprTree->left.flags&EXPR_FIELD)
      {  if(exprTree->left.val.dataFld->recPermission == READ_ONLY)
         {  char *tmpName = exprTree->left.val.dataFld->recordName;
            int  i=0;

            while(tmpName[i]!='.' && tmpName[i]!='{' && tmpName[i]!='[')
            {  errToken[i] = tmpName[i];
               i++;
            }
            errToken[i] = '\0';
            status = AddErrMsg(FAIL_ENGINE|PARSE_EXPR|SET_READ_ONLY_TMPLT, 
                                         errToken, 0, parse);
            return(status);
         }
         else if(exprTree->left.val.dataFld->fldPermission == READ_ONLY)
         {  sprintf(errToken, "%s$%s", 
                              exprTree->left.val.dataFld->recordName,
                              exprTree->left.val.dataFld->fieldName);
            status = AddErrMsg(FAIL_ENGINE|PARSE_EXPR|SET_READ_ONLY_FLD, errToken, 0, parse);
            return(status);
         }

         /* Make sure if setting records, they are same type */
         if(exprTree->left.val.dataFld->type == RECORD_TYPE)
         {  if(exprTree->right.type != RECORD_TYPE)
            {  sprintf(errToken, "%s.%s$%s",
                         exprTree->left.val.dataFld->recordName,
                         exprTree->left.val.dataFld->fieldName,
                         exprTree->left.val.dataFld->fldData->fld->typeFmt->record.recName);
               status = AddErrMsg(FAIL_ENGINE|PARSE_EXPR|SET_RECMISMATCH, 
                                  errToken, exprTree->right.type, parse);
               return(status);
            }        
            else
            {  if(exprTree->right.flags&EXPR_VARBLE)
               {  if(NoCaseStrcmp(
                      exprTree->left.val.dataFld->fldData->fld->typeFmt->record.recName,
                      ((tRecInstance*)exprTree->right.val.var->data)->data->record->recName))
                  {  sprintf(errToken, "%s.%s$%s$%s",
                        exprTree->left.val.dataFld->recordName,
                        exprTree->left.val.dataFld->fieldName,
                        exprTree->left.val.dataFld->fldData->fld->typeFmt->record.recName,
                        ((tRecInstance*)exprTree->right.val.var->data)->data->record->recName);
                     status = AddErrMsg(FAIL_ENGINE|PARSE_EXPR|SET_RECMISMATCH, 
                                                             errToken, 0, parse);
                     return(status);
                  }        
               }
               else if(exprTree->right.flags&EXPR_FIELD)
               {  if(NoCaseStrcmp(
                      exprTree->left.val.dataFld->fldData->fld->typeFmt->record.recName,
                      exprTree->right.val.dataFld->fldData->fld->typeFmt->record.recName))
                  {  sprintf(errToken, "%s.%s$%s$%s",
                         exprTree->left.val.dataFld->recordName,
                         exprTree->left.val.dataFld->fieldName,
                         exprTree->left.val.dataFld->fldData->fld->typeFmt->record.recName,
                         exprTree->right.val.dataFld->fldData->fld->typeFmt->record.recName);
                     status = AddErrMsg(FAIL_ENGINE|PARSE_EXPR|SET_RECMISMATCH, 
                                                             errToken, 0, parse);
                     return(status);
                  }        
               }
            }

            return(OK);
         }
         else if(exprTree->left.val.dataFld->type == ARRAY_TYPE)
         {  if(exprTree->right.type != ARRAY_TYPE)
            {  sprintf(errToken, "%s.%s$%s",
                            exprTree->left.val.dataFld->recordName,
                            exprTree->left.val.dataFld->fieldName,
                            exprTree->left.val.dataFld->fldData->fld->typeFmt->array.name);
               status = AddErrMsg(FAIL_ENGINE|PARSE_EXPR|SET_ARRMISMATCH, 
                        errToken, exprTree->right.type, parse);
               return(status);
            }        
            else 
            {  if(exprTree->right.flags&EXPR_VARBLE)
               {  if(NoCaseStrcmp(
                      ((tArrayData*)exprTree->left.val.var->data)->array->name,
                      ((tArrayData*)exprTree->right.val.var->data)->array->name))
                  {  sprintf(errToken, "%s.%s$%s$%s",
                             exprTree->left.val.dataFld->recordName,
                             exprTree->left.val.dataFld->fieldName,
                             exprTree->left.val.dataFld->fldData->fld->typeFmt->array.name,
                             ((tArrayData*)exprTree->right.val.var->data)->array->name);
                     status = AddErrMsg(FAIL_ENGINE|PARSE_EXPR|SET_ARRMISMATCH, 
                                                             errToken, 0, parse);
                     return(status);
                  }        
               }
               else if(exprTree->right.flags&EXPR_FIELD)
               {  if(NoCaseStrcmp(
                      ((tArrayData*)exprTree->left.val.var->data)->array->name,
                      exprTree->right.val.dataFld->fldData->fld->typeFmt->array.name))
                  {  sprintf(errToken, "%s.%s$%s$%s",
                             exprTree->left.val.dataFld->recordName,
                             exprTree->left.val.dataFld->fieldName,
                             exprTree->left.val.dataFld->fldData->fld->typeFmt->array.name,
                             exprTree->right.val.dataFld->fldData->fld->typeFmt->array.name);
                     status = AddErrMsg(FAIL_ENGINE|PARSE_EXPR|SET_ARRMISMATCH, 
                                                             errToken, 0, parse);
                     return(status);
                  }        
               }
            }
         
            return(OK);
         }
      }
      else if(exprTree->left.flags&EXPR_VARBLE)
      {  /* Cannot set a non-reference parameter */
         if(exprTree->left.val.var->flags&VAR_PARAMETER &&
            !(exprTree->left.val.var->flags&VAR_REFERENCE))
         {  status = AddErrMsg(FAIL_ENGINE|PARSE_EXPR|SET_PARAMETER, 
                    exprTree->left.val.var->varName, 0, parse);
            return(status);
         }

         /* Check if variable is a loop control var that may not be modified */
         if(IsLoopVariable(exprTree->left.val.var->varName))
         {  status = AddErrMsg(FAIL_ENGINE|PARSE_EXPR|CANT_MOD_LOOPVAR, 
                    exprTree->left.val.var->varName, 0, parse);
            return(status);
         }
      
         
         /* Make sure if setting records, they are same type */
         if(exprTree->left.val.var->type == RECORD_TYPE)
         {  if(exprTree->right.type != RECORD_TYPE)
            {  sprintf(errToken, "%s$%s",
                  exprTree->left.val.var->varName,
                 ((tRecInstance*)exprTree->left.val.var->data)->data->record->recName);
               status = AddErrMsg(FAIL_ENGINE|PARSE_EXPR|SET_RECMISMATCH, 
                        errToken, exprTree->right.type, parse);
               return(status);
            }        
            else
            {  if(exprTree->right.flags&EXPR_VARBLE)
               {  if(NoCaseStrcmp(
                      ((tRecInstance*)exprTree->left.val.var->data)->data->record->recName,
                      ((tRecInstance*)exprTree->right.val.var->data)->data->record->recName))
                  {  sprintf(errToken, "%s$%s$%s",
                        exprTree->left.val.var->varName,
                        ((tRecInstance*)exprTree->left.val.var->data)->data->record->recName,
                        ((tRecInstance*)exprTree->right.val.var->data)->data->record->recName);
                     status = AddErrMsg(FAIL_ENGINE|PARSE_EXPR|SET_RECMISMATCH, 
                                                             errToken, 0, parse);
                     return(status);
                  }        
               }
               else if(exprTree->right.flags&EXPR_FIELD)
               {  if(NoCaseStrcmp(
                      ((tRecInstance*)exprTree->left.val.var->data)->data->record->recName,
                      exprTree->right.val.dataFld->fldData->fld->typeFmt->record.recName))
                  {  sprintf(errToken, "%s$%s$%s",
                         exprTree->left.val.var->varName,
                         ((tRecInstance*)exprTree->left.val.var->data)->data->record->recName,
                         exprTree->right.val.dataFld->fldData->fld->typeFmt->record.recName);
                     status = AddErrMsg(FAIL_ENGINE|PARSE_EXPR|SET_RECMISMATCH, 
                                                             errToken, 0, parse);
                     return(status);
                  }        
               }
            }
         
            return(OK);
         }
         else if(exprTree->left.val.var->type == ARRAY_TYPE)
         {  if(exprTree->right.type != ARRAY_TYPE)
            {  sprintf(errToken, "%s$%s",
                            exprTree->left.val.var->varName,
                            ((tArrayData*)exprTree->left.val.var->data)->array->name);
               status = AddErrMsg(FAIL_ENGINE|PARSE_EXPR|SET_ARRMISMATCH, 
                        errToken, exprTree->right.type, parse);
               return(status);
            }        
            else 
            {  if(exprTree->right.flags&EXPR_VARBLE)
               {  if(NoCaseStrcmp(
                      ((tArrayData*)exprTree->left.val.var->data)->array->name,
                      ((tArrayData*)exprTree->right.val.var->data)->array->name))
                  {  sprintf(errToken, "%s$%s$%s",
                             exprTree->left.val.var->varName,
                             ((tArrayData*)exprTree->left.val.var->data)->array->name,
                             ((tArrayData*)exprTree->right.val.var->data)->array->name);
                     status = AddErrMsg(FAIL_ENGINE|PARSE_EXPR|SET_ARRMISMATCH, 
                                                             errToken, 0, parse);
                     return(status);
                  }        
               }
               else if(exprTree->right.flags&EXPR_FIELD)
               {  if(NoCaseStrcmp(
                      ((tArrayData*)exprTree->left.val.var->data)->array->name,
                      exprTree->right.val.dataFld->fldData->fld->typeFmt->array.name))
                  {  sprintf(errToken, "%s$%s$%s",
                             exprTree->left.val.var->varName,
                             ((tArrayData*)exprTree->left.val.var->data)->array->name,
                             exprTree->right.val.dataFld->fldData->fld->typeFmt->array.name);
                     status = AddErrMsg(FAIL_ENGINE|PARSE_EXPR|SET_ARRMISMATCH, 
                                                             errToken, 0, parse);
                     return(status);
                  }        
               }
            }
         
            return(OK);
         }
      }
   }
   else if(exprTree->left.flags&EXPR_VARBLE && 
           exprTree->left.val.var->type == RECORD_TYPE)
   {  status = AddErrMsg(FAIL_ENGINE|PARSE_EXPR|BAD_RECORD_OP, 
                              exprTree->left.val.var->varName, 0, parse);
      return(status);
   }


   /************************************************/
   /* Check for illegal operations on string types */
   /************************************************/
   if((!IsNumeric(leftType) && exprTree->left.flags&EXPR_NEGATE) ||
      (!IsNumeric(rightType) && exprTree->right.flags&EXPR_NEGATE))
   {  status = AddErrMsg(FAIL_ENGINE|PARSE_EXPR|CANT_NEG_STR, 
                                                      NULL, 0, parse);
      return(status);
   }

   if(!IsNumeric(leftType) && 
      (exprTree->op == MINUS ||
       exprTree->op == MULTIPLY ||
       exprTree->op == DIVIDE ||
       exprTree->left.flags&EXPR_NEGATE))
   {  status = AddErrMsg(FAIL_ENGINE|PARSE_EXPR|BAD_STRING_OP, 
                                        GetOpStr(exprTree->op), 0, parse);
      return(status);
   }
   else if((!IsNumeric(leftType) &&
             exprTree->op != MINUS &&
             exprTree->op != MULTIPLY &&
             exprTree->op != DIVIDE) ||
             (IsString(leftType) && exprTree->op == SET))
   {  /***************************************************************/
      /* Make sure that all string types get treated as HNC_STRING's */ 
      /* when operated upon.                                         */  
      /***************************************************************/
      leftType = HNC_STRING;
      if(IsString(rightType)) rightType = HNC_STRING;
   }


   /********************************************************************/
   /* Set the expression type.  If it is a boolean op, then the return */
   /* type is numeric.  Otherwise, it is the same as the type of the   */
   /* operands.                                                        */
   /********************************************************************/
   if(exprTree->op == EQUAL || 
      exprTree->op == NOTEQL || 
      exprTree->op == GRTREQL || 
      exprTree->op == LESSEQL || 
      exprTree->op == GRTR || 
      exprTree->op == LESS || 
      exprTree->op == ANDBOOL || 
      exprTree->op == ORBOOL)
      exprTree->type = NUMERIC_TYPE;
   else
   {  if(leftType == HNC_STRING || leftType == HNC_CHAR ||
         rightType == HNC_STRING || rightType == HNC_CHAR)
      {  if(exprTree->left.flags&EXPR_TREE)
         {  if((status=ResetTreeType(exprTree->left.val.tree, HNC_STRING, parse)))
               return(status);
         }
         if(exprTree->right.flags&EXPR_TREE)
         {  if((status=ResetTreeType(exprTree->right.val.tree, HNC_STRING, parse)))
               return(status);
         }

         if(IsString(leftType)) exprTree->left.type = HNC_STRING;
         if(IsString(rightType)) exprTree->right.type = HNC_STRING;
         exprTree->type = HNC_STRING;
      }
      else if(leftType == HNC_FLOAT ||
               leftType == HNC_DOUBLE ||
               leftType == HNC_INT16 ||
               leftType == HNC_UINT16 ||
               leftType == HNC_INT32 ||
               leftType == HNC_UINT32 ||
               leftType == NUMERIC_TYPE ||
               rightType == HNC_FLOAT ||
               rightType == HNC_DOUBLE ||
               rightType == HNC_INT16 ||
               rightType == HNC_UINT16 ||
               rightType == HNC_INT32 ||
               rightType == NUMERIC_TYPE ||
               rightType == HNC_UINT32)
      {  if(exprTree->left.flags&EXPR_TREE)
         {  if((status=ResetTreeType(exprTree->left.val.tree, NUMERIC_TYPE, parse)))
               return(status);
         }
         if(exprTree->right.flags&EXPR_TREE)
         {  if((status=ResetTreeType(exprTree->right.val.tree, NUMERIC_TYPE, parse)))
               return(status);
         }

         exprTree->type = NUMERIC_TYPE;
      }
      else
         exprTree->type = leftType;
   }

   /******************************************************/
   /* If both the left and right branches are constants, */
   /* then do the calculation, putting the result in the */
   /* left branch of exprTree                            */
   /******************************************************/
   if(!CalculateTree(exprTree))
   {  if(exprTree->parent)
      {  if((exprTree->parent->left.flags & EXPR_TREE) &&
             exprTree->parent->left.val.tree == exprTree)
         {  exprTree->parent->left = exprTree->left;
            exprTree->parent->left.flags = EXPR_CONSTANT;
         }
         else
         {  exprTree->parent->right = exprTree->left;
            exprTree->parent->right.flags = EXPR_CONSTANT;
         }

         if(type) *type = exprTree->type;

         memset((tExprTree*)exprTree, 0, sizeof(exprTree));
         FreeExprTree(exprTree);

         return(OK);
      }
   }


   /********************************************/
   /* Make sure the left and right types match */
   /********************************************/
   if(!(exprTree->op == NOOP && rightType == NOTYPE))
   {  if((status=CompareType(leftType, rightType)))
      {  status = AddErrMsg(FAIL_ENGINE|PARSE_EXPR|status, 
                                      GetOpStr(exprTree->op), 0, parse);
         return(status);
      }
   }


   if(type) *type = exprTree->type;

   return(OK);
}
