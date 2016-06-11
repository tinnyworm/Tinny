/*
** $Id: runct.c,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: runct.c,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.6  1997/01/16  22:44:06  can
 * Lots of changes for 2.1 release
 *
 * Revision 2.3  1996/05/01  01:19:04  can
 * Fixed SetData function
 *
 * Revision 1.25  1996/01/09  01:04:42  can
 * Fixed bug where string setting was incorrectly placing the
 * null character.
 *
 * Revision 1.24  1995/12/14  17:50:20  can
 * Fixed bug in EvalExpression where NOOP arguments were not processed
 * properly.  Also, put NOT and NEGATE work back into VAL_ARG macro.
 * I had taken it out to make it faster, but it caused problems (found
 * when trying to NOT an IN statement).
 *
 * Revision 1.21  1995/11/03  23:57:26  can
 * Copied exact type for string parameters, instead of blindly setting to
 * HNC_STRING.
 *
 * Revision 1.20  1995/10/26  19:24:03  can
 * Modified so that string parameter don't copy the data area, but rather
 * just get the pointer and data size.
 *
 * Revision 1.19  1995/10/18  00:24:57  can
 * Added EVAL_EXPRESSION back into the CalcNode switch.
 *
 * Revision 1.18  1995/09/22  17:21:08  can
 * More optimizations.  Replaced atof with StringToFloat.  Made separate
 * function for EvalExpression.
 *
 * Revision 1.17  1995/09/14  17:44:31  can
 * Optimized in-place functions to only evaluate the left
 * argument if it is a parameter
 *
 * Revision 1.16  1995/09/13  02:19:29  can
 * Fixed bug in CALC_IT where pointer was being changed instead of
 * the data being copied (old bug... how did it get back in!?!)
 *
 * Revision 1.15  1995/09/06  23:55:22  can
 * Make EVAL_ARG a smaller macro and an inline function, EvalArg.
 * It seems to be faster this way.  Also, fix a couple of bugs.
 *
 * Revision 1.14  1995/08/11  18:26:01  can
 * Not malloc'ing errStr here anymore.  Also, fixed bug that was not
 * passing correct error structure to functions.
 *
 * Revision 1.13  1995/08/10  00:01:27  can
 * Change to work with new string free'ing scheme.  That is, all
 * strings that may not be freed are labelling ATOM_ADDRESS, which
 * was screwing up the EVAL_ARG.
 *
 * Revision 1.12  1995/08/08  23:54:18  can
 * Added special NOOP check.  Also, fixed bug that was assuming all
 * parameters are floats in EVAL_ARG
 *
 * Revision 1.11  1995/08/02  00:10:30  can
 * Added loops to execute iteration init rules and iteration
 * terminate rules.
 *
 * Revision 1.10  1995/07/31  17:10:13  can
 * Fixed bugs in string variable work.
 *
 * Revision 1.9  1995/07/28  00:15:46  can
 * Added ruleBaseName to ruleBaseData, so access to it comes from
 * ruleBaseData instead of calcTab
 *
 * Revision 1.8  1995/07/27  00:16:51  can
 * Took out try block around main CALC_IT to speed things up
 * Also, removed ClearCalcDones function.  This will be done
 * differently.
 *
 * Revision 1.7  1995/07/26  21:35:01  can
 * Modified expression calculation scheme to reduce recursion.
 *
 * Revision 1.5  1995/07/21  15:35:44  can
 * Added support for variables and parameters
 *
 * Revision 1.4  1995/07/14  18:05:38  wle
 * Made sure all calc nodes get their calc done flags set to FALSE
 * at the start of the iteration.
 *
 * Revision 1.3  1995/07/13  23:25:15  can
 * Updated to make function calls work.  Still need to add
 * exception handling
 *
 * Revision 1.2  1995/07/09  23:07:20  can
 * Fixed some bugs and updated the error handling
 *
 * Revision 1.1  1995/07/03  21:18:26  wle
 * Initial revision
 *
*/


/* System include files */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

/* HNC rules engine include files */
#include "mymall.h"
#include "exptree.h"
#include "engerr.h"
#include "calctab.h"
#include "bltins.h"
#include "typechk.h"
#include "runct.h"

/* HNC common include files */
#include "except.h"


static long  ResolveFixedArray(tArrayData *array, char **data, long length, tREStatus *status);

static FILE *gTraceFile;
static long gLevel;



