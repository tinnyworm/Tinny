/*
** $Id: dyntabl.c,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: dyntabl.c,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.7  1997/05/31  00:24:46  can
 * Fixed bug in DoStringTableRange.
 *
 * Revision 2.6  1997/05/13  08:57:10  cde
 * fix for hp compile
 *
 * Revision 2.5  1997/02/05  04:16:30  can
 * Optimize the storage of the data records, including assuming that the
 * key is always the first field in the record.  Also, allow short integers
 * as keys
 *
 * Revision 2.4  1997/01/25  07:17:33  can
 * Fixed bug in work with multi-dimension tables.
 *
 * Revision 2.3  1997/01/17  18:53:23  can
 * Changed hncRulePase to hncRuleParse (duh!)
 *
 * Revision 2.2  1997/01/17  17:22:36  can
 * New functions for latest release.
 *
**
*/

/* System include files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>

/* HNC Rule Engine includes */
#include "mymall.h"
#include "engine.h"
#include "engerr.h"
#include "typechk.h"

/* HNC common includes */
#include "dyntabl.h"
#include "rulpars.h"
#include "rulfunc.h"
#include "config.h"
#include "except.h"
#include "hncrtyp.h"

extern int decrypt;

long LoadTableDefaults(tTableTemplate *table, tTableTemplate *tblList, FILE *fp, char *from);
long LoadTableData(tTableTemplate *table, tTableTemplate *tblList, FILE *fp, char *from);
long LoadDataElement
(char *newData, tTableTemplate *table, tTableTemplate *tblList, 
   int nbrDat, char *buf, FILE *fp, char *from);
long LoadTableFields
(tTableTemplate *table, tTableTemplate *tblList, FILE *fp, 
   int *nbrFldPtr, int *dataLen, char *from, tType *globalTypes);

long StringToHNCType
(char *data, char *inputData, int outputtype, int outputlength, int inputType);
long WriteDataElement
(char *newData, tRecField *field, char *token, int tokenType);


/********************************************************/
/* data must be initialize to null; Output will written */
/* only as many as length;                              */
/********************************************************/
long StringToHNCType
(char *data, char *inputData, int outputtype, int outputlength, int inputType)
{
   unsigned long     tULongInt;
   unsigned short    tUShort;
   short             tShort;
   int               tInt;
   long              tLongInt;
   float             tFloat;
   double            tDouble;
   int               inputDataLen;

   
   inputDataLen = strlen(inputData);

   errno = 0;
   switch(outputtype)
   {
      case HNC_UINT32:
         if(inputType != WORD_DIGIT)
         {  fprintf(stderr, "\nInvalid data for UINT32;len '%d'; inputType '%d';",
                                                           inputDataLen,inputType);
            return(-1);
         }
         tULongInt = strtoul(inputData, (char **)NULL,10);
         if(errno == ERANGE)
         {  fprintf(stderr, "\nCannot fit into UINT32;inputdata '%s'; inputType '%d';",
                                                          inputData,inputType);
            return(-1);
         }
         memcpy(data,&tULongInt,outputlength);
         break;
      case HNC_INT32:
         if(inputType != WORD_DIGIT)
         {  fprintf(stderr, "\nInvalid data for INT32;len '%d'; inputType '%d';",
                                                           inputDataLen,inputType);
            return(-1);
         }
         tLongInt = strtol(inputData, (char **)NULL,10);
         if(errno == ERANGE)
         {  fprintf(stderr, "\nCannot fit into INT32;inputdata '%s'; inputType '%d';",
                                                          inputData,inputType);
            return(-1);
         }
         memcpy(data,&tLongInt,outputlength);
         break;
      case HNC_FLOAT:
         if(inputType != WORD_FLOAT && inputType != WORD_DIGIT)
         {  fprintf(stderr, "\nInvalid data for FLOAT;len '%d'; inputType '%d';",
                                                          inputDataLen,inputType);
            return(-1);
         }
         tDouble = atof(inputData);
         if(fabs(tDouble) == HUGE_VAL)
         {
            fprintf(stderr, "\nCannot fit into FLOAT;inputdata '%s'; inputType '%d';",
                                                          inputData,inputType);
            return(-1);
         }
         tFloat = (float)tDouble;
         memcpy(data,&tFloat,outputlength);
         break;
      case HNC_DOUBLE:
         if(inputType != WORD_FLOAT && inputType != WORD_DIGIT)
         {
            fprintf(stderr, "\nInvalid data for DOUBLE;len '%d'; inputType '%d';",
                                                          inputDataLen,inputType);
            return(-1);
         }
         tDouble = atof(inputData);
         if(fabs(tDouble) == HUGE_VAL)
         {
            fprintf(stderr, "\nCannot fit into FLOAT;inputdata '%s'; inputType '%d';",
                                                          inputData,inputType);
            return(-1);
         }
         memcpy(data,&tDouble,outputlength);
         break;
      case HNC_INT16:
         if(inputType != WORD_DIGIT)
         {
            fprintf(stderr, "\nInvalid data for INT16;len '%d'; inputType '%d';",
                                                          inputDataLen,inputType);
            return(-1);
         }
         tInt = atoi(inputData);
         if(tInt >= SHRT_MIN && tInt <= SHRT_MAX)
            tShort = (short)tInt;
         else
         {
            fprintf(stderr, "\nCannot fit into INT16;input data '%s'; inputType '%d';",
                                                          inputData,inputType);
            return(-1);
         }
         memcpy(data,&tShort,outputlength);
         break;
      case HNC_UINT16:
         if(inputType != WORD_DIGIT)
         {
            fprintf(stderr, "\nInvalid data for INT16;len '%d'; inputType '%d';",
                                                         inputDataLen,inputType);
            return(-1);
         }
         tInt = atoi(inputData);
         if(tInt >= 0 && tInt <= USHRT_MAX)
            tUShort = (unsigned short)tInt;
         else
         {
            fprintf(stderr, "\nCannot fit into INT16;input data '%s'; inputType '%d';",
                                                        inputData,inputType);
            return(-1);
         }
         memcpy(data,&tUShort,outputlength);
         break;
      case HNC_INTSTR:
      case HNC_FLOATSTR:
      case HNC_STRING:
      case HNC_DATESTR:
      case HNC_TIMESTR:
      case HNC_DTSTR:
      case HNC_CHAR:
         memcpy(data,inputData,inputDataLen);
         break;
      default:
            fprintf(stderr, "\nInvalid data type;type '%d';",outputtype);
            return(-1);
   }

   return(0);
}


long WriteDataElement
(char *newData, tRecField *field, char *token, int tokenType)
{
   if(StringToHNCType(newData+field->offset, token, 
                    field->type, field->length, tokenType) != 0)
      return(-1);

   return(0);
}


long CheckTableTypeMatch(tRecField *fld, tTableTemplate *table)
{
   tRecField    *recFld, *tabFld;

   /* Field must be a record type */
   if(fld->type != RECORD_TYPE)
      return(0);

   /* If the field lists are the same, it is OK */
   if(table->fieldList == fld->typeFmt->record.fieldList)
      return(1);

   /* Loop through record fields for the given table */
   /* and check the corresponding fields from the    */
   /* given record to make sure the types match      */
   tabFld = table->fieldList; 
   recFld = fld->typeFmt->record.fieldList;
   for(; recFld && tabFld; recFld=recFld->next, tabFld=tabFld->next)
   {  /* Must have matching types */   
      if(tabFld->type != recFld->type)
         return(0);

      /* If record types, record must have the same name */
      if(tabFld->type == RECORD_TYPE)
      {  if(strcmp(tabFld->typeFmt->record.recName, recFld->typeFmt->record.recName))
            return(0);
      }
   }

   /* If either has remaining fields, it is an error */
   if(tabFld || recFld)
      return(0);

   return(1);
}


