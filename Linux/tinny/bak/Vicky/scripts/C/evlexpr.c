/*
** $Id: evlexpr.c,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: evlexpr.c,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 1.4  1997/02/10  23:57:05  can
 * Fixed bug where X := Y := Q; was not working properly
 *
 * Revision 1.3  1997/01/20  23:40:30  can
 * Fixed bug in string addition and also for returning records
 * with EvalSgl_OtherType.
 *
 * Revision 1.2  1997/01/18  00:30:16  can
 * Fixed bug in EvalString.
 *
 * Revision 1.1  1997/01/16  22:44:06  can
 * Initial revision
 *
*/

/* System includes */
#include <stdio.h>
#include <string.h>

/* HNC Rule Engine includes */
#include "evlexpr.h"
#include "calctab.h"
#include "bltins.h"
#include "typechk.h"
#include "engerr.h"
#include "runct.h"

/* HNC common includes */
#include "except.h"




void EvalQuickLongExpr(tAtom *exprEval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   long       *left, *right, *eval, evalArray[MAX_EVALS]; 
   tExprCalc  *exprCalc = (tExprCalc*)arg->atom.data.uVal; 
 
   /*****************************************/ 
   /* Evaluate the list of expression calcs */ 
   /*****************************************/ 
   while(1) 
   {  /* Get the left argument */ 
      if(exprCalc->flags&LEFT_ARG) 
      {  if(((tArg*)exprCalc->left)->flags&ATOM_ADDRESS) 
            left = (long*)((tArg*)exprCalc->left)->atom.data.uVal; 
         else 
            left = &((tArg*)exprCalc->left)->atom.data.lVal; 
      } 
      else 
         left = evalArray + (int)exprCalc->left; 
             
      /* Get the right argument */ 
      if(exprCalc->flags&RIGHT_ARG) 
      {  if(((tArg*)exprCalc->right)->flags&ATOM_ADDRESS) 
            right = (long*)((tArg*)exprCalc->right)->atom.data.uVal; 
         else 
            right = &((tArg*)exprCalc->right)->atom.data.lVal; 
      } 
      else 
         right = evalArray + (int)exprCalc->right; 
             
      eval = evalArray + exprCalc->evalIdx; 
 
      switch(exprCalc->func)  
      {  case EVAL_ANDBOOL: 
            *eval = (*left && *right); 
            break; 
         case EVAL_ORBOOL: 
            *eval = (*left || *right); 
            break; 
         case EVAL_EQUAL: 
            *eval = (*left == *right); 
            break; 
         case EVAL_NOTEQL: 
            *eval = (*left != *right); 
            break; 
         case EVAL_GRTREQL: 
            *eval = (*left >= *right); 
            break; 
         case EVAL_LESSEQL: 
            *eval = (*left <= *right); 
            break; 
         case EVAL_GRTR: 
            *eval = (*left > *right); 
            break; 
         case EVAL_LESS: 
            *eval = (*left < *right); 
            break; 
         case EVAL_MINUS: 
            *eval = (*left - *right); 
            break; 
         case EVAL_PLUS: 
            *eval = (*left + *right); 
            break; 
         case EVAL_MULTIPLY: 
            *eval = (*left * (*right)); 
            break; 
         case EVAL_DIVIDE: 
            if(*right) *eval = (*left / (*right)); 
            else *eval = 0; 
            break; 
         case EVAL_POWER: 
            *eval = pow(*left, *right); 
            break; 
         case EVAL_SET: 
            *left = *eval = *right; 
            break; 
      } 
 
      if(exprCalc->flags&IS_ANDED && (*eval==0)) 
      {  exprCalc = exprCalc->parent; 
         *eval = 0; 
      } 
      else if(exprCalc->flags&IS_ORED && *eval) 
      {  exprCalc = exprCalc->parent; 
         *eval = 1; 
      } 

      if(exprCalc->next) exprCalc = exprCalc->next; 
      else break; 
   } 

   exprEval->type = NUMERIC_TYPE;
   NUM_SETINT32(exprEval->data.num, evalArray[0]);
}

