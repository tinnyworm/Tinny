/*
** $Id: modfunc.c,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: modfunc.c,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.2  1997/01/17  17:22:36  can
 * Updates for latest release
 *
**
*/


/* System include files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

/* HNC Rule Engine includes */
#include "mymall.h"

/* HNC common includes */
#include "modfunc.h"
#include "varcalc.h"
#include "except.h"
#include "hncrtyp.h"
#include "dates.h"
#include "mgmodel.h"
#include "rulfunc.h"

/* Product includes */
#include "prdmfnc.h"



typedef struct sReasDiff {
   short reason;
   short group;
   float diff;
   tModelVar *modVar;
   float sclVal;
   float expScore;
} tReasDiff;


#define MAX_VARS      5000
#define MAX_REASONS     10 
long Reasonator(tModel *model, float score, tReasDiff *diffs, int numBins)
{
   int          i, j, r, save, shift, use;
   tReasDiff    tmp, tmp2;
   tNetInput    *input;
   float        diff, scaledIn[MAX_VARS];

   /*************************/
   /* Get the scaled values */
   /*************************/
   for(i=0; i<model->pNet->numScaled; i++)
   {  scaledIn[i] =
       (model->pNet->aScaled[i]>0)?
       (float)((long)((model->pNet->aScaled[i]+.0099999)*100.0))/100.0:
       (float)((long)(model->pNet->aScaled[i]*100.0))/100.0;
   }

   /*********************/
   /* Clear the reasons */
   /*********************/
   for(r=0; r<numBins; r++)
   {  diffs[r].diff = 9999999.0; 
      diffs[r].group = 0;
      diffs[r].reason = 0;
   }

   /*************************************************************************/
   /* Loop through the inputs and find out which bin the scaled value falls */
   /* into to get the expected output value for that bin                    */
   /*************************************************************************/
   for(i=0, input=model->inputList; input; input=input->next, i++)
   {  for(j=0; j<input->modelVar->expBins.numBins; j++)
      {  if((scaledIn[i] <= input->modelVar->expBins.bin[j].range) || 
             (j==input->modelVar->expBins.numBins-1))
         {  diff = (float)fabs((double)(score - input->modelVar->expBins.bin[j].expect));

            /* Insert reason, if any */
            r=0; shift=1; use=1;
            while(r < numBins)
            {  if(diff < diffs[r].diff)
               {  if(diffs[r].group == input->modelVar->explainGrp) shift = 0; 
                  else shift = 1;

                  /* Remove any other matching reasons in the top three */
                  save = r;
                  while(r < numBins)
                  {  if(diffs[r].group == input->modelVar->explainGrp)
                     {  diffs[r].diff = 9999999.0; 
                        diffs[r].group = 0;
                        diffs[r].reason = 0;
                     }

                     r++;
                  }

                  r = save;
                  break;
               }
               else if(diffs[r].group == input->modelVar->explainGrp)
               {  use = 0;
                  break;
               }

               r++;
            }

            if(r<numBins && use)
            {  tmp = diffs[r];
               diffs[r].diff = diff;
               diffs[r].group = input->modelVar->explainGrp; 
               diffs[r].reason = input->modelVar->explainVal; 
               diffs[r].modVar = input->modelVar; 

               if(shift)
               {  r++;
                  while(r<numBins)
                  {  if(tmp.diff < diffs[r].diff)
                     {  tmp2 = diffs[r];
                        diffs[r] = tmp;
                        tmp = tmp2;
                        r++;
                     }
                     else break;
                  }
               }
            }
             
            break;
         }
      }
   }

   return(0);
}


