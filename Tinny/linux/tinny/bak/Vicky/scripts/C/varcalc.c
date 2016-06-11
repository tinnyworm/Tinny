/*
** $Id: varcalc.c,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: varcalc.c,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.6  1997/02/07  20:21:21  can
 * Changed all exp() calls to SAFE_EXP macro
 *
 * Revision 2.5  1997/02/07  16:51:24  can
 * Added optional parameter to the MaxDecay and MaxShift functions to
 * allow for decay of both old and new values
 *
 * Revision 2.4  1997/02/05  04:16:30  can
 * Added short integers as key options for table lookups.  Also,
 * now forcing these table looking functions to have the table
 * match the type.
 *
 * Revision 2.3  1997/01/25  07:17:57  can
 * Added RATE_DECAY function as previous missing function #7
 *
 * Revision 2.2  1997/01/17  17:22:36  can
 * Updates for latest release
 *
**
*/

/* System includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

/* HNC Rule Engine includes */
#include "typechk.h"

/* HNC common includes */
#include "modfunc.h"
#include "except.h"
#include "hncrtyp.h"
#include "varcalc.h"
#include "config.h"
#include "dates.h"
#include "mgmodel.h"
#include "dyntabl.h"

/* Product includes */
#include "prdcalc.h"



#define MAX(x,y) ((x<y)?(y):(x))
#define MIN(x,y) ((x<y)?(x):(y))

void CalcMonthDiff(tEngVar *calcVar, tModelVar *modelVar)
{
   float date1;
   float date2;
   int   yr1, mo1, day1;
   int   yr2, mo2, day2;
   int   monthDiff=0;

   date1 = *((float*)modelVar->args->var->data);
   date2 = *((float*)modelVar->args->next->var->data);

   GetYrMoDayFromDay90(date1, &yr1, &mo1, &day1);
   GetYrMoDayFromDay90(date2, &yr2, &mo2, &day2);

   monthDiff = (yr1 - yr2) * 12;
   monthDiff += mo1 - mo2;
  
   *((float*)calcVar->data) = (float)monthDiff;
}


void ExpDecay(tEngVar *calcVar, tModelVar *modelVar)
{
   float value;

   value = *((float *)modelVar->args->var->data);

   if(modelVar->args->next)
      *((float*)calcVar->data) = (float)SAFE_EXP((double)-MAX(0.0, value / 
                                   *((float*)modelVar->args->next->var->data)));
   else
      *((float*)calcVar->data) = (float)SAFE_EXP((double)-MAX(0.0, value));
}


void Variance(tEngVar *calcVar, tModelVar *modelVar)
{
   float val1;
   float val2;
   float diff;

   val1 = *((float*)modelVar->args->var->data);
   val2 = *((float*)modelVar->args->next->var->data);

   diff = val1 - val2 * val2; 
   if(diff < 0)
      *((float*)calcVar->data) = 0.0;
   else
      *((float*)calcVar->data) = sqrt(diff);
}


void WhichBin(tEngVar *calcVar, tModelVar *modelVar)
{
   float value;
   float bin;

   value = *((float*)modelVar->args->var->data);
   bin = *((float*)modelVar->args->next->var->data);

   *((float*)calcVar->data) = (value == bin);
}



void GetDecimal(tEngVar *calcVar, tModelVar *modelVar)
{
   float  fltVal;
   
   fltVal = *((float*)modelVar->args->var->data); 
   
   *((float*)calcVar->data) = fltVal - (long)fltVal;
}


void CalcLog10(tEngVar *calcVar, tModelVar *modelVar)
{
   /* the function interpreat the following SAS statement */
   /* xlspdst = log10(max(xespdst,0) +1);                 */

   *((float*)calcVar->data) = 
       (float)log10((double)(MAX(*((float *)modelVar->args->var->data), 0) + 1)); 
}


void MaxShift(tEngVar *calcVar, tModelVar *modelVar)
{
   float *isvar;
   float *shift;
   float *pvar;
   float *bvar;
   float *mvar;
   float *events;
   
   float  new;
   float  coef;
   float  old;

   isvar = (float *)modelVar->args->var->data; 

   events = (float *)modelVar->args->next->var->data;
   shift  = (float *)modelVar->args->next->next->var->data;
   pvar   = (float *)modelVar->args->next->next->next->var->data;
   bvar   = (float *)modelVar->args->next->next->next->next->var->data;
   mvar   = (float *)calcVar->data;
   new  =  *((float*)modelVar->args->next->next->next->next->next->var->data);
   coef =  *((float*)modelVar->args->next->next->next->next->next->next->var->data);

   /* do initial shift */
   if(*shift > 1.0)
      *pvar = *bvar = *mvar;
   else if(*shift > 0.0)
   {
      *pvar = *bvar;
      *bvar = *mvar;
   }
   
   /* calc max */
   if(*isvar > 0.0)
   {  old = (*mvar) * coef;
      if(modelVar->args->next->next->next->next->next->next->next)
         new *= *((float*)modelVar->args->next->next->next->next->next->next->next->var->data);
      *mvar = (new > old) ? new : old;
   }
   else  
      *mvar = (*mvar) * coef;

   /* do shift if ?_EVENTS = 1 */
   if(*isvar > 0.0 && *events == 1.0)
      *pvar = *bvar = *mvar;
}


void Normalize(tEngVar *calcVar, tModelVar *modelVar)
{
   float quotient;
   float numer = *((float  *)modelVar->args->var->data);
   float denom = *((float  *)modelVar->args->next->var->data);
   float upper = *((float  *)modelVar->args->next->next->var->data);
   float deflt = *((float  *)modelVar->args->next->next->next->var->data);

   if(denom == 0.0)
   {
      if(numer == 0.0)
         *((float*)calcVar->data) = deflt;
      else
      {  /* set upper bound */
         *((float*)calcVar->data) = (numer > 0.0) ? upper : -(upper);
      }
   }
   else
   {  quotient = numer / denom;

      /* result will be capped at +/- upper */
      *((float*)calcVar->data) = MAX(-upper, MIN(quotient, upper));
   }
}


