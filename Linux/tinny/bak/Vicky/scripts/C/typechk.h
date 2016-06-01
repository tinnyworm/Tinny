#ifndef TYPECHK_H
#define TYPECHK_H

/*
** $Id: typechk.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: typechk.h,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.3  1997/01/20  23:40:30  can
 * Added NoCaseMemcmp
 * ,.
 *
 * Revision 2.2  1997/01/16  22:44:06  can
 * Lots of changes for 2.1 release
 *
 * Revision 2.1  1996/04/22  17:56:17  can
 * Several fixes and changes for 2.1
 *
 * Revision 1.4  1995/07/26  20:53:26  can
 * Added IsBinNumeric and fixed a bug in type checking strings and numerics
 *
 * Revision 1.3  1995/07/21  16:20:39  can
 * Added function prototypes
 *
 * Revision 1.2  1995/07/09  23:07:20  can
 * Fixed some bugs
 *
 * Revision 1.1  1995/07/03  21:18:26  wle
 * Initial revision
 *
*/

#include "engine.h"
#include "parstrc.h"
#include "exptree.h"


#define IsNumString(type) \
(type == HNC_FLOATSTR || type == HNC_INTSTR)

#define IsIntNumeric(type) \
(type == HNC_INT16 || type == HNC_UINT16 || type == HNC_INT32 || \
 type == HNC_INTSTR || type == NUMERIC_TYPE || type == HNC_UINT32)

#define IsBinNumeric(type) \
(type == HNC_FLOAT || type == HNC_DOUBLE || type == HNC_INT16 || \
 type == HNC_UINT16 || type == HNC_INT32 || type == NUMERIC_TYPE || \
 type == HNC_UINT32)

#define IsNumeric(type) \
(type == NUMERIC_TYPE || type == HNC_FLOAT || type == HNC_DOUBLE || \
 type == HNC_INT16 || type == HNC_UINT16 || type == HNC_INT32 || \
 type == HNC_UINT32 || type == HNC_INTSTR || type == HNC_FLOATSTR)

#define IsString(type) \
(type == HNC_STRING || type == HNC_CHAR || type == HNC_DATESTR || \
 type == HNC_TIMESTR || type == HNC_DTSTR || type == HNC_STRING || \
 type == HNC_INTSTR || type == HNC_FLOATSTR)

#define IsDate(type) \
(type == HNC_DATESTR || type == HNC_DTSTR)

#define IsTime(type) \
(type == HNC_TIMESTR || type == HNC_DTSTR)


long NoCaseStrcmp(char* str1, char* str2);
long NoCaseMemcmp(char* str1, char* str2, int len);

long AddLoopVariable(char* varName);
long RemoveLoopVariable(char* varName);
long IsLoopVariable(char* varName);

int  IsValidDateStr(char* string);
int  IsValidTimeStr(char* string);
int  IsValidDateTimeStr(char* string);

long CompareType(int expectedType, int usedType);

tType* FindType(char *typeName, tParse *parse);

long CompareRuleArgType
(tExprTree *exprTree, tRuleArg* ruleArg, char* ruleName, int argNum, tParse *parse);
long CompareFuncParamType
(tExprTree *exprTree, tParamList* funcParam, char* funcName, int argNum, tParse *parse);

long CheckRule(char* ruleName, tParse *parse, tRuleData **ruleData);
long CheckFunction(char* function, tParse *parse, tFunc **funcStruct);
long CheckVariable(char* varName, tParse *parse);
long CheckRecordField(char* field, tParse *parse);

void SetOpType(tOp *op, char *opStr);
long TypeCheckAndRollUpExpression(tExprTree *exprTree, tParse *parse, int *treeType);

long AddVarToAllData(tDataPtr *dataList, tEngVar *var);
long AddVarToList(tEngVar **varList, tEngVar *var);

long AddTypeToAllData(tDataPtr *dataList, tType *type);
long AddTypeToList(tType **typeList, tType *type);

long ResetTreeType(tExprTree *exprTree, int type, tParse *parse);


#endif
