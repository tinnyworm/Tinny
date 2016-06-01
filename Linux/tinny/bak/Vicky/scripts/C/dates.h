#ifndef DATES_H
#define DATES_H

/*
** $Id: dates.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: dates.h,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.2  1997/01/17  17:22:36  can
 * New functions for latest release.
 *
**
*/


/* HNC Rule Engine includes */
#include "engine.h"

/* HNC common includes */
#include "rulfunc.h"


#define _1DAYSECS  86400
#define _1HOURSECS  3600
#define _1DAYHOURS    24

long GetYrMoDayFromDay90(float day90, int *yr, int *mo, int *day);
long GetDay90(char *dateString);
long GetTimeInSecs(char *timeString);

void ValidDateCreate(tFuncInit *funcInit, tFunc **funcList);
void ValidTimeCreate(tFuncInit *funcInit, tFunc **funcList);
void DayOfWeekCreate(tFuncInit *funcInit, tFunc **funcList);
void TimeConvertCreate(tFuncInit *funcInit, tFunc **funcList);
void DateConvertCreate(tFuncInit *funcInit, tFunc **funcList);
void DateTimeConvertCreate(tFuncInit *funcInit, tFunc **funcList);
void IsWeekDayCreate(tFuncInit *funcInit, tFunc **funcList);
void DateDiffCreate(tFuncInit *funcInit, tFunc **funcList);
void MonthDiffCreate(tFuncInit *funcInit, tFunc **funcList);
void Day90ToStringCreate(tFuncInit *funcInit, tFunc **funcList);

tAtom ValidTime(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom ValidDateFunc(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom DayOfWeek(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom TimeConvert(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom DateConvert(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom DateTimeConvert(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom IsWeekDay(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom DateDiff(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom MonthDiff(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom Day90ToString(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);


#endif
