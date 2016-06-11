#ifndef EVLEXPR_H
#define EVLEXPR_H

/*
** $Id: evlexpr.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: evlexpr.h,v $
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
#include "bltins.h"
#include "typechk.h"
#include "engerr.h"
#include "except.h"
#include "runct.h"


/* Expression calc functions */
void EvalQuickFloatExpr(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalMixedFloatExpr(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalQuickLongExpr(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalMixedLongExpr(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalQuickNumericExpr(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalMixedNumericExpr(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalStringNumExpr(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalStringExpr(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalMixedExpr(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);


/* Calc rule variable calculation functions */
void EvalSgl_NumVarCalc(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalSgl_StrVarCalc(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalSgl_OthVarCalc(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalSgl_LngVarCalc(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalSgl_FltVarCalc(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);

#endif