void AgeDecay(tEngVar *calcVar, tModelVar *modelVar)
{
   float    *out;
   float    *acctage;
   float    *lstage;
   float    *tim;
   float    *event;
   float    divval;
   tCalcVar *argList;
   
   /* this function will interpreat the following SAS statement */
   /* tslw = SAFE_EXP(-(acct_age - lstw_age)/&tim_6m)*(w_events > 0)*(x_events >0);*/

   argList = modelVar->args->next->next->next;
   out   = (float *)calcVar->data;

   /* Any event var is not greater than 0 return */
   while(argList)
   {
      event = (float *)argList->var->data;
      if(*event > 0)
         argList = argList->next;
      else
      {  *out = 0.0;
         return;
      }
   }

   acctage = (float  *)modelVar->args->var->data;
   lstage = (float  *)modelVar->args->next->var->data;
   tim = (float  *)modelVar->args->next->next->var->data;

   if((*tim) != 0.0) divval = (*acctage - *lstage)/(*tim);
   else divval = 0.0;
   
   *out = (float )SAFE_EXP(-((double)divval));
}


void MaxDecayCoefCalc(tEngVar *calcVar, tModelVar *modelVar)
{
   float  DelConNbr;
   float  ConTim;

   if(*((float *)modelVar->args->var->data) < 1.0)
   {  *((float*)calcVar->data) = 1.0;
      return;
   }

   DelConNbr = *((float  *)modelVar->args->next->var->data); 
   ConTim = *((float  *)modelVar->args->next->next->var->data); 

   *((float*)calcVar->data) = (float)SAFE_EXP((double)(-DelConNbr/ConTim));
}


#define WEEK_DAYS  7 
void DailyRateCoef1Calc(tEngVar *calcVar, tModelVar *modelVar)
{
   float  *ConstExp;
   float  *DecayTime;
   float  *DelTime;
   float  ProfAge, expVal;
   
   DecayTime = (float  *)modelVar->args->var->data; 
   ConstExp = (float  *)modelVar->args->next->var->data; 
   DelTime = (float  *)modelVar->args->next->next->var->data; 
   ProfAge = *((float  *)modelVar->args->next->next->next->var->data) + WEEK_DAYS; 

   expVal = 1.0 - (float)SAFE_EXP((double)(-(ProfAge)/(*DecayTime)));

   if(expVal != 0.0) *((float*)calcVar->data) = ((*ConstExp) / expVal);
   else *((float*)calcVar->data) = *ConstExp;
}


void DailyRateCoef2Calc(tEngVar *calcVar, tModelVar *modelVar)
{
   float  *DecayTime;
   float  *DelTime;
   float  ProfAge, expValNumer, expValDenom;
   
   DecayTime = (float  *)modelVar->args->var->data; 
   DelTime = (float  *)modelVar->args->next->var->data; 
   ProfAge = *((float  *)modelVar->args->next->next->var->data) + WEEK_DAYS; 

   expValNumer = (float)(1.0 - SAFE_EXP((double)(-(*DelTime)/(*DecayTime))));
   expValDenom = (float)(1.0 - SAFE_EXP((double)(-(ProfAge)/(*DecayTime))));

   if(expValDenom != 0.0) *((float*)calcVar->data) = (1.0 - (expValNumer/expValDenom));
   else *((float*)calcVar->data) = expValNumer;
}


void EventAvgCoef1Calc(tEngVar *calcVar, tModelVar *modelVar)
{
   float  *ConstantExp;
   float  *DecayEvents;
   float  *NbrEvents;
   double expVal;

   ConstantExp = (float  *)modelVar->args->var->data;
   DecayEvents = (float  *)modelVar->args->next->var->data; 
   NbrEvents = (float  *)modelVar->args->next->next->var->data; 

   expVal = (1.0 - SAFE_EXP((double)(-(*NbrEvents)/(*DecayEvents)))); 

   if(expVal != 0.0)
      *((float*)calcVar->data) = ((*ConstantExp) / (float)expVal);
   else
      *((float*)calcVar->data) = (*ConstantExp);
}


#define DO_ADD  1.0
#define DO_SUB  2.0
#define DO_MUL  3.0
#define DO_DIV  4.0
#define DO_EQU  5.0
#define DO_LOG  6.0

void DoArithmetic(tEngVar *calcVar, tModelVar *modelVar)
{
   float    fopr=0.0;
   float    var2;
   float    result=0.0;
   int      argCount=0;
   tCalcVar *arg=modelVar->args;
   
   while(arg)
   {  if(argCount%2 == 1)
         fopr = *((float  *)arg->var->data);
      else 
      {  if(fopr)
         {  var2 = *((float  *)arg->var->data);

            if(fopr == DO_ADD)
               result += var2;
            else if(fopr == DO_SUB)
               result -= var2;
            else if(fopr == DO_MUL)
               result *= var2;
            else if(fopr == DO_DIV)
               if(var2 != 0) result /= var2;
               else result = 0;
            else if(fopr == DO_LOG)
               if(var2 > 0.0) result = (float)log((double)(MAX(var2, 0)));
               else result = 0;
         }
         else
            result = *((float *)arg->var->data);
      }

      arg = arg->next;
      argCount++;
   }

   *((float*)calcVar->data) = result;
}


