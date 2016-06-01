#ifndef HNC_MODEL_TAB_H
#define HNC_MODEL_TAB_H

/*
** $Id: modtab.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: modtab.h,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 1.2  1997/01/17  17:22:36  can
 * Updates for latest release
 *
**
*/


#include "model.h"


long ScanFile
(char *fileName, FILE *fp, tModel *model, int *loaded,
  tEngVar **sharedVars, tMasterData *masterData);


#define MODEL_VARS    0x0001
#define MODEL_RULES   0x0002
#define MODEL_TMPLTS  0x0004
#define MODEL_CONSTS  0x0008
#define MODEL_PROFMAP 0x0010
#define MODEL_PINITRB 0x0020
#define MODEL_TABLES  0x0040
#define MODEL_BASERUL 0x0080
#define MODEL_EXT     0x0100
#define MODEL_XPL     0x0200
#define MODEL_TYPES   0x0400



#endif
