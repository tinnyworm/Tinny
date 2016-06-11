#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rulfunc.h"
#include "hncrtyp.h"
#include "mymall.h"


char charMap[27] =
{ 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
  'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };
tAtom PackZip(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom eval;
   char  chr, *postCode = aParams[0].data.strVal.str;
   long  packed = 0;
   int   index, val=0;


   /*********************************************************/
   /* Pack a five character postal code into a long integer */
   /*********************************************************/
   for(index=0; index<5; index++)
   {  chr = postCode[index];

      if(!(isalnum(chr) || chr==' ')) 
      {  NUM_SETINT32(eval.data.num, 0)
         eval.type = NUMERIC_TYPE;
         return(eval);
      }

      if(isalpha(chr))
      {  int first=0, last=25, i=0, found=0;
         chr = toupper(chr);

         while(first <= last && !found)
         {  i = (first+last)/2;

            if(chr == charMap[i])
            {  found=1;
               break;
            }

            else if(chr < charMap[i]) last = i - 1;
            else first = i + 1;
         }

         val = i;
      }
      else if(isdigit(chr))
         val = (chr-'0') + 26;
      else if(chr == ' ')
         val = 36;

      if(index < 5) packed *= 0x0000003F;
      packed+=val;
   }

   NUM_SETINT32(eval.data.num, packed)
   eval.type = NUMERIC_TYPE;
   return(eval);
}


tAtom OldPackZip(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom eval;
   char  chr, *postCode = aParams[0].data.strVal.str;
   long  packed = 0;
   int   index, val=0;


   /*********************************************************/
   /* Pack a five character postal code into a long integer */
   /*********************************************************/
   for(index=0; index<5 && postCode[index] != ' '; index++)
   {  chr = postCode[index];

      if(!isalnum(chr))
      {  NUM_SETINT32(eval.data.num, 0)
         eval.type = NUMERIC_TYPE;
         return(eval);
      }

      if(isalpha(chr))
      {  int first=0, last=25, i=0, found=0;
         chr = toupper(chr);

         while(first <= last && !found)
         {  i = (first+last)/2;

            if(chr == charMap[i])
            {  found=1;
               break;
            }

            else if(chr < charMap[i]) last = i - 1;
            else first = i + 1;
         }

         val = i;
      }
      else if(isdigit(chr))
         val = (chr-'0') + 26;

      if(index < 5) packed *= 0x0000003F;
      packed+=val;
   }

   NUM_SETINT32(eval.data.num, packed)
   eval.type = NUMERIC_TYPE;
   return(eval);
}


tAtom UnpackZip(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom eval;
   long  packed = NUM_GETLNG(aParams[0].data.num);
   char  *unpacked = aParams[1].data.strVal.str;
   unsigned char  chr;
   int   i;

   /*********************************************************/
   /* Pack a five character postal code into a long integer */
   /*********************************************************/
   for(i=4; i>=0 && packed >= 0; i--)
   {  chr = packed % 0x0000003F;
      packed = packed / 0x0000003F;

      if(chr<26) chr=charMap[chr];
      else if(chr==36) chr = ' ';
      else chr = (chr-26) + '0';

      unpacked[i] = chr;
   }

   aParams[1].data.strVal.len = 5;

   NUM_SETINT32(eval.data.num, 0)
   eval.type = NUMERIC_TYPE;
   return(eval);
}


tAtom OldUnpackZip(tFuncCallData *funcCallData, tAtom *aParams, int numArgs)
{
   tAtom eval;
   long  packed = NUM_GETLNG(aParams[0].data.num);
   char  *unpacked = aParams[1].data.strVal.str;
   unsigned char  chr;
   int   i;

   /*********************************************************/
   /* Pack a five character postal code into a long integer */
   /*********************************************************/
   for(i=4; i>=0 && packed >= 0; i--)
   {  chr = packed % 0x0000003F;
      packed = packed / 0x0000003F;

      if(chr<26) chr=charMap[chr];
      else chr = (chr-26) + '0';

      unpacked[i] = chr;
   }

   aParams[1].data.strVal.len = 5;

   NUM_SETINT32(eval.data.num, 0)
   eval.type = NUMERIC_TYPE;
   return(eval);
}





void UnpackZipCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (2,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (2,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Unpack_Zip");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 2;
   typeElem[0].type = NUMERIC_TYPE;
   typeElem[0].next = NULL;
   typeElem[1].type = HNC_STRING;
   typeElem[1].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = paramList+1;
   paramList[1].reference = 0;
   paramList[1].list = typeElem+1;
   paramList[1].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr=UnpackZip;
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


void PackZipCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (1,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (1,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Pack_Zip");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 1;
   typeElem[0].type = HNC_STRING;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr=PackZip;
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


void OldUnpackZipCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (2,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (2,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "Old_Unpack_Zip");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 2;
   typeElem[0].type = NUMERIC_TYPE;
   typeElem[0].next = NULL;
   typeElem[1].type = HNC_STRING;
   typeElem[1].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = paramList+1;
   paramList[1].reference = 0;
   paramList[1].list = typeElem+1;
   paramList[1].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr = OldUnpackZip;
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


void OldPackZipCreate(tFuncInit *funcInit, tFunc **funcList)
{
   static tFunc*       funcElem;
   static tTypeList*   typeElem;
   static tParamList*  paramList;
   tFunc*              tempList;

   funcElem = (tFunc*) MyCalloc (1,sizeof(tFunc));
   typeElem = (tTypeList*) MyCalloc (1,sizeof(tTypeList));
   paramList = (tParamList*) MyCalloc (1,sizeof(tParamList));

   AllocFuncName(&funcElem->functionName, "OLD_Pack_Zip");
   funcElem->unlimitedParams = 0;
   funcElem->numParams = 1;
   typeElem[0].type = HNC_STRING;
   typeElem[0].next = NULL;
   paramList[0].reference = 0;
   paramList[0].list = &typeElem[0];
   paramList[0].next = NULL;

   funcElem->paramTypeList = paramList;
   funcElem->returnType = NUMERIC_TYPE;
   funcElem->FuncPtr = OldPackZip;
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

long LoadFalconModelFunctions(tFuncInit *funcInit, tFunc **funcList)
{
   PackZipCreate(funcInit, funcList);
   UnpackZipCreate(funcInit, funcList);
   OldPackZipCreate(funcInit, funcList);
   OldUnpackZipCreate(funcInit, funcList);

   return(0);
}
