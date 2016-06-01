#ifndef ENGDATA_H
#define ENGDATA_H

/*
** $Id: engdata.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: engdata.h,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.4  1997/02/05  03:31:22  can
 * Added a new TYPE_SHARED flags that indicates that a fieldList
 * is shared among record types.
 *
 * Revision 2.3  1997/01/16  22:44:06  can
 * Lots of changes for 2.1 release
 *
 * Revision 2.1  1996/04/22  17:56:07  can
 * Several fixes and changes for 2.1
 *
 *
*/

#include "datcalc.h"

/***********************************************************/
/* Define new types, other than those defined in hncrtyp.h */
/***********************************************************/
#define NUMERIC_TYPE  100 
#define RECORD_TYPE   101 
#define ARRAY_TYPE    102 

/************************************************/
/* Structure to hold information about an array */
/************************************************/
typedef struct sArray {
   char           *name;
   long           startIndex;
   long           endIndex;
   long           type;
   long           numElems;
   long           elemSize;
   long           dataSize;
   union uTypeFmt *typeFmt; 
} tEngArray;

/*******************************************/
/* Structure to hold the data for an array */
/*******************************************/
typedef struct sArrayData {
   tEngArray  *array;      
   long       endIndex;
   long       nonFixed;
   char       **data;
} tArrayData;


/************************************************/
/* Structure to hold information about a record */
/************************************************/
typedef struct sRecField {
   char      *fieldName;
   long      type;
   long      offset;
   long      length;

   struct sNonFixed *nonFixed;
   union uTypeFmt   *typeFmt;
   
   struct sRecField *next;
} tRecField;

typedef struct sRecord {
   char       *recName;
   long       dataSize;
   long       numFlds;
   tRecField  *fieldList;
   void       *user;
} tEngRecord;


/*******************************************/
/* Structure to hold the data for a record */
/*******************************************/
typedef struct sFlatData {
   char    *data;
   long    length;
} tFlatData;

#define FIELD_NOT_HIT     0x1
#define FIELD_DUMMY       0x2
typedef struct sFldData {
   tRecField  *fld;
   short      flags;
   short      origData;
   char       *data;
} tFldData;

typedef struct sRecData {
   tEngRecord *record;
   char       **data;
   tFldData   *flds; 
} tRecData;

#define NON_FIXED   0x01
typedef struct sRecInstance {
   tRecData   *data;
   long       flags;
  
   struct sRecInstance *next;
} tRecInstance;


/********************************************************************/
/* Structure to hold information regarding lists of available types */
/********************************************************************/
typedef union uTypeFmt {
   tEngRecord record;
   tEngArray  array;
} tTypeFmt;

#define TYPE_GLOBINT  0x01
#define TYPE_TEMPLATE 0x02
#define TYPE_SUBTMPLT 0x04
#define TYPE_SHARED   0x08
typedef struct sType {
   char     *typeName; 
   int      flags;
   int      type;
   tTypeFmt fmt;

   struct sType *next;
} tType;


/*********************************************/
/* Structure for a generic data storage atom */
/* for passing data to functions and storing */
/* data as it is calculated in the engine    */
/*********************************************/
typedef struct sAtom {
   int type;    
   union {
      tEngNum  num;
      char     cVal;
      float    fVal;
      long     lVal;
      struct {
         char  *str;
         int   len;
      } strVal;
      void     *uVal;
   } data;
} tAtom;

/*****************************************************/
/* Structure to hold information regarding variables */
/*****************************************************/
typedef struct sCalcRule {
   char  *code;   
   void  *data;
   short converted;
   short noAutoCalc;
   char  *calcDone;
} tCalcRule;

#define VAR_REFERENCE   0x01
#define VAR_PARAMETER   0x02
#define VAR_CONSTANT    0x04
#define VAR_LOCAL       0x08
#define VAR_NOFREE      0x10
#define VAR_GLOBINT     0x20
#define VAR_TEMPLATE    0x40
#define VAR_EXTDATA     0x80
typedef struct sEngVar {
   char      *varName;
   tCalcRule *calcRule;
   long      flags;
   int       type;
   void      *data;
   int       dataSize;
   void      *user;

   struct sEngVar *next;
} tEngVar;


#endif
