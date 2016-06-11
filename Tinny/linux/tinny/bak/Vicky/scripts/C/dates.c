/*
** $Id: dates.c,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
**
*/

/* System include files */
#include <stdio.h>
#include <stdlib.h>

/* HNC Rule Engine includes */ 
#include "mymall.h"

/* HNC common includes */
#include "dates.h"
#include "hncrtyp.h"


static int endofmonth[12] = {
 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365
};

#define NUM_DAYS_IN_YEAR(i) (((i%4)==0) ? 366 : 365)
long GetYrMoDayFromDay90(float fltDay90, int *year, int *month, int *day)
{
   long nd,daytmp,day90=(long)fltDay90;
   int i,iyear,index,iday,leapyear,incleap;

   iyear=1990;
   i=2;
   daytmp=day90+1;

   while(daytmp > (nd=NUM_DAYS_IN_YEAR(i)))
   {  daytmp=daytmp-nd;
      i++;
      iyear++;
   }

   *year=iyear;

   index=0;
   incleap=0;
   if((i/4)*4==i) leapyear=1;
   else leapyear=0;

   while (daytmp > (long)(endofmonth[index]+incleap))
   {  index++;
      if(leapyear) incleap=1;
   }

   *month=index+1;

   if(index==0)
   {
     iday=daytmp;
   }
   else
   {  if(index==1) iday = daytmp-(long)(endofmonth[0]);
      else iday = daytmp-(long)(endofmonth[index-1]+incleap);
   }
   *day = iday;

   return(0);
}


long GetTimeInSecs(char* timeStr)
{
   int   hours, minutes, seconds;
   long  daySecs;

   hours   = (*(timeStr+0)-'0')*10 + *(timeStr+1)-'0';
   minutes = (*(timeStr+2)-'0')*10 + *(timeStr+3)-'0';
   seconds = (*(timeStr+4)-'0')*10 + *(timeStr+5)-'0';

   if(hours < 0 || hours > 23)
      hours = 0;
   else if(minutes < 0 || minutes > 59)
      minutes = 0;
   else if(seconds < 0 || seconds > 59) 
      seconds = 0;

   /*************************************************************/
   /* Calculate the total number of seconds since last midnight */
   /*************************************************************/
   daySecs = (float)hours*_1HOURSECS + (float)minutes*60L + (float)seconds;

   /*********************/
   /* Sanity check time */
   /*********************/

   return(daySecs);
}


static int monthDays[12] = {
 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};
long GetDay90(char* dateStr)
{
   int    i, year, prevYear, month, prevMonth, date;
   long  day90;

   for(year=0, i=0; i<4; i++)
   {  if(*dateStr<'0' || *dateStr>'9') return(-1);
      year = (year * 10) + (*dateStr++ -'0');
   }
   for(month=0, i=0; i<2; i++)
   {  if(*dateStr<'0' || *dateStr>'9') return(-1);
      month = (month * 10) + (*dateStr++ -'0');
   }
   for(date=0, i=0; i<2; i++)
   {  if(*dateStr<'0' || *dateStr>'9') return(-1);
      date = (date * 10) + (*dateStr++ -'0');
   }

   if(month < 1 || month > 12)
      return(0);
   else if(date<1 || date>(*(monthDays+(month-1))))
   {  if(!(date==29 && month==2 && (year%4)==0))
         return(0);
   }

   if(year<1990)
   {  for(day90=0, prevYear=1989; prevYear>year; prevYear--)
        day90 -= ((prevYear%4) ? 365L : 366L);
      for(prevMonth=11; (prevMonth+1)>month; prevMonth--)
      {  if (prevMonth==1 && (year%4)==0)
           day90 -= *(monthDays+prevMonth)+1;
         else
           day90 -= *(monthDays+prevMonth);
      }
      if ((year%4)==0 && month==2)
         day90 -= (*(monthDays+(month-1))+2) - date;
      else
         day90 -= (*(monthDays+(month-1))+1) - date;
   }
   else
   {  for(day90=0, prevYear=1990; prevYear<year; prevYear++)
         day90 += ((prevYear%4) ? 365L : 366L);
      for(prevMonth=0; (prevMonth+1)<month; prevMonth++)
      {  if (prevMonth==1 && (prevYear%4)==0)
            day90 += *(monthDays+prevMonth)+1;
         else
            day90 += *(monthDays+prevMonth);
      }

      day90 += date-1;
   }

   return(day90);
}


