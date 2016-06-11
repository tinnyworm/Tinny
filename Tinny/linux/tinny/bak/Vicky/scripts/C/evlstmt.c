/*
** $Id: evlstmt.c,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: evlstmt.c,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 1.1  1997/01/16  22:44:06  can
 * Initial revision
 *
*/

#include <stdio.h>
#include <string.h>

#include "evlstmt.h"
#include "engerr.h"
#include "runct.h"
 

void EvalNumIncrement(tAtom *eval, tArg *stmtArg, tREStatus *status, tCalcTable *calcTab)
{
   tArg *arg = ((tArgCalc*)stmtArg->atom.data.uVal)->argList;

   arg->func->EvalFunc(eval, arg, status, calcTab);
   NUM_INCR(eval->data.uVal)
}

void EvalNumIPADD(tAtom *eval, tArg *stmtArg, tREStatus *status, tCalcTable *calcTab)
{
   tAtom right;
   tArg  *arg = ((tArgCalc*)stmtArg->atom.data.uVal)->argList;

   arg->func->EvalFunc(eval, arg, status, calcTab);
   arg->next->func->EvalFunc(&right, arg->next, status, calcTab);
   NUM_IPADD(eval->data.uVal, right.data.num)
}

void EvalNumIPSUB(tAtom *eval, tArg *stmtArg, tREStatus *status, tCalcTable *calcTab)
{
   tAtom right;
   tArg  *arg = ((tArgCalc*)stmtArg->atom.data.uVal)->argList;

   arg->func->EvalFunc(eval, arg, status, calcTab);
   arg->next->func->EvalFunc(&right, arg->next, status, calcTab);
   NUM_IPSUB(eval->data.uVal, right.data.num)
}

void EvalNumIPMUL(tAtom *eval, tArg *stmtArg, tREStatus *status, tCalcTable *calcTab)
{
   tAtom right;
   tArg  *arg = ((tArgCalc*)stmtArg->atom.data.uVal)->argList;

   arg->func->EvalFunc(eval, arg, status, calcTab);
   arg->next->func->EvalFunc(&right, arg->next, status, calcTab);
   NUM_IPMUL(eval->data.uVal, right.data.num)
}

void EvalNumIPDIV(tAtom *eval, tArg *stmtArg, tREStatus *status, tCalcTable *calcTab)
{
   tAtom right;
   tArg  *arg = ((tArgCalc*)stmtArg->atom.data.uVal)->argList;

   arg->func->EvalFunc(eval, arg, status, calcTab);
   arg->next->func->EvalFunc(&right, arg->next, status, calcTab);
   NUM_IPDIV(eval->data.uVal, right.data.num)
}

void EvalFloatIncrement(tAtom *eval, tArg *stmtArg, tREStatus *status, tCalcTable *calcTab)
{
   tArg  *arg = ((tArgCalc*)stmtArg->atom.data.uVal)->argList;

   arg->func->EvalFunc(eval, arg, status, calcTab);
   (*((float*)eval->data.uVal))++;
}

void EvalFloatIPADD(tAtom *eval, tArg *stmtArg, tREStatus *status, tCalcTable *calcTab)
{
   tAtom right;
   tArg  *arg = ((tArgCalc*)stmtArg->atom.data.uVal)->argList;

   arg->func->EvalFunc(eval, arg, status, calcTab);
   arg->next->func->EvalFunc(&right, arg->next, status, calcTab);
   (*((float*)eval->data.uVal)) += NUM_GETFLT(right.data.num);
}

void EvalFloatIPSUB(tAtom *eval, tArg *stmtArg, tREStatus *status, tCalcTable *calcTab)
{
   tAtom right;
   tArg  *arg = ((tArgCalc*)stmtArg->atom.data.uVal)->argList;

   arg->func->EvalFunc(eval, arg, status, calcTab);
   arg->next->func->EvalFunc(&right, arg->next, status, calcTab);
   (*((float*)eval->data.uVal)) -= NUM_GETFLT(right.data.num);
}

void EvalFloatIPMUL(tAtom *eval, tArg *stmtArg, tREStatus *status, tCalcTable *calcTab)
{
   tAtom right;
   tArg  *arg = ((tArgCalc*)stmtArg->atom.data.uVal)->argList;

   arg->func->EvalFunc(eval, arg, status, calcTab);
   arg->next->func->EvalFunc(&right, arg->next, status, calcTab);
   (*((float*)eval->data.uVal)) *= NUM_GETFLT(right.data.num);
}

