/*
** $Id: modtab.c,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: modtab.c,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.8  1997/05/13  17:55:48  can
 * Added some casts to fix C++ compile problems.
 *
 * Revision 2.7  1997/02/07  19:50:34  can
 * Made sure all files were of FILENAME_LEN
 *
 * Revision 2.6  1997/02/05  04:16:30  can
 * Changed all malloc calls the MyTmpMalloc.  This is for a
 * different memory mamangement scheme (MPAGE option) for the
 * mainframe
 *
 * Revision 2.5  1997/01/25  07:15:59  can
 * Fixed bug in parsing types with sub-records.
 *
 * Revision 2.4  1997/01/21  21:02:50  can
 * Took out '/' in model file name creation.  The modelPath
 * now includes the '/'.  This was done for the mainframe.
 *
 * Revision 2.3  1997/01/17  18:56:09  can
 * Changed hncRulePase to hncRuleParse (duh!)
 *
 * Revision 2.2  1997/01/17  17:22:36  can
 * Updates for latest release
 *
**
*/

/* System include files */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* HNC Rule Engine includes */
#include "mymall.h"
#include "typechk.h"

/* HNC common includes */
#include "modtab.h"
#include "modfunc.h"
#include "rulpars.h"
#include "except.h"
#include "hncrtyp.h"
#include "config.h"
#include "varcalc.h"
#include "safopen.h"

extern int decrypt;

static long LoadModelVarArgs(tModelVar *modelVar, tModel *model, char* fileName, char *buf);
static long AddCalcVarElement(tEngVar *var, tCalcVar **varList);
static long AddVarToVarBlock(tEngVar *var, tVarBlock *varBlock);
static long SetProfileDefault
(tProfileMap *PRM, char *defaultStr, tModel *model, char* fileName);
static int CheckPRM
(tProfileMap *PRM, tModel *model, tEngVar **sharedVarList, char* fileName);
static long AddToSection(tModel *model, int sectType, void *data);

static long LoadModelTypes
(tModel *model, char *buf, char *fileName, FILE *fp, long oldSpot);
static long LoadModelVariable
(tModel *model, tEngVar **sharedVarList, char *buf, char *fileName, FILE *fp, long oldSpot);
static long LoadModelExplainVars
(tModel *model, tEngVar **sharedVarList, char *buf, char *fileName, FILE *fp, long oldSpot);
static long LoadModelRule
(tModel *model,  char *buf, char *fileName, FILE *fp, long oldSpot);
static long LoadModelTemplate
(tModel *model, char *buf, char *fileName, FILE *fp, long oldSpot, tMasterData *masterData);
static long LoadBaseRuleList
(tModel *model, char *buf, char *fileName, FILE *fp, long oldSpot);
static long LoadModelConstant
(tModel *model, char *buf, char *fileName, FILE *fp, long oldSpot, tMasterData *masterData);
static long LoadModelProfInitRB
(tModel *model, char *buf, char *fileName, FILE *fp, long oldSpot, tMasterData *masterData);
static long LoadModelProfileMap
(tModel *model, char *buf, char *fileName, FILE *fp, long oldSpot, tEngVar **sharedVarList);
static long LoadExternalData
(tModel *model, char *buf, char *fileName, FILE *fp, long oldSpot, int *loaded, 
  tEngVar **sharedVars, tMasterData *masterData);



