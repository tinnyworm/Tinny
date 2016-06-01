#ifndef ENGINE_H
#define ENGINE_H

/*
** $Id: engine.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: engine.h,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.3  1997/01/16  22:44:06  can
 * Lots of changes for 2.1 release
 *
 * Revision 2.2  1996/05/01  01:18:43  can
 * Added inputData to tFuncCallData
 *
 * Revision 1.10  1995/08/30  16:09:56  can
 * Added calc table to the tFuncCallData structure.
 *
 * Revision 1.9  1995/08/25  01:25:48  can
 * Fixed compile warning messages
 *
 * Revision 1.8  1995/08/02  00:07:32  can
 * Added support rules for rule bases
 *
 * Revision 1.7  1995/07/28  00:12:42  can
 * Took out ParseRuleBase function.  Added ruleBaseName to ruleBaseData
 * structure.  Reduced CreateRuleBase to two parameters.
 *
 * Revision 1.6  1995/07/25  16:00:09  can
 * Added ruleBaseData to tFuncCallData structure
 *
 * Revision 1.5  1995/07/21  16:20:39  can
 * Modified atom structure to have specific types.
 * Modified tFunc structure by adding tFuncCallData structure
 *
 * Revision 1.4  1995/07/13  22:49:39  can
 * Added numArgs to function pointer, and added tREStatus structure
 * for passing to function pointer.
 * Also, added inputData field to function structure, which gets
 * passed to SysInitFuncPtr.
 * Also, added MAX_ARGS for maximum number of args allowed to for
 * a function
 *
 * Revision 1.3  1995/07/09  23:25:43  can
 * Added exception handling prototypes
 *
 * Revision 1.2  1995/07/09  23:15:16  wle
 * Added datasize to template structure.
 *
 * Revision 1.1  1995/07/03  21:18:26  wle
 * Initial revision
 *
*/

/* System include files */
#include <stdio.h>

/* HNC include files */
#include "engdata.h"


/*************************/
/* For permission fields */
/*************************/
#define READ_ONLY   'R'
#define READ_WRITE  'W'

/***********************************************/
/* Structure for each data field in a template */
/***********************************************/
/* To hold data specifically for non-fixed templates */
typedef struct sNonFixed {
   char        *fldID;
   char        IDLen;
   char        lengthInd;
   char        required; 
   char        skip; 
   long        maxElements; 
} tNonFixed;

typedef struct sDataField {
   char         *fieldName;
   short        type;
   char         permission;
   long         offset;
   long         length;
   tNonFixed    nonFixed;
   char         *subTmpltName;

   struct sDataField *next;
} tDataField;

/**********************************************/
/* Structure for data templates.  Used to map */
/* out a data record generically              */
/**********************************************/
typedef struct sDataTemplate {
   char         *templateName;
   char         permission;
   char         *data;
   long         dataSize;
   tDataField   *fieldList;
   tRecInstance *record;     /* Leave NULL--engine will set and free */

   struct sDataTemplate *next;
} tDataTemplate;



/************************************************/
/* Structure to hold an rule parsing error code */
/************************************************/
typedef struct sRuleErr {
   long   errCode;
   char   *errToken;
   short  lineNum;
   short  errData;

   struct sRuleErr *next;
} tRuleErr;

/*************************************************/
/* Struct for holding arguments passed to a rule */
/*************************************************/
typedef struct sRuleArg {
   char *argName;
   short reference;
   short type;

   struct sRuleArg *next;
} tRuleArg;

/***********************************************/
/* Structure to hold rule information, that is */
/* the rule name, the rule text, and the parse */
/* error code.                                 */
/***********************************************/
typedef struct sRuleData {
   int           ruleID;
   char          *ruleName;  
   char          *ruleText;
   tRuleErr      *errList;
   long          ruleWeight;
   int           returnType;
   int           numArgs;
   tRuleArg      *argList;

   struct sRuleData *next;
} tRuleData;


/*******************************************/
/* Structures to lists of parameter types  */
/* allowed for a given function            */
/*******************************************/
typedef struct sTypeList {
   int type;
   struct sTypeList *next;
} tTypeList;

typedef struct sParamList {
   int reference;
   struct sTypeList  *list;
   struct sParamList *next;
} tParamList;

/**********************************************/
/* Structure to hold rule engine error status */
/**********************************************/
#define MAX_ERR_STR  512
typedef struct sREStatus {
   long  errCode;
   char  *errStr;
} tREStatus;

/*************************************************************/
/* The maximum number of arguments that a function will take */
/*************************************************************/
#define MAX_ARGS  100

/***************************************/
/* Structure to hold trace information */
/***************************************/
typedef struct sStmtText {
   char *text;
   struct sStmtText *next;
} tStmtText;

typedef struct sTrace {
   short     stmtType;
   short     level;
   tStmtText *textList;
} tTrace;

/****************************************************/
/* Structure used to pass information to a function */
/****************************************************/
typedef struct sFuncCallData {
   tREStatus     errStatus;
   char          *funcData; 
   void          *inputData; 
   tRuleData     *ruleData; 
   void          *calcTable;
   tTrace        *trace;
   struct sRuleBaseData *ruleBaseData; 
} tFuncCallData;