void EvalFloatIPDIV(tAtom *eval, tArg *stmtArg, tREStatus *status, tCalcTable *calcTab)
{
   float denom;
   tAtom right;
   tArg  *arg = ((tArgCalc*)stmtArg->atom.data.uVal)->argList;

   arg->func->EvalFunc(eval, arg, status, calcTab);
   arg->next->func->EvalFunc(&right, arg->next, status, calcTab);

   denom = NUM_GETFLT(right.data.num);
   if(denom) (*((float*)eval->data.uVal)) /= denom;
   else (*((float*)eval->data.uVal)) = 0;
}

void EvalLongIncrement(tAtom *eval, tArg *stmtArg, tREStatus *status, tCalcTable *calcTab)
{
   tArg  *arg = ((tArgCalc*)stmtArg->atom.data.uVal)->argList;

   arg->func->EvalFunc(eval, arg, status, calcTab);
   (*((long*)eval->data.uVal))++;
}

void EvalLongIPADD(tAtom *eval, tArg *stmtArg, tREStatus *status, tCalcTable *calcTab)
{
   tAtom right;
   tArg  *arg = ((tArgCalc*)stmtArg->atom.data.uVal)->argList;

   arg->func->EvalFunc(eval, arg, status, calcTab);
   arg->next->func->EvalFunc(&right, arg->next, status, calcTab);
   (*((long*)eval->data.uVal)) += NUM_GETLNG(right.data.num);
}

void EvalLongIPSUB(tAtom *eval, tArg *stmtArg, tREStatus *status, tCalcTable *calcTab)
{
   tAtom right;
   tArg  *arg = ((tArgCalc*)stmtArg->atom.data.uVal)->argList;

   arg->func->EvalFunc(eval, arg, status, calcTab);
   arg->next->func->EvalFunc(&right, arg->next, status, calcTab);
   (*((long*)eval->data.uVal)) -= NUM_GETLNG(right.data.num);
}

void EvalLongIPMUL(tAtom *eval, tArg *stmtArg, tREStatus *status, tCalcTable *calcTab)
{
   tAtom right;
   tArg  *arg = ((tArgCalc*)stmtArg->atom.data.uVal)->argList;

   arg->func->EvalFunc(eval, arg, status, calcTab);
   arg->next->func->EvalFunc(&right, arg->next, status, calcTab);
   (*((long*)eval->data.uVal)) *= NUM_GETLNG(right.data.num);
}

void EvalLongIPDIV(tAtom *eval, tArg *stmtArg, tREStatus *status, tCalcTable *calcTab)
{
   long  denom;
   tAtom right;
   tArg  *arg = ((tArgCalc*)stmtArg->atom.data.uVal)->argList;

   arg->func->EvalFunc(eval, arg, status, calcTab);
   arg->next->func->EvalFunc(&right, arg->next, status, calcTab);

   denom = NUM_GETLNG(right.data.num);
   if(denom) (*((long*)eval->data.uVal)) /= denom;
   else (*((long*)eval->data.uVal)) = 0;
}



void EvalVarIf(tAtom *eval, tArg *stmtArg, tREStatus *status, tCalcTable *calcTab)
{
   tArg  *arg = ((tArgCalc*)stmtArg->atom.data.uVal)->argList;

   /*****************************************************/
   /* Evaluate first argument.  If it is TRUE, then run */
   /* the secord argument, otherwise, run the third     */
   /* argument.                                         */
   /*****************************************************/
   arg->func->EvalFunc(eval, arg, status, calcTab);
   if(IS_TRUE(eval->data.num))
   {  arg->next->func->EvalFunc(eval, arg->next, status, calcTab);

      if(arg->next->flags&ATOM_VAR_RETURN)
      {  if(status->errCode == 0) 
            status->errCode = RETURN_FOUND;
      }
   }
   else if(arg->next->next)
   {  arg->next->next->func->EvalFunc(eval, arg->next->next, status, calcTab);

      if(arg->next->next->flags&ATOM_VAR_RETURN)
      {  if(status->errCode == 0) 
            status->errCode = RETURN_FOUND;
      }
   }
}

void EvalIf(tAtom *eval, tArg *stmtArg, tREStatus *status, tCalcTable *calcTab)
{
   tArg  *arg = ((tArgCalc*)stmtArg->atom.data.uVal)->argList;

   /*****************************************************/
   /* Evaluate first argument.  If it is TRUE, then run */
   /* the secord argument, otherwise, run the third     */
   /* argument.                                         */
   /*****************************************************/
   arg->func->EvalFunc(eval, arg, status, calcTab);
   if(IS_TRUE(eval->data.num))
      arg->next->func->EvalFunc(eval, arg->next, status, calcTab);
   else if(arg->next->next)
      arg->next->next->func->EvalFunc(eval, arg->next->next, status, calcTab);
}

