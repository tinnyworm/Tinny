#ifndef HNC_MG_MODEL_H
#define HNC_MG_MODEL_H

/*
** $Id: mgmodel.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: mgmodel.h,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.4  1997/01/21  19:05:34  can
 * Changed _InitProfiles and _ResetModels back to previous names, without '_'.
 * Also, changed the rule wrapper function InitProfiles to InitProfilesFunc,
 * and did similarly with the ResetModels wrapper.
 *
 * Revision 2.3  1997/01/21  00:36:53  gak
 * Created rule wrapper functions for ResetModels and InitProfiles
 *
** Revision 2.2  1997/01/17 17:22:36  can
** Updates for latest release
**
**
*/


#include "engine.h"
#include "model.h"
#include "rulfunc.h"


/**************************************************************/
/* This structure stores all data revelant to a set of models */
/* Essentially, a model set is a list of models that are      */
/* loaded together, may share some variables, and may all be  */
/* executed for a single transaction.                         */
/**************************************************************/
typedef struct sModelSet {
   tModel  *modelList;
   tEngVar *sharedVarList;

   struct sModelSet *next;
} tModelSet;

/**********************************************************/
/* This structure is used to track all of the models that */
/* are loaded into memory.  Also, the modelSupport        */
/* structure holds data that is relevant for all models   */
/**********************************************************/
typedef struct sModelManage {
   int           numLoaded;
   int           maxSets;
   int           numCalcDones;
   char          *calcDones;
   tMasterData   *masterData;
   tModelSet     *setList;
   tModel        *currModel;
} tModelManage;


#define MAXMODSETS     "MAX_SETS_LOADED"
#define MAXSETSLEN     10 
#define MODELSET       "MODEL_SET"
#define MODELSETLEN    4096 


/* Class level error codes */
#define FIREMODEL         0x9700
#define GETMODERR         0x9800
#define SETMODVAL         0x9900

/* Low level error codes */
#define MODEL_UNDEFINED   0x00F0
#define NO_MODELS         0x00F1



tAtom FireModel(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom InitProfilesFunc(tFuncCallData* funcCallData, tAtom *aParams, int numArgs);
tAtom ResetModelsFunc(tFuncCallData* funcCallData, tAtom *aParams, int numArgs);
tAtom GetModelErrorString(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);

long  InitModels(char *configFile, tMasterData *masterData, void **modelHandle);
long  InitProfiles(void *modelHandle);
long  ResetModels(void *modelHandle);
long  FreeModels(void *modelHandle);

long  SetTableList(void *modelHandle, void *dynTableList);


#endif
