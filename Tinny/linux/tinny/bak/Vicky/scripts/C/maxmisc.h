#ifndef _MAXMISC_H
#define _MAXMISC_H

/*
** $Id: maxmisc.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
**
*/

#include "engine.h"
#include "maxstrc.h"



char *rtrim(char *In);
char *ltrim(char *In);
char *rltrim(char *In);
long GetTemplate(int dbHanlde, char *templateName, tDataTemplate **template);

#ifdef ITM
void RegisterPMaxPointers(tMax *max, long *errCode);
void ResetFunctionPointers(void *handle, long *errCode);
#endif


#endif
