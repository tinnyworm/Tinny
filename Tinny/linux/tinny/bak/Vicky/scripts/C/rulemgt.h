#ifndef _RULEMGT_H
#define _RULEMGT_H

/*
** $Id: rulemgt.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
**
*/


#include "engine.h"
#include "mgmodel.h"
#include "rulpars.h"

#define RULEBASENAMELEN 50
#define RULENAMELEN     50

typedef struct sRuleBase {
   tRuleBaseData *ruleBaseList;
   void          *rootHandle;
} tRuleBase;

typedef struct sRuleManage {
   tRuleBase     *ruleBase;
   tTmpltMap     *templateMaps;
   tMasterData   masterData;
   void          *modelHandle;
   tFuncInit     *funcInit;
} tRuleManage;

#endif