tAtom GetReasons(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tReasDiff    diffs[MAX_REASONS];
   tAtom        retVal;
   int          r;
   float        score;
   tModelManage *modelManage = 
      (tModelManage*)((tFuncInit*)funcCallData->inputData)->modelHandle;

   /****************************************************/
   /* Make sure the  current model is set to something */
   /****************************************************/
   if(modelManage)
   {  if(!modelManage->currModel)
      {  ZERO(retVal.data.num)
         retVal.type = NUMERIC_TYPE;
         return(retVal);
      }
      else if(modelManage->currModel->byPassNet)
      {  ZERO(retVal.data.num)
         retVal.type = NUMERIC_TYPE;
         return(retVal);
      }
   }

   /**********************************************/
   /* Call Reasonator with the raw network score */ 
   /**********************************************/
   score = (modelManage->currModel->pNet->output / 500) - 1;
   Reasonator(modelManage->currModel, score, diffs, numArgs);

   /*******************/
   /* Set the reasons */
   /*******************/
   for(r=0; r<numArgs; r++)
      *((short*)aParams[r].data.uVal) = diffs[r].reason;

   ZERO(retVal.data.num);
   retVal.type = NUMERIC_TYPE;

   return(retVal);
}


tAtom GetReasonsValue(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tReasDiff    diffs[MAX_REASONS];
   tAtom        retVal;
   int          r;
   float        score;
   tModelManage *modelManage = 
      (tModelManage*)((tFuncInit*)funcCallData->inputData)->modelHandle;

   /****************************************************/
   /* Make sure the  current model is set to something */
   /****************************************************/
   if(modelManage)
   {  if(!modelManage->currModel)
      {  ZERO(retVal.data.num)
         retVal.type = NUMERIC_TYPE;
         return(retVal);
      }
      else if(modelManage->currModel->byPassNet)
      {  ZERO(retVal.data.num)
         retVal.type = NUMERIC_TYPE;
         return(retVal);
      }
   }

   /*************************************************/
   /* Call Reasonator with the first argument value */
   /*************************************************/
   score = NUM_GETFLT(aParams[0].data.num);
   Reasonator(modelManage->currModel, score, diffs, numArgs-1);

   /*******************/
   /* Set the reasons */
   /*******************/
   for(r=1; r<numArgs; r++)
      *((short*)aParams[r].data.uVal) = diffs[r-1].reason;

   ZERO(retVal.data.num);
   retVal.type = NUMERIC_TYPE;

   return(retVal);
}


long GetVarData(void **data, long *dataSize, tEngVar *var)
{
   static float  fltVal;

   switch(var->type)
   {  case NUMERIC_TYPE:
         fltVal = NUM_GETFLT((*((tEngNum*)var->data)));
         *data = (void*)&fltVal;
         break;
      case HNC_FLOAT:
         fltVal = *((float*)var->data);
         *data = (void*)&fltVal;
         break;
      case HNC_DOUBLE:
         fltVal = (float)*((double*)var->data);
         *data = (void*)&fltVal;
         break;
      case HNC_INT16:
         fltVal = (float)*((short*)var->data);
         *data = (void*)&fltVal;
         break;
      case HNC_UINT16:
         fltVal = (float)*((unsigned short*)var->data);
         *data = (void*)&fltVal;
         break;
      case HNC_INT32:
         fltVal = (float)*((long*)var->data);
         *data = (void*)&fltVal;
         break;
      case HNC_UINT32:
         fltVal = (float)*((unsigned long*)var->data);
         *data = (void*)&fltVal;
         break;
      default:
         *data = var->data;
         *dataSize = var->dataSize;
         break;
   }

   return(0);
}


