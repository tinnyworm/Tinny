#ifndef OPTINET_H
#define OPTINET_H

/*
** $Id: optinet.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: optinet.h,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 1.3  1997/02/07  19:40:47  can
 * Added define for MODELPATHLEN
 *
 * Revision 1.2  1997/01/18  03:28:39  can
 * Added missing and div/range values to cfgNode
 *
 * Revision 1.1  1997/01/17  19:17:40  can
 * Initial revision
 *
 *
**
*/

#include <limits.h>

/* Model name length limits */
#ifdef TANDEM
#include "$system.zsysdefs.zsysc(filename_constant)"
#define MODELPATHLEN   ZSYS_VAL_LEN_FILENAME
#else
#define MODELPATHLEN   PATH_MAX
#endif


/* Variable types */
#define VAR_BINARY        1 
#define VAR_CONTINUOUS    2 
#define VAR_GROUP         3 
#define VAR_EXCLUDE       4 
#define VAR_ID            5 

/* Slab types */
#define SLAB_NONE         0 
#define SLAB_INPUT        1 
#define SLAB_TARGET       2 

/* Scale types */
#define SCALE_NONE        0
#define SCALE_LOG         1
#define SCALE_LIN         2
#define SCALE_Z           3

typedef struct sCfgNode {
  char    *name;
  short   varType;
  short   slabType;
  int     recCnt;
  float   minVal;
  float   maxVal;
  float   mean;
  float   stddev;
  float   missingVal;
  short   length;
  short   scaleType;
  int     numSymbols;
  char    **aSymbols;
  float   divisor;
  float   range;
} tCfgNode;


#define SYMBOLIC  1
#define NUMERIC   2
typedef struct sNodeData {
   union {
      char  *str; 
      float num;
   } data;
   short type;
} tNodeData;


typedef struct sHidden {
   float  initWeight;
   float  *aWeights;
   float  outputVal;
   float  outputWeight;
} tHidden;


typedef struct sOptiNet {
   short     numCfgNodes;
   short     squashOutput;
   tCfgNode  *aCfgNodes;
   tNodeData *aInputData;

   int       numScaled;
   float     *aScaled;

   int       numHidden;
   tHidden   *aHidden;

   float     outputWeight;
   float     output;
} tOptiNet;


long InitOptiNet(char *baseName, tOptiNet **net);
long IterOptiNet(tOptiNet *net);
long FreeOptiNet(tOptiNet *net);


#endif
