/*
** $Id: strstmt.c,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: strstmt.c,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.2  1997/01/16  22:44:06  can
 * Lots of changes for 2.1 release
 *
 * Revision 2.1  1996/04/22  17:55:53  can
 * Several fixes and changes for 2.1
 *
 *
*/

/* System includes */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* HNC Rule Engine includes */
#include "mymall.h"
#include "exptree.h"
#include "prsrule.h"
#include "lexrule.h"
#include "engerr.h"
#include "calctab.h"
#include "bltins.h"
#include "typechk.h"
#include "except.h"
#include "strstmt.h"


/*--------------------------------------------------------------------------*/
/* GetOpStr()                                                               */
/*                                                                          */
/*--------------------------------------------------------------------------*/
char* GetOpStr(tOp op)
{
   switch(op)
   {  case EQUAL: return "=";
      case LESS: return "<";
      case GRTR: return ">";
      case LESSEQL: return "<=";
      case GRTREQL: return ">=";
      case NOTEQL: return "<>";
      case ANDBOOL: return "AND";
      case ORBOOL: return "OR";
      case PLUS: return "+";
      case MINUS: return "-";
      case MULTIPLY: return "*";
      case DIVIDE: return "/";
      case SET: return ":=";
      case POWER: return "**";
      case NOOP: return "";
   }

   return "";
}


