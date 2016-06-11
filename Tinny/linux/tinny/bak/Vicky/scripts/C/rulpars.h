#ifndef HNC_RULE_PARSE_H
#define HNC_RULE_PARSE_H

/*
** $Id: rulpars.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: rulpars.h,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.2  1997/01/17  17:22:36  can
 * Updates for latest release
 *
**
*/


#include <stdio.h>

#include "engine.h"
#include "hncdef.h"
 
#define WORD_FLOAT   1
#define WORD_STRING  2
#define WORD_DIGIT   3
#define WORD_END     4
#define WORD_DELIM   5
#define WORD_HEX     6


#define MAX_RULE_LEN 20480


char CheckPermission(char* permission);
int  CheckFldType(char* type, void **data, int *dataSize);
int  GetToken(char *buf, int *index, char *token);
long ParseRuleName(char *buf, int *index, tRuleData *ruleData, char *from);
int  GetWord(char *buf, int *index, char *token, int *tokenType);

long GetTemplateFields
(FILE *fp, tDataTemplate *pTemplateList, int nbrFields, char *from, int decrypt);
long ReadRuleBase
(FILE *fp, tRuleBaseData *ruleBaseData, int nbrRule, char *from, int decrypt);

char *GetRcrd(char *s, int n, FILE *stream, int decrypt);


typedef struct sTmpltNode {
   char          name[hncTEMPLATENAMELEN+1];
   tDataTemplate *tmplt;
   short         optionCt;
   struct sTmpltNode *next;
} tTmpltNode;

typedef struct sTmpltMap {
   tTmpltNode *nodeList;
   struct sTmpltMap *next;
} tTmpltMap;

long ReadTemplateMaps(tTmpltMap **tmpltMap, FILE *fp);
long CombineTemplates(tTmpltMap *tmpltMap, tDataTemplate *tmpltList);

#endif