tAtom ProfInit(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom        retVal;
   tProfileMap  *profMap;
   tModelVar    *modelVar;
   void         *data;
   long         dataSize;
   tModelManage *modelManage = 
      (tModelManage*)((tFuncInit*)funcCallData->inputData)->modelHandle;


   for(profMap=modelManage->currModel->PRMList; profMap; profMap=profMap->next)
   {  if(profMap->defValue->flags&VAR_CONSTANT)
         GetVarData(&data, &dataSize, profMap->defValue);
      else if(profMap->defValue->calcRule) 
      {  if(!(*profMap->defValue->calcRule->calcDone))
         {  if((CalcVarRule(profMap->defValue, modelManage->currModel->modelRuleHandle)))
            {  ZERO(retVal.data.num);
               retVal.type = NUMERIC_TYPE;
               return retVal;
            }
         }
         GetVarData(&data, &dataSize, profMap->defValue);
      }
      else if(profMap->defValue->user)
      {  modelVar = (tModelVar*)profMap->defValue->user;
         if(!*(modelVar->calcDone) && modelVar->func)
         {  modelVar->func(profMap->defValue, modelVar);
            *(modelVar->calcDone) = 1;
         }
         GetVarData(&data, &dataSize, profMap->defValue);
      }
  
      switch(profMap->dataType)
      {  case NUMERIC_TYPE:
            NUM_SETFLT((*((tEngNum*)profMap->data)), *((float*)data))
            break;
         case HNC_FLOAT:
            *((float*)profMap->data) = *((float*)data);
            break;
         case HNC_DOUBLE:
            *((double*)profMap->data) = (double)*((float*)data);
            break;
         case HNC_INT16:
            *((short*)profMap->data) = (short)*((float*)data);
            break;
         case HNC_UINT16:
            *((unsigned short*)profMap->data) = (unsigned short)*((float*)data);
            break;
         case HNC_INT32:
            *((long*)profMap->data) = (long)*((float*)data);
            break;
         case HNC_UINT32:
            *((unsigned long*)profMap->data) = (unsigned long)*((float*)data);
            break;
         default:
            memcpy(profMap->data, (char*)data, dataSize);
            break;
      }
   }
   
   ZERO(retVal.data.num);
   retVal.type = NUMERIC_TYPE;

   return(retVal);
}


tAtom DumpData(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   static char  fileNames[20][256];
   static FILE  *fps[20];
   static int   numFiles=0;
   FILE         *fp=NULL; 
   int          f, i;
   tAtom        retVal;
   tNetInput    *input;
   tModelManage *modelManage = 
      (tModelManage*)((tFuncInit*)funcCallData->inputData)->modelHandle;

   ONE(retVal.data.num);

   if(numFiles > 0)
   {  for(f=0; f<numFiles; f++)
         if(!strcmp(aParams[0].data.strVal.str, fileNames[f]))
         {  fp = fps[f];
            break;
         }

      if(!fp && numFiles < 20)
      {  if((fps[numFiles] = fopen(aParams[0].data.strVal.str, "w+"))==NULL)
         {  ZERO(retVal.data.num);
            return(retVal);
         }

         strcpy(fileNames[f], aParams[0].data.strVal.str);
         fp = fps[numFiles];
         numFiles++;
      }
      else if(!fp)
      {  fclose(fps[0]);

         if((fps[0] = fopen(aParams[0].data.strVal.str, "w+"))==NULL)
         {  ZERO(retVal.data.num);
            return(retVal);
         }

         strcpy(fileNames[0], aParams[0].data.strVal.str);
         fp = fps[0];
         numFiles++;
      }
   }
   else
   {  if((fps[0] = fopen(aParams[0].data.strVal.str, "w+"))==NULL)
      {  ZERO(retVal.data.num);
         return(retVal);
      }

      strcpy(fileNames[0], aParams[0].data.strVal.str);
      fp = fps[0];
      numFiles++;
   }

   /****************************************/
   /* Output the net inputs to a flat file */
   /****************************************/
   for(i=0, input=modelManage->currModel->inputList; input; input=input->next, i++)
   {  switch(input->type)
      {  case  HNC_STRING:
            fprintf(fp, "%*.*s", input->dataSize, input->dataSize, (char*)input->data);
            break;
         default :
            if((float)(*((float*)input->data) - (long)*((float*)input->data))==0.0)
               fprintf(fp, "%ld ", (long)*((float*)input->data));
            else
               fprintf(fp, "%.4f ", *((float*)input->data));
            break;
      }
   }
   fprintf(fp, "\n");
   fflush(fp);
   
   return(retVal);
}