long AddToSection(tModel *model, int sectionType, void *data)
{
   static tSectItem   *lastItem=NULL;

   tSectItem   *newItem;
   tTabSection *section;

   /* Get the current section (the last in the list) */
   for(section=model->sectionList; section->next; section=section->next);

   /* Allocate a new item for the section */
   if((newItem = (tSectItem*)MyTmpMalloc(sizeof(tSectItem)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)newItem, 0, sizeof(tSectItem));

   /* Link the items */
   if(lastItem && section->itemList)
      lastItem->next = newItem;
   else
   {  /* Must have a new section */
      section->itemList = newItem;
      section->sectionType = sectionType;
   }

   /* Set the item data */
   newItem->data = data;
   lastItem = newItem;

   return(0);
}


long LoadModelVarArgs(tModelVar *modelVar, tModel *model, char* fileName, char *buf)
{
   int      i, j, tokenType, index, loopCount, funcCode=0;
   char     token[256], field[128], errMsg[256];
   tEngVar  *globVar, *argVar;
   tCalcVar *newArg=NULL;

   index = 0;
   loopCount = 0;
   while(buf[index] != '\0')
   {
      loopCount++;
      GetWord(buf,&index,token,&tokenType);
      if(loopCount == 1)
      {  if(tokenType == WORD_DIGIT)
            funcCode = atoi(token);
         else if(tokenType == WORD_STRING)
         {  /* Function code must be a numeric constant */
            for(globVar=model->modelRBData->globalVarList; globVar; globVar=globVar->next)
               if(VarMatch(globVar, token))
                  break;

            if(!globVar)
            {  sprintf(errMsg, "First item in arg list for variable '%s' " 
                               "in '%s' must be a function code, and '%s' "
                               "is not a defined constant",
                                modelVar->cfgName, fileName, token);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }
            else if(!(globVar->flags&VAR_CONSTANT)) 
            {  sprintf(errMsg, "First item in arg list for variable '%s' " 
                               "in '%s' must be a function code, and '%s' "
                               "is a variable, not a constant",
                                modelVar->cfgName, fileName, token);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }
            else if(!IsBinNumeric(globVar->type))
            {  sprintf(errMsg, "First item in arg list for variable '%s' " 
                               "in '%s' must be a numeric function code, and "
                               "constant '%s' is a string, not a numeric value ",
                                modelVar->cfgName, fileName, token);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }
            else
            {  if(globVar->type == HNC_FLOAT)
                  funcCode = (int)*((float*)globVar->data);
               else
                  funcCode = (int)*((long*)globVar->data);
            }
         }
         else
         {  sprintf(errMsg, "First item in arg list for variable '%s' " 
                            "in '%s' must be a function code, not '%s'", 
                             modelVar->cfgName, fileName, token);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }
      }
      else if(tokenType == WORD_STRING)
      {  /* Look for .field, indicating a record field */ 
         field[0] = '\0';
         if(buf[index] == '.')
         {  i=index;
            if(isalnum(buf[i+1]) || buf[i+1] == '_')
            {  i++; j=0;
               while(buf[i]!='\0' && (isalnum(buf[i]) || buf[i]=='_'))
               {  field[j] = buf[i]; 
                  i++; j++;
               }
               field[j] = '\0';
           
               if(buf[i]!='\0') i++;
               index = i;
            }
         }

         /* If have a record and a field, get or create a variable pointing to its data */
         if(field[0]!='\0')
         {  tEngVar  *fldVar, *prevFld;
            char     varName[256];

            /* See of variable for field has already been defined */
            fldVar=NULL; prevFld=NULL;
            sprintf(varName, "%s.%s", token, field);
            if(model->fieldVars)
            {  for(fldVar=model->fieldVars; fldVar; fldVar=fldVar->next)
               {  if(!NoCaseStrcmp(varName, fldVar->varName))
                     break;
                  prevFld = fldVar;
               }
            }

            /* See of variable for field has already been defined */
            if(!fldVar)
            {  /* Allocate a new field variable */
               if((fldVar = (tEngVar*)MyMalloc(sizeof(tEngVar)))==NULL)
                  throw(MALLOC_EXCEPTION);
               memset((char*)fldVar, 0, sizeof(tEngVar));

               /* Indicate that the data is not to be free'd */
               fldVar->flags |= VAR_NOFREE;

               /* Set the variable name */
               if((fldVar->varName = (char*)MyMalloc(strlen(varName)+1))==NULL)
                  throw(MALLOC_EXCEPTION);
               strcpy(fldVar->varName, varName);
               
               /* Link in the field variable */
               if(prevFld) prevFld->next = fldVar;
               else model->fieldVars = fldVar;

               /* Make sure the record is an existing record variable or a template */
               for(globVar=model->modelRBData->globalVarList; globVar; globVar=globVar->next)
                  if(VarMatch(globVar, token))
                     break;

               if(!globVar)
               {  tDataTemplate *tmplt;
                  tDataField    *tmpltFld;

                 /* Check for a template */ 
                 for(tmplt=model->modelRBData->templateList; tmplt; tmplt=tmplt->next)
                    if(!NoCaseStrcmp(token, tmplt->templateName))
                       break;
                  
                  if(!tmplt)
                  {  sprintf(errMsg, "Argument '%s' for variable '%s' in model '%s' "
                                     "is invalid because there is no variable or "
                                     "template named '%s'",
                                     varName, modelVar->cfgName, model->modelName, token);
                     SetConfigErrorMessage(errMsg);
                     throw(CONFIGURATION_EXCEPTION);
                  }

                  /* Make sure template has a defined data area */
                  if(!tmplt->data)
                  {  sprintf(errMsg, "Argument '%s' for variable '%s' in model '%s' "
                                     "is invalid because there is template '%s' "
                                     "is not fixed", 
                                     varName, modelVar->cfgName, model->modelName, token);
                     SetConfigErrorMessage(errMsg);
                     throw(CONFIGURATION_EXCEPTION);
                  }

                  /* Make sure field is defined for template */
                  for(tmpltFld=tmplt->fieldList; tmpltFld; tmpltFld=tmpltFld->next) 
                    if(!NoCaseStrcmp(field, tmpltFld->fieldName))
                       break;

                  if(!tmpltFld)
                  {  sprintf(errMsg, "Argument '%s' for variable '%s' in model '%s' "
                                     "is invalid because field '%s' is not defined "
                                     "for template '%s'", varName, modelVar->cfgName, 
                                      model->modelName, field, token);
                     SetConfigErrorMessage(errMsg);
                     throw(CONFIGURATION_EXCEPTION);
                  }

                  /* Get the data for the field */
                  fldVar->type = tmpltFld->type;
                  fldVar->dataSize = tmpltFld->length;  
                  fldVar->data = (char*)(tmplt->data + tmpltFld->offset);
               }
               else
               {  tRecData  *recData;
                  tRecField *fld; 
                  int       f;

                  /* Must be a fixed format record only */
                  if(globVar->type != RECORD_TYPE)
                  {  sprintf(errMsg, "Argument '%s' for variable '%s' in model '%s' "
                                     "is invalid because variable '%s' is not a "
                                     "record type", varName, modelVar->cfgName, 
                                     model->modelName, token);
                     SetConfigErrorMessage(errMsg);
                     throw(CONFIGURATION_EXCEPTION);
                  }

                  /* Make sure the field is found for the record */
                  recData = ((tRecInstance*)globVar->data)->data;
                  for(f=0, fld=recData->record->fieldList; fld; fld=fld->next, f++)
                     if(!NoCaseStrcmp(fld->fieldName,  field))
                        break;

                  if(!fld)
                  {  sprintf(errMsg, "Argument '%s' for variable '%s' in model '%s' "
                                     "is invalid because field '%s' is not defined " 
                                     "for record '%s'", varName, modelVar->cfgName, 
                                     model->modelName, field, token);
                     SetConfigErrorMessage(errMsg);
                     throw(CONFIGURATION_EXCEPTION);
                  }

                  /* Get the data for the field */
                  fldVar->type = fld->type;
                  fldVar->dataSize = fld->length;  
                  fldVar->data = ((tFlatData*)recData->flds[f].data)->data;
               }
            }

            argVar = fldVar;
         }
         else
         {  /* Find the variable in the global var list */  
            for(argVar=model->modelRBData->globalVarList; argVar; argVar=argVar->next)
               if(VarMatch(argVar, token))
                  break;
  
            /* If variable not found, then then this is an error */
            if(!argVar && funcCode != DO_NOTHING)
            {  sprintf(errMsg, "Argument '%s' for variable '%s' is not defined in " 
                               "'%s'.  '%s' must be defined BEFORE '%s'", 
                                token, modelVar->cfgName, fileName,
                                token, modelVar->cfgName);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }
         }
   
         /* Allocate a new argument and add it to the list */
         if(newArg)
         {  if((newArg->next = (tCalcVar *)MyMalloc(sizeof(tCalcVar))) == NULL)
            {  fprintf(stderr,"Could not allocate argument variable\n");
                  throw(MALLOC_EXCEPTION);
            }
            newArg = newArg->next;
         }
         else
         {  if((newArg = (tCalcVar *)MyMalloc(sizeof(tCalcVar))) == NULL)
            {  fprintf(stderr,"Could not allocate argument variable\n");
               throw(MALLOC_EXCEPTION);
            }
            modelVar->args = newArg;
         }
         memset((char*)newArg, 0, sizeof(tCalcVar));

         if(funcCode != DO_NOTHING)
            newArg->var = argVar;
         else
         {  if((newArg->var = (tEngVar*)MyMalloc(sizeof(tEngVar)))==NULL)
               throw(MALLOC_EXCEPTION);
            memset((char*)newArg->var, 0, sizeof(tEngVar)); 

            if((newArg->var->varName = (char*)MyMalloc(strlen(token)+1))==NULL)
               throw(MALLOC_EXCEPTION);

            strcpy(newArg->var->varName, token);
         }
      }
   }

   /* Validate function and arguments */
   ValidateFunction(modelVar, (short)funcCode, fileName);
   
   return(0);
}


long AddCalcVarElement(tEngVar *var, tCalcVar **varList)
{
   tCalcVar *tmpCalc, *newCalc;

   /* allocate and populate new var element */
   if((newCalc = (tCalcVar*)MyMalloc(sizeof(tCalcVar))) == NULL)
   {
      fprintf(stderr, "\nFailed to allocate calcArgVarList\n\n");
      throw(MALLOC_EXCEPTION);
   }

   newCalc->var = var;
   newCalc->next = NULL;

   /* concatenate  new var element to varlist */
   tmpCalc = *varList;
   if(tmpCalc)
   {  while(tmpCalc->next)
         tmpCalc = tmpCalc->next;
      tmpCalc->next = newCalc;
   }
   else
      *varList = newCalc;

   return(0);
}


long AddVarToVarBlock(tEngVar *var, tVarBlock *varBlock)
{
   tModelVar *modelVar;

   /* Must have a modelVar structure */
   modelVar = (tModelVar *)var->user;
   if(!modelVar) return(0);

   /* Add the variable to the proper list */
   if(var->calcRule || modelVar->args)
      AddCalcVarElement(var, &varBlock->calcList);
   else
      AddCalcVarElement(var, &varBlock->noCalcList);

   return(0);
}


long LoadModelTypes
(tModel *model, char *buf, char *fileName, FILE *fp, long oldSpot)
{
   tType *type, *prevType;
   int   k, loopCount=0, numTypes, numElems, numScans;
   char  whichType[16], typeName[64], elemTypeStr[64], errMsg[256];

   /* Reset file to beginning of the segment */
   fseek(fp, oldSpot, 0);
   GetRcrd(buf, 1024, fp,decrypt);
   if(sscanf(buf, "TYP . %d\n", &numTypes) != 1)
   {  sprintf(errMsg, "Bad format for 'TYP' = '%s' in '%s'", 
                                            buf, fileName);
      SetConfigErrorMessage(errMsg);
      throw(CONFIGURATION_EXCEPTION);
   }

   /* Loop through reading all the types */ 
   while(loopCount < numTypes)
   {  
      if(GetRcrd(buf,1024,fp,decrypt)==NULL)
      {  sprintf(errMsg, "Failed to read to types in '%s'", fileName);
         SetConfigErrorMessage(errMsg);
         throw(CONFIGURATION_EXCEPTION);
      }
  
      k = 0;

      while(isspace(buf[k]) && buf[k]!='\n') k++; 
      if(buf[k] == '\n') continue;

      loopCount++;

      /* Allow RECORD and ARRAY types.  Find out type and name */
      numScans = sscanf(buf, "%s . %s . %d . %s\n", 
                        whichType, typeName, &numElems, elemTypeStr);
      if(numScans < 3)
      {  sprintf(errMsg, "Bad format for type in '%s' at type #'%d'\n", fileName, loopCount);
         SetConfigErrorMessage(errMsg);
         throw(CONFIGURATION_EXCEPTION);
      }

      /* Make sure type with same name doesn't already exist */
      prevType = NULL;
      for(type=model->modelRBData->globalTypeList; type; type=type->next)
      {  if(!NoCaseStrcmp(typeName, type->typeName))
         {  sprintf(errMsg, "Type '%s' already exists.  Cannot be "
                            "redefined in '%s'\n", typeName, fileName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }
         prevType = type;
      }

      /* Allocate a new type */
      if((type = (tType*)MyMalloc(sizeof(tType)))==NULL)
         throw(MALLOC_EXCEPTION);
      memset((char*)type, 0, sizeof(tType));

      /* Allocate typeName */
      if((type->typeName = (char*)MyMalloc(strlen(typeName)+1))==NULL)
         throw(MALLOC_EXCEPTION);
      strcpy(type->typeName, typeName);

      /* Link type into global type list */
      if(prevType) prevType->next = type;
      else model->modelRBData->globalTypeList = type;

      /* Add the type to the current section */
      AddToSection(model, TYP_SECTION, (void*)type);

      /* If type is RECORD, parse out fields and create record type */
      if(!strcmp(whichType, "RECORD") || !strcmp(whichType, "101"))
      {  tRecField *fld, *tmpFld; 
         char      fldTypeStr[64], fldName[64];
         int       fldCount, fldType=0, fldSize, offset;

         type->type = RECORD_TYPE;
         
         /* Set up tEngRecord structure */
         type->fmt.record.recName = type->typeName;
         type->fmt.record.numFlds = numElems;

         /* Read the fields */
         fldCount=0; fld=NULL; offset=0;
         while(fldCount < numElems)
         {  /* Read the field data */
            if(GetRcrd(buf,1024,fp,decrypt)==NULL)
            {  sprintf(errMsg, "Failed to read to field #'%d' for record "
                               "'%s' in '%s'\n", fldCount, typeName, fileName); 
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }
  
            k = 0;
            while(isspace(buf[k]) && buf[k]!='\n') k++; 
            if(buf[k] == '\n') continue;
            fldCount++;

            /* Get the field name and type */ 
            if((sscanf(buf, "%s . %s", fldName, fldTypeStr)) != 2)
            {  sprintf(errMsg, "Bad format of field #'%d' for record '%s' in '%s'", 
                               fldCount, typeName, fileName);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }

            /* Make sure field isn't a duplicate */
            for(tmpFld=type->fmt.record.fieldList; tmpFld; tmpFld=tmpFld->next)
               if(!NoCaseStrcmp(tmpFld->fieldName, fldName))
               {  sprintf(errMsg, "Field '%s' is already defined for record "
                                  "'%s' in '%s'\n", fldName, typeName, fileName);
                  SetConfigErrorMessage(errMsg);
                  throw(CONFIGURATION_EXCEPTION);
               }

            /* Allocate a new field */
            if(fld)
            {  if((fld->next = (tRecField*)MyMalloc(sizeof(tRecField)))==NULL)
                  throw(MALLOC_EXCEPTION);
               fld = fld->next;
            }
            else
            {  if((fld = (tRecField*)MyMalloc(sizeof(tRecField)))==NULL)
                  throw(MALLOC_EXCEPTION);
               type->fmt.record.fieldList = fld;
            }
            memset((char*)fld, 0, sizeof(tRecField));

            /* Convert the type string */
            if(isdigit(fldTypeStr[0]))
            {  fldSize=0;
               if((fldType = CheckFldType(fldTypeStr, NULL, &fldSize)) == -1)
               {  sprintf(errMsg, "Bad type specified for field '%s' of record "
                                  "'%s' in '%s'\n", fldName, typeName, fileName);
                  SetConfigErrorMessage(errMsg);
                  throw(CONFIGURATION_EXCEPTION);
               }

               fld->length = fldSize;
               fld->type = fldType;
            }
            else
            {  tType *globType;

               /* Make sure type is known user defined type */
               for(globType=model->modelRBData->globalTypeList; 
                                         globType; globType=globType->next)
                  if(!NoCaseStrcmp(fldTypeStr, globType->typeName))
                     break;

               if(!globType)
               {  sprintf(errMsg, "Bad type, '%s', specified for field '%s' of record "
                                  "'%s' in '%s'\n", fldTypeStr, fldName, typeName, fileName);
                  SetConfigErrorMessage(errMsg);
                  throw(CONFIGURATION_EXCEPTION);
               }
               else
               {  if(globType->type != RECORD_TYPE)
                  {  sprintf(errMsg, "Type '%s' specified for field '%s' of record "
                                     "'%s' in '%s' must be a RECORD_TYPE\n", 
                                     fldTypeStr, fldName, typeName, fileName);
                     SetConfigErrorMessage(errMsg);
                     throw(CONFIGURATION_EXCEPTION);
                  }
   
                  fldType = fld->type = RECORD_TYPE;
                  fld->typeFmt = &globType->fmt;
                  fld->length = globType->fmt.record.dataSize;
               }
            }

            /* Set up field info */
            if((fld->fieldName = (char*)MyMalloc(strlen(fldName)+1))==NULL)
               throw(MALLOC_EXCEPTION);
            strcpy(fld->fieldName, fldName);

            /* Make sure fields are aligned for their type */
            if(fldType == RECORD_TYPE)
               fldType = fld->typeFmt->record.fieldList->type;

            if((fldType == HNC_INT16 || fldType == HNC_UINT16) && 
               (offset%sizeof(short)!=0))
               offset += sizeof(short) - offset%sizeof(short);
            else if((fldType == HNC_INT32 || fldType == HNC_UINT32) && 
                    (offset%sizeof(long)!=0))
               offset += sizeof(long) - offset%sizeof(long);
            else if((fldType == HNC_FLOAT) && (offset%sizeof(float)!=0))
               offset += sizeof(float) - offset%sizeof(float);
            else if((fldType == HNC_DOUBLE) && (offset%sizeof(double)!=0))
               offset += sizeof(double) - offset%sizeof(double);
                
            fld->offset = offset;
            offset += fld->length;
         }

         type->fmt.record.dataSize = offset;
      }
      /* If type is ARRAY, parse out fields and create array type */
      else if(!strcmp(whichType, "ARRAY") || !strcmp(whichType, "102"))
      {  int elemType=0, elemSize=0;

         type->type = ARRAY_TYPE;

         /* Element type must be specified */ 
         elemSize = 0;
         if(numScans != 4)
         {  sprintf(errMsg, "Must specify number of elements for array "
                            "type '%s' in '%s'\n", typeName, fileName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }
         else if((elemType = CheckFldType(elemTypeStr, NULL, &elemSize)) == -1)
         {  sprintf(errMsg, "Bad type specified for array elements "
                            "for array '%s' in '%s'\n", 
                            typeName, fileName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }

     
         /* numElems must have be valid */
         if(numElems < 1)
         {  sprintf(errMsg, "Number of elements for array type "
                            "'%s' in '%s' must be greater than 0\n",
                            typeName, fileName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }
       
         /* Set up the tEngArray data */
         type->fmt.array.name = type->typeName;
         type->fmt.array.numElems = numElems;
         type->fmt.array.startIndex = 1;
         type->fmt.array.endIndex = numElems;
         type->fmt.array.type = elemType;
         type->fmt.array.elemSize = elemSize;
         type->fmt.array.dataSize = numElems * elemSize;
      }
      else
      {  sprintf(errMsg, "Invalid type '%s' in '%s' at type #'%d'\n", 
                                                 whichType, fileName, loopCount);
         SetConfigErrorMessage(errMsg);
         throw(CONFIGURATION_EXCEPTION);
      }
   }

   return(0);
}


long LoadModelVariable
(tModel *model, tEngVar **sharedVarList, char *buf, char *fileName, FILE *fp, long oldSpot)
{
   int           nbrVar, loopCount, k,  i, numScans, isShared;
   long          status;
   tEngVar       *prevShare, *sharedVar, *prevVar, *globVar=NULL, *tmp; 
   tCalcVar      *argVar, *delArg;
   tVarBlock     *tmpBlock, *varBlock=NULL;
   tRuleBaseData *ruleBaseData;
   tModelVar     *modelVar, *tmpMVar, *shareMVar;
   float         range;
   char          errMsg[256], type[50], listName[50], expStr[30];
   char          cName[100], sasName[20], cfgName[50];
   char          varRule[MAX_RULE_LEN];
  

   /* reset file to beginning of the segment */
   fseek(fp, oldSpot, 0);
#ifdef DEBUG
fprintf(stderr, "\t\toldSpot = %ld\n",oldSpot);
#endif
   GetRcrd(buf, 1024, fp, decrypt);
   listName[0] = '\0';
   if(sscanf(buf, "VAR . %d . %s\n", &nbrVar, listName) < 1)
   {  sprintf(errMsg, "Bad format for 'VAR' = '%s' in '%s'", 
                                                  buf, fileName);
      SetConfigErrorMessage(errMsg);
      throw(CONFIGURATION_EXCEPTION);
   } 
   else if(nbrVar > 0)
   {
#ifdef DEBUG
fprintf(stderr, "\t\tnbrVar = %d\n",nbrVar);
#endif
      loopCount = 0;
      ruleBaseData = model->modelRBData;
      if(ruleBaseData->globalVarList)
      {  for(tmp=ruleBaseData->globalVarList; tmp->next; tmp=tmp->next);
         globVar = tmp;
      }

      /* Allocate a new variable block structure */
      if((varBlock = (tVarBlock*)MyMalloc(sizeof(tVarBlock)))==NULL)
         throw(MALLOC_EXCEPTION);
      memset((char*)varBlock, 0, sizeof(tVarBlock));

      /* If there is a name, add to the sub block list */
      /* Otherwise, add to the main block list         */
      if(listName[0]!='\0')
      {  tVarBlock *prevBlock=NULL;

         /* See if a list by the same name already exists */
         for(tmpBlock=model->subBlockList; tmpBlock; tmpBlock=tmpBlock->next)  
         {  if(!strcmp(listName, model->subBlockList->name))
            {  sprintf(errMsg, "Variable block '%s' defined in '%s' "
                                  "already exists", listName, fileName);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }

            prevBlock = tmpBlock;
         }

         /* Link in the new list */
         if(prevBlock) prevBlock->next = varBlock;
         else model->subBlockList = varBlock;

         /* Set the name */
         if((varBlock->name = (char*)MyMalloc(strlen(listName)+1))==NULL)
            throw(MALLOC_EXCEPTION);
         strcpy(varBlock->name, listName);
         listName[0] = '\0';
      }
      else
      {  /* Go to the end of the list, if any */
         if(model->mainBlockList)
         {  for(tmpBlock=model->mainBlockList; tmpBlock->next; tmpBlock=tmpBlock->next);
            tmpBlock->next = varBlock;
         }
         else
            model->mainBlockList = varBlock;
      }

      /* Make the variable block the current section */
      AddToSection(model, VAR_SECTION, (void*)varBlock);

      while(loopCount < nbrVar)
      {
         if(GetRcrd(buf,1024,fp,decrypt)==NULL)
         {  sprintf(errMsg, "Failed read to variable names in '%s'", fileName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }
    
         k = 0;

         while(isspace(buf[k]) && buf[k]!='\n') k++; 
         if(buf[k] == '\n') continue;

         loopCount++;

         /***************************/
         /* Allocate a new variable */
         /***************************/
         prevVar = globVar;
         if(globVar==NULL)
         {  
            if((globVar = (tEngVar*)MyMalloc(sizeof(tEngVar)))==NULL)
            {  fprintf(stderr, "\nFailed to allocate globVar\n\n");
               throw(MALLOC_EXCEPTION);
            }
            ruleBaseData->globalVarList = globVar;
         }
         else
         {  
            if((globVar->next = (tEngVar*)MyMalloc(sizeof(tEngVar)))==NULL)
            {  fprintf(stderr, "\nFailed to allocate globVar->Next\n\n");
               throw(MALLOC_EXCEPTION);
            }
            globVar = globVar->next;
         }
         memset((char*)globVar, 0, sizeof(tEngVar));

         /**********************************************/
         /* Allocate a special model variable structue */
         /**********************************************/
         if((modelVar = (tModelVar*)MyMalloc(sizeof(tModelVar)))==NULL)
         {  fprintf(stderr, "\nFailed to allocate modelVar\n\n");
            throw(MALLOC_EXCEPTION);
         }
         memset((char*)modelVar, 0, sizeof(tModelVar));
         globVar->user = (void*)modelVar;

         /**********************/
         /* Get Variable Names */
         /**********************/
         numScans = sscanf(buf, "%s . %s . %s . %s . %d\n", 
                      cfgName, cName, sasName, type, &isShared);

         if(numScans < 4)
         {  if(prevVar) sprintf(errMsg, "Error reading variable names after variable '%s' "
                                        "in '%s' at '%s'",
                                         prevVar->varName, fileName, buf);
            else  sprintf(errMsg, "Error reading variable names in '%s' at '%s'",
                                         fileName, buf);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }
         else if(numScans == 4 || !sharedVarList)
            modelVar->isShared = NOT_SHARED;
         else if(numScans == 5 && isShared)
            modelVar->isShared = (int)ORIGINAL;

         if(!NoCaseStrcmp(cfgName, "C") || !NoCaseStrcmp(cfgName, "S"))
         {  sprintf(errMsg, "'%s' is not a valid CFG variable name in '%s'", 
                                                            cfgName, fileName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }

         /* Allocate names and copy them */
         if((modelVar->cfgName = (char*)MyMalloc(strlen(cfgName)+1))==NULL) 
            throw(MALLOC_EXCEPTION);
         strcpy(modelVar->cfgName, cfgName);

         if((modelVar->sasName = (char*)MyTmpMalloc(strlen(sasName)+1))==NULL) 
            throw(MALLOC_EXCEPTION);
         strcpy(modelVar->sasName, sasName);

         if((modelVar->cName = (char*)MyTmpMalloc(strlen(cName)+1))==NULL) 
            throw(MALLOC_EXCEPTION);
         strcpy(modelVar->cName, cName);

         globVar->varName = modelVar->cfgName;
         globVar->dataSize = 0;

         /* Type may be HNC coded type, or user defined type */
         if(isdigit(type[0]))
         {  if((globVar->type = CheckFldType(type, &globVar->data, &globVar->dataSize)) == -1)
            {  sprintf(errMsg, "Bad type '%s' for variable '%s' in '%s'", 
                                           type, globVar->varName, fileName);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }
         }
         else
         {  tType *globType;

            /* Make sure type is known user defined type */
            for(globType=model->modelRBData->globalTypeList; globType; globType=globType->next)
               if(!NoCaseStrcmp(type, globType->typeName))
                  break;

            if(!globType)
            {  sprintf(errMsg, "Variable '%s' in model '%s' is defined as type '%s', "
                               "but that is not a globally defined type.",
                                globVar->varName, model->modelName, type);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }
            else
            {  if((status=MakeTypeInstance(globVar, globType)))
               {  sprintf(errMsg, "Failed in MakeTypeInstance with status '%ld' for "
                                  "variable '%s' in model '%s'",
                                   status, globVar->varName, model->modelName);
                  SetConfigErrorMessage(errMsg);
                  throw(CONFIGURATION_EXCEPTION);
               }
            }
         }

         /*********************************************/
         /* Make sure the var name is not a duplicate */
         /*********************************************/
         errMsg[0] = '\0';
         for(tmp=ruleBaseData->globalVarList; tmp->next; tmp=tmp->next)
         {  if(!NoCaseStrcmp(tmp->varName, globVar->varName))
            {  sprintf(errMsg, "Variable '%s' in '%s' has already been defined", 
                                                       globVar->varName, fileName);
            }
            else if(tmp->user)
            {  tmpMVar = (tModelVar*)tmp->user;
               if(!NoCaseStrcmp(modelVar->cfgName, tmpMVar->cName))
               {  sprintf(errMsg, "Variable '%s' in '%s' has same CFG name "
                                  "as the C name of variable '%s'", 
                                     globVar->varName, fileName, tmpMVar->cfgName);
               }
               
               if(!NoCaseStrcmp(modelVar->cfgName, tmpMVar->sasName))
               {  sprintf(errMsg, "Variable '%s' in '%s' has same CFG name "
                                  "as the SAS name of variable '%s'", 
                                     globVar->varName, fileName, tmpMVar->cfgName);
               }
               
               if(NoCaseStrcmp(modelVar->cName, "C"))
               {  if(!NoCaseStrcmp(modelVar->cName, tmpMVar->cfgName))
                  {  sprintf(errMsg, "Variable '%s' in '%s' has same C name "
                                     "as the CFG name of variable '%s'", 
                                        globVar->varName, fileName, tmpMVar->cfgName);
                  }
               
                  if(!NoCaseStrcmp(modelVar->cName, tmpMVar->cName))
                  {  sprintf(errMsg, "Variable '%s' in '%s' has same C name "
                                     "as the C name of variable '%s'", 
                                        globVar->varName, fileName, tmpMVar->cfgName);
                  }
                  
                  if(!NoCaseStrcmp(modelVar->cName, tmpMVar->sasName))
                  {  sprintf(errMsg, "Variable '%s' in '%s' has same C name "
                                     "as the SAS name of variable '%s'", 
                                        globVar->varName, fileName, tmpMVar->cfgName);
                  }
               }
               
               if(NoCaseStrcmp(modelVar->sasName, "S"))
               {  if(!NoCaseStrcmp(modelVar->sasName, tmpMVar->cfgName))
                  {  sprintf(errMsg, "Variable '%s' in '%s' has same SAS name "
                                     "as the CFG name of variable '%s'", 
                                        globVar->varName, fileName, tmpMVar->cfgName);
                  }
                  
                  if(!NoCaseStrcmp(modelVar->sasName, tmpMVar->cName))
                  {  sprintf(errMsg, "Variable '%s' in '%s' has same SAS name "
                                     "as the C name of variable '%s'", 
                                        globVar->varName, fileName, tmpMVar->cfgName);
                  }
                  
                  if(!NoCaseStrcmp(modelVar->sasName, tmpMVar->sasName))
                  {  sprintf(errMsg, "Variable '%s' in '%s' has same SAS name "
                                     "as the SAS name of variable '%s'", 
                                        globVar->varName, fileName, tmpMVar->cfgName);
                  }
               }
            }

            if(errMsg[0] != '\0')
            {  SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }
         }

         /************************************************/
         /* Read the explanation code, then loop through */
         /* and read the explanation bin data, if any    */
         /************************************************/
         if(GetRcrd(buf,1024,fp,decrypt)==NULL)
         {  sprintf(errMsg, "Failed to read '%s' at explanations for variable '%s'",
                                                    fileName, globVar->varName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }

         if(sscanf(buf, "%s . %d", expStr, &modelVar->expBins.numBins) != 2)
         {  sprintf(errMsg, "Bad read of explanation code and count at "
                            "'%s' for var '%s' in '%s'", 
                               buf, globVar->varName, fileName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }

         /* Check for explainVal/explainGrp */
         if(strchr(expStr, '('))
            sscanf(expStr, "%hd(%hd)", &modelVar->explainVal, &modelVar->explainGrp);
         else
         {  sscanf(expStr, "%hd", &modelVar->explainVal);
            modelVar->explainGrp = modelVar->explainVal;
         }

         if(modelVar->expBins.numBins > 0)
         {  if((modelVar->expBins.bin = (tBin*)MyCalloc(modelVar->expBins.numBins,
                                                      sizeof(tBin)))==NULL)
               throw(MALLOC_EXCEPTION);

            for(i=0; i<modelVar->expBins.numBins; i++)
            {  if(GetRcrd(buf,1024,fp,decrypt)==NULL)
               {  sprintf(errMsg, "File '%s' read error at explanation data for '%s'", 
                                                       fileName, globVar->varName);
                  SetConfigErrorMessage(errMsg);
                  throw(CONFIGURATION_EXCEPTION);
               }
   
               if(sscanf(buf, "%g %g", &range, 
                                       &modelVar->expBins.bin[i].expect) != 2)
               {  sprintf(errMsg, "Bad read of explanation bin data for variable "
                                  "'%s' at bin '%d' in '%s'", 
                                          globVar->varName, i, fileName);
                  SetConfigErrorMessage(errMsg);
                  throw(CONFIGURATION_EXCEPTION);
               }
     
               if(range < 0)
                  modelVar->expBins.bin[i].range =
                                          (float)((int)((range*100.0)-.5)/100.0);
               else
                  modelVar->expBins.bin[i].range =
                                          (float)((int)((range*100.0)+.5)/100.0);
            }
         }

         /******************************************************/
         /* Read the calculation data, if any.  This is either */  
         /* a zero, a function and a list of arguments, or a   */ 
         /* a rule that calculates this variable.              */
         /******************************************************/
         if(GetRcrd(buf,1024,fp,decrypt)==NULL)
         {  sprintf(errMsg, "Failed to read calc for var '%s' in '%s'", 
                                                            globVar->varName,
                                                            fileName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }

         if(buf[0] == '0')
         {  
#ifdef DEVELOP
            if(modelVar->isShared)
            {  fprintf(stderr, "Warning: Variable '%s' in '%s' "
                               "is shared but it has no calculation data\n",
                                                            globVar->varName,
                                                            fileName);
            }
#endif
         }
         else if(strchr(buf, ','))
         {  if(LoadModelVarArgs(modelVar, model, fileName, buf) != 0)
            {  sprintf(errMsg, "LoadModelVarArgs error for variable '%s' in '%s'", 
                                                                     globVar->varName,
                                                                     fileName);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }
         }
         else
         {  char *ptr;
            /* Allocate a calcRule structure for the var */ 
            if((globVar->calcRule = (tCalcRule*)MyMalloc(sizeof(tCalcRule)))==NULL)
               throw(MALLOC_EXCEPTION);
            memset((char*)globVar->calcRule, 0, sizeof(tCalcRule)); 

            /* Read the rule text up to ENDVAR */
            ptr = varRule;
            sprintf(ptr, "%s", buf);
            ptr += strlen(buf);
  
            while(1)
            {  if(GetRcrd(buf,1024,fp,decrypt)==NULL)
               {  sprintf(errMsg, "Failed to calc rule data for var '%s' in '%s'", 
                                                                     globVar->varName,
                                                                     fileName);
                  SetConfigErrorMessage(errMsg);
                  throw(CONFIGURATION_EXCEPTION);
               }

               if(!memcmp(buf, "ENDVAR", 6))
               {  /* Look for _NOAUTO */ 
                  if(strlen(buf) > 8)
                  {  if(!memcmp(buf, "ENDVAR_NOAUTO", 13)) 
                        globVar->calcRule->noAutoCalc = 1;
                  }

                  /* Erase all after "END" */
                  buf[3] = '\0';
                  sprintf(ptr, "%s", buf);
                  ptr += strlen(buf);
                  break;
               }

               sprintf(ptr, "%s", buf);
               ptr += strlen(buf);
            }            

            if((globVar->calcRule->code=(char*)MyTmpMalloc(strlen(varRule)+1))==NULL)
               throw(MALLOC_EXCEPTION);
            strcpy(globVar->calcRule->code, varRule);
         }

         if(modelVar->isShared && sharedVarList)
         {  /*****************************************************/
            /* See if the variable is already on the shared list */
            /*****************************************************/
            prevShare = *sharedVarList;
            for(sharedVar=*sharedVarList; sharedVar; sharedVar=sharedVar->next)
            {  if(!NoCaseStrcmp(globVar->varName, sharedVar->varName))
                  break;
               prevShare = sharedVar;
            }

            /* If var on list, make sure calc data matches */
            if(sharedVar)
            {  if(sharedVar->calcRule)
               {  if(NoCaseStrcmp(sharedVar->calcRule->code, globVar->calcRule->code))
                  {  sprintf(errMsg, "Calc rule for variable '%s' in '%s' doesn't "
                                     "match calc rule for shared variable", 
                                                                     globVar->varName,
                                                                     fileName);
                     SetConfigErrorMessage(errMsg);
                     throw(CONFIGURATION_EXCEPTION);
                  }
                  else
                  {  MyFree((void*)globVar->calcRule->code);
                     MyFree((void*)globVar->calcRule);
                     globVar->calcRule = sharedVar->calcRule;
                  }
               }

               MyFree((void*)globVar->data);
               globVar->data = sharedVar->data;
               globVar->varName = sharedVar->varName;

               if(globVar->user)
               {  tmpMVar = (tModelVar*)globVar->user;
                  shareMVar = (tModelVar*)sharedVar->user;

                  argVar=tmpMVar->args;
                  while(argVar)
                  {  delArg = argVar;

                     if(!(argVar->var->user || argVar->var->flags&VAR_CONSTANT)) 
                     {  MyFree((void*)argVar->var->varName);
                        MyFree((void*)argVar->var);
                     }
                     
                     argVar = argVar->next;
                     MyFree((void*)delArg);
                  }
                  tmpMVar->args = shareMVar->args;
                  tmpMVar->isShared = COPY;

                  MyFree((void*)tmpMVar->cName);
                  tmpMVar->cName = shareMVar->cName;
                  MyFree((void*)tmpMVar->cfgName);
                  tmpMVar->cfgName = shareMVar->cfgName;
                  MyFree((void*)tmpMVar->sasName);
                  tmpMVar->sasName = shareMVar->sasName;
               }
            }
            else
            {  /* Allocate a new shared var and copy globVar */
               if((sharedVar=(tEngVar*)MyMalloc(sizeof(tEngVar)))==NULL)
                  throw(MALLOC_EXCEPTION);

                *sharedVar = *globVar;

                if(prevShare)
                   prevShare->next = sharedVar;
                else
                   *sharedVarList = sharedVar;
            }
         }

         /************************************/
         /* Add the variable to calc section */
         /************************************/
         AddVarToVarBlock(globVar, varBlock);
      }
   }
#ifdef DEBUG
fprintf(stderr, "\t\tloopCount = %d\n",loopCount);
#endif

   return(0);
}


/* Flag to discern these variables from normal variables */
long LoadModelExplainVars
(tModel *model, tEngVar **sharedVarList, char *buf, 
   char *fileName, FILE *fp, long oldSpot)
{
   int           nbrVar, loopCount, k,  i, numScans;
   tEngVar       *sharedVar, *prevVar, *globVar=NULL, *tmp; 
   tRuleBaseData *ruleBaseData;
   tModelVar     *modelVar, *tmpMVar, *shareMVar;
   float         range;
   char          errMsg[256], cName[100];
   char          sasName[20], cfgName[50], expStr[30];
  

   /* reset file to beginning of the segment */
   fseek(fp, oldSpot, 0);
#ifdef DEBUG
fprintf(stderr, "\t\toldSpot = %ld\n",oldSpot);
#endif
   GetRcrd(buf, 1024, fp, decrypt);
   if(sscanf(buf, "XPL . %d\n", &nbrVar) < 1)
   {  sprintf(errMsg, "Bad format for 'XPL' = '%s' in '%s'", 
                                                  buf, fileName);
      SetConfigErrorMessage(errMsg);
      throw(CONFIGURATION_EXCEPTION);
   } 
   else if(nbrVar > 0)
   {
#ifdef DEBUG
fprintf(stderr, "\t\tnbrVar = %d\n",nbrVar);
#endif
      loopCount = 0;
      ruleBaseData = model->modelRBData;
      if(ruleBaseData->globalVarList)
      {  for(tmp=ruleBaseData->globalVarList; tmp->next; tmp=tmp->next);
         globVar = tmp;
      }

      while(loopCount < nbrVar)
      {
         if(GetRcrd(buf,1024,fp,decrypt)==NULL)
         {  sprintf(errMsg, "Failed to variable names in '%s'", fileName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }
    
         k = 0;

         while(isspace(buf[k]) && buf[k]!='\n') k++; 
         if(buf[k] == '\n') continue;

         loopCount++;

         /***************************/
         /* Allocate a new variable */
         /***************************/
         prevVar = globVar;
         if(globVar==NULL)
         {  
            if((globVar = (tEngVar*)MyMalloc(sizeof(tEngVar)))==NULL)
            {  fprintf(stderr, "\nFailed to allocate globVar\n\n");
               throw(MALLOC_EXCEPTION);
            }
            ruleBaseData->globalVarList = globVar;
         }
         else
         {  
            if((globVar->next = (tEngVar*)MyMalloc(sizeof(tEngVar)))==NULL)
            {  fprintf(stderr, "\nFailed to allocate globVar->Next\n\n");
               throw(MALLOC_EXCEPTION);
            }
            globVar = globVar->next;
         }
         memset((char*)globVar, 0, sizeof(tEngVar));

         /* Add the variable to the current section */
         AddToSection(model, XPL_SECTION, (void*)globVar);

         /**********************************************/
         /* Allocate a special model variable structue */
         /**********************************************/
         if((modelVar = (tModelVar*)MyMalloc(sizeof(tModelVar)))==NULL)
         {  fprintf(stderr, "\nFailed to allocate modelVar\n\n");
            throw(MALLOC_EXCEPTION);
         }
         memset((char*)modelVar, 0, sizeof(tModelVar));
         globVar->user = (void*)modelVar;

         /**********************/
         /* Get Variable Names */
         /**********************/
         numScans = sscanf(buf, "%s . %s . %s\n", cfgName, cName, sasName);

         if(numScans < 3)
         {  if(prevVar) sprintf(errMsg, "Error reading explain variable names "
                                        "after variable '%s' in '%s' at '%s'",
                                         prevVar->varName, fileName, buf);
            else  sprintf(errMsg, "Error reading explain variable names in '%s' at '%s'",
                                         fileName, buf);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }

         if(!NoCaseStrcmp(cfgName, "C") || !NoCaseStrcmp(cfgName, "S"))
         {  sprintf(errMsg, "'%s' is not a valid CFG variable name in '%s'", 
                                                            cfgName, fileName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }

         /* Allocate names and copy them */
         if((modelVar->cfgName = (char*)MyMalloc(strlen(cfgName)+1))==NULL) 
            throw(MALLOC_EXCEPTION);
         strcpy(modelVar->cfgName, cfgName);

         if((modelVar->sasName = (char*)MyMalloc(strlen(sasName)+1))==NULL) 
            throw(MALLOC_EXCEPTION);
         strcpy(modelVar->sasName, sasName);

         if((modelVar->cName = (char*)MyMalloc(strlen(cName)+1))==NULL) 
            throw(MALLOC_EXCEPTION);
         strcpy(modelVar->cName, cName);

         globVar->varName = modelVar->cfgName;

         /*********************************************/
         /* Make sure the var name is not a duplicate */
         /*********************************************/
         errMsg[0] = '\0';
         for(tmp=ruleBaseData->globalVarList; tmp->next; tmp=tmp->next)
         {  if(!NoCaseStrcmp(tmp->varName, globVar->varName))
            {  sprintf(errMsg, "Variable '%s' in '%s' has already been defined", 
                                                       globVar->varName, fileName);
            }
            else if(tmp->user)
            {  tmpMVar = (tModelVar*)tmp->user;
               if(!NoCaseStrcmp(modelVar->cfgName, tmpMVar->cName))
               {  sprintf(errMsg, "Variable '%s' in '%s' has same CFG name "
                                  "as the C name of variable '%s'", 
                                     globVar->varName, fileName, tmpMVar->cfgName);
               }
               
               if(!NoCaseStrcmp(modelVar->cfgName, tmpMVar->sasName))
               {  sprintf(errMsg, "Variable '%s' in '%s' has same CFG name "
                                  "as the SAS name of variable '%s'", 
                                     globVar->varName, fileName, tmpMVar->cfgName);
               }
               
               if(NoCaseStrcmp(modelVar->cName, "C"))
               {  if(!NoCaseStrcmp(modelVar->cName, tmpMVar->cfgName))
                  {  sprintf(errMsg, "Variable '%s' in '%s' has same C name "
                                     "as the CFG name of variable '%s'", 
                                        globVar->varName, fileName, tmpMVar->cfgName);
                  }
               
                  if(!NoCaseStrcmp(modelVar->cName, tmpMVar->cName))
                  {  sprintf(errMsg, "Variable '%s' in '%s' has same C name "
                                     "as the C name of variable '%s'", 
                                        globVar->varName, fileName, tmpMVar->cfgName);
                  }
                  
                  if(!NoCaseStrcmp(modelVar->cName, tmpMVar->sasName))
                  {  sprintf(errMsg, "Variable '%s' in '%s' has same C name "
                                     "as the SAS name of variable '%s'", 
                                        globVar->varName, fileName, tmpMVar->cfgName);
                  }
               }
               
               if(NoCaseStrcmp(modelVar->sasName, "S"))
               {  if(!NoCaseStrcmp(modelVar->sasName, tmpMVar->cfgName))
                  {  sprintf(errMsg, "Variable '%s' in '%s' has same SAS name "
                                     "as the CFG name of variable '%s'", 
                                        globVar->varName, fileName, tmpMVar->cfgName);
                  }
                  
                  if(!NoCaseStrcmp(modelVar->sasName, tmpMVar->cName))
                  {  sprintf(errMsg, "Variable '%s' in '%s' has same SAS name "
                                     "as the C name of variable '%s'", 
                                        globVar->varName, fileName, tmpMVar->cfgName);
                  }
                  
                  if(!NoCaseStrcmp(modelVar->sasName, tmpMVar->sasName))
                  {  sprintf(errMsg, "Variable '%s' in '%s' has same SAS name "
                                     "as the SAS name of variable '%s'", 
                                        globVar->varName, fileName, tmpMVar->cfgName);
                  }
               }
            }

            if(errMsg[0] != '\0')
            {  SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }
         }

         /************************************************/
         /* Read the explanation code, then loop through */
         /* and read the explanation bin data, if any    */
         /************************************************/
         if(GetRcrd(buf,1024,fp,decrypt)==NULL)
         {  sprintf(errMsg, "Failed to read '%s' at explanations for explain variable '%s'",
                                                    fileName, globVar->varName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }

         if(sscanf(buf, "%s . %d", expStr, &modelVar->expBins.numBins) != 2)
         {  sprintf(errMsg, "Bad read of explanation code and count at "
                            "'%s' for var '%s' in '%s'", 
                               buf, globVar->varName, fileName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }

         /* Check for explainVal/explainGrp */
         if(strchr(expStr, '('))
            sscanf(expStr, "%hd(%hd)", &modelVar->explainVal, &modelVar->explainGrp);
         else
         {  sscanf(expStr, "%hd", &modelVar->explainVal);
            modelVar->explainGrp = modelVar->explainVal;
         }

         if(modelVar->expBins.numBins > 0)
         {  if((modelVar->expBins.bin = (tBin*)MyCalloc(modelVar->expBins.numBins,
                                                      sizeof(tBin)))==NULL)
               throw(MALLOC_EXCEPTION);

            for(i=0; i<modelVar->expBins.numBins; i++)
            {  if(GetRcrd(buf,1024,fp,decrypt)==NULL)
               {  sprintf(errMsg, "File '%s' read error at explanation data for '%s'", 
                                                       fileName, globVar->varName);
                  SetConfigErrorMessage(errMsg);
                  throw(CONFIGURATION_EXCEPTION);
               }
   
               if(sscanf(buf, "%g %g", &range, 
                                       &modelVar->expBins.bin[i].expect) != 2)
               {  sprintf(errMsg, "Bad read of explanation bin data for variable "
                                  "'%s' at bin '%d' in '%s'", 
                                          globVar->varName, i, fileName);
                  SetConfigErrorMessage(errMsg);
                  throw(CONFIGURATION_EXCEPTION);
               }
     
               if(range < 0)
                  modelVar->expBins.bin[i].range =
                                          (float)((int)((range*100.0)-.5)/100.0);
               else
                  modelVar->expBins.bin[i].range =
                                          (float)((int)((range*100.0)+.5)/100.0);
            }
         }

         /***********************************************/
         /* Variable must already be on the shared list */
         /***********************************************/
         for(sharedVar=*sharedVarList; sharedVar; sharedVar=sharedVar->next)
         {  if(!NoCaseStrcmp(globVar->varName, sharedVar->varName))
               break;
         }

         /* Error out if variable not shared */
         if(!sharedVar)
         {  sprintf(errMsg, "Explanation variable '%s' must be a previously defined "
                            "shared variable in file '%s'", globVar->varName, fileName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }

         /* Otherwise, set var up with all the shared data */
         globVar->data = sharedVar->data;
         globVar->type = sharedVar->type;
         globVar->dataSize = sharedVar->dataSize;
         globVar->varName = sharedVar->varName;
         tmpMVar = (tModelVar*)globVar->user;
         shareMVar = (tModelVar*)sharedVar->user;

         tmpMVar->func = shareMVar->func;
         tmpMVar->funcCode = shareMVar->funcCode;
         tmpMVar->args = shareMVar->args;
         tmpMVar->isShared = COPY;

         MyFree((void*)tmpMVar->cName);
         tmpMVar->cName = shareMVar->cName;
         MyFree((void*)tmpMVar->cfgName);
         tmpMVar->cfgName = shareMVar->cfgName;
         MyFree((void*)tmpMVar->sasName);
         tmpMVar->sasName = shareMVar->sasName;
      }
   }

   return(0);
}


long LoadModelRule
(tModel *model,  char *buf, char *fileName, FILE *fp, long oldSpot)
{
   int             nbrRule;
   tRuleBaseData   RBData;
   tRuleData       *rule;
   char            errMsg[256], file[FILENAME_LEN+1];

   /* reset file to beginning of the segment */
   fseek(fp, oldSpot, 0);
   GetRcrd(buf, 1024, fp,decrypt);
   if(sscanf(buf, "RUL . %d\n",&nbrRule) != 1)
   {  sprintf(errMsg, "Bad format for 'RUL' = '%s' in '%s'", 
                                            buf, fileName);
      SetConfigErrorMessage(errMsg);
      throw(CONFIGURATION_EXCEPTION);
   }
   else if(nbrRule > 0)
   {  sprintf(file, "%s", fileName); 

      /* Read the rules into a new rule base */
      memset((char*)&RBData, 0, sizeof(tRuleBaseData));
      if(ReadRuleBase(fp, &RBData, nbrRule, file, decrypt) != 0)
      {
         sprintf(errMsg, "Error reading Rulebase in '%s'", fileName);
         SetConfigErrorMessage(errMsg);
         throw(CONFIGURATION_EXCEPTION);
      }

      /* Add the rules to the current model rule base */
      if(RBData.supportRules.sysInitRuleList)
      {  if(model->modelRBData->supportRules.sysInitRuleList)
         {  for(rule=model->modelRBData->supportRules.sysInitRuleList; 
                                                 rule->next; rule=rule->next);
            rule->next = RBData.supportRules.sysInitRuleList;
         }
         else
            model->modelRBData->supportRules.sysInitRuleList = 
                                 RBData.supportRules.sysInitRuleList;

         /* Put the rules in the current section */
         for(rule=RBData.supportRules.sysInitRuleList; rule; rule=rule->next)
            AddToSection(model, RUL_SECTION, (void*)rule);
      }
            
      if(RBData.supportRules.sysTermRuleList)
      {  if(model->modelRBData->supportRules.sysTermRuleList)
         {  for(rule=model->modelRBData->supportRules.sysTermRuleList; 
                                                 rule->next; rule=rule->next);
            rule->next = RBData.supportRules.sysTermRuleList;
         }
         else
            model->modelRBData->supportRules.sysTermRuleList = 
                                 RBData.supportRules.sysTermRuleList;

         /* Put the rules in the current section */
         for(rule=RBData.supportRules.sysTermRuleList; rule; rule=rule->next)
            AddToSection(model, RUL_SECTION, (void*)rule);
      }

      if(RBData.supportRules.iterInitRuleList)
      {  if(model->modelRBData->supportRules.iterInitRuleList)
         {  for(rule=model->modelRBData->supportRules.iterInitRuleList; 
                                                 rule->next; rule=rule->next);
            rule->next = RBData.supportRules.iterInitRuleList;
         }
         else
            model->modelRBData->supportRules.iterInitRuleList = 
                                 RBData.supportRules.iterInitRuleList;

         /* Put the rules in the current section */
         for(rule=RBData.supportRules.iterInitRuleList; rule; rule=rule->next)
            AddToSection(model, RUL_SECTION, (void*)rule);
      }

      if(RBData.supportRules.iterTermRuleList)
      {  if(model->modelRBData->supportRules.iterTermRuleList)
         {  for(rule=model->modelRBData->supportRules.iterTermRuleList; 
                                                 rule->next; rule=rule->next);
            rule->next = RBData.supportRules.iterTermRuleList;
         }
         else
            model->modelRBData->supportRules.iterTermRuleList = 
                                 RBData.supportRules.iterTermRuleList;

         /* Put the rules in the current section */
         for(rule=RBData.supportRules.iterTermRuleList; rule; rule=rule->next)
            AddToSection(model, RUL_SECTION, (void*)rule);
      }

      if(RBData.ruleList)
      {  if(model->modelRBData->ruleList)
         {  for(rule=model->modelRBData->ruleList; rule->next; rule=rule->next);
            rule->next = RBData.ruleList;
         }
         else
            model->modelRBData->ruleList = RBData.ruleList;

         /* Put the rules in the current section */
         for(rule=RBData.ruleList; rule; rule=rule->next)
            AddToSection(model, RUL_SECTION, (void*)rule);
      }
   }

   return(0);
}


long LoadModelTemplate
(tModel *model, char *buf, char *fileName, FILE *fp, long oldSpot, tMasterData *masterData)
{
   int            nbrTemplate, loopCount, nbrFields;
   tDataTemplate  *modTmplts=NULL, *tmplt=NULL;
   char           templateName[100], errMsg[256];
#ifdef DEBUG
char ch;
#endif

   /* reset file to beginning of the segment */
   fseek(fp, oldSpot, 0);
   GetRcrd(buf, 1024, fp,decrypt);
   if(sscanf(buf, "TPL . %d\n",&nbrTemplate) != 1)
   {  sprintf(errMsg, "Bad format for 'TPL' = '%s' in '%s'", 
                                            buf, fileName);
      SetConfigErrorMessage(errMsg);
      throw(CONFIGURATION_EXCEPTION);
   }
   else if(nbrTemplate > 0)
   {
#ifdef DEBUG
fprintf(stderr, "\tTemplate nbr = %d\n",nbrTemplate);
#endif
      loopCount = 0;
      while(loopCount < nbrTemplate)
      {  
         if(GetRcrd(buf,1024,fp,decrypt)==NULL)
         {  sprintf(errMsg, "Failed to read '%s' template name", fileName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }

         if(sscanf(buf, "%s . %d\n",templateName,&nbrFields) != 2)
         {  sprintf(errMsg, "Template name format failure in '%s'", fileName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }

         loopCount++;

         /*********************************************/
         /* if template is specified in the .tab file */
         /*********************************************/
         if(nbrFields != 0)
         {  
            /******************************************************/
            /* Check to see if template is defined in master list */
            /* If so, then it is an error to redefine it here.    */
            /******************************************************/
            if(CheckTemplateExists(model->modelRBData->templateList, templateName))
            {  sprintf(errMsg, "Template %s already exists.  Can't redefine in %s\n",
                                           templateName, fileName);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }
              
            if(modTmplts)
            {  if((tmplt->next = (tDataTemplate*)MyMalloc(sizeof(tDataTemplate)))==NULL)
                  throw(MALLOC_EXCEPTION);
               tmplt = tmplt->next;
            }
            else
            {  if((tmplt = (tDataTemplate*)MyMalloc(sizeof(tDataTemplate)))==NULL)
                  throw(MALLOC_EXCEPTION);
               modTmplts = tmplt;
            }
            memset((char*)tmplt, 0, sizeof(tDataTemplate));

            if((tmplt->templateName = (char *)MyMalloc(strlen(templateName)))==NULL)
               throw(MALLOC_EXCEPTION);
            else
               strcpy(tmplt->templateName, templateName);

            sprintf(buf, "%s", fileName);
            if(GetTemplateFields(fp, tmplt, nbrFields, buf, decrypt) != 0)
            {  sprintf(errMsg, "GetTemplateFields failed for template '%s' in '%s'\n",
                                           templateName, fileName);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }
         }


         /***************************************************************/
         /* If the template is in the master list, continue.  Else, get */
         /* the template from the database.                             */
         /***************************************************************/
         else
         {  if(CheckTemplateExists(model->modelRBData->templateList, templateName))
               continue;

            GetTemplateFromDB(&modTmplts, templateName, masterData);
         }

#ifdef DEBUG
fprintf(stderr, "\tloopCount = %d\n",loopCount);
ch = getchar();
#endif

      }
   }

   if(modTmplts)
   {  for(tmplt=modTmplts; tmplt->next; tmplt=tmplt->next);
      tmplt->next = model->modelRBData->templateList;
      model->modelRBData->templateList = modTmplts;
   }
         
   return(0);
}


#define MAX_BASE_RULES_LEN 4096
long LoadBaseRuleList
(tModel *model, char *buf, char *fileName, FILE *fp, long oldSpot)
{
   int              nbrRules, loopCount, k;
   char             *ptr, errMsg[256];

   /* reset file to beginning of the segment */
   fseek(fp, oldSpot, 0);
#ifdef DEBUG
fprintf(stderr, "\t\toldSpot = %ld\n",oldSpot);
#endif
   GetRcrd(buf,1024,fp,decrypt);
   if(sscanf(buf, "BSR . %d\n",&nbrRules) != 1)
   {  sprintf(errMsg, "Bad format for 'BSR' = '%s' in '%s'", 
                                            buf, fileName);
      SetConfigErrorMessage(errMsg);
      throw(CONFIGURATION_EXCEPTION);
   }
   else if(nbrRules> 0)
   {
#ifdef DEBUG
fprintf(stderr, "\t\tnbrRules = %d\n",nbrRules);
#endif
      loopCount = 0;
      if(model->baseRules)
         memset((char*)model->baseRules, 0, MAX_BASE_RULES_LEN);
      else
      {  if((model->baseRules = (char*)MyMalloc(MAX_BASE_RULES_LEN))==NULL) 
            throw(MALLOC_EXCEPTION);
         memset((char*)model->baseRules, 0, MAX_BASE_RULES_LEN);
      }
      ptr = model->baseRules;

      while(loopCount < nbrRules)
      {  if(GetRcrd(buf,1024,fp,decrypt)==NULL)
         {  sprintf(errMsg, "Failed to read base rule name in '%s'", fileName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }

         k = 0;
         while(isspace(buf[k]) && buf[k]!='\n') k++; 
         if(buf[k] == '\n') continue;

         buf[strlen(buf)-1] = '\0';

         if(loopCount==0)
            sprintf(ptr, "%s", buf);
         else
            sprintf(ptr, ",%s", buf);

         ptr += strlen(ptr);

         loopCount++;
      }
   }

#ifdef DEBUG
fprintf(stderr, "\t\tloopCount = %d\n",loopCount);
#endif

   return(0);
}


long LoadModelConstant
(tModel *model, char *buf, char *fileName, FILE *fp, long oldSpot, tMasterData *masterData)
{
   int              nbrCons, loopCount, k, tokType;
   tEngVar          *globVar=NULL, *tmp=NULL; 
   tRuleBaseData    *ruleBaseData;
   tSiteValue       *siteVal;
   char             token[100], errMsg[256];

   /* reset file to beginning of the segment */
   fseek(fp, oldSpot, 0);
   GetRcrd(buf,1024,fp,decrypt);
   if(sscanf(buf, "CON . %d\n",&nbrCons) != 1)
   {  sprintf(errMsg, "Bad format for 'CON' = '%s' in '%s'", 
                                            buf, fileName);
      SetConfigErrorMessage(errMsg);
      throw(CONFIGURATION_EXCEPTION);
   }
   else if(nbrCons > 0)
   {  loopCount = 0;
      ruleBaseData = model->modelRBData;
      if(ruleBaseData->globalVarList)
      {  for(tmp=ruleBaseData->globalVarList; tmp->next; tmp=tmp->next);
         globVar = tmp;
      }

      while(loopCount < nbrCons)
      {  if(GetRcrd(buf,1024,fp,decrypt)==NULL)
         {  sprintf(errMsg, "Failed to read constant in '%s'", fileName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }

         k = 0;

         while(isspace(buf[k]) && buf[k]!='\n') k++; 
         if(buf[k] == '\n') continue;

         loopCount++;

         /***************************/
         /* Allocate a new constant */
         /***************************/
         if(globVar==NULL)
         {  if((globVar = (tEngVar*)MyMalloc(sizeof(tEngVar)))==NULL)
            {  fprintf(stderr, "\nFailed to allocate globVar\n\n");
               throw(MALLOC_EXCEPTION);
            }
            ruleBaseData->globalVarList = globVar;
         }
         else
         {  if((globVar->next = (tEngVar*)MyMalloc(sizeof(tEngVar)))==NULL)
            {  fprintf(stderr, "\nFailed to allocate globVar->Next\n\n");
               throw(MALLOC_EXCEPTION);
            }
            globVar = globVar->next;
         }
         memset((char*)globVar, 0, sizeof(tEngVar));
         globVar->flags |= VAR_CONSTANT;

         /* Add the constant to the current section */
         AddToSection(model, CON_SECTION, (void*)globVar);

         /*********************/
         /* Get Constant name */
         /*********************/
         k = 0;
         GetWord(buf, &k, token, &tokType);
         if(tokType != WORD_STRING)
         {  sprintf(errMsg, "Bad constant name %s", token);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }
         if((globVar->varName = (char*)MyMalloc(strlen(token)+1))==NULL)
            throw(MALLOC_EXCEPTION);
         strcpy(globVar->varName, token);

         /**********************/
         /* Get Constant Value */
         /**********************/
         GetWord(buf, &k, token, &tokType);

         /* See if referencing other constant */
         if(tokType==WORD_STRING)
         {  for(tmp=ruleBaseData->globalVarList; tmp; tmp=tmp->next)
               if(!NoCaseStrcmp(tmp->varName, token) && tmp->flags&VAR_CONSTANT)
                  break;
         }

         if(tokType==WORD_STRING)
         {  globVar->type = HNC_STRING;

            if(!tmp)
            {  if((globVar->data = (void*)MyMalloc(strlen(token)+1))==NULL)
                  throw(MALLOC_EXCEPTION);
               strcpy((char*)globVar->data, token); 
               globVar->dataSize = strlen(token);
            }
            else
            {  globVar->type = tmp->type;
               if((globVar->data = (void*)MyMalloc(tmp->dataSize))==NULL)
                  throw(MALLOC_EXCEPTION);
               globVar->dataSize = tmp->dataSize;
               memcpy(globVar->data, tmp->data, tmp->dataSize);
            }
         }
         else if(tokType==WORD_DIGIT || tokType==WORD_FLOAT) 
         {  globVar->type = HNC_FLOAT;
            if((globVar->data = (void*)MyMalloc(sizeof(float)))==NULL)
               throw(MALLOC_EXCEPTION);
            globVar->dataSize = sizeof(float);
            *((float*)globVar->data) = (float)atof(token);
         }
         else if(tokType==WORD_HEX)
         {  globVar->type = HNC_INT32;
            if((globVar->data = (void*)MyMalloc(sizeof(long)))==NULL)
               throw(MALLOC_EXCEPTION);
            globVar->dataSize = sizeof(long);
            *((long*)globVar->data) = strtol(token, NULL, 16);
         }
         else
         {  sprintf(errMsg, "Bad constant value '%s' for "
                            "constant '%s'", token, globVar->varName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }

         /***************************************************/
         /* Look up the constant in the siteValues list and */
         /* change its value if it is found.                */
         /***************************************************/
         if(masterData->siteValues)
         {  for(siteVal=masterData->siteValues; siteVal; siteVal=siteVal->next)
            {  if(!NoCaseStrcmp(siteVal->name, globVar->varName))
                  break;
            }

            /* If match found, change the value */
            if(siteVal)
            {  if(globVar->type == HNC_FLOAT)
                  *((float*)globVar->data) = (float)siteVal->data;
               else if(globVar->type == HNC_INT32)
                  *((long*)globVar->data) = (long)siteVal->data;
               else 
               {  sprintf(errMsg, "Constant '%s' for model '%s' is a string "
                                  "constant and cannot be reset by a site "
                                  "specific value", globVar->varName, model->modelName); 
                  SetConfigErrorMessage(errMsg);
                  throw(CONFIGURATION_EXCEPTION);
               }
            }
         }
      }
   }

   return(0);
}


long LoadModelProfInitRB
(tModel *model, char *buf, char *fileName, FILE *fp, long oldSpot, tMasterData *masterData)
{
   int    nbrRule;
   char   errMsg[256], file[FILENAME_LEN+1];

   /* Only one profile initialization section is allowed */
   if(model->modelPInitData)
   {  sprintf(errMsg, "Profile initialization rules (PIR) aleady defined "
                      "for model '%s'", model->modelName);
      SetConfigErrorMessage(errMsg);
      throw(CONFIGURATION_EXCEPTION);
   }

   /* reset file to beginning of the segment */
   fseek(fp, oldSpot, 0);
   GetRcrd(buf, 1024, fp, decrypt);
   if(sscanf(buf, "PIR . %d\n",&nbrRule) != 1)
   {  sprintf(errMsg, "Bad format for 'RUL' = '%s' in '%s'",
                                            buf, fileName);
      SetConfigErrorMessage(errMsg);
      throw(CONFIGURATION_EXCEPTION);
   }

   /* Allocate a rule base data structure for profile initialization */
   if((model->modelPInitData = (tRuleBaseData*)MyMalloc(sizeof(tRuleBaseData)))==NULL)
   {  fprintf(stderr, "\nFailed to allocate prof init rule base\n");
      throw(MALLOC_EXCEPTION);
   }
   memset((char*)model->modelPInitData, 0, sizeof(tRuleBaseData));
   model->modelPInitData->templateList = masterData->templateList;
   model->modelPInitData->funcList = masterData->funcList;

   /* Set the rule base name */
   sprintf(buf, "%s_PROF_INIT_RB", fileName);
   if((model->modelPInitData->ruleBaseName = (char*)MyMalloc(strlen(buf)+1))==NULL)
      throw(MALLOC_EXCEPTION);
   strcpy(model->modelPInitData->ruleBaseName, buf);

   if(nbrRule > 0)
   {  sprintf(file, "%s", fileName);

      if(ReadRuleBase(fp, model->modelPInitData, nbrRule, file, decrypt) != 0)
      {
         sprintf(errMsg, "Error reading Rulebase in '%s'", fileName);
         SetConfigErrorMessage(errMsg);
         throw(CONFIGURATION_EXCEPTION);
      }
   }

   /* Create a section for these rules */
   AddToSection(model, PIR_SECTION, (void*)model->modelPInitData);

   return(0);
}


long SetProfileDefault
(tProfileMap *PRM, char *defaultStr, tModel *model, char* fileName)
{
   tEngVar *var, *prevVar=NULL;
   float   value;
   char    errMsg[256];
   char    newName[256];

   if(isdigit(defaultStr[0]))
   {  if(!IsBinNumeric(PRM->dataType))
      {  sprintf(errMsg, "Profile variable '%s.%s' cannot be initialized with "
                         "numeric value '%s' in '%s' because it is not a numeric type",
                         PRM->prof, PRM->field, defaultStr, fileName);
         SetConfigErrorMessage(errMsg);
         throw(CONFIGURATION_EXCEPTION);
      }

      value = atof(defaultStr);
      for(var=model->modelRBData->globalVarList; var; var=var->next)
      {  if(var->flags&VAR_CONSTANT && 
              var->type==HNC_FLOAT && 
              *((float*)var->data) == value)
            break;
         prevVar = var;
      }

      if(!var)
      {  if((var = (tEngVar*)MyMalloc(sizeof(tEngVar)))==NULL)
            throw(MALLOC_EXCEPTION);
         memset((char*)var, 0, sizeof(tEngVar));

         sprintf(newName, "PRMCON_%ld_%ld", (long)value, (long)(value - (long)value)*1000);
         if((var->varName = (char*)MyMalloc(strlen(newName)+1))==NULL)
            throw(MALLOC_EXCEPTION);
         strcpy(var->varName, newName);

         var->flags |= VAR_CONSTANT;
         switch(PRM->dataType)
         {  case HNC_FLOAT:
               if((var->data = MyMalloc(sizeof(float)))==NULL)
                  throw(MALLOC_EXCEPTION);
               *((float*)var->data) = value;

               var->type = HNC_FLOAT;
               var->dataSize = sizeof(float);
               break;
            case HNC_DOUBLE:
               if((var->data = MyMalloc(sizeof(double)))==NULL)
                  throw(MALLOC_EXCEPTION);
               *((double*)var->data) = (double)value;

               var->type = HNC_DOUBLE;
               var->dataSize = sizeof(double);
               break;
            case HNC_INT32:
               if((var->data = MyMalloc(sizeof(long)))==NULL)
                  throw(MALLOC_EXCEPTION);
               *((long*)var->data) = (long)value;

               var->type = HNC_INT32;
               var->dataSize = sizeof(long);
               break;
            case HNC_INT16:
               if((var->data = MyMalloc(sizeof(short)))==NULL)
                  throw(MALLOC_EXCEPTION);
               *((short*)var->data) = (short)value;

               var->type = HNC_INT16;
               var->dataSize = sizeof(short);
               break;
         }

         if(prevVar) prevVar->next = var;
         else model->modelRBData->globalVarList = var;
      }

      if((PRM->defValue = (tEngVar*)MyMalloc(sizeof(tEngVar)))==NULL)
         throw(MALLOC_EXCEPTION);
      *PRM->defValue = *var;
   }
   else if(defaultStr[0] == '"')
   {  if(!IsString(PRM->dataType))
      {  sprintf(errMsg, "Profile variable '%s.%s' cannot be initialized with "
                         "string %s in '%s' because it is not a string type",
                         PRM->prof, PRM->field, defaultStr, fileName);
         SetConfigErrorMessage(errMsg);
         throw(CONFIGURATION_EXCEPTION);
      }

      defaultStr[strlen(defaultStr)-1] = '\0';
      for(var=model->modelRBData->globalVarList; var; var=var->next)
      {  if(var->flags&VAR_CONSTANT && var->type==HNC_STRING)
         {  if(!NoCaseStrcmp((char*)var->data, defaultStr))
               break;
         }
         prevVar = var;
      }

      if(!var)
      {  if((var = (tEngVar*)MyMalloc(sizeof(tEngVar)))==NULL)
            throw(MALLOC_EXCEPTION);
         memset((char*)var, 0, sizeof(tEngVar));

         if((var->data = MyMalloc(strlen(defaultStr)))==NULL)
            throw(MALLOC_EXCEPTION);
         strcpy((char*)var->data, defaultStr+1);
   
         var->flags |= VAR_CONSTANT;
         var->type = HNC_STRING;
         var->dataSize = strlen((char*)var->data);

         if(prevVar) prevVar->next = var;
         else model->modelRBData->globalVarList = var;
      }

      if((PRM->defValue = (tEngVar*)MyMalloc(sizeof(tEngVar)))==NULL)
         throw(MALLOC_EXCEPTION);
      *PRM->defValue = *var;
   }
   else
   {  for(var=model->modelRBData->globalVarList; var; var=var->next)
         if(VarMatch(var, defaultStr))
            break;
      
      if(!var)
      {  sprintf(errMsg, "Variable '%s' is not defined before it is used as the "
                         "initial value for profile field '%s.%s' in '%s'", 
                           defaultStr, PRM->prof, PRM->field, fileName);
         SetConfigErrorMessage(errMsg);
         throw(CONFIGURATION_EXCEPTION);
      }

      if(CompareType(PRM->dataType, var->type))
      {  sprintf(errMsg, "Cannot set initial value of profile field '%s.%s' "
                         "with variable '%s' int '%s' becuase they don't "
                         "have compatible types",
                           PRM->prof, PRM->field, defaultStr, fileName);
         SetConfigErrorMessage(errMsg);
         throw(CONFIGURATION_EXCEPTION);
      }

      if((PRM->defValue = (tEngVar*)MyMalloc(sizeof(tEngVar)))==NULL)
         throw(MALLOC_EXCEPTION);
      *PRM->defValue = *var;
   }

   return(0);
}


int CheckPRM
(tProfileMap *PRM, tModel *model, tEngVar **sharedVarList, char* fileName)
{
   tDataTemplate *tmplt;
   tDataField    *fld;
   tEngVar       *globVar, *shareVar;
   char          errMsg[256];
   void          *oldData=NULL;

   /*************************************/
   /* Find the template for the profile */
   /*************************************/
   for(tmplt=model->modelRBData->templateList; tmplt; tmplt=tmplt->next)
   {  if(!NoCaseStrcmp(PRM->prof, tmplt->templateName))
      {  
         /***************************************************/
         /* Find the matching field in the profile template */ 
         /***************************************************/
         for(fld=tmplt->fieldList; fld; fld=fld->next)
         {  if(!NoCaseStrcmp(PRM->field, fld->fieldName))
            {  
               /******************************************************************/
               /* Find the variable that is associated with the profile variable */
               /* and set that variable's data to the profile data area          */
               /******************************************************************/
               for(globVar=model->modelRBData->globalVarList; globVar; globVar=globVar->next)
               {  if(VarMatch(globVar, PRM->varName))
                  {  
                     /******************************/ 
                     /* Make sure the type matches */ 
                     /******************************/ 
                     if(globVar->type != fld->type)
                     {  sprintf(errMsg, "Profile variable '%s.%s' cannot be "
                                        "mapped to variable '%s' in '%s' "
                                        "because they have different types", 
                                              PRM->prof, PRM->field, 
                                              fileName, PRM->varName);
                        SetConfigErrorMessage(errMsg);
                        throw(CONFIGURATION_EXCEPTION);
                     }

                     /*******************************************************/
                     /* If is a string, make sure they have the same length */
                     /*******************************************************/
                     if(globVar->type==HNC_STRING && globVar->dataSize!=fld->length)
                     {  sprintf(errMsg, "Profile variable '%s.%s' cannot be "
                                        "mapped to variable '%s' in '%s' "
                                        "because they have different lengths " 
                                        "(%d vs. %ld)",
                                              PRM->prof, PRM->field, 
                                              PRM->varName, fileName,
                                              globVar->dataSize, fld->length);
                        SetConfigErrorMessage(errMsg);
                        throw(CONFIGURATION_EXCEPTION);
                     }

                     /*********************************************************************/
                     /* All profile variables must be shared if there are multiple models */
                     /*********************************************************************/
                     if(!(((tModelVar*)globVar->user)->isShared) && sharedVarList)
                     {  sprintf(errMsg, "Variable '%s' must be shared "
                                        "because it maps to profile field '%s.%s' "
                                        "in '%s'", 
                                             globVar->varName, PRM->prof, 
                                             PRM->field, fileName);
                        SetConfigErrorMessage(errMsg);
                        throw(CONFIGURATION_EXCEPTION);
                     }

                     /**************************************************/
                     /* If variable data is already set to profile     */
                     /* data, then it must have been a shared variable */
                     /* set up that way.  Otherwise, need to set it up */
                     /**************************************************/
                     if(globVar->data != (void*)(tmplt->data + fld->offset))
                     {  if(globVar->data)
                           oldData = globVar->data;
                        globVar->flags |= VAR_NOFREE;
                        globVar->data = tmplt->data + fld->offset;
                        PRM->data = (char*)globVar->data;
                        PRM->dataType = fld->type;
                     }
                     else
                     {  PRM->data = (char*)globVar->data;
                        PRM->dataType = fld->type;
                     }

                     /*******************************************************/
                     /* Find the shared variable in the shared list, if any */
                     /*******************************************************/
                     if(sharedVarList)
                     {  for(shareVar=*sharedVarList; shareVar; shareVar=shareVar->next)
                        {  if(!NoCaseStrcmp(shareVar->varName, globVar->varName))
                           {  if(shareVar->data == oldData) 
                              {  MyFree((void*)oldData);
                                 shareVar->flags |= VAR_NOFREE;
                                 shareVar->data = globVar->data; 
                              }
                              else if(shareVar->data != globVar->data)
                              {  sprintf(errMsg, "Variable '%s' is shared, but is either "
                                                 "not previously mapped to a profile field "
                                                 "or it is mapped to a different profile "
                                                 "field than '%s.%s' in '%s'",
                                                      shareVar->varName, PRM->prof, 
                                                      PRM->field, fileName);
                                 SetConfigErrorMessage(errMsg);
                                 throw(CONFIGURATION_EXCEPTION);
                              }

                              break;
                           }
                        }

                        /****************************************************/
                        /* If the variable is not found, something is wrong */
                        /****************************************************/
                        if(!shareVar)
                        {  sprintf(errMsg, "Could not find variable '%s' in the shared "
                                           "variable list to map to profile field '%s.%s' "
                                           "in '%s'", 
                                                PRM->varName, PRM->prof, 
                                                PRM->field, fileName);
                           SetConfigErrorMessage(errMsg);
                           throw(CONFIGURATION_EXCEPTION);
                        }
                     }
                     else
                        MyFree((void*)oldData);

                     break;
                  }
               }

               if(globVar) break;
  
               sprintf(errMsg, "Variable '%s' does not exist to map "
                               "to profile field '%s.%s' in '%s'", 
                                              PRM->varName, PRM->prof, 
                                              PRM->field, fileName);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }
         }
 
         if(fld) break;

         sprintf(errMsg, "Field '%s' is not defined for profile '%s' in '%s'", 
                                      PRM->field, PRM->prof, fileName);
         SetConfigErrorMessage(errMsg);
         throw(CONFIGURATION_EXCEPTION);
      }
   }

   if(!tmplt)
   {   sprintf(errMsg, "Profile '%s' template is not defined in '%s'", 
                                           PRM->prof, fileName);
      SetConfigErrorMessage(errMsg);
      throw(CONFIGURATION_EXCEPTION);
   }
  
   return(0);
}


long LoadModelProfileMap
(tModel *model, char *buf, char *fileName, FILE *fp, long oldSpot, tEngVar **sharedVarList)
{
   int              nbrPrf, loopCount, k;
   tProfileMap      *profMap=NULL, *tmpPRM;
   char             tmpbuf[256],profileName[100],profileVarName[100],
                    VarName[100],token[256],errMsg[256];
   char             profileInitValue[256];
   int              index,tokenType,PRMReduce,numScans;

   /* Make sure profile map has not already been defined */
   if(model->PRMList)
   {  sprintf(errMsg, "Error Profile Map (PRM) has already been define"
                      "for model '%s'", model->modelName);
      SetConfigErrorMessage(errMsg);
      throw(CONFIGURATION_EXCEPTION);
   }

   /* reset file to beginning of the segment */
   fseek(fp, oldSpot, 0);
   GetRcrd(buf, 1024, fp, decrypt);
   if((numScans=sscanf(buf, "PRM . %d . %d\n",&nbrPrf,&PRMReduce)) < 1)
   {  sprintf(errMsg, "Bad format for 'PRM' = '%s' in '%s'", 
                                            buf, fileName);
      SetConfigErrorMessage(errMsg);
      throw(CONFIGURATION_EXCEPTION);
   }
   else if(nbrPrf > 0)
   {  if(numScans==2)
         model->PRMReduce = PRMReduce;
      else
         model->PRMReduce = 0;
       
      loopCount = 0;
      while(loopCount < nbrPrf)
      {
         if(GetRcrd(buf,1024,fp,decrypt)==NULL)
         {  sprintf(errMsg, "Failed to read profile map in '%s'", fileName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }

         k = 0;
         while(isspace(buf[k]) && buf[k]!='\n') k++; 
         if(buf[k] == '\n') continue;

         loopCount++;

         if(profMap)
         {  if((profMap->next=(tProfileMap*)MyMalloc(sizeof(tProfileMap)))==NULL)
               throw(MALLOC_EXCEPTION);
             profMap = profMap->next;
         }
         else
         {  if((profMap=(tProfileMap*)MyMalloc(sizeof(tProfileMap)))==NULL)
               throw(MALLOC_EXCEPTION);
             model->PRMList = profMap;
         }
         memset((char*)profMap, 0, sizeof(tProfileMap));
   
         if(sscanf(buf, "%s %s %s", tmpbuf, VarName, profileInitValue)!=3)
         {  sprintf(errMsg, "Incorrect number of columns in profile map "
                            "at line '%d' in file '%s'", loopCount, fileName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }

         index = 0;
         GetWord(tmpbuf, &index, token, &tokenType);
         if(tokenType == WORD_STRING)
         {
            strcpy(profileName,token);
            GetWord(tmpbuf, &index, token, &tokenType);
            if(tokenType == WORD_DELIM && !strcmp(token,"."))
            {
               GetWord(tmpbuf, &index, token, &tokenType);
               if(tokenType == WORD_STRING)
                  strcpy(profileVarName,token);
               else
               {  sprintf(errMsg, "Bad profile template name format at line "
                                  "'%d' in PRM in file '%s'", loopCount, fileName);
                  SetConfigErrorMessage(errMsg);
                  throw(CONFIGURATION_EXCEPTION);
               }
            } 
            else
            {  sprintf(errMsg, "Bad profile field name format at line "
                               "'%d' in PRM in file '%s'", loopCount, fileName);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }
         }
         else
         {  sprintf(errMsg, "Bad profile map initial value field for '%s.%s' "
                            "at line '%d' in PRM in file '%s'", 
                             profileName, profileVarName, loopCount, fileName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }

         if((profMap->prof = 
            (char *)MyMalloc(strlen(profileName)+1)) == NULL)
            throw(MALLOC_EXCEPTION);
         strcpy(profMap->prof,profileName);

         if((profMap->field = 
            (char *)MyMalloc(strlen(profileVarName)+1)) == NULL)
            throw(MALLOC_EXCEPTION);
         strcpy(profMap->field,profileVarName);

         if((profMap->varName = 
            (char *)MyMalloc(strlen(VarName)+1)) == NULL)
            throw(MALLOC_EXCEPTION);
         strcpy(profMap->varName,VarName);

         /******************************************************/
         /* Make sure this profile field is not already mapped */
         /******************************************************/
         for(tmpPRM=model->PRMList; tmpPRM; tmpPRM=tmpPRM->next)
         {  if(tmpPRM==profMap) break;
            else if(!NoCaseStrcmp(tmpPRM->prof, profMap->prof) && 
                    !NoCaseStrcmp(tmpPRM->field, profMap->field)) 
            {  sprintf(errMsg, "Profile field '%s.%s' is multiply mapped in '%s'", 
                                profMap->prof, profMap->field, fileName);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }
            else if(!NoCaseStrcmp(tmpPRM->varName, profMap->varName)) 
            {  sprintf(errMsg, "Variable '%s' is mapped to both profile field '%s.%s' "
                               "and field '%s.%s in '%s'", 
                                profMap->varName, profMap->prof, profMap->field, 
                                tmpPRM->prof, tmpPRM->field,
                                fileName);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }
         }

         CheckPRM(profMap, model, sharedVarList, fileName);
         SetProfileDefault(profMap, profileInitValue, model, fileName);
      }
   }

   /* Create a section for the profile map */
   AddToSection(model, PRM_SECTION, (void*)model->PRMList);

   return(0);
}


#define MAX_EXT_NAME_LEN  10240 
long LoadExternalData
(tModel *model, char *buf, char *fileName, FILE *fp, long oldSpot, int *loaded, 
  tEngVar **sharedVars, tMasterData *masterData)
{
   int    nbrExt, loopCount, k;
   char   errMsg[256], file[FILENAME_LEN+1];
   char   *ptr, extNames[MAX_EXT_NAME_LEN], *storeExt;
   FILE   *extFp;

   /* reset file to beginning of the segment */
   fseek(fp, oldSpot, 0);
   GetRcrd(buf,1024,fp,decrypt);
   if(sscanf(buf, "EXT . %d\n",&nbrExt) != 1)
   {  sprintf(errMsg, "Bad format for 'EXT' = '%s' in '%s'", 
                                            buf, fileName);
      SetConfigErrorMessage(errMsg);
      throw(CONFIGURATION_EXCEPTION);
   }
   else if(nbrExt> 0)
   {  loopCount = 0;

      ptr = extNames;
      ptr[0] = '\0';
      while(loopCount < nbrExt)
      {  if(GetRcrd(buf,1024,fp,decrypt)==NULL)
         {  sprintf(errMsg, "Failed to external file name in '%s'", fileName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }

         k = 0;
         while(isspace(buf[k]) && buf[k]!='\n') k++; 
         if(buf[k] == '\n') continue;

         /* Remove the newline character */
         buf[strlen(buf)-1] = '\0';
          
         /* Open the file */
         sprintf(file, "%s%s", model->modelPath, buf);
         if((extFp=SafeFopen(file))==NULL)
         {  sprintf(errMsg, "Failed to open external file '%s' for reading", file);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }

         /* Scan the external file */
         ScanFile(buf, extFp, model, loaded, sharedVars, masterData); 
         fclose(extFp);

         /* Add the file to the list of file names for this section */
         if(ptr[0] == '\0') sprintf(ptr, "%s", buf);
         else 
         {  /* Make sure won't overrun the buffer */ 
            if(strlen(extNames) + strlen(buf) > MAX_EXT_NAME_LEN)
            {  sprintf(errMsg, "External file list in '%s' is too long.  Failed "
                               "at '%s'.  Break up the list", file, buf);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }

            sprintf(ptr, ", %s", buf);
         }
         ptr += strlen(ptr);

         loopCount++;
      }
   }

   /* Add the file name to the current section */
   if((storeExt = (char*)MyTmpMalloc(strlen(extNames)+1))==NULL)
      throw(MALLOC_EXCEPTION);
   strcpy(storeExt, extNames);
   AddToSection(model, EXT_SECTION, (void*)storeExt);

   return(0);
}


long ScanFile
(char *fileName, FILE *fp, tModel *model, int *loaded, 
  tEngVar **sharedVars, tMasterData *masterData)
{
   long        oldSpot;
   char        buf[1024], errMsg[256];
   int         needSection=1, i;
   tTabSection *section=NULL; 

   while(1)
   {  /* save offset of each segment */
      oldSpot = ftell(fp);

      /* read first line */
      if((GetRcrd(buf,1024,fp,decrypt))==NULL)
         break;

      /* Allocate a new section */
      if(needSection)
      {  if(model->sectionList)
         {  for(section=model->sectionList; section->next; section=section->next);
            if((section->next = (tTabSection*)MyTmpMalloc(sizeof(tTabSection)))==NULL)
               throw(MALLOC_EXCEPTION);
            section = section->next;  
         }
         else
         {  if((section = (tTabSection*)MyTmpMalloc(sizeof(tTabSection)))==NULL)
               throw(MALLOC_EXCEPTION);
            model->sectionList = section;
         }
         memset((char*)section, 0, sizeof(tTabSection));
         needSection = 0;

         /* Set the file name */
         strcpy(section->fileName, fileName);
      }

      /* Check for TYP segment */
      if(memcmp(buf, "TYP", 3) == 0)
      {  /* Load the model types */
         if(LoadModelTypes(model, buf, fileName, fp, oldSpot) != 0)
         {  sprintf(errMsg, "Failed to load types for model '%s'", model->modelName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }
         *loaded |= MODEL_TYPES;

         needSection = 1;
      }

      /* Check for VAR segment */
      else if(memcmp(buf, "VAR", 3) == 0)
      {  /* Load the variables */
         if(LoadModelVariable(model, sharedVars, buf, fileName, fp, oldSpot) != 0)
         {  sprintf(errMsg, "Failed to load variables for model '%s'", model->modelName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }
         *loaded |= MODEL_VARS;

         needSection = 1;
      }

      /* Check for XPL segment */
      else if(memcmp(buf, "XPL", 3) == 0)
      {  /* This section is only allowed if there is a shared variable list */ 
         if(!sharedVars)
         {  sprintf(errMsg, "Model '%s' in '%s' cannot have an XPL section "
                            "because it is a stand-alone model. ", 
                            model->modelName, fileName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }

         /* Load the variables */
         if(LoadModelExplainVars(model, sharedVars, buf, fileName, fp, oldSpot) != 0)
         {  sprintf(errMsg, "Failed to load variable explanations "
                            "for model '%s'", model->modelName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }
         *loaded |= MODEL_XPL;

         needSection = 1;
      }

      /* Check for rule definition segment */
      else if(memcmp(buf, "RUL", 3) == 0)
      {  if(LoadModelRule(model,buf,fileName,fp,oldSpot) != 0)
         {  sprintf(errMsg, "Failed to load rules for model '%s'", model->modelName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }
         *loaded |= MODEL_RULES;

         needSection = 1;
      }

      /* Check for constant definitions */
      else if(memcmp(buf, "CON", 3) == 0)
      {  if(LoadModelConstant(model,buf,fileName,fp,oldSpot,masterData) != 0)
         {  sprintf(errMsg, "Failed to load constants for model '%s'", model->modelName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }
         *loaded |= MODEL_CONSTS;

         needSection = 1;
      }

      /* Check for profile map */
      else if(memcmp(buf, "PRM", 3) == 0)
      {  if(LoadModelProfileMap(model,buf,fileName,fp,oldSpot,sharedVars) != 0)
         {  sprintf(errMsg, "Failed to load profile map for model '%s'", model->modelName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }
         *loaded |= MODEL_PROFMAP;

         needSection = 1;
      }

      /* Check for profile initialization rule base */
      else if(memcmp(buf, "PIR", 3) == 0)
      {  if(LoadModelProfInitRB(model,buf,fileName,fp,oldSpot,masterData) != 0)
         {  sprintf(errMsg, "Failed to load profile init rule base "
                            "for model '%s'", model->modelName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }
         *loaded |= MODEL_PINITRB;

         needSection = 1;
      }

      /* Check for a list of external files */
      else if(memcmp(buf, "EXT", 3) == 0)
      {  if(LoadExternalData(model,buf,fileName,fp,
                        oldSpot,loaded,sharedVars,masterData) != 0)
         {  sprintf(errMsg, "Failed to load external data "
                            "for model '%s'", model->modelName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }
         *loaded |= MODEL_EXT;

         needSection = 1;
      }

      /* Check for dynamic table definition */
      else if(memcmp(buf, "TBL", 3) == 0)
      {  tTableTemplate *table, *tableList;
     
         tableList = NULL;
         if(LoadTables(&tableList, buf, fp, oldSpot, 
                      fileName, &model->modelRBData->globalTypeList) != 0)
         {  sprintf(errMsg, "Failed to load data tables for "
                            "model '%s'", model->modelName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }
         *loaded |= MODEL_TABLES;

         /* Set the tables in the current section */
         for(table=tableList; table; table=table->next)
            AddToSection(model, TAB_SECTION, table);

         /* Add the tables to the master list.  Check for dups as well */
         if(model->tableList)
         {  tTableTemplate *tbl, *prevTbl=NULL;

            for(tbl=model->tableList; tbl; tbl=tbl->next)
            {  for(table=tableList; table; table=table->next)
               {  if(!NoCaseStrcmp(tbl->tableName, table->tableName))
                  {  sprintf(errMsg, "Table '%s' cannot be redefined for model "
                                     "'%s' in '%s'", 
                                     table->tableName, model->modelName, fileName);
                     SetConfigErrorMessage(errMsg);
                     throw(CONFIGURATION_EXCEPTION);
                  }
               }

               prevTbl = tbl;
            }

            prevTbl->next = tableList;
         }
         else
            model->tableList = tableList;

         needSection = 1;
      }

      /* Check for a list of base rules */
      else if(memcmp(buf, "BSR", 3) == 0)
      {  if(LoadBaseRuleList(model,buf,fileName,fp,oldSpot) != 0)
         {  sprintf(errMsg, "Failed to load base rule list "
                            "for model '%s'", model->modelName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }
         *loaded |= MODEL_BASERUL;

         /* Don't store this section.  Reset the comments */
         section->comments[0] = '\0';
      }

      /* Check for template definitions */
      else if(memcmp(buf, "TPL", 3) == 0)
      {  if(LoadModelTemplate(model,buf,fileName,fp,oldSpot,masterData) != 0)
         {  sprintf(errMsg, "Failed to load templates for model '%s'", model->modelName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }
         *loaded |= MODEL_TMPLTS;

         /* Don't store this section.  Reset the comments */
         section->comments[0] = '\0';
      }

      /* Check for spaces, comments, or unknown segments */
      else if(buf[0] == '&')
      {  if(strlen(section->comments) + strlen(buf) < COMMENTS_LEN)
            strcat(section->comments, buf);
         else
         {  memcpy(section->comments+strlen(section->comments), 
                       buf, (COMMENTS_LEN - strlen(section->comments) - 1));
            section->comments[COMMENTS_LEN-1] = '\0';
         }
         continue;
      }
      else if(buf[0] == '#')
      {  if(strlen(section->comments) + strlen(buf) < COMMENTS_LEN)
            strcat(section->comments, buf);
         else
         {  memcpy(section->comments+strlen(section->comments), 
                       buf, (COMMENTS_LEN - strlen(section->comments) - 1));
            section->comments[COMMENTS_LEN-1] = '\0';
         }
         continue;
      }
      else
      {  i=0;
         while(isspace(buf[i]) && buf[i]!='\0') i++;

         if(buf[i]!='\0')
         {  sprintf(errMsg, "Unknown Segment in '%s' at '%s'", fileName, buf);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }
      }
   }

   return(0);
}
