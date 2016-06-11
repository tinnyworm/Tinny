#ifndef HNC_FUNCS_H
#define HNC_FUNCS_H

/*
** $Id: rulfunc.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: rulfunc.h,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.2  1997/01/17  17:22:36  can
 * Updates for latest release
 *
**
*/


#include "engine.h"

typedef struct sFuncInit {
   int  DbHandle;
   void *modelHandle;
   void *dynTableList;
} tFuncInit;


long AllocFuncName(char **funcName, char *name);

void GetGeneralRuleFuncs(tFuncInit *funcInit, tFunc **funcList);
void FireModelCreate(tFuncInit *funcInit, tFunc **funcList);
void InitProfilesCreate(tFuncInit *funcInit, tFunc **funcList);
void ResetModelsCreate(tFuncInit *funcInit, tFunc **funcList);

tAtom GetFraction(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom Power(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom Round(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom Sqrt(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom Log(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom Log10(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom Mod(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom Div(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom Left(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom Right(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom Mid(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom ValidAmount(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom Trunc(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom Exp(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom Min(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom Max(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom Abs(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom IsMissing(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom IsIntString(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom IsDigit(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom IsAlpha(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom IsAlnum(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom Writeln(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom Write(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom Print(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom StringToNum(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);

#endif