#ifdef MODELING
#include "modout.h"
#endif
tAtom ScoreModel(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   int          i, s=0;
   char         strInput[4096];
   tNetInput    *input;
   tAtom        retVal;
   tModelManage *modelManage = 
      (tModelManage*)((tFuncInit*)funcCallData->inputData)->modelHandle;

   /************************************************/
   /* Make sure the current model set to something */
   /************************************************/
   if(modelManage->currModel==NULL)
   {  ZERO(retVal.data.num)
      retVal.type = NUMERIC_TYPE;
      return(retVal);
   }


#ifdef MODELING
   /* Output data for modeling */
   OutModelData(modelManage->currModel);
#endif
      
   if(modelManage->currModel->byPassNet)
   {  ZERO(retVal.data.num)
      retVal.type = NUMERIC_TYPE;
      return(retVal);
   }

   /*********************************************************/
   /* Loop through net input data table, setting the inputs */
   /*********************************************************/
   for(i=0, input=modelManage->currModel->inputList; input; input=input->next, i++)
   {  if(input->type == HNC_STRING)
      {  modelManage->currModel->pNet->aInputData[i].type = SYMBOLIC;

         /* Have to null terminate the input, and there may not */
         /* be memory to do it, so copy into a buffer.          */
         memcpy(strInput+s, (char*)input->data, input->dataSize);
         *(strInput+s+input->dataSize) = '\0';
  
         modelManage->currModel->pNet->aInputData[i].data.str = strInput+s;
         s += input->dataSize+1;
      }
      else
      {  modelManage->currModel->pNet->aInputData[i].type = NUMERIC;
         modelManage->currModel->pNet->aInputData[i].data.num = 
                                                  *((float*)input->data);
      }
   }

   /****************************************/
   /* Crank the network to yield the score */
   /****************************************/
   if(IterOptiNet(modelManage->currModel->pNet))
   {  ZERO(retVal.data.num)
      retVal.type = NUMERIC_TYPE;

      /* SET UP AN ERROR!! */
      return(retVal);
   }

   /****************************/
   /* Get and return the score */
   /****************************/
   NUM_SETFLT(retVal.data.num, modelManage->currModel->pNet->output)
   retVal.type = NUMERIC_TYPE;

   return(retVal);
}


tAtom CalcRisk(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom  retVal;
   float  risk;
   float  numGood = NUM_GETFLT(aParams[0].data.num);
   float  numBad = NUM_GETFLT(aParams[1].data.num);
   float  popGood = NUM_GETFLT(aParams[2].data.num);
   float  popBad = NUM_GETFLT(aParams[3].data.num);
   float  totRisk = NUM_GETFLT(aParams[4].data.num);

   risk = 100 *
         (numBad + (totRisk * (popBad/(popBad + popGood))) + 1) /
         (numBad + numGood + totRisk + 2);
    
   NUM_SETFLT(retVal.data.num, risk);
   retVal.type = NUMERIC_TYPE;

   return(retVal);
}


#define SQR(X) (X)*(X)
tAtom CalcZ(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom  retVal;
   float  zscore;
   float  value = NUM_GETFLT(aParams[0].data.num);
   float  num = NUM_GETFLT(aParams[1].data.num);
   float  avg = NUM_GETFLT(aParams[2].data.num);
   float  std = NUM_GETFLT(aParams[3].data.num);
   float  popNum = NUM_GETFLT(aParams[4].data.num);
   float  popAvg = NUM_GETFLT(aParams[5].data.num);
   float  popStd = NUM_GETFLT(aParams[6].data.num);
   float  combAvg, sqr1, sqr2;

   combAvg = ((num * avg + popNum * popAvg)/(num + popNum));
   sqr1 = (SQR(avg) + SQR(std));
   sqr2 = (SQR(popAvg) + SQR(popStd));

   zscore = (value - combAvg) /
      sqrt(((num * sqr1 + popNum * sqr2)/(num + popNum)) - (SQR(combAvg)));

   NUM_SETFLT(retVal.data.num, zscore);
   retVal.type = NUMERIC_TYPE;

   return(retVal);
}


