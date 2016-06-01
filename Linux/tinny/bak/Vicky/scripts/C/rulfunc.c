/*
** $Id: rulfunc.c,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: rulfunc.c,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.7  1997/02/07  19:44:14  can
 * Use SAFE_EXP instead of direct EXP call
 *
 * Revision 2.6  1997/02/05  04:16:30  can
 * Change calloc function calls to MyCalloc
 *
 * Revision 2.5  1997/01/25  07:46:07  can
 * Allowing spaces at end of amount strings in ValidAmount
 *
 * Revision 2.4  1997/01/21  19:06:18  can
 * Set up InitProfiles and ResetMoodels wrapper functions with their
 * changed wrapper function names.
 *
 * Revision 2.3  1997/01/21  00:38:56  gak
 * Added func creates for InitProfiles and ResetModels
 * changed GetGeneralRuleFuncs to GetExtraRuleFuncs
 *
** Revision 2.2  1997/01/17 17:22:36  can
** Updates for latest release
**
**
*/


/* System include files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

/* HNC Rule Engine includes */
#include "mymall.h"
#include "typechk.h"

/* HNC common includes */
#include "rulfunc.h"
#include "mgmodel.h"
#include "model.h"
#include "hnctype.h"
#include "hncrtyp.h"
#include "except.h"
#include "dyntabl.h"
#include "dates.h"

/* Product includes */
#include "prdrfnc.h"


long AllocFuncName(char **funcName, char *name)
{
   if((*funcName = (char*)MyMalloc(strlen(name)+1))==NULL)
      throw(MALLOC_EXCEPTION);
   strcpy(*funcName, name);

   return(0);
}


tAtom GetFraction(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom retVal;

   NUM_SETDBL(retVal.data.num, 
                NUM_GETDBL(aParams[0].data.num) - 
                NUM_GETLNG(aParams[0].data.num))

   retVal.type = NUMERIC_TYPE;
   return(retVal);
}


tAtom Power(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom retVal;

   NUM_SETDBL(retVal.data.num, 
                    pow(NUM_GETDBL(aParams[0].data.num),
                        NUM_GETDBL(aParams[1].data.num)))

   retVal.type = NUMERIC_TYPE;
   return(retVal);
}


tAtom Round(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom retVal;
   tEngNum val1, val2;

   NUM_SETDBL(val1, .5);
   NUM_ADD(val2, aParams[0].data.num, val1);

   NUM_SETINT32(retVal.data.num, NUM_GETLNG(val2))

   retVal.type = NUMERIC_TYPE;
   return(retVal);
}


tAtom Sqrt(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom retVal;

   NUM_SETDBL(retVal.data.num, sqrt(NUM_GETDBL(aParams[0].data.num)))

   retVal.type = NUMERIC_TYPE;
   return(retVal);
}


tAtom Log(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom  retVal;
   double logVal;

   logVal = NUM_GETDBL(aParams[0].data.num);
   logVal = logVal<=0?1:logVal;

   NUM_SETDBL(retVal.data.num, log(logVal));

   retVal.type = NUMERIC_TYPE;
   return(retVal);
}


tAtom Log10(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom retVal;
   double logVal;

   logVal = NUM_GETDBL(aParams[0].data.num);
   logVal = logVal<=0?1:logVal;

   NUM_SETDBL(retVal.data.num, log10(logVal));

   retVal.type = NUMERIC_TYPE;
   return(retVal);
}


tAtom Mod(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom retVal;
   long  numer, denom;

   retVal.type = NUMERIC_TYPE;

   numer = NUM_GETLNG(aParams[0].data.num);
   denom = NUM_GETLNG(aParams[1].data.num);

   if(denom==0) 
   {  ZERO(retVal.data.num)
      return(retVal); 
   }

   NUM_SETINT32(retVal.data.num, (numer%denom))

   return(retVal);
}