long LoadDataElement
(char *newData, tTableTemplate *table, tTableTemplate *tblList, 
   int nbrDat, char *buf, FILE *fp, char *from)
{
   int           loopCount, k;
   int           tokenType;
   char          token[256], errMsg[256];
   tRecField     *fld;

   loopCount = 0;
   k = 0;
   fld = table->fieldList;
   while(loopCount < table->fieldCount)
   {
      GetWord(buf,&k, token, &tokenType);
      if(tokenType == WORD_END)
      {  /* read next line */
         if(GetRcrd(buf,1024,fp,decrypt)==NULL)
         {  if(nbrDat==-1)
            {  if(from) sprintf(errMsg, "Failed to read default table data "
                                        "for table '%s' from file '%s'", 
                                        table->tableName, from);
               else  sprintf(errMsg, "Failed to read default table data for table '%s'",
                                     table->tableName);
            }
            else
            {  if(from) sprintf(errMsg, "Failed to read table data for table '%s' "
                                        "at row '%d' from file '%s'", 
                                        table->tableName, nbrDat, from);
               else  sprintf(errMsg, "Failed to read table data for table '%s' "
                                     "at row '%d'",
                                     table->tableName, nbrDat);
            }
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }

         k = 0;
      }
      else if(tokenType != WORD_DELIM)
      {
         loopCount++;
         if(!fld)
         {  if(nbrDat==-1)
            {  if(from) sprintf(errMsg, "Too many data fields in default data record "
                                        "for table '%s' from file '%s'", 
                                        table->tableName, from);
               else sprintf(errMsg, "Too many data fields in default data record "
                                    "in table '%s'", 
                                    table->tableName);
            }
            else
            {  if(from) sprintf(errMsg, "Too many data fields at row number '%d' "
                                        "in table '%s' from file '%s'", 
                                        nbrDat, table->tableName, from);
               else sprintf(errMsg, "Too many data fields at row number '%d' "
                                    "in table '%s'", 
                                    nbrDat, table->tableName);
            }

            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }

         if(IsString(fld->type) && (strlen(token) > fld->length))
         {   
            if(nbrDat==-1)
            {  if(from) sprintf(errMsg, "Data field '%d' in default record is too long.  "
                                        "It is '%d' bytes but limit for column is '%ld', "
                                        "in table '%s' from file '%s'",
                                        loopCount, strlen(token), fld->length, 
                                        table->tableName, from);
               else sprintf(errMsg, "Data field '%d' in default record too long.  "
                                    "It is '%d' bytes but limit for column is '%ld', "
                                    "in table '%s'",
                                    loopCount, strlen(token), fld->length, 
                                    table->tableName);
            }
            else
            {  if(from) sprintf(errMsg, "Data field '%d' in row '%d' is too long.  "
                                        "It is '%d' bytes but limit for column is '%ld', "
                                        "in table '%s' from file '%s'",
                                        loopCount, nbrDat, strlen(token), fld->length, 
                                        table->tableName, from);
               else sprintf(errMsg, "Data field '%d' in row '%d' is too long.  "
                                    "It is '%d' bytes but limit for column is '%ld', "
                                    "in table '%s'",
                                    loopCount, nbrDat, strlen(token), fld->length, 
                                    table->tableName);
            }
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }
         else if(fld->type == RECORD_TYPE)
         {  tTableTemplate *tmpTab;

            /* If the field is a record, the token should be a table name */
            for(tmpTab=tblList; tmpTab; tmpTab=tmpTab->next)
               if(!NoCaseStrcmp(tmpTab->tableName, token))
                  break;

            /* If no matching table is found, error out */
            if(!tmpTab)
            {  if(nbrDat==-1)
               {  if(from) sprintf(errMsg, "Failed to write data element for column '%d' "
                                           "in default record for table '%s' from file '%s' " 
                                           "because table '%s' is not defined",
                                           loopCount, table->tableName, from, token);
                  else sprintf(errMsg, "Failed to write data element for column '%d' "
                                       "in default record for table '%s' " 
                                       "because table '%s' is not defined",
                                       loopCount, table->tableName, token);
               }
               else
               {  if(from) sprintf(errMsg, "Failed to write data element for column '%d' "
                                           "in row '%d' of table '%s' from file '%s' " 
                                           "because table '%s' is not defined",
                                           loopCount, nbrDat, table->tableName, from, token);
                  else sprintf(errMsg, "Failed to write data element for column '%d' "
                                       "in row '%d' of table '%s' " 
                                       "because table '%s' is not defined",
                                       loopCount, nbrDat, table->tableName, token);
               }
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }

            /* Table must have compatible type with field type */
            if(!CheckTableTypeMatch(fld, tmpTab))
            {  if(nbrDat==-1)
               {  if(from) sprintf(errMsg, "Failed to write data element for column '%d' "
                                           "in default record for table '%s' from file '%s' " 
                                           "because table '%s' is not the right format",
                                           loopCount, table->tableName, from, token);
                  else sprintf(errMsg, "Failed to write data element for column '%d' "
                                       "in default record for table '%s' " 
                                       "because table '%s' is not the right format",
                                       loopCount, table->tableName, token);
               }
               else
               {  if(from) sprintf(errMsg, "Failed to write data element for column '%d' "
                                           "in row '%d' of table '%s' from file '%s' " 
                                           "because table '%s' is not the right format",
                                           loopCount, nbrDat, table->tableName, from, token);
                  else sprintf(errMsg, "Failed to write data element for column '%d' "
                                       "in row '%d' of table '%s' " 
                                       "because table '%s' is not the right format",
                                       loopCount, nbrDat, table->tableName, token);
               }
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }

            /* Set the table pointer in the data buffer */
            memcpy(newData+fld->offset, (char*)&tmpTab, sizeof(tTableTemplate*));
         }
         else if(WriteDataElement(newData, fld, token, tokenType) != 0)
         {  if(nbrDat==-1) 
            {  if(from) sprintf(errMsg, "Failed to write data element for column '%d' "
                                        "in default record for table '%s' from file '%s'", 
                                        loopCount, table->tableName, from);
               else sprintf(errMsg, "Failed to write data element for column '%d' "
                                    "in default record for table '%s'",
                                     loopCount, table->tableName);
            }
            else
            {  if(from) sprintf(errMsg, "Failed to write data element for column '%d' "
                                        "in row '%d' of table '%s' from file '%s'", 
                                        loopCount, nbrDat, table->tableName, from);
               else sprintf(errMsg, "Failed to write data element for column '%d' "
                                    "in row '%d' of table '%s'",
                                     loopCount, nbrDat, table->tableName);
            }
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }

         fld = fld->next; 
      }
   }

   return(0);
}


static int  gKeyType, gKeyLength;
static char *gDupKey;
int CompareData(const void *x, const void *y)
{
   char  *left = *((char**)x);
   char  *right = *((char**)y);
   float insertIt;

   if(gKeyType == HNC_INT32)
      insertIt = (float)(*((long*)left) - *((long*)right));
   else if(gKeyType == HNC_INT16)
      insertIt = (float)(*((short*)left) - *((short*)right));
   else if(gKeyType == HNC_FLOAT)
      insertIt = *((float*)left) - *((float*)right);
   else
      insertIt = (float)memcmp(left, right, gKeyLength);

   if(insertIt > 0.0) return(1);
   else if(insertIt < 0.0) return(-1);
   else
   {  gDupKey = left;
      return(0);
   }
}


long LoadTableData(tTableTemplate *table, tTableTemplate *tblList, FILE *fp, char *from)
{
   int       nbrDat, loopCount, k, i;
   char      buf[1024], errMsg[256];
   char      *newData;

   /* Read the DAT line */
   GetRcrd(buf, 1024, fp, decrypt);
   if(sscanf(buf, "DAT . %d\n",&nbrDat) != 1)
   {  if(from) sprintf(errMsg, "LoadTableData from '%s', bad "
                               "format for 'DAT' = '%s' for table '%s'", 
                               from, buf, table->tableName);
      else sprintf(errMsg, "LoadTableData, bad "
                           "format for 'DAT' = '%s' for table '%s'", 
                           buf, table->tableName);
      SetConfigErrorMessage(errMsg);
      throw(CONFIGURATION_EXCEPTION);
   }

   /* Must have fields defined for table */
   if(table->fieldCount <= 0 && !table->fieldList)
   {  if(from) sprintf(errMsg, "LoadTableData from '%s', no fields "
                               "define for table '%s'", from, table->tableName);
      else sprintf(errMsg, "LoadTableData, no fields "
                           "define for table '%s'", table->tableName);
      SetConfigErrorMessage(errMsg);
      throw(CONFIGURATION_EXCEPTION);
   }

   /* Can't have <= 0 rows in the table */
   if(nbrDat <= 0) 
   {  if(from) sprintf(errMsg, "LoadTableData from '%s', bad value, '%d', for number "
                               "of rows in table '%s'", from, nbrDat, table->tableName);
      else sprintf(errMsg, "LoadTableData, bad value, '%d', for number "
                           "of rows in table '%s'", nbrDat, table->tableName);
      SetConfigErrorMessage(errMsg);
      throw(CONFIGURATION_EXCEPTION);
   }


   loopCount = 0;
   table->numRows = nbrDat;

   /* Allocate space for all the data */
   if((table->dataList = (char**)MyCalloc(table->numRows, sizeof(char*)))==NULL)
   {  fprintf(stderr, "\nFailed to allocate table data for table '%s'\n\n", table->tableName);
      throw(MALLOC_EXCEPTION);
   }
   for(i=0; i<table->numRows; i++)
   {  if((table->dataList[i] = (char*)MyMalloc(table->dataLength))==NULL)
      {  fprintf(stderr, "\nFailed to allocate table data record for table '%s'\n\n", table->tableName);
         throw(MALLOC_EXCEPTION);
      }
   }

   while(loopCount < nbrDat)
   {
      /* Read the data record */
      if(GetRcrd(buf,1024,fp,decrypt)==NULL)
      {  if(from) sprintf(errMsg, "LoadTableData from '%s', failed to read "
                                  "data record for table '%s'",  
                                  from, table->tableName);
         else sprintf(errMsg, "LoadTableData, failed to read "
                              "data record for table '%s'", table->tableName);
         SetConfigErrorMessage(errMsg);
         throw(CONFIGURATION_EXCEPTION);
      }

      k = 0;

      /* Convert the character record into the correct format */
      newData = table->dataList[loopCount++];
      if(LoadDataElement(newData, table, tblList, loopCount, buf, fp, from) != 0)
      {  if(from) sprintf(errMsg, "LoadTableData from '%s', LoadDataElement "
                                  "failed on record %d for table '%s'", 
                                  from, loopCount, table->tableName);
         else sprintf(errMsg, "LoadTableData, LoadDataElement "
                              "failed on record %d for table '%s'", 
                              loopCount, table->tableName);
         SetConfigErrorMessage(errMsg);
         throw(CONFIGURATION_EXCEPTION);
      }
   }

   /* Make sure data is sorted */
   gKeyType = table->keyType;
   gKeyLength = table->keyLength;
   gDupKey = NULL;
   qsort(table->dataList, table->numRows, sizeof(char*), CompareData);

   /* See if have duplicate */
   if(gDupKey)
   {  /* Error... can't have records with duplicate keys */
      if(table->keyType == HNC_STRING)
      {  if(from) sprintf(errMsg, "LoadTableData from '%s', have record "
                                  "with duplicate key '%-*.*s' for table '%s'", 
                                  from, table->keyLength, table->keyLength, 
                                  gDupKey, table->tableName);
         else sprintf(errMsg, "LoadTableData, have record with "
                              "duplicate key '%-*.*s' for table '%s'", 
                               table->keyLength, table->keyLength, 
                               gDupKey, table->tableName);
         SetConfigErrorMessage(errMsg);
         throw(CONFIGURATION_EXCEPTION);
      }
      else if(table->keyType == HNC_INT32)
      {  if(from) sprintf(errMsg, "LoadTableData from '%s', have record "
                                  "with duplicate key '%ld' for table '%s'", 
                                  from, *((long*)gDupKey), table->tableName);
         else sprintf(errMsg, "LoadTableData, have record with "
                              "duplicate key '%ld' for table '%s'", 
                               *((long*)gDupKey), table->tableName);
         SetConfigErrorMessage(errMsg);
         throw(CONFIGURATION_EXCEPTION);
      }
      else if(table->keyType == HNC_INT16)
      {  if(from) sprintf(errMsg, "LoadTableData from '%s', have record "
                                  "with duplicate key '%d' for table '%s'", 
                                  from, *((short*)gDupKey), table->tableName);
         else sprintf(errMsg, "LoadTableData, have record with "
                              "duplicate key '%d' for table '%s'", 
                               *((short*)gDupKey), table->tableName);
         SetConfigErrorMessage(errMsg);
         throw(CONFIGURATION_EXCEPTION);
      }
      else
      {  if(from) sprintf(errMsg, "LoadTableData from '%s', have record "
                                  "with duplicate key '%f' for table '%s'", 
                                  from, *((float*)gDupKey), table->tableName);
         else sprintf(errMsg, "LoadTableData, have record with "
                              "duplicate key '%f' for table '%s'", 
                               *((float*)gDupKey), table->tableName);
         SetConfigErrorMessage(errMsg);
         throw(CONFIGURATION_EXCEPTION);
      }
   }

   return(0);
}


