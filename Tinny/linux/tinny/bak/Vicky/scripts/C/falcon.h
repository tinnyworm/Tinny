/*----------------------------------------------------------------------------*\
$Source: /work/price/falcon/dvl/ccpp/drv_falcon30/RCS/falcon.h,v $
$Log: falcon.h,v $
Revision 1.2  1999/08/26 00:23:38  xxs
 change profile from (REAL*) to (char*)

Revision 1.1  1998/06/08 22:55:28  jcb
Initial revision

 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 *
\*----------------------------------------------------------------------------*/
#ifndef _FALCON
#define _FALCON

#include "hnctype.h"
#include "scorstr.h"

void CSCORE
(BYTE* tran, char* profile, SCORESTRUCT* score, void* boot, int explain, INT32 *errCode);
// XXS: change profile from (REAL*) to (char*)
void INITPROF(char* profile, BYTE* tran, void* boot, INT32 *errCode);
void INITNET(char* cfgFile, void** boot, INT32 *errCode);
void FREENET(void* boot, INT32 *errCode);
void setModelVar(void* handle, char *varName, char *value, INT32 *errCode);

#endif
