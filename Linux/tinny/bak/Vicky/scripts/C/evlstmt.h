#ifndef EVLSTMT_H
#define EVLSTMT_H

/*
** $Id: evlstmt.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: evlstmt.h,v $
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

/* Statement calc functions */ 
void EvalNumIncrement(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalNumIPADD(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalNumIPSUB(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalNumIPMUL(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalNumIPDIV(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalFloatIncrement(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalFloatIPADD(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalFloatIPSUB(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalFloatIPMUL(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalFloatIPDIV(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalLongIncrement(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalLongIPADD(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalLongIPSUB(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalLongIPMUL(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalLongIPDIV(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);

void EvalReturn(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalTerminate(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalIf(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalVarIf(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalStringCase(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalNumericCase(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalFor(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void EvalWhile(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);

/* Loop through a list of statements */
void CalcStatements(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);
void CalcVarStatements(tAtom *eval, tArg *arg, tREStatus *status, tCalcTable *calcTab);

#endif
