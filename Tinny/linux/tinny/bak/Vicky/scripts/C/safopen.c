/*
** $Id: safopen.c,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
**
*/

#include <stdio.h>
#include <stdlib.h>

#ifndef MFRAME
#include <dirent.h>
#endif


#include "safopen.h"



/***********************************************************************/
/* SafeFopen
**    This simply does an fopen for reading, unless the MFRAME 
**    compile open is used.  In that case, the CVR_DDNAME API is
**    called to convert the file name for the mainframe
************************************************************************/
FILE* SafeFopen(char *fileName)
{
#ifdef MFRAME
   char  openName[256];

   CVR_DDNAME(fileName, openName);
   return(fopen(openName, "r"));
#else
   DIR *dp;

   if((dp = opendir(fileName))!=NULL)
      return(NULL);
   else
      return(fopen(fileName, "r"));
#endif
}
