#ifndef _RULPROC_H
#define _RULPROC_H
/*
** $Id: rulproc.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
**
*/

#include "rulemgt.h"


#define RB_FILE       "RULE_BASE_FILE"
#define RBFILELEN     256
#define TMPLT_FILE    "TEMPLATE_FILE"
#define TMPLTFILELEN  256


long ResolveData(char *data, char *tmpltName, tRuleManage *ruleManage);
long RulesInitialize(tRuleManage **ruleManage, char *configFile);
long ProcessData(tRuleManage *ruleManage);
long FreeRules(tRuleManage *ruleManage);

#endif