/*--------------------------------------------------------------------------*/
/* MakeSubExpression()                                                      */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void MakeSubExpression(char **buf, tExprTree *exprTree) 
{
   tParamExpr  *paramExpr;
   char        *ptr = *buf;

   if(exprTree==NULL) return;
    
   if(exprTree->left.flags&EXPR_FUNCTION)
   {  if(exprTree->left.flags&EXPR_NOT) 
      {  sprintf(ptr, "NOT(");
         ptr += strlen(ptr);
      }
      if(exprTree->left.flags&EXPR_NEGATE)
      {  sprintf(ptr, "-(");
         ptr += strlen(ptr);
      }

      sprintf(ptr, "%s(", exprTree->left.val.funcCall->funcData->functionName);
      ptr += strlen(ptr);
      paramExpr = exprTree->left.val.funcCall->paramList;
      while(paramExpr)
      {  MakeSubExpression(&ptr, paramExpr->exprTree);
         if(paramExpr->next)
         {  sprintf(ptr, ", ");
            ptr += strlen(ptr);
         }
         paramExpr = paramExpr->next;
      }
      
      sprintf(ptr, ")");
      ptr += strlen(ptr);

      if(exprTree->left.flags&EXPR_NOT) 
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
      if(exprTree->left.flags&EXPR_NEGATE)
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
   }
   else if(exprTree->left.flags&EXPR_FIELD)
   {  if(exprTree->left.flags&EXPR_NOT) 
      {  sprintf(ptr, "NOT(");
         ptr += strlen(ptr);
      }
      if(exprTree->left.flags&EXPR_NEGATE)
      {  sprintf(ptr, "-(");
         ptr += strlen(ptr);
      }

      sprintf(ptr, "%s.%s", exprTree->left.val.dataFld->recordName,
                            exprTree->left.val.dataFld->fieldName);
      ptr += strlen(ptr);

      if(exprTree->left.flags&EXPR_NOT) 
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
      if(exprTree->left.flags&EXPR_NEGATE)
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
   }
   else if(exprTree->left.flags&EXPR_VARBLE)
   {  if(exprTree->left.flags&EXPR_NOT) 
      {  sprintf(ptr, "NOT(");
         ptr += strlen(ptr);
      }
      if(exprTree->left.flags&EXPR_NEGATE)
      {  sprintf(ptr, "-(");
         ptr += strlen(ptr);
      }

      sprintf(ptr, "%s", exprTree->left.val.var->varName);
      ptr += strlen(ptr);
      
      if(exprTree->left.flags&EXPR_NOT) 
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
      if(exprTree->left.flags&EXPR_NEGATE)
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
   }
   else if(exprTree->left.flags&EXPR_IN)
   {  if(exprTree->left.flags&EXPR_NOT) 
      {  sprintf(ptr, "NOT(");
         ptr += strlen(ptr);
      }
      if(exprTree->left.flags&EXPR_NEGATE)
      {  sprintf(ptr, "-(");
         ptr += strlen(ptr);
      }

      MakeSubExpression(&ptr, exprTree->left.val.inCall->keyExpr);
      sprintf(ptr, " IN (");
      ptr += strlen(ptr);
      paramExpr = exprTree->left.val.inCall->paramList;
      while(paramExpr)
      {  MakeSubExpression(&ptr, paramExpr->exprTree);
         if(paramExpr->next) 
         {  sprintf(ptr, ", ");
            ptr += strlen(ptr);
         }
         paramExpr = paramExpr->next;
      }

      sprintf(ptr, ")");
      ptr += strlen(ptr);
      
      if(exprTree->left.flags&EXPR_NOT) 
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
      if(exprTree->left.flags&EXPR_NEGATE)
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
   }
   else if(exprTree->left.flags&EXPR_RULE)
   {  if(exprTree->left.flags&EXPR_NOT) 
      {  sprintf(ptr, "NOT(");
         ptr += strlen(ptr);
      }
      if(exprTree->left.flags&EXPR_NEGATE)
      {  sprintf(ptr, "-(");
         ptr += strlen(ptr);
      }

      sprintf(ptr, "@%s(", exprTree->left.val.ruleCall->ruleData->ruleName);
      ptr += strlen(ptr);

      paramExpr = exprTree->left.val.ruleCall->paramList;
      while(paramExpr)
      {  MakeSubExpression(&ptr, paramExpr->exprTree);
         if(paramExpr->next) 
         {  sprintf(ptr, ", ");
            ptr += strlen(ptr);
         }
         paramExpr = paramExpr->next;
      }

      sprintf(ptr, ")");
      ptr += strlen(ptr);
      
      if(exprTree->left.flags&EXPR_NOT) 
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
      if(exprTree->left.flags&EXPR_NEGATE)
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
   }
   else if(exprTree->left.flags&EXPR_TREE)
   {  if(exprTree->left.flags&EXPR_NOT) 
      {  sprintf(ptr, "NOT(");
         ptr += strlen(ptr);
      }
      if(exprTree->left.flags&EXPR_NEGATE)
      {  sprintf(ptr, "-(");
         ptr += strlen(ptr);
      }

      sprintf(ptr, "(");
      ptr += strlen(ptr);
      MakeSubExpression(&ptr, exprTree->left.val.tree);
      sprintf(ptr, ")");
      ptr += strlen(ptr);

      if(exprTree->left.flags&EXPR_NOT) 
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
      if(exprTree->left.flags&EXPR_NEGATE)
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
   }
   else if(IsNumeric(exprTree->left.type))
   {  if(exprTree->left.flags&EXPR_NOT) 
      {  sprintf(ptr, "NOT(");
         ptr += strlen(ptr);
      }
      if(exprTree->left.flags&EXPR_NEGATE)
      {  sprintf(ptr, "-(");
         ptr += strlen(ptr);
      }

      sprintf(ptr, "%g", NUM_GETFLT(exprTree->left.val.num));
      ptr += strlen(ptr);

      if(exprTree->left.flags&EXPR_NOT) 
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
      if(exprTree->left.flags&EXPR_NEGATE)
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
   }
   else if(IsString(exprTree->left.type))
   {  if(exprTree->left.flags&EXPR_NOT) 
      {  sprintf(ptr, "NOT(");
         ptr += strlen(ptr);
      }

      if(exprTree->left.type == HNC_CHAR || strlen(exprTree->left.val.str)==1)
      {  sprintf(ptr, "'%c'", exprTree->left.val.str[0]);
         ptr += strlen(ptr);
      }
      else
      {  sprintf(ptr, "\"%s\"", exprTree->left.val.str);
         ptr += strlen(ptr);
      }

      if(exprTree->left.flags&EXPR_NOT) 
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
   }

   if(exprTree->op == NOOP)
   {  *buf = ptr;
      return;
   }

   sprintf(ptr, " %s ", GetOpStr(exprTree->op));
   ptr += strlen(ptr);

   if(exprTree->right.flags&EXPR_FUNCTION)
   {  if(exprTree->right.flags&EXPR_NOT) 
      {  sprintf(ptr, "NOT(");
         ptr += strlen(ptr);
      }
      if(exprTree->right.flags&EXPR_NEGATE)
      {  sprintf(ptr, "-(");
         ptr += strlen(ptr);
      }

      sprintf(ptr, "%s(", exprTree->right.val.funcCall->funcData->functionName);
      ptr += strlen(ptr);
      paramExpr = exprTree->right.val.funcCall->paramList;
      while(paramExpr)
      {  MakeSubExpression(&ptr, paramExpr->exprTree);
         if(paramExpr->next)
         {  sprintf(ptr, ", ");
            ptr += strlen(ptr);
         }
         paramExpr = paramExpr->next;
      }
      
      sprintf(ptr, ")");
      ptr += strlen(ptr);

      if(exprTree->right.flags&EXPR_NOT) 
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
      if(exprTree->right.flags&EXPR_NEGATE)
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
   }
   else if(exprTree->right.flags&EXPR_FIELD)
   {  if(exprTree->right.flags&EXPR_NOT) 
      {  sprintf(ptr, "NOT(");
         ptr += strlen(ptr);
      }
      if(exprTree->right.flags&EXPR_NEGATE)
      {  sprintf(ptr, "-(");
         ptr += strlen(ptr);
      }

      sprintf(ptr, "%s.%s", exprTree->right.val.dataFld->recordName,
                            exprTree->right.val.dataFld->fieldName);
      ptr += strlen(ptr);

      if(exprTree->right.flags&EXPR_NOT) 
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
      if(exprTree->right.flags&EXPR_NEGATE)
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
   }
   else if(exprTree->right.flags&EXPR_VARBLE)
   {  if(exprTree->right.flags&EXPR_NOT) 
      {  sprintf(ptr, "NOT(");
         ptr += strlen(ptr);
      }
      if(exprTree->right.flags&EXPR_NEGATE)
      {  sprintf(ptr, "-(");
         ptr += strlen(ptr);
      }

      sprintf(ptr, "%s", exprTree->right.val.var->varName);
      ptr += strlen(ptr);
      
      if(exprTree->right.flags&EXPR_NOT) 
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
      if(exprTree->right.flags&EXPR_NEGATE)
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
   }
   else if(exprTree->right.flags&EXPR_IN)
   {  if(exprTree->right.flags&EXPR_NOT) 
      {  sprintf(ptr, "NOT(");
         ptr += strlen(ptr);
      }
      if(exprTree->right.flags&EXPR_NEGATE)
      {  sprintf(ptr, "-(");
         ptr += strlen(ptr);
      }

      MakeSubExpression(&ptr, exprTree->right.val.inCall->keyExpr);
      sprintf(ptr, " IN (");
      ptr += strlen(ptr);
      paramExpr = exprTree->right.val.inCall->paramList;
      while(paramExpr)
      {  MakeSubExpression(&ptr, paramExpr->exprTree);
         if(paramExpr->next) 
         {  sprintf(ptr, ", ");
            ptr += strlen(ptr);
         }
         paramExpr = paramExpr->next;
      }

      sprintf(ptr, ")");
      ptr += strlen(ptr);
      
      if(exprTree->right.flags&EXPR_NOT) 
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
      if(exprTree->right.flags&EXPR_NEGATE)
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
   }
   else if(exprTree->right.flags&EXPR_RULE)
   {  if(exprTree->right.flags&EXPR_NOT) 
      {  sprintf(ptr, "NOT(");
         ptr += strlen(ptr);
      }
      if(exprTree->right.flags&EXPR_NEGATE)
      {  sprintf(ptr, "-(");
         ptr += strlen(ptr);
      }

      sprintf(ptr, "@%s(", exprTree->right.val.ruleCall->ruleData->ruleName);
      ptr += strlen(ptr);

      paramExpr = exprTree->right.val.ruleCall->paramList;
      while(paramExpr)
      {  MakeSubExpression(&ptr, paramExpr->exprTree);
         if(paramExpr->next) 
         {  sprintf(ptr, ", ");
            ptr += strlen(ptr);
         }
         paramExpr = paramExpr->next;
      }

      sprintf(ptr, ")");
      ptr += strlen(ptr);
      
      if(exprTree->right.flags&EXPR_NOT) 
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
      if(exprTree->right.flags&EXPR_NEGATE)
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
   }
   else if(exprTree->right.flags&EXPR_TREE)
   {  if(exprTree->right.flags&EXPR_NOT) 
      {  sprintf(ptr, "NOT(");
         ptr += strlen(ptr);
      }
      if(exprTree->right.flags&EXPR_NEGATE)
      {  sprintf(ptr, "-(");
         ptr += strlen(ptr);
      }

      sprintf(ptr, "(");
      ptr += strlen(ptr);
      MakeSubExpression(&ptr, exprTree->right.val.tree);
      sprintf(ptr, ")");
      ptr += strlen(ptr);

      if(exprTree->right.flags&EXPR_NOT) 
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
      if(exprTree->right.flags&EXPR_NEGATE)
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
   }
   else if(IsNumeric(exprTree->right.type))
   {  if(exprTree->right.flags&EXPR_NOT) 
      {  sprintf(ptr, "NOT(");
         ptr += strlen(ptr);
      }
      if(exprTree->right.flags&EXPR_NEGATE)
      {  sprintf(ptr, "-(");
         ptr += strlen(ptr);
      }

      sprintf(ptr, "%g", NUM_GETFLT(exprTree->right.val.num));
      ptr += strlen(ptr);

      if(exprTree->right.flags&EXPR_NOT) 
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
      if(exprTree->right.flags&EXPR_NEGATE)
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
   }
   else if(IsString(exprTree->right.type))
   {  if(exprTree->right.flags&EXPR_NOT) 
      {  sprintf(ptr, "NOT(");
         ptr += strlen(ptr);
      }

      if(exprTree->right.type == HNC_CHAR || strlen(exprTree->right.val.str)==1)
      {  sprintf(ptr, "'%c'", exprTree->right.val.str[0]);
         ptr += strlen(ptr);
      }
      else
      {  sprintf(ptr, "\"%s\"", exprTree->right.val.str);
         ptr += strlen(ptr);
      }

      if(exprTree->right.flags&EXPR_NOT) 
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
   }

   *buf = ptr;
}