tAtom Div(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom retVal;
   long  numer, denom;

   retVal.type = NUMERIC_TYPE;

   numer = NUM_GETLNG(aParams[0].data.num);
   denom = NUM_GETLNG(aParams[1].data.num);

   if(denom==0) 
   {  ZERO(retVal.data.num)
      return(retVal); 
   }

   NUM_SETINT32(retVal.data.num, (numer/denom))

   return(retVal);
}


tAtom Left(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom retVal;
   long  length = NUM_GETLNG(aParams[1].data.num);

   retVal.data.strVal.str = aParams[0].data.strVal.str;

   if(length > aParams[0].data.strVal.len)
      retVal.data.strVal.len = aParams[0].data.strVal.len;
   else
      retVal.data.strVal.len = length;

   retVal.type = HNC_STRING;
   return(retVal);
}


tAtom Right(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom retVal;
   long  length = NUM_GETLNG(aParams[1].data.num);

   if(length > aParams[0].data.strVal.len)
   {  retVal.data.strVal.str = aParams[0].data.strVal.str;
      retVal.data.strVal.len = aParams[0].data.strVal.len;
   }
   else
   {  retVal.data.strVal.str = aParams[0].data.strVal.str + 
                                 (aParams[0].data.strVal.len - length);
      retVal.data.strVal.len = length;
   }

   retVal.type = HNC_STRING;
   return(retVal);
}


tAtom Mid(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom retVal;
   long  offset = NUM_GETLNG(aParams[1].data.num);
   long  length = NUM_GETLNG(aParams[2].data.num);

   if(length > aParams[0].data.strVal.len)
   {  retVal.data.strVal.str = aParams[0].data.strVal.str;
      retVal.data.strVal.len = aParams[0].data.strVal.len;
   }
   else if((offset + length) > aParams[0].data.strVal.len)
   {  retVal.data.strVal.str = aParams[0].data.strVal.str + offset; 
      retVal.data.strVal.len = aParams[0].data.strVal.len - offset;
   }
   else
   {  retVal.data.strVal.str = aParams[0].data.strVal.str + offset; 
      retVal.data.strVal.len = length;
   }

   retVal.type = HNC_STRING;
   return(retVal);
}


tAtom ValidAmount(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom  eval;
   int    i, haveDecimal=0;

   eval.type = NUMERIC_TYPE;
   ZERO(eval.data.num);

   /* skip leading spaces */
   i = 0;
   while(isspace(aParams[0].data.strVal.str[i]) &&
         i < aParams[0].data.strVal.len)
      i++;

   /* If all spaces, it is bad */
   if(i==aParams[0].data.strVal.len)
      return(eval);
   
   /* May have a + or - sign */
   if(aParams[0].data.strVal.str[i] == '+' ||
      aParams[0].data.strVal.str[i] == '-')
     i++;

   /* check for non digits except for decimal point, stop at space */
   while(i < aParams[0].data.strVal.len)
   {
      if(aParams[0].data.strVal.str[i] == '.' && !haveDecimal)
      {  haveDecimal=1;
         i++;
      }
      else if(!isdigit(aParams[0].data.strVal.str[i]))
      {  if(aParams[0].data.strVal.str[i] == ' ')
         {  ONE(eval.data.num);
            return(eval);
         }
         else
            return(eval);
      }

      i++;
   }

   ONE(eval.data.num);
   return(eval);
}


tAtom Trunc(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom retVal;

   NUM_SET(retVal.data.num, aParams[0].data.num);
   TRUNC(retVal.data.num);

   retVal.type = NUMERIC_TYPE;
   return(retVal);
}


tAtom Exp(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom retVal;

   NUM_SETDBL(retVal.data.num, SAFE_EXP(NUM_GETDBL(aParams[0].data.num)));

   retVal.type = NUMERIC_TYPE;
   return(retVal);
}