/*********************************************************/
/* This function expects a parameter which will indecate */
/* if the function should be performed or not            */
/*********************************************************/
void DoCondArithmetic(tEngVar *calcVar, tModelVar *modelVar)
{
   float    fopr=0.0;
   float    var2;
   float    result=0.0;
   int      argCount=0;
   tCalcVar *arg=modelVar->args;
   
   /* If first argument is zero, return now */
   if(*((float*)arg->var->data)==0.0)
      return;

   arg = arg->next;
   while(arg)
   {  if(argCount%2 == 1)
         fopr = *((float  *)arg->var->data);
      else 
      {  if(fopr)
         {  var2 = *((float  *)arg->var->data);

            if(fopr == DO_ADD)
               result += var2;
            else if(fopr == DO_SUB)
               result -= var2;
            else if(fopr == DO_MUL)
               result *= var2;
            else if(fopr == DO_DIV)
               if(var2 != 0) result /= var2;
               else result = 0;
            else if(fopr == DO_LOG)
               if(var2 > 0.0) result = (float)log((double)(MAX(var2, 0)));
               else result = 0;
         }
         else
            result = *((float *)arg->var->data);
      }

      arg = arg->next;
      argCount++;
   }

   *((float*)calcVar->data) = result;
}


void MaxDecay(tEngVar *calcVar, tModelVar *modelVar)
{
   float  new =  *((float  *)modelVar->args->next->var->data);
   float  coef = *((float  *)modelVar->args->next->next->var->data);
   float  old =  *((float  *)calcVar->data);

   old *= coef;
   if(modelVar->args->next->next->next)
      new *= *((float *)modelVar->args->next->next->next->var->data);

   if(*((float*)modelVar->args->var->data) > 0.0 && new > old)
      *((float*)calcVar->data) = new;
   else
      *((float*)calcVar->data) = old;
}


void DailyRate(tEngVar *calcVar, tModelVar *modelVar)
{
   float  isvar, multiplier=1, newVal, coef1, coef2, *old;

   isvar = *((float  *)modelVar->args->var->data);
   old = (float *)calcVar->data;

   /* if a fifth varable exists (it indicates a multipler exists */
   if(modelVar->args->next->next->next->next)
   {
      multiplier  = *((float  *)modelVar->args->next->next->var->data);
      coef1 = *((float  *)modelVar->args->next->next->next->var->data);
      coef2 = *((float  *)modelVar->args->next->next->next->next->var->data);
   }
   else
   {
      coef1 = *((float  *)modelVar->args->next->next->var->data);
      coef2 = *((float  *)modelVar->args->next->next->next->var->data);
   }

   if(isvar > 0.0)
   {  newVal  = *((float  *)modelVar->args->next->var->data) * multiplier;
      *old = (coef1 * newVal) + (coef2 * (*old));
   }
   else
      *old = (coef2 * (*old));
}


void DoNothing(tEngVar *calcVar, tModelVar *modelVar)
{
   return;
}


void EventAvg(tEngVar *calcVar, tModelVar *modelVar)
{
   float *value, new, coef1, coef2;

   if(*((float  *)modelVar->args->var->data) == 0.0)
      return;

   value  = (float  *)calcVar->data;
   new     = *((float  *)modelVar->args->next->var->data);
   coef1   = *((float  *)modelVar->args->next->next->var->data);
   coef2   = *((float  *)modelVar->args->next->next->next->var->data);

   *value = (coef1 * new) + (coef2 * (*value));
}


void CalcTableLookup(tEngVar *calcVar, tModelVar *modelVar)
{
   tTableTemplate   *table;
   short            shortKey;
   long             longKey;
   float            fltKey;
   tRecData         *inputRecord;

   /************************/
   /* Get the input record */
   /************************/
   inputRecord = ((tRecInstance*)calcVar->data)->data;

   /************************************************/
   /* Get the table and do the lookup for the type */ 
   /************************************************/
   table = (tTableTemplate*)inputRecord->record->user;
   if(table->keyType == HNC_INT32)
   {  switch(modelVar->args->var->type)
      {  case HNC_INT32:
            longKey = *((long*)modelVar->args->var->data);
            break;
         case HNC_INT16:
            longKey = (long)*((short*)modelVar->args->var->data);
            break;
         case HNC_FLOAT:
            longKey = (long)*((float*)modelVar->args->var->data);
            break;
         default:
            longKey = 0;
      }

      DoLongTableLookup(table, longKey, inputRecord); 
   }
   else if(table->keyType == HNC_INT16)
   {  switch(modelVar->args->var->type)
      {  case HNC_INT32:
            shortKey = (short)*((long*)modelVar->args->var->data);
            break;
         case HNC_INT16:
            shortKey = *((short*)modelVar->args->var->data);
            break;
         case HNC_FLOAT:
            shortKey = (short)*((float*)modelVar->args->var->data);
            break;
         default:
            shortKey = 0;
      }

      DoShortTableLookup(table, shortKey, inputRecord); 
   }
   else if(table->keyType == HNC_FLOAT)
   {  switch(modelVar->args->var->type)
      {  case HNC_INT32:
            fltKey = (float)*((long*)modelVar->args->var->data);
            break;
         case HNC_INT16:
            fltKey = (float)*((short*)modelVar->args->var->data);
            break;
         case HNC_FLOAT:
            fltKey = *((float*)modelVar->args->var->data);
            break;
         default:
            fltKey = 0;
      }

      DoFloatTableLookup(table, fltKey, inputRecord); 
   }
   else 
   {  DoStringTableLookup(table, (char*)modelVar->args->var->data, 
                               modelVar->args->var->dataSize, inputRecord); 
   }
}