/******************************************/
/* Structure to hold function pointer and */
/* all relative data to that function     */
/******************************************/
typedef struct sFunc {
   char        *functionName;
   int         unlimitedParams;
   int         lastOptional;
   int         numParams;
   tParamList  *paramTypeList;
   int         returnType;
   tAtom       (*FuncPtr)(tFuncCallData* funcCallData, tAtom *aParams, int numArgs);
   void        (*SysInitFuncPtr)(tFuncCallData *funcCallData, void* inputData);
   void        (*IterInitFuncPtr)(tFuncCallData *funcCallData);
   void        (*SysTermFuncPtr)(tFuncCallData *funcCallData);
   void        (*IterTermFuncPtr)(tFuncCallData *funcCallData);
   void        (*ParamCheck)(int parameter);
   char        *funcData;
   void        *inputData;

   struct sFunc *next;
} tFunc;


/******************************************************************/
/* Structure to hold lists of special rules that are automtically */
/* run at the start end finish of the system, and each iteration  */
/******************************************************************/
typedef struct sSupportRules {
   tRuleData *sysInitRuleList;
   tRuleData *sysTermRuleList;
   tRuleData *iterInitRuleList;
   tRuleData *iterTermRuleList;
} tSupportRules;

/****************************************************/
/* Structure to hold the necessary data for parsing */
/* the rules and creating the run-time table        */ 
/****************************************************/
typedef struct sRuleBaseData {
   char           *ruleBaseName;
   tRuleData      *ruleList; 
   tDataTemplate  *templateList;
   tFunc          *funcList;
   tEngVar        *globalVarList;
   tType          *globalTypeList;
   tREStatus      status;
   tSupportRules  supportRules;
   FILE           *traceFile;
   int            whileLoopLimit;
  
   struct sRuleBaseData *next;
} tRuleBaseData;


/****************************************************/
/* CreateRuleBase is called to parse the rules and  */
/* allocate a run-time calculation table, which is  */
/* returned via the ruleBaseHandle parameter.       */
/*                                                  */
/* Any exceptions will throw an ENGINE_EXCEPTION    */
/* which can be examined via the supplied rountines */
/****************************************************/
long CreateRuleBase(tRuleBaseData *ruleBaseData,
                    void **ruleBaseHandle); 

/**********************************************************/
/* ExecuteRuleBase takes a ruleBaseHandle from a previous */
/* call to CreateRuleBase, and executes the rules, based  */
/* on the data in the data buffers from the original      */
/* data templates.                                        */
/*                                                        */
/* Any exceptions will throw an ENGINE_EXCEPTION  which   */
/* can be examined via the supplied rountines             */
/**********************************************************/
long ExecuteRuleBase(void *ruleBaseHandle); 

/********************************************************/
/* Free all space allocated by the CreateRuleBase call. */
/* Note: this does not include any data buffers passed  */
/* into CreateRuleBase.                                 */
/*                                                      */
/* Any exceptions will throw an ENGINE_EXCEPTION        */
/* which can be examined via the supplied rountines     */
/********************************************************/
long FreeRuleBase(void* ruleBaseHandle); 



/*********************************************************/
/* Functions to turn on/off automatic clearing of calc   */
/* done flags for shared calculations. The allows shared */
/* calcs to be saved across calls to ExecuteRuleBase     */
/*                                                       */
/* Note: these functions are not normally used, but were */
/* added to support the sharing of variables across      */
/* calls to multiple models for a single transaction     */
/*********************************************************/
long SetNoAutoReset(void *ruleBaseHandle);
long SetAutoReset(void *ruleBaseHandle);
long ResetSharedCalcs(void *ruleBaseHandle);

/******************************************************/
/* These functions are used as a quick rule parse to  */
/* essentially build a list of the variables used in  */
/* a piece of rule text.                              */
/******************************************************/
long GetGlobalVarsInRule(char *varNames, char *ruleText);
long GetRecordsInRule(char *varNames, char *ruleText);
long GetSetVarsInRule(char *varNames, char *ruleText);
long IsFuncCallInRule(char *ruleText);
long GetDependentVarsInRule(char *varName, char *varNames, char *ruleText);

/***************************************************/
/* Function to free the calcRule structure, if any */
/***************************************************/
long FreeCalcRule(tCalcRule *calcRule);

/**************************************************/
/* Function to get the offset of a template field */
/**************************************************/
char* GetFieldData
(char *templateName, char *fieldName, tDataTemplate *templateList);

/*********************************************************/
/* Functions to parse and calculate a variable calc rule */
/*********************************************************/
long ParseVarRule(tEngVar *var, void *RBHandle);
long CalcVarRule(tEngVar *var, void *RBHandle);

/***********************************************************************/
/* Function to resolve incoming data with the registered template data */
/***********************************************************************/
long ResolveTemplateRecord(char **data, tDataTemplate *tmplt, void *handle);
long SetTemplateRecordData(char *data, tDataTemplate *tmplt, void *handle);
long ResetDynamicPool(void *handle);

/*************************************************************/
/* Functions to allow exterior management of calc done flags */
/*************************************************************/
int  GetCountStr2NumCalcs(void *handle);
long MoveStr2NumCalcDones(void *handle, char *buffer, int *index);

/********************************/
/* Exeception handling routines */
/********************************/
char* GetEngineExceptionString(tRuleBaseData *ruleBaseData);
long  GetEngineExceptionNumber(tRuleBaseData *ruleBaseData);
long  GetEngineExceptionType(tRuleBaseData *ruleBaseData);

/*****************************************************/
/* Function to create an instance of a variable type */
/*****************************************************/
long MakeTypeInstance(tEngVar *var, tType *type);


/* Macro to get the engine version number */
#define RULE_ENGINE_VERSION   "HNC Rule Engine, Version 2.2"



#endif
