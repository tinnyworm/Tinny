#ifndef _PROFMAX_H
#define _PROFMAX_H

/*
** $Id: profmax.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
**
*/


void MAXINIT
(char *configFile, void **handle, long *errCode);

void MAXPINIT
(char *tran, char *profile, void *handle, long *errCode);

void MAXSCORE
(char *tran, char *profile, char *score, char *explain, void *handle, long *errCode);

void MAXFREE
(void *handle, long *errCode);

#ifdef ITM
void ResolvePMaxFunctions(void *handle, long *errCode);
#endif



#endif