long LoadTableFields
(tTableTemplate *table, tTableTemplate *tblList, FILE *fp, 
   int *nbrFldPtr, int *dataLen, char *from, tType *globalTypes)
{
   int              loopCount, k;
   char             token[256],buf[256],errMsg[256];
   tRecField        *field=NULL;
   tType            *type;   
   int              keyInd, haveType=0, fldType, tokenType, fldLen, numScans;

   *dataLen = 0;

   /********************************************************************/
   /* Read the first line which will either be COL . <num columns>, or */
   /* it will give the type name that the table will be using          */
   /********************************************************************/
   GetRcrd(buf, 1024, fp, decrypt);
   if(sscanf(buf, "COL . %d\n",nbrFldPtr) != 1)
   {  /* Look for matching type name */  
      buf[strlen(buf)-1] = '\0';
      for(type=globalTypes; type; type=type->next)
      {  if(!NoCaseStrcmp(type->typeName, buf))
            break;
      }

      if(!type)
      {  if(from) sprintf(errMsg, "LoadTableFields from '%s' for table '%s', bad format "
                                  "of COL indicator, or type '%s' does not exist", 
                                  from, table->tableName, buf);
         else sprintf(errMsg, "LoadTableFields for table '%s', bad format "
                              "of COL indicator, or type '%s' does not exist", 
                              table->tableName, buf);
         SetConfigErrorMessage(errMsg);
         throw(CONFIGURATION_EXCEPTION);
      }
      else if(type->type != RECORD_TYPE)
      {  if(from) sprintf(errMsg, "LoadTableFields from '%s' for table '%s', type '%s' "
                                  "is not a record type", 
                                  from, table->tableName, buf);
         else sprintf(errMsg, "LoadTableFields for table '%s', type '%s' "
                              "is not a record type", 
                              table->tableName, buf);
         SetConfigErrorMessage(errMsg);
         throw(CONFIGURATION_EXCEPTION);
      }
      else 
      {  if((CreateTableFromType(table, &type->fmt.record, tblList)))
         {  if(from) sprintf(errMsg, "LoadTableFields from '%s' for table '%s', failed "
                                     "in CreateTableFromType for type '%s' ",
                                     from, table->tableName, buf);
            else sprintf(errMsg, "LoadTableFields, for table '%s', failed "
                                 "in CreateTableFromType for type '%s' ",
                                 table->tableName, buf);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }

         return(0);
      }    
   }
   else if(*nbrFldPtr > 0)
   {
      loopCount = 0;


      /***********************************/
      /* Init keyLength for accumalation */
      /***********************************/
      while(loopCount < *nbrFldPtr)
      {
         /********************************************************/
         /* Only allow sub-records as the last field in the list */ 
         /* If at this point and field is a record, then error   */
         /********************************************************/
         if(field)
         {  if(field->type == RECORD_TYPE)
            {  if(from) sprintf(errMsg, "LoadTableFields from '%s', field '%s' in table '%s' "
                                        "cannot be a record because it is not the last field "
                                        "in the field list",
                                        from, field->fieldName, table->tableName); 
               else sprintf(errMsg, "LoadTableFields, field '%s' in table '%s' "
                                    "cannot be a record because it is not the last field "
                                    "in the field list",
                                    field->fieldName, table->tableName); 
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }
         }
 
         /****************************/
         /* Allocate new field space */
         /****************************/
         if(table->fieldList)
         {  if((field->next = (tRecField*)MyMalloc(sizeof(tRecField)))==NULL)
            {  fprintf(stderr, "\nFailed to allocate field\n\n");
               throw(MALLOC_EXCEPTION);
            }

            field = field->next;
         }
         else
         {  if((field = (tRecField*)MyMalloc(sizeof(tRecField)))==NULL)
            {  fprintf(stderr, "\nFailed to allocate field\n\n");
               throw(MALLOC_EXCEPTION);
            }

            table->fieldList = field;
         }
         memset((char*)field, 0, sizeof(tRecField));

         loopCount++;

         /**************************************************/
         /* Get field name and default value, if available */
         /**************************************************/
         if(GetRcrd(buf,1024,fp,decrypt)==NULL)
         {  if(from) sprintf(errMsg, "LoadTableFields from '%s', failed to read "
                                     "field name for table '%s'", from, table->tableName);
            else sprintf(errMsg, "LoadTableFields, failed to read "
                                 "field name for table '%s'", table->tableName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }

         if((numScans = sscanf(buf, "%s", token)) != 1)
         {  if(from) sprintf(errMsg, "LoadTableFields from '%s', bad format of "
                                     "field name for table '%s'", from, table->tableName);
            else sprintf(errMsg, "LoadTableFields, bad format of "
                                 "field name for table '%s'", table->tableName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }

         if((field->fieldName = (char*)MyMalloc(strlen(token)+1))==NULL)
         {  fprintf(stderr, "\nFailed to allocate newFiled->fieldName\n\n");
            throw(MALLOC_EXCEPTION);
         }
         strcpy(field->fieldName, token);


         /**************************************************************/
         /* Get key flag -- note this is not really used, but is there */
         /* for backward compatability.  Only first field can be a key */
         /* This field may not exist at all.                           */
         /**************************************************************/
         if(GetRcrd(buf,1024,fp,decrypt)==NULL)
         {  if(from) sprintf(errMsg, "LoadTableFields from '%s', failed to read "
                                     "key type for table '%s'", from, table->tableName);
            else sprintf(errMsg, "LoadTableFields, failed to read "
                                 "key type for table '%s'", table->tableName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }

         k = 0;
         GetWord(buf,&k,token,&tokenType);
         if(tokenType == WORD_DIGIT)
         {  keyInd = atoi(token);
       
            /* Only allow key in first field */
            if(keyInd==1 && field!=table->fieldList)
            {  if(from) sprintf(errMsg, "LoadTableFields from '%s', field '%s' "
                                        "in table '%s' cannot be a key because "
                                        "it is not the first field",
                                        from, field->fieldName, table->tableName);
               else sprintf(errMsg, "LoadTableFields, field '%s' "
                                    "in table '%s' cannot be a key because "
                                    "it is not the first field",
                                    field->fieldName, table->tableName);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }

            /* Determine if have the key indicator, or if have the type */
            if(keyInd!=1 && keyInd!=0)
               haveType = 1;
            else
               haveType = 0;
         }
         else
            haveType = 1;
         

         /*****************************/
         /* Get field type and length */
         /*****************************/
         if(!haveType)
         {  if(GetRcrd(buf,1024,fp,decrypt)==NULL)
            {  if(from) sprintf(errMsg, "LoadTableFields from '%s', failed to read "
                                        "field type from field '%s' for table '%s'", 
                                        from, field->fieldName, table->tableName);
               else sprintf(errMsg, "LoadTableFields, failed to read "
                                    "field type from field '%s' for table '%s'", 
                                    field->fieldName, table->tableName);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }
         }

         k = 0;
         if((numScans=sscanf(buf, "%s . %d", token, &fldLen))==1)
            fldLen = 0;
         else if(numScans!=2)
         {  if(from) sprintf(errMsg, "LoadTableFields from '%s', bad scan of "
                                     "field type from field '%s' for table '%s'", 
                                     from, field->fieldName, table->tableName);
            else sprintf(errMsg, "LoadTableFields, bad scan of "
                                 "field type from field '%s' for table '%s'", 
                                 field->fieldName, table->tableName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }

         /* If first byte is a digit, then it is a normal data type */
         /* If not, then it must be a record type.                  */
         if(isdigit(token[0]))
         {  if((field->type = CheckFldType(token,(void **) NULL, &fldLen))==-1)
            {  if(from) sprintf(errMsg, "LoadTableFields from '%s', bad "
                                        "field type from field '%s' for table '%s'", 
                                        from, field->fieldName, table->tableName);
               else sprintf(errMsg, "LoadTableFields, bad "
                                    "field type from field '%s' for table '%s'", 
                                    field->fieldName, table->tableName);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }

            field->length = (long)fldLen;
         }
         else
         {  tType *globType;

            /* Make sure type is known user defined type */
            for(globType=globalTypes; globType; globType=globType->next)
               if(!NoCaseStrcmp(token, globType->typeName))
                  break;

            if(!globType)
            {  if (from) sprintf(errMsg, "LoadTableFields from '%s', type '%s' given for "
                                         "field '%s' in table '%s' is not defined", 
                                         from, token, field->fieldName, table->tableName);
               else sprintf(errMsg, "LoadTableFields, type '%s' given for "
                                    "field '%s' in table '%s' is not defined", 
                                    token, field->fieldName, table->tableName);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }
            else
            {  if(globType->type != RECORD_TYPE)
               {  if (from) sprintf(errMsg, "LoadTableFields from '%s', type '%s' given for "
                                            "field '%s' in table '%s' is not a record", 
                                            from, token, field->fieldName, table->tableName);
                  else sprintf(errMsg, "LoadTableFields, type '%s' given for "
                                       "field '%s' in table '%s' is not a record", 
                                       token, field->fieldName, table->tableName);
                  SetConfigErrorMessage(errMsg);
                  throw(CONFIGURATION_EXCEPTION);
               }

               field->type = RECORD_TYPE;
               field->typeFmt = &globType->fmt;

               /* Field length is the sizeof a table pointer */
               field->length = sizeof(tTableTemplate*);
            }
         }



         /**************************************************************/
         /* If have first field, it is the key and it must be a short, */
         /* long, string, or float                                     */ 
         /**************************************************************/
         if(field == table->fieldList)
         {  /* Set key length for table */
            table->keyLength = field->length;

            /* Check the type */
            if(field->type!=HNC_INT32 && 
               field->type!=HNC_INT16 && 
               field->type!=HNC_STRING && 
               field->type!=HNC_FLOAT) 
            {  if(from) sprintf(errMsg, "LoadTableFields from '%s', key field "
                                        "'%s' in table '%s' must be an "
                                        "HNC_INT32, HNC_INT16, HNC_STRING, or HNC_FLOAT", 
                                        from, field->fieldName, table->tableName);
               else sprintf(errMsg, "LoadTableFields, key field "
                                    "'%s' in table '%s' must be an "
                                    "HNC_INT32, HNC_INT16, HNC_STRING, or HNC_FLOAT", 
                                    field->fieldName, table->tableName);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }

            table->keyType = field->type;
         }

         /**************/
         /* Set offset */
         /**************/
         /* Get the field type */
         if(field->type != RECORD_TYPE)
            fldType = field->type;
         else
         {  /* Get type of first field in record */
            fldType = field->typeFmt->record.fieldList->type;
         }

         /* Make sure fields are aligned for their type */
         if((fldType == HNC_INT16 || fldType == HNC_UINT16) && 
                                              (*dataLen%sizeof(short)!=0))
            *dataLen += sizeof(short) - *dataLen%sizeof(short);
         else if((fldType == HNC_INT32 || fldType == HNC_UINT32) && 
                                              (*dataLen%sizeof(long)!=0))
            *dataLen += sizeof(long) - *dataLen%sizeof(long);
         else if((fldType == HNC_FLOAT) && (*dataLen%sizeof(float)!=0))
            *dataLen += sizeof(float) - *dataLen%sizeof(float);
         else if((fldType == HNC_DOUBLE) && (*dataLen%sizeof(double)!=0))
            *dataLen += sizeof(double) - *dataLen%sizeof(double);

         field->offset = *dataLen;
         *dataLen += field->length;
      }
   }

   return(0);
}


long LoadTableDefaults
(tTableTemplate *table, tTableTemplate *tblList, FILE *fp, char *from)
{ 
   char       buf[1024], errMsg[256];
   long       startSpot;

   /* Mark the current file location */
   startSpot = ftell(fp);

   /* Look for the word DEFAULT_DATA */
   GetRcrd(buf, 1024, fp, decrypt);
   if(!strstr(buf, "DEFAULT_DATA"))
   {  /* No default data.  Reset file pointer and return */
      fseek(fp, startSpot, 0);
      table->defaultData = NULL;
      return(0);
   }

   /* Allocate space for default record */
   if((table->defaultData = (char*)MyMalloc(table->dataLength))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)table->defaultData, 0, table->dataLength);

   if(GetRcrd(buf,1024,fp,decrypt)==NULL)
   {  if(from) sprintf(errMsg, "LoadTableDefaults from '%s', failed to read "
                               "data record for table '%s'",
                               from, table->tableName);
      else sprintf(errMsg, "LoadTableDefaults, failed to read "
                           "data record for table '%s'", table->tableName);
      SetConfigErrorMessage(errMsg);
      throw(CONFIGURATION_EXCEPTION);
   }

   if(LoadDataElement(table->defaultData, table, tblList, -1, buf, fp, from) != 0)
   {  if(from) sprintf(errMsg, "LoadTableDefaults from '%s', LoadDataElement "
                               "failed for  table '%s'",
                               from, table->tableName);
      else sprintf(errMsg, "LoadTableDefaults, LoadDataElement "
                           "failed for table '%s'",
                           table->tableName);
      SetConfigErrorMessage(errMsg);
      throw(CONFIGURATION_EXCEPTION);
   }
   
   return(0);
}


long LoadTables
(tTableTemplate **tblList, char *buf, FILE *fp, long oldSpot, char *from, tType **globTypes)
{
   int            nbrTbl, loopCount, k,tokenType;
   char           token[256], errMsg[256];
   tTableTemplate *tmpTableTemplate,*newTable;

   /* reset file to beginning of the segment */
   fseek(fp, oldSpot, 0);

   GetRcrd(buf, 1024, fp, decrypt);
   if(sscanf(buf, "TBL . %d\n",&nbrTbl) != 1)
   {  if(from) sprintf(errMsg, "LoadTable from '%s', bad "
                               "format for 'TBL' = '%s'", from, buf);
      else sprintf(errMsg, "LoadTable, bad format for 'TBL' = '%s'", buf);
      SetConfigErrorMessage(errMsg);
      throw(CONFIGURATION_EXCEPTION);
   }
   else if(nbrTbl > 0)
   {
      loopCount = 0;
      tmpTableTemplate = *tblList;
      while(loopCount < nbrTbl)
      {
         if(GetRcrd(buf,1024,fp,decrypt)==NULL)
         {  if(from) sprintf(errMsg, "LoadTable from '%s', failed to read static "
                                     "data table name", from);
            else sprintf(errMsg, "LoadTable, failed to read static "
                                 "data table name");
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }

         /****************************/
         /* Allocate new table space */
         /****************************/
         if((newTable = (tTableTemplate*)MyMalloc(sizeof(tTableTemplate)))==NULL)
         {  fprintf(stderr, "\nFailed to allocate newTable\n\n");
            throw(MALLOC_EXCEPTION);
         }
         memset((char*)newTable, 0, sizeof(tTableTemplate));

         k = 0;
         loopCount++;
         /*************************************************/
         /* Put new table in proper position of tableList */
         /*************************************************/
         if(tmpTableTemplate==NULL)
            *tblList = tmpTableTemplate = newTable;
         else
         {
            while(tmpTableTemplate->next)
                tmpTableTemplate = tmpTableTemplate->next;
            tmpTableTemplate->next = newTable;
         }

         /******************/
         /* Get table name */
         /******************/
         GetWord(buf,&k,token,&tokenType);
         if((newTable->tableName = (char*)MyMalloc(strlen(token)+1))==NULL)
         {  fprintf(stderr, "\nFailed to allocate newTable->tableName\n\n");
            throw(MALLOC_EXCEPTION);
         }
         strcpy(newTable->tableName, token);

         /*****************************/
         /* Load fields & related info*/
         /*****************************/
         if(LoadTableFields(newTable, *tblList, fp, &newTable->fieldCount, 
                               &newTable->dataLength, from, *globTypes))
         {  if(from) sprintf(errMsg, "LoadTable from '%s', failed "
                                     "LoadTableFields for table '%s'", 
                                      from, newTable->tableName);
            else sprintf(errMsg, "LoadTable, failed "
                                 "LoadTableFields for table '%s'", 
                                 newTable->tableName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }

         /*******************************/
         /* Load default record, if any */
         /*******************************/
         if(LoadTableDefaults(newTable, *tblList, fp, from))
         {  if(from) sprintf(errMsg, "LoadTable from '%s', failed "
                                     "LoadTableDefault for table '%s'", 
                                      from, newTable->tableName);
            else sprintf(errMsg, "LoadTable, failed "
                                 "LoadTableDefault for table '%s'", 
                                 newTable->tableName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }

         /********************/
         /* Load in the data */
         /********************/
         if(LoadTableData(newTable, *tblList, fp, from))
         {  if(from) sprintf(errMsg, "LoadTable from '%s', failed "
                                     "LoadTableData for table '%s'", 
                                      from, newTable->tableName);
            else sprintf(errMsg, "LoadTable, failed "
                                 "LoadTableData for table '%s'", 
                                 newTable->tableName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }

         /******************************************/
         /* Add the table to the global tType list */
         /******************************************/
         AddTableToEngineTypes(newTable, globTypes);
      }
   }

   return(0);
}


long FreeTable(tTableTemplate *table)
{
   int i;

   /***********************/
   /* Free the table data */
   /***********************/
   for(i=0; i<table->numRows; i++)
      MyFree((void*)table->dataList[i]);  
   MyFree((void*)table->dataList);  
   
   /****************************/
   /* Free the table structure */
   /****************************/
   if(table->defaultData)
      MyFree((void*)table->defaultData);

   MyFree((void*)table->tableName);
   MyFree((void*)table);

   return(OK);
}


#define MATCH_LOOKUP   1
#define RANGE_LOOKUP   2
#define DIRECT_LOOKUP  3
#define GET_DEFLT      4

#define MAX_KEYS      10 
tAtom TableMultiLookup(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tTableTemplate   *table;
   tAtom            retVal;
   tRecData         *inputRecord, *record;
   short            lookupType[MAX_KEYS];
   char             keyType[MAX_KEYS];
   tAtom*           keys[MAX_KEYS];
   long             lastIdx, a, k, numKeys, status=0;


   /****************************************************************************/
   /* Get the input record.  Unlike other lookup routines, the record is first */
   /****************************************************************************/
   inputRecord = (tRecData*)aParams[0].data.uVal;

   /*********************************************************/
   /* Read all the keys and lookup types that are passed in */
   /*********************************************************/
   if(numArgs < 3)
   {  ZERO(retVal.data.num)
      sprintf(funcCallData->errStatus.errStr, "Table_Multi_Lookup must have at "
                                              "least 3 parameters");
      funcCallData->errStatus.errCode = FAILURE|TABLE_LOOKUP|BAD_PARAMS;
      return(retVal);
   }

   a = 1; k = 0;
   while(1)
   {  /* Get key */     
      keys[k] = aParams + a;
      if(aParams[a].type == NUMERIC_TYPE) keyType[k] = NUMERIC_TYPE;
      else keyType[k] = HNC_STRING;


      /* Get lookup type */
      a++;
      lookupType[k] = NUM_GETSHRT(aParams[a].data.num);

      if(lookupType[k] != MATCH_LOOKUP && lookupType[k] != RANGE_LOOKUP &&
         lookupType[k] != DIRECT_LOOKUP && lookupType[k] != GET_DEFLT)
      {  ZERO(retVal.data.num)
         sprintf(funcCallData->errStatus.errStr, 
                          "Parameter #%ld to Table_Multi_Lookup, %d, is "
                          "not a valid lookup type", a, lookupType[k]);
         funcCallData->errStatus.errCode = FAILURE|TABLE_LOOKUP|BAD_PARAMS;
         return(retVal);
      }
 

      /* Go to next key and argument  */
      a++;
      if(a == numArgs || a == numArgs-1)
         break;


      /* Increment and check the key index */
      k++;
      if(k == MAX_KEYS)
      {  ZERO(retVal.data.num)
         sprintf(funcCallData->errStatus.errStr, 
                          "Too many keys specified for Table_Multi_Lookup.  Max "
                          "is %d", MAX_KEYS);
         funcCallData->errStatus.errCode = FAILURE|TABLE_LOOKUP|BAD_PARAMS;
         return(retVal);
      }
   }


   /******************************************************************/
   /* Get the table to be used.  If there is a last argument, assume */
   /* it gives the name of the table.  Otherwise, the table is tied  */
   /* directly to the record type.                                   */
   /******************************************************************/
   if(a == (numArgs - 1))
   {  tTableTemplate   *tableList =
        (tTableTemplate*)((tFuncInit*)funcCallData->inputData)->dynTableList;

      /**********************************************/
      /* Get the table corresponding to the request */
      /**********************************************/
      table=tableList;
      while(table)
      {  if(!strcmp(aParams[a].data.strVal.str, table->tableName))
            break;

         table = table->next;
      }

      if(!table)
      {  ZERO(retVal.data.num)
         sprintf(funcCallData->errStatus.errStr, "The table '%s' does not exist",
                                                         aParams[a].data.strVal.str);
         funcCallData->errStatus.errCode = FAILURE|TABLE_LOOKUP|TABLE_NO_EXIST;
         return(retVal);
      }
   }
   else if((table = (tTableTemplate*)inputRecord->record->user)==NULL)
   {  ZERO(retVal.data.num)
      sprintf(funcCallData->errStatus.errStr, "The table '%s' does not exist", 
                                                      inputRecord->record->recName);
      funcCallData->errStatus.errCode = FAILURE|TABLE_LOOKUP|TABLE_NO_EXIST;
      return(retVal);
   }


   /*******************************************************/
   /* Loop through the keys doing the correct lookup type */
   /*******************************************************/
   numKeys = k+1;
   record = inputRecord;
   for(k=0; k<numKeys; k++)
   {  switch(table->keyType)
      {  case HNC_INT32:
            switch(lookupType[k])
            {  case MATCH_LOOKUP:
                  status = DoLongTableLookup(table, NUM_GETLNG(keys[k]->data.num), record);
                  break;
               case RANGE_LOOKUP:
                  status = DoLongTableRange(table, NUM_GETLNG(keys[k]->data.num), record);
                  break;
               case DIRECT_LOOKUP:
                  status = DoTableDirect(table, NUM_GETLNG(keys[k]->data.num), record);
                  break;
               case GET_DEFLT:
                  if(table->defaultData)
                     memcpy(*record->data, table->defaultData, table->dataLength);
                  else
                     status = 1;
                  break;
            }

#ifdef DEVELOP 
            if(status)
               fprintf(stderr, "A record for key '%ld' is "
                               "not found in table '%s'\n", 
                               NUM_GETLNG(keys[k]->data.num),
                               table->tableName);
#endif

            break; 

         case HNC_INT16:
            switch(lookupType[k])
            {  case MATCH_LOOKUP:
                  status = DoShortTableLookup(table, NUM_GETSHRT(keys[k]->data.num), record);
                  break;
               case RANGE_LOOKUP:
                  status = DoShortTableRange(table, NUM_GETSHRT(keys[k]->data.num), record);
                  break;
               case DIRECT_LOOKUP:
                  status = DoTableDirect(table, NUM_GETLNG(keys[k]->data.num), record);
                  break;
               case GET_DEFLT:
                  if(table->defaultData)
                     memcpy(*record->data, table->defaultData, table->dataLength);
                  else
                     status = 1;
                  break;
            }

#ifdef DEVELOP 
            if(status)
               fprintf(stderr, "A record for key '%d' is "
                               "not found in table '%s'\n", 
                               NUM_GETSHRT(keys[k]->data.num),
                               table->tableName);
#endif

            break; 

         case HNC_FLOAT:
            switch(lookupType[k])
            {  case MATCH_LOOKUP:
                  status = DoFloatTableLookup(table, 
                                      NUM_GETFLT(keys[k]->data.num), record); 
                  break;
               case RANGE_LOOKUP:
                  status = DoFloatTableRange(table, 
                                      NUM_GETFLT(keys[k]->data.num), record); 
                  break;
               case GET_DEFLT:
                  if(table->defaultData)
                     memcpy(*record->data, table->defaultData, table->dataLength);
                  else
                     status = 1;
                  break;
            }

#ifdef DEVELOP 
            if(status)
               fprintf(stderr, "A record for key '%f' is "
                               "not found in table '%s'\n", 
                               NUM_GETFLT(keys[k]->data.num),
                               table->tableName);
#endif
            break; 

         default:
            switch(lookupType[k])
            {  case MATCH_LOOKUP:
                  status = DoStringTableLookup(table, keys[k]->data.strVal.str,
                                                      keys[k]->data.strVal.len, record);
                  break;
               case RANGE_LOOKUP:
                  status = DoStringTableRange(table, keys[k]->data.strVal.str,
                                                     keys[k]->data.strVal.len, record);
                  break;
               case GET_DEFLT:
                  if(table->defaultData)
                     memcpy(*record->data, table->defaultData, table->dataLength);
                  else
                     status = 1;
                  break;
            }

#ifdef DEVELOP 
            if(status)
               fprintf(stderr, "A record for key '%*.*s' is "
                               "not found in table '%s'\n", 
                               keys[k]->data.strVal.len,
                               keys[k]->data.strVal.len,
                               keys[k]->data.strVal.str,
                               table->tableName);
#endif

            break; 
      }

      /* If failed a lookup and there is no default data, break out now */
      /* Otherwise, keep on going.                                      */
      if(status && !table->defaultData) 
         break;

      /* Get the next sub-record, if any.  Assume any sub-record */
      /* is going to be the last field in the record             */
      lastIdx = record->record->numFlds-1;
      if(record->flds[lastIdx].fld->type == RECORD_TYPE)
      {  /* If there are no more keys, then error out */ 
         if(k == numKeys-1)
         {  ZERO(retVal.data.num)
            sprintf(funcCallData->errStatus.errStr, "There are not enough keys to "
                                                    "Table_Multi_Lookup for "
                                                    "the record '%s'", 
                                                    inputRecord->record->recName);
            funcCallData->errStatus.errCode = FAILURE|TABLE_LOOKUP|BAD_PARAMS;
            return(retVal);
         }

         /* Pointer to table is the last field in the record */
         table = *((tTableTemplate**)(*record->data + record->flds[lastIdx].fld->offset));
         record = (tRecData*)record->flds[lastIdx].data;
      }
      else if(k < numKeys-1)
      {  ZERO(retVal.data.num)
         sprintf(funcCallData->errStatus.errStr, 
                                    "There are too many keys, %ld, for the " 
                                    "record '%s'", numKeys, inputRecord->record->recName);
         funcCallData->errStatus.errCode = FAILURE|TABLE_LOOKUP|BAD_PARAMS;
         return(retVal);
      }
   }

   if(status) ZERO(retVal.data.num)
   else ONE(retVal.data.num)
   retVal.type = NUMERIC_TYPE;

   return(retVal);
}


long DoLongTableRange(tTableTemplate *table, long key, tRecData *record)
{
   int     max, hi, lo, mid=0;
   long    compResult;

   /***************************************************/
   /* Find the data in the table that matches the key */
   /***************************************************/
   max = hi = table->numRows - 1;
   lo = 0;

   while(lo <= hi)
   {  mid = (hi + lo)/2;
      compResult = key - *((long*)table->dataList[mid]);
      if(compResult > 0)
      {  if(mid != max)
         {  if((compResult = key - *((long*)table->dataList[mid+1]))<0)
               break;
            else if(compResult == 0)
            {  mid++;
               break;
            }
         }
         else
            break;

         lo = mid + 1;
      }
      else if(compResult < 0)
         hi = mid - 1;
      else
         break;
   }

   /****************************************/
   /* Copy the data into the return record */
   /****************************************/
   memcpy(*record->data, table->dataList[mid], table->dataLength); 

   return(0);
}


long DoShortTableRange(tTableTemplate *table, short key, tRecData *record)
{
   int     max, hi, lo, mid=0;
   short   compResult;

   /***************************************************/
   /* Find the data in the table that matches the key */
   /***************************************************/
   max = hi = table->numRows - 1;
   lo = 0;

   while(lo <= hi)
   {  mid = (hi + lo)/2;
      compResult = key - *((short*)table->dataList[mid]);
      if(compResult > 0)
      {  if(mid != max)
         {  if((compResult = key - *((short*)table->dataList[mid+1]))<0)
               break;
            else if(compResult == 0)
            {  mid++;
               break;
            }
         }
         else
            break;

         lo = mid + 1;
      }
      else if(compResult < 0)
         hi = mid - 1;
      else
         break;
   }

   /****************************************/
   /* Copy the data into the return record */
   /****************************************/
   memcpy(*record->data, table->dataList[mid], table->dataLength); 

   return(0);
}


long DoFloatTableRange(tTableTemplate *table, float key, tRecData *record)
{
   int              max, hi, lo, mid=0;
   float            compResult;

   /***************************************************/
   /* Find the data in the table that matches the key */
   /***************************************************/
   max = hi = table->numRows - 1;
   lo = 0;

   while(lo <= hi)
   {  mid = (hi + lo)/2;
      compResult = key - *((float*)table->dataList[mid]);
      if(compResult > 0)
      {  if(mid != max)
         {  if((compResult = key - *((float*)table->dataList[mid+1]))<0)
               break;
            else if(compResult == 0)
            {  mid++;
               break;
            }
         }
         else
            break;

         lo = mid + 1;
      }
      else if(compResult < 0)
         hi = mid - 1;
      else
         break;
   }

   /****************************************/
   /* Copy the data into the return record */
   /****************************************/
   memcpy(*record->data, table->dataList[mid], table->dataLength); 

   return(0);
}


long DoStringTableRange(tTableTemplate *table, char *key, int keyLen, tRecData *record)
{
   int              max, hi, lo, mid=0, len;
   long             compResult;

   /***************************************************/
   /* Find the data in the table that matches the key */
   /***************************************************/
   max = hi = table->numRows - 1;
   lo = 0;

   len = (keyLen < table->keyLength) ? keyLen : table->keyLength;
   while(lo <= hi)
   {  mid = (hi + lo)/2;
      compResult = memcmp(table->dataList[mid], key, len);
      if(compResult < 0)
      {  if(mid != max)
         {  if((compResult = memcmp(table->dataList[mid+1], key, len)) < 0)
               break;
            else if(compResult == 0)
            {  mid++;
               break;
            }

            lo = mid + 1;
         }
         else
            break;
      }
      else if(compResult > 0)
         hi = mid - 1;
      else if(len<table->keyLength && table->dataList[mid][len]!='\0')
         hi = mid - 1;
      else
         break;
   }

   /****************************************/
   /* Copy the data into the return record */
   /****************************************/
   memcpy(*record->data, table->dataList[mid], table->dataLength); 

   return(0);
}


tAtom TableRangeLookup(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tTableTemplate   *table;
   tAtom            retVal;
   tRecData         *inputRecord;


   /************************/
   /* Get the input record */
   /************************/
   inputRecord = (tRecData*)aParams[1].data.uVal;

   /*******************************************************************/
   /* Get the table to be used.  If there is a third argument, assume */
   /* it gives the name of the table.  Otherwise, the table is tied   */
   /* directly to the record type.                                    */
   /*******************************************************************/
   if(numArgs > 2)
   {  tTableTemplate   *tableList =
        (tTableTemplate*)((tFuncInit*)funcCallData->inputData)->dynTableList;

      /**********************************************/
      /* Get the table corresponding to the request */
      /**********************************************/
      table=tableList;
      while(table)
      {  if(!strcmp(aParams[2].data.strVal.str, table->tableName))
            break;

         table = table->next;
      }

      if(!table)
      {  ZERO(retVal.data.num)
         sprintf(funcCallData->errStatus.errStr, "The table '%s' does not exist",
                                                         inputRecord->record->recName);
         funcCallData->errStatus.errCode = FAILURE|TABLE_LOOKUP|TABLE_NO_EXIST;
         return(retVal);
      }
   }
   else if((table = (tTableTemplate*)inputRecord->record->user)==NULL)
   {  ZERO(retVal.data.num)
      sprintf(funcCallData->errStatus.errStr, "The table '%s' does not exist", 
                                                      inputRecord->record->recName);
      funcCallData->errStatus.errCode = FAILURE|TABLE_LOOKUP|TABLE_NO_EXIST;
      return(retVal);
   }

   /**********************************************/
   /* Do the table lookup for the right key type */
   /**********************************************/
   if(table->keyType == HNC_INT32)
   {  if(DoLongTableRange(table, NUM_GETLNG(aParams[0].data.num), inputRecord)) 
         ZERO(retVal.data.num)
      else 
         ONE(retVal.data.num)
   }
   else if(table->keyType == HNC_FLOAT)
   {  if(DoFloatTableRange(table, NUM_GETFLT(aParams[0].data.num), inputRecord)) 
         ZERO(retVal.data.num)
      else 
         ONE(retVal.data.num)
   }
   else if(table->keyType == HNC_INT16)
   {  if(DoShortTableRange(table, NUM_GETSHRT(aParams[0].data.num), inputRecord)) 
         ZERO(retVal.data.num)
      else 
         ONE(retVal.data.num)
   }
   else
   {  if(DoStringTableRange(table, aParams[0].data.strVal.str,
                         aParams[0].data.strVal.len, inputRecord))
         ZERO(retVal.data.num)
      else
         ONE(retVal.data.num)
   }

   return(retVal);
}


long DoLongTableLookup(tTableTemplate *table, long key, tRecData *record)
{
   int              hi, lo, mid=0, compResult, foundMatch;

   /***************************************************/
   /* Find the data in the table that matches the key */
   /***************************************************/
   hi = table->numRows - 1;
   lo = 0;
   foundMatch = FALSE;

   while(lo <= hi)
   {  mid = (hi + lo)/2;
      compResult = key - *((long*)table->dataList[mid]);
      if(compResult > 0)
         lo = mid + 1;
      else if(compResult < 0)
         hi = mid - 1;
      else
      {  foundMatch = TRUE;
         break;
      }
   }

   /*************************************************************/
   /* If found match, then copy the data into the return record */
   /*************************************************************/
   if(foundMatch)
      memcpy(*record->data, table->dataList[mid], table->dataLength); 
   else
   {  
#ifdef DEVELOP
      fprintf(stderr, "A record for key '%ld' is "
                      "not found in table '%s'\n", 
                       key,
                       record->record->recName);
#endif
      if(table->defaultData)
         memcpy(*record->data, table->defaultData, table->dataLength);

      return(1);
   }

   return(0);
}


long DoShortTableLookup(tTableTemplate *table, short key, tRecData *record)
{
   int  hi, lo, mid=0, compResult, foundMatch;

   /***************************************************/
   /* Find the data in the table that matches the key */
   /***************************************************/
   hi = table->numRows - 1;
   lo = 0;
   foundMatch = FALSE;

   while(lo <= hi)
   {  mid = (hi + lo)/2;
      compResult = key - *((short*)table->dataList[mid]);
      if(compResult > 0)
         lo = mid + 1;
      else if(compResult < 0)
         hi = mid - 1;
      else
      {  foundMatch = TRUE;
         break;
      }
   }

   /*************************************************************/
   /* If found match, then copy the data into the return record */
   /*************************************************************/
   if(foundMatch)
      memcpy(*record->data, table->dataList[mid], table->dataLength); 
   else
   {  
#ifdef DEVELOP
      fprintf(stderr, "A record for key '%d' is "
                      "not found in table '%s'\n", 
                       key,
                       record->record->recName);
#endif
      if(table->defaultData)
         memcpy(*record->data, table->defaultData, table->dataLength);

      return(1);
   }

   return(0);
}


long DoFloatTableLookup(tTableTemplate *table, float key, tRecData *record)
{
   int              hi, lo, mid=0, foundMatch;
   float            compResult;

   /***************************************************/
   /* Find the data in the table that matches the key */
   /***************************************************/
   hi = table->numRows - 1;
   lo = 0;
   foundMatch = FALSE;

   while(lo <= hi)
   {  mid = (hi + lo)/2;
      compResult = key - *((float*)table->dataList[mid]);
      if(compResult > 0)
         lo = mid + 1;
      else if(compResult < 0)
         hi = mid - 1;
      else
      {  foundMatch = TRUE;
         break;
      }
   }

   /*************************************************************/
   /* If found match, then copy the data into the return record */
   /*************************************************************/
   if(foundMatch)
      memcpy(*record->data, table->dataList[mid], table->dataLength); 
   else
   {  
#ifdef DEVELOP
      fprintf(stderr, "A record for key '%f' is "
                      "not found in table '%s'\n", 
                       key,
                       record->record->recName);
#endif
      if(table->defaultData)
         memcpy(*record->data, table->defaultData, table->dataLength);

      return(1);
   }

   return(0);
}


long DoStringTableLookup(tTableTemplate *table, char *key, int keyLen, tRecData *record)
{
   int              hi, lo, mid=0, foundMatch, len;
   int              compResult;

   /***************************************************/
   /* Find the data in the table that matches the key */
   /***************************************************/
   hi = table->numRows - 1;
   lo = 0;
   foundMatch = FALSE;

   len = (keyLen < table->keyLength) ? keyLen : table->keyLength;
   while(lo <= hi)
   {  mid = (hi + lo)/2;
      compResult = memcmp(table->dataList[mid], key, len);
      if(compResult < 0)
         lo = mid + 1;
      else if(compResult > 0)
         hi = mid - 1;
      else if(len<table->keyLength && table->dataList[mid][len]!='\0')
         hi = mid - 1;
      else
      {  foundMatch = TRUE;
         break;
      }
   }
   
   /*************************************************************/
   /* If found match, then copy the data into the return record */
   /*************************************************************/
   if(foundMatch)
      memcpy(*record->data, table->dataList[mid], table->dataLength); 
   else
   {  
#ifdef DEVELOP
      fprintf(stderr, "A record for key '%*.*s' is "
                      "not found in table '%s'\n", 
                       keyLen, keyLen, key,
                       record->record->recName);
#endif
      if(table->defaultData)
         memcpy(*record->data, table->defaultData, table->dataLength);

      return(1);
   }

   return(0);
}


tAtom TableLookup(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tTableTemplate   *table;
   tAtom            retVal;
   tRecData         *inputRecord;


   /************************/
   /* Get the input record */
   /************************/
   inputRecord = (tRecData*)aParams[1].data.uVal;

   /*******************************************************************/
   /* Get the table to be used.  If there is a third argument, assume */
   /* it gives the name of the table.  Otherwise, the table is tied   */
   /* directly to the record type.                                    */
   /*******************************************************************/
   if(numArgs > 2)
   {  tTableTemplate   *tableList =
        (tTableTemplate*)((tFuncInit*)funcCallData->inputData)->dynTableList;

      /**********************************************/
      /* Get the table corresponding to the request */
      /**********************************************/
      table=tableList;
      while(table)
      {  if(!strcmp(aParams[2].data.strVal.str, table->tableName))
            break;
   
         table = table->next;
      }

      if(!table)
      {  ZERO(retVal.data.num)
         sprintf(funcCallData->errStatus.errStr, "The table '%s' does not exist",
                                                         inputRecord->record->recName);
         funcCallData->errStatus.errCode = FAILURE|TABLE_LOOKUP|TABLE_NO_EXIST;
         return(retVal);
      }
   } 
   else if((table = (tTableTemplate*)inputRecord->record->user)==NULL)
   {  ZERO(retVal.data.num)
      sprintf(funcCallData->errStatus.errStr, "The table '%s' does not exist", 
                                                      inputRecord->record->recName);
      funcCallData->errStatus.errCode = FAILURE|TABLE_LOOKUP|TABLE_NO_EXIST;
      return(retVal);
   }


   /**********************************************/
   /* Do the table lookup for the right key type */
   /**********************************************/
   if(table->keyType == HNC_INT32)
   {  if(DoLongTableLookup(table, NUM_GETLNG(aParams[0].data.num), inputRecord)) 
         ZERO(retVal.data.num)
      else 
         ONE(retVal.data.num)
   }
   else if(table->keyType == HNC_FLOAT)
   {  if(DoFloatTableLookup(table, NUM_GETFLT(aParams[0].data.num), inputRecord)) 
         ZERO(retVal.data.num)
      else 
         ONE(retVal.data.num)
   }
   else if(table->keyType == HNC_INT16)
   {  if(DoShortTableLookup(table, NUM_GETSHRT(aParams[0].data.num), inputRecord)) 
         ZERO(retVal.data.num)
      else 
         ONE(retVal.data.num)
   }
   else 
   {  if(DoStringTableLookup(table, aParams[0].data.strVal.str, 
                         aParams[0].data.strVal.len, inputRecord))
         ZERO(retVal.data.num)
      else 
         ONE(retVal.data.num)
   }

   return(retVal);
}


tAtom TableGetDefaults
(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tTableTemplate   *table;
   tAtom            retVal;
   tRecData         *inputRecord;


   /************************/
   /* Get the input record */
   /************************/
   inputRecord = (tRecData*)aParams[0].data.uVal;

   /*******************************************************************/
   /* Get the table to be used.  If there is a third argument, assume */
   /* it gives the name of the table.  Otherwise, the table is tied   */
   /* directly to the record type.                                    */
   /*******************************************************************/
   if(numArgs > 1)
   {  tTableTemplate   *tableList =
        (tTableTemplate*)((tFuncInit*)funcCallData->inputData)->dynTableList;

      /**********************************************/
      /* Get the table corresponding to the request */
      /**********************************************/
      table=tableList;
      while(table)
      {  if(!strcmp(aParams[1].data.strVal.str, table->tableName))
            break;

         table = table->next;
      }

      if(!table)
      {  ZERO(retVal.data.num)
         sprintf(funcCallData->errStatus.errStr, "The table '%*.*s' does not exist",
                                                         aParams[1].data.strVal.len,
                                                         aParams[1].data.strVal.len,
                                                         aParams[1].data.strVal.str);
         funcCallData->errStatus.errCode = FAILURE|TABLE_LOOKUP|TABLE_NO_EXIST;
         return(retVal);
      }
   }
   else if((table = (tTableTemplate*)inputRecord->record->user)==NULL)
   {  ZERO(retVal.data.num)
      sprintf(funcCallData->errStatus.errStr, "The table '%s' does not exist", 
                                                      inputRecord->record->recName);
      funcCallData->errStatus.errCode = FAILURE|TABLE_LOOKUP|TABLE_NO_EXIST;
      return(retVal);
   }

   if(table->defaultData)
   {  memcpy(*inputRecord->data, table->defaultData, table->dataLength);
      ONE(retVal.data.num)
   }
   else
      ZERO(retVal.data.num)
      
   retVal.type = NUMERIC_TYPE;
   return(retVal);
}


long DoTableDirect(tTableTemplate *table, long index, tRecData *record)
{
   /**********************************************/
   /* Make sure index isn't greater than numRows */
   /**********************************************/
   if(index > (table->numRows - 1) || index < 0)
   {  
#ifdef DEVELOP
         fprintf(stderr, "Index '%ld' is out of range for table '%s'\n",
                         index,
                         record->record->recName);
#endif
      if(table->defaultData)
         memcpy(*record->data, table->defaultData, table->dataLength);
      return(0);
   }
   else
   {  memcpy(*record->data, table->dataList[index], table->dataLength); 
      return(1);
   }
}


tAtom TableDirectIndex
(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tTableTemplate   *table;
   long             index=0;
   tAtom            retVal;
   tRecData         *inputRecord;


   /************************/
   /* Get the input record */
   /************************/
   inputRecord = (tRecData*)aParams[1].data.uVal;

   /*******************************************************************/
   /* Get the table to be used.  If there is a third argument, assume */
   /* it gives the name of the table.  Otherwise, the table is tied   */
   /* directly to the record type.                                    */
   /*******************************************************************/
   if(numArgs > 2)
   {  tTableTemplate   *tableList =
        (tTableTemplate*)((tFuncInit*)funcCallData->inputData)->dynTableList;

      /**********************************************/
      /* Get the table corresponding to the request */
      /**********************************************/
      table=tableList;
      while(table)
      {  if(!strcmp(aParams[2].data.strVal.str, table->tableName))
            break;

         table = table->next;
      }

      if(!table)
      {  ZERO(retVal.data.num)
         sprintf(funcCallData->errStatus.errStr, "The table '%s' does not exist",
                                                         inputRecord->record->recName);
         funcCallData->errStatus.errCode = FAILURE|TABLE_LOOKUP|TABLE_NO_EXIST;
         return(retVal);
      }
   }
   else if((table = (tTableTemplate*)inputRecord->record->user)==NULL)
   {  ZERO(retVal.data.num)
      sprintf(funcCallData->errStatus.errStr, "The table '%s' does not exist", 
                                                      inputRecord->record->recName);
      funcCallData->errStatus.errCode = FAILURE|TABLE_LOOKUP|TABLE_NO_EXIST;
      return(retVal);
   }

   /**************************************************/
   /* Can only do a direct index with an integer key */
   /**************************************************/
   index = NUM_GETLNG(aParams[0].data.num);
   if(DoTableDirect(table, index, inputRecord))
      ZERO(retVal.data.num)
   else 
      ONE(retVal.data.num)
      
   return(retVal);
}


void TableRangeLookupCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (4,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (3,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Table_Range_Lookup");
   funcElem->unlimitedParams = 0;
   funcElem->lastOptional = 1;
   funcElem->numParams = 3;
   typeElem[0].type = HNC_STRING;
   typeElem[0].next = typeElem+1;
   typeElem[1].type = NUMERIC_TYPE;
   typeElem[1].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = paramList+1;
   typeElem[2].type = RECORD_TYPE;
   typeElem[2].next = NULL;
   paramList[1].reference = 0;
   paramList[1].list = &typeElem[2];
   paramList[1].next = paramList+2;
   typeElem[3].type = HNC_STRING;
   typeElem[3].next = NULL;
   paramList[2].reference = 0;
   paramList[2].list = &typeElem[3];
   paramList[2].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr=TableRangeLookup;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;
   funcElem->inputData = (void*)funcInit;
   funcElem->next = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}


void TableMultiLookupCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (5,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (3,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Table_Multi_Lookup");
   funcElem->unlimitedParams = 1;
   funcElem->lastOptional = 0;
   funcElem->numParams = 3;
   typeElem[0].type = RECORD_TYPE;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = paramList+1;

   typeElem[1].type = HNC_STRING;
   typeElem[1].next = typeElem+2;
   typeElem[2].type = NUMERIC_TYPE;
   typeElem[2].next = NULL;
   paramList[1].reference = 0;
   paramList[1].list = &typeElem[1];
   paramList[1].next = paramList+2;

   typeElem[3].type = HNC_STRING;
   typeElem[3].next = typeElem+4;
   typeElem[4].type = NUMERIC_TYPE;
   typeElem[4].next = NULL;
   paramList[2].reference = 0;
   paramList[2].list = &typeElem[3];
   paramList[2].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr = TableMultiLookup;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;
   funcElem->inputData = (void*)funcInit;
   funcElem->next = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}


void TableLookupCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (4,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (3,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Table_Lookup");
   funcElem->unlimitedParams = 0;
   funcElem->lastOptional = 1;
   funcElem->numParams = 3;
   typeElem[0].type = HNC_STRING;
   typeElem[0].next = typeElem+1;
   typeElem[1].type = NUMERIC_TYPE;
   typeElem[1].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = paramList+1;
   typeElem[2].type = RECORD_TYPE;
   typeElem[2].next = NULL;
   paramList[1].reference = 0;
   paramList[1].list = &typeElem[2];
   paramList[1].next = paramList+2;
   typeElem[3].type = HNC_STRING;
   typeElem[3].next = NULL;
   paramList[2].reference = 0;
   paramList[2].list = &typeElem[3];
   paramList[2].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr=TableLookup;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;
   funcElem->inputData = (void*)funcInit;
   funcElem->next = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}


void TableDirectIndexCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (3,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (3,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Table_Direct_Index");
   funcElem->unlimitedParams = 0;
   funcElem->lastOptional = 1;
   funcElem->numParams = 3;
   typeElem[0].type = NUMERIC_TYPE;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = paramList+1;
   typeElem[1].type = RECORD_TYPE;
   typeElem[1].next = NULL;
   paramList[1].reference = 0;
   paramList[1].list = &typeElem[1];
   paramList[1].next = paramList+2;
   typeElem[2].type = HNC_STRING;
   typeElem[2].next = NULL;
   paramList[2].reference = 0;
   paramList[2].list = &typeElem[2];
   paramList[2].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr = TableDirectIndex;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;
   funcElem->inputData = (void*)funcInit;
   funcElem->next = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}


void TableGetDefaultsCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (2,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (2,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Table_Get_Defaults");
   funcElem->unlimitedParams = 0;
   funcElem->lastOptional = 1;
   funcElem->numParams = 2;
   typeElem[0].type = RECORD_TYPE;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = paramList+1;
   typeElem[1].type = HNC_STRING;
   typeElem[1].next = NULL;
   paramList[1].reference = 0;
   paramList[1].list = &typeElem[1];
   paramList[1].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr = TableGetDefaults;
   funcElem->SysInitFuncPtr = NULL;
   funcElem->IterInitFuncPtr = NULL;
   funcElem->SysTermFuncPtr = NULL;
   funcElem->IterTermFuncPtr = NULL;
   funcElem->inputData = (void*)funcInit;
   funcElem->next = NULL;

   if (*funcList == NULL)
      *funcList=funcElem;
   else
   {  tempList=*funcList;
      while(tempList->next)
         tempList=tempList->next;
      tempList->next=funcElem;
   }
}


long AddTableToEngineTypes(tTableTemplate *table, tType **typeList)
{
   tType       *prevType, *type, *newType;
   tRecField   *tabFld;
   long        dataLength = table->dataLength;

   /**********************************************/
   /* Look for existing type, or for end of list */
   /**********************************************/
   prevType = NULL;
   for(type=*typeList; type; type=type->next) 
   {  if(!NoCaseStrcmp(type->typeName, table->tableName))
         return(0);
      prevType = type;
   }
 
   /***********************/
   /* Allocate a new type */
   /***********************/
   if((newType = (tType*)MyMalloc(sizeof(tType)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)newType, 0, sizeof(tType));

   if((newType->typeName = (char*)MyMalloc(strlen(table->tableName)+1))==NULL)
      throw(MALLOC_EXCEPTION);
   strcpy(newType->typeName, table->tableName);

   newType->type = RECORD_TYPE;
   newType->fmt.record.recName = newType->typeName;
   newType->fmt.record.user = (void*)table;

   /**************************************************************/
   /* Set the type flag that indicates that this is a table type */
   /**************************************************************/
   newType->flags |= TYPE_TABLE;

   /**************************************************************/
   /* Set a flag to indicate if the type is shared across tables */
   /**************************************************************/
   if(table->typeName)
      newType->flags |= TYPE_SHARED;

   /**************************/
   /* Link newType into list */
   /**************************/
   if(prevType) prevType->next = newType;
   else *typeList = newType;

   /**********************************************/
   /* Set the field list and get the data length */
   /**********************************************/
   newType->fmt.record.fieldList = table->fieldList;
   for(tabFld=table->fieldList; tabFld; tabFld=tabFld->next)
   {  
      /* If field is a record, table field is sizeof table pointer, but */
      /* the record field should be the length of the sub-record.       */
      if(tabFld->type == RECORD_TYPE) 
      {  /* Subtract table field length */
         dataLength -= sizeof(tTableTemplate*);

         /* Add in record length */
         dataLength += tabFld->typeFmt->record.dataSize;
      }

      /* Increment the field count */
      newType->fmt.record.numFlds++;
   }
   
   /* Set the final record length */
   newType->fmt.record.dataSize = dataLength;
   return(0);
}


long CreateTableFromType(tTableTemplate *table, tEngRecord *record, tTableTemplate *tblList)
{
   tRecField  *recFld;
   long       dataLength = record->dataSize;

   /*******************************************************/
   /* Go through the fields to get the data length, which */
   /* is different for the table if there is a sub-record */
   /*******************************************************/
   table->fieldList = record->fieldList;
   for(recFld=record->fieldList; recFld; recFld=recFld->next)
   {  
      /* If the field is a sub-record, then the table field is only   */
      /* the sizeof a table pointer.  Also, only allow one sub-record */
      /* per table, that being the last field in the record           */
      if(recFld->type == RECORD_TYPE && recFld->next == NULL)
      {  /* Subtract length of the record */
         dataLength -= recFld->typeFmt->record.dataSize;

         /* Add the length of the table pointer */
         dataLength += sizeof(tTableTemplate*);
      }
      else if(recFld->type == RECORD_TYPE)
         return(1);

      /* Assume first field is the key */
      if(recFld == table->fieldList)
      {  table->keyType = recFld->type;
         table->keyLength = recFld->length;
      }
   }

   /***************************************************/
   /* If the table has the same name as the type, set */
   /* the table in the user field of the type.  Else  */
   /* set the table list in the user field.           */
   /***************************************************/
   if(!record->user)
   {  if(!NoCaseStrcmp(table->tableName, record->recName))
         record->user = (void*)table;
      else
         record->user = (void*)tblList;
   }

   /* Set other table data */
   table->fieldCount = record->numFlds;
   table->dataLength = dataLength;
   table->typeName = record->recName;

   return(0);
}