void EvalNumericCase(tAtom *eval, tArg *stmtArg, tREStatus *status, tCalcTable *calcTab)
{
   tArg  *arg = ((tArgCalc*)stmtArg->atom.data.uVal)->argList;
   int   found = 0;

   /**********************************************/
   /* Evaluate first argument, which is the case */
   /* expression.  Compare the result to args    */
   /* until arg is statement list.  If had hit   */
   /* an arg, then run the statement list and    */
   /* leave.                                     */
   /**********************************************/
   arg->func->EvalFunc(eval, arg, status, calcTab);

   arg = arg->next;
   while(arg)
   {  
      /*****************************************************/
      /* See if current value matches.  If so, skip to the */ 
      /* statement block and get out.  If not, look at the */
      /* other data in this list                           */
      /*****************************************************/
      if(IS_EQUAL(eval->data.num, arg->atom.data.num))
      {  while(!(arg->flags&ATOM_CASE_STMT))
            arg = arg->next;
         break;
      }
      else
      {  while(!(arg->flags&ATOM_CASE_STMT))
         {  if(IS_EQUAL(eval->data.num, arg->atom.data.num))
               found = 1;
            arg = arg->next;
         }
        
         /* If found match, break out */
         if(found) break;

         /* Skip the statement block for this list */
         arg = arg->next;
      }

      /* If next argument is a statement block, it must be the default */
      if(arg)
      {  if(arg->flags&ATOM_CASE_STMT)
            break;
      }
   }

   if(arg)
   {  arg->func->EvalFunc(eval, arg, status, calcTab);

      if(arg->flags&ATOM_VAR_RETURN)
      {  if(status->errCode == 0) 
            status->errCode = RETURN_FOUND;
      }
   }
}

void EvalStringCase(tAtom *eval, tArg *stmtArg, tREStatus *status, tCalcTable *calcTab)
{
   tArg  *arg = ((tArgCalc*)stmtArg->atom.data.uVal)->argList;
   int   found = 0;

   /**********************************************/
   /* Evaluate first argument, which is the case */
   /* expression.  Compare the result to args    */
   /* until arg is statement list.  If had hit   */
   /* an arg, then run the statement list and    */
   /* leave.                                     */
   /**********************************************/
   arg->func->EvalFunc(eval, arg, status, calcTab);

   arg = arg->next;
   while(arg)
   {  
      /*****************************************************/
      /* See if current value matches.  If so, skip to the */ 
      /* statement block and get out.  If not, look at the */
      /* other data in this list                           */
      /*****************************************************/
      if(eval->data.strVal.str[0] == arg->atom.data.cVal)
      {  while(!(arg->flags&ATOM_CASE_STMT))
            arg = arg->next;
         break;
      }
      else
      {  while(!(arg->flags&ATOM_CASE_STMT))
         {  if(eval->data.strVal.str[0] == arg->atom.data.cVal)
               found = 1;
            arg = arg->next;
         }
        
         /* If found match, break out */
         if(found) break;

         /* Skip the statement block for this list */
         arg = arg->next;
      }

      /* If next argument is a statement block, it must be the default */
      if(arg)
      {  if(arg->flags&ATOM_CASE_STMT)
            break;
      }
   }

   if(arg)
   {  arg->func->EvalFunc(eval, arg, status, calcTab);

      if(arg->flags&ATOM_VAR_RETURN)
      {  if(status->errCode == 0) 
            status->errCode = RETURN_FOUND;
      }
   }
}

void EvalFor(tAtom *eval, tArg *stmtArg, tREStatus *status, tCalcTable *calcTab)
{
   tEngNum  *forLoopVar, forLimit, forStep, forInit;
   tArg     *arg = ((tArgCalc*)stmtArg->atom.data.uVal)->argList;

   /******************************************************/
   /* Evaluate first argument, which is the initializer, */
   /* the second argument, which is the limit, and the   */
   /* third argument, which is the step value.  Then     */
   /* keep looping until loop variable (fourth arg)      */
   /* plus the step value equals the limit.              */
   /******************************************************/
   /* Run the initialize expression */
   arg->func->EvalFunc(eval, arg, status, calcTab);
   NUM_SET(forInit, eval->data.num)

   /* Get the limit */
   arg->next->func->EvalFunc(eval, arg->next, status, calcTab);
   NUM_SET(forLimit, eval->data.num)

   /* Get the step value */
   arg->next->next->func->EvalFunc(eval, arg->next->next, status, calcTab);
   NUM_SET(forStep, eval->data.num)

   /* Break out now if necessary */
   if((IS_GRTR(forInit, forLimit) && IS_POS(forStep)) ||
      (IS_LESS(forInit, forLimit) && IS_NEG(forStep)))
   {  memset((char*)eval, 0, sizeof(tAtom));
      return;
   }

   /* Make sure loop doesn't have a zero step value */
   if(IS_ZERO(forStep))
   {  status->errCode = WARN_ENGINE|CALC_NODE|FOR_ZERO_STEP;
      sprintf(status->errStr, "For loop in rule '%s' will be infinite because "
                              "init is '%f', limit is '%f' and step is '%f'",
                               calcTab->currentRuleCalc->id,
                               NUM_GETFLT(forInit), NUM_GETFLT(forLimit), 
                               NUM_GETFLT(forStep)); 
      memset((char*)eval, 0, sizeof(tAtom));
      return;
   }


   /* Get the loop variable address */
   forLoopVar = (tEngNum*)arg->next->next->next->atom.data.uVal;

   arg = arg->next->next->next->next;
   while(1)
   {  if(IS_POS(forStep) && IS_GRTR((*(forLoopVar)), forLimit))
      {  NUM_SUB((*(forLoopVar)), (*(forLoopVar)), forStep)
         break;
      }
      else if(IS_NEG(forStep) && IS_LESS((*(forLoopVar)), forLimit))
      {  NUM_SUB((*(forLoopVar)), (*(forLoopVar)), forStep)
         break;
      }

      arg->func->EvalFunc(eval, arg, status, calcTab);
      if(DONE(status->errCode))
         return;

      NUM_ADD((*(forLoopVar)), (*(forLoopVar)), forStep)
   }
}