tAtom Day90ToString(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom eval;
   int   year, month, day; 

   /* Get the year month day */
   GetYrMoDayFromDay90(NUM_GETFLT(aParams[0].data.num), &year, &month, &day);

   /* Make the string */
   sprintf(aParams[1].data.strVal.str, "%4.4d%2.2d%2.2d", year, month, day); 

   ZERO(eval.data.num) 
   eval.type = NUMERIC_TYPE;

   return(eval);
}


tAtom IsWeekDay(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom eval;
   long  dow;
   float date, hrTime;

   eval.type = NUMERIC_TYPE;

   date = NUM_GETFLT(aParams[0].data.num);

   dow = (((long)(date+1)%7)+1);

   hrTime = (date - (long)date) * _1DAYHOURS;
   if((dow == 6 && hrTime >= 18) || dow == 7 || dow == 1) 
      ZERO(eval.data.num)
   else
      ONE(eval.data.num)

   return(eval);
}


tAtom ValidDateFunc(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom  eval;
   int    i, year, month, date;
   char   *dateStr;

   eval.type = NUMERIC_TYPE;
   dateStr = aParams[0].data.strVal.str;

   ZERO(eval.data.num);

   for(year=0, i=0; i<4; i++)
   {  if(*dateStr<'0' || *dateStr>'9') return(eval);
      year = (year * 10) + (*dateStr++ -'0');
   }
   for(month=0, i=0; i<2; i++)
   {  if(*dateStr<'0' || *dateStr>'9') return(eval);
      month = (month * 10) + (*dateStr++ -'0');
   }
   for(date=0, i=0; i<2; i++)
   {  if(*dateStr<'0' || *dateStr>'9') return(eval);
      date = (date * 10) + (*dateStr++ -'0');
   }

   if(year<1900 || month<1 || month>12) 
         return(eval);
   else if(date<1 || date>(*(monthDays+(month-1))))
   {  if(!(date==29 && month==2 && (year%4)==0))
         return(eval);
   }
   
   ONE(eval.data.num);
   return(eval);
}


tAtom ValidTime(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom eval;
   int   hours, minutes, seconds;
   char  *timeStr;

   eval.type = NUMERIC_TYPE;

   timeStr = aParams[0].data.strVal.str;

   hours   = (*(timeStr+0)-'0')*10 + *(timeStr+1)-'0';
   minutes = (*(timeStr+2)-'0')*10 + *(timeStr+3)-'0';
   seconds = (*(timeStr+4)-'0')*10 + *(timeStr+5)-'0';

   /*********************************************************************/
   /* Barces are necessary around the single line statements, otherwise */ 
   /* otherwise  the compiler gets "parse error before `else'"; Perhaps */
   /* because ZERO is defined as a macro of multiple lines              */
   /*********************************************************************/
   if ( hours < 0 || hours > 23 || minutes < 0 || minutes > 59 || seconds < 0 || seconds > 59)
      ZERO(eval.data.num)
   else
      ONE(eval.data.num)

   return(eval);
}


tAtom DateDiff(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom eval;

   eval.type = NUMERIC_TYPE;
   NUM_SETINT32(eval.data.num,
                  (long)(GetDay90(aParams[0].data.strVal.str) - 
                         GetDay90(aParams[1].data.strVal.str)))

   return(eval);
}


tAtom MonthDiff(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom eval;
   long  i, yr1=0, yr2=0, mo1=0, mo2=0, months;
   char  *dateStr1 = aParams[0].data.strVal.str;
   char  *dateStr2 = aParams[1].data.strVal.str;

   for(i=0; i<4; i++)
   {  yr1 = (yr1 * 10) + (dateStr1[i] - '0');
      yr2 = (yr2 * 10) + (dateStr2[i] - '0');
   }
     
   for(i=4; i<6; i++)
   {  mo1 = (mo1 * 10) + (dateStr1[i] - '0');
      mo2 = (mo2 * 10) + (dateStr2[i] - '0');
   }

   months = (yr1 - yr2) * 12;
   months += mo1 - mo2;

   eval.type = NUMERIC_TYPE;
   NUM_SETINT32(eval.data.num, months); 

   return(eval);
}