/*--------------------------------------------------------------------------*/
/* SetTemplateRecordData()                                                  */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long SetTemplateRecordData(char *data, tDataTemplate *tmplt, void *handle)
{
   char         buf[512];
   tRecInstance *tmpltRec;
   tCalcTable   *calcTab = (tCalcTable*)handle;

   /* Find the record data structure for the template */
   for(tmpltRec=calcTab->tmpltRecList; tmpltRec; tmpltRec=tmpltRec->next)
   {  sprintf(buf, "%s_TYPE", tmplt->templateName); 
      if(tmpltRec->data)
      {  if(!NoCaseStrcmp(buf, tmpltRec->data->record->recName))
            break;
      }
   }

   /* If no record data structure exists (should never happen!!), error out */
   if(!tmpltRec)
   {  sprintf(calcTab->ruleBaseData->status.errStr, 
                              "Template '%s' does not have a record "
                              "data structure", tmplt->templateName); 
      return(FATAL_ENGINE|RESOLVE_TMPLT|NO_REC_DATA);
   }

   /* Set the record data */
   *tmpltRec->data->data = data;

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* ResolveTemplateRecord()                                                  */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long ResolveTemplateRecord(char **data, tDataTemplate *tmplt, void *handle)
{
   char         buf[512];
   tRecInstance *tmpltRec;
   tCalcTable   *calcTab = (tCalcTable*)handle;
   long         retStat;

   /* Find the record data structure for the template */
   for(tmpltRec=calcTab->tmpltRecList; tmpltRec; tmpltRec=tmpltRec->next)
   {  sprintf(buf, "%s_TYPE", tmplt->templateName); 
      if(!NoCaseStrcmp(buf, tmpltRec->data->record->recName))
         break;
   }

   /* If no record data structure exists (should never happen!!), error out */
   if(!tmpltRec)
   {  sprintf(calcTab->ruleBaseData->status.errStr, 
                              "Template '%s' does not have a record "
                              "data structure", tmplt->templateName); 
      return(FATAL_ENGINE|RESOLVE_TMPLT|NO_REC_DATA);
   }

   /* Resolve the record data */
   if(tmplt->dataSize)
   {  if((retStat=ResolveRecordAllLen(tmpltRec->data, data, tmplt->dataSize, 
                                 &calcTab->ruleBaseData->status, calcTab->dynPool)))
      {  calcTab->ruleBaseData->status.errCode = retStat; 
         return(retStat);
      }
   }
   else
   {  if((retStat=ResolveRecordNoLen(tmpltRec->data, data,
                                 &calcTab->ruleBaseData->status, calcTab->dynPool)))
      {  calcTab->ruleBaseData->status.errCode = retStat; 
         return(retStat);
      }
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* ResolveFixedArray()                                                      */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long 
ResolveFixedArray(tArrayData *array, char **data, long length, tREStatus *status)
{
   long a, retStat;

   for(a=0; a<array->array->numElems; a++)
   {  if(array->array->type == RECORD_TYPE)
      {  if((retStat=ResolveRecordAllLen(((tRecData*)array->data[a]), 
                              data, array->array->elemSize, status, NULL)))
            return(retStat);
      }
      else
      {  array->data[a] = (char*)*data; 
         *data += array->array->elemSize;
      }
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* StrToLen()                                                               */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long StrToLen(char *data, long len)
{
   long i, value=0;

   for(i=0; i<len; i++)
      value = (value*10) + data[i] - '0';

   return(value);
}


/*--------------------------------------------------------------------------*/
/* ResetDynmaicPool()                                                       */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long ResetDynamicPool(void *handle)
{
   tCalcTable *calcTab = (tCalcTable*)handle;

   if(calcTab->dynPool)
   {  if(calcTab->dynPool->mainCalcTab == calcTab)
      {  calcTab->dynPool->curPage = calcTab->dynPool->pageList;
         calcTab->dynPool->offset = 0;
      }
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* GetPoolData()                                                            */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long
GetPoolData(tDynPool *dynPool, char **poolData, long size, tREStatus *status)
{
   /* Make sure data will fit on a single page */
   if(size > POOL_PAGE_SIZE)
   {  sprintf(status->errStr, "Size '%ld' is too large to be processed dynamically."
                              "Maximum size is '%ld'", size, POOL_PAGE_SIZE);
      return(FATAL_ENGINE|GET_POOL_DATA|SIZE_TOO_BIG);
   }

   /* Make sure data will fit into current page */
   if(dynPool->offset + size > POOL_PAGE_SIZE)  
   {  /* Go to next page, if any, or allocate one now */ 
      if(dynPool->curPage->next)
         dynPool->curPage = dynPool->curPage->next;
      else
      {   if((dynPool->curPage->next = (tPoolPage*)MyMalloc(POOL_PAGE_SIZE))==NULL)
          {  sprintf(status->errStr, "Cannot malloc new dynamic memory page");
             return(FATAL_ENGINE|GET_POOL_DATA|BAD_MALLOC);
          }
          dynPool->curPage = dynPool->curPage->next;
      }
      dynPool->offset = 0;
   }

   /* Set the pointer and increment the offset */ 
   *poolData = dynPool->curPage->data + dynPool->offset;
   dynPool->offset += size;

   return(OK); 
}


/*--------------------------------------------------------------------------*/
/* GetRecordPointer()                                                       */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long
GetRecordPointer(tDynPool *dynPool, tRecData **pRecData, tEngRecord *record, tREStatus *status)
{
   char       *poolData;
   tRecData   *recData, *subRec;
   tRecField  *recFld;
   tFldData   *fld;
   tArrayData *subArray;
   long       retStat, f;

   /* Get the tRecData structure */
   if((retStat = GetPoolData(dynPool, &poolData, sizeof(tRecData), status))) 
      return(retStat);
   *pRecData = recData = (tRecData*)poolData;
   recData->record = record;

   /* Get the char **data pointer for the record */
   if((retStat = GetPoolData(dynPool, &poolData, sizeof(char**), status))) 
      return(retStat);
   recData->data = (char**)poolData;

   /* Get the field data structures for the record */
   if((retStat = GetPoolData(dynPool, &poolData, record->numFlds*sizeof(tFldData), status))) 
      return(retStat);
   recData->flds = (tFldData*)poolData;
   
   /* Loop through the fields setting the field data pointers */
   for(f=0, recFld=record->fieldList; f<record->numFlds; f++, recFld=recFld->next)
   {  fld = recData->flds+f;
      fld->fld = recFld;

      if(fld->fld->type==RECORD_TYPE)
      {  if((retStat = GetRecordPointer(dynPool, &subRec, &recFld->typeFmt->record, status)))
            return(retStat);
         fld->data = (char*)subRec;
      }
      else if(fld->fld->type==ARRAY_TYPE)
      {  if((retStat = GetPoolData(dynPool, &poolData, sizeof(tArrayData), status))) 
            return(retStat);
         subArray = (tArrayData*)poolData;
         subArray->array = &recFld->typeFmt->array;
         fld->data = (char*)subArray;

         /* If is a fixed array, need data pointers */
         if(!fld->fld->nonFixed->fldID && !fld->fld->nonFixed->lengthInd)
         {  if((retStat = GetPoolData(dynPool, &poolData,
                            sizeof(char*)*recFld->typeFmt->array.numElems, status)))
               return(retStat);
            subArray->data = (char**)poolData;
            subArray->endIndex = recFld->typeFmt->array.endIndex;
         }
         else
            subArray->endIndex = 0;
      }
      else
      {  if((retStat = GetPoolData(dynPool, &poolData, sizeof(tFlatData), status))) 
            return(retStat);
         fld->data = poolData;
      }
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* GetArrayElementMemory()                                                  */
/*                                                                          */
/*--------------------------------------------------------------------------*/
#define EXTRA_ELEMENTS  20
static long
GetArrayElementMemory
(tDynPool *dynPool, tArrayData *arrayData, long element, tREStatus *status)
{
   long       retStat=OK, retVal=OK, i;
   char       *poolData, **newData;
   tRecData   *recData;
  
   /* Get pointers for maxElements if it hasn't been done already */
   if(!arrayData->data)
   {  retStat = GetPoolData(dynPool, &poolData, 
              arrayData->array->numElems*sizeof(char*), status);
      if(retStat && notWarning(retStat)) return(retStat);
      else retVal = retStat;
  
      arrayData->data = (char**)poolData;
      arrayData->nonFixed = 1;
   }

   /* Make sure element is less than the maximum specified for the array */
   if(element > arrayData->array->numElems)
   {  /* Hit max elements.  Put out a warning */
      sprintf(status->errStr, "Warning: maximum elements, %ld, for array '%s' has "
                              "been reached.  Adding space for extra data.",
                              arrayData->array->numElems,
                              arrayData->array->name);
      retVal = (WARN_ENGINE|RESOLVE_REC|MAX_ELEM_REACHED);

      /* Get maxElements + EXTRA_ELEMENTS pointers */
      retStat = GetPoolData(dynPool, &poolData, 
                arrayData->array->numElems+EXTRA_ELEMENTS*sizeof(char*), status);
      if(retStat && notWarning(retStat)) return(retStat);
      else retVal = retStat;

      /* Copy the data to the new element array */
      newData = (char**)poolData;
      for(i=0; i<arrayData->array->numElems; i++)
         newData[i] = arrayData->data[i]; 

      arrayData->array->numElems += EXTRA_ELEMENTS;
      arrayData->data = newData;
   }

   /* Get the memeory for the data at the given array element */
   if(arrayData->array->type == RECORD_TYPE)
   {  /* Get the tRecData structure */
      retStat = GetRecordPointer(dynPool, &recData, 
                         &arrayData->array->typeFmt->record, status);
      if(retStat && notWarning(retStat)) return(retStat);
      else retVal = retStat;

      arrayData->data[element] = (char*)recData;
   }
   else
   {  /* Get a tFlatData structure */
      retStat = GetPoolData(dynPool, &poolData, sizeof(tFlatData), status);
      if(retStat && notWarning(retStat)) return(retStat);
      else retVal = retStat;

      arrayData->data[element] = poolData;
   }

   return(retVal); 
}


/*--------------------------------------------------------------------------*/
/* SetRecordFieldsNotHit()                                                  */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long SetRecordFieldsNotHit(tRecData *rec)
{
   tFldData *fld;
   long     f;

   for(f=0; f<rec->record->numFlds; f++) 
   {  fld = rec->flds+f;
      fld->flags |= FIELD_NOT_HIT;

      if(fld->data == NULL)
         continue;
      else if(fld->fld->type == RECORD_TYPE)
         SetRecordFieldsNotHit((tRecData*)fld->data);
      else if(fld->fld->type == ARRAY_TYPE)
      {  ((tArrayData*)fld->data)->endIndex = 0;
         ((tArrayData*)fld->data)->data = NULL;
      }
      else if(IsString(fld->fld->type))
      {  ((tFlatData*)fld->data)->data = "";
         ((tFlatData*)fld->data)->length = 0;
      }
      else
         ((tFlatData*)fld->data)->data = NULL;
   }

   return(0);
}


/*--------------------------------------------------------------------------*/
/* ResolveFixedPortion()                                                    */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long ResolveFixedPortion
(tRecData *rec, char **data, long recLen, 
   long *nextFld, tREStatus *status, tDynPool *dynPool)
{
   long       f, remain, useOffset=1, atLength=0;
   long       len, retStat=OK, retVal=OK, adjusted=0;
   tFldData   *fld;
   char       *startAddr = *data;

   /****************************************************************/
   /* Loop through the record to get all the fixed fields resolved */
   /****************************************************************/
   remain = recLen;
   for(f=0; f<rec->record->numFlds; f++)
   {  fld = rec->flds+f;
      fld->flags = 0;
      adjusted=0;
      if(!fld->fld->nonFixed->fldID && !fld->fld->nonFixed->lengthInd)
      {  if(fld->fld->type == RECORD_TYPE)
         {  retStat = ResolveRecordAllLen((tRecData*)fld->data, data, 
                                              fld->fld->length, status, dynPool);
            if(retStat && notWarning(retStat)) return(retStat);
            else retVal = retStat;

            len = fld->fld->length;
            adjusted=1;
         }
         else if(fld->fld->type == ARRAY_TYPE)
         {  retStat = ResolveFixedArray((tArrayData*)fld->data, data, 
                                                  fld->fld->length, status);
            if(retStat && notWarning(retStat)) return(retStat);
            else retVal = retStat;

            len = fld->fld->length;
            adjusted=1;
         }
         else
         {  if(useOffset) 
            {  ((tFlatData*)fld->data)->data = startAddr + fld->fld->offset;
               if(fld->fld->length)
               {  len = fld->fld->length + (startAddr + fld->fld->offset - *data); 
                  ((tFlatData*)fld->data)->length = fld->fld->length;
               }
               else
                  ((tFlatData*)fld->data)->length = len = remain;
            }
            else 
            {  ((tFlatData*)fld->data)->data = *data;
               if(fld->fld->length) 
                  ((tFlatData*)fld->data)->length = len = fld->fld->length;
               else 
                  ((tFlatData*)fld->data)->length = len = remain;
            }
         }
      }
      else if(!fld->fld->nonFixed->fldID && fld->fld->nonFixed->lengthInd)
      {  /* Get length from lengthInd */
         len = StrToLen(*data, fld->fld->nonFixed->lengthInd);
         remain -= fld->fld->nonFixed->lengthInd;
         *data += fld->fld->nonFixed->lengthInd;

         if(fld->fld->type == RECORD_TYPE)
         {  retStat = ResolveRecordAllLen((tRecData*)fld->data, data, len, status, dynPool);
            if(retStat && notWarning(retStat)) return(retStat);
            else retVal = retStat;

           adjusted=1;
         }
         else
         {  ((tFlatData*)fld->data)->data = *data;
            ((tFlatData*)fld->data)->length = len;

            /* Can't use field offsets anymore */
            useOffset = 0;
         }
      }
      else 
      {  *nextFld = f;
         break;
      }

      if(recLen)
      {  remain -= len;
         *nextFld = f+1;
         if(remain > 0)
         {  if(!adjusted) 
               *data += len;
         }
         else if(remain == 0)
         {  if(!adjusted)
               *data += len;
            break;
         }
         else
         {  sprintf(status->errStr, "Length %ld is not correct for record '%s' "
                                    "because field '%s' extends over that length",
                                    recLen,
                                    rec->record->recName, 
                                    fld->fld->fieldName);
            return(FATAL_ENGINE|RESOLVE_REC|BAD_LENGTH);
         }
      }
      else 
      {  if(!adjusted)
            *data += len;
         *nextFld = f+1;
      }
   }


   /****************************************************************/
   /* If at length, and a length was specified, check for missing  */
   /* required fields and return                                   */
   /****************************************************************/
   if(atLength && recLen)
   {  for(; f<rec->record->numFlds; f++)
      {  fld=rec->flds+f;
   
         /* Check for more fixed data */
         if(!fld->fld->nonFixed->fldID)
         {  sprintf(status->errStr, "Length %ld is not correct for record %s "
                                    "because length hit while fixed data still remains",
                                       recLen,
                                       rec->record->recName);
            return(FATAL_ENGINE|RESOLVE_REC|BAD_LENGTH);
         }
         else if(fld->fld->nonFixed->required)
         {  sprintf(status->errStr, "Length %ld is not correct for record %s "
                                    "because required field %s not found",
                                    recLen, rec->record->recName, fld->fld->fieldName);
            return(FATAL_ENGINE|RESOLVE_REC|BAD_LENGTH);
         }
      }
   }

   return(retVal);
}


/*--------------------------------------------------------------------------*/
/* ResolveNonFixedPortion()                                                 */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long ResolveNonFixedPortion
(tRecData *rec, char **data, char *startAddr, long recLen, long remain, 
   long nextFld, tREStatus *status, tDynPool *dynPool)
{
   long       f, index;
   long       atLength=0, len=0, retStat=OK, retVal=OK, found=0, adjusted=0;
   tFldData   *fld=NULL;
   tRecField  *lastFldHit=NULL;
   char       *lastAddr = *data;
   tArrayData *arrayData;
   tRecData   *recData;
   tFlatData  *flatData;

   
   /********************************************************************************/
   /* Keep looping while there is data remaining, or until an unknown field is hit */
   /********************************************************************************/
   while(1)
   {  for(f=nextFld; f<rec->record->numFlds; f++)
      {  fld=rec->flds+f;
         found=0;
         if(!memcmp(fld->fld->nonFixed->fldID, *data, fld->fld->nonFixed->IDLen))
         {  found=1;
            lastFldHit = fld->fld;
            adjusted = 0;

            /* See if multiple fields found without array definition */
            if(fld->fld->type != ARRAY_TYPE  &&
                !(fld->flags&FIELD_NOT_HIT) &&
                !(fld->fld->nonFixed->skip))
            {  sprintf(status->errStr, "Field '%s' in record '%s' is not an array, but "
                                       "it has a second occurence at offset %ld",
                                       fld->fld->fieldName,
                                       rec->record->recName, 
                                       (long)(*data - startAddr));
               return(FATAL_ENGINE|RESOLVE_REC|BAD_LENGTH);
            }
            fld->flags &= (0xFFFFFFFF - FIELD_NOT_HIT);

            /* Get length from lengthInd, or from field length */
            if(fld->fld->nonFixed->lengthInd)
            {  len = StrToLen(*data+fld->fld->nonFixed->IDLen, 
                                 fld->fld->nonFixed->lengthInd);
            
               *data += fld->fld->nonFixed->IDLen + fld->fld->nonFixed->lengthInd; 
               if(fld->fld->type == RECORD_TYPE) 
               {  len -= fld->fld->nonFixed->IDLen + fld->fld->nonFixed->lengthInd; 
                  if(recLen) remain -= 
                         fld->fld->nonFixed->IDLen + fld->fld->nonFixed->lengthInd; 
               }
               else if(fld->fld->type == ARRAY_TYPE)
               {  if(fld->fld->typeFmt->array.type == RECORD_TYPE)
                  {  len -= fld->fld->nonFixed->IDLen + fld->fld->nonFixed->lengthInd; 
                     if(recLen) 
                        remain -= fld->fld->nonFixed->IDLen + fld->fld->nonFixed->lengthInd; 
                  }
               }
            }
            else
            {  len = fld->fld->length - fld->fld->nonFixed->IDLen; 
               if(recLen) remain -= fld->fld->nonFixed->IDLen; 
               *data += fld->fld->nonFixed->IDLen; 
            }

            /* If fld is to be skipped, then go on to next */
            if(fld->fld->nonFixed->skip)
            {  if(recLen)
               {  remain -= len;
                  if(remain > 0)
                     *data += len;
                  else if(remain == 0)
                  {  *data += len;
                     atLength = 1;
                     break;
                  }
                  else
                  {  sprintf(status->errStr, "Length %ld is not correct for record '%s' "
                                             "because field '%s' extends over that length",
                                             recLen,
                                             rec->record->recName, 
                                             fld->fld->fieldName);
                     return(FATAL_ENGINE|RESOLVE_REC|BAD_LENGTH);
                  }
               }
               else 
                  *data += len;


               adjusted = 1;
               break;
            }

            /* If field is an array, set the array element */
            if(fld->fld->type == ARRAY_TYPE)
            {  arrayData = (tArrayData*)fld->data;

               retStat = GetArrayElementMemory(dynPool, arrayData, 
                                             arrayData->endIndex, status);
               if(retStat && notWarning(retStat)) return(retStat);
               else retVal = retStat;

               index = arrayData->endIndex;
               arrayData->endIndex++;

               /* If record, resolve record */
               if(fld->fld->typeFmt->array.type == RECORD_TYPE)
               {  recData = (tRecData*)arrayData->data[index];
                  lastAddr = *data;

                  retStat = ResolveRecordNoLen(recData, data, status, dynPool);
                  if(retStat && notWarning(retStat)) return(retStat);
                  else retVal = retStat;

                  adjusted = 1;            
                  len = (long)(*data - lastAddr);
               }
               else
               {  flatData = (tFlatData*)arrayData->data[index];
                  flatData->data = (char*)*data;
                  flatData->length = len;
               }
            }
            else
            {  /* If record, resolve record */
               if(fld->fld->type == RECORD_TYPE)
               {  recData = (tRecData*)fld->data;
                  lastAddr = *data;

                  retStat = ResolveRecordNoLen(recData, data, status, dynPool);
                  if(retStat && notWarning(retStat)) return(retStat);
                  else retVal = retStat;

                  adjusted = 1;
                  len = (long)(*data - lastAddr);
               }
               else
               {  ((tFlatData*)fld->data)->data = (char*)*data;
                  ((tFlatData*)fld->data)->length = len;
               }
            }

            break; 
         }
      }

      /***********************************************************************/
      /* If current data position not a valid field and the length is given  */
      /* then there is any error.  Otherwise, just assume the record is done */
      /* and break here.                                                     */
      /***********************************************************************/
      if(!found)
      {  if(recLen)
         {  if(!lastFldHit)
            {   sprintf(status->errStr, "Unknown field ID for record %s, "
                                        "at buffer offset %ld", 
                                           rec->record->recName,
                                           (long)(*data - startAddr));
            }
            else
            {   sprintf(status->errStr, "Unknown field ID for record %s, "
                                        "after field '%s' at buffer offset %ld", 
                                          rec->record->recName,
                                          lastFldHit->fieldName, (long)(*data - startAddr));
            }
            return(FATAL_ENGINE|RESOLVE_REC|UNKNOWN_FLD);
         }
         else
            break;
      }
     
      /* Incremenet the data pointer, if it hasn't been done already and if the */
      /* record length, if any, hasn't been reached.                            */
      if(recLen)
      {  if(atLength) break;
         else
         {  remain -= len;
            if(remain > 0)
            {  if(!adjusted) 
                  *data += len;
            }
            else if(remain == 0)
            {  if(!adjusted) 
                  *data += len;
               atLength = 1;
               break;
            }
            else
            {  sprintf(status->errStr, "Length %ld is not correct for record '%s' "
                                       "because field '%s' extends over that length",
                                       recLen,
                                       rec->record->recName, 
                                       fld->fld->fieldName);
               return(FATAL_ENGINE|RESOLVE_REC|BAD_LENGTH);
            }
         }
      }
      else
      {  if(!adjusted) 
            *data += len;
      }
   }

   /**************************************************/
   /* Loop through all non fixed fields to make sure */
   /* all required fields were set.                  */
   /**************************************************/
   for(f=nextFld; f<rec->record->numFlds; f++)
   {  fld=rec->flds+f;
      if(fld->fld->nonFixed->required && fld->flags&FIELD_NOT_HIT)
      {  sprintf(status->errStr, "Field '%s' in record '%s' is required but "
                                 "it wasn't found.",
                                    fld->fld->fieldName,
                                    rec->record->recName);
         return(FATAL_ENGINE|RESOLVE_REC|NO_REQ_FLD);
      }
      else if(fld->flags&FIELD_NOT_HIT)
      {  /* Make sure (recursively) all fields in the record are set to FIELD_NOT_HIT */
         if(fld->data == NULL)
            continue;
         else if(fld->fld->type == RECORD_TYPE)
            SetRecordFieldsNotHit((tRecData*)fld->data);
         else if(fld->fld->type != ARRAY_TYPE)
         {  if(IsString(fld->fld->type))
            {  ((tFlatData*)fld->data)->data = "";
               ((tFlatData*)fld->data)->length = 0;
            }
            else
               ((tFlatData*)fld->data)->data = NULL;
         }
      }
   }

   return(retVal);
}


/*--------------------------------------------------------------------------*/
/* ResolveRecordAllLen()                                                    */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long ResolveRecordAllLen
(tRecData *rec, char **data, long recLen, tREStatus *status, tDynPool *dynPool)
{
   long       f, remain, nextFld;
   long       atLength=0, retStat=OK, retVal=OK;
   tFldData   *fld;
   char       *startAddr = *data;

   /*****************************************/
   /* Set the data in the recData structure */
   /*****************************************/
   *rec->data = *data;

   /*******************************************/
   /* Resolve the fixed portion of the record */
   /*******************************************/
   retStat = ResolveFixedPortion(rec, data, recLen, &nextFld, status, dynPool);
   if(retStat && notWarning(retStat)) return(retStat);
   else retVal = retStat;
   
   if((remain = (startAddr+recLen) - *data)==0)
      atLength = 1;

   
   /****************************************************************/
   /* Resolve the remainder of the non-fixed portion of the record */
   /* if there are any fields and data remaining.                  */
   /****************************************************************/
   if(nextFld < rec->record->numFlds && !atLength)
   {  /* Reset all the array types, if any */
      for(f=nextFld; f<rec->record->numFlds; f++)
      {  fld=rec->flds+f;
         fld->flags |= FIELD_NOT_HIT;
         if(fld->fld->type==ARRAY_TYPE) 
         {  ((tArrayData*)fld->data)->endIndex = 0;
            ((tArrayData*)fld->data)->data = NULL;
         }
      }
      
      retStat = ResolveNonFixedPortion(rec, data, startAddr, recLen, 
                                        remain, nextFld, status, dynPool);
      if(retStat && notWarning(retStat)) return(retStat); 
      else retVal = retStat;
   }

   return(retVal);
}


/*--------------------------------------------------------------------------*/
/* ResolveRecordNoLen()                                                     */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long ResolveRecordNoLen
(tRecData *rec, char **data, tREStatus *status, tDynPool *dynPool)
{
   long       f, nextFld=0;
   long       retStat=OK, retVal=OK;
   tFldData   *fld;
   char       *startAddr = *data;

   /*****************************************/
   /* Set the data in the recData structure */
   /*****************************************/
   *rec->data = *data;

   /*******************************************/
   /* Resolve the fixed portion of the record */
   /*******************************************/
   retStat = ResolveFixedPortion(rec, data, 0, &nextFld, status, dynPool);
   if(retStat && notWarning(retStat)) return(retStat);
   else retVal = retStat;
   
   /***********************************************************************/
   /* Continue resolving data if there is some length or fields remaining */
   /***********************************************************************/
   if(nextFld < rec->record->numFlds)
   {  for(f=nextFld; f<rec->record->numFlds; f++)
      {  fld=rec->flds+f;
         fld->flags |= FIELD_NOT_HIT;
         if(fld->fld->type==ARRAY_TYPE) 
         {  ((tArrayData*)fld->data)->endIndex = 0;
            ((tArrayData*)fld->data)->data = NULL;
         }
      }
         
      retStat = ResolveNonFixedPortion(rec, data, startAddr, 0, 0, nextFld,  status, dynPool);
      if(retStat && notWarning(retStat)) return(retStat);
      else retVal = retStat;
   }

   return(retVal);
}


/*--------------------------------------------------------------------------*/
/* GetMemory()                                                              */
/*                                                                          */
/*--------------------------------------------------------------------------*/
char* GetMemory(tCalcTable *calcTab, int size, tREStatus *status)
{
   /***************************************************************/
   /* For now, will assume that all run-time memory needs are met */
   /* with the MEM_POOL_SIZE.  If it is exceeed, it will fail     */
   /***************************************************************/
   if(calcTab->memPool->index+size > MEM_POOL_SIZE)
   {  status->errCode = FATAL_ENGINE|GET_MEMORY|MEM_POOL_EXCEEDED;
      sprintf(status->errStr, "Ran out of memory in run-time pool.  "
                              "Currently have %ld bytes.  "
                              "Failed for rule '%s' in rule base '%s'", 
                                                MEM_POOL_SIZE,
                                                calcTab->currentRuleCalc->id, 
                                                calcTab->ruleBaseData->ruleBaseName);
      throw(ENGINE_EXCEPTION);
   }

   calcTab->memPool->index += size;
   return(calcTab->memPool->data + calcTab->memPool->index-size);
}


/*--------------------------------------------------------------------------*/
/* CalcRuleStatements()                                                     */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void CalcRuleStatements(tRuleCalc *ruleCalc, tREStatus *status, tCalcTable *calcTab)
{ 
   tAtom   eval;
   tArg    *stmtArg = ruleCalc->stmtBlock->argList;
   
   /* Loop through all the statement calcs in the statement list */
   while(stmtArg && !DONE(status->errCode))
   {  stmtArg->func->EvalFunc(&eval, stmtArg, status, calcTab);
      stmtArg = stmtArg->next;
   }
}


/*--------------------------------------------------------------------------*/
/* RunCalcTable()                                                           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long RunCalcTable(tCalcTable *calcTab)
{
   tRuleCalc    *ruleCalc;
   tMemPool     mp, *memPool = &mp;

   /******************/
   /* Set up globals */
   /******************/
   gTraceFile = calcTab->ruleBaseData->traceFile;
   gLevel = 0;

   /***************************************/
   /* Initialize the run-time memory pool */  
   /***************************************/
   memPool->index = 0;
   calcTab->memPool = memPool;
   
   /*******************************************************/
   /* Set the calcDone flags from all calc nodes to FALSE */ 
   /* to make sure that they are all calculated.          */
   /*******************************************************/
   if(!calcTab->noAutoResetShared && calcTab->numCalcDones)
      memset((char*)calcTab->calcDones, 0, calcTab->numCalcDones);


   /**********************/
   /* Start status as OK */
   /**********************/
   ZERO(calcTab->retValue.data.num)
   calcTab->ruleBaseData->status.errCode = OK;


   /*************************************************************/
   /* Loop through the iterInitList and evaluate the calc nodes */
   /*************************************************************/
   for(ruleCalc=calcTab->iterInitList; ruleCalc; ruleCalc=ruleCalc->next)
   {  calcTab->ruleBaseData->status.errCode = OK;
      calcTab->currentRuleCalc = ruleCalc;
      CalcRuleStatements(ruleCalc, &calcTab->ruleBaseData->status, calcTab);

      if(calcTab->ruleBaseData->status.errCode==TERMINATE_FOUND)
         break;
   }


   /*************************************************************/
   /* Loop through the ruleCalcList and evaluate the calc nodes */
   /*************************************************************/
   for(ruleCalc=calcTab->ruleCalcList; ruleCalc; ruleCalc=ruleCalc->next)
   {  calcTab->ruleBaseData->status.errCode = OK;
      calcTab->currentRuleCalc = ruleCalc;
      CalcRuleStatements(ruleCalc, &calcTab->ruleBaseData->status, calcTab);

      if(calcTab->ruleBaseData->status.errCode==TERMINATE_FOUND)
         break;
   }


   /*************************************************************/
   /* Loop through the iterTermList and evaluate the calc nodes */
   /*************************************************************/
   for(ruleCalc=calcTab->iterTermList; ruleCalc; ruleCalc=ruleCalc->next)
   {  calcTab->ruleBaseData->status.errCode = OK;
      calcTab->currentRuleCalc = ruleCalc;
      CalcRuleStatements(ruleCalc, &calcTab->ruleBaseData->status, calcTab);

      if(calcTab->ruleBaseData->status.errCode==TERMINATE_FOUND)
         break;
   }


   if(calcTab->ruleBaseData->status.errCode!=TERMINATE_FOUND)
      ZERO(calcTab->retValue.data.num)

   if(calcTab->ruleBaseData->status.errCode==TERMINATE_FOUND ||
      calcTab->ruleBaseData->status.errCode==RETURN_FOUND)
      calcTab->ruleBaseData->status.errCode = OK;  

   return(calcTab->ruleBaseData->status.errCode);
}


/*--------------------------------------------------------------------------*/
/* SetNoAutoReset()                                                         */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long SetNoAutoReset(void *handle)
{
   tCalcTable *calcTab;

   for(calcTab=((tCalcTable*)handle);calcTab;calcTab=calcTab->next)
      calcTab->noAutoResetShared = 1; 

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* SetAutoReset()                                                           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long SetAutoReset(void *handle)
{
   tCalcTable *calcTab;

   for(calcTab=((tCalcTable*)handle);calcTab;calcTab=calcTab->next)
      calcTab->noAutoResetShared = 0; 

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* ResetSharedCalcs()                                                       */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long ResetSharedCalcs(void *handle)
{
   tCalcTable *calcTab = (tCalcTable*)handle;

   if(calcTab->numCalcDones)
      memset((char*)calcTab->calcDones, 0, calcTab->numCalcDones);

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CalcVarRule()                                                            */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long CalcVarRule(tEngVar *var, void *RBHandle)
{
   tVarCalc   *varCalc = (tVarCalc*)var->calcRule->data;
   tCalcTable *calcTab = (tCalcTable*)RBHandle;
   tAtom      eval;

   *varCalc->var->calcRule->calcDone = 1; 
   varCalc->arg->func->EvalFunc(&eval, varCalc->arg, &calcTab->ruleBaseData->status, calcTab);

   if(calcTab->ruleBaseData->status.errCode == RETURN_FOUND) 
      calcTab->ruleBaseData->status.errCode = 0;

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* GetCountStr2NumCalcs()                                                   */
/*                                                                          */
/*--------------------------------------------------------------------------*/
int GetCountStr2NumCalcs(void *RBHandle)
{
   tCalcTable   *calcTab = (tCalcTable*)RBHandle;
   tStr2NumCalc *s2nCalc;
   int          numCalcs=0;

   /* Loop through list.  This must be done instead of simply returning */
   /* calcTab->numCalcDones because that may not be up do date with     */
   /* calcs added outside the engine via ParseVarRule                   */
   for(s2nCalc=calcTab->str2numList; s2nCalc; s2nCalc=s2nCalc->next, numCalcs++);
   
   return(numCalcs);
}


/*--------------------------------------------------------------------------*/
/* MoveStr2NumCalcDones()                                                   */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long MoveStr2NumCalcDones(void *RBHandle, char *buffer, int *index)
{
   int          idx = *index;
   tCalcTable   *calcTab = (tCalcTable*)RBHandle;
   tStr2NumCalc *s2nCalc;

   /* Free the old calc done buffer */
   MyFree((void*)calcTab->calcDones);
   calcTab->calcDones = NULL;
   calcTab->numCalcDones = 0;

   /* Loop through string to numeric calcs */
   for(s2nCalc=calcTab->str2numList; s2nCalc; s2nCalc=s2nCalc->next, idx++)
      s2nCalc->calcDone = buffer + idx;
   
   *index = idx;
   return(OK);
}

