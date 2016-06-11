/*
** $Id: exptree.c,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: exptree.c,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.3  1997/02/05  03:31:22  can
 * Changed malloc of a tree to MyTmpMalloc
 *
 * Revision 2.2  1997/01/18  00:33:41  can
 * Made sure addrExpr in the resolve structure is freed if necessary
 *
 * Revision 2.1  1997/01/16  22:44:06  can
 * Lots of changes for 2.1 release
 *
 * Revision 2.0  1996/03/18  18:45:44  gak
 * GUTS 2.0.1 alpha release
 *
 * Revision 1.7  1995/08/25  01:36:46  can
 * Fixed compile warning messages
 *
 * Revision 1.6  1995/08/16  17:59:31  can
 * Made sure data is free'd properly
 *
 * Revision 1.5  1995/08/10  00:03:59  can
 * Added check to see if a branch is a variable before freeing the
 * string for that variable.
 *
 * Revision 1.4  1995/07/21  15:35:44  can
 * Modified clean-up routines.  Added code to handle use of variables.
 *
 * Revision 1.3  1995/07/16  18:24:58  can
 * Added check to make sure that a single expression statement does
 * something, i.e. must set a value, call a function, or call a rule.
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
#include <string.h>

/* Rules engine include files */
#include "mymall.h"
#include "engerr.h"
#include "exptree.h"
#include "typechk.h"

/* HNC common include files */
#include "except.h"


long FreeExprBranch(tExprBranch exprBranch);
long FreeParamList(tParamExpr *paramList);

/*--------------------------------------------------------------------------*/
/* AllocateExprTree()                                                       */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long AllocateExprTree(tExprTree **exprTree)
{
   if((*exprTree = (tExprTree*)MyTmpMalloc(sizeof(tExprTree)))==NULL)
      throw(MALLOC_EXCEPTION);

   memset((char*)*exprTree, 0, sizeof(tExprTree));
   
   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CheckExpressionFunction()                                                */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long CheckExpressionFunction(tExprTree *exprTree)
{
   /****************************************/
   /* Make sure the tree is a good pointer */
   /****************************************/
   if(!exprTree)
      return(OK);

   /******************************************/
   /* Check to see if the operation is a SET */
   /******************************************/
   if(exprTree->op == SET)
      return(OK);

   /*********************************************************/
   /* If operation was an EQUAL and the left operand was an */
   /* lvalue, then clue as maybe forgot the ":" from ":="   */
   /*********************************************************/
   if(exprTree->op == EQUAL && 
      (exprTree->left.flags&EXPR_FIELD || exprTree->left.flags&EXPR_VARBLE))
      return(EQUAL_NOT_SET);

   /****************************************/
   /* Check for function call or rule call */   
   /****************************************/
   if((exprTree->left.flags&EXPR_FUNCTION || exprTree->left.flags&EXPR_RULE)
       && exprTree->op == NOOP) 
      return(OK);
    
   return(EXPR_NO_FUNCTION);
}


/*--------------------------------------------------------------------------*/
/* FreeParamList()                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long FreeParamList(tParamExpr *paramList)
{
   tParamExpr *tmpParam, *delParam;

   tmpParam = paramList;
   while(tmpParam)
   {  FreeExprTree(tmpParam->exprTree); 

      delParam = tmpParam;
      tmpParam = tmpParam->next;
      MyFree((void*)delParam);
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* FreeExprBranch()                                                         */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long FreeExprBranch(tExprBranch exprBranch)
{
   /******************************************************/
   /* Free allocated strings, structures for rule calls, */
   /* function calls, etc.                               */
   /******************************************************/
   if(exprBranch.flags&EXPR_FIELD)
   {  if(!exprBranch.val.dataFld)
         return(OK);
      else if(!exprBranch.val.dataFld->noFree || exprBranch.flags&EXPR_UNUSED)
      {  if(!exprBranch.val.dataFld->arrayProc)
         {  MyFree((void*)exprBranch.val.dataFld->fieldName); 
            MyFree((void*)exprBranch.val.dataFld->recordName); 
            MyFree((void*)exprBranch.val.dataFld);
         }
         else 
         {  tArrayProc *array, *delArray;
            tRecUse    *recUse = exprBranch.val.dataFld;

            array = recUse->arrayProc;
            while(array)
            {  delArray = array;
               array = array->next;

               if(delArray->arrayUse)
               {  FreeExprTree((tExprTree*)delArray->arrayUse->indexExpr);
                  MyFree((void*)delArray->arrayUse);
               }

               MyFree((void*)delArray);
            }

            if(recUse->fldData->flags&FIELD_DUMMY)
            MyFree((void*)recUse->fldData);
            recUse->arrayProc = NULL;

            MyFree((void*)exprBranch.val.dataFld->fieldName); 
            MyFree((void*)exprBranch.val.dataFld->recordName); 
            MyFree((void*)exprBranch.val.dataFld);
         }
      }
   }
   else if(exprBranch.flags & EXPR_FUNCTION)
   {  if(exprBranch.val.funcCall)
      {  FreeParamList(exprBranch.val.funcCall->paramList);
         MyFree((void*)exprBranch.val.funcCall);
      }
   }
   else if(exprBranch.flags & EXPR_RESOLVE)
   {  if(exprBranch.val.resolve)
      {  FreeExprTree(exprBranch.val.resolve->lenExpr);
         if(exprBranch.val.resolve->addrExpr)
            FreeExprTree(exprBranch.val.resolve->addrExpr);

         MyFree((void*)exprBranch.val.resolve);
      }
   }
   else if(exprBranch.flags & EXPR_SIZEOF)
   {  if(exprBranch.val.sizeOp)
         MyFree((void*)exprBranch.val.sizeOp);
   }
   else if(exprBranch.flags & EXPR_IN)
   {  if(exprBranch.val.inCall)
      {  FreeExprTree(exprBranch.val.inCall->keyExpr);
         FreeParamList(exprBranch.val.inCall->paramList);
         MyFree((void*)exprBranch.val.inCall);
      }
   }
   else if(exprBranch.flags & EXPR_RULE)
   {  if(exprBranch.val.ruleCall)
      {  FreeParamList(exprBranch.val.ruleCall->paramList);
         MyFree((void*)exprBranch.val.ruleCall);
      }
   }
   else if(exprBranch.flags & EXPR_VARBLE)
      /* Do nothing */;
   else if(exprBranch.flags & EXPR_CONSTANT)
   {  if(IsString(exprBranch.type))
         MyFree((void*)exprBranch.val.str);
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* FreeExprTree()                                                           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long FreeExprTree(tExprTree *exprTree)
{
   /*******************************************/
   /* Make sure there is a tree to free first */
   /*******************************************/
   if(!exprTree)
      return(OK);

   /********************************************/
   /* Free left branch sub-expressions, if any */
   /********************************************/
   if((exprTree->left.flags&EXPR_TREE) && exprTree->left.val.tree!=NULL) 
      FreeExprTree(exprTree->left.val.tree);
   else
      FreeExprBranch(exprTree->left);
  
   /*********************************************/
   /* Free right branch sub-expressions, if any */
   /*********************************************/
   if((exprTree->right.flags&EXPR_TREE) && exprTree->right.val.tree!=NULL) 
      FreeExprTree(exprTree->right.val.tree);
   else if(exprTree->op != NOOP)
      FreeExprBranch(exprTree->right);

   MyFree((void*)exprTree);

   return(OK);
}
