#ifndef FALCALC_H
#define FALCALC_H

#include "prdcalc.h"

/********************************************/
/* This is for Falcon specific calculations */
/********************************************/
typedef enum eFalFuncCode {
   FAL_EVENT_AVG_COEF1=LAST_CALC,
   FAL_EVENT_AVG_COEF2,
   FAL_DLY_COEF1,
   FAL_CONSEC,
   FAL_NORM_TIME,
   FAL_NORM_DAY,
   FAL_AGE_DECAY,
   FAL_RISK_RATE,
   FAL_RISK_Z,
   FAL_CODE_CONV,
   FAL_DIST_HOME,
   FAL_GEO_RISK,
   FAL_DIST_PREV,
} tFalFuncCode;

long ValidateFalconFunction
(tModelVar *modelVar, short funcCode, char *modelName, char *errMsg);

void FalEventAvgCoef1 (tEngVar *calcVar, tModelVar *modelVar);
void FalEventAvgCoef2 (tEngVar *calcVar, tModelVar *modelVar);
void FalDlyCoef1 (tEngVar *calcVar, tModelVar *modelVar);
void FalConsec (tEngVar *calcVar, tModelVar *modelVar);
void FalNormTime (tEngVar *calcVar, tModelVar *modelVar);
void FalNormDay (tEngVar *calcVar, tModelVar *modelVar);
void FalAgeDecay(tEngVar *calcVar, tModelVar *modelVar);
void FalRiskRate(tEngVar *calcVar, tModelVar *modelVar);
void FalRiskZ(tEngVar *calcVar, tModelVar *modelVar);
void FalCodeConv(tEngVar *calcVar, tModelVar *modelVar);
void FalDistHome(tEngVar *calcVar, tModelVar *modelVar);
void FalDistPrev(tEngVar *calcVar, tModelVar *modelVar);
void FalGeoRisk(tEngVar *calcVar, tModelVar *modelVar);

#endif