tAtom Min(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom minVal;
   int   a;

   minVal.type = NUMERIC_TYPE;
   NUM_SET(minVal.data.num, aParams[0].data.num);

   for(a=1; a<numArgs; a++)
      if(IS_GRTR(minVal.data.num, aParams[a].data.num))
         NUM_SET(minVal.data.num, aParams[a].data.num); 

   return(minVal);
}


tAtom Max(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom maxVal;
   int   a;

   maxVal.type = NUMERIC_TYPE;
   NUM_SET(maxVal.data.num, aParams[0].data.num);

   for(a=1; a<numArgs; a++)
      if(IS_LESS(maxVal.data.num, aParams[a].data.num))
         NUM_SET(maxVal.data.num, aParams[a].data.num); 

   return(maxVal);
}


tAtom Abs(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom  retVal;

   NUM_SET(retVal.data.num, aParams[0].data.num);
   ABS(retVal.data.num);
 
   retVal.type = NUMERIC_TYPE;
   return(retVal);
}


tAtom IsMissing(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom eval;
   int   i;

   for(i=0; i<aParams[0].data.strVal.len; i++)
      if(!isspace(aParams[0].data.strVal.str[i]) &&  
          aParams[0].data.strVal.str[i] != '\0')
      {  ZERO(eval.data.num);
         eval.type = NUMERIC_TYPE;
         return(eval);
      }
   
   ONE(eval.data.num);
   eval.type = NUMERIC_TYPE;
   return(eval);
}


tAtom IsIntString(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom eval;
   int   i;

   eval.type = NUMERIC_TYPE;

   for(i=0; i<aParams[0].data.strVal.len; i++)
   {  if((aParams[0].data.strVal.str[i] != ' ') &&
         (!isdigit(aParams[0].data.strVal.str[i])))
      {  ZERO(eval.data.num)
         return(eval);
      }
   }

   ONE(eval.data.num)
   return(eval);
}


tAtom IsDigit(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom eval;

   if(isdigit(aParams[0].data.strVal.str[0]))
      ONE(eval.data.num)
   else
      ZERO(eval.data.num)

   eval.type = NUMERIC_TYPE;
   return(eval);
}


tAtom IsAlpha(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom eval;

   if(isalpha(aParams[0].data.strVal.str[0]))
      ONE(eval.data.num)
   else
      ZERO(eval.data.num)

   eval.type = NUMERIC_TYPE;
   return(eval);
}


tAtom IsAlnum(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom eval;

   if(isalnum(aParams[0].data.strVal.str[0]))
      ONE(eval.data.num)
   else
      ZERO(eval.data.num)

   eval.type = NUMERIC_TYPE;
   return(eval);
}


tAtom StringToNum(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom  retVal;

   NUM_SETSTR(&retVal.data.num, 
          aParams[0].data.strVal.str, aParams[0].data.strVal.len);

   retVal.type = NUMERIC_TYPE;
   return(retVal);
}


tAtom Writeln(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom eval;
   int   a;

   for(a=0; a<numArgs; a++)
   {  if(aParams[a].type==NUMERIC_TYPE)
      {  if((NUM_GETFLT(aParams[a].data.num) - 
                    (float)NUM_GETLNG(aParams[a].data.num))==0.0)
            fprintf(stderr, "%ld", NUM_GETLNG(aParams[a].data.num));
         else
            fprintf(stderr, "%12.6f", NUM_GETDBL(aParams[a].data.num));
      }
      else
         fprintf(stderr, "%-*.*s", aParams[a].data.strVal.len, 
                                   aParams[a].data.strVal.len, 
                                   aParams[a].data.strVal.str);
   }

   fprintf(stderr, "\n");

   ZERO(eval.data.num);
   eval.type = NUMERIC_TYPE;

   return(eval);
}


