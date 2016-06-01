#ifndef EVLARG_H
#define EVLARG_H

/*
** $Id: evlarg.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: evlarg.h,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 1.1  1997/01/16  22:44:06  can
 * Initial revision
 *
 *
*/

#include "calctab.h"


/* API to create the list of argument functions */
long CreateArgFuncArray(tCalcTable *calcTab);
long ResetArgFuncs(void *handle);

/* API to the the tArgFunc structure given a function */
tArgFunc* GetArgFunc(tCalcTable *calcTab, tfArgEval EvalFunc);
tArgFunc* GetArgFuncName(tCalcTable *calcTab, char *funcName);


/* Non NOT or NEGATE adjusted argument calcs */
void EvalErrorStr(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalVarCalcData(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab); 
void EvalStringVarCalc(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab); 
void EvalFldString(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalFldData(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalAsIs(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalParamData(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalStringParam(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalArrayProc(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalResolve(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalErrorNum(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalExists(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalSizeof(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalRBExec(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalLinkedFunc(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalRule(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalStringIn(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalNumericIn(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalArrayVar(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalStr2Num(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);

void EvalNumericVarCalc(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab); 
void EvalFloatVarCalc(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab); 
void EvalDoubleVarCalc(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab); 
void EvalShortVarCalc(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab); 
void EvalUShortVarCalc(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab); 
void EvalLongVarCalc(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab); 
void EvalULongVarCalc(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab); 
void EvalNumericParam(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalFloatParam(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalDoubleParam(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalShortParam(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalUShortParam(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalLongParam(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalULongParam(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalNumericAddr(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalFloatAddr(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalDoubleAddr(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalShortAddr(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalUShortAddr(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalLongAddr(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalULongAddr(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalNumericFld(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalFloatFld(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalDoubleFld(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalShortFld(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalUShortFld(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalLongFld(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalULongFld(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);



/* Copies of above functions, but with NOT or NEGATE adjustments */
void EvalArrayProcAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalResolveAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalErrorNumAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalExistsAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalSizeofAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalRBExecAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalLinkedFuncAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalRuleAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalStringInAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalNumericInAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalArrayVarAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalStr2NumAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalNumericVarCalcAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab); 
void EvalFloatVarCalcAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab); 
void EvalDoubleVarCalcAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab); 
void EvalShortVarCalcAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab); 
void EvalUShortVarCalcAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab); 
void EvalLongVarCalcAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab); 
void EvalULongVarCalcAdj(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab); 

void EvalNumericParamNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalFloatParamNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalDoubleParamNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalShortParamNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalUShortParamNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalLongParamNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalULongParamNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalNumericAddrNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalFloatAddrNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalDoubleAddrNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalShortAddrNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalUShortAddrNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalLongAddrNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalULongAddrNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalNumericFldNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalFloatFldNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalDoubleFldNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalShortFldNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalUShortFldNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalLongFldNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalULongFldNot(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);

void EvalNumericParamNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalFloatParamNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalDoubleParamNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalShortParamNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalUShortParamNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalLongParamNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalULongParamNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalNumericAddrNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalFloatAddrNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalDoubleAddrNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalShortAddrNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalUShortAddrNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalLongAddrNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalULongAddrNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalNumericFldNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalFloatFldNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalDoubleFldNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalShortFldNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalUShortFldNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalLongFldNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalULongFldNeg(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);


#endif
