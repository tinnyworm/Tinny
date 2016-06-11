#ifndef FREECT_H
#define FREECT_H

/*
** $Id: freect.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: freect.h,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.3  1997/01/16  22:44:06  can
 * Lots of changes for 2.1 release
 *
 * Revision 2.1  1996/04/22  17:56:13  can
 * Several fixes and changes for 2.1
 *
 * Revision 1.3  1995/08/10  00:02:15  can
 * Added integer flag to FreeCalcTab, which indicates that the
 * eval atom should be free'd or not.
 *
 * Revision 1.2  1995/07/09  23:07:20  can
 * Fixed some bugs
 *
 *
*/

#include "calctab.h"

long FreeCalcTables(tCalcTable *calcTab);
long FreeAllDataList(tDataPtr *allData);
long FreeType(tType *type);
long FreeInternalGlobals(tRuleBaseData *ruleBaseData);
long FreeArrayData(tArrayData* arrayData, int freeBuffer);
long FreeRecordData(tRecData* recData, int freeBuffer);

#endif