/*--------------------------------------------------------------------------*/
/* MakeExpression()                                                         */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void MakeExpression(char *buf, tExprTree *exprTree) 
{
   tParamExpr  *paramExpr;
   char        *ptr = buf; 

   if(exprTree==NULL) return;
    
   if(exprTree->left.flags&EXPR_FUNCTION)
   {  if(exprTree->left.flags&EXPR_NOT) 
      {  sprintf(ptr, "NOT(");
         ptr += strlen(ptr);
      }
      if(exprTree->left.flags&EXPR_NEGATE)
      {  sprintf(ptr, "-(");
         ptr += strlen(ptr);
      }

      sprintf(ptr, "%s(", exprTree->left.val.funcCall->funcData->functionName);
      ptr += strlen(ptr);
      paramExpr = exprTree->left.val.funcCall->paramList;
      while(paramExpr)
      {  MakeSubExpression(&ptr, paramExpr->exprTree);
         if(paramExpr->next)
         {  sprintf(ptr, ", ");
            ptr += strlen(ptr);
         }
         paramExpr = paramExpr->next;
      }
      
      sprintf(ptr, ")");
      ptr += strlen(ptr);

      if(exprTree->left.flags&EXPR_NOT) 
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
      if(exprTree->left.flags&EXPR_NEGATE)
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
   }
   else if(exprTree->left.flags&EXPR_FIELD)
   {  if(exprTree->left.flags&EXPR_NOT) 
      {  sprintf(ptr, "NOT(");
         ptr += strlen(ptr);
      }
      if(exprTree->left.flags&EXPR_NEGATE)
      {  sprintf(ptr, "-(");
         ptr += strlen(ptr);
      }

      sprintf(ptr, "%s.%s", exprTree->left.val.dataFld->recordName,
                            exprTree->left.val.dataFld->fieldName);
      ptr += strlen(ptr);

      if(exprTree->left.flags&EXPR_NOT) 
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
      if(exprTree->left.flags&EXPR_NEGATE)
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
   }
   else if(exprTree->left.flags&EXPR_VARBLE)
   {  if(exprTree->left.flags&EXPR_NOT) 
      {  sprintf(ptr, "NOT(");
         ptr += strlen(ptr);
      }
      if(exprTree->left.flags&EXPR_NEGATE)
      {  sprintf(ptr, "-(");
         ptr += strlen(ptr);
      }

      sprintf(ptr, "%s", exprTree->left.val.var->varName);
      ptr += strlen(ptr);
      
      if(exprTree->left.flags&EXPR_NOT) 
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
      if(exprTree->left.flags&EXPR_NEGATE)
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
   }
   else if(exprTree->left.flags&EXPR_IN)
   {  if(exprTree->left.flags&EXPR_NOT) 
      {  sprintf(ptr, "NOT(");
         ptr += strlen(ptr);
      }
      if(exprTree->left.flags&EXPR_NEGATE)
      {  sprintf(ptr, "-(");
         ptr += strlen(ptr);
      }

      MakeSubExpression(&ptr, exprTree->left.val.inCall->keyExpr);
      sprintf(ptr, " IN (");
      ptr += strlen(ptr);
      paramExpr = exprTree->left.val.inCall->paramList;
      while(paramExpr)
      {  MakeSubExpression(&ptr, paramExpr->exprTree);
         if(paramExpr->next) 
         {  sprintf(ptr, ", ");
            ptr += strlen(ptr);
         }
         paramExpr = paramExpr->next;
      }

      sprintf(ptr, ")");
      ptr += strlen(ptr);
      
      if(exprTree->left.flags&EXPR_NOT) 
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
      if(exprTree->left.flags&EXPR_NEGATE)
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
   }
   else if(exprTree->left.flags&EXPR_RULE)
   {  if(exprTree->left.flags&EXPR_NOT) 
      {  sprintf(ptr, "NOT(");
         ptr += strlen(ptr);
      }
      if(exprTree->left.flags&EXPR_NEGATE)
      {  sprintf(ptr, "-(");
         ptr += strlen(ptr);
      }

      sprintf(ptr, "@%s(", exprTree->left.val.ruleCall->ruleData->ruleName);
      ptr += strlen(ptr);

      paramExpr = exprTree->left.val.ruleCall->paramList;
      while(paramExpr)
      {  MakeSubExpression(&ptr, paramExpr->exprTree);
         if(paramExpr->next) 
         {  sprintf(ptr, ", ");
            ptr += strlen(ptr);
         }
         paramExpr = paramExpr->next;
      }

      sprintf(ptr, ")");
      ptr += strlen(ptr);
      
      if(exprTree->left.flags&EXPR_NOT) 
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
      if(exprTree->left.flags&EXPR_NEGATE)
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
   }
   else if(exprTree->left.flags&EXPR_TREE)
   {  if(exprTree->left.flags&EXPR_NOT) 
      {  sprintf(ptr, "NOT(");
         ptr += strlen(ptr);
      }
      if(exprTree->left.flags&EXPR_NEGATE)
      {  sprintf(ptr, "-(");
         ptr += strlen(ptr);
      }

      sprintf(ptr, "(");
      ptr += strlen(ptr);
      MakeSubExpression(&ptr, exprTree->left.val.tree);
      sprintf(ptr, ")");
      ptr += strlen(ptr);

      if(exprTree->left.flags&EXPR_NOT) 
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
      if(exprTree->left.flags&EXPR_NEGATE)
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
   }
   else if(IsNumeric(exprTree->left.type))
   {  if(exprTree->left.flags&EXPR_NOT) 
      {  sprintf(ptr, "NOT(");
         ptr += strlen(ptr);
      }
      if(exprTree->left.flags&EXPR_NEGATE)
      {  sprintf(ptr, "-(");
         ptr += strlen(ptr);
      }

      sprintf(ptr, "%g", NUM_GETFLT(exprTree->left.val.num));
      ptr += strlen(ptr);

      if(exprTree->left.flags&EXPR_NOT) 
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
      if(exprTree->left.flags&EXPR_NEGATE)
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
   }
   else if(IsString(exprTree->left.type))
   {  if(exprTree->left.flags&EXPR_NOT) 
      {  sprintf(ptr, "NOT(");
         ptr += strlen(ptr);
      }

      if(exprTree->left.type == HNC_CHAR || strlen(exprTree->left.val.str)==1)
      {  sprintf(ptr, "'%c'", exprTree->left.val.str[0]);
         ptr += strlen(ptr);
      }
      else
      {  sprintf(ptr, "\"%s\"", exprTree->left.val.str);
         ptr += strlen(ptr);
      }

      if(exprTree->left.flags&EXPR_NOT) 
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
   }

   if(exprTree->op == NOOP)
      return;

   sprintf(ptr, " %s ", GetOpStr(exprTree->op));
   ptr += strlen(ptr);

   if(exprTree->right.flags&EXPR_FUNCTION)
   {  if(exprTree->right.flags&EXPR_NOT) 
      {  sprintf(ptr, "NOT(");
         ptr += strlen(ptr);
      }
      if(exprTree->right.flags&EXPR_NEGATE)
      {  sprintf(ptr, "-(");
         ptr += strlen(ptr);
      }

      sprintf(ptr, "%s(", exprTree->right.val.funcCall->funcData->functionName);
      ptr += strlen(ptr);
      paramExpr = exprTree->right.val.funcCall->paramList;
      while(paramExpr)
      {  MakeSubExpression(&ptr, paramExpr->exprTree);
         if(paramExpr->next)
         {  sprintf(ptr, ", ");
            ptr += strlen(ptr);
         }
         paramExpr = paramExpr->next;
      }
      
      sprintf(ptr, ")");
      ptr += strlen(ptr);

      if(exprTree->right.flags&EXPR_NOT) 
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
      if(exprTree->right.flags&EXPR_NEGATE)
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
   }
   else if(exprTree->right.flags&EXPR_FIELD)
   {  if(exprTree->right.flags&EXPR_NOT) 
      {  sprintf(ptr, "NOT(");
         ptr += strlen(ptr);
      }
      if(exprTree->right.flags&EXPR_NEGATE)
      {  sprintf(ptr, "-(");
         ptr += strlen(ptr);
      }

      sprintf(ptr, "%s.%s", exprTree->right.val.dataFld->recordName,
                            exprTree->right.val.dataFld->fieldName);
      ptr += strlen(ptr);

      if(exprTree->right.flags&EXPR_NOT) 
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
      if(exprTree->right.flags&EXPR_NEGATE)
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
   }
   else if(exprTree->right.flags&EXPR_VARBLE)
   {  if(exprTree->right.flags&EXPR_NOT) 
      {  sprintf(ptr, "NOT(");
         ptr += strlen(ptr);
      }
      if(exprTree->right.flags&EXPR_NEGATE)
      {  sprintf(ptr, "-(");
         ptr += strlen(ptr);
      }

      sprintf(ptr, "%s", exprTree->right.val.var->varName);
      ptr += strlen(ptr);
      
      if(exprTree->right.flags&EXPR_NOT) 
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
      if(exprTree->right.flags&EXPR_NEGATE)
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
   }
   else if(exprTree->right.flags&EXPR_IN)
   {  if(exprTree->right.flags&EXPR_NOT) 
      {  sprintf(ptr, "NOT(");
         ptr += strlen(ptr);
      }
      if(exprTree->right.flags&EXPR_NEGATE)
      {  sprintf(ptr, "-(");
         ptr += strlen(ptr);
      }

      MakeSubExpression(&ptr, exprTree->right.val.inCall->keyExpr);
      sprintf(ptr, " IN (");
      ptr += strlen(ptr);
      paramExpr = exprTree->right.val.inCall->paramList;
      while(paramExpr)
      {  MakeSubExpression(&ptr, paramExpr->exprTree);
         if(paramExpr->next) 
         {  sprintf(ptr, ", ");
            ptr += strlen(ptr);
         }
         paramExpr = paramExpr->next;
      }

      sprintf(ptr, ")");
      ptr += strlen(ptr);
      
      if(exprTree->right.flags&EXPR_NOT) 
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
      if(exprTree->right.flags&EXPR_NEGATE)
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
   }
   else if(exprTree->right.flags&EXPR_RULE)
   {  if(exprTree->right.flags&EXPR_NOT) 
      {  sprintf(ptr, "NOT(");
         ptr += strlen(ptr);
      }
      if(exprTree->right.flags&EXPR_NEGATE)
      {  sprintf(ptr, "-(");
         ptr += strlen(ptr);
      }

      sprintf(ptr, "@%s(", exprTree->right.val.ruleCall->ruleData->ruleName);
      ptr += strlen(ptr);

      paramExpr = exprTree->right.val.ruleCall->paramList;
      while(paramExpr)
      {  MakeSubExpression(&ptr, paramExpr->exprTree);
         if(paramExpr->next) 
         {  sprintf(ptr, ", ");
            ptr += strlen(ptr);
         }
         paramExpr = paramExpr->next;
      }

      sprintf(ptr, ")");
      ptr += strlen(ptr);
      
      if(exprTree->right.flags&EXPR_NOT) 
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
      if(exprTree->right.flags&EXPR_NEGATE)
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
   }
   else if(exprTree->right.flags&EXPR_TREE)
   {  if(exprTree->right.flags&EXPR_NOT) 
      {  sprintf(ptr, "NOT(");
         ptr += strlen(ptr);
      }
      if(exprTree->right.flags&EXPR_NEGATE)
      {  sprintf(ptr, "-(");
         ptr += strlen(ptr);
      }

      sprintf(ptr, "(");
      ptr += strlen(ptr);
      MakeSubExpression(&ptr, exprTree->right.val.tree);
      sprintf(ptr, ")");
      ptr += strlen(ptr);

      if(exprTree->right.flags&EXPR_NOT) 
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
      if(exprTree->right.flags&EXPR_NEGATE)
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
   }
   else if(IsNumeric(exprTree->right.type))
   {  if(exprTree->right.flags&EXPR_NOT) 
      {  sprintf(ptr, "NOT(");
         ptr += strlen(ptr);
      }
      if(exprTree->right.flags&EXPR_NEGATE)
      {  sprintf(ptr, "-(");
         ptr += strlen(ptr);
      }

      sprintf(ptr, "%g", NUM_GETFLT(exprTree->right.val.num));
      ptr += strlen(ptr);

      if(exprTree->right.flags&EXPR_NOT) 
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
      if(exprTree->right.flags&EXPR_NEGATE)
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
   }
   else if(IsString(exprTree->right.type))
   {  if(exprTree->right.flags&EXPR_NOT) 
      {  sprintf(ptr, "NOT(");
         ptr += strlen(ptr);
      }

      if(exprTree->right.type == HNC_CHAR || strlen(exprTree->right.val.str)==1)
      {  sprintf(ptr, "'%c'", exprTree->right.val.str[0]);
         ptr += strlen(ptr);
      }
      else
      {  sprintf(ptr, "\"%s\"", exprTree->right.val.str);
         ptr += strlen(ptr);
      }

      if(exprTree->right.flags&EXPR_NOT) 
      {  sprintf(ptr, ")");
         ptr += strlen(ptr);
      }
   }

}