tAtom DateConvert(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom eval;

   eval.type = NUMERIC_TYPE;
   NUM_SETFLT(eval.data.num,
            (float)GetDay90(aParams[0].data.strVal.str))

   return(eval);
}


tAtom TimeConvert(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom eval;

   eval.type = NUMERIC_TYPE;
   NUM_SETFLT(eval.data.num,
            ((float)GetTimeInSecs(aParams[0].data.strVal.str)))

   return(eval);
}


tAtom DateTimeConvert(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom eval;

   eval.type = NUMERIC_TYPE;
   NUM_SETFLT(eval.data.num,
            (float)GetDay90(aParams[0].data.strVal.str) + 
            ((float)GetTimeInSecs(aParams[1].data.strVal.str))/((float)_1DAYSECS))

   return(eval);
}


tAtom DayOfWeek(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom eval;
   float date;

   date = NUM_GETFLT(aParams[0].data.num);
   NUM_SETINT32(eval.data.num, (((long)(date+1)%7)+1)) 

   eval.type = NUMERIC_TYPE;
   return(eval);
}


void ValidTimeCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (1,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (1,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Valid_Time");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 1;
   typeElem[0].type = HNC_TIMESTR;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = typeElem;
   paramList[0].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr = ValidTime;
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


void ValidDateCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (1,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (1,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Valid_Date");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 1;
   typeElem[0].type = HNC_DATESTR;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = typeElem;
   paramList[0].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr = ValidDateFunc;
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


void DateTimeConvertCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (2,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (2,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Date_Time_Convert");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 2;
   typeElem[0].type = HNC_DATESTR;
   typeElem[0].next = NULL;
   typeElem[1].type = HNC_TIMESTR;
   typeElem[1].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = typeElem;
   paramList[0].next = paramList+1;
   paramList[1].reference = 0;
   paramList[1].list = typeElem+1;
   paramList[1].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr = DateTimeConvert;
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


void DateConvertCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (1,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (1,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Date_Convert");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 1;
   typeElem[0].type = HNC_DATESTR;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = typeElem;
   paramList[0].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr = DateConvert;
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


void TimeConvertCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (1,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (1,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Time_Convert");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 1;
   typeElem[0].type = HNC_TIMESTR;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = typeElem;
   paramList[0].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr = TimeConvert;
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


void IsWeekDayCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (1,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (1,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "IS_WEEK_DAY");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 1;
   funcElem->returnType = NUMERIC_TYPE;

   typeElem[0].type = NUMERIC_TYPE;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = typeElem;
   paramList[0].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->FuncPtr = IsWeekDay;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}


void DateDiffCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (2,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (2,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Date_Diff");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 2;
   funcElem->returnType = NUMERIC_TYPE;

   typeElem[0].type = HNC_DATESTR;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = typeElem;
   paramList[0].next = paramList+1;
   typeElem[1].type = HNC_DATESTR;
   typeElem[1].next = NULL;
   paramList[1].reference = 0;
   paramList[1].list = typeElem+1;
   paramList[1].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->FuncPtr = DateDiff;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}


void MonthDiffCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (2,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (2,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Month_Diff");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 2;
   funcElem->returnType = NUMERIC_TYPE;

   typeElem[0].type = HNC_DATESTR;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = typeElem;
   paramList[0].next = paramList+1;
   typeElem[1].type = HNC_DATESTR;
   typeElem[1].next = NULL;
   paramList[1].reference = 0;
   paramList[1].list = typeElem+1;
   paramList[1].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->FuncPtr = MonthDiff;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}


void DayOfWeekCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (1,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (1,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Day_Of_Week");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 1;
   funcElem->returnType = NUMERIC_TYPE;

   typeElem[0].type = NUMERIC_TYPE;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = typeElem;
   paramList[0].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->FuncPtr = DayOfWeek;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}


void Day90ToStringCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (2,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (2,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Day90_To_String");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 2;
   funcElem->returnType = NUMERIC_TYPE;

   typeElem[0].type = NUMERIC_TYPE;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = typeElem;
   paramList[0].next = paramList+1;

   typeElem[1].type = HNC_STRING;
   typeElem[1].next = NULL;
   paramList[1].reference = 0;
   paramList[1].list = typeElem+1;
   paramList[1].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->FuncPtr = Day90ToString;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}