void CalcTableRange(tEngVar *calcVar, tModelVar *modelVar)
{
   tTableTemplate   *table;
   short            shortKey;
   long             longKey;
   float            fltKey;
   tRecData         *inputRecord;

   /************************/
   /* Get the input record */
   /************************/
   inputRecord = ((tRecInstance*)calcVar->data)->data;

   /************************************************/
   /* Get the table and do the lookup for the type */ 
   /************************************************/
   table = (tTableTemplate*)inputRecord->record->user;
   if(table->keyType == HNC_INT32)
   {  switch(modelVar->args->var->type)
      {  case HNC_INT32:
            longKey = *((long*)modelVar->args->var->data);
            break;
         case HNC_INT16:
            longKey = (long)*((short*)modelVar->args->var->data);
            break;
         case HNC_FLOAT:
            longKey = (long)*((float*)modelVar->args->var->data);
            break;
         default:
            longKey = 0;
      }

      DoLongTableRange(table, longKey, inputRecord); 
   }
   else if(table->keyType == HNC_INT16)
   {  switch(modelVar->args->var->type)
      {  case HNC_INT32:
            shortKey = (short)*((long*)modelVar->args->var->data);
            break;
         case HNC_INT16:
            shortKey = *((short*)modelVar->args->var->data);
            break;
         case HNC_FLOAT:
            shortKey = (short)*((float*)modelVar->args->var->data);
            break;
         default:
            shortKey = 0;
      }

      DoShortTableRange(table, shortKey, inputRecord); 
   }
   else if(table->keyType == HNC_FLOAT)
   {  switch(modelVar->args->var->type)
      {  case HNC_INT32:
            fltKey = (float)*((long*)modelVar->args->var->data);
            break;
         case HNC_INT16:
            fltKey = (float)*((short*)modelVar->args->var->data);
            break;
         case HNC_FLOAT:
            fltKey = *((float*)modelVar->args->var->data);
            break;
         default:
            fltKey = 0;
      }

      DoFloatTableRange(table, fltKey, inputRecord); 
   }
   else 
   {  DoStringTableRange(table, (char*)modelVar->args->var->data, 
                               modelVar->args->var->dataSize, inputRecord); 
   }
}


void CalcTableDirect(tEngVar *calcVar, tModelVar *modelVar)
{
   tTableTemplate   *table;
   long             index=0;
   tRecData         *inputRecord;


   /************************/
   /* Get the input record */
   /************************/
   inputRecord = ((tRecInstance*)calcVar->data)->data;


   /**************************************************/
   /* Can only do a direct index with an integer key */
   /**************************************************/
   table = (tTableTemplate*)inputRecord->record->user;
   switch(modelVar->args->var->type)
   {  case HNC_INT32:
         index = *((long*)modelVar->args->var->data);
         break;
      case HNC_INT16:
         index = (long)*((short*)modelVar->args->var->data);
         break;
      case HNC_FLOAT:
         index = (long)*((float*)modelVar->args->var->data);
         break;
      default:
         index = 0;
   }


   /*************************************************************/
   /* Make sure index isn't greater than numRows or less than 0 */
   /*************************************************************/
   if(index > (table->numRows - 1) || index < 0)
   {  
#ifdef DEVELOP
         fprintf(stderr, "Index '%ld' is out of range for table '%s'\n",
                         index,
                         inputRecord->record->recName);
#endif
      if(table->defaultData)
         memcpy(*inputRecord->data, table->defaultData, table->dataLength);
   }
   else
      memcpy(*inputRecord->data, table->dataList[index], table->dataLength); 
}


void StrEql(tEngVar *calcVar, tModelVar *modelVar)
{
   char *str1 = (char*)modelVar->args->var->data;
   char *str2 = (char*)modelVar->args->next->var->data;

   *((float*)calcVar->data) = (str1[0] == str2[0]);
}


void FloatSet(tEngVar *calcVar, tModelVar *modelVar)
{
   *((float*)calcVar->data) = *((float*)modelVar->args->var->data);
}

void FloatAdd(tEngVar *calcVar, tModelVar *modelVar)
{
   float    sum=0.0;
   tCalcVar *arg = modelVar->args;

   for(; arg; arg=arg->next)
      sum += *((float*)arg->var->data);

   *((float*)calcVar->data) = sum;
}

void FloatSub(tEngVar *calcVar, tModelVar *modelVar)
{
   *((float*)calcVar->data) = 
           *((float*)modelVar->args->var->data) -
           *((float*)modelVar->args->next->var->data);
}

void FloatMul(tEngVar *calcVar, tModelVar *modelVar)
{
   float    prod=1.0;
   tCalcVar *arg = modelVar->args;

   for(; arg; arg=arg->next)
      prod *= *((float*)arg->var->data);

   *((float*)calcVar->data) = prod;
}

void FloatDiv(tEngVar *calcVar, tModelVar *modelVar)
{
   float denom = *((float*)modelVar->args->next->var->data);

   if(denom)
      *((float*)calcVar->data) = *((float*)modelVar->args->var->data) / denom; 
   else
      *((float*)calcVar->data) = 0.0;
}

void FloatEql(tEngVar *calcVar, tModelVar *modelVar)
{
   *((float*)calcVar->data) = 
           *((float*)modelVar->args->var->data) ==  
           *((float*)modelVar->args->next->var->data);
}

void FloatNotEql(tEngVar *calcVar, tModelVar *modelVar)
{
   *((float*)calcVar->data) = 
           *((float*)modelVar->args->var->data) != 
           *((float*)modelVar->args->next->var->data);
}

void FloatGrtr(tEngVar *calcVar, tModelVar *modelVar)
{
   *((float*)calcVar->data) = 
           *((float*)modelVar->args->var->data) > 
           *((float*)modelVar->args->next->var->data);
}

void FloatGrtrEql(tEngVar *calcVar, tModelVar *modelVar)
{
   *((float*)calcVar->data) = 
           *((float*)modelVar->args->var->data) >= 
           *((float*)modelVar->args->next->var->data);
}

void FloatLess(tEngVar *calcVar, tModelVar *modelVar)
{
   *((float*)calcVar->data) = 
           *((float*)modelVar->args->var->data) < 
           *((float*)modelVar->args->next->var->data);
}

void FloatLessEql(tEngVar *calcVar, tModelVar *modelVar)
{
   *((float*)calcVar->data) = 
           *((float*)modelVar->args->var->data) <= 
           *((float*)modelVar->args->next->var->data);
}

void FloatTrunc(tEngVar *calcVar, tModelVar *modelVar)
{
   *((float*)calcVar->data) = 
           (float)((long)*((float*)modelVar->args->var->data));
}

