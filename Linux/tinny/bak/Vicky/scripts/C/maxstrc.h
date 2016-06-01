#ifndef _MAXSTRC_H
#define _MAXSTRC_H

/*
** $Id: maxstrc.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
**
*/


#include "rulemgt.h"

#define TRAN_TYPE_FIELD_NAME    "TRANSACTION_TYPE"

#define TRAN_TEMPLATE_NAME      "TRAN"
#define PROFILE_TEMPLATE_NAME   "PROF"
#define SCORE_TEMPLATE_NAME     "SCOR"
#define EXPLAIN_TEMPLATE_NAME   "EXPF"

#define USED_MEMFILE    0x01

typedef struct sMax {
   tRuleManage   *ruleMg;

   char          *tranData;
   long          tranLen;

   char          *profData;
   long          profLen;

   char          *scoreData;
   long          scoreLen;

   char          *explnData;
   long          explnLen;

   long          flags;
   struct sMax   *self;
} tMax;

#endif
