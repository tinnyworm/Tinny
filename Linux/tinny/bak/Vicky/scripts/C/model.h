#ifndef HNC_MODEL_H
#define HNC_MODEL_H

/*
** $Id: model.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: model.h,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.5  1997/02/24  20:31:04  can
 * Added definition for PATH_MAX if not in limits.h
 *
 * Revision 2.4  1997/02/12  00:15:45  can
 * Made sure the SAFE_EXP macro was properly parenthesized.
 *
 * Revision 2.3  1997/02/07  19:51:01  can
 * Added SAFE_EXP macro, plus put in TANDEM define
 * for path name length
 *
 * Revision 2.2  1997/01/17  17:22:36  can
 * Updates for latest release
 *
**
*/

/* System inlcudes */
#include <limits.h>
#include <math.h>

/* HNC Rule Engine includes */
#include "engine.h"

/* HNC common includes */
#include "ismdata.h"
#include "dyntabl.h"
#include "rulfunc.h"

/* Neural Network includes */
#include "optinet.h"


/* Path and name limits */ 
#ifdef TANDEM
#include "$system.zsysdefs.zsysc(filename_constant)"
#define FILENAME_LEN   ZSYS_VAL_LEN_FILENAME
#define MODELNAMELEN   ZSYS_VAL_LEN_FILENAME
#else
#ifndef PATH_MAX
#define PATH_MAX 256
#endif

#define FILENAME_LEN   PATH_MAX
#define MODELNAMELEN   PATH_MAX
#endif


/* Macro to cap an exp() call */
#define LO_CAP    -77
#define HI_CAP     78
#define HI_VALUE   2.76E33
#define LO_VALUE   0
#define SAFE_EXP(x) ((x) <= LO_CAP ? LO_VALUE : (x) > HI_CAP ? HI_VALUE : exp(x))


/************************************************************/
/* This structure holds a list of site specific values that */
/* are to replace constants defined in the model .tab file  */
/************************************************************/
#define MAX_SITEVAL_LEN   32
typedef struct sSiteValues {
   char  name[MAX_SITEVAL_LEN];
   float data;

   struct sSiteValues *next;
} tSiteValue;

/**************************************************************/
/* This structure holds all the "master" data that the models */
/* need to run with.                                          */
/**************************************************************/
typedef struct sMasterData {
   tDataTemplate *templateList;
   tFunc         *funcList;
   int           DbHandle;
   char          *DbUserId;
   char          *DbPassword;
   tISAMData     *ISAMList;
   tFuncInit     *funcInit;
   tSiteValue    *siteValues;
} tMasterData;


/********************************************************/
/* This structure holds data to allow a map between the */
/* variables and the profile templates.  In addition,   */
/* the profile initial values are stored here           */
/********************************************************/
typedef struct sProfileMap {
   char     *prof;
   char     *field;
   char     *varName;
   tEngVar  *defValue;
   int      dataType;
   char     *data;

   struct sProfileMap *next;
} tProfileMap;


/*****************************************************/
/* This structure allows storing a list of variables */
/*****************************************************/
typedef struct sCalcVar {
   tEngVar   *var;

   struct sCalcVar *next;
} tCalcVar;


/**************************************************************/
/* These structures hold data for determining the explanation */
/* bin that a variable belongs to                             */
/**************************************************************/
typedef struct sBin {
  float  expect;
  float  range;
} tBin;

typedef struct sVarBins {
  int   numBins;
  tBin  *bin;
} tVarBins;


/******************************************************/
/* This structure holds store model specific data for */
/* variables.  It will be put into the user field of  */
/* the tEngVar structure.                             */
/******************************************************/
#define NOT_SHARED  0
#define ORIGINAL    1
#define COPY        2
typedef struct sModelVar {
  char        *cName;
  char        *sasName;
  char        *cfgName;
  char        *calcDone;
  void        (*func)(tEngVar *this, struct sModelVar *modelVar);
  short       funcCode; 
  short       isShared; 
  tCalcVar    *args;
  short       explainVal;
  short       explainGrp;
  tVarBins    expBins;
} tModelVar;



/*******************************************************/
/* This structure holds a list of data atoms, parallel */
/* to the .cfg file.  These data atoms will be set by  */
/* the calculation rules, and then the list is         */
/* traversed to set the inputs.                        */
/*******************************************************/
typedef struct sNetInput {
   char      *name;
   int       type;
   int       dataSize;
   void      *data;
   tModelVar *modelVar;
   
   struct sNetInput *next;
} tNetInput;



/*****************************************************/
/* This structure allows storing a list of variables */
/*****************************************************/
typedef struct sVarBlock {
   char      *name;
   tCalcVar  *calcList;
   tCalcVar  *noCalcList;

   struct sVarBlock *next;
} tVarBlock;


/***********************************************************/
/* These help maintain the section layout of the .tab file */
/***********************************************************/
#define CON_SECTION   1
#define TYP_SECTION   2
#define VAR_SECTION   3
#define XPL_SECTION   4
#define RUL_SECTION   5
#define PRM_SECTION   6
#define PIR_SECTION   7
#define TAB_SECTION   8
#define EXT_SECTION   9 
typedef struct sSectItem {
   void  *data;
   struct sSectItem  *next;   
} tSectItem;

#define COMMENTS_LEN   1024
typedef struct sTabSection {
   char      comments[COMMENTS_LEN];
   char      fileName[FILENAME_LEN];
   int       sectionType;
   tSectItem *itemList;

   struct sTabSection *next;
} tTabSection; 


/***********************************************************/
/* This structure stores all data that is used to deploy a */
/* single model.                                           */
/***********************************************************/
typedef struct sModel {
   char           modelPath[MODELPATHLEN+1];
   char           modelName[MODELNAMELEN+1];
   short          PRMReduce;
   short          byPassNet;
   tOptiNet       *pNet;
   char           *baseRules;
   char           *extFiles;
   tNetInput      *inputList;
   tISAMData      *ISAMList;
   tTableTemplate *tableList;
   tProfileMap    *PRMList;
   tEngVar        *fieldVars; 
   tEngVar        *constantList; 
   tVarBlock      *mainBlockList;
   tVarBlock      *subBlockList;
   tRuleBaseData  *modelRBData;
   void           *modelRuleHandle;
   tRuleBaseData  *modelPInitData;
   void           *modelPInitHandle;
   tTabSection    *sectionList;

   struct sModel *next;
} tModel;


long  LoadModel(tModel *model, tEngVar **sharedVars, tMasterData *masterData);
long  FreeModel(tModel *model, tMasterData *masterData);
long  InitModelProfiles(tModel *model);

long  GetModelData(tModel *model);
long  SaveModelData(tModel *model);
long  GetModelData(tModel *model);

long  CheckTemplateExists(tDataTemplate *templateList, char *templateName);
void  GetTemplateFromDB
(tDataTemplate **templateList, char *templateName, tMasterData *masterData);

int   VarMatch(tEngVar *var, char *varName);

/* Prototypes of functions to build calc var lists */
long LoadFuncCalcVarList(tModel *model);
long LoadRuleCalcVarList(tModel *model);


#endif