/*--------------------------------------------------------------------------*/
/* MakeWhile()                                                              */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void MakeWhile(char *buf, tWhileData *whileData)
{
   char      buf1[4096];
   
   MakeExpression(buf1, whileData->condExpr);

   sprintf(buf, "WHILE %s DO", buf1);
}


/*--------------------------------------------------------------------------*/
/* MakeFor()                                                                */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void MakeFor(char *buf, tForData *forData)
{
   char      buf1[4096];
   char      buf2[4096];
   char      buf3[4096];
   
   MakeExpression(buf1, forData->initExpr);
   MakeExpression(buf2, forData->limitExpr);
   MakeExpression(buf3, forData->stepExpr);

   if(forData->direction == COUNT_UP)
      sprintf(buf, "FOR %s TO %s STEP %s DO", buf1, buf2, buf3);
   else
      sprintf(buf, "FOR %s DOWNTO %s STEP %s DO", buf1, buf2, buf3);
}


/*--------------------------------------------------------------------------*/
/* MakeTerminate()                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void MakeTerminate(char *buf, tExprTree *termExpr)
{
   char      buf1[4096];
   
   if(termExpr)
   {  MakeExpression(buf1, termExpr);
      sprintf(buf, "TERMINATE %s", buf1);
   }
   else
      sprintf(buf, "TERMINATE");
}


/*--------------------------------------------------------------------------*/
/* MakeReturn()                                                             */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void MakeReturn(char *buf, tExprTree *retExpr)
{
   char      buf1[4096];
   
   if(retExpr)
   {  MakeExpression(buf1, retExpr);
      sprintf(buf, "RETURN %s", buf1);
   }
   else
      sprintf(buf, "RETURN");
}