void EvalWhile(tAtom *eval, tArg *stmtArg, tREStatus *status, tCalcTable *calcTab)
{
   long  limit, loopCt=0;
   tArg  *arg = ((tArgCalc*)stmtArg->atom.data.uVal)->argList;

   /***********************************************/
   /* Evaluate first argument.  While it is TRUE, */
   /* run the secord argument.  Do this up to the */
   /* limit, which is defined in the third arg    */
   /***********************************************/
   /* Get the limit */
   limit = NUM_GETLNG(arg->next->next->atom.data.num);

   arg->func->EvalFunc(eval, arg, status, calcTab);
   while(IS_TRUE(eval->data.num) && loopCt <= limit)
   {  arg->next->func->EvalFunc(eval, arg->next, status, calcTab);
      if(DONE(status->errCode))
         return;

      arg->func->EvalFunc(eval, arg, status, calcTab);
      loopCt++;
   }

   if(loopCt > limit)
   {  status->errCode = WARN_ENGINE|CALC_NODE|WHILE_LIMIT_HIT;
      sprintf(status->errStr, "While loop limit of %ld hit in rule '%s'",
                               limit,
                               calcTab->currentRuleCalc->id); 
   }
}

void EvalReturn(tAtom *eval, tArg *stmtArg, tREStatus *status, tCalcTable *calcTab)
{
   tArg  *arg = ((tArgCalc*)stmtArg->atom.data.uVal)->argList;

   /*************************************************************/
   /* Evaluate expression, if any, and send back a RETURN_FOUND */
   /*************************************************************/
   if(arg) 
      arg->func->EvalFunc((&calcTab->retValue), arg, status, calcTab);
   else memset((char*)&calcTab->retValue, 0, sizeof(tAtom));

   status->errCode = RETURN_FOUND;
}

void EvalTerminate(tAtom *eval, tArg *stmtArg, tREStatus *status, tCalcTable *calcTab)
{
   tArg  *arg = ((tArgCalc*)stmtArg->atom.data.uVal)->argList;

   /****************************************************************/
   /* Evaluate expression, if any, and send back a TERMINATE_FOUND */
   /****************************************************************/
   if(arg)
      arg->func->EvalFunc((&calcTab->retValue), arg, status, calcTab);
   else memset((char*)&calcTab->retValue, 0, sizeof(tAtom));

   status->errCode = TERMINATE_FOUND;
}

void CalcStatements(tAtom *eval, tArg *stmtArg, tREStatus *status, tCalcTable *calcTab)
{
   stmtArg = ((tStmtCalc*)stmtArg->atom.data.uVal)->argList;

   /* Loop through all the statement calcs in the statement list */
   while(stmtArg && !DONE(status->errCode))
   {  stmtArg->func->EvalFunc(eval, stmtArg, status, calcTab);
      stmtArg = stmtArg->next;
   }
}


void CalcVarStatements(tAtom *eval, tArg *stmtArg, tREStatus *status, tCalcTable *calcTab)
{
   stmtArg = ((tStmtCalc*)stmtArg->atom.data.uVal)->argList;

   /* Loop through all the statement calcs in the statement list */
   while(stmtArg && !DONE(status->errCode))
   {  stmtArg->func->EvalFunc(eval, stmtArg, status, calcTab);
      if(stmtArg->flags&ATOM_VAR_RETURN)
      {  status->errCode = RETURN_FOUND;
         break;
      }

      stmtArg = stmtArg->next;
   }
}