tAtom Write(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom eval;
   int   a;

   for(a=0; a<numArgs; a++)
   {  if(aParams[a].type==NUMERIC_TYPE)
         fprintf(stderr, "%f", NUM_GETDBL(aParams[a].data.num));
      else
      {  char data[1024];

         memcpy(data, aParams[a].data.strVal.str, aParams[a].data.strVal.len);
         data[aParams[a].data.strVal.len] = '\0';
         fprintf(stderr, "%s", data);
      }
   }

   ZERO(eval.data.num);
   eval.type = NUMERIC_TYPE;

   return(eval);
}


tAtom Print(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom eval;
   char  *str;
   char data[1024];

   if(aParams[1].type==NUMERIC_TYPE)
   {  if((str=strstr(aParams[0].data.strVal.str, "%s")))
         str[1] = 'f';
      
      fprintf(stderr, aParams[0].data.strVal.str, NUM_GETDBL(aParams[1].data.num));
   }
   else
   {  memcpy(data, aParams[1].data.strVal.str, aParams[1].data.strVal.len);
      data[aParams[1].data.strVal.len] = '\0';
      fprintf(stderr, aParams[0].data.strVal.str, data);
   }

   fprintf(stderr, "\n");

   ZERO(eval.data.num);
   eval.type = NUMERIC_TYPE;

   return(eval);
}


void PrintCreate(tFuncInit *funcInit, tFunc **funcList)
{
   tFunc*      FuncElem;
   tParamList* ParamList;
   tTypeList*  TypeElem;
   tFunc*      TempList;

   FuncElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   TypeElem = (tTypeList*) MyCalloc (3,sizeof(tTypeList));
   ParamList = (tParamList*) MyCalloc (2,sizeof(tParamList));


   AllocFuncName(&FuncElem->functionName, "PRINT");
   FuncElem->unlimitedParams = 0;
   FuncElem->numParams = 2;
   TypeElem[0].type = HNC_STRING;
   TypeElem[0].next = NULL;
   TypeElem[1].type = HNC_STRING;
   TypeElem[1].next = &TypeElem[2];
   TypeElem[2].type = HNC_STRING;
   TypeElem[2].next = NULL;
   ParamList[0].reference = 0;
   ParamList[0].list = &TypeElem[0];
   ParamList[0].next = &ParamList[1];
   ParamList[1].reference = 0;
   ParamList[1].list = &TypeElem[1];
   ParamList[1].next = NULL;

   FuncElem->paramTypeList = ParamList;
   FuncElem->returnType = NUMERIC_TYPE;
   FuncElem->FuncPtr = Print;
   FuncElem->SysInitFuncPtr = NULL;
   FuncElem->IterInitFuncPtr = NULL;
   FuncElem->SysTermFuncPtr = NULL;
   FuncElem->IterTermFuncPtr = NULL;
   FuncElem->inputData = NULL;
   FuncElem->next = NULL;

   if (*funcList == NULL)
      *funcList=FuncElem;
   else
   {  TempList=*funcList;
      while(TempList->next)
         TempList=TempList->next;
      TempList->next=FuncElem;
   }
}


void IsAlphaCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (1,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (1,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Is_Alpha");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 1;
   typeElem[0].type = HNC_STRING;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr = IsAlpha;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;
   funcElem->inputData = (void*)funcInit;
   funcElem->next = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}


void IsAlnumCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (1,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (1,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Is_Alnum");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 1;
   typeElem[0].type = HNC_STRING;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr = IsAlnum;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;
   funcElem->inputData = (void*)funcInit;
   funcElem->next = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}


void IsIntStringCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (1,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (1,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Is_Int_String");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 1;
   typeElem[0].type = HNC_STRING;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr = IsIntString;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;
   funcElem->inputData = (void*)funcInit;
   funcElem->next = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}


void IsDigitCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (1,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (1,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Is_Digit");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 1;
   typeElem[0].type = HNC_STRING;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr=IsDigit;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;
   funcElem->inputData = (void*)funcInit;
   funcElem->next = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}


void IsMissingCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (1,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (1,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Is_Missing");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 1;
   typeElem[0].type = HNC_STRING;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr=IsMissing;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;
   funcElem->inputData = (void*)funcInit;
   funcElem->next = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}


void GetModelErrorStringCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (3,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (3,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Get_Model_Error_String");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 3;
   typeElem[0].type = HNC_STRING;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = paramList+1;
   typeElem[1].type = NUMERIC_TYPE;
   typeElem[1].next = NULL;
   paramList[1].reference = 0;
   paramList[1].list = &typeElem[1];
   paramList[1].next = paramList+2;
   typeElem[2].type = HNC_STRING;
   typeElem[2].next = NULL;
   paramList[2].reference = 1;
   paramList[2].list = &typeElem[2];
   paramList[2].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr = GetModelErrorString;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;
   funcElem->inputData = (void*)funcInit;
   funcElem->next = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}


void FireModelCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (2,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (2,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Fire_Model");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 2;
   typeElem[0].type = HNC_STRING;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = paramList+1;
   typeElem[1].type = NUMERIC_TYPE;
   typeElem[1].next = NULL;
   paramList[1].reference = 1;
   paramList[1].list = &typeElem[1];
   paramList[1].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = HNC_STRING;
   funcElem->FuncPtr=FireModel;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;
   funcElem->inputData = (void*)funcInit;
   funcElem->next = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}

void InitProfilesCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));

   AllocFuncName(&funcElem->functionName, "InitProfiles");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 0;
   funcElem->paramTypeList = NULL;
   funcElem->returnType = HNC_FLOAT;
   funcElem->FuncPtr = InitProfilesFunc;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;
   funcElem->inputData = (void*)funcInit;
   funcElem->next = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}

void ResetModelsCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));

   AllocFuncName(&funcElem->functionName, "ResetModels");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 0;
   funcElem->paramTypeList = NULL;
   funcElem->returnType = HNC_FLOAT;
   funcElem->FuncPtr = ResetModelsFunc;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;
   funcElem->inputData = (void*)funcInit;
   funcElem->next = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}

void MinCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (2,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (2,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Min");
   funcElem->unlimitedParams = 1;
   funcElem->numParams = 2;
   typeElem[0].type = NUMERIC_TYPE;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = paramList+1;
   typeElem[1].type = NUMERIC_TYPE;
   typeElem[1].next = NULL;
   paramList[1].reference = 0;
   paramList[1].list = &typeElem[1];
   paramList[1].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr=Min;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;
   funcElem->inputData = (void*)funcInit;
   funcElem->next = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}


void MaxCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (2,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (2,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Max");
   funcElem->unlimitedParams = 1;
   funcElem->numParams = 2;
   typeElem[0].type = NUMERIC_TYPE;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = paramList+1;
   typeElem[1].type = NUMERIC_TYPE;
   typeElem[1].next = NULL;
   paramList[1].reference = 0;
   paramList[1].list = &typeElem[1];
   paramList[1].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr=Max;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;
   funcElem->inputData = (void*)funcInit;
   funcElem->next = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}


void WritelnCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (2,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (1,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Writeln");
   funcElem->unlimitedParams = 1;
   funcElem->numParams = 0;
   typeElem[0].type = HNC_STRING;
   typeElem[0].next = &typeElem[1];
   typeElem[1].type = NUMERIC_TYPE;
   typeElem[1].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr = Writeln;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;
   funcElem->inputData = (void*)funcInit;
   funcElem->next = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}


void WriteCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (2,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (1,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Write");
   funcElem->unlimitedParams = 1;
   funcElem->numParams = 1;
   typeElem[0].type = NUMERIC_TYPE;
   typeElem[0].next = &typeElem[1];
   typeElem[1].type = HNC_STRING;
   typeElem[1].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr = Write;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;
   funcElem->inputData = (void*)funcInit;
   funcElem->next = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}


void TruncCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (1,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (1,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Trunc");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 1;
   typeElem[0].type = NUMERIC_TYPE;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = typeElem;
   paramList[0].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr = Trunc;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;
   funcElem->inputData = (void*)funcInit;
   funcElem->next = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}


void ExpCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (1,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (1,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Exp");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 1;
   typeElem[0].type = NUMERIC_TYPE;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = typeElem;
   paramList[0].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr = Exp;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;
   funcElem->inputData = (void*)funcInit;
   funcElem->next = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}


void AbsCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (1,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (1,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Abs");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 1;

   typeElem[0].type = NUMERIC_TYPE;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr = Abs;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;
   funcElem->inputData = (void*)funcInit;
   funcElem->next = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}


void ValidAmountCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (1,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (1,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Valid_Amount");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 1;
   typeElem[0].type = HNC_FLOATSTR;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = typeElem;
   paramList[0].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr = ValidAmount;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;
   funcElem->inputData = (void*)funcInit;
   funcElem->next = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}


void LeftCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (2,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (2,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Left");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 2;

   typeElem[0].type = HNC_STRING;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = paramList+1;
   typeElem[1].type = NUMERIC_TYPE;
   typeElem[1].next = NULL;
   paramList[1].reference = 0;
   paramList[1].list = &typeElem[1];
   paramList[1].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = HNC_STRING;
   funcElem->FuncPtr = Left; 
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;
   funcElem->inputData = (void*)funcInit;
   funcElem->next = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}


void RightCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (2,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (2,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Right");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 2;

   typeElem[0].type = HNC_STRING;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = paramList+1;
   typeElem[1].type = NUMERIC_TYPE;
   typeElem[1].next = NULL;
   paramList[1].reference = 0;
   paramList[1].list = &typeElem[1];
   paramList[1].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = HNC_STRING;
   funcElem->FuncPtr = Right;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;
   funcElem->inputData = (void*)funcInit;
   funcElem->next = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}


void MidCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (3, sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (3,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Mid");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 3;

   typeElem[0].type = HNC_STRING;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = paramList+1;
   typeElem[1].type = NUMERIC_TYPE;
   typeElem[1].next = NULL;
   paramList[1].reference = 0;
   paramList[1].list = &typeElem[1];
   paramList[1].next = paramList+2;
   typeElem[2].type = NUMERIC_TYPE;
   typeElem[2].next = NULL;
   paramList[2].reference = 0;
   paramList[2].list = &typeElem[2];
   paramList[2].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = HNC_STRING;
   funcElem->FuncPtr = Mid;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;
   funcElem->inputData = (void*)funcInit;
   funcElem->next = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}


void DivCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (2,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (2,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Div");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 2;

   typeElem[0].type = NUMERIC_TYPE;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = paramList+1;
   typeElem[1].type = NUMERIC_TYPE;
   typeElem[1].next = NULL;
   paramList[1].reference = 0;
   paramList[1].list = &typeElem[1];
   paramList[1].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr = Div;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;
   funcElem->inputData = (void*)funcInit;
   funcElem->next = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}


void ModCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (2,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (2,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Mod");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 2;

   typeElem[0].type = NUMERIC_TYPE;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = paramList+1;
   typeElem[1].type = NUMERIC_TYPE;
   typeElem[1].next = NULL;
   paramList[1].reference = 0;
   paramList[1].list = &typeElem[1];
   paramList[1].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr = Mod;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;
   funcElem->inputData = (void*)funcInit;
   funcElem->next = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}


void Log10Create(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (1,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (1,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Log10");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 1;

   typeElem[0].type = NUMERIC_TYPE;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr = Log10;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;
   funcElem->inputData = (void*)funcInit;
   funcElem->next = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}


void LogCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (1,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (1,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Log");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 1;

   typeElem[0].type = NUMERIC_TYPE;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr = Log;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;
   funcElem->inputData = (void*)funcInit;
   funcElem->next = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}


void SqrtCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (1,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (1,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Sqrt");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 1;

   typeElem[0].type = NUMERIC_TYPE;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr = Sqrt;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;
   funcElem->inputData = (void*)funcInit;
   funcElem->next = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}


void GetFractionCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (1,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (1,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Get_Fraction");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 1;

   typeElem[0].type = NUMERIC_TYPE;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr = GetFraction;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;
   funcElem->inputData = (void*)funcInit;
   funcElem->next = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}


void RoundCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (1,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (1,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Round");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 1;

   typeElem[0].type = NUMERIC_TYPE;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr = Round;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;
   funcElem->inputData = (void*)funcInit;
   funcElem->next = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}


void PowerCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (2,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (2,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Power");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 2;

   typeElem[0].type = NUMERIC_TYPE;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = paramList+1;
   typeElem[1].type = NUMERIC_TYPE;
   typeElem[1].next = NULL;
   paramList[1].reference = 0;
   paramList[1].list = &typeElem[1];
   paramList[1].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr = Power;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;
   funcElem->inputData = (void*)funcInit;
   funcElem->next = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}

void StringToNumericCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (1,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (1,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "String_To_Num");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 1;

   typeElem[0].type = HNC_STRING;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr = StringToNum;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;
   funcElem->inputData = (void*)funcInit;
   funcElem->next = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}


void GetGeneralRuleFuncs(tFuncInit *funcInit, tFunc **funcList)
{
   FireModelCreate(funcInit, funcList);
   InitProfilesCreate(funcInit,funcList);
   ResetModelsCreate(funcInit,funcList);
   GetModelErrorStringCreate(funcInit, funcList);

   PrintCreate(funcInit, funcList);
   WriteCreate(funcInit, funcList);
   WritelnCreate(funcInit, funcList);

   IsMissingCreate(funcInit, funcList);

   IsIntStringCreate(funcInit, funcList);
   IsDigitCreate(funcInit, funcList);
   IsAlphaCreate(funcInit, funcList);
   IsAlnumCreate(funcInit, funcList);

   MaxCreate(funcInit, funcList);
   MinCreate(funcInit, funcList);
   ExpCreate(funcInit, funcList);
   TruncCreate(funcInit, funcList);
   AbsCreate(funcInit, funcList);
   PowerCreate(funcInit, funcList);
   RoundCreate(funcInit, funcList);
   SqrtCreate(funcInit, funcList);
   LogCreate(funcInit, funcList);
   Log10Create(funcInit, funcList);
   ModCreate(funcInit, funcList);
   DivCreate(funcInit, funcList);
   GetFractionCreate(funcInit, funcList);

   LeftCreate(funcInit, funcList);
   RightCreate(funcInit, funcList);
   MidCreate(funcInit, funcList);
   StringToNumericCreate(funcInit, funcList);

   ValidDateCreate(funcInit, funcList);
   ValidTimeCreate(funcInit, funcList);
   ValidAmountCreate(funcInit, funcList);
   TimeConvertCreate(funcInit, funcList);
   DateConvertCreate(funcInit, funcList);
   DateTimeConvertCreate(funcInit, funcList);
   IsWeekDayCreate(funcInit, funcList);
   DayOfWeekCreate(funcInit, funcList);
   DateDiffCreate(funcInit, funcList);
   MonthDiffCreate(funcInit, funcList);
   Day90ToStringCreate(funcInit, funcList);

   TableLookupCreate(funcInit, funcList);
   TableRangeLookupCreate(funcInit, funcList);
   TableDirectIndexCreate(funcInit, funcList);
   TableGetDefaultsCreate(funcInit, funcList);
   TableMultiLookupCreate(funcInit, funcList);

   LoadProductRuleFunctions(funcInit, funcList);
}