void FloatMin(tEngVar *calcVar, tModelVar *modelVar)
{
   tCalcVar  *arg = modelVar->args;
   float     val, minVal;

   minVal = *((float*)arg->var->data);
   for(arg=arg->next; arg; arg=arg->next)
   {  val = *((float*)arg->var->data);
      if(val < minVal) minVal = val;
   }

   *((float*)calcVar->data) = minVal; 
}

void FloatMax(tEngVar *calcVar, tModelVar *modelVar)
{
   tCalcVar  *arg = modelVar->args;
   float     val, maxVal;

   maxVal = *((float*)arg->var->data);
   for(arg=arg->next; arg; arg=arg->next)
   {  val = *((float*)arg->var->data);
      if(val > maxVal) maxVal = val;
   }

   *((float*)calcVar->data) = maxVal; 
}

void RateDecay(tEngVar *calcVar, tModelVar *modelVar)
{
   float    rate, time, age;

   time = *((float*)modelVar->args->var->data);
   rate = *((float*)modelVar->args->next->var->data);

   if(modelVar->args->next->next)
   {  age = *((float*)modelVar->args->next->next->var->data);

/*******  jcb   5/29/98  minimum account age should be one day *****/
      if ( age < 1.0) age = 1.0;

      rate = (rate > age) ? age : rate;
   }

   if(rate <= 0 || time <= 0) {
      *((float*)calcVar->data) = 1.0;
   } else {
      *((float*)calcVar->data) = (float)SAFE_EXP((double)(-time/rate));
   }
}


tAtom CalcSubList(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tModelVar    *modelVar;
   tCalcVar     *calcVar;
   tVarBlock    *varBlock;
   tAtom        retVal;
   long         status;
   tModelManage *modManage = 
      (tModelManage*)((tFuncInit*)funcCallData->inputData)->modelHandle;

   /* Find the calc block */
   for(varBlock=modManage->currModel->subBlockList; varBlock; varBlock=varBlock->next)
   {  if(!strcmp(varBlock->name, aParams[0].data.strVal.str))
         break;
   }

   /* If no matching calc type, that is bad */
   if(!varBlock)
   {   fprintf(stderr, "ERROR: CalcSubList with '%*.*s' does not exist\n",
                                                aParams[0].data.strVal.len,
                                                aParams[0].data.strVal.len,
                                                aParams[0].data.strVal.str);
       return(retVal);
   }
  
   /* Calculate all the variables in the list */
   for(calcVar=varBlock->calcList; calcVar; calcVar=calcVar->next)
   {  /* If variable is calculated by a rule, execute it */
      if(calcVar->var->calcRule)
      {  if(!(*calcVar->var->calcRule->calcDone))
         {  if((status = CalcVarRule(calcVar->var, modManage->currModel->modelRuleHandle)))
            {  ZERO(retVal.data.num);
               retVal.type = NUMERIC_TYPE;
               return retVal; 
            }
         }
      }

      /* Else call the variable calc function */
      else 
      {  modelVar = (tModelVar*)calcVar->var->user;
         if(!*(modelVar->calcDone))
         {  modelVar->func(calcVar->var, modelVar);
            *(modelVar->calcDone) = 1;
         }
      }
   }

   ZERO(retVal.data.num);
   retVal.type = NUMERIC_TYPE;

   return(retVal);
}


tAtom ZeroSubList(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tModelVar    *modelVar;
   tCalcVar     *calcVar;
   tVarBlock    *varBlock;
   tAtom        retVal;
   tModelManage *modManage = 
      (tModelManage*)((tFuncInit*)funcCallData->inputData)->modelHandle;

   /* Find the calc block */
   for(varBlock=modManage->currModel->subBlockList; varBlock; varBlock=varBlock->next)
   {  if(!strcmp(varBlock->name, aParams[0].data.strVal.str))
         break;
   }

   /* If no matching calc type, that is bad */
   if(!varBlock)
   {   fprintf(stderr, "ERROR: ZeroSubList with '%*.*s' does not exist\n",
                                                aParams[0].data.strVal.len,
                                                aParams[0].data.strVal.len,
                                                aParams[0].data.strVal.str);
       return(retVal);
   }
  
   /* Set a value of zero for all the numeric variables in the list */
   for(calcVar=varBlock->calcList; calcVar; calcVar=calcVar->next)
   {  modelVar = (tModelVar*)calcVar->var->user;
      if(!*(modelVar->calcDone))
      {  if(IsBinNumeric(calcVar->var->type))
            memset((char*)calcVar->var->data, 0, calcVar->var->dataSize);
         *(modelVar->calcDone) = 1;
      }
   }

   ZERO(retVal.data.num);
   retVal.type = NUMERIC_TYPE;

   return(retVal);
}


tAtom CalcVars(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   long         status;
   tModelVar    *modelVar;
   tCalcVar     *calcVar;
   tAtom        retVal;
   tVarBlock    *varBlock;
   tModelManage *modManage = 
      (tModelManage*)((tFuncInit*)funcCallData->inputData)->modelHandle;
  
   for(varBlock=modManage->currModel->mainBlockList; varBlock; varBlock=varBlock->next)
   {  for(calcVar=varBlock->calcList; calcVar; calcVar=calcVar->next)
      {  /* If variable is calculated by a rule, execute it */
         if(calcVar->var->calcRule)
         {  if(!(*calcVar->var->calcRule->calcDone))
            {  if((status = CalcVarRule(calcVar->var, modManage->currModel->modelRuleHandle)))
               {  ZERO(retVal.data.num);
                  retVal.type = NUMERIC_TYPE;
                  return retVal; 
               }
            }
         }

         /* Else call the variable calc function */
         else 
         {  modelVar = (tModelVar*)calcVar->var->user;
            if(!*(modelVar->calcDone))
            {  modelVar->func(calcVar->var, modelVar);
               *(modelVar->calcDone) = 1;
            }
         }
      }
   }

   ZERO(retVal.data.num);
   retVal.type = NUMERIC_TYPE;

   return(retVal);
}


