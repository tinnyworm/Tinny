#ifndef VARCALC_H
#define VARCALC_H

/*
** $Id: varcalc.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: varcalc.h,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.3  1997/01/25  07:18:39  can
 * Added RATE_DECAY function as previous missing function #7
 *
 * Revision 2.2  1997/01/17  17:22:36  can
 * Updates for latest release
 *
**
*/


#include "engine.h"
#include "model.h"

typedef enum eFuncCode {
   EVENT_AVG=1,
   DAILY_RATE,
   MAX_DECAY,
   NORMALIZE,
   AGE_DECAY,
   DO_NOTHING,
   RATE_DECAY,
   MAX_SHIFT,
   DO_ARITHMETIC,
   EVENT_COEF1_CALC,
   RATE_COEF1_CALC,
   RATE_COEF2_CALC,
   MAX_COEF_CALC,
   LOG10,
   GET_DECIMAL,
   WHICH_BIN,
   DO_COND_ARITHMETIC,
   VARIANCE,
   EXP_DECAY,
   MONTH_DIFF,
   LOOKUP_TAB,
   DIRECT_TAB,
   RANGE_TAB,
   STR_EQL,
   FLOAT_SET,
   FLOAT_ADD,
   FLOAT_SUB,
   FLOAT_MUL,
   FLOAT_DIV,
   FLOAT_EQL,
   FLOAT_NOTEQL,
   FLOAT_GRTR,
   FLOAT_GRTREQL,
   FLOAT_LESS,
   FLOAT_LESSEQL,
   FLOAT_TRUNC,
   FLOAT_MAX,
   FLOAT_MIN,
 
   LAST_CALC
} tFuncCode;


/* Error codes for parsing function arguments */
#define  ARG_NOT_FLOAT     1
#define  ARG_NOT_LONG      2
#define  ARG_NOT_DOUBLE    3
#define  ARG_NOT_STRING    4
#define  ARG_NOT_RECORD    5
#define  ARG_NOT_FSL       6
#define  ARG_NOT_NONZERO   7
#define  ARG_NOT_SHARED    8
#define  TOO_MANY_ARGS     9
#define  TOO_FEW_ARGS      10 
#define  ARG_NOT_ARITH     11


/* Prototypes for API functions */
tAtom CalcVars(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom CalcSubList(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom ZeroSubList(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
long  ValidateFunction(tModelVar *modelVar, short funcCode, char *modelName);


/* Prototypes for calc functions */
void CalcMonthDiff(tEngVar *calcVar, tModelVar *modelVar);
void ExpDecay(tEngVar *calcVar, tModelVar *modelVar);
void RateDecay(tEngVar *calcVar, tModelVar *modelVar);
void Variance(tEngVar *calcVar, tModelVar *modelVar);
void WhichBin(tEngVar *calcVar, tModelVar *modelVar);
void GetDecimal(tEngVar *calcVar, tModelVar *modelVar);
void CalcLog10(tEngVar *calcVar, tModelVar *modelVar);
void MaxShift(tEngVar *calcVar, tModelVar *modelVar);
void Normalize(tEngVar *calcVar, tModelVar *modelVar);
void AgeDecay(tEngVar *calcVar, tModelVar *modelVar);
void MaxDecayCoefCalc(tEngVar *calcVar, tModelVar *modelVar);
void DailyRateCoef1Calc(tEngVar *calcVar, tModelVar *modelVar);
void DailyRateCoef2Calc(tEngVar *calcVar, tModelVar *modelVar);
void EventAvgCoef1Calc(tEngVar *calcVar, tModelVar *modelVar);
void DoArithmetic(tEngVar *calcVar, tModelVar *modelVar);
void DoCondArithmetic(tEngVar *calcVar, tModelVar *modelVar);
void MaxDecay(tEngVar *calcVar, tModelVar *modelVar);
void DailyRate(tEngVar *calcVar, tModelVar *modelVar);
void DoNothing(tEngVar *calcVar, tModelVar *modelVar);
void EventAvg(tEngVar *calcVar, tModelVar *modelVar);
void CalcTableLookup(tEngVar *calcVar, tModelVar *modelVar);
void CalcTableDirect(tEngVar *calcVar, tModelVar *modelVar);
void CalcTableRange(tEngVar *calcVar, tModelVar *modelVar);
void StrEql(tEngVar *calcVar, tModelVar *modelVar);
void FloatSet(tEngVar *calcVar, tModelVar *modelVar);
void FloatAdd(tEngVar *calcVar, tModelVar *modelVar);
void FloatSub(tEngVar *calcVar, tModelVar *modelVar);
void FloatMul(tEngVar *calcVar, tModelVar *modelVar);
void FloatDiv(tEngVar *calcVar, tModelVar *modelVar);
void FloatEql(tEngVar *calcVar, tModelVar *modelVar);
void FloatNotEql(tEngVar *calcVar, tModelVar *modelVar);
void FloatGrtr(tEngVar *calcVar, tModelVar *modelVar);
void FloatGrtrEql(tEngVar *calcVar, tModelVar *modelVar);
void FloatLess(tEngVar *calcVar, tModelVar *modelVar);
void FloatLessEql(tEngVar *calcVar, tModelVar *modelVar);
void FloatTrunc(tEngVar *calcVar, tModelVar *modelVar);
void FloatMax(tEngVar *calcVar, tModelVar *modelVar);
void FloatMin(tEngVar *calcVar, tModelVar *modelVar);


#endif