/*--------------------------------------------------------------------------*/
/* MakeCase()                                                               */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void MakeCase(tStmtText **textList, tCaseData *caseData)
{
   char      *ptr, buf1[4096], buf2[4096];
   tStmtText *newText;
   tCase     *tmpCase;
   tCaseArg  *tmpArg;

   MakeExpression(buf1, caseData->caseExpr);
   
   if((newText = (tStmtText*)MyMalloc(sizeof(tStmtText)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)newText, 0, sizeof(tStmtText));
   *textList = newText;

   sprintf(buf2, "CASE %s OF", buf1);
   if((newText->text = (char*)MyMalloc(strlen(buf2)+1))==NULL)
      throw(MALLOC_EXCEPTION);
   strcpy(newText->text, buf2);

   for(tmpCase=caseData->caseList; tmpCase; tmpCase=tmpCase->next)
   {  sprintf(buf1, "\t");
      ptr = buf1;
      ptr += strlen(ptr);
      for(tmpArg=tmpCase->valList; tmpArg; tmpArg=tmpArg->next) 
      {  if(tmpArg->atom.type == NUMERIC_TYPE)
         {  if(tmpArg==tmpCase->valList)
               sprintf(ptr, "%ld", NUM_GETLNG(tmpArg->atom.data.num));
            else
               sprintf(ptr, ",%ld", NUM_GETLNG(tmpArg->atom.data.num));
         }
         else
         {  if(tmpArg==tmpCase->valList)
               sprintf(ptr, "'%c'", tmpArg->atom.data.cVal);
            else
               sprintf(ptr, ",'%c'", tmpArg->atom.data.cVal);
         }

         ptr += strlen(ptr);
      }

      sprintf(ptr, ":");

      if((newText->next = (tStmtText*)MyMalloc(sizeof(tStmtText)))==NULL)
         throw(MALLOC_EXCEPTION);
      newText = newText->next;
      memset((char*)newText, 0, sizeof(tStmtText));

      if((newText->text = (char*)MyMalloc(strlen(buf1)+1))==NULL)
         throw(MALLOC_EXCEPTION);
      strcpy(newText->text, buf1);
   }
}
 

/*--------------------------------------------------------------------------*/
/* MakeIf()                                                                 */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void MakeIf(tStmtText **textList, tIfData *ifData, int isElse)
{
   char      buf1[4096], buf2[4096];
   tStmtText *newText;

   MakeExpression(buf1, ifData->condExpr);

   if((newText = (tStmtText*)MyMalloc(sizeof(tStmtText)))==NULL)
      throw(MALLOC_EXCEPTION);
   newText->next = NULL;
   *textList = newText;

   if(isElse)
      sprintf(buf2, "ELSE IF %s THEN", buf1);
   else
      sprintf(buf2, "IF %s THEN", buf1);

   if((newText->text = (char*)MyMalloc(strlen(buf2)+1))==NULL)
      throw(MALLOC_EXCEPTION);
   strcpy(newText->text, buf2);
      

   if(ifData->secondaryType == ELSE_STATEMENT)
   {  if((newText->next = (tStmtText*)MyMalloc(sizeof(tStmtText)))==NULL)
         throw(MALLOC_EXCEPTION);
      newText = newText->next;
      newText->next = NULL;

      sprintf(buf1, "ELSE");
      if((newText->text = (char*)MyMalloc(strlen(buf1)+1))==NULL)
         throw(MALLOC_EXCEPTION);
      strcpy(newText->text, buf1);
   }
}

   
/*--------------------------------------------------------------------------*/
/* MakeStatementText()                                                      */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long MakeStatementText(tStmtText **textList, tStatement *stmt)
{   
   char       buf[4096];

   switch(stmt->type) 
   {  case IF_STATEMENT:
         MakeIf(textList, stmt->data.ifData, FALSE);
         break;
      case CASE_STATEMENT:
         MakeCase(textList, stmt->data.caseData);
         break;
      case EXPR_STATEMENT:
         MakeExpression(buf, stmt->data.exprTree);
         break;
      case RETURN_STATEMENT:
         MakeReturn(buf, stmt->data.exprTree);
         break;
      case TERM_STATEMENT:
         MakeTerminate(buf, stmt->data.exprTree);
         break;
      case FOR_STATEMENT:
         MakeFor(buf, stmt->data.forData);
         break;
      case WHILE_STATEMENT:
         MakeWhile(buf, stmt->data.whileData);
         break;
   }

   if(*textList==NULL)
   {  if((*textList = (tStmtText*)MyMalloc(sizeof(tStmtText)))==NULL)
         throw(MALLOC_EXCEPTION);

      if(((*textList)->text = (char*)MyMalloc(strlen(buf)+1))==NULL)
         throw(MALLOC_EXCEPTION);

      strcpy((*textList)->text, buf);
      (*textList)->next = NULL;
   }

   return(OK);
}
