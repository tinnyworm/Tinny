#ifndef HNC_DYN_TABLE_H
#define HNC_DYN_TABLE_H

/*
** $Id: dyntabl.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: dyntabl.h,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.3  1997/02/05  04:16:30  can
 * Optimize the storage of the data records, including assuming that the
 * key is always the first field in the record.  Also, allow short integers
 * as keys
 *
 * Revision 2.2  1997/01/17  17:22:36  can
 * Added default data to tables and added TableGetDefaults.  Also,
 * added TableMultiLookup for multi dimension tables.
 *
**
*/


#include "rulfunc.h"
#include "engine.h"


typedef struct sTableTemplate {
   char         *tableName;
   int          dataLength;
   int          fieldCount;
   int          keyType;
   int          keyLength;
   int          numRows;
   char         *typeName;
   tRecField    *fieldList;
   char         **dataList;
   char         *defaultData;

   struct sTableTemplate *next;
} tTableTemplate;



long LoadTables
(tTableTemplate **tblList, char *buf, FILE *fp, long oldSpot, char *from, tType **globTypes);
long FreeTable(tTableTemplate *table);
long AddTableToEngineTypes(tTableTemplate *table, tType **typeList);
long CreateTableFromType(tTableTemplate *table, tEngRecord *record, tTableTemplate *tblList);

void TableMultiLookupCreate(tFuncInit *funcInit, tFunc **funcList);
void TableLookupCreate(tFuncInit *funcInit, tFunc **funcList);
void TableRangeLookupCreate(tFuncInit *funcInit, tFunc **funcList);
void TableDirectIndexCreate(tFuncInit *funcInit, tFunc **funcList);
void TableGetDefaultsCreate(tFuncInit *funcInit, tFunc **funcList);

tAtom TableMultiLookup(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom TableLookup(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom TableRangeLookup(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom TableDirectIndex(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);
tAtom TableGetDefaults(tFuncCallData *funcCallData, tAtom *aParams, int numArgs);

long  DoTableDirect(tTableTemplate *table, long key, tRecData *record);

long  DoLongTableLookup(tTableTemplate *table, long key, tRecData *record);
long  DoShortTableLookup(tTableTemplate *table, short key, tRecData *record);
long  DoFloatTableLookup(tTableTemplate *table, float key, tRecData *record);
long  DoStringTableLookup(tTableTemplate *table, char *key, int keyLen, tRecData *record);

long  DoLongTableRange(tTableTemplate *table, long key, tRecData *record);
long  DoShortTableRange(tTableTemplate *table, short key, tRecData *record);
long  DoFloatTableRange(tTableTemplate *table, float key, tRecData *record);
long  DoStringTableRange(tTableTemplate *table, char *key, int keyLen, tRecData *record);

/* Error codes */
#define TABLE_LOOKUP   0x3000

#define NO_TABLES        0x0001
#define TABLE_NO_EXIST   0x0002
#define RECORD_NOT_FOUND 0x0003
#define BAD_PARAMS       0x0004

/* Flag set in tType flags field to indicate that a type */
/* is specific to a single table.                        */
#define TYPE_TABLE       0x80

#endif
