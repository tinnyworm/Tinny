#ifndef FALMFNC_H
#define FALMFNC_H 

#include "prdmfnc.h"

long LoadFalconModelFunctions(tFuncInit *funcInit, tFunc **funcList);

tAtom UnpackZip(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom PackZip(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom OldUnpackZip(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom OldPackZip(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);

#endif
