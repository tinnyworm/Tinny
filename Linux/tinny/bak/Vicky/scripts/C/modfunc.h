#ifndef HNC_MODEL_FUNCS_H
#define HNC_MODEL_FUNCS_H

/*
** $Id: modfunc.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: modfunc.h,v $
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


#include "rulfunc.h"
#include "model.h"

long LoadModelFunctions(tFuncInit *funcInit, tFunc **funcList);

tAtom GetReasons(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom GetReasonsValue(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom ProfInit(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom DumpData(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom ScoreModel(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom CalcRisk(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom CalcZ(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom ProbNorm(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom GetGMTOffset(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);

#endif