long ValidateFunction(tModelVar *modelVar, short funcCode, char *fileName)
{
   tCalcVar *arg;
   int      argCt=1, badArg=0, errCode=0, expectArgs=0;
   char     errMsg[256];


   /*****************************************************/
   /* Set the function pointer, given the function code */
   /* Also, for each function, make sure that the right */
   /* number and type of variables in the arg list      */
   /*****************************************************/
   errMsg[0] = '\0';
   modelVar->funcCode = funcCode;
   switch(funcCode)
   {  case EVENT_AVG:
         modelVar->func = EventAvg;

         expectArgs=4;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
               case 2:
               case 3:
               case 4:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode) 
            {  badArg = argCt;
               break; 
            }
         }

         if(argCt-1 < expectArgs && !errCode)
            errCode = TOO_FEW_ARGS;
         
         break;
      case EVENT_COEF1_CALC:
         modelVar->func = EventAvgCoef1Calc;

         expectArgs=3;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
               case 3:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               case 2:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  else if(*((float*)arg->var->data) == 0.0)
                     errCode = ARG_NOT_NONZERO;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode) 
            {  badArg = argCt;
               break; 
            }
         }

         if(argCt-1 < expectArgs && !errCode)
            errCode = TOO_FEW_ARGS;

         break;
      case DAILY_RATE:
         modelVar->func = DailyRate;

         expectArgs=4;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
               case 2:
               case 3:
               case 4:
               case 5:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode) 
            {  badArg = argCt;
               break; 
            }
         }

         if(argCt-1 < expectArgs && !errCode)
            errCode = TOO_FEW_ARGS;

         break;
      case RATE_COEF1_CALC:
         modelVar->func = DailyRateCoef1Calc;

         expectArgs=4;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  else if(*((float*)arg->var->data) == 0.0)
                     errCode = ARG_NOT_NONZERO;
                  break;
               case 2:
               case 3:
               case 4:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode) 
            {  badArg = argCt;
               break; 
            }
         }

         if(argCt-1 < expectArgs && !errCode)
            errCode = TOO_FEW_ARGS;

         break;
      case RATE_COEF2_CALC:
         modelVar->func = DailyRateCoef2Calc;

         expectArgs=3;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  else if(*((float*)arg->var->data) == 0.0)
                     errCode = ARG_NOT_NONZERO;
                  break;
               case 2:
               case 3:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode) 
            {  badArg = argCt;
               break; 
            }
         }

         if(argCt-1 < expectArgs && !errCode)
            errCode = TOO_FEW_ARGS;

         break;
      case MAX_DECAY:
         modelVar->func = MaxDecay;

         expectArgs=3;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
               case 2:
               case 3:
               case 4:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode) 
            {  badArg = argCt;
               break; 
            }
         }

         if(argCt-1 < expectArgs && !errCode)
            errCode = TOO_FEW_ARGS;

         break;
      case MAX_COEF_CALC:
         modelVar->func = MaxDecayCoefCalc;

         expectArgs=3;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
               case 2:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               case 3:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  else if(*((float*)arg->var->data) == 0.0)
                     errCode = ARG_NOT_NONZERO;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode) 
            {  badArg = argCt;
               break; 
            }
         }

         if(argCt-1 < expectArgs && !errCode)
            errCode = TOO_FEW_ARGS;

         break;
      case RATE_DECAY:
         modelVar->func = RateDecay;

         expectArgs=2;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
               case 2:
               case 3:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode) 
            {  badArg = argCt;
               break; 
            }
         }

         if(argCt-1 < expectArgs && !errCode)
            errCode = TOO_FEW_ARGS;

         break;
      case MAX_SHIFT:
         modelVar->func = MaxShift;

         expectArgs=7;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
               case 2:
               case 3:
               case 4:
               case 5:
               case 6:
               case 7:
               case 8:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode) 
            {  badArg = argCt;
               break; 
            }
         }

         if(argCt-1 < expectArgs && !errCode)
            errCode = TOO_FEW_ARGS;

         break;
      case NORMALIZE:
         modelVar->func = Normalize;

         expectArgs=4;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
               case 2:
               case 3:
               case 4:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode) 
            {  badArg = argCt;
               break; 
            }
         }

         if(argCt-1 < expectArgs && !errCode)
            errCode = TOO_FEW_ARGS;

         break;
      case DO_NOTHING:
         modelVar->func = DoNothing;
         break;
      case DO_ARITHMETIC:
         modelVar->func = DoArithmetic;

         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  if(arg->var->type != HNC_FLOAT)
            {  errCode = ARG_NOT_FLOAT;
               break;
            }
            else if((argCt%2)==0)
            {  if(*((float*)arg->var->data) > 6)
               {  errCode = ARG_NOT_ARITH;
                  break;
               }
            }
         }

         break;
      case GET_DECIMAL:
         modelVar->func = GetDecimal;

         expectArgs=1;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode) 
            {  badArg = argCt;
               break; 
            }
         }

         if(argCt-1 < expectArgs && !errCode)
            errCode = TOO_FEW_ARGS;

         break;
      case LOG10:
         modelVar->func = CalcLog10;

         expectArgs=1;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode) 
            {  badArg = argCt;
               break; 
            }
         }

         if(argCt-1 < expectArgs && !errCode)
            errCode = TOO_FEW_ARGS;

         break;
      case AGE_DECAY:
         modelVar->func = AgeDecay;

         expectArgs=3;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
               case 2:
               case 3:
               case 4:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode) 
            {  badArg = argCt;
               break; 
            }
         }

         if(argCt-1 < expectArgs && !errCode)
            errCode = TOO_FEW_ARGS;

         break;
      case VARIANCE:
         modelVar->func = Variance;

         expectArgs=2;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
               case 2:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode)
            {  badArg = argCt;
               break;
            }
         }

         if(argCt-1 < expectArgs && !errCode)
            errCode = TOO_FEW_ARGS;

         break;
      case WHICH_BIN:
         modelVar->func = WhichBin;

         expectArgs=2;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
               case 2:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode)
            {  badArg = argCt;
               break;
            }
         }

         if(argCt-1 < expectArgs && !errCode)
            errCode = TOO_FEW_ARGS;

         break;
      case DO_COND_ARITHMETIC:
         modelVar->func = DoCondArithmetic;

         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  if(arg->var->type != HNC_FLOAT)
            {  errCode = ARG_NOT_FLOAT;
               break;
            }
            else if(argCt > 1 && ((argCt+1)%2)==0)
            {  if(*((float*)arg->var->data) > 6)
               {  errCode = ARG_NOT_ARITH;
                  break;
               }
            }
         }

         break;
      case EXP_DECAY:
         modelVar->func = ExpDecay;

         expectArgs=1;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               case 2:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  else if(*((float*)arg->var->data) == 0.0)
                     errCode = ARG_NOT_NONZERO;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode)
            {  badArg = argCt;
               break;
            }
         }

         if(argCt-1 < expectArgs && !errCode)
            errCode = TOO_FEW_ARGS;

         break;
      case MONTH_DIFF:
         modelVar->func = CalcMonthDiff;

         expectArgs=2;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
               case 2:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode)
            {  badArg = argCt;
               break;
            }
         }

         if(argCt-1 < expectArgs && !errCode)
            errCode = TOO_FEW_ARGS;

         break;
      case LOOKUP_TAB:
         modelVar->func = CalcTableLookup;

         expectArgs=1;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
                  if(arg->var->type != HNC_FLOAT && 
                     arg->var->type != HNC_INT32 && 
                     arg->var->type != HNC_INT16 && 
                     arg->var->type != HNC_STRING) 
                     errCode = ARG_NOT_FSL;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode)
            {  badArg = argCt;
               break;
            }
         }

         if(argCt-1 < expectArgs && !errCode)
            errCode = TOO_FEW_ARGS;

         break;
      case RANGE_TAB:
         modelVar->func = CalcTableRange;

         expectArgs=1;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
                  if(arg->var->type != HNC_FLOAT && 
                     arg->var->type != HNC_INT32 && 
                     arg->var->type != HNC_INT16 && 
                     arg->var->type != HNC_STRING) 
                     errCode = ARG_NOT_FSL;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode)
            {  badArg = argCt;
               break;
            }
         }

         if(argCt-1 < expectArgs && !errCode)
            errCode = TOO_FEW_ARGS;

         break;
      case DIRECT_TAB:
         modelVar->func = CalcTableDirect;

         expectArgs=1;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
                  if(arg->var->type != HNC_FLOAT && 
                     arg->var->type != HNC_INT32 && 
                     arg->var->type != HNC_INT16)
                     errCode = ARG_NOT_LONG;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode)
            {  badArg = argCt;
               break;
            }
         }

         if(argCt-1 < expectArgs && !errCode)
            errCode = TOO_FEW_ARGS;

         break;
      case STR_EQL:
         modelVar->func = StrEql;

         expectArgs=2;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
               case 2:
                  if(arg->var->type != HNC_STRING && arg->var->type != HNC_CHAR)
                     errCode = ARG_NOT_STRING;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode)
            {  badArg = argCt;
               break;
            }
         }

         if(argCt==1 && !errCode)
            errCode = TOO_FEW_ARGS;

         break;
      case FLOAT_TRUNC:
         modelVar->func = FloatTrunc;

         expectArgs=1;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode)
            {  badArg = argCt;
               break;
            }
         }

         if(argCt==1 && !errCode)
            errCode = TOO_FEW_ARGS;

         break;
      case FLOAT_LESSEQL:
         modelVar->func = FloatLessEql;

         expectArgs=2;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
               case 2:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode)
            {  badArg = argCt;
               break;
            }
         }

         if(argCt==1 && !errCode)
            errCode = TOO_FEW_ARGS;

         break;
      case FLOAT_LESS:
         modelVar->func = FloatLess;

         expectArgs=2;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
               case 2:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode)
            {  badArg = argCt;
               break;
            }
         }

         if(argCt==1 && !errCode)
            errCode = TOO_FEW_ARGS;

         break;
      case FLOAT_GRTREQL:
         modelVar->func = FloatGrtrEql;

         expectArgs=2;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
               case 2:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode)
            {  badArg = argCt;
               break;
            }
         }

         if(argCt==1 && !errCode)
            errCode = TOO_FEW_ARGS;

         break;
      case FLOAT_GRTR:
         modelVar->func = FloatGrtr;

         expectArgs=2;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
               case 2:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode)
            {  badArg = argCt;
               break;
            }
         }

         if(argCt==1 && !errCode)
            errCode = TOO_FEW_ARGS;

         break;
      case FLOAT_NOTEQL:
         modelVar->func = FloatNotEql;

         expectArgs=2;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
               case 2:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode)
            {  badArg = argCt;
               break;
            }
         }

         if(argCt==1 && !errCode)
            errCode = TOO_FEW_ARGS;

         break;
      case FLOAT_EQL:
         modelVar->func = FloatEql;

         expectArgs=2;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
               case 2:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode)
            {  badArg = argCt;
               break;
            }
         }

         if(argCt==1 && !errCode)
            errCode = TOO_FEW_ARGS;

         break;
      case FLOAT_DIV:
         modelVar->func = FloatDiv;

         expectArgs=2;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
               case 2:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode)
            {  badArg = argCt;
               break;
            }
         }

         if(argCt==1 && !errCode)
            errCode = TOO_FEW_ARGS;

         break;
      case FLOAT_MUL:
         modelVar->func = FloatMul;

         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  if(arg->var->type != HNC_FLOAT)
            {  errCode = ARG_NOT_FLOAT;
               break;
            }
         }

         if(errCode)
         {  badArg = argCt;
            break;
         }

         if(argCt<2 && !errCode)
            errCode = TOO_FEW_ARGS;

         break;
      case FLOAT_SUB:
         modelVar->func = FloatSub;

         expectArgs=2;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
               case 2:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode)
            {  badArg = argCt;
               break;
            }
         }

         if(argCt==1 && !errCode)
            errCode = TOO_FEW_ARGS;

         break;
      case FLOAT_ADD:
         modelVar->func = FloatAdd;

         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  if(arg->var->type != HNC_FLOAT)
            {  errCode = ARG_NOT_FLOAT;
               break;
            }
         }

         if(errCode)
         {  badArg = argCt;
            break;
         }

         if(argCt<2 && !errCode)
            errCode = TOO_FEW_ARGS;

         break;
      case FLOAT_SET:
         modelVar->func = FloatSet;

         expectArgs=1;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode)
            {  badArg = argCt;
               break;
            }
         }

         if(argCt==1 && !errCode)
            errCode = TOO_FEW_ARGS;

         break;
      case FLOAT_MAX:
         modelVar->func = FloatMax;

         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {   if(arg->var->type != HNC_FLOAT)
             {  errCode = ARG_NOT_FLOAT;
                break;
             }
         }

         if(errCode)
         {  badArg = argCt;
            break;
         }

         if(argCt==1 && !errCode)
            errCode = TOO_FEW_ARGS;

         break;
      case FLOAT_MIN:
         modelVar->func = FloatMin;

         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {   if(arg->var->type != HNC_FLOAT)
             {  errCode = ARG_NOT_FLOAT;
                break;
             }
         }

         if(errCode)
         {  badArg = argCt;
            break;
         }

         if(argCt==1 && !errCode)
            errCode = TOO_FEW_ARGS;

         break;
      default:
         ValidateProductFunction(modelVar, funcCode, fileName, errMsg);
         break;
   }


   /********************************/
   /* Set the error string, if any */
   /********************************/
   if(errCode)
   {  switch(errCode)
      {  case ARG_NOT_FLOAT:
            sprintf(errMsg, "Argument #%d to function '%d' for "
                            "variable '%s' in '%s' "
                            "must be an HNC_FLOAT(11) type variable",
                            badArg, funcCode, modelVar->cfgName, fileName); 
            break;
         case ARG_NOT_DOUBLE:
            sprintf(errMsg, "Argument #%d to function '%d' for "
                            "variable '%s' in '%s' "
                            "must be an HNC_DOUBLE(12) type variable",
                            badArg, funcCode, modelVar->cfgName, fileName); 
            break;
         case ARG_NOT_FSL:
            sprintf(errMsg, "Argument #%d to function '%d' for "
                            "variable '%s' in '%s' must be an HNC_INT32(9), "
                            "HNC_FLOAT(11), or HNC_STRING(3) type variable",
                            badArg, funcCode, modelVar->cfgName, fileName); 
            break;
         case ARG_NOT_STRING:
            sprintf(errMsg, "Argument #%d to function '%d' for "
                            "variable '%s' in '%s' must be an "
                            "HNC_STRING(3) type variable",
                            badArg, funcCode, modelVar->cfgName, fileName); 
            break;
         case ARG_NOT_LONG:
            sprintf(errMsg, "Argument #%d to function '%d' for "
                            "variable '%s' in '%s' "
                            "must be an HNC_INT32(9) type variable",
                            badArg, funcCode, modelVar->cfgName, fileName); 
            break;
         case ARG_NOT_RECORD:
            sprintf(errMsg, "Argument #%d to function '%d' for "
                            "variable '%s' in '%s' "
                            "must be an record variable",
                            badArg, funcCode, modelVar->cfgName, fileName); 
            break;
         case ARG_NOT_NONZERO:
            sprintf(errMsg, "Argument #%d to function '%d' for "
                            "variable '%s' in '%s' "
                            "must be a non-zero constant value",
                            badArg, funcCode, modelVar->cfgName, fileName); 
            break;
         case TOO_MANY_ARGS:
            sprintf(errMsg, "Wrong number of parameters passed to "
                            "function '%d' for variable '%s' in '%s'.  "
                            "Expecting a minimum of %d",   
                             funcCode, modelVar->cfgName, fileName, expectArgs);
            break;
         case TOO_FEW_ARGS:
            sprintf(errMsg, "Wrong number of parameters passed to "
                            "function '%d' for variable '%s' in '%s'.  "
                            "Expecting a minimum of %d, and received only %d",   
                            funcCode, modelVar->cfgName, fileName, expectArgs, argCt);
            break;
         case ARG_NOT_ARITH:
            sprintf(errMsg, "Argument #%d to function '%d' for "
                            "variable '%s' in '%s' "
                            "must be ADD, SUB, MUL, DIV, or EQ",
                            badArg, funcCode, modelVar->cfgName, fileName); 
            break;
      }
   }


   /****************************************************************/
   /* If the variable is shared, then all arguments must be either */
   /* shared variables, or they must be constants.                 */
   /****************************************************************/
   if(modelVar->isShared)
   {  for(arg=modelVar->args; arg; arg=arg->next, argCt++)
      {  if(arg->var)
         {  if(arg->var->user)
            {  if(!((tModelVar*)arg->var->user)->isShared)   
               {  sprintf(errMsg, "Argument #%d to function '%d' for "
                                  "variable '%s' in '%s' "
                                  "must be a shared variable",
                                  argCt, funcCode, modelVar->cfgName, fileName); 
                  break;
               }
            }
         }
      }
   }

   
   if(errMsg[0] != '\0')
   {  SetConfigErrorMessage(errMsg);
      throw(CONFIGURATION_EXCEPTION);
   }
      
   return(0);
}