void EvalMixedLongExpr(tAtom *exprEval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{   
   tAtom     leftArg, rightArg; 
   long      *eval, evalArray[MAX_EVALS]; 
   long      *left, *right, tmpL, tmpR; 
   tExprCalc *exprCalc = ((tExprCalc*)arg->atom.data.uVal); 
 
   while(1) 
   {  if(exprCalc->func == EVAL_NEGATE)  
      {  evalArray[(int)exprCalc->evalIdx] = -(evalArray[(int)exprCalc->evalIdx]); 
         if(exprCalc->next) 
         {  exprCalc = exprCalc->next; 
            continue; 
         } 
         break; 
      } 
      else if(exprCalc->func == EVAL_NOT)  
      {  evalArray[(int)exprCalc->evalIdx] = !(evalArray[(int)exprCalc->evalIdx]); 
         if(exprCalc->next) 
         {  exprCalc = exprCalc->next; 
            continue; 
         } 
         break; 
      } 
      else  
      {  /* Get the left argument */ 
         if(exprCalc->flags&LEFT_ARG) 
         {  ((tArg*)exprCalc->left)->func->EvalFunc(&leftArg, 
                              (tArg*)exprCalc->left, status, calcTab); 
            if(((tArg*)exprCalc->left)->flags&ATOM_REFERENCE) 
               left = (long*)leftArg.data.uVal; 
            else 
            {  tmpL = NUM_GETLNG(leftArg.data.num); 
               left = &tmpL; 
            } 
         } 
         else 
            left = evalArray + (int)exprCalc->left; 
             
         /* Get the right argument */ 
         if(exprCalc->flags&RIGHT_ARG) 
         {  ((tArg*)exprCalc->right)->func->EvalFunc(&rightArg, 
                             (tArg*)exprCalc->right, status, calcTab); 
            tmpR = NUM_GETLNG(rightArg.data.num); 
            right = &tmpR; 
         } 
         else 
            right = evalArray + (int)exprCalc->right; 
             
         eval = evalArray + exprCalc->evalIdx; 
      }  
 
      switch(exprCalc->func)  
      {  case EVAL_ANDBOOL: 
            *eval = (*left && *right); 
            break; 
         case EVAL_ORBOOL: 
            *eval = (*left || *right); 
            break; 
         case EVAL_EQUAL: 
            *eval = (*left == *right); 
            break; 
         case EVAL_NOTEQL: 
            *eval = (*left != *right); 
            break; 
         case EVAL_GRTREQL: 
            *eval = (*left >= *right); 
            break; 
         case EVAL_LESSEQL: 
            *eval = (*left <= *right); 
            break; 
         case EVAL_GRTR: 
            *eval = (*left > *right); 
            break; 
         case EVAL_LESS: 
            *eval = (*left < *right); 
            break; 
         case EVAL_MINUS: 
            *eval = (*left - *right); 
            break; 
         case EVAL_PLUS: 
            *eval = (*left + *right); 
            break; 
         case EVAL_MULTIPLY: 
            *eval = (*left * (*right)); 
            break; 
         case EVAL_DIVIDE: 
            if(*right) *eval = (*left / (*right)); 
            else *eval = 0; 
            break; 
         case EVAL_POWER: 
            *eval = pow(*left, *right); 
            break; 
         case EVAL_SET: 
            *left = *eval = *right; 
            break; 
      } 
 
      if(exprCalc->flags&IS_ANDED && (*eval==0)) 
      {  exprCalc = exprCalc->parent; 
         *eval = 0; 
      } 
      else if(exprCalc->flags&IS_ORED && *eval) 
      {  exprCalc = exprCalc->parent; 
         *eval = 1; 
      } 

      if(exprCalc->next) exprCalc = exprCalc->next; 
      else break; 
   } 

   exprEval->type = NUMERIC_TYPE;
   NUM_SETINT32(exprEval->data.num, evalArray[0]);
}

void EvalQuickFloatExpr(tAtom *exprEval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   float      *left, *right, *eval, evalArray[MAX_EVALS]; 
   tExprCalc  *exprCalc = ((tExprCalc*)arg->atom.data.uVal); 
 
   /*****************************************/ 
   /* Evaluate the list of expression calcs */ 
   /*****************************************/ 
   while(1) 
   {  /* Get the left argument */ 
      if(exprCalc->flags&LEFT_ARG) 
      {  if(((tArg*)exprCalc->left)->flags&ATOM_ADDRESS) 
            left = (float*)((tArg*)exprCalc->left)->atom.data.uVal; 
         else 
            left = &((tArg*)exprCalc->left)->atom.data.fVal; 
      } 
      else 
         left = evalArray + (int)exprCalc->left; 
             
      /* Get the right argument */ 
      if(exprCalc->flags&RIGHT_ARG) 
      {  if(((tArg*)exprCalc->right)->flags&ATOM_ADDRESS) 
            right = (float*)((tArg*)exprCalc->right)->atom.data.uVal; 
         else 
            right = &((tArg*)exprCalc->right)->atom.data.fVal; 
      } 
      else 
         right = evalArray + (int)exprCalc->right; 
             
      eval = evalArray + exprCalc->evalIdx; 
 
      switch(exprCalc->func)  
      {  case EVAL_ANDBOOL: 
            *eval = (*left && *right); 
            break; 
         case EVAL_ORBOOL: 
            *eval = (*left || *right); 
            break; 
         case EVAL_EQUAL: 
            *eval = (*left == *right); 
            break; 
         case EVAL_NOTEQL: 
            *eval = (*left != *right); 
            break; 
         case EVAL_GRTREQL: 
            *eval = (*left >= *right); 
            break; 
         case EVAL_LESSEQL: 
            *eval = (*left <= *right); 
            break; 
         case EVAL_GRTR: 
            *eval = (*left > *right); 
            break; 
         case EVAL_LESS: 
            *eval = (*left < *right); 
            break; 
         case EVAL_MINUS: 
            *eval = (*left - *right); 
            break; 
         case EVAL_PLUS: 
            *eval = (*left + *right); 
            break; 
         case EVAL_MULTIPLY: 
            *eval = (*left * (*right)); 
            break; 
         case EVAL_DIVIDE: 
            if(*right) *eval = (*left / (*right)); 
            else *eval = 0; 
            break; 
         case EVAL_POWER: 
            *eval = pow(*left, *right); 
            break; 
         case EVAL_SET: 
            *left = *eval = *right; 
            break; 
      } 
 
      if(exprCalc->flags&IS_ANDED && (*eval==0)) 
      {  exprCalc = exprCalc->parent; 
         *eval = 0; 
      } 
      else if(exprCalc->flags&IS_ORED && *eval) 
      {  exprCalc = exprCalc->parent; 
         *eval = 1; 
      } 

      if(exprCalc->next) exprCalc = exprCalc->next; 
      else break; 
   } 

   exprEval->type = NUMERIC_TYPE;
   NUM_SETFLT(exprEval->data.num, evalArray[0]);
}

void EvalMixedFloatExpr(tAtom *exprEval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   tAtom     leftArg, rightArg; 
   float     *eval, evalArray[MAX_EVALS]; 
   float     *left, *right, tmpL, tmpR; 
   tExprCalc *exprCalc = ((tExprCalc*)arg->atom.data.uVal); 
 
   while(1) 
   {  if(exprCalc->func == EVAL_NEGATE)  
      {  evalArray[(int)exprCalc->evalIdx] =  
                      -(evalArray[(int)exprCalc->evalIdx]); 
         if(exprCalc->next) 
         {  exprCalc = exprCalc->next; 
            continue; 
         } 
 
         break; 
      } 
      else if(exprCalc->func == EVAL_NOT)  
      {  evalArray[(int)exprCalc->evalIdx] =  
                     !(evalArray[(int)exprCalc->evalIdx]); 
         if(exprCalc->next) 
         {  exprCalc = exprCalc->next; 
            continue; 
         } 
 
         break; 
      } 
      else  
      {  /* Get the left argument */ 
         if(exprCalc->flags&LEFT_ARG) 
         {  ((tArg*)exprCalc->left)->func->EvalFunc(&leftArg,  
                                 (tArg*)exprCalc->left, status, calcTab); 
            if(((tArg*)exprCalc->left)->flags&ATOM_REFERENCE) 
               left = (float*)leftArg.data.uVal; 
            else 
            {  tmpL = NUM_GETFLT(leftArg.data.num); 
               left = &tmpL; 
            } 
         } 
         else 
            left = evalArray + (int)exprCalc->left; 
             
         /* Get the right argument */ 
         if(exprCalc->flags&RIGHT_ARG) 
         {  ((tArg*)exprCalc->right)->func->EvalFunc(&rightArg, 
                              (tArg*)exprCalc->right, status, calcTab); 
            tmpR = NUM_GETFLT(rightArg.data.num); 
            right = &tmpR; 
         } 
         else 
            right = evalArray + (int)exprCalc->right; 
             
         eval = evalArray + exprCalc->evalIdx; 
      }  
 
      switch(exprCalc->func)  
      {  case EVAL_ANDBOOL: 
            *eval = (*left && *right); 
            break; 
         case EVAL_ORBOOL: 
            *eval = (*left || *right); 
            break; 
         case EVAL_EQUAL: 
            *eval = (*left == *right); 
            break; 
         case EVAL_NOTEQL: 
            *eval = (*left != *right); 
            break; 
         case EVAL_GRTREQL: 
            *eval = (*left >= *right); 
            break; 
         case EVAL_LESSEQL: 
            *eval = (*left <= *right); 
            break; 
         case EVAL_GRTR: 
            *eval = (*left > *right); 
            break; 
         case EVAL_LESS: 
            *eval = (*left < *right); 
            break; 
         case EVAL_MINUS: 
            *eval = (*left - *right); 
            break; 
         case EVAL_PLUS: 
            *eval = (*left + *right); 
            break; 
         case EVAL_MULTIPLY: 
            *eval = (*left * (*right)); 
            break; 
         case EVAL_DIVIDE: 
            if(*right) *eval = (*left / (*right)); 
            else *eval = 0; 
            break; 
         case EVAL_POWER: 
            *eval = pow(*left, *right); 
            break; 
         case EVAL_SET: 
            *left = *eval = *right; 
            break; 
      } 

      if(exprCalc->flags&IS_ANDED && (*eval==0)) 
      {  exprCalc = exprCalc->parent; 
         *eval = 0; 
      } 
      else if(exprCalc->flags&IS_ORED && *eval) 
      {  exprCalc = exprCalc->parent; 
         *eval = 1; 
      } 

      if(exprCalc->next) exprCalc = exprCalc->next; 
      else break; 
   } 

   exprEval->type = NUMERIC_TYPE;
   NUM_SETFLT(exprEval->data.num, evalArray[0]);
}

void EvalQuickNumericExpr(tAtom *exprEval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   tEngNum    *left, *right, *eval, evalArray[MAX_EVALS]; 
   tExprCalc  *exprCalc = ((tExprCalc*)arg->atom.data.uVal); 
 
   /*****************************************/ 
   /* Evaluate the list of expression calcs */ 
   /*****************************************/ 
   while(1) 
   {  /* Get the left argument */ 
      if(exprCalc->flags&LEFT_ARG) 
      {  if(((tArg*)exprCalc->left)->flags&ATOM_ADDRESS) 
            left = (tEngNum*)((tArg*)exprCalc->left)->atom.data.uVal; 
         else 
            left = &((tArg*)exprCalc->left)->atom.data.num; 
      } 
      else 
         left = evalArray + (int)exprCalc->left; 
             
      /* Get the right argument */ 
      if(exprCalc->flags&RIGHT_ARG) 
      {  if(((tArg*)exprCalc->right)->flags&ATOM_ADDRESS) 
            right = (tEngNum*)((tArg*)exprCalc->right)->atom.data.uVal; 
         else 
            right = &((tArg*)exprCalc->right)->atom.data.num; 
      } 
      else 
         right = evalArray + (int)exprCalc->right; 
             
      eval = evalArray + exprCalc->evalIdx; 
 
      switch(exprCalc->func)  
      {  case EVAL_ANDBOOL: 
            NUM_AND((*eval), (*left), (*right)) 
            break; 
         case EVAL_ORBOOL: 
            NUM_OR((*eval), (*left), (*right)) 
            break; 
         case EVAL_EQUAL: 
            NUM_EQUAL((*eval), (*left), (*right)) 
            break; 
         case EVAL_NOTEQL: 
            NUM_NOTEQL((*eval), (*left), (*right)) 
            break; 
         case EVAL_GRTREQL: 
            NUM_GRTREQL((*eval), (*left), (*right)) 
            break; 
         case EVAL_LESSEQL: 
            NUM_LESSEQL((*eval), (*left), (*right)) 
            break; 
         case EVAL_GRTR: 
            NUM_GRTR((*eval), (*left), (*right)) 
            break; 
         case EVAL_LESS: 
            NUM_LESS((*eval), (*left), (*right)) 
            break; 
         case EVAL_MINUS: 
            NUM_SUB((*eval), (*left), (*right)) 
            break; 
         case EVAL_PLUS: 
            NUM_ADD((*eval), (*left), (*right)) 
            break; 
         case EVAL_MULTIPLY: 
            NUM_MUL((*eval), (*left), (*right)) 
            break; 
         case EVAL_DIVIDE: 
            NUM_DIV((*eval), (*left), (*right)) 
            break; 
         case EVAL_POWER: 
            NUM_SETDBL((*eval), pow(NUM_GETDBL((*left)), NUM_GETDBL((*right))) ) 
            break; 
         case EVAL_SET: 
            *eval = *left = *right; 
            break; 
      }  

      if(exprCalc->flags&IS_ANDED && IS_FALSE((*eval))) 
      {  exprCalc = exprCalc->parent; 
         ZERO((*eval)) 
      } 
      else if(exprCalc->flags&IS_ORED && IS_TRUE((*eval))) 
      {  exprCalc = exprCalc->parent; 
         ONE((*eval)) 
      } 

      if(exprCalc->next) exprCalc = exprCalc->next; 
      else break; 
   } 

   exprEval->type = NUMERIC_TYPE;
   exprEval->data.num = evalArray[0];
}

void EvalMixedNumericExpr(tAtom *exprEval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   tAtom     leftArg, rightArg; 
   tEngNum   *eval, evalArray[MAX_EVALS]; 
   tEngNum   *left, *right; 
   tExprCalc *exprCalc = ((tExprCalc*)arg->atom.data.uVal); 
 
   while(1) 
   {  if(exprCalc->func == EVAL_NEGATE)  
      {  NUM_NEG(evalArray[(int)exprCalc->evalIdx]) 
         if(exprCalc->next) 
         {  exprCalc = exprCalc->next; 
            continue; 
         } 
 
         break; 
      } 
      else if(exprCalc->func == EVAL_NOT)  
      {  NUM_NOT(evalArray[(int)exprCalc->evalIdx]) 
         if(exprCalc->next) 
         {  exprCalc = exprCalc->next; 
            continue; 
         } 
 
         break; 
      } 
      else  
      {  /* Get the left argument */ 
         if(exprCalc->flags&LEFT_ARG) 
         {  ((tArg*)exprCalc->left)->func->EvalFunc(&leftArg, 
                                   (tArg*)exprCalc->left, status, calcTab); 
            left = &leftArg.data.num; 
         } 
         else 
            left = evalArray + (int)exprCalc->left; 
             
         /* Get the right argument */ 
         if(exprCalc->flags&RIGHT_ARG) 
         {  ((tArg*)exprCalc->right)->func->EvalFunc(&rightArg, 
                            (tArg*)exprCalc->right, status, calcTab); 
            right = &rightArg.data.num; 
         } 
         else 
            right = evalArray + (int)exprCalc->right; 
             
         eval = evalArray + exprCalc->evalIdx; 
      }  
 
      switch(exprCalc->func)  
      {  case EVAL_ANDBOOL: 
            NUM_AND((*eval), (*left), (*right)) 
            break; 
         case EVAL_ORBOOL: 
            NUM_OR((*eval), (*left), (*right)) 
            break; 
         case EVAL_EQUAL: 
            NUM_EQUAL((*eval), (*left), (*right)) 
            break; 
         case EVAL_NOTEQL: 
            NUM_NOTEQL((*eval), (*left), (*right)) 
            break; 
         case EVAL_GRTREQL: 
            NUM_GRTREQL((*eval), (*left), (*right)) 
            break; 
         case EVAL_LESSEQL: 
            NUM_LESSEQL((*eval), (*left), (*right)) 
            break; 
         case EVAL_GRTR: 
            NUM_GRTR((*eval), (*left), (*right)) 
            break; 
         case EVAL_LESS: 
            NUM_LESS((*eval), (*left), (*right)) 
            break; 
         case EVAL_MINUS: 
            NUM_SUB((*eval), (*left), (*right)) 
            break; 
         case EVAL_PLUS: 
            NUM_ADD((*eval), (*left), (*right)) 
            break; 
         case EVAL_MULTIPLY: 
            NUM_MUL((*eval), (*left), (*right)) 
            break; 
         case EVAL_DIVIDE: 
            NUM_DIV((*eval), (*left), (*right)) 
            break; 
         case EVAL_POWER: 
            NUM_SETDBL((*eval), pow(NUM_GETDBL((*left)), NUM_GETDBL((*right))) ) 
            break; 
         case EVAL_SET: 
            switch(leftArg.type) 
            {  case NUMERIC_TYPE: 
                  *((tEngNum*)leftArg.data.uVal) = (*right); 
                  break; 
               case HNC_FLOAT: 
                  *((float*)(leftArg.data.uVal)) = NUM_GETFLT((*right)); 
                  break; 
               case HNC_DOUBLE: 
                  *((double*)(leftArg.data.uVal)) = NUM_GETDBL((*right)); 
                  break; 
               case HNC_INT16: 
                  *((short*)(leftArg.data.uVal)) = NUM_GETSHRT((*right)); 
                  break; 
               case HNC_UINT16: 
                  *((unsigned short*)(leftArg.data.uVal)) = UNUM_GETSHRT((*right)); 
                  break; 
               case HNC_INT32: 
                  *((long*)(leftArg.data.uVal)) = NUM_GETLNG((*right)); 
                  break; 
               case HNC_UINT32: 
                  *((unsigned long*)(leftArg.data.uVal)) = UNUM_GETLNG((*right)); 
                  break; 
            } 

            *eval = *right;
      } 

      if(exprCalc->flags&IS_ANDED && IS_FALSE((*eval))) 
      {  exprCalc = exprCalc->parent; 
         ZERO((*eval)) 
      } 
      else if(exprCalc->flags&IS_ORED && IS_TRUE((*eval))) 
      {  exprCalc = exprCalc->parent; 
         ONE((*eval)) 
      } 

      if(exprCalc->next) exprCalc = exprCalc->next; 
      else break; 
   } 

   exprEval->type = NUMERIC_TYPE;
   exprEval->data.num = evalArray[0];
}

void EvalStringNumExpr(tAtom *exprEval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   char      *left, *right;  
   tAtom     temp, *eval, evalArray[MAX_EVALS];  
   int       leftLen, rightLen, len;  
   tExprCalc *exprCalc = (tExprCalc*)arg->atom.data.uVal;  
  
   /*****************************************/  
   /* Evaluate the list of expression calcs */  
   /*****************************************/  
   while(1)  
   {  if(exprCalc->flags&LEFT_ARG)  
      {  ((tArg*)exprCalc->left)->func->EvalFunc(&temp, 
                       (tArg*)exprCalc->left, status, calcTab);  
         left = temp.data.strVal.str;  
         leftLen = temp.data.strVal.len;  
      }  
      else  
      {  left = evalArray[(int)exprCalc->left].data.strVal.str;  
         leftLen = evalArray[(int)exprCalc->left].data.strVal.len;  
      }  
  
      if(exprCalc->flags&RIGHT_ARG)  
      {  ((tArg*)exprCalc->right)->func->EvalFunc(&temp, 
                            (tArg*)exprCalc->right, status, calcTab);  
         right = temp.data.strVal.str;  
         rightLen = temp.data.strVal.len;  
      }  
      else  
      {  right = evalArray[(int)exprCalc->right].data.strVal.str;  
         rightLen = evalArray[(int)exprCalc->right].data.strVal.len;  
      }  
 
      eval = evalArray + exprCalc->evalIdx;  
  
      switch(exprCalc->func)   
      {  case EVAL_EQUAL:  
            len = leftLen < rightLen ? leftLen : rightLen;  
            NUM_SETINT32(eval->data.num, (long)(memcmp(left, right, len)==0))  
            eval->type = NUMERIC_TYPE;  
            break;  
         case EVAL_NOTEQL:  
            len = leftLen < rightLen ? leftLen : rightLen;  
            NUM_SETINT32(eval->data.num, (long)(memcmp(left, right, len)!=0))  
            eval->type = NUMERIC_TYPE;  
            break;  
         case EVAL_GRTREQL:  
            len = leftLen < rightLen ? leftLen : rightLen;  
            NUM_SETINT32(eval->data.num, (long)(memcmp(left, right, len)>=0))  
            eval->type = NUMERIC_TYPE;  
            break;  
         case EVAL_LESSEQL:  
            len = leftLen < rightLen ? leftLen : rightLen;  
            NUM_SETINT32(eval->data.num, (long)(memcmp(left, right, len)<=0))  
            eval->type = NUMERIC_TYPE;  
            break;  
         case EVAL_GRTR:  
            len = leftLen < rightLen ? leftLen : rightLen;  
            NUM_SETINT32(eval->data.num, (long)(memcmp(left, right, len)>0))  
            eval->type = NUMERIC_TYPE;  
            break;  
         case EVAL_LESS:  
            len = leftLen < rightLen ? leftLen : rightLen;  
            NUM_SETINT32(eval->data.num, (long)(memcmp(left, right, len)<0))  
            eval->type = NUMERIC_TYPE;  
            break;  
      }   
 
      if(exprCalc->next) exprCalc = exprCalc->next; 
      else break; 
   }  

   *exprEval = evalArray[0];
}

void EvalStringExpr(tAtom *exprEval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{  
   char      *left, *right; 
   tAtom     temp, *eval, evalArray[MAX_EVALS]; 
   int       leftLen, rightLen, i, j, len; 
   tExprCalc *exprCalc = ((tExprCalc*)arg->atom.data.uVal); 
 
   /*****************************************/ 
   /* Evaluate the list of expression calcs */ 
   /*****************************************/ 
   while(1) 
   {  if(exprCalc->flags&LEFT_ARG) 
      {  ((tArg*)exprCalc->left)->func->EvalFunc(&temp, 
                   (tArg*)exprCalc->left, status, calcTab); 
         left = temp.data.strVal.str; 
         leftLen = temp.data.strVal.len; 
      } 
      else 
      {  left = evalArray[(int)exprCalc->left].data.strVal.str; 
         leftLen = evalArray[(int)exprCalc->left].data.strVal.len; 
      } 
 
      if(exprCalc->flags&RIGHT_ARG) 
      {  ((tArg*)exprCalc->right)->func->EvalFunc(&temp, 
                            (tArg*)exprCalc->right, status, calcTab); 
         right = temp.data.strVal.str; 
         rightLen = temp.data.strVal.len; 
      } 
      else 
      {  right = evalArray[(int)exprCalc->right].data.strVal.str; 
         rightLen = evalArray[(int)exprCalc->right].data.strVal.len; 
      } 
 
      eval = evalArray + exprCalc->evalIdx; 
 
      switch(exprCalc->func)  
      {  case EVAL_SET: 
            len = (rightLen < leftLen) ? rightLen : leftLen; 
 
            memcpy(left, right, len);  
 
            if((len == rightLen) && (len < leftLen)) 
               memset(left+rightLen, 0, leftLen-len);  
 
            eval->data.strVal.str = left; 
            eval->data.strVal.len = len; 
            eval->type = HNC_STRING; 
            break; 
 
         case EVAL_PLUS: 
            eval->data.strVal.len = leftLen + rightLen + 1; 
            eval->data.strVal.str =  
                (char*)GetMemory(calcTab, eval->data.strVal.len, status); 
 
            i=0; 
            while(i < leftLen) 
            {  if(left[i] == '\0')
                  break; 
               eval->data.strVal.str[i] = left[i]; 
               i++; 
            } 
 
            j=0; 
            while(j < rightLen) 
            {  if(right[j] == '\0')
                  break; 
               eval->data.strVal.str[i] = right[j]; 
               i++; j++; 
            } 
 
            eval->data.strVal.str[i] = '\0';
            eval->type = HNC_STRING; 
 
            break; 
      }  
 
      if(exprCalc->next) exprCalc = exprCalc->next; 
      else break; 
   } 

   exprEval->type = HNC_STRING;
   *exprEval = evalArray[0];
}

void EvalMixedExpr(tAtom *exprEval, tArg *arg, tREStatus *status, tCalcTable *calcTab)
{ 
   tAtom     tmpLeft, *left,  tmpRight, *right; 
   tAtom     *eval, evalArray[MAX_EVALS]; 
   int       i, j, len; 
   tExprCalc *exprCalc = ((tExprCalc*)arg->atom.data.uVal); 
 
   /*****************************************/ 
   /* Evaluate the list of expression calcs */ 
   /*****************************************/ 
   while(1) 
   {  if(exprCalc->func == EVAL_NEGATE)  
      {  NUM_NEG(evalArray[(int)exprCalc->evalIdx].data.num); 
         if(exprCalc->next) 
         {  exprCalc = exprCalc->next; 
            continue; 
         } 
 
         break; 
      } 
      else if(exprCalc->func == EVAL_NOT)  
      {  NUM_NOT(evalArray[(int)exprCalc->evalIdx].data.num); 
         if(exprCalc->next) 
         {  exprCalc = exprCalc->next; 
            continue; 
         } 
 
         break; 
      } 
      else  
      {  if(exprCalc->flags&LEFT_ARG) 
         {  ((tArg*)exprCalc->left)->func->EvalFunc(&tmpLeft, 
                            (tArg*)exprCalc->left, status, calcTab); 
            left = &tmpLeft; 
         } 
         else 
           left = evalArray + (int)exprCalc->left; 
             
         if(exprCalc->flags&RIGHT_ARG) 
         {  ((tArg*)exprCalc->right)->func->EvalFunc(&tmpRight, 
                            (tArg*)exprCalc->right, status, calcTab); 
            right = &tmpRight; 
         } 
         else 
            right = evalArray + (int)exprCalc->right; 
 
         eval = evalArray + exprCalc->evalIdx; 
      }  
 
      switch(exprCalc->func)  
      {  case EVAL_SET: 
            switch(left->type) 
            {  case NUMERIC_TYPE: 
                  NUM_SET((*((tEngNum*)left->data.uVal)), right->data.num) 
                  break; 
               case HNC_FLOAT: 
                  *((float*)(left->data.uVal)) = NUM_GETFLT(right->data.num); 
                  break; 
               case HNC_DOUBLE: 
                  *((double*)(left->data.uVal)) = NUM_GETDBL(right->data.num); 
                  break; 
               case HNC_INT16: 
                  *((short*)(left->data.uVal)) = NUM_GETSHRT(right->data.num); 
                  break; 
               case HNC_UINT16: 
                  *((unsigned short*)(left->data.uVal)) =  
                                               UNUM_GETSHRT(right->data.num); 
                  break; 
               case HNC_INT32: 
                  *((long*)(left->data.uVal)) = NUM_GETLNG(right->data.num); 
                  break; 
               case HNC_UINT32: 
                  *((unsigned long*)(left->data.uVal)) =  
                                               UNUM_GETLNG(right->data.num); 
                  break; 
               case ARRAY_TYPE: 
                  memcpy(*(((tArrayData*)left->data.uVal)->data), 
                         *(((tArrayData*)right->data.uVal)->data), 
                         ((tArrayData*)left->data.uVal)->array->dataSize); 
                  break; 
               case RECORD_TYPE: 
                  memcpy(*(((tRecData*)left->data.uVal)->data), 
                         *(((tRecData*)right->data.uVal)->data), 
                         ((tRecData*)left->data.uVal)->record->dataSize); 
                  break; 
               default: 
                  len = (right->data.strVal.len < left->data.strVal.len) ?  
                         right->data.strVal.len : left->data.strVal.len; 
 
                  memcpy(left->data.strVal.str,  
                                right->data.strVal.str,  
                                len);  
 
                  if((len == right->data.strVal.len) &&  
                      (len < left->data.strVal.len)) 
                     memset(left->data.strVal.str+len,  
                                   0, left->data.strVal.len-len);  
            } 
 
            *eval = *right; 
            break; 
         case EVAL_ANDBOOL: 
            NUM_AND(eval->data.num, left->data.num, right->data.num) 
            eval->type = NUMERIC_TYPE; 
            break; 
         case EVAL_ORBOOL: 
            NUM_OR(eval->data.num, left->data.num, right->data.num) 
            eval->type = NUMERIC_TYPE; 
            break; 
         case EVAL_EQUAL: 
            if(left->type == NUMERIC_TYPE) 
               NUM_EQUAL(eval->data.num, left->data.num, right->data.num) 
            else 
            {  len = left->data.strVal.len < right->data.strVal.len ? 
                     left->data.strVal.len : right->data.strVal.len; 
               NUM_SETINT32(eval->data.num, 
                   (long)(memcmp(left->data.strVal.str, right->data.strVal.str, len)==0)) 
            } 
 
            eval->type = NUMERIC_TYPE; 
            break; 
         case EVAL_NOTEQL: 
            if(left->type == NUMERIC_TYPE) 
               NUM_NOTEQL(eval->data.num, left->data.num, right->data.num) 
            else 
            {  len = left->data.strVal.len < right->data.strVal.len ? 
                     left->data.strVal.len : right->data.strVal.len; 
               NUM_SETINT32(eval->data.num, 
                   (long)(memcmp(left->data.strVal.str, right->data.strVal.str, len)!=0)) 
            } 
 
            eval->type = NUMERIC_TYPE; 
            break; 
         case EVAL_GRTREQL: 
            if(left->type == NUMERIC_TYPE) 
               NUM_GRTREQL(eval->data.num, left->data.num, right->data.num) 
            else 
            {  len = left->data.strVal.len < right->data.strVal.len ? 
                     left->data.strVal.len : right->data.strVal.len; 
               NUM_SETINT32(eval->data.num, 
                   (long)(memcmp(left->data.strVal.str, right->data.strVal.str, len)>=0)) 
            } 
 
            eval->type = NUMERIC_TYPE; 
            break; 
         case EVAL_LESSEQL: 
            if(left->type == NUMERIC_TYPE) 
               NUM_LESSEQL(eval->data.num, left->data.num, right->data.num) 
            else 
            {  len = left->data.strVal.len < right->data.strVal.len ? 
                     left->data.strVal.len : right->data.strVal.len; 
               NUM_SETINT32(eval->data.num, 
                   (long)(memcmp(left->data.strVal.str, right->data.strVal.str, len)<=0)) 
            } 
 
            eval->type = NUMERIC_TYPE; 
            break; 
         case EVAL_GRTR: 
            if(left->type == NUMERIC_TYPE) 
               NUM_GRTR(eval->data.num, left->data.num, right->data.num) 
            else 
            {  len = left->data.strVal.len < right->data.strVal.len ? 
                     left->data.strVal.len : right->data.strVal.len; 
               NUM_SETINT32(eval->data.num, 
                   (long)(memcmp(left->data.strVal.str, right->data.strVal.str, len)>0)) 
            } 
 
            eval->type = NUMERIC_TYPE; 
            break; 
         case EVAL_LESS: 
            if(left->type == NUMERIC_TYPE) 
               NUM_LESS(eval->data.num, left->data.num, right->data.num) 
            else 
            {  len = left->data.strVal.len < right->data.strVal.len ? 
                     left->data.strVal.len : right->data.strVal.len; 
               NUM_SETINT32(eval->data.num, 
                   (long)(memcmp(left->data.strVal.str, right->data.strVal.str, len)<0)) 
            } 
 
            eval->type = NUMERIC_TYPE; 
            break; 
         case EVAL_MINUS: 
            NUM_SUB(eval->data.num, left->data.num, right->data.num) 
 
            eval->type = NUMERIC_TYPE; 
            break; 
         case EVAL_PLUS: 
            if(left->type == NUMERIC_TYPE) 
            {  NUM_ADD(eval->data.num, left->data.num, right->data.num) 
 
               eval->type = NUMERIC_TYPE; 
            } 
            else 
            {  eval->data.strVal.len =  
                                  left->data.strVal.len +   
                                  right->data.strVal.len + 1;   
               eval->data.strVal.str =  
                   (char*)GetMemory(calcTab, eval->data.strVal.len, status); 
 
               i=0; 
               while(i < left->data.strVal.len) 
               {  if(left->data.strVal.str[i] == '\0')
                     break; 
                  eval->data.strVal.str[i] = 
                                        left->data.strVal.str[i]; 
                  i++; 
               } 
 
               j=0; 
               while(j < right->data.strVal.len) 
               {  if(right->data.strVal.str[j] == '\0')
                     break; 
                  eval->data.strVal.str[i] = 
                                        right->data.strVal.str[j]; 
                  i++; j++; 
               } 
 
               eval->data.strVal.str[i] = '\0';
               eval->type = HNC_STRING; 
            } 
            break; 
         case EVAL_MULTIPLY: 
            NUM_MUL(eval->data.num, left->data.num, right->data.num) 
 
            eval->type = NUMERIC_TYPE; 
            break; 
         case EVAL_POWER: 
            NUM_SETDBL(eval->data.num,  
                     pow(NUM_GETDBL(left->data.num), NUM_GETDBL(right->data.num))); 
 
            eval->type = NUMERIC_TYPE; 
            break; 
         case EVAL_DIVIDE: 
            NUM_DIV(eval->data.num, left->data.num, right->data.num) 
 
            eval->type = NUMERIC_TYPE; 
            break; 
      }  
 
      if(exprCalc->flags&IS_ANDED && IS_FALSE(eval->data.num)) 
      {  exprCalc = exprCalc->parent; 
         ZERO(eval->data.num) 
      } 
      else if(exprCalc->flags&IS_ORED && IS_TRUE(eval->data.num)) 
      {  exprCalc = exprCalc->parent; 
         ONE(eval->data.num) 
      } 

      if(exprCalc->next) exprCalc = exprCalc->next; 
      else break; 
   } 

   *exprEval = evalArray[0];
}



/**************************************************************************/
/* The following are specific calculations for calc var return statements */
/**************************************************************************/

void EvalSgl_LngVarCalc(tAtom *eval, tArg *stmtArg, tREStatus *status, tCalcTable *calcTab)
{
   tArg     *arg = ((tArgCalc*)stmtArg->atom.data.uVal)->argList;
   tVarCalc *varCalc = (tVarCalc*)arg->next->atom.data.uVal;

   arg->func->EvalFunc(eval, arg, status, calcTab);

   *((long*)varCalc->var->data) = NUM_GETLNG(eval->data.num);
}

void EvalSgl_FltVarCalc(tAtom *eval, tArg *stmtArg, tREStatus *status, tCalcTable *calcTab)
{
   tArg     *arg = ((tArgCalc*)stmtArg->atom.data.uVal)->argList;
   tVarCalc *varCalc = (tVarCalc*)arg->next->atom.data.uVal;

   arg->func->EvalFunc(eval, arg, status, calcTab);

   *((float*)varCalc->var->data) = NUM_GETFLT(eval->data.num);
}

void EvalSgl_NumVarCalc(tAtom *eval, tArg *stmtArg, tREStatus *status, tCalcTable *calcTab)
{
   tArg     *arg = ((tArgCalc*)stmtArg->atom.data.uVal)->argList;
   tVarCalc *varCalc = (tVarCalc*)arg->next->atom.data.uVal;

   arg->func->EvalFunc(eval, arg, status, calcTab);

   *((tEngNum*)varCalc->var->data) = eval->data.num;
}

void EvalSgl_StrVarCalc(tAtom *eval, tArg *stmtArg, tREStatus *status, tCalcTable *calcTab)
{
   tArg     *arg = ((tArgCalc*)stmtArg->atom.data.uVal)->argList;
   tVarCalc *varCalc = (tVarCalc*)arg->next->atom.data.uVal;
   int      len;

   arg->func->EvalFunc(eval, arg, status, calcTab);

   len = varCalc->var->dataSize < eval->data.strVal.len ? 
          varCalc->var->dataSize : eval->data.strVal.len;

   memcpy((char*)varCalc->var->data, eval->data.strVal.str, len);
}

void EvalSgl_OthVarCalc(tAtom *eval, tArg *stmtArg, tREStatus *status, tCalcTable *calcTab)
{
   tArg     *arg = ((tArgCalc*)stmtArg->atom.data.uVal)->argList;
   tVarCalc *varCalc = (tVarCalc*)arg->next->atom.data.uVal;

   arg->func->EvalFunc(eval, arg, status, calcTab);

   switch(varCalc->var->type)
   {  case HNC_DOUBLE:
         *((double*)varCalc->var->data) = NUM_GETDBL(eval->data.num);
         break;
      case HNC_INT16:
         *((short*)varCalc->var->data) = NUM_GETSHRT(eval->data.num);
         break;
      case HNC_UINT16:
         *((unsigned short*)varCalc->var->data) = UNUM_GETSHRT(eval->data.num);
         break;
      case HNC_UINT32:
         *((unsigned long*)varCalc->var->data) = UNUM_GETLNG(eval->data.num);
         break;
      case ARRAY_TYPE: 
         memcpy(*(((tArrayData*)varCalc->var->data)->data), 
                *(((tArrayData*)eval->data.uVal)->data),
                ((tArrayData*)varCalc->var->data)->array->dataSize);
         break;
      case RECORD_TYPE:
         memcpy(*(((tRecInstance*)varCalc->var->data)->data->data),
                *(((tRecData*)eval->data.uVal)->data),
                ((tRecInstance*)varCalc->var->data)->data->record->dataSize);
         break; 
   }
}