#ifdef TANDEM
#define erf(x) (1)
#endif

#define SQRT2  1.4142
tAtom ProbNorm(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom  retVal;
   double zscore, result; 
   
   zscore = NUM_GETDBL(aParams[0].data.num) / SQRT2;

   if(zscore >= 0.0)
      result = 0.5 + 0.5 * erf(zscore);
   else
      result = 0.5 - 0.5 * erf(-zscore);
 
   NUM_SETDBL(retVal.data.num, result);

   retVal.type = NUMERIC_TYPE;
   return(retVal);
}


tAtom GetGMTOffset(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom eval;
   float offset;
   char  zip[2];

   eval.type = NUMERIC_TYPE;
   ZERO(eval.data.num);

   if(isdigit(aParams[1].data.strVal.str[0]) && 
      isdigit(aParams[1].data.strVal.str[1]))
      memcpy(zip,aParams[1].data.strVal.str,2);
   else if(isdigit(aParams[0].data.strVal.str[0]) && 
           isdigit(aParams[0].data.strVal.str[1]))
      memcpy(zip,aParams[0].data.strVal.str,2);
   else
      return(eval);

   if(memcmp(zip,"96",2) == 0)
      offset = -10.0;
   else if(memcmp(zip,"99",2) == 0)
      offset = -9.0;
   else if(memcmp(zip,"89",2) >= 0)
      offset = -8.0;
   else if(memcmp(zip,"80",2) >= 0 || memcmp(zip,"59",2) == 0)
      offset = -7.0;
   else if(memcmp(zip,"50",2) >= 0 || memcmp(zip,"38",2) == 0 ||
      memcmp(zip,"39",2) == 0 )
      offset = -6.0;
   else if(memcmp(zip,"00",2) >= 0)
      offset = -5.0;
   else
      offset = 0.0;

   NUM_SETFLT(eval.data.num, offset);
   return(eval);
}


void ProfInitCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));

   AllocFuncName(&funcElem->functionName, "Prof_Init");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 0;
   funcElem->paramTypeList = NULL;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr=ProfInit;
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


void ScoreModelCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));

   AllocFuncName(&funcElem->functionName, "Score_Model");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 0;
   funcElem->paramTypeList = NULL;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr=ScoreModel;
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


void DumpDataCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (1,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (1,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Dump_Data");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 1;
   typeElem[0].type = HNC_STRING;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = 0;
   funcElem->FuncPtr=DumpData;
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


void GetReasonsValueCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (2,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (2,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Get_Reasons_Value");
   funcElem->unlimitedParams = 1;
   funcElem->numParams = 2;
   typeElem[0].type = NUMERIC_TYPE;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = &paramList[1];
   typeElem[1].type = HNC_INT16;
   typeElem[1].next = NULL;
   paramList[1].reference = 1;
   paramList[1].list = &typeElem[1];
   paramList[1].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = 0;
   funcElem->FuncPtr=GetReasonsValue;
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


void GetReasonsCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (1,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (1,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Get_Reasons");
   funcElem->unlimitedParams = 1;
   funcElem->numParams = 1;
   typeElem[0].type = HNC_INT16;
   typeElem[0].next = NULL;
   paramList[0].reference = 1;
   paramList[0].list = &typeElem[0];
   paramList[0].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = 0;
   funcElem->FuncPtr=GetReasons;
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


void CalcVarsCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));

   AllocFuncName(&funcElem->functionName, "Calc_Vars");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 0;
   funcElem->paramTypeList = NULL;
   funcElem->returnType = 0;
   funcElem->FuncPtr=CalcVars;
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


void CalcSubListCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (1,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (1,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Calc_Sub_List");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 1;

   typeElem[0].type = HNC_STRING;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr = CalcSubList;
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


void ZeroSubListCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (1,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (1,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Zero_Sub_List");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 1;

   typeElem[0].type = HNC_STRING;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr = ZeroSubList;
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


void GetGMTOffsetCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (2,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (2,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Get_GMT_Offset");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 2;

   typeElem[0].type = HNC_STRING;
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
   funcElem->FuncPtr = GetGMTOffset;
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


void ProbNormCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (1,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (1,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Prob_Norm");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 1;

   typeElem[0].type = NUMERIC_TYPE;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr = ProbNorm;
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


void CalcRiskCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (5,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (5,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Calc_Risk");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 5;

   typeElem[0].type = NUMERIC_TYPE;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = paramList+1;

   typeElem[1].type = NUMERIC_TYPE;
   typeElem[1].next = NULL;
   paramList[1].reference = 0;
   paramList[1].list = &typeElem[1];
   paramList[1].next = paramList+2;

   typeElem[2].type = NUMERIC_TYPE;
   typeElem[2].next = NULL;
   paramList[2].reference = 0;
   paramList[2].list = &typeElem[2];
   paramList[2].next = paramList+3;

   typeElem[3].type = NUMERIC_TYPE;
   typeElem[3].next = NULL;
   paramList[3].reference = 0;
   paramList[3].list = &typeElem[3];
   paramList[3].next = paramList+4;

   typeElem[4].type = NUMERIC_TYPE;
   typeElem[4].next = NULL;
   paramList[4].reference = 0;
   paramList[4].list = &typeElem[4];
   paramList[4].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr=CalcRisk;
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


void CalcZCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (7,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (7,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Calc_Z");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 7;

   typeElem[0].type = NUMERIC_TYPE;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = paramList+1;

   typeElem[1].type = NUMERIC_TYPE;
   typeElem[1].next = NULL;
   paramList[1].reference = 0;
   paramList[1].list = &typeElem[1];
   paramList[1].next = paramList+2;

   typeElem[2].type = NUMERIC_TYPE;
   typeElem[2].next = NULL;
   paramList[2].reference = 0;
   paramList[2].list = &typeElem[2];
   paramList[2].next = paramList+3;

   typeElem[3].type = NUMERIC_TYPE;
   typeElem[3].next = NULL;
   paramList[3].reference = 0;
   paramList[3].list = &typeElem[3];
   paramList[3].next = paramList+4;

   typeElem[4].type = NUMERIC_TYPE;
   typeElem[4].next = NULL;
   paramList[4].reference = 0;
   paramList[4].list = &typeElem[4];
   paramList[4].next = paramList+5;

   typeElem[5].type = NUMERIC_TYPE;
   typeElem[5].next = NULL;
   paramList[5].reference = 0;
   paramList[5].list = &typeElem[5];
   paramList[5].next = paramList+6;

   typeElem[6].type = NUMERIC_TYPE;
   typeElem[6].next = NULL;
   paramList[6].reference = 0;
   paramList[6].list = &typeElem[6];
   paramList[6].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr=CalcZ;
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


long LoadModelFunctions(tFuncInit *funcInit, tFunc **funcList)
{
   ScoreModelCreate(funcInit, funcList);
   GetReasonsCreate(funcInit, funcList);
   GetReasonsValueCreate(funcInit, funcList);
   CalcVarsCreate(funcInit, funcList);
   CalcSubListCreate(funcInit, funcList);
   ZeroSubListCreate(funcInit, funcList);
   ProfInitCreate(funcInit, funcList);
   CalcRiskCreate(funcInit, funcList);
   CalcZCreate(funcInit, funcList);
   ProbNormCreate(funcInit, funcList);
   GetGMTOffsetCreate(funcInit, funcList);
   DumpDataCreate(funcInit, funcList);

   LoadProductModelFunctions(funcInit, funcList);

   return(0);
}
