/* ** $Id: prsrule.c,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: prsrule.c,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.12  1997/03/14  22:15:16  can
 * Fixed bug caused by last fix dealing with template lists and multiple rulebases
 *
 * Revision 2.11  1997/03/05  20:23:44  can
 * Fixed bug in CreateVariablesFromTemplates where it assumed that all
 * rule bases used the same template list.
 *
 * Revision 2.10  1997/02/12  00:06:25  can
 * Modified to ensure cleanup of all memory when multiple rulebases
 * are used.
 *
 * Revision 2.9  1997/02/05  03:31:22  can
 * Changed all malloc calls to MyTmpMalloc.
 *
 * Revision 2.8  1997/01/20  18:36:06  can
 * Made TERMINATE act the same as RETURN for exit statement checking
 *
 * Revision 2.7  1997/01/16  22:44:06  can
 * Lots of changes for 2.1 release
 *
 * Revision 2.4  1996/06/13  22:06:01  can
 * Added function DoOpSwap to make sure that "like" operands, i.e.
 * + and -, * and /, AND and OR, and =,<,>,<>,<=,>= are indeed
 * treated the same to ensure that left to right performance of
 * like operations is performed
 *
 * Revision 2.3  1996/06/12  18:44:31  can
 * Fixed bug in ParseTerminateStatement that wouldn't allow a
 * TERMINATE to without an expression.
 *
 * Revision 2.2  1996/05/01  01:18:53  can
 * Added GetFieldData function
 *
 * Revision 1.22  1995/10/26  19:24:03  can
 * Fixed parameters so that memory is only allocated for binary
 * data.  String data will always just get the data pointer.
 *
 * Revision 1.21  1995/09/20  17:17:27  can
 * Fixed function parameter check to compare argNum against
 * numParams, and not numParams-1.  I'm not sure why I had
 * the -1 in there!
 *
 * Revision 1.20  1995/09/05  22:25:41  can
 * Fixed bug in processing expressions with parentheses
 *
 * Revision 1.19  1995/09/01  22:30:52  wle
 * Made sure severities get compared properly (WARNING is numerically higher than
 * FATA, which is against logic, since FATAL is "higher" e
 * severity.
 *
 * Revision 1.18  1995/09/01  01:39:44  wle
 * Made sure memory is set to 0 after being MyTmpMalloced.
 *
 * Revision 1.17  1995/08/31  00:05:52  can
 * Fixed more compiler warnings.
 *
 * Revision 1.16  1995/08/25  01:25:48  can
 * Fixed compile warning messages
 *
 * Revision 1.15  1995/08/23  17:23:53  can
 *  Made all lines less than 100 bytes
 *
 * Revision 1.14  1995/08/16  17:59:31  can
 * Made changes to ensure all MyTmpMalloced data is free'd
 *
 * Revision 1.13  1995/08/11  01:40:04  can
 * Added variable to list of allData.
 *
 * Revision 1.12  1995/08/08  22:45:15  can
 * Fixed bug in parsing first statement after VAR block
 *
 * Revision 1.11  1995/08/08  16:56:03  can
 * Added fields to make sure RETURN statements exist for
 * rules that have a return type
 *
 * Revision 1.10  1995/08/04  21:38:52  can
 * Added ability to put a ; before an ELSE, unlike Pascal
 *
 * Revision 1.9  1995/08/02  00:09:39  can
 * Added ParseRuleList to support new lists of multiple rule
 * types, namely the rule base support rules.
 *
 * Revision 1.8  1995/07/28  00:15:46  can
 * Made sure new statments get memset to 0
 *
 * Revision 1.7  1995/07/26  20:56:47  can
 * Modified allowable variable types.
 *
 * Revision 1.6  1995/07/21  16:45:39  can
 * Put debug_out function call in #ifdef DEVELOP block in ParseLocalVariables
 *
 * Revision 1.5  1995/07/21  15:35:44  can
 * Added functions to parse and handle variables and rule parameters
 *
 * Revision 1.4  1995/07/16  18:27:08  can
 * Added call to function CheckExpressionFunction.
 *
 * Revision 1.3  1995/07/09  23:07:20  can
 * Fixed some bugs and updated the error handling
 *
 * Revision 1.2  1995/07/03  21:22:51  wle
 * Added string.h to clear up prototype problem.
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

/* HNC Rules engine include files */
#include "mymall.h"
#include "exptree.h"
#include "prsrule.h"
#include "lexrule.h"
#include "engerr.h"
#include "typechk.h"
#include "creatct.h"

/* HNC Common includes */
#include "except.h"

#ifdef DEVELOP
#include "rbdebug.h"
#endif


long FreeParseResults(tParseResults *parseResults, int freeVars);

static long CheckRuleUnique(tRuleBaseData *ruleBaseData, tParse *parse);
static long CheckOptionTerms(char* token, char* optionTerms);
static long CheckReadTillEnd(tParse *parse, char* optionTerms);
static long ReadTillExprEnd(tParse *parse, char* optionTerms);
static long CreateVariablesFromTemplates(tParse *parse);
static long ParseLocalTypes(tParse *parse);
static long ParseLocalConstants(tParse *parse);
static long ParseLocalVariables(tParse *parse);
static long ParseOneRuleBase(tParse *parse, tEngRBData *engRB);
static long ParseSizeof(tParse *parse, tSizeOf **pSizeOp, char *optionTerms);
static long ParseExists(tParse *parse, tRecUse **pDataFld, char *optionTerms);
static long ParseResolve(tParse *parse, tResolve **pResolve, char *optionTerms);

long CreateRecordData(tEngRecord *record, tRecData **pRecData, char *data, short inArray);
long CreateArrayData(tEngArray *array, tArrayData **pArrayData, char *data);


/*******************************************************************/
/* These flags define the origin of the call to ParseStatementList */
/* They are necessary to set up the proper expression terminators  */
/*******************************************************************/
#define FROM_OTHER   0
#define FROM_CASE    1
#define FROM_IF      2


/*--------------------------------------------------------------------------*/
/* CheckOptionTerms()                                                       */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long CheckOptionTerms(char* token, char* optionTerms)
{
   int   i=0, j;
   char  optToken[50];

   if(optionTerms==NULL)
      return(FALSE);

   if(token==NULL)
      return(FALSE);

   while(optionTerms[i] != '\0') 
   {  while(isspace(optionTerms[i])) i++;

      j=0;
      while(optionTerms[i] != ' ' && optionTerms[i] != '\0') 
         optToken[j++] = optionTerms[i++]; 
      optToken[j] = '\0';
     
      if(!strcmp(token, optToken)) return(TRUE);
   }

   return(FALSE);
}


/*--------------------------------------------------------------------------*/
/* ReadTillExprEnd()                                                        */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long ReadTillExprEnd(tParse *parse, char* optionTerms)
{
   long status;

   /************************************************/
   /* Read tokens until hit an optional terminator */
   /************************************************/
   while(1)
   {  /* inExpr=3 means read as if in an expression, but */
      /* don't allocate any structures.                  */
      parse->inExpr = 3;
      status = LexRule(parse);
      parse->inExpr = 0;
      if(status==FATAL_ENGINE) return(FATAL_ENGINE);
      else if(status==END_OF_RULE)
         break;
      else if(CheckOptionTerms(parse->token, optionTerms))
         break;
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* ParseRuleCall()                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long ParseRuleCall(tParse *parse, tRuleCall **ruleCall)
{
   char       ruleName[50];
   long       retStatus=OK, status;
   int        argNum=0;
   tExprTree  *exprTree=NULL;
   tRuleData  *ruleData;
   tRuleArg   *ruleArg=NULL;
   tParamExpr *paramExpr=NULL;

   strcpy(ruleName, parse->token);

#ifdef DEVELOP
   debug_out("parsing rule call %s\n", ruleName);
#endif


   /*****************************************/
   /* First, make sure that the rule exists */
   /*****************************************/
   *ruleCall = NULL;
   if((status=CheckRule(parse->token, parse, &ruleData)))
   {  if(AddErrMsg(FAIL_ENGINE|PARSE_RULE_CALL|status, 
                                 parse->token, 0, parse)==FATAL_ENGINE)
         return(FATAL_ENGINE);

      /**********************************************/
      /* See if the rule was called with parameters */
      /* so they can be skipped.                    */
      /**********************************************/
      while((status=LexRule(parse))==FAIL_ENGINE);
      if(status==FATAL_ENGINE)
         return(FATAL_ENGINE);
      else if(!strcmp(parse->token, "("))
      {  if(ReadTillExprEnd(parse, ")")==FATAL_ENGINE)
            return(FATAL_ENGINE);
      }
      
      return(FAIL_ENGINE);
   }
   

   /******************************************/
   /* Allocate space for tRuleCall structure */
   /******************************************/
   if((*ruleCall = (tRuleCall*)MyTmpMalloc(sizeof(tRuleCall)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)*ruleCall, 0, sizeof(tRuleCall));

   (*ruleCall)->ruleData = ruleData;
   (*ruleCall)->paramList = NULL;
   

   /******************************************/
   /* First check if there are any arguments */
   /******************************************/
   while((status=LexRule(parse))==FAIL_ENGINE);
   if(status==FATAL_ENGINE)
   {  MyFree((void*)(*ruleCall));  
      *ruleCall = NULL;
      return(FATAL_ENGINE);
   }
   else if(status==END_OF_RULE)
      retStatus=OK;
   else if(!strcmp(parse->token, "("))
   {  /**********************************************************/
      /* Rule called with a parameter list.  Make sure it is OK */
      /**********************************************************/
      /* First check to see if there really are any parameters */
      while((status=LexRule(parse))==FAIL_ENGINE);
      if(status==FATAL_ENGINE)
      {  MyFree((void*)(*ruleCall));  
         *ruleCall = NULL;
         return(FATAL_ENGINE);
      }
      else if(status==END_OF_RULE)
         retStatus = OK;
      else if(!strcmp(parse->token, ")"))
         retStatus = OK;
      else
      {  PutBackToken(parse);  /* Have a parameter list */

         /* If no parameters are expected, then there is an error */
         if(ruleData->numArgs==0)
         {  AddErrMsg(FATAL_ENGINE|PARSE_RULE_CALL|BAD_NUM_PARMS, 
                                             ruleName, ruleData->numArgs, parse);
            MyFree((void*)(*ruleCall));  
            *ruleCall = NULL;
            return(FATAL_ENGINE);
         }

         /**********************************************************/
         /* Parse the arguments.  The ParseExpresion function will */
         /* read arguments until a comma, semi-colon, or unmatched */
         /* left paren is hit.  This loop stops on the left paren  */
         /**********************************************************/
         ruleArg = ruleData->argList;  
         while(strcmp(parse->token, ")"))
         {  if((status=ParseExpression(parse, ") ,", &exprTree)))
            {  retStatus=status;
               if(retStatus==FATAL_ENGINE)
                  break;
            }
            
            /********************************************************/
            /* Make sure number of parameters has not been exceeded */
            /********************************************************/
            if(argNum==ruleData->numArgs)
            {  retStatus = 
                  AddErrMsg(FATAL_ENGINE|PARSE_RULE_CALL|BAD_NUM_PARMS, 
                                              ruleName, ruleData->numArgs, parse); 
               if(retStatus==FATAL_ENGINE)
                  break;

               if(strcmp(parse->token, ")"))
               {  if(ReadTillExprEnd(parse, ")")==FATAL_ENGINE)
                     break;
               }
    
               break;
            }

            if(exprTree)
            {  if((*ruleCall)->paramList==NULL)
               {  if((paramExpr = (tParamExpr*)MyTmpMalloc(sizeof(tParamExpr)))==NULL)
                     throw(MALLOC_EXCEPTION);
                  (*ruleCall)->paramList = paramExpr;
               }
               else
               {  if((paramExpr->next = (tParamExpr*)MyTmpMalloc(sizeof(tParamExpr)))==NULL)
                     throw(MALLOC_EXCEPTION);
                  paramExpr = paramExpr->next;
               }
               memset((char*)paramExpr, 0, sizeof(tParamExpr));
               paramExpr->exprTree = exprTree;
               exprTree->reference = ruleArg->reference;
            }
    
            /************************************************************/
            /* Make sure the expression type matches the parameter type */
            /************************************************************/
            if(exprTree)
            {  if((status=CompareRuleArgType(exprTree, ruleArg, ruleName, argNum+1, parse)))
               {  if(status==FATAL_ENGINE) break;
                  retStatus = status;
               }
            }

            argNum++;
            if(argNum < ruleData->numArgs)
               ruleArg = ruleArg->next;
         }
      }
   }
   else
   {  /************************************************************/
      /* Found something other than a parenthesis, so put it back */
      /* to determine what it is.                                 */
      /************************************************************/
      PutBackToken(parse);
   }
   
   if(retStatus==OK && exprTree)
   {  /***********************************************************/
      /* Make sure that the minimum number of arguments was used */
      /***********************************************************/
      if(argNum < ruleData->numArgs)
      {  retStatus = AddErrMsg(FAIL_ENGINE|PARSE_RULE_CALL|BAD_NUM_PARMS, 
                                        ruleName, ruleData->numArgs, parse); 
         if(retStatus==FATAL_ENGINE)
            return(FATAL_ENGINE);
      }
      else if(exprTree)
      {  if((status=CompareRuleArgType(exprTree, ruleArg, ruleName, argNum+1, parse)))
            retStatus = status;
      }
   }      

   if(retStatus!=OK)
   {  FreeParamList((*ruleCall)->paramList);
      MyFree((void*)(*ruleCall));
      *ruleCall = NULL;
   }

   return(retStatus);
}


/*--------------------------------------------------------------------------*/
/* ParseResolve()                                                           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long 
ParseResolve(tParse *parse, tResolve **pResolve, char *optionTerms)
{
   long      status, getAddress=0;
   tExprTree *exprTree, *lenExpr, *addrExpr=NULL;
   tRecData  *recData=NULL;

#ifdef DEVELOP
   debug_out("parsing RESOLVE use\n", "");
#endif

   *pResolve = NULL;

   /***************************/
   /* Must have a parenthesis */
   /***************************/
   while((status=LexRule(parse))==FAIL_ENGINE);
   if(status==FATAL_ENGINE)
      return(FATAL_ENGINE);
   else if(strcmp(parse->token, "("))
      return(AddErrMsg(FAIL_ENGINE|PARSE_RESOLVE|NO_LPAREN, "RESOLVE", 0, parse));


   /**********************************/
   /* Get the expression to be sized */
   /**********************************/
   if((status=ParseExpression(parse, ",", &exprTree)))
      return(status);

   /*******************************************/
   /* Expression must have ended with a comma */
   /*******************************************/
   if(strcmp(parse->token, ","))
   {  status = AddErrMsg(FAIL_ENGINE|PARSE_RESOLVE|RESOLVE_NOCOMMA, NULL, 0, parse);
      FreeExprTree(exprTree);
      return(status);
   }

   /*******************************/
   /* Can only resolve a template */
   /*******************************/
   if(exprTree->left.flags&EXPR_VARBLE)
   {  if(exprTree->left.val.var->flags&VAR_TEMPLATE && 
         exprTree->left.val.var->type == RECORD_TYPE)
         recData = ((tRecInstance*)exprTree->left.val.var->data)->data;
      else
      {  status = AddErrMsg(FAIL_ENGINE|PARSE_RESOLVE|RESOLVE_BADTYPE, NULL, 0, parse);
         FreeExprTree(exprTree);
         return(status);
      }

      FreeExprTree(exprTree);
   }


   /***********************************************/
   /* Get the length of the buffer to be resolved */
   /***********************************************/
   if((status=ParseExpression(parse, ", )", &lenExpr)))
      return(status);

   /************************************************************/
   /* Expression must have ended with a parenthesis or a comma */
   /************************************************************/
   if(strcmp(parse->token, ")") && strcmp(parse->token, ","))
   {  status = AddErrMsg(FAIL_ENGINE|PARSE_RESOLVE|NO_RPAREN, "RESOLVE", 0, parse);
      FreeExprTree(lenExpr);
      return(status);
   }
   else if(!strcmp(parse->token, ","))
      getAddress = 1;

   /**********************************************************/
   /* The length parameter must be a numeric type expression */
   /**********************************************************/
   if(!IsNumeric(lenExpr->type))
   {  status = AddErrMsg(FAIL_ENGINE|PARSE_RESOLVE|RESOLVE_NONUMLEN, NULL, 0, parse);
      FreeExprTree(lenExpr);
      return(status);
   }
   else if(IsNumString(lenExpr->type)) 
      lenExpr->type = NUMERIC_TYPE;


   /***********************************************************************/
   /* If not hit parenthesis yet, must have a third argument which is the */
   /* starting address for the data buffer to be resolved.                */
   /***********************************************************************/
   if(getAddress)
   {  if((status=ParseExpression(parse, ")", &addrExpr)))
         return(status);
 
      /* Expression must have ended with a parenthesis */
      if(strcmp(parse->token, ")"))
      {  status = AddErrMsg(FAIL_ENGINE|PARSE_RESOLVE|NO_RPAREN, "RESOLVE", 0, parse);
         FreeExprTree(addrExpr);
         return(status);
      }

      /* The address parameter must be a numeric type expression */
      if(!IsNumeric(addrExpr->type))
      {  status = AddErrMsg(FAIL_ENGINE|PARSE_RESOLVE|RESOLVE_NONUMLEN, NULL, 0, parse);
         FreeExprTree(addrExpr);
         return(status);
      }
      else if(IsNumString(addrExpr->type)) 
         addrExpr->type = NUMERIC_TYPE;
   }

   
   if(recData)
   {  if((*pResolve = (tResolve*)MyTmpMalloc(sizeof(tResolve)))==NULL)
         throw(MALLOC_EXCEPTION);
      (*pResolve)->recData = recData;
      (*pResolve)->lenExpr = lenExpr;
      (*pResolve)->addrExpr = addrExpr;
   }
 
   return(OK);
}


/*--------------------------------------------------------------------------*/
/* ParseExists()                                                            */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long 
ParseExists(tParse *parse, tRecUse **pDataFld, char *optionTerms)
{
   long       status;
   tExprTree  *exprTree;
   tRecUse    *dataFld=NULL;

#ifdef DEVELOP
   debug_out("parsing EXISTS use\n", "");
#endif

   /***************************/
   /* Must have a parenthesis */
   /***************************/
   while((status=LexRule(parse))==FAIL_ENGINE);
   if(status==FATAL_ENGINE)
      return(FATAL_ENGINE);
   else if(strcmp(parse->token, "("))
   {  if((AddErrMsg(FAIL_ENGINE|PARSE_EXISTS|NO_LPAREN, "EXISTS", 0, parse))==FATAL_ENGINE)
         return(FATAL_ENGINE);
   }


   /*******************************/
   /* Get the expression to check */
   /*******************************/
   if((status=ParseExpression(parse, ")", &exprTree)))
      return(status);


   /*******************************************************/
   /* Expression must have ended with a right parenthesis */
   /*******************************************************/
   if(strcmp(parse->token, ")"))
   {  if((AddErrMsg(FAIL_ENGINE|PARSE_EXISTS|NO_RPAREN, "EXISTS", 0, parse))==FATAL_ENGINE)
      {  FreeExprTree(exprTree);
         return(FATAL_ENGINE);
      }
   }


   /*********************************************************/
   /* EXISTS is only useful for variable data with optional */
   /* fields.  It returns a 1 for all expressions except    */
   /* those fields that are not required.                   */
   /*********************************************************/
   if(exprTree->left.flags&EXPR_FIELD && exprTree->op == NOOP)
   {  dataFld = exprTree->left.val.dataFld;
      dataFld->noFree = 1;
   }

   /*******************************************/
   /* Allocate a tSizeOf structure and return */
   /*******************************************/
   if(!dataFld)
   {  if((AddErrMsg(WARN_ENGINE|PARSE_EXISTS|ALWAYS_ONE, "EXISTS", 0, parse))==FATAL_ENGINE)
      {  FreeExprTree(exprTree);
         return(FATAL_ENGINE);
      }
   }
   else
      *pDataFld = dataFld;

   FreeExprTree(exprTree);
   return(OK);
}


/*--------------------------------------------------------------------------*/
/* ParseSizeof()                                                            */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long 
ParseSizeof(tParse *parse, tSizeOf **pSizeOp, char *optionTerms)
{
   long       status;
   tExprTree  *exprTree;
   long       *direct=NULL;
   tRecUse    *dataFld=NULL, *fldUse;
   tEngVar    *var;

#ifdef DEVELOP
   debug_out("parsing SIZEOF use\n", "");
#endif

   *pSizeOp = NULL;

   /***************************/
   /* Must have a parenthesis */
   /***************************/
   while((status=LexRule(parse))==FAIL_ENGINE);
   if(status==FATAL_ENGINE)
      return(FATAL_ENGINE);
   else if(strcmp(parse->token, "("))
   {  if((AddErrMsg(FAIL_ENGINE|PARSE_SIZEOF|NO_LPAREN, "SIZEOF", 0, parse))==FATAL_ENGINE)
         return(FATAL_ENGINE);
   }


   /**********************************/
   /* Get the expression to be sized */
   /**********************************/
   if((status=ParseExpression(parse, ")", &exprTree)))
      return(status);


   /*******************************************************/
   /* Expression must have ended with a right parenthesis */
   /*******************************************************/
   if(strcmp(parse->token, ")"))
   {  if((AddErrMsg(FAIL_ENGINE|PARSE_SIZEOF|NO_RPAREN, "SIZEOF", 0, parse))==FATAL_ENGINE)
      {  FreeExprTree(exprTree);
         return(FATAL_ENGINE);
      }
   }


   /************************************************/
   /* SIZEOF is only useful for strings and arrays */
   /* If have anything but a string variable or an */
   /* array, SIZEOF X will be 1.                   */
   /************************************************/
   if(exprTree->op != NOOP);
   else if(exprTree->left.flags&EXPR_VARBLE)
   {  var = exprTree->left.val.var;
      if(IsString(var->type))
         direct = (long*)&var->dataSize;
      else if(var->type == ARRAY_TYPE)
         direct = (long*)&(((tArrayData*)var->data)->array->numElems);
      else if(exprTree->left.val.var->type == RECORD_TYPE)
      {  if(!(((tRecInstance*)var->data)->flags&NON_FIXED))
            direct = (long*)&var->dataSize;
      }
   }
   else if(exprTree->left.flags&EXPR_ARRAYVAR)
   {  if(IsString(exprTree->left.val.arrayUse->arrayData->array->type))
         direct = (long*)&exprTree->left.val.arrayUse->arrayData->array->elemSize;
   }
   else if(exprTree->left.flags&EXPR_FIELD)
   {  fldUse = exprTree->left.val.dataFld;
      if(fldUse->fldData->fld->type == ARRAY_TYPE)
      {  if(!fldUse->fldData->fld->nonFixed)
         {  if(fldUse->type == ARRAY_TYPE)
               direct = (long*)&fldUse->fldData->fld->typeFmt->array.numElems;
            else if(IsString(fldUse->type))
               direct = (long*)&fldUse->fldData->fld->typeFmt->array.elemSize;
            else if(fldUse->type == RECORD_TYPE)
               direct = (long*)&fldUse->fldData->fld->typeFmt->record.dataSize;
         }
         else if(IsString(fldUse->type))
         {  if(fldUse->fldData->fld->nonFixed->lengthInd==0)
               direct = (long*)&fldUse->fldData->fld->length;
            else
               dataFld = fldUse;
         }
         else if(fldUse->fldData->data)
            direct = &((tArrayData*)fldUse->fldData->data)->endIndex;
         else
            dataFld = fldUse;
      }
      else if(IsString(fldUse->fldData->fld->type))
      {  if(!fldUse->fldData->fld->nonFixed)
            direct = (long*)&fldUse->fldData->fld->length;
         else if(fldUse->fldData->fld->nonFixed->lengthInd==0)
            direct = (long*)&fldUse->fldData->fld->length;
         else if(fldUse->arrayProc) 
            dataFld = fldUse;
         else
            direct = (long*)&(((tFlatData*)fldUse->fldData->data)->length);
      }
   }


   /*******************************************/
   /* Allocate a tSizeOf structure and return */
   /*******************************************/
   if(direct || dataFld)
   {  if((*pSizeOp = (tSizeOf*)MyTmpMalloc(sizeof(tSizeOf)))==NULL)
         throw(MALLOC_EXCEPTION);
      if(direct) 
      {  (*pSizeOp)->size = (char*)direct;
         (*pSizeOp)->type = SIZE_DIRECT;

         /* Indicate that argument parsed for expression is not used */
         exprTree->left.flags |= EXPR_UNUSED;
      }
      else
      {  (*pSizeOp)->size = (char*)dataFld;
         dataFld->noFree = 1;
         (*pSizeOp)->type = SIZE_ARRAYPROC;
      }
   }
   else
   {  if((AddErrMsg(WARN_ENGINE|PARSE_SIZEOF|ALWAYS_ONE, "SIZEOF", 0, parse))==FATAL_ENGINE)
      {  FreeExprTree(exprTree);
         return(FATAL_ENGINE);
      }
   }

   FreeExprTree(exprTree);
   return(OK);
}


/*--------------------------------------------------------------------------*/
/* ParseRBExec()                                                            */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long ParseRBExec(tParse *parse, tEngRBData **pRBExec, char *optionTerms)
{
   long       retStatus=OK, status;
   tEngRBData *RBExec;
   char       *RBName;

#ifdef DEVELOP
   debug_out("parsing rule base EXEC call\n", "");
#endif

   *pRBExec = NULL;

   /************************************/
   /* Get the rule base to be executed */
   /************************************/
   while((status=LexRule(parse))==FAIL_ENGINE);
   if(status==FATAL_ENGINE)
      return(FATAL_ENGINE);
   else if(status==END_OF_RULE)
   {  AddErrMsg(FATAL_ENGINE|PARSE_RBEXEC|EARLY_TERM, NULL, 0, parse);
      return(FATAL_ENGINE);
   }
   else if(parse->tokenType != T_RBNAME && parse->tokenType != T_VARIABLE)
   {  AddErrMsg(FATAL_ENGINE|PARSE_RBEXEC|EXEC_NOT_FOUND, parse->token, 0, parse);
      return(FATAL_ENGINE);
   }

   /*********************************************/
   /* If tEngRBData structure not found, get it */
   /*********************************************/
   RBName = parse->token;
   if(!parse->data.engRB)
   {  for(RBExec=parse->RBList; RBExec; RBExec=RBExec->next) 
         if(!strcmp(RBName, RBExec->RBData->ruleBaseName))
            break;
   }
   else
      RBExec = parse->data.engRB;

   /**************************************/
   /* If not RBExec, then it is an error */
   /**************************************/
   if(!RBExec)
   {  AddErrMsg(FATAL_ENGINE|PARSE_RBEXEC|EXEC_NOT_FOUND, RBName, 0, parse);
      return(FATAL_ENGINE);
   }

   /**************************************************/
   /* Make sure the rule base gets parsed eventually */
   /**************************************************/
   RBExec->parseIt = TRUE;

   *pRBExec = RBExec;
   return(retStatus);
}


/*--------------------------------------------------------------------------*/
/* ParseFunction()                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long ParseFunction(tParse *parse, tFuncCall **funcCall)
{
   char       funcName[50];
   long       retStatus=OK, status;
   int        argNum=0;
   tExprTree  *exprTree=NULL;
   tFunc      *funcStruct;
   tParamList *paramType=NULL;
   tParamExpr *paramExpr=NULL;

   strcpy(funcName, parse->token);

#ifdef DEVELOP
   debug_out("parsing function call %s\n", funcName);
#endif

   /*********************************************/
   /* First, make sure that the function exists */
   /*********************************************/
   *funcCall = NULL;
   if((status=CheckFunction(parse->token, parse, &funcStruct)))
   {  retStatus=AddErrMsg(FAIL_ENGINE|PARSE_FUNC|status, 
                                           parse->token, 0, parse);
      if(retStatus==FATAL_ENGINE || ReadTillExprEnd(parse, ")")==FATAL_ENGINE)
         return(FATAL_ENGINE);

      return(retStatus);
   }
   
   /******************************************/
   /* Allocate space for tFuncCall structure */
   /******************************************/
   if((*funcCall = (tFuncCall*)MyTmpMalloc(sizeof(tFuncCall)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)*funcCall, 0, sizeof(tFuncCall));

   (*funcCall)->funcData = funcStruct;
   (*funcCall)->paramList = NULL;
   (*funcCall)->ruleData = parse->curRule;
   
   /******************************************/
   /* First check if there are any arguments */
   /******************************************/
   while((status=LexRule(parse))==FAIL_ENGINE);
   if(status==FATAL_ENGINE)
      return(FATAL_ENGINE);
   else if(status==END_OF_RULE)
   {  AddErrMsg(FATAL_ENGINE|PARSE_FUNC|EARLY_TERM, funcName, 0, parse);
      MyFree((void*)(*funcCall));
      *funcCall = NULL;
      return(FATAL_ENGINE);
   }
   else if(!strcmp(parse->token, ")"))
   {  /**********************************************************/
      /* Function called with no arguments.  Make sure it is OK */
      /**********************************************************/
      if(funcStruct->numParams!=0)
      {  AddErrMsg(FATAL_ENGINE|PARSE_FUNC|BAD_NUM_PARMS, 
                                   funcName, funcStruct->numParams, parse);
         MyFree((void*)(*funcCall));
         *funcCall = NULL;
         return(FATAL_ENGINE);
      }
   }
   else
   {  PutBackToken(parse);
   
      /**********************************************************/
      /* Parse the arguments.  The ParseExpresion function will */
      /* read arguments until a comma, semi-colon, or unmatched */
      /* left paren is hit.  This loop stops on the left paren  */
      /**********************************************************/
      paramType = funcStruct->paramTypeList;  
      while(strcmp(parse->token, ")"))
      {  if((status=ParseExpression(parse, ") ,", &exprTree)))
         {  retStatus=status;
            if(retStatus==FATAL_ENGINE)
               break;
         }
         
         /*****************************************************************/
         /* Make sure that the number of parameters has not been exceeded */
         /*****************************************************************/
         if(argNum==funcStruct->numParams && !funcStruct->unlimitedParams)
         {  retStatus = 
               AddErrMsg(FATAL_ENGINE|PARSE_FUNC|BAD_NUM_PARMS, 
                                       funcName, funcStruct->numParams, parse); 
            if(retStatus==FATAL_ENGINE)
               break;
            if(strcmp(parse->token, ")"))
            {  if(ReadTillExprEnd(parse, ")")==FATAL_ENGINE)
                  break;
            }
            
            break;
         }
    
         /********************************************/
         /* Add the expression to the parameter list */
         /********************************************/
         if(exprTree)
         {  if((*funcCall)->paramList==NULL)
            {  if((paramExpr = (tParamExpr*)MyTmpMalloc(sizeof(tParamExpr)))==NULL)
                  throw(MALLOC_EXCEPTION);
               (*funcCall)->paramList = paramExpr;
            }
            else
            {  if((paramExpr->next = (tParamExpr*)MyTmpMalloc(sizeof(tParamExpr)))==NULL)
                  throw(MALLOC_EXCEPTION);
               paramExpr = paramExpr->next;
            }
            memset((char*)paramExpr, 0, sizeof(tParamExpr));
            paramExpr->exprTree = exprTree;
            exprTree->reference = paramType->reference;
         }
         
         /************************************************************/
         /* Make sure the expression type matches the parameter type */
         /************************************************************/
         if(exprTree)
         {  if((status=CompareFuncParamType(exprTree, paramType, funcName, argNum+1, parse)))
            {  if(status==FATAL_ENGINE) break;
               retStatus = status;
            }
         }
         
         argNum++;
         if(argNum < funcStruct->numParams)
            paramType = paramType->next;
      }
   }
   
   if(retStatus==OK && exprTree)
   {  /***********************************************************/
      /* Make sure that the minimum number of arguments was used */
      /* Note that the last parameter may be optional.           */
      /***********************************************************/
      if((argNum < funcStruct->numParams) &&
         !(argNum == funcStruct->numParams-1 && funcStruct->lastOptional))
      {  
         retStatus = AddErrMsg(FAIL_ENGINE|PARSE_FUNC|BAD_NUM_PARMS, 
                                     funcName, funcStruct->numParams, parse); 
         if(retStatus==FATAL_ENGINE)
            return(FATAL_ENGINE);
      }
   }      

   if(retStatus!=OK)
   {  FreeParamList((*funcCall)->paramList);
      MyFree((void*)(*funcCall));
      *funcCall = NULL;
   }

   return(retStatus);
}


/*--------------------------------------------------------------------------*/
/* ParseInOperation()                                                       */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long ParseInOperation(tParse *parse, tExprTree *exprTree)
{
   tParamExpr *paramExpr=NULL;
   tExprTree  *tmpExpr;
   tInCall    *inCall;
   int        keyType, argNum=0;
   long       retStatus=OK, status;

   /**********************************/
   /* Allocate new tInCall structure */
   /**********************************/
   if((inCall = (tInCall*)MyTmpMalloc(sizeof(tInCall)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)inCall, 0, sizeof(tInCall));

   /********************************************************************/
   /* Build the key expression for the inCall.  The IN operation will  */ 
   /* have a highest precedence, so the left expression of the current */
   /* tree will become the key expression.                             */
   /********************************************************************/
   if(exprTree->left.flags & EXPR_TREE)
   {  inCall->keyExpr = exprTree->left.val.tree;  
      inCall->keyExpr->parent = NULL;
      if((status=TypeCheckAndRollUpExpression(inCall->keyExpr, parse, NULL)))
      {  retStatus = status;
         if(status==FATAL_ENGINE)
         {  MyFree((void*)inCall);
            return(FATAL_ENGINE);
         }
      }
   }
   else
   {  /**************************************************/
      /* Allocate an expression tree for the inCall key */
      /**************************************************/
      if((status=AllocateExprTree(&inCall->keyExpr)))
      {  AddErrMsg(FATAL_ENGINE|PARSE_IN|status, NULL, 0, parse);
         MyFree((void*)inCall);
      }

      /***********************************************************/
      /* For type checking, make sure NOT'd strings are detected */ 
      /***********************************************************/
      if(exprTree->left.flags&EXPR_NOT && 
         (exprTree->left.type == HNC_STRING || exprTree->left.type == HNC_CHAR))
      {  exprTree->left.flags = EXPR_CONSTANT;
         exprTree->left.type = NUMERIC_TYPE;
         ZERO(exprTree->left.val.num)
         if(AddErrMsg(WARN_ENGINE|PARSE_IN|NOT_STRING, NULL, 0, parse)==FATAL_ENGINE)
         {  FreeExprTree(inCall->keyExpr);
            MyFree((void*)inCall);
            return(FATAL_ENGINE);
         }
      }

      inCall->keyExpr->left = exprTree->left;
      inCall->keyExpr->type = exprTree->left.type;
      inCall->keyExpr->op = NOOP;
   }
   keyType = inCall->keyExpr->type;

   /*****************************************************/
   /* Indicate that the left branch is an IN expression */ 
   /*****************************************************/
   exprTree->left.flags = 0L;
   exprTree->left.type = NUMERIC_TYPE;
   exprTree->left.flags |= EXPR_IN;
   exprTree->left.val.inCall = inCall; 

   /******************************************************/
   /* Build the parameter list that should follow the IN */
   /******************************************************/
   /* Check for first parenthesis */
   while((status=LexRule(parse))==FAIL_ENGINE);
   if(status==FATAL_ENGINE)
   {  FreeExprTree(inCall->keyExpr);
      MyFree((void*)inCall);
      exprTree->left.val.inCall = NULL;
      return(FATAL_ENGINE);
   }
   else if(status==END_OF_RULE)
   {  AddErrMsg(FATAL_ENGINE|PARSE_IN|EARLY_TERM, NULL, 0, parse);
      FreeExprTree(inCall->keyExpr);
      MyFree((void*)inCall);
      exprTree->left.val.inCall = NULL;
      return(FATAL_ENGINE);
   }
   else if(strcmp(parse->token, "("))
   {  if(AddErrMsg(FAIL_ENGINE|PARSE_IN|ILLEGAL_IN, NULL, 0, parse)==FATAL_ENGINE)
      {  FreeExprTree(inCall->keyExpr);
         MyFree((void*)inCall);
         exprTree->left.val.inCall = NULL;
         return(FATAL_ENGINE);
      }
   }

   
   /**********************************************************/
   /* Parse the arguments.  The ParseExpresion function will */
   /* read arguments until a comma, semi-colon, or unmatched */
   /* left paren is hit.  This loop stops on the left paren  */
   /**********************************************************/
   while(strcmp(parse->token, ")"))
   {  if((status=ParseExpression(parse, ") ,", &tmpExpr)))
      {  retStatus=status;
         if(retStatus==FATAL_ENGINE)
            break;
      }
      
      /********************************************/
      /* Add the expression to the parameter list */
      /********************************************/
      if(tmpExpr)
      {  if(inCall->paramList==NULL)
         {  if((paramExpr = (tParamExpr*)MyTmpMalloc(sizeof(tParamExpr)))==NULL)
               throw(MALLOC_EXCEPTION);
            inCall->paramList = paramExpr;
         }
         else
         {  if((paramExpr->next = (tParamExpr*)MyTmpMalloc(sizeof(tParamExpr)))==NULL)
               throw(MALLOC_EXCEPTION);
            paramExpr = paramExpr->next;
         }
         memset((char*)paramExpr, 0, sizeof(tParamExpr));
         paramExpr->exprTree = tmpExpr;
      }
      argNum++;
      
      /*****************************************************/
      /* Make sure the parameter type matches the key type */
      /*****************************************************/
      if(tmpExpr)
      {  if((status=CompareType(keyType, tmpExpr->type)))
         {  retStatus = 
               AddErrMsg(FAIL_ENGINE|PARSE_IN|status, NULL, argNum, parse); 
            if(retStatus==FATAL_ENGINE)
               break;
         }
      }
   }
   
   if(retStatus==OK && tmpExpr)
   {  /**********************************************************/
      /* Make sure the last parameter type matches the key type */
      /**********************************************************/
      if((status=CompareType(keyType, tmpExpr->type)))
      {  retStatus = AddErrMsg(FAIL_ENGINE|PARSE_IN|status,  
                                                NULL, argNum+1, parse); 
         if(retStatus==FATAL_ENGINE)
            return(FATAL_ENGINE);
      }
   }      

   if(retStatus!=OK)
   {  FreeParamList(inCall->paramList);
      FreeExprTree(inCall->keyExpr);
      MyFree((void*)inCall);
      exprTree->left.val.inCall = NULL;
   }

   return(retStatus);
}


/*--------------------------------------------------------------------------*/
/* ParseArrayIndex()                                                        */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long ParseArrayIndex
(tParse *parse, char *arrayVar, char *arrayIndex, tArrayUse *arrayUse)
{
   long      index, status=OK;
   char      errToken[100];
   tParse    tp, *tmpParse=&tp;
   tExprTree *indexExpr;

   /* Copy the current parse structure */
   tp = *parse;
   tmpParse->index = 0;
   tmpParse->lastIndex = 0;
   tmpParse->ruleText = arrayIndex;

   /* Get start bracket, if any */
   while((status=LexRule(tmpParse))==FAIL_ENGINE);
   if(status==END_OF_RULE)
   {  AddErrMsg(FATAL_ENGINE|PARSE_ARRAY_IDX|EARLY_TERM, NULL, 0, parse);
      return(FATAL_ENGINE);
   }
   else if(!strcmp(tmpParse->token, "[") || !strcmp(tmpParse->token, "{"))
   {  /* Parse the index expression */ 
      if((status=ParseExpression(tmpParse, "} ]", &indexExpr)))
         return(status);

      /* Make sure index is an integer type */
      if(!IsIntNumeric(indexExpr->type))
      {  if((status=AddErrMsg(FAIL_ENGINE|PARSE_ARRAY_IDX|BAD_INDEX_TYPE, 
                                     arrayVar, 0, tmpParse))==FATAL_ENGINE)
            return(FAIL_ENGINE);
      }

      /* If index is a simple constant, make sure it is in range */
      if(indexExpr->op == NOOP && 
           IsIntNumeric(indexExpr->type) && 
           (indexExpr->left.flags&EXPR_CONSTANT))
      {  index = NUM_GETLNG(indexExpr->left.val.num);
         if(index < arrayUse->arrayData->array->startIndex)
         {  sprintf(errToken, "%ld$%s", arrayUse->arrayData->array->startIndex, arrayVar);
            if((status=AddErrMsg(FAIL_ENGINE|PARSE_ARRAY_IDX|INDEX_OB_LO, 
                                        errToken, index, tmpParse))==FATAL_ENGINE)
               return(FATAL_ENGINE);
         }
         else
         if(index > arrayUse->arrayData->array->endIndex)
         {  sprintf(errToken, "%ld$%s", arrayUse->arrayData->array->endIndex, arrayVar);
            if((status=AddErrMsg(FAIL_ENGINE|PARSE_ARRAY_IDX|INDEX_OB_HI, 
                                         errToken, index, tmpParse))==FATAL_ENGINE)
               return(FATAL_ENGINE);
         }
      }
     
      /* Get the terminating bracket */
      if(strcmp(tmpParse->token, "]") && strcmp(tmpParse->token, "}"))
      {  if((status=AddErrMsg(FAIL_ENGINE|PARSE_ARRAY_IDX|MISSING_RBRACKET, 
                                        tmpParse->token, 0, tmpParse))==FATAL_ENGINE)
            return(FATAL_ENGINE);
      }
   }

   arrayUse->indexExpr = (void*)indexExpr;
   return(status);
}


/*--------------------------------------------------------------------------*/
/* ParseOperand()                                                           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long ParseOperand(tParse *parse, char *optionTerms, tExprTree *exprTree)
{
   long          retStatus=OK, status;
   tFuncCall     *funcCall;
   tRuleCall     *ruleCall;
   tEngRBData    *RBExec;
   tSizeOf       *sizeOp;
   tResolve      *resolve;
   tRecUse       *dataFld;

   /*************/
   /* Get token */ 
   /*************/
   parse->inExpr = TRUE;
   while((status=LexRule(parse))==FAIL_ENGINE);
   parse->inExpr = FALSE;
   if(status==FATAL_ENGINE)
      return(FATAL_ENGINE);
   else if(status==END_OF_RULE)
   {  AddErrMsg(FATAL_ENGINE|PARSE_EXPR|EARLY_TERM, NULL, 0, parse);
      return(FATAL_ENGINE);
   }
   else if(!strcmp(parse->token, "("))
   {  /***********************************/
      /* Allocate an expression sub-tree */
      /***********************************/
      if((status=AllocateExprTree(&exprTree->left.val.tree)))
      {  AddErrMsg(FATAL_ENGINE|PARSE_EXPR|status, NULL, 0, parse);
         return(FATAL_ENGINE);
      }
      exprTree->left.flags |= EXPR_TREE;
      exprTree->left.flags |= EXPR_PAREN;
      exprTree->left.val.tree->parent = exprTree;
      exprTree->left.val.tree->numParens = exprTree->numParens;
      (*exprTree->numParens)++;

      retStatus = ParseExprTree(parse, optionTerms, exprTree->left.val.tree);
   }
   else if(!strcmp(parse->token, "+") && parse->tokenType == T_ARITH_OPERATOR)
   {
#ifdef DEVELOP
      debug_out("Have Unary '+' sign\n", "");
#endif

      retStatus = ParseOperand(parse, optionTerms, exprTree);
   }
   else if(!strcmp(parse->token, "-") && parse->tokenType == T_ARITH_OPERATOR)
   {
#ifdef DEVELOP
      debug_out("Have Unary '-' sign\n", "");
#endif
      if(exprTree->left.flags&EXPR_NEGATE)
         exprTree->left.flags &= (0XFFFFFFFF - EXPR_NEGATE);
      else
         exprTree->left.flags |= EXPR_NEGATE;

      retStatus = ParseOperand(parse, optionTerms, exprTree);
   }
   else if(parse->tokenType == T_BOOL_NEGATE)
   {
#ifdef DEVELOP
      debug_out("Have NOT\n", "");
#endif
      if(exprTree->left.flags&EXPR_NOT)
         exprTree->left.flags &= (0XFFFFFFFF - EXPR_NOT);
      else
         exprTree->left.flags |= EXPR_NOT;

      retStatus = ParseOperand(parse, optionTerms, exprTree);
   }
   else if(parse->tokenType == T_FLOAT_CONSTANT)
   {
#ifdef DEVELOP
      debug_out("Have a float constant '%s'\n", parse->token);
#endif

      exprTree->left.flags |= EXPR_CONSTANT;
      exprTree->left.type = NUMERIC_TYPE;
      exprTree->left.val.num = parse->data.num;
      if(exprTree->left.flags&EXPR_NEGATE)
      {  NUM_NEG(exprTree->left.val.num)
         exprTree->left.flags &= (0XFFFFFFFF - EXPR_NEGATE);
      }
      if(exprTree->left.flags&EXPR_NOT)
      {  NUM_NOT(exprTree->left.val.num)
         exprTree->left.flags &= (0XFFFFFFFF - EXPR_NOT);
      }
   }
   else if(parse->tokenType == T_INTEGER_CONSTANT)
   {
#ifdef DEVELOP
      debug_out("Have an integer constant '%s'\n", parse->token);
#endif

      exprTree->left.flags |= EXPR_CONSTANT;
      exprTree->left.type = NUMERIC_TYPE;
      exprTree->left.val.num = parse->data.num;
      if(exprTree->left.flags&EXPR_NEGATE)
      {  NUM_NEG(exprTree->left.val.num)
         exprTree->left.flags &= (0XFFFFFFFF - EXPR_NEGATE);
      }
      if(exprTree->left.flags&EXPR_NOT)
      {  NUM_NOT(exprTree->left.val.num)
         exprTree->left.flags &= (0XFFFFFFFF - EXPR_NOT);
      }
   }
   else if(parse->tokenType == T_STRING_CONSTANT)
   {
#ifdef DEVELOP
      debug_out("Have a string constant %s\n", parse->token);
#endif

      exprTree->left.flags |= EXPR_CONSTANT;
      if(exprTree->left.flags&EXPR_NOT)
      {  exprTree->left.type = NUMERIC_TYPE;
         ZERO(exprTree->left.val.num)
         exprTree->left.flags &= (0XFFFFFFFF - EXPR_NOT);
      }
      else
      {  /************************************/
         /* Malloc space and copy the string */
         /************************************/
         if((exprTree->left.val.str = (char*)MyMalloc(strlen(parse->token)+1))==NULL)
            throw(MALLOC_EXCEPTION);
         strcpy(exprTree->left.val.str, parse->token); 

         /*****************************************************/
         /* Check to see if it is a valid date or time string */
         /*****************************************************/
         if(IsValidDateStr(parse->token)) exprTree->left.type = HNC_DATESTR;
         else if(IsValidTimeStr(parse->token)) exprTree->left.type = HNC_TIMESTR;
         else if(IsValidDateTimeStr(parse->token)) exprTree->left.type = HNC_DTSTR;
         else exprTree->left.type = HNC_STRING;
      }
   }
   else if(parse->tokenType == T_CHAR_CONSTANT)
   {
#ifdef DEVELOP
      debug_out("Have a character constant %s\n", parse->token);
#endif

      exprTree->left.flags |= EXPR_CONSTANT;
      if(exprTree->left.flags&EXPR_NOT)
      {  exprTree->left.type = NUMERIC_TYPE;
         ZERO(exprTree->left.val.num)
         exprTree->left.flags &= (0XFFFFFFFF - EXPR_NOT);
      }
      else
      {  exprTree->left.type = HNC_CHAR;
         if((exprTree->left.val.str = (char*)MyMalloc(strlen(parse->token)+1))==NULL)
            throw(MALLOC_EXCEPTION);
         strcpy(exprTree->left.val.str, parse->token); 
      }
   }
   else if(parse->tokenType == T_VARIABLE)
   {
#ifdef DEVELOP
      debug_out("Have a variable '%s'\n", parse->token);
#endif

      if(parse->data.var)
      {  exprTree->left.flags |= EXPR_VARBLE;
         exprTree->left.val.var = parse->data.var;
         exprTree->left.type = parse->data.var->type;
      }
      else
         retStatus = FAIL_ENGINE;
   }
   else if(parse->tokenType == T_ARRAYVAR)
   {
#ifdef DEVELOP
      debug_out("Have an array variable '%s'\n", parse->token);
#endif
      if(parse->data.arrayUse)
      {  exprTree->left.flags |= EXPR_ARRAYVAR;
         exprTree->left.val.arrayUse = parse->data.arrayUse;
         exprTree->left.type = parse->data.arrayUse->arrayData->array->type;
      }
      else
         retStatus = FAIL_ENGINE;
   }
   else if(parse->tokenType == T_RECFLD)
   {
#ifdef DEVELOP
      debug_out("Have a data field '%s'\n", parse->token);
#endif

      if(parse->data.dataFld)
      {  exprTree->left.flags |= EXPR_FIELD;
         exprTree->left.val.dataFld = parse->data.dataFld;
         exprTree->left.type = parse->data.dataFld->type;
      }
      else
         retStatus = FAIL_ENGINE;
   }
   else if(parse->tokenType == T_RULE)
   {
#ifdef DEVELOP
      debug_out("Have a rule '%s'\n", parse->token);
#endif

      if((retStatus=ParseRuleCall(parse, &ruleCall))==OK)
      {  exprTree->left.flags |= EXPR_RULE;
         exprTree->left.val.ruleCall = ruleCall;
         exprTree->left.type = ruleCall->ruleData->returnType;
      }
   }
   else if(parse->tokenType == T_FUNCTION)
   {
#ifdef DEVELOP
      debug_out("Have a function '%s'\n", parse->token);
#endif

      if((retStatus = ParseFunction(parse, &funcCall))==OK)
      {  exprTree->left.flags |= EXPR_FUNCTION;
         exprTree->left.val.funcCall = funcCall;
         exprTree->left.type = funcCall->funcData->returnType;
      }
   }
   else if(!strcmp(parse->token, "EXEC"))
   {
#ifdef DEVELOP
      debug_out("Have a EXEC'\n", "");
#endif

      if((retStatus = ParseRBExec(parse, &RBExec, optionTerms))==OK)
      {  exprTree->left.flags |= EXPR_RBEXEC;
         exprTree->left.val.RBExec = RBExec;
         exprTree->left.type = NUMERIC_TYPE;
      }
   }
   else if(!strcmp(parse->token, "EXISTS"))
   {
#ifdef DEVELOP
      debug_out("Have an EXISTS'\n", "");
#endif

      if((retStatus = ParseExists(parse, &dataFld, optionTerms))==OK)
      {  if(dataFld)
         {  exprTree->left.flags |= EXPR_EXISTS;
            exprTree->left.val.dataFld = dataFld;
            exprTree->left.type = NUMERIC_TYPE;
         }
         else
         {  exprTree->left.flags |= EXPR_CONSTANT;
            exprTree->left.type = NUMERIC_TYPE;
            NUM_SETINT32(exprTree->left.val.num, 1)
            if(exprTree->left.flags&EXPR_NEGATE)
            {  NUM_NEG(exprTree->left.val.num)
               exprTree->left.flags &= (0XFFFFFFFF - EXPR_NEGATE);
            }
            if(exprTree->left.flags&EXPR_NOT)
            {  NUM_NOT(exprTree->left.val.num)
               exprTree->left.flags &= (0XFFFFFFFF - EXPR_NOT);
            }
         }
      } 
   }
   else if(!strcmp(parse->token, "SIZEOF"))
   {
#ifdef DEVELOP
      debug_out("Have a SIZEOF'\n", "");
#endif

      if((retStatus = ParseSizeof(parse, &sizeOp, optionTerms))==OK)
      {  if(sizeOp)
         {  exprTree->left.flags |= EXPR_SIZEOF;
            exprTree->left.val.sizeOp = sizeOp;
            exprTree->left.type = NUMERIC_TYPE;
         }
         else
         {  exprTree->left.flags |= EXPR_CONSTANT;
            exprTree->left.type = NUMERIC_TYPE;
            NUM_SETINT32(exprTree->left.val.num, 1)
            if(exprTree->left.flags&EXPR_NEGATE)
            {  NUM_NEG(exprTree->left.val.num)
               exprTree->left.flags &= (0XFFFFFFFF - EXPR_NEGATE);
            }
            if(exprTree->left.flags&EXPR_NOT)
            {  NUM_NOT(exprTree->left.val.num)
               exprTree->left.flags &= (0XFFFFFFFF - EXPR_NOT);
            }
         }
      } 
   }
   else if(!strcmp(parse->token, "ERRNO"))
   {
#ifdef DEVELOP
      debug_out("Have an ERRNO'\n", "");
#endif

      exprTree->left.flags |= EXPR_ERRNO;
      exprTree->left.type = HNC_INT32;
   }
   else if(!strcmp(parse->token, "ERRSTR"))
   {
#ifdef DEVELOP
      debug_out("Have an ERRSTR'\n", "");
#endif

      exprTree->left.flags |= EXPR_ERRSTR;
      exprTree->left.type = HNC_STRING;
   }
   else if(!strcmp(parse->token, "RESOLVE"))
   {
#ifdef DEVELOP
      debug_out("Have a RESOLVE'\n", "");
#endif

      if((retStatus = ParseResolve(parse, &resolve, optionTerms))==OK)
      {  if(resolve)
         {  exprTree->left.flags |= EXPR_RESOLVE;
            exprTree->left.val.resolve = resolve;
            exprTree->left.type = NUMERIC_TYPE;
         }
         else
         {  exprTree->left.flags |= EXPR_CONSTANT;
            exprTree->left.type = NUMERIC_TYPE;
            NUM_SETINT32(exprTree->left.val.num, 1)
            if(exprTree->left.flags&EXPR_NEGATE)
            {  NUM_NEG(exprTree->left.val.num)
               exprTree->left.flags &= (0XFFFFFFFF - EXPR_NEGATE);
            }
            if(exprTree->left.flags&EXPR_NOT)
            {  NUM_NOT(exprTree->left.val.num)
               exprTree->left.flags &= (0XFFFFFFFF - EXPR_NOT);
            }
         }
      } 
   }
   else
      retStatus = AddErrMsg(FAIL_ENGINE|PARSE_EXPR|OPERAND_EXPCT, parse->token, 0, parse);

   /* If operand was bad, read next token to check if it is */
   /* a valid expression terminator.  If not, put it back   */
   if(retStatus)
   {  while((status=LexRule(parse))==FAIL_ENGINE);
      if(status==FATAL_ENGINE)
         return(FATAL_ENGINE);
      else if(status==END_OF_RULE)
         PutBackToken(parse);
      else if(!CheckOptionTerms(parse->token, optionTerms))
         PutBackToken(parse);
   }

   return(retStatus);
}
  

/*--------------------------------------------------------------------------*/
/* ParseOperator()                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
#define END_OF_EXPR  -99
long ParseOperator(tParse *parse, char *optionTerms, tExprTree *exprTree)
{
   long       retStatus=OK, status;
   char       errToken[150];

   /*************/
   /* Get token */ 
   /*************/
   while((status=LexRule(parse))==FAIL_ENGINE);
   if(status==FATAL_ENGINE)
      return(FATAL_ENGINE);
   else if(status == END_OF_RULE)
   {
#ifdef DEVELOP
      debug_out("Found End of Expression with EOR\n", "");
#endif
      return(END_OF_EXPR);
   }
   else if(!strcmp(parse->token, ")"))
   {
#ifdef DEVELOP
      debug_out("Have Right Parens\n", "");
#endif

      if(*exprTree->numParens == 0)
      {  if(!CheckOptionTerms(")", optionTerms))
         {  retStatus = AddErrMsg(FAIL_ENGINE|PARSE_EXPR|UNMTCH_RPAREN, NULL, 0, parse);
            return(retStatus);
         }
      }
      else (*exprTree->numParens)--;
      
      return(END_OF_EXPR);
   }
   else if(CheckOptionTerms(parse->token, optionTerms))
   {
#ifdef DEVELOP
      debug_out("Found End of Expression with '%s'\n", parse->token);
#endif
      return(END_OF_EXPR);
   }
   else if(parse->tokenType==T_BOOL_OPERATOR ||
           parse->tokenType==T_ARITH_OPERATOR ||
           parse->tokenType==T_BOOL_JOIN)
   {
#ifdef DEVELOP
      debug_out("Have operator %s\n", parse->token);
#endif
      SetOpType(&exprTree->op, parse->token);
   }
   else if(parse->tokenType == T_KEYWORD)
   {  /***********************************************************/
      /* If it is an "IN", parse it and go ahead.  Otherwise, it */
      /* is an error.                                            */ 
      /***********************************************************/
      if(!strcmp(parse->token, "IN"))
      {  if((status = ParseInOperation(parse, exprTree)))
            return(status);
         return(ParseOperator(parse, optionTerms, exprTree));
      }
      else
      {
#ifdef DEVELOP
         debug_out("Found End of Expression with "
                      "unexpected keyword '%s'\n", parse->token);
#endif

         sprintf(errToken, "%s$%s", parse->token, optionTerms);
         if(AddErrMsg(FAIL_ENGINE|PARSE_EXPR|UNEXPCT_KEYWD, 
                                         errToken, 0, parse)==FATAL_ENGINE)
            return(FATAL_ENGINE);

         PutBackToken(parse);
         retStatus = FAIL_ENGINE;
      }
   }
   else 
   {  if(parse->tokenType != T_PUNCTUATION)
      {  sprintf(errToken, "%s$%s", parse->token, optionTerms);
         retStatus = AddErrMsg(FAIL_ENGINE|PARSE_EXPR|OPERATR_EXPCT, 
                                                   errToken, 0, parse);
         PutBackToken(parse);
         return(retStatus);
      }
      else
         retStatus = AddErrMsg(FAIL_ENGINE|PARSE_EXPR|SYNTX_ERR, 
                                                  parse->token, 0, parse);
   }

   return(retStatus);
}


/*--------------------------------------------------------------------------*/
/* DoOpSwap()                                                               */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long DoOpSwap(int op1, int op2)
{
   if(op1==SET && op2==SET) return(0);
   else if(op1 == ORBOOL && op2 == ORBOOL) return(0);
   else if(op1 == ANDBOOL && op2 == ANDBOOL) return(0);
   else if(op1 >= op2) return(1);
   else if(op1 == PLUS && op2 == MINUS) return(1);
   else if(op1 == MULTIPLY && op2 == DIVIDE) return(1);
   else if(op1 == ANDBOOL && op2 == ORBOOL) return(1);
   else if((op1 >= EQUAL && op1 <= LESS) && (op2 >= EQUAL && op2 <= LESS))
      return(1);

   return(0);
}


/*--------------------------------------------------------------------------*/
/* ParseExprTree()                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long ParseExprTree
(tParse *parse, char *optionTerms, tExprTree *exprTree)
{
   long       status;
   tExprTree  *temp, *leftTree;

#ifdef DEVELOP
   debug_out("parsing expression\n", "");
#endif
   
   /***************************/
   /* Parse left hand operand */
   /***************************/
   if((status = ParseOperand(parse, optionTerms, exprTree)))
   {  if(status!=END_OF_EXPR)
         return(status);
   }

   /*****************************************/
   /* Parse the operator for the expression */
   /*****************************************/
   if((status=ParseOperator(parse, optionTerms, exprTree)))
   {  if(status==FATAL_ENGINE)
         return(FATAL_ENGINE);
      else if(status==END_OF_EXPR)
      {  /**********************************************/
         /* Make sure these is a parent to pull up to. */
         /**********************************************/
         if(!exprTree->parent)
         {  /*************************************************/
            /* Check for the case of superfluous parentheses */
            /*************************************************/
            if(exprTree->op == NOOP &&  exprTree->left.flags&EXPR_TREE &&
               !(exprTree->left.flags&EXPR_NEGATE) && !(exprTree->left.flags&EXPR_NOT))
            {  temp = exprTree->left.val.tree;

               exprTree->left.flags = temp->left.flags;
               exprTree->left = temp->left;
               if(exprTree->left.flags&EXPR_TREE)
                  exprTree->left.val.tree->parent = exprTree;

               exprTree->op = temp->op;

               exprTree->right.flags = temp->right.flags;
               exprTree->right = temp->right;
               if(exprTree->right.flags&EXPR_TREE)
                  exprTree->right.val.tree->parent = exprTree;

               memset((char*)temp, 0, sizeof(tExprTree));
               FreeExprTree(temp);
            }
            return(OK);
         }
          
         /**************************************************************************/
         /* Pull the left branch of the current tree up to the parent, maintaining */ 
         /* whether of not the branch was negated or NOT'ed                        */
         /**************************************************************************/
         if(exprTree->parent->op == NOOP)
         {  /****************************************************/
            /* Check for negated and double negated expressions */ 
            /****************************************************/
            if(exprTree->parent->left.flags&EXPR_NEGATE && exprTree->left.flags&EXPR_NEGATE) 
               exprTree->left.flags &= (0xFFFFFFFF - EXPR_NEGATE);
            else if(exprTree->parent->left.flags&EXPR_NEGATE || 
                     exprTree->left.flags&EXPR_NEGATE) 
               exprTree->left.flags |= EXPR_NEGATE;

            if(exprTree->parent->left.flags&EXPR_NOT && exprTree->left.flags&EXPR_NOT) 
               exprTree->left.flags &= (0xFFFFFFFF - EXPR_NOT);
            else if(exprTree->parent->left.flags&EXPR_NOT || exprTree->left.flags&EXPR_NOT) 
               exprTree->left.flags |= EXPR_NOT;

            exprTree->parent->left = exprTree->left;
         }
         else
         {  /****************************************************/
            /* Check for negated and double negated expressions */ 
            /****************************************************/
            if(exprTree->parent->right.flags&EXPR_NEGATE && exprTree->right.flags&EXPR_NEGATE) 
               exprTree->right.flags &= (0xFFFFFFFF - EXPR_NEGATE);
            else if(exprTree->parent->right.flags&EXPR_NEGATE || 
                    exprTree->right.flags&EXPR_NEGATE) 
               exprTree->right.flags |= EXPR_NEGATE;

            if(exprTree->parent->right.flags&EXPR_NOT && exprTree->right.flags&EXPR_NOT) 
               exprTree->right.flags &= (0xFFFFFFFF - EXPR_NOT);
            else if(exprTree->parent->right.flags&EXPR_NOT || exprTree->right.flags&EXPR_NOT) 
               exprTree->right.flags |= EXPR_NOT;

            exprTree->parent->right = exprTree->left;
         }

         /*************************************************************************/
         /* Set the left branch parent, if necessary, and remove the current tree */
         /*************************************************************************/
         if(exprTree->left.flags & EXPR_TREE) 
            exprTree->left.val.tree->parent = exprTree->parent;
 
         memset(exprTree, 0, sizeof(tExprTree));
         FreeExprTree(exprTree);

         return(OK);
      }
      else 
         return(status);
   }


   /*************************************/
   /* Parse remainder of the expression */
   /*************************************/
   /* First, allocate a new expression sub-tree */
   if((status=AllocateExprTree(&exprTree->right.val.tree)))
   {  AddErrMsg(FATAL_ENGINE|PARSE_EXPR|status, NULL, 0, parse);
      return(FATAL_ENGINE);
   }
   exprTree->right.flags |= EXPR_TREE;
   exprTree->right.val.tree->parent = exprTree;
   exprTree->right.val.tree->numParens = exprTree->numParens;
   
   if((status = ParseExprTree(parse, optionTerms, exprTree->right.val.tree)))
   {  if(status!=END_OF_EXPR)
         return(status);
   }

   /*******************************************************************/
   /* Check if this expression has a lower precedence than its parent */
   /* If so, then swap them around so that this branch becomes the    */
   /* parent and the old parent becomes the left branch.              */ 
   /*******************************************************************/
   if(exprTree->parent)
   {  if(DoOpSwap(exprTree->parent->op, exprTree->op))
      {  /******************************************************/
         /* Allocate a new sub-tree, which will become the new */
         /* left-most branch of the parent                     */
         /******************************************************/
         if((status=AllocateExprTree(&temp)))
         {  AddErrMsg(FATAL_ENGINE|PARSE_EXPR|status, NULL, 0, parse);
            return(FATAL_ENGINE);
         }

         /********************************************************/
         /* Find left-most branch of the current, which is where */ 
         /* the new node will go.                                */
         /********************************************************/
         leftTree = exprTree;
         while((leftTree->left.flags & EXPR_TREE) &&
               !(leftTree->left.flags & EXPR_PAREN) &&
                DoOpSwap(exprTree->parent->op, leftTree->left.val.tree->op))
            leftTree = leftTree->left.val.tree;

         temp->op = exprTree->parent->op;
         temp->numParens = exprTree->numParens;

         temp->left = exprTree->parent->left;
         if(temp->left.flags & EXPR_TREE)
            temp->left.val.tree->parent = temp;

         temp->right = leftTree->left;
         if(temp->right.flags & EXPR_TREE)
            temp->right.val.tree->parent = temp;

         exprTree->parent->left.flags = EXPR_TREE;
         if(leftTree==exprTree) 
         {  exprTree->parent->left.val.tree = temp;
            temp->parent = exprTree->parent;
         }
         else 
         {  exprTree->parent->left.val.tree = exprTree->left.val.tree;
            exprTree->left.val.tree->parent = exprTree->parent;

            leftTree->left.flags = EXPR_TREE;
            leftTree->left.val.tree = temp;
            temp->parent = leftTree;
         }

         exprTree->parent->op = exprTree->op;
         exprTree->parent->right = exprTree->right;
         if(exprTree->right.flags & EXPR_TREE)
            exprTree->right.val.tree->parent = exprTree->parent;

         memset(exprTree, 0, sizeof(tExprTree));
         FreeExprTree(exprTree);
      }
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CheckReadTillEnd()                                                       */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long CheckReadTillEnd(tParse *parse, char *optionTerms)
{
   /********************************************************/
   /* Only want to read until the end of the expression if */
   /* there is an expected ';' terminator and the status   */
   /* error was not UNEXPCT_KEYWD                          */
   /********************************************************/
   if(CheckOptionTerms(";", optionTerms) && strcmp(parse->token, ";"))
   {  if(parse->curErr)
      {  if((parse->curErr->errCode&0x000000FF) == UNEXPCT_KEYWD)
         {  /* Trick the engine into thinking the expression was */
            /* properly terminated with a ';' so parsing can     */
            /* continue.                                         */
            parse->ruleText[parse->lastIndex-1] = ';';
            parse->index--;
            parse->lastIndex--;
         }
         else
            return(ReadTillExprEnd(parse, optionTerms));
      }
   } 
   
   return(OK);
}


/*--------------------------------------------------------------------------*/
/* ParseExpression()                                                        */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long ParseExpression
(tParse *parse, char *optionTerms, tExprTree **pExprTree)
{
   long       retStatus=OK, status;
   tExprTree  *exprTree;
   int        numParens=0;
   
   if(pExprTree) *pExprTree = NULL;


   /**************************/
   /* Allocate the root node */
   /**************************/
   if((status=AllocateExprTree(&exprTree)))
   {  AddErrMsg(FATAL_ENGINE|PARSE_EXPR|status, NULL, 0, parse);
      return(FATAL_ENGINE);
   }
   exprTree->parent = NULL;
   exprTree->numParens = &numParens;

   /******************************************************/
   /* Recursively parse the expression.  If anything but */
   /* OK is returned free the tree and return.           */
   /******************************************************/
   status = ParseExprTree(parse, optionTerms, exprTree);
   if(status!=OK && status!=END_OF_RULE)
   {  FreeExprTree(exprTree);
      if(status==FATAL_ENGINE) 
         return(status);

      if(CheckReadTillEnd(parse, optionTerms)==FATAL_ENGINE)
         return(FATAL_ENGINE);

      return(status);
   }
   
   /***********************************************/
   /* Make sure that the parentheses were matched */
   /***********************************************/
   if(numParens > 0)
      retStatus = AddErrMsg(FAIL_ENGINE|PARSE_EXPR|UNMTCH_LPAREN, NULL, 0, parse);
   else if(numParens < 0)
      retStatus = AddErrMsg(FAIL_ENGINE|PARSE_EXPR|UNMTCH_RPAREN, NULL, 0, parse);
   else
   {  if((status=TypeCheckAndRollUpExpression(exprTree, parse, NULL)))
      {  FreeExprTree(exprTree);

         return(status);
      }
   }

   /************************************************/
   /* Set the return pointer, if one is passed in. */
   /* Otherwise, free the tree.                    */
   /************************************************/
   if(pExprTree) *pExprTree = exprTree;
   else FreeExprTree(exprTree);
   
   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CheckForExpression()                                                     */
/*                                                                          */
/*--------------------------------------------------------------------------*/
int CheckForExpression(tParse *parse, char *optionTerms)
{
   int  indexStore = parse->index;
   int  retValue=TRUE;
   long status;
   
   while((status=LexRule(parse))==FAIL_ENGINE);
   if(status==END_OF_RULE)
      retValue=FALSE;
   else if(CheckOptionTerms(parse->token, optionTerms))
      retValue=FALSE;
   else if(!strcmp(parse->token, "("))
   {  /* Check for empty parentheses */
      while((status=LexRule(parse))==FAIL_ENGINE);
      if(!strcmp(parse->token, ")"))
         return(FALSE);
   }
   
   parse->index = indexStore;
   return(retValue);   
}


/*--------------------------------------------------------------------------*/
/* ParseReturnStatement()                                                   */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long ParseReturnStatement(tParse *parse, char *optionTerms, tExprTree **pExprTree)
{
   long      status;
   tExprTree *exprTree=NULL;
   int       type=NOTYPE;

#ifdef DEVELOP
   debug_out("parsing return call\n", "");
#endif
   
   /********************************************/
   /* Check for return expression and parse it */
   /********************************************/
   if(CheckForExpression(parse, optionTerms))
   {  /*******************************/
      /* Parse the return expression */
      /*******************************/
      if((status=ParseExpression(parse, optionTerms, &exprTree)))
         return(status);
      type = exprTree->type;
  
      /******************************/
      /* Put back terminating token */
      /******************************/
      PutBackToken(parse);
   }
      
   /******************************************************/
   /* Make sure expression type matches rule return type */
   /******************************************************/
   if((status=CompareType(parse->curRule->returnType, type)))
   {  AddErrMsg(FAIL_ENGINE|PARSE_RETURN|status, NULL, 0, parse);
      return(FAIL_ENGINE);
   } 

   /************************************************************/
   /* If the rule return type is numeric, make sure the return */
   /* expression is set as a numeric type.                     */               
   /************************************************************/
   if(parse->curRule->returnType == NUMERIC_TYPE)
      ResetTreeType(exprTree, NUMERIC_TYPE, parse); 

   if(pExprTree) *pExprTree = exprTree;
   else if(exprTree) FreeExprTree(exprTree);

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* ParseTerminateStatement()                                                */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long ParseTerminateStatement(tParse *parse, char *optionTerms, tExprTree **pExprTree)
{
   long      status;
   tExprTree *exprTree=NULL;
   int       type=NOTYPE;

#ifdef DEVELOP
   debug_out("parsing terminate call\n", "");
#endif
   
   /***********************************************/
   /* Check for terminate expression and parse it */
   /***********************************************/
   if(CheckForExpression(parse, optionTerms))
   {  /*******************************/
      /* Parse the return expression */
      /*******************************/
      if((status=ParseExpression(parse, optionTerms, &exprTree)))
         return(status);
      type = exprTree->type;

      /******************************/
      /* Put back terminating token */
      /******************************/
      PutBackToken(parse);
      
      /***********************************/
      /* Make sure expression is numeric */
      /***********************************/
      if(!IsNumeric(exprTree->type))
      {  AddErrMsg(FAIL_ENGINE|PARSE_TERMINATE|TERM_NOT_NUMERIC, NULL, 0, parse);
         return(FAIL_ENGINE);
      }    

      if(pExprTree) *pExprTree = exprTree;
      else if(exprTree) FreeExprTree(exprTree);
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* ParseCaseStatement()                                                     */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long ParseCaseStatement(tParse *parse, tCaseData **pCaseData)
{
   long      status;
   int       caseType, badType = FALSE;
   tExprTree *caseExpr;
   tCase     *newCase=NULL, *caseList=NULL, *tmpCase, *delCase;
   tCaseArg  *newVal=NULL, *tmpVal, *delVal;

#ifdef DEVELOP
   debug_out("parsing CASE statement\n", "");
#endif

   /********************************************************/
   /* If pCaseData is not NULL, then allocate space for it */
   /********************************************************/
   if(pCaseData)
   {  if((*pCaseData = (tCaseData*)MyTmpMalloc(sizeof(tCaseData)))==NULL)
         throw(MALLOC_EXCEPTION);
       memset((char*)*pCaseData, 0, sizeof(tCaseData));
   }


   /***************************/
   /* Get the case expression */
   /***************************/
   if((status=ParseExpression(parse, "OF", &caseExpr)==FATAL_ENGINE))
      return(FATAL_ENGINE);
   else if(status==FAIL_ENGINE || !caseExpr)
   {  if(ReadTillExprEnd(parse, "OF")==FATAL_ENGINE) 
         return(FATAL_ENGINE);
      caseType = NUMERIC_TYPE;
   }
   else 
      caseType = caseExpr->type;

   if(pCaseData) (*pCaseData)->caseExpr = caseExpr;

   /*************************************/
   /* Make sure case is numeric or char */
   /*************************************/
   if(!IsIntNumeric(caseType) && caseType != HNC_CHAR)
   {  if(AddErrMsg(FAIL_ENGINE|PARSE_CASE|BAD_CASE_TYPE, NULL, 0, parse)==FATAL_ENGINE)
         return(FATAL_ENGINE); 
      badType = TRUE;
   }
   

   /****************************/
   /* Make sure there is an OF */
   /****************************/
   if(strcmp(parse->token, "OF"))
   {  if(AddErrMsg(FAIL_ENGINE|PARSE_CASE|CASE_NO_OF, NULL, 0, parse)==FATAL_ENGINE)
         return(FATAL_ENGINE); 
   }


   /****************************************/
   /* Read all the cases and their actions */ 
   /****************************************/
   while(1)
   {  /* Allocate a case structure, if needed */
      if(newCase)
      {  if((newCase->next = (tCase*)MyTmpMalloc(sizeof(tCase)))==NULL) 
            throw(MALLOC_EXCEPTION);
         newCase = newCase->next;
      }
      else
      {  if((newCase = (tCase*)MyTmpMalloc(sizeof(tCase)))==NULL) 
            throw(MALLOC_EXCEPTION);
         caseList = newCase;
      }
      memset((char*)newCase, 0, sizeof(tCase));

      /* Read all the case values */
      newVal = NULL;
      while(1)
      {  /* Read a value token */ 
         parse->inExpr = TRUE;
         while((status=LexRule(parse))==FAIL_ENGINE);
         parse->inExpr = FALSE;
         if(status==FATAL_ENGINE)
            return(FATAL_ENGINE);
         else if(status==END_OF_RULE)
         {  AddErrMsg(FATAL_ENGINE|PARSE_CASE|EARLY_TERM, NULL, 0, parse);
            return(FATAL_ENGINE);
         } 
         else if(!strcmp(parse->token, "END"))
         {  if(newCase->valList)
            {  if((AddErrMsg(FAIL_ENGINE|PARSE_CASE|END_IN_LIST, 
                                            NULL, 0, parse))==FATAL_ENGINE)
                  return(FATAL_ENGINE);
            } 
            else
            {  PutBackToken(parse);
               if(caseList==newCase) caseList = NULL;
               else
               {  for(tmpCase=caseList; tmpCase; tmpCase=tmpCase->next)
                     if(tmpCase->next==newCase)
                     {  tmpCase->next = NULL; 
                        break;
                     }
               }
               MyFree((void*)newCase);

               newCase = NULL;
               break;
            }
         }
         else if(parse->tokenType==T_KEYWORD && strcmp(parse->token, "DEFAULT"))
         {  if(AddErrMsg(FAIL_ENGINE|PARSE_CASE|CASE_NO_END, 
                                parse->token, 0, parse)==FATAL_ENGINE)
               return(FATAL_ENGINE);
            PutBackToken(parse);
            if(caseList==newCase) caseList = NULL;
            else
            {  for(tmpCase=caseList; tmpCase; tmpCase=tmpCase->next)
                  if(tmpCase->next==newCase)
                     tmpCase->next = NULL; 
            }
            MyFree((void*)newCase);
            newCase = NULL;
            break;
         }
         else if(parse->tokenType==T_VARIABLE)
         {  if((status=CheckVariable(parse->token, parse)))  
            {  if(AddErrMsg(FAIL_ENGINE|PARSE_CASE|BAD_CASE_TOK, 
                                   parse->token, 0, parse)==FATAL_ENGINE)
                  return(FATAL_ENGINE);
            }
         }
         else if(strcmp(parse->token, "DEFAULT"))
         {  /* Allocate new value */
            if(newVal)
            {  if((newVal->next = (tCaseArg*)MyMalloc(sizeof(tCaseArg)))==NULL) 
                  throw(MALLOC_EXCEPTION);
               newVal = newVal->next;
            }
            else
            {  if((newVal = (tCaseArg*)MyMalloc(sizeof(tCaseArg)))==NULL) 
                  throw(MALLOC_EXCEPTION);
               newCase->valList = newVal;
            }
            memset((char*)newVal, 0, sizeof(tCaseArg));

            if(parse->tokenType==T_INTEGER_CONSTANT)
            {  /* Make sure that the case type is numeric */ 
               if(!IsIntNumeric(caseType) && !badType)
               {  if(AddErrMsg(FAIL_ENGINE|PARSE_CASE|CASE_NOT_STRING, 
                                        parse->token, 0, parse)==FATAL_ENGINE)
                     return(FATAL_ENGINE); 
               }

               /* Set the value */
               newVal->atom.data.num = parse->data.num;
               newVal->atom.type = NUMERIC_TYPE;
   
               /* Make sure the value is unique */
               for(tmpCase=caseList; tmpCase; tmpCase=tmpCase->next)
                  for(tmpVal=tmpCase->valList; tmpVal; tmpVal=tmpVal->next)
                     if(IS_EQUAL(parse->data.num, tmpVal->atom.data.num) &&
                        tmpVal != newVal)
                     {  if(AddErrMsg(FAIL_ENGINE|PARSE_CASE|CASE_VAL_DUP, 
                                           parse->token, 0, parse)==FATAL_ENGINE)
                           return(FATAL_ENGINE); 
                     }
            }
            else if(parse->tokenType==T_CHAR_CONSTANT)
            {  /* Make sure that the case type is char or string */  
               if(!IsString(caseType) && !badType)
               {  if(AddErrMsg(FAIL_ENGINE|PARSE_CASE|CASE_NOT_NUMERIC, 
                                        parse->token, 0, parse)==FATAL_ENGINE)
                     return(FATAL_ENGINE); 
               }
   
               /* Set the value */
               newVal->atom.data.cVal = parse->token[0];
               newVal->atom.type = HNC_CHAR;
   
               /* Make sure the value is unique */
               for(tmpCase=caseList; tmpCase; tmpCase=tmpCase->next)
                  for(tmpVal=tmpCase->valList; tmpVal; tmpVal=tmpVal->next)
                     if(parse->token[0] == tmpVal->atom.data.cVal && 
                        tmpVal != newVal)
                     {  if(AddErrMsg(FAIL_ENGINE|PARSE_CASE|CASE_VAL_DUP, 
                                           parse->token, 0, parse)==FATAL_ENGINE)
                           return(FATAL_ENGINE); 
                     }
            }
            else
            {  if(AddErrMsg(FAIL_ENGINE|PARSE_CASE|BAD_CASE_TOK, 
                                   parse->token, 0, parse)==FATAL_ENGINE)
                  return(FATAL_ENGINE);
            }
         }

         /* Read the comma or the colon */
         while((status=LexRule(parse))==FAIL_ENGINE);
         if(status==FATAL_ENGINE)
            return(FATAL_ENGINE);
         else if(status==END_OF_RULE)
         {  AddErrMsg(FATAL_ENGINE|PARSE_CASE|EARLY_TERM, NULL, 0, parse);
            return(FATAL_ENGINE);
         } 
         else if(!strcmp(parse->token, ":"))
            break;
         else if(!strcmp(parse->token, ",") && newCase->valList)
            continue;
         else
         {  if(AddErrMsg(FAIL_ENGINE|PARSE_CASE|BAD_CASE_TOK, 
                                parse->token, 0, parse)==FATAL_ENGINE)
               return(FATAL_ENGINE);
         }
      }

      /* Get the statement list for this case, if any */
      if(newCase)
      {  if(*pCaseData)
         {  if(ParseStatementList(parse, FROM_CASE, &newCase->stmtList)==FATAL_ENGINE)
               return(FATAL_ENGINE);
         }
         else
         {  /* Just parse the statement list without storing it */
            if(ParseStatementList(parse, FROM_CASE, NULL)==FATAL_ENGINE)
               return(FATAL_ENGINE);
         }
      }

      /* Look for the END to the CASE */
      while((status=LexRule(parse))==FAIL_ENGINE);
      if(status==FATAL_ENGINE)
         return(FATAL_ENGINE);
      else if(status==END_OF_RULE)
      {  AddErrMsg(FATAL_ENGINE|PARSE_CASE|EARLY_TERM, NULL, 0, parse);
         return(FATAL_ENGINE);
      } 
      else if(!strcmp(parse->token, "END"))
         break;
      else if(!strcmp(parse->token, ";"))
         continue;
      else    
         PutBackToken(parse);
   }

   /* Make sure there are case element */
   if(caseList == NULL)
   {  if(AddErrMsg(FAIL_ENGINE|PARSE_CASE|EMPTY_CASE, NULL, 0, parse)==FATAL_ENGINE)
         return(FATAL_ENGINE);
   } 

   if(pCaseData)
      (*pCaseData)->caseList = caseList;
   else
   {  tmpCase = caseList;
      while(tmpCase) 
      {  tmpVal = tmpCase->valList; 
         while(tmpVal)
         {  delVal = tmpVal;
            tmpVal = tmpVal->next;
            MyFree((void*)tmpVal);
         }

         delCase = tmpCase;
         tmpCase = tmpCase->next;
         MyFree((void*)tmpCase);
      }
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* ParseForLoop()                                                           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long ParseForLoop(tParse *parse, tForData **pForData)
{
   long      direction;
   char      loopVar[50];
   tEngNum   initVal, limitVal, stepVal;
   tExprTree *initExpr, *limitExpr, *stepExpr=NULL;

#ifdef DEVELOP
   debug_out("parsing FOR loop\n", "");
#endif

   /*******************************************************/
   /* If pForData is not NULL, then allocate space for it */
   /*******************************************************/
   if(pForData)
   {  if((*pForData = (tForData*)MyTmpMalloc(sizeof(tForData)))==NULL)
         throw(MALLOC_EXCEPTION);
       memset((char*)*pForData, 0, sizeof(tForData));
   }

   /***********************************/
   /* Get the initializing expression */
   /***********************************/
   if(ParseExpression(parse, "TO DOWNTO", &initExpr)==FATAL_ENGINE)
      return(FATAL_ENGINE);

   /*****************************************************/
   /* Make sure the init expression is a SET expression */
   /*****************************************************/
   if(initExpr)
   {  if(initExpr->op != SET)
      {  if(initExpr->op == EQUAL)
         {  if(AddErrMsg(FAIL_ENGINE|PARSE_FOR|FOR_USE_EQL, NULL, 0, parse)==FATAL_ENGINE)
               return(FATAL_ENGINE); 
         }
         else 
         {  if(AddErrMsg(FAIL_ENGINE|PARSE_FOR|FOR_BAD_INIT, NULL, 0, parse)==FATAL_ENGINE)
               return(FATAL_ENGINE); 
         }
      }

      /*****************************************************/
      /* Get loop control variable.  It must be a variable */
      /*****************************************************/
      if(!(initExpr->left.flags&EXPR_VARBLE))
      {  if(AddErrMsg(FAIL_ENGINE|PARSE_FOR|LOOP_NOTVAR, NULL, 0, parse)==FATAL_ENGINE)
            return(FATAL_ENGINE); 
         loopVar[0] = '\0';
      }
      else if(!(initExpr->left.val.var->flags&VAR_LOCAL))
      {  if(AddErrMsg(FAIL_ENGINE|PARSE_FOR|LOOP_NOTVAR, NULL, 0, parse)==FATAL_ENGINE)
            return(FATAL_ENGINE); 
         loopVar[0] = '\0';
      }
      else
      {  strcpy(loopVar, initExpr->left.val.var->varName);
         AddLoopVariable(loopVar);
      }

      /************************************************************/
      /* Also, (for now) make sure that the expression is numeric */
      /* Pascal can loop through characters, but that isn't       */
      /* supported here.                                          */
      /************************************************************/
      if(!IsNumeric(initExpr->type))
      {  if(AddErrMsg(FAIL_ENGINE|PARSE_FOR|FOR_NOT_NUMERIC, NULL, 0, parse)==FATAL_ENGINE)
            return(FATAL_ENGINE); 
      }
   }

   /**************************************************************/
   /* Get the direction of the loop.  Must have a TO or a DOWNTO */
   /**************************************************************/
   if(!strcmp(parse->token, "TO"))
      direction = COUNT_UP;
   else if(!strcmp(parse->token, "DOWNTO"))
      direction = COUNT_DOWN;
   else 
   {  AddErrMsg(FATAL_ENGINE|PARSE_FOR|MISSING_TO, NULL, 0, parse);
      return(FATAL_ENGINE);
   }


   /*********************************/
   /* Get the comparison expression */
   /*********************************/
   if(ParseExpression(parse, "DO STEP", &limitExpr)==FATAL_ENGINE)
      return(FATAL_ENGINE);

   /************************************************************/
   /* Also, (for now) make sure that the expression is numeric */
   /* Pascal can loop through characters, but that isn't       */
   /* supported here.                                          */
   /************************************************************/
   if(limitExpr)
   {  if(!IsNumeric(limitExpr->type))
      {  if(AddErrMsg(FAIL_ENGINE|PARSE_FOR|FOR_NOT_NUMERIC, NULL, 0, parse)==FATAL_ENGINE)
            return(FATAL_ENGINE); 
      }
   }


   /**************************************************************/
   /* Check for a STEP expression.  There must be a STEP or a DO */
   /**************************************************************/
   if(!strcmp(parse->token, "STEP"))
   {  /***************************/
      /* Get the step expression */
      /***************************/
      if(ParseExpression(parse, "DO", &stepExpr)==FATAL_ENGINE)
         return(FATAL_ENGINE);

      /************************************************************/
      /* Also, (for now) make sure that the expression is numeric */
      /* Pascal can loop through characters, but that isn't       */
      /* supported here.                                          */
      /************************************************************/
      if(!IsNumeric(stepExpr->type))
      {  if(AddErrMsg(FAIL_ENGINE|PARSE_FOR|FOR_NOT_NUMERIC, NULL, 0, parse)==FATAL_ENGINE)
            return(FATAL_ENGINE); 
      }

      /******************************/
      /* Make sure it ended with DO */
      /******************************/
      if(strcmp(parse->token, "DO"))
      {  if(AddErrMsg(FAIL_ENGINE|PARSE_FOR|FOR_NO_DO, NULL, 0, parse)==FATAL_ENGINE)
            return(FATAL_ENGINE); 
      }
   }
   else if(strcmp(parse->token, "DO"))
   {  if(AddErrMsg(FAIL_ENGINE|PARSE_FOR|FOR_NO_DO, NULL, 0, parse)==FATAL_ENGINE)
         return(FATAL_ENGINE); 
   }
   else
   {  /* Create a step expression */
      if((stepExpr = (tExprTree*)MyTmpMalloc(sizeof(tExprTree)))==NULL)
         throw(MALLOC_EXCEPTION);
      memset((char*)stepExpr, 0, sizeof(tExprTree));
      stepExpr->op = NOOP;
      stepExpr->type = NUMERIC_TYPE;

      ONE(stepExpr->left.val.num)
      stepExpr->left.type = NUMERIC_TYPE;
      stepExpr->left.flags |= EXPR_CONSTANT;
      
      (*pForData)->stepExpr = stepExpr;
   }

   /********************************************************************/
   /* Check to see if loop data is valid and won't be an infinite loop */
   /********************************************************************/
   if(initExpr && limitExpr)
   {  if(initExpr->right.flags&EXPR_CONSTANT)
      {  initVal = initExpr->right.val.num;
   
         if(limitExpr->op == NOOP && limitExpr->left.flags&EXPR_CONSTANT)
         {  limitVal = limitExpr->left.val.num;
   
            if(stepExpr->op == NOOP && stepExpr->left.flags&EXPR_CONSTANT)
            {  stepVal = stepExpr->left.val.num;
   
               if(IS_GRTR(initVal, limitVal) && 
                                    direction == COUNT_UP && IS_POS(stepVal))
               {  if(AddErrMsg(WARN_ENGINE|PARSE_FOR|FOR_NO_RUN, 
                                     NULL, 0, parse)==FATAL_ENGINE)
                     return(FATAL_ENGINE); 
               }
               else if(IS_LESS(initVal, limitVal) && 
                                    direction == COUNT_DOWN && IS_POS(stepVal))
               {  if(AddErrMsg(WARN_ENGINE|PARSE_FOR|FOR_NO_RUN, 
                                     NULL, 0, parse)==FATAL_ENGINE)
                     return(FATAL_ENGINE); 
               }
               else if(IS_GRTR(initVal, limitVal) && 
                                    direction == COUNT_DOWN && IS_NEG(stepVal))
               {  if(AddErrMsg(WARN_ENGINE|PARSE_FOR|FOR_NO_RUN, 
                                     NULL, 0, parse)==FATAL_ENGINE)
                     return(FATAL_ENGINE); 
               }
               else if(IS_LESS(initVal, limitVal) && 
                                    direction == COUNT_UP && IS_NEG(stepVal))
               {  if(AddErrMsg(WARN_ENGINE|PARSE_FOR|FOR_NO_RUN, 
                                     NULL, 0, parse)==FATAL_ENGINE)
                     return(FATAL_ENGINE); 
               }
               else if(IS_ZERO(stepVal))
               {  if(AddErrMsg(FAIL_ENGINE|PARSE_FOR|FOR_ZERO_STEP, 
                                     NULL, 0, parse)==FATAL_ENGINE)
                     return(FATAL_ENGINE); 
               }
            }   
         }
      }
   }


   /************************************************************/
   /* If pForData is not NULL, then parse the statement list   */
   /* and set the primary statement list in ifData             */
   /************************************************************/
   if(pForData)
   {  if(ParseStatementList(parse, FROM_OTHER, &(*pForData)->stmtList)==FATAL_ENGINE)
         return(FATAL_ENGINE);
   }
   else
   {  /****************************************************/
      /* Just parse the statement list without storing it */
      /****************************************************/
      if(ParseStatementList(parse, FROM_OTHER, NULL)==FATAL_ENGINE)
         return(FATAL_ENGINE);
   }

   /*********************************************************/
   /* Set the expressions in the pForData structure, if any */
   /*********************************************************/
   if(pForData)
   {  (*pForData)->initExpr = initExpr; 
      (*pForData)->limitExpr = limitExpr; 
     
      if(initExpr)
         (*pForData)->loopVar = initExpr->left.val.var;
     
      /* If no step expression, then create one */
      (*pForData)->stepExpr = stepExpr;
      
      (*pForData)->direction = direction;
   }
   else
   {  if(stepExpr) FreeExprTree(initExpr);
      if(stepExpr) FreeExprTree(limitExpr);
      if(stepExpr) FreeExprTree(stepExpr);
   }
  
   RemoveLoopVariable(loopVar);
   return(OK);
}


/*--------------------------------------------------------------------------*/
/* ParseWhileLoop()                                                         */
/*                                                                          */
/*--------------------------------------------------------------------------*/
#define WHILE_LIMIT   10000
long ParseWhileLoop(tParse *parse, tWhileData **pWhileData)
{
   long  status, limit;

#ifdef DEVELOP
   debug_out("parsing WHILE loop\n", "");
#endif

   /**************************************************************/
   /* If pWhileData is not NULL, then allocate space for it and  */
   /* parse the conditional expression, storing the expression   */
   /**************************************************************/
   if(pWhileData)
   {  if((*pWhileData = (tWhileData*)MyTmpMalloc(sizeof(tWhileData)))==NULL)
         throw(MALLOC_EXCEPTION);
       memset((char*)*pWhileData, 0, sizeof(tWhileData));

      if(ParseExpression(parse, "LIMIT DO", &(*pWhileData)->condExpr)==FATAL_ENGINE)
         return(FATAL_ENGINE);
   }
   else
   {  /************************************************/
      /* Just parse the expression without storing it */
      /************************************************/
      if(ParseExpression(parse, "LIMIT DO", NULL)==FATAL_ENGINE)
         return(FATAL_ENGINE);
   }


   /*******************/
   /* Check for LIMIT */
   /*******************/
   limit = (parse->whileLimit > 0)?parse->whileLimit:WHILE_LIMIT;
   if(!strcmp(parse->token, "LIMIT"))
   {  /* Get the limit for the while loop */
      parse->inExpr = TRUE;
      while((status=LexRule(parse))==FAIL_ENGINE);
      parse->inExpr = FALSE;
      if(status==FATAL_ENGINE)
         return(FATAL_ENGINE);
      else if(status==END_OF_RULE)
      {  AddErrMsg(FATAL_ENGINE|PARSE_WHILE|EARLY_TERM, NULL, 0, parse);
         return(FATAL_ENGINE);
      }
      else if(parse->tokenType == T_INTEGER_CONSTANT)
         limit = NUM_GETLNG(parse->data.num);
      else 
      {  if(AddErrMsg(FAIL_ENGINE|PARSE_WHILE|BAD_WHILE_LIMIT, NULL, 0, parse)==FATAL_ENGINE)
            return(FATAL_ENGINE); 
         status=FAIL_ENGINE;
      }

      if(limit <= 0)
      {  if(AddErrMsg(FAIL_ENGINE|PARSE_WHILE|BAD_WHILE_LIMIT, NULL, 0, parse)==FATAL_ENGINE)
            return(FATAL_ENGINE); 
         limit = (parse->whileLimit > 0)?parse->whileLimit:WHILE_LIMIT;
         status=FAIL_ENGINE;
      }

      /* Get next token, which should be a 'DO' */
      if(strcmp(parse->token, "DO"))
      {  while((status=LexRule(parse))==FAIL_ENGINE);
         if(status==FATAL_ENGINE)
            return(FATAL_ENGINE);
         else if(status==END_OF_RULE)
         {  AddErrMsg(FATAL_ENGINE|PARSE_WHILE|EARLY_TERM, NULL, 0, parse);
            return(FATAL_ENGINE);
         }
         else if(strcmp(parse->token, "DO"))
         {  if(AddErrMsg(FAIL_ENGINE|PARSE_WHILE|SYNTX_ERR, 
                               parse->token, 0, parse)==FATAL_ENGINE)
               return(FATAL_ENGINE); 
            if(ReadTillExprEnd(parse, "DO BEGIN END ;")==FATAL_ENGINE) 
               return(FATAL_ENGINE);
         }
      }
   }

   /****************/
   /* Check for DO */
   /****************/
   if(strcmp(parse->token, "DO"))
   {  if(AddErrMsg(FAIL_ENGINE|PARSE_WHILE|WHILE_NO_DO, NULL, 0, parse)==FATAL_ENGINE)
         return(FATAL_ENGINE); 
   }


   /************************************************************/
   /* If pWhileData is not NULL, then parse the statement list */
   /* and set the statement list in whileData                  */
   /************************************************************/
   if(pWhileData)
   {  if(ParseStatementList(parse, FROM_OTHER, &(*pWhileData)->stmtList)==FATAL_ENGINE)
         return(FATAL_ENGINE);
      (*pWhileData)->limit = limit;
   }
   else
   {  /****************************************************/
      /* Just parse the statement list without storing it */
      /****************************************************/
      if(ParseStatementList(parse, FROM_OTHER, NULL)==FATAL_ENGINE)
         return(FATAL_ENGINE);
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* ParseElse()                                                              */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long ParseElse(tParse *parse, tIfData *ifData)
{
   long status;

#ifdef DEVELOP
   debug_out("parsing ELSE statement\n", "");
#endif

   /****************************************/
   /* See if there is an IF with this ELSE */
   /****************************************/
   while((status=LexRule(parse))==FAIL_ENGINE);
   if(status==FATAL_ENGINE)
      return(FATAL_ENGINE);
   else if(status==END_OF_RULE)
   {  AddErrMsg(FATAL_ENGINE|PARSE_ELSE|EARLY_TERM, NULL, 0, parse);
      return(FATAL_ENGINE);
   } 
   else if(!strcmp(parse->token, "IF"))
   {  /************************************************/
      /* If ifData is not NULL, set secondary type to */
      /* reflect ELSE IF and parse the IF, storing it */
      /************************************************/
      if(ifData) 
      {  ifData->secondaryType = ELSE_IF_STATEMENT;
         status=ParseIfStatement(parse, &(ifData->secondary.ifData));
      }
      else 
         /************************************************/
         /* Just parse and don't store parse info for IF */
         /************************************************/
         status=ParseIfStatement(parse, NULL);

      if(status!=OK) return(status);
   }
   else if(!strcmp(parse->token, ";"))
   {
#ifdef DEVELOP
      debug_out("Empty ELSE statement\n", "");
#endif
      return(OK);
   }
   else 
   {  /******************************************************/
      /* Have a statement list, put token back and parse it */
      /******************************************************/
      PutBackToken(parse);

      /********************************************************/
      /* If ifData is not NULL, set secondary type to reflect */ 
      /* ELSE and parse its statement block, storing the      */
      /* statement list.                                      */
      /********************************************************/
      if(ifData)
      {  ifData->secondaryType = ELSE_STATEMENT;
         if(ParseStatementList(parse, FROM_IF, &ifData->secondary.sList)==FATAL_ENGINE)
            return(FATAL_ENGINE);
      }
      else
      {  /******************************************************/
         /* Just parse and don't store statement list for ELSE */
         /******************************************************/
         if(ParseStatementList(parse, FROM_IF, NULL)==FATAL_ENGINE)
            return(FATAL_ENGINE);
      }
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* ParseIf()                                                                */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long ParseIfStatement(tParse *parse, tIfData **pIfData)
{
   long  status;
   int   lineNumber, lastIndex;

#ifdef DEVELOP
   debug_out("parsing IF statement\n", "");
#endif

   /************************************************************/
   /* If pIfData is not NULL, then allocate space for it and   */
   /* parse the conditional expression, storing the expression */
   /************************************************************/
   if(pIfData)
   {  if((*pIfData = (tIfData*)MyTmpMalloc(sizeof(tIfData)))==NULL)
         throw(MALLOC_EXCEPTION);
       memset((char*)*pIfData, 0, sizeof(tIfData));

      if((status=ParseExpression(parse, "THEN", &(*pIfData)->condExpr)))
      {  if(status==FATAL_ENGINE)
            return(FATAL_ENGINE);
         if(ReadTillExprEnd(parse, "THEN")==FATAL_ENGINE) return(FATAL_ENGINE);
      }
   }
   else
   {  /************************************************/
      /* Just parse the expression without storing it */
      /************************************************/
      if(ParseExpression(parse, "THEN", NULL)==FATAL_ENGINE)
         return(FATAL_ENGINE);
   }


   /***********************************************************/
   /* Make sure terminated conditional expression with "THEN" */
   /***********************************************************/
   if(strcmp(parse->token, "THEN"))
   {  if(AddErrMsg(FAIL_ENGINE|PARSE_IF|IF_NO_THEN, NULL, 0, parse)==FATAL_ENGINE)
         return(FATAL_ENGINE); 
   }


   /*********************************************************/
   /* If pIfData is not NULL, then parse the statement list */
   /* and set the primary statement list in ifData          */
   /*********************************************************/
   if(pIfData)
   {  if(ParseStatementList(parse, FROM_IF, &(*pIfData)->primeSList)==FATAL_ENGINE)
         return(FATAL_ENGINE);
   }
   else
   {  /****************************************************/
      /* Just parse the statement list without storing it */
      /****************************************************/
      if(ParseStatementList(parse, FROM_IF, NULL)==FATAL_ENGINE)
         return(FATAL_ENGINE);
   }


   /****************************************************/
   /* Get terminating ';', or "ELSE" statement, if any */
   /* Initially assume that there is no secondary.     */
   /****************************************************/
   if(pIfData) (*pIfData)->secondaryType = NO_SECOND;
   while((status=LexRule(parse))==FAIL_ENGINE);
   if(status==FATAL_ENGINE)
      return(FATAL_ENGINE);
   else if(status==END_OF_RULE)
      return(OK);
   else if(!strcmp(parse->token, "ELSE"))
   {  /****************************/
      /* Parse ELSE statment list */
      /****************************/
      if(pIfData) 
      {  if(ParseElse(parse, *pIfData)==FATAL_ENGINE)
            return(FATAL_ENGINE);
      }
      else
      {  if(ParseElse(parse, NULL)==FATAL_ENGINE)
            return(FATAL_ENGINE);
      }
   }
   else if(!strcmp(parse->token, ";")) 
   {  /****************************************************/
      /* Check to see if there was a ';' and then an ELSE */
      /* This should be allowed, though Pascal doesn't    */
      /****************************************************/
      lastIndex = parse->lastIndex; 
      lineNumber = parse->lineNumber; 
      while((status=LexRule(parse))==FAIL_ENGINE);
      if(status==FATAL_ENGINE)
         return(FATAL_ENGINE);
      else if(status==END_OF_RULE)
         return(OK);
      else if(!strcmp(parse->token, "ELSE"))
      {  /****************************/
         /* Parse ELSE statment list */
         /****************************/
         if(pIfData) 
         {  if(ParseElse(parse, *pIfData)==FATAL_ENGINE)
               return(FATAL_ENGINE);
         }
         else
         {  if(ParseElse(parse, NULL)==FATAL_ENGINE)
               return(FATAL_ENGINE);
         }
      }
      else 
      {  parse->index = lastIndex;
         parse->lineNumber = lineNumber;
      }
   }
   else 
      PutBackToken(parse);
   
   return(OK);
}


/*--------------------------------------------------------------------------*/
/* ParseStatement()                                                         */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long ParseStatement(tParse *parse, char* optionTerms, tStatement *curStatement)
{
   long       status;
   tExprTree  *tmpExpr;

   /************************************************/
   /* Assume first token of statement already read */
   /************************************************/
   switch(parse->tokenType)  
   {  case T_KEYWORD:
         if(!strcmp(parse->token, "IF"))
         {  /*********************************************/
            /* If curStatement is not NULL, parse the IF */ 
            /* passing in an ifData structure to store   */
            /* parse information.                        */
            /*********************************************/
            if(curStatement)
            {  curStatement->type = IF_STATEMENT;
               if(ParseIfStatement(parse, &curStatement->data.ifData)==FATAL_ENGINE)
                  return(FATAL_ENGINE);
            }
            else
            {  if(ParseIfStatement(parse, NULL)==FATAL_ENGINE)
                  return(FATAL_ENGINE);
            }
            break;
         }
         else if(!strcmp(parse->token, "TERMINATE"))
         {
#ifdef DEVELOP
            debug_out("FOUND TERMINATE OPTION\n", "");
#endif
            /*****************************************************/
            /* If curStatement is not NULL, parse the expression */ 
            /* and store the tree.                               */
            /*****************************************************/
            if(curStatement)
            {  curStatement->type = TERM_STATEMENT;
               if(ParseTerminateStatement(parse, 
                                           optionTerms, 
                                           &curStatement->data.exprTree)==FATAL_ENGINE)
                    return(FATAL_ENGINE);
            }
            else
            {  if(ParseTerminateStatement(parse, optionTerms, NULL)==FATAL_ENGINE)
                  return(FATAL_ENGINE);
            }

            return(TERM_PARSE);
         }
         else if(!strcmp(parse->token, "RETURN"))
         {  /*****************************************************/
            /* If curStatement is not NULL, parse the expression */ 
            /* and store the tree.                               */
            /*****************************************************/
            if(curStatement)
            {  curStatement->type = RETURN_STATEMENT;
               if(ParseReturnStatement(parse, 
                                           optionTerms, 
                                           &curStatement->data.exprTree)==FATAL_ENGINE)
                  return(FATAL_ENGINE);
            }
            else
            {  if(ParseReturnStatement(parse, optionTerms, NULL)==FATAL_ENGINE)
                  return(FATAL_ENGINE);
            }

            return(RETURN_PARSE);
         }
         else if(!strcmp(parse->token, "FOR"))
         {  /*****************************************************/
            /* If curStatement is not NULL, parse the expression */ 
            /* and store the tree.                               */
            /*****************************************************/
            if(curStatement)
            {  curStatement->type = FOR_STATEMENT;
               if(ParseForLoop(parse, &curStatement->data.forData)==FATAL_ENGINE)
                  return(FATAL_ENGINE);
            }
            else
            {  if(ParseForLoop(parse, NULL)==FATAL_ENGINE)
                  return(FATAL_ENGINE);
            }
            break;
         }
         else if(!strcmp(parse->token, "WHILE"))
         {  /*****************************************************/
            /* If curStatement is not NULL, parse the expression */ 
            /* and store the tree.                               */
            /*****************************************************/
            if(curStatement)
            {  curStatement->type = WHILE_STATEMENT;
               if(ParseWhileLoop(parse, &curStatement->data.whileData)==FATAL_ENGINE)
                  return(FATAL_ENGINE);
            }
            else
            {  if(ParseWhileLoop(parse, NULL)==FATAL_ENGINE)
                  return(FATAL_ENGINE);
            }
            break;
         }
         else if(!strcmp(parse->token, "CASE"))
         {  /*****************************************************/
            /* If curStatement is not NULL, parse the expression */ 
            /* and store the tree.                               */
            /*****************************************************/
            if(curStatement)
            {  curStatement->type = CASE_STATEMENT;
               if(ParseCaseStatement(parse, &curStatement->data.caseData)==FATAL_ENGINE)
                  return(FATAL_ENGINE);
            }
            else
            {  if(ParseCaseStatement(parse, NULL)==FATAL_ENGINE)
                  return(FATAL_ENGINE);
            }
            break;
         }
         else if(!strcmp(parse->token, "RESOLVE") || !strcmp(parse->token, "EXEC"))
         {  /*********************************************************/
            /* Put back starting token for expression parser to read */
            /*********************************************************/
            PutBackToken(parse);
         
            /*****************************************************/
            /* If curStatement is not NULL, parse the expression */ 
            /* and store the tree.                               */
            /*****************************************************/
            if(curStatement)
            {  curStatement->type = EXPR_STATEMENT;
               if((status=ParseExpression(parse, 
                                      optionTerms, 
                                      &curStatement->data.exprTree))==FATAL_ENGINE)
                  return(FATAL_ENGINE);
               tmpExpr = curStatement->data.exprTree;
            }
            else
            {  if((status=ParseExpression(parse, optionTerms, &tmpExpr))==FATAL_ENGINE)
                  return(FATAL_ENGINE);
            }

            /******************************/
            /* Put back terminating token */
            /******************************/
            PutBackToken(parse);

            /*********************************************************/
            /* Return now if there was a status from ParseExpression */
            /*********************************************************/
            if(status) return(status);
            break;
         }
         else if(!strcmp(parse->token, "TRACEON"))
         {  parse->traceOn = TRUE;
            break;
         }
         else if(!strcmp(parse->token, "TRACEOFF"))
         {  parse->traceOn = FALSE;
            break;
         }
         else if(CheckOptionTerms(parse->token, optionTerms))
            break;
         else
         {  if(AddErrMsg(FAIL_ENGINE|PARSE_STATMNT|BAD_STATMNT, 
                                          parse->token, 0, parse)==FATAL_ENGINE)
               return(FATAL_ENGINE);
         }
         break;

      case T_PUNCTUATION:
         if(!strcmp(parse->token, ";"))
         {  PutBackToken(parse);
            if(AddErrMsg(WARN_ENGINE|PARSE_STATMNT|EMPTY_STATMNT, 
                                                  NULL, 0, parse)==FATAL_ENGINE)
               return(FATAL_ENGINE);
         }
         else
            if(AddErrMsg(FAIL_ENGINE|PARSE_STATMNT|BAD_STATMNT, 
                                          parse->token, 0, parse)==FATAL_ENGINE)
               return(FATAL_ENGINE);
         break;

      default:
         /*********************************************************/
         /* Put back starting token for expression parser to read */
         /*********************************************************/
         PutBackToken(parse);
         
         /*****************************************************/
         /* If curStatement is not NULL, parse the expression */ 
         /* and store the tree.                               */
         /*****************************************************/
         if(curStatement)
         {  curStatement->type = EXPR_STATEMENT;
            if((status=ParseExpression(parse, 
                                   optionTerms, 
                                   &curStatement->data.exprTree))==FATAL_ENGINE)
               return(FATAL_ENGINE);
            tmpExpr = curStatement->data.exprTree;
         }
         else
         {  if((status=ParseExpression(parse, optionTerms, &tmpExpr))==FATAL_ENGINE)
               return(FATAL_ENGINE);
         }

         /******************************/
         /* Put back terminating token */
         /******************************/
         PutBackToken(parse);

         /*********************************************************/
         /* Return now if there was a status from ParseExpression */
         /*********************************************************/
         if(status) return(status);

         /***************************************************/
         /* Make sure that the expression "does something"  */
         /* That is, it should call a rule or a function or */
         /* it should set a data field value.               */
         /*                                                 */
         /* NOTE: This forces expression statements to be   */
         /* simple rule calls, function calls, or value     */
         /* setting expressions only.                       */
         /***************************************************/
         if((status=CheckExpressionFunction(tmpExpr)))
         {  if(AddErrMsg(FAIL_ENGINE|PARSE_STATMNT|status, 
                                        NULL, 0, parse)==FATAL_ENGINE)
            return(FATAL_ENGINE);
         }
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* ParseStatementList()                                                     */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long ParseStatementList(tParse *parse, int from, tStatement **statementList)
{
   long        allocStmt=1, numStatements=0, status, endStatus=OK;
   tStatement *newStatement=NULL;
   char       *optionTerms;


   /***********************************************************/
   /* Looking for "BEGIN" -- May get first token of statement */
   /***********************************************************/
   while((status=LexRule(parse))==FAIL_ENGINE);
   if(status==FATAL_ENGINE)
      return(FATAL_ENGINE);
   else if(status==END_OF_RULE)
   {  AddErrMsg(FATAL_ENGINE|PARSE_STATMNT_LST|EARLY_TERM, NULL, 0, parse);
      return(FATAL_ENGINE);
   }
   else if(!strcmp(parse->token, "BEGIN"))
   {  /************************************************************/
      /* Increment numBegins to indicate that current state is in */
      /* a BEGIN/END block.                                       */
      /************************************************************/
      parse->numBegins++;  
      if(from!=FROM_OTHER) parse->numSubs++;

      /**********************************************/
      /* Expecting to read statements until hit END */
      /**********************************************/
      while(strcmp(parse->token, "END"))
      {  /*************************************/
         /* Read first token of the statement */
         /*************************************/
         while((status=LexRule(parse))==FAIL_ENGINE);
         if(status==FATAL_ENGINE)
            return(FATAL_ENGINE);
         else if(status==END_OF_RULE)
         {  AddErrMsg(FATAL_ENGINE|PARSE_STATMNT_LST|EARLY_TERM, NULL, 0, parse);
            return(FATAL_ENGINE);
         }
         else if(!strcmp(parse->token, "END"))
            break;

         if(endStatus==RETURN_PARSE)
         {  if(AddErrMsg(WARN_ENGINE|PARSE_STATMNT_LST|IGNORE_AFTER_TOK, 
                                                 "RETURN", 0, parse)==FATAL_ENGINE)
               return(FATAL_ENGINE);

            if(ReadTillExprEnd(parse, "END")==FATAL_ENGINE) return(FATAL_ENGINE);
            else return(OK);
         }
         else if(endStatus==TERM_PARSE)
         {  if(AddErrMsg(WARN_ENGINE|PARSE_STATMNT_LST|IGNORE_AFTER_TOK, 
                                                 "TERMINATE", 0, parse)==FATAL_ENGINE)
               return(FATAL_ENGINE);

            if(ReadTillExprEnd(parse, "END")==FATAL_ENGINE) return(FATAL_ENGINE);
            else return(OK);
         }

         /******************************************************************/
         /* Allocate the next (or first) statement in the list, if requred */
         /******************************************************************/
         if(statementList)
         {  if(allocStmt)
            {  if(newStatement==NULL)
               {  if((newStatement = (tStatement*)MyTmpMalloc(sizeof(tStatement)))==NULL)
                     throw(MALLOC_EXCEPTION);
                  *statementList = newStatement;
               }
               else
               {  if((newStatement->next = (tStatement*)MyTmpMalloc(sizeof(tStatement)))==NULL)
                     throw(MALLOC_EXCEPTION);
                  newStatement = newStatement->next;
               }
               memset((char*)newStatement, 0, sizeof(tStatement));
            }

            newStatement->traceOn = parse->traceOn;
            newStatement->level = parse->numBegins;
         }

         /*******************************/
         /* Parse the current statement */ 
         /*******************************/
         if((endStatus=ParseStatement(parse, "; END", newStatement))==FATAL_ENGINE)
            return(FATAL_ENGINE);

         /*************************************************************/
         /* If just TRACEON or TRACEOFF, then that is not a statement */
         /*************************************************************/
         if(!strcmp(parse->token, "TRACEON") || !strcmp(parse->token, "TRACEOFF"))
         {  allocStmt = 0;
            continue;
         }
         else
         {  numStatements++;
            allocStmt = 1;
         }

         /*******************************/
         /* Count the number of RETURNS */
         /*******************************/
         if(endStatus==RETURN_PARSE || endStatus==TERM_PARSE) 
         {  if(from==FROM_OTHER) parse->mainReturn = 1;
            parse->numReturns++;
         }

         /***********************************************************/
         /* Get the terminating token for the statement, unless was */
         /* an empty statement (a ';' only)                         */
         /***********************************************************/
         while((status=LexRule(parse))==FAIL_ENGINE);
         if(status==FATAL_ENGINE)
            return(FATAL_ENGINE);
         else if(status==END_OF_RULE)
         {  AddErrMsg(FATAL_ENGINE|PARSE_STATMNT_LST|UNMTCH_BEGIN, NULL, 0, parse);
            return(FATAL_ENGINE);
         }
         else if (!strcmp(parse->token, "END"))
            break;
         else if (strcmp(parse->token, ";"))
         {  if(AddErrMsg(FAIL_ENGINE|PARSE_STATMNT_LST|SYNTX_ERR, 
                                          parse->token, 0, parse)==FATAL_ENGINE)
               return(FATAL_ENGINE);
         }
      }

      /*****************************************************************/
      /* Make sure there are some statements, not just empty BEGIN/END */
      /*****************************************************************/
      if(numStatements == 0)
      {  if((status = 
              AddErrMsg(WARN_ENGINE|PARSE_STATMNT_LST|EMPTY_BLOCK, 
                                          NULL, 0, parse))==FATAL_ENGINE)
            return(FATAL_ENGINE);
      }

      /************************************************************/
      /* Decrement numBegins to indicate that current state is no */
      /* longer necessarily in a BEGIN/END block.                 */
      /************************************************************/
      if(parse->numBegins > 0) parse->numBegins--;
      else
      {  if((status = 
              AddErrMsg(FAIL_ENGINE|PARSE_STATMNT_LST|UNMTCH_END, 
                                          NULL, 0, parse))==FATAL_ENGINE)
            return(FATAL_ENGINE);
      }
   }
   else
   {  /****************************************************************/
      /* Allocate the first (only) statement in the list, if required */
      /****************************************************************/
      if(statementList)
      {  if((newStatement = (tStatement*)MyTmpMalloc(sizeof(tStatement)))==NULL)
            throw(MALLOC_EXCEPTION);
         memset((char*)newStatement, 0, sizeof(tStatement));
         newStatement->next = NULL;
         *statementList = newStatement;

         newStatement->traceOn = parse->traceOn;
         newStatement->level = parse->numBegins + 1;
      }

      /**********************************************************************/
      /* Set up the optional terminators.  If currently in an IF statement, */
      /* then allow an "ELSE".  If currently in a BEGIN/END block, then     */
      /* allow an "END".  If current in CASE statement, also allow an END   */ 
      /**********************************************************************/
      if(from==FROM_IF) 
      {  if(parse->numBegins) optionTerms="; ELSE END";
         else optionTerms = "; ELSE";
      }
      else
      {  if(parse->numBegins || from==FROM_CASE) optionTerms="; END";
         else optionTerms = ";";
      }

      /******************************/
      /* Parse the single statement */
      /******************************/
      if((endStatus=ParseStatement(parse, optionTerms, newStatement))==FATAL_ENGINE)
         return(FATAL_ENGINE);

      /********************/
      /* Check for RETURN */
      /********************/
      if(from!=FROM_OTHER) parse->numSubs++;
      if(endStatus==RETURN_PARSE || endStatus==TERM_PARSE) 
      {  if(from==FROM_OTHER) parse->mainReturn = 1;
         parse->numReturns++;
      }

      /***********************************************************/
      /* Put the "ELSE" token back for the "IF" block to pick up */
      /* Put the "END" token back for the statement list loop to */
      /* pick it up.                                             */   
      /***********************************************************/
      if(!strcmp(parse->token, "END") || 
         (!strcmp(parse->token, "ELSE") && from==FROM_IF))
            PutBackToken(parse);
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* ParseArrayDef()                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long ParseArrayDef(tParse *parse, tType *type)
{
   long  startIndex=0, endIndex=0, thisType=0, strLen=0, size=0, status=0;
   char  errToken[50];
   tType *arrayType=NULL;

   /* Get start bracket */
   while((status=LexRule(parse))==FAIL_ENGINE);
   if(status==END_OF_RULE)
   {  AddErrMsg(FATAL_ENGINE|PARSE_ARRAY|EARLY_TERM, NULL, 0, parse);
      return(FATAL_ENGINE);
   }
   else if(strcmp(parse->token, "[") && strcmp(parse->token, "{"))
   {  AddErrMsg(FATAL_ENGINE|PARSE_ARRAY|MISSING_LBRACKET, parse->token, 0, parse);
      return(FATAL_ENGINE);
   }

   /* Get start index */
   parse->inExpr = 1;
   while((status=LexRule(parse))==FAIL_ENGINE);
   parse->inExpr = 0;
   if(status==END_OF_RULE)
   {  AddErrMsg(FATAL_ENGINE|PARSE_ARRAY|EARLY_TERM, NULL, 0, parse);
      return(FATAL_ENGINE);
   }
   else if(parse->tokenType == T_INTEGER_CONSTANT)
      startIndex = NUM_GETLNG(parse->data.num);
   else
   {  sprintf(errToken, "%s$%s", parse->token, type->typeName);
      if(AddErrMsg(FAIL_ENGINE|PARSE_ARRAY|BAD_START_INDEX,
                                   errToken, 0, parse)==FATAL_ENGINE)
            return(FATAL_ENGINE);
   }

   /* Get .. */
   while((status=LexRule(parse))==FAIL_ENGINE);
   if(status==END_OF_RULE)
   {  AddErrMsg(FATAL_ENGINE|PARSE_ARRAY|EARLY_TERM, NULL, 0, parse);
      return(FATAL_ENGINE);
   }
   else if(strcmp(parse->token, ".."))
   {  AddErrMsg(FATAL_ENGINE|PARSE_ARRAY|MISSING_DOTDOT, type->typeName, 0, parse);
      return(FATAL_ENGINE);
   }

   /* Get ending index */
   parse->inExpr = 2;
   while((status=LexRule(parse))==FAIL_ENGINE);
   parse->inExpr = 0;
   if(status==END_OF_RULE)
   {  AddErrMsg(FATAL_ENGINE|PARSE_ARRAY|EARLY_TERM, NULL, 0, parse);
      return(FATAL_ENGINE);
   }
   else if(parse->tokenType == T_INTEGER_CONSTANT)
      endIndex = NUM_GETLNG(parse->data.num);
   else
   {  sprintf(errToken, "%s$%s", parse->token, type->typeName);
      if(AddErrMsg(FAIL_ENGINE|PARSE_ARRAY|BAD_END_INDEX,
                                    errToken, 0, parse)==FATAL_ENGINE)
            return(FATAL_ENGINE);
   }

   /* Make sure ending index is greater than or equal to starting */
   if(endIndex < startIndex)
   {  sprintf(errToken, "%ld$%ld$%s", startIndex, endIndex, type->typeName);
      AddErrMsg(FATAL_ENGINE|PARSE_ARRAY|BAD_RANGE, errToken, 0, parse);
      return(FATAL_ENGINE);
   }
   
   /* Get Ending bracket */
   while((status=LexRule(parse))==FAIL_ENGINE);
   if(status==END_OF_RULE)
   {  AddErrMsg(FATAL_ENGINE|PARSE_ARRAY|EARLY_TERM, NULL, 0, parse);
      return(FATAL_ENGINE);
   }
   else if(strcmp(parse->token, "]") && strcmp(parse->token, "}"))
   {  AddErrMsg(FATAL_ENGINE|PARSE_ARRAY|MISSING_RBRACKET, parse->token, 0, parse);
      return(FATAL_ENGINE);
   }
   
   /* Get 'OF' bracket */
   while((status=LexRule(parse))==FAIL_ENGINE);
   if(status==END_OF_RULE)
   {  AddErrMsg(FATAL_ENGINE|PARSE_ARRAY|EARLY_TERM, NULL, 0, parse);
      return(FATAL_ENGINE);
   }
   else if(strcmp(parse->token, "OF"))
   {  sprintf(errToken, "%s$%s", parse->token, type->typeName);
      AddErrMsg(FATAL_ENGINE|PARSE_ARRAY|MISSING_OF, errToken, 0, parse);
      return(FATAL_ENGINE);
   }
   
   /* Get element type */
   while((status=LexRule(parse))==FAIL_ENGINE);
   if(status==END_OF_RULE)
   {  AddErrMsg(FATAL_ENGINE|PARSE_ARRAY|EARLY_TERM, NULL, 0, parse);
      return(FATAL_ENGINE);
   }
   else if(!strcmp(parse->token, "NUMERIC"))
      thisType = NUMERIC_TYPE;
   else if(!strcmp(parse->token, "STRING"))
   {  thisType = HNC_STRING;
      
      /* Look for string length */
      while((status=LexRule(parse))==FAIL_ENGINE);
      if(status==END_OF_RULE)
      {  AddErrMsg(FATAL_ENGINE|PARSE_ARRAY|EARLY_TERM, NULL, 0, parse);
         return(FATAL_ENGINE);
      }
      else if(!strcmp(parse->token, "[") || !strcmp(parse->token, "{"))
      {  /* Get length */ 
         parse->inExpr = 1;
         while((status=LexRule(parse))==FAIL_ENGINE);
         parse->inExpr = 0;
         if(status==END_OF_RULE)
         {  AddErrMsg(FATAL_ENGINE|PARSE_ARRAY|EARLY_TERM, NULL, 0, parse);
            return(FATAL_ENGINE);
         }
         else if(parse->tokenType == T_INTEGER_CONSTANT)
            strLen = NUM_GETLNG(parse->data.num);
         else
         {  if(AddErrMsg(FAIL_ENGINE|PARSE_ARRAY|BAD_STRLEN, 
                                          parse->token, 0, parse)==FATAL_ENGINE)
                  return(FATAL_ENGINE);
         }

         /* Get Ending bracket */ 
         while((status=LexRule(parse))==FAIL_ENGINE);
         if(status==END_OF_RULE)
         {  AddErrMsg(FATAL_ENGINE|PARSE_ARRAY|EARLY_TERM, NULL, 0, parse);
            return(FATAL_ENGINE);
         }
         else if(strcmp(parse->token, "]") && strcmp(parse->token, "}"))
         {  AddErrMsg(FATAL_ENGINE|PARSE_ARRAY|STRLEN_NO_END, parse->token, 0, parse);
            return(FATAL_ENGINE);
         }
      }
      else
      {  PutBackToken(parse);
         strLen = 255;
      }
   }
   else if((arrayType=FindType(parse->token, parse)))
   {  if(arrayType->type == RECORD_TYPE) 
         thisType = RECORD_TYPE;
      else if(arrayType->type == ARRAY_TYPE)
      {  sprintf(errToken, "%s$%s", parse->token, type->typeName);
         if(AddErrMsg(FAIL_ENGINE|PARSE_ARRAY|ARRAY_OF_ARRAY, 
                                        errToken, 0, parse)==FATAL_ENGINE)
            return(FATAL_ENGINE);
      }
      else
      {  sprintf(errToken, "%s$%s", parse->token, type->typeName);
         if(AddErrMsg(FAIL_ENGINE|PARSE_ARRAY|BAD_ARRAY_TYPE, 
                                        errToken, 0, parse)==FATAL_ENGINE)
            return(FATAL_ENGINE);
      }
   }
   else
   {  sprintf(errToken, "%s$%s", parse->token, type->typeName);
      if(AddErrMsg(FAIL_ENGINE|PARSE_ARRAY|BAD_ARRAY_TYPE, 
                                     errToken, 0, parse)==FATAL_ENGINE)
         return(FATAL_ENGINE);
   }

   /* Set the type in the array structure */
   if(thisType==NUMERIC_TYPE)
   {  type->fmt.array.type = NUMERIC_TYPE;
      size = sizeof(tEngNum);
   }
   else if(thisType==HNC_STRING)
   {  if(strLen==1) type->fmt.array.type = HNC_CHAR;
      else type->fmt.array.type = HNC_STRING;
      size = strLen;
   }
   else if(thisType==RECORD_TYPE)
   {  type->fmt.array.type = RECORD_TYPE;
      type->fmt.array.typeFmt = &arrayType->fmt;
      size = arrayType->fmt.record.dataSize; 
   }
   else if(thisType==ARRAY_TYPE)
   {  type->fmt.array.type = ARRAY_TYPE;
      type->fmt.array.typeFmt = &arrayType->fmt;
      size = arrayType->fmt.array.dataSize; 
   }

   /* Set up the rest of the array structure */
   type->fmt.array.name = type->typeName;
   type->fmt.array.startIndex = startIndex;
   type->fmt.array.endIndex = endIndex;
   type->fmt.array.elemSize = size;
   type->fmt.array.numElems = (endIndex - startIndex) + 1;
   type->fmt.array.dataSize = (type->fmt.array.numElems) * size;

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* ParseRecordDef()                                                         */
/*                                                                          */
/*--------------------------------------------------------------------------*/
#define MAX_VARS  50
static long ParseRecordDef(tParse *parse, tType *type)
{
   tRecField *fld=NULL, *curFld=NULL, *strtFld=NULL;
   tType     *fldType=NULL;
   char      fldName[MAX_VARS][100], errToken[100];
   long      thisType=NUMERIC_TYPE, i, j, strLen=255, status, numFlds=0, offset=0;

   /***************************************************/
   /* Loop through the list of fields for this record */
   /* Should be list of <field_name> : <type>,        */
   /* terminated with END                             */  
   /***************************************************/
   while(1)
   {  /******************/
      /* Get field name */
      /******************/
      while((status=LexRule(parse))==FAIL_ENGINE);
      if(status==END_OF_RULE)
      {  AddErrMsg(FATAL_ENGINE|PARSE_RECORD|EARLY_TERM, NULL, 0, parse);
         return(FATAL_ENGINE);
      }
      else if(parse->tokenType == T_VARIABLE)
      {  i = 0;
         strcpy(fldName[i], parse->token);

         /************************/
         /* Check for ':' or ',' */
         /************************/
         while((status=LexRule(parse))==FAIL_ENGINE);
         if(status==END_OF_RULE)
         {  AddErrMsg(FATAL_ENGINE|PARSE_RECORD|EARLY_TERM, NULL, 0, parse);
            return(FATAL_ENGINE);
         }
         else if(!strcmp(parse->token, ","))
         {  /* Get a list of fields of the same type */
            while(i < MAX_VARS)
            {  /* Get next field name */
               while((status=LexRule(parse))==FAIL_ENGINE);
               if(status==END_OF_RULE)
               {  AddErrMsg(FATAL_ENGINE|PARSE_RECORD|EARLY_TERM, NULL, 0, parse);
                  return(FATAL_ENGINE);
               }
               else if(parse->tokenType == T_VARIABLE)
               {  i++;
                  strcpy(fldName[i], parse->token);
               }
               else
               {  if(AddErrMsg(FAIL_ENGINE|PARSE_RECORD|SYNTX_ERR,
                                            parse->token, 0, parse)==FATAL_ENGINE)
                     return(FATAL_ENGINE);
               }

               /* Get the ',' or the ':' */
               while((status=LexRule(parse))==FAIL_ENGINE);
               if(status==END_OF_RULE)
               {  AddErrMsg(FATAL_ENGINE|PARSE_RECORD|EARLY_TERM, NULL, 0, parse);
                  return(FATAL_ENGINE);
               }
               else if(!strcmp(parse->token, ":"))
                  break;
               else if(!strcmp(parse->token, ","))
                  continue;
               else
               {  if(AddErrMsg(FAIL_ENGINE|PARSE_RECORD|MISSING_COLON,
                                            fldName[0], 0, parse)==FATAL_ENGINE)
                     return(FATAL_ENGINE);

                  if(parse->tokenType == T_VARIABLE || parse->tokenType == T_KEYWORD)
                     PutBackToken(parse);
               }
            }
         }
         else if(strcmp(parse->token, ":"))
         {  if(AddErrMsg(FAIL_ENGINE|PARSE_RECORD|MISSING_COLON, 
                                     fldName[0], 0, parse)==FATAL_ENGINE)
               return(FATAL_ENGINE);
            PutBackToken(parse);
         }

         /**************************************************************/
         /* Loop through fields allocating space and checking for dups */
         /**************************************************************/
         for(j=0; j<=i; j++)
         {  /************************************************/
            /* Make sure field is not duplicate of previous */
            /************************************************/
            fld = type->fmt.record.fieldList;
            while(fld)
            {  if(!NoCaseStrcmp(fldName[j], fld->fieldName))
               {  sprintf(errToken, "%s$%s", type->typeName, fldName[j]);
                  if(AddErrMsg(FAIL_ENGINE|PARSE_RECORD|FLD_EXISTS, 
                                        errToken, 0, parse)==FATAL_ENGINE)
                     return(FATAL_ENGINE);
               }
   
               fld = fld->next;
            }
   
            /********************************************/
            /* Create new field element for this record */
            /********************************************/
            if(type->fmt.record.fieldList)
            {  if((curFld->next = (tRecField*)MyMalloc(sizeof(tRecField)))==NULL)
                  throw(MALLOC_EXCEPTION);
               curFld = curFld->next;
            }
            else
            {  if((curFld = (tRecField*)MyMalloc(sizeof(tRecField)))==NULL)
                  throw(MALLOC_EXCEPTION);
               type->fmt.record.fieldList = curFld;
            }
            memset((char*)curFld, 0, sizeof(tRecField));
            
            /* Set the name */
            if((curFld->fieldName = (char*)MyMalloc(strlen(fldName[j])+1))==NULL)
               throw(MALLOC_EXCEPTION);
            strcpy(curFld->fieldName, fldName[j]);
   
            numFlds++;
            if(j==0) strtFld = curFld;
         }
      }
      else if(!strcmp(parse->token, "END"))
      {  /* Set the rest of the record data */
         type->fmt.record.recName = type->typeName;
         if((offset%sizeof(tEngNum))!=0) offset += sizeof(tEngNum) - (offset%sizeof(tEngNum));
         type->fmt.record.dataSize = offset;
         type->fmt.record.numFlds = numFlds;
          
         /* Get final ';', if any.  Pascal requires one, but it isn't necessary */
         while((status=LexRule(parse))==FAIL_ENGINE);
         if(status==END_OF_RULE)
         {  AddErrMsg(FATAL_ENGINE|PARSE_RECORD|EARLY_TERM, NULL, 0, parse);
            return(FATAL_ENGINE);
         }
         else if(strcmp(parse->token, ";"))
            PutBackToken(parse); 

         return(OK);
      }
      else
      {  AddErrMsg(FATAL_ENGINE|PARSE_RECORD|BAD_REC_TERM, parse->token, 0, parse);
         return(FATAL_ENGINE);
      }

      /******************/
      /* Get field type */
      /******************/
      while((status=LexRule(parse))==FAIL_ENGINE);
      if(status==END_OF_RULE)
      {  AddErrMsg(FATAL_ENGINE|PARSE_RECORD|EARLY_TERM, NULL, 0, parse);
         return(FATAL_ENGINE);
      }
      else if(!strcmp(parse->token, "NUMERIC"))
         thisType = NUMERIC_TYPE;
      else if(!strcmp(parse->token, "STRING"))
      {  thisType = HNC_STRING;
         
         /* Look for string length */
         while((status=LexRule(parse))==FAIL_ENGINE);
         if(status==END_OF_RULE)
         {  AddErrMsg(FATAL_ENGINE|PARSE_RECORD|EARLY_TERM, NULL, 0, parse);
            return(FATAL_ENGINE);
         }
         else if(!strcmp(parse->token, "[") || !strcmp(parse->token, "{"))
         {  /* Get length */ 
            parse->inExpr = 1;
            while((status=LexRule(parse))==FAIL_ENGINE);
            parse->inExpr = 0;
            if(status==END_OF_RULE)
            {  AddErrMsg(FATAL_ENGINE|PARSE_RECORD|EARLY_TERM, NULL, 0, parse);
               return(FATAL_ENGINE);
            }
            else if(parse->tokenType == T_INTEGER_CONSTANT)
               strLen = NUM_GETLNG(parse->data.num);
            else
            {  if(AddErrMsg(FAIL_ENGINE|PARSE_RECORD|BAD_STRLEN, 
                                             parse->token, 0, parse)==FATAL_ENGINE)
                     return(FATAL_ENGINE);
            }
  
            /* Get Ending bracket */ 
            while((status=LexRule(parse))==FAIL_ENGINE);
            if(status==END_OF_RULE)
            {  AddErrMsg(FATAL_ENGINE|PARSE_RECORD|EARLY_TERM, NULL, 0, parse);
               return(FATAL_ENGINE);
            }
            else if(strcmp(parse->token, "]") && strcmp(parse->token, "}"))
            {  AddErrMsg(FATAL_ENGINE|PARSE_RECORD|STRLEN_NO_END, parse->token, 0, parse);
               return(FATAL_ENGINE);
            }
         }
         else
         {  PutBackToken(parse);
            strLen = 255;
         }
      }
      else if((fldType=FindType(parse->token, parse)))
      {  if(fldType->type == RECORD_TYPE) 
            thisType = RECORD_TYPE;
         else if(fldType->type == ARRAY_TYPE)
            thisType = ARRAY_TYPE;
         else
         {  sprintf(errToken, "%s$%s$%s", type->typeName, fldName[0], parse->token);
            if(AddErrMsg(FAIL_ENGINE|PARSE_RECORD|BAD_FLD_TYPE, 
                                           errToken, 0, parse)==FATAL_ENGINE)
               return(FATAL_ENGINE);
         }
      }
      else
      {  sprintf(errToken, "%s$%s$%s", type->typeName, fldName[0], parse->token);
         if(AddErrMsg(FAIL_ENGINE|PARSE_RECORD|BAD_FLD_TYPE, 
                                        errToken, 0, parse)==FATAL_ENGINE)
            return(FATAL_ENGINE);
      }

      /*********************************************************/
      /* Loop through list of "like" fields, setting the types */
      /*********************************************************/
      for(curFld = strtFld; curFld; curFld=curFld->next)
      {  if(thisType==NUMERIC_TYPE)
         {  curFld->type = NUMERIC_TYPE;
            if((offset%sizeof(tEngNum))!=0) 
               offset += sizeof(tEngNum) - (offset%sizeof(tEngNum));
            curFld->offset = offset;
            curFld->length = sizeof(tEngNum);
            offset += curFld->length;
         }
         else if(thisType==HNC_STRING)
         {  if(strLen==1) curFld->type = HNC_CHAR;
            else curFld->type = HNC_STRING;
            curFld->offset = offset;
            curFld->length = strLen;
            offset += curFld->length;
         }
         else if(thisType==RECORD_TYPE)
         {  curFld->type = RECORD_TYPE;
            curFld->typeFmt = &fldType->fmt;
            if((offset%sizeof(tEngNum))!=0) 
               offset += sizeof(tEngNum) - (offset%sizeof(tEngNum));
            curFld->offset = offset;
            curFld->length = fldType->fmt.record.dataSize;
            offset += curFld->length;
         }
         else if(thisType==ARRAY_TYPE)
         {  curFld->type = ARRAY_TYPE;
            curFld->typeFmt = &fldType->fmt;
            if((offset%sizeof(tEngNum))!=0) 
               offset += sizeof(tEngNum) - (offset%sizeof(tEngNum));
            curFld->offset = offset;
            curFld->length = fldType->fmt.array.dataSize;
            offset += curFld->length;
         }
      }

      /**********************************/
      /* Put curFld back at end of list */
      /**********************************/
      for(curFld = strtFld; curFld->next; curFld=curFld->next);

      /***********/
      /* Get ';' */
      /***********/
      while((status=LexRule(parse))==FAIL_ENGINE);
      if(status==END_OF_RULE)
      {  AddErrMsg(FATAL_ENGINE|PARSE_RECORD|EARLY_TERM, NULL, 0, parse);
         return(FATAL_ENGINE);
      }
      else if(strcmp(parse->token, ";"))
      {  if(AddErrMsg(FAIL_ENGINE|PARSE_RECORD|MISSING_SEMI_COLON,
                                           fldName[0], 0, parse)==FATAL_ENGINE)
            return(FATAL_ENGINE);
         PutBackToken(parse);
      }
   }
}


/*--------------------------------------------------------------------------*/
/* ParseLocalTypes()                                                        */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long ParseLocalTypes(tParse *parse)
{
   long     status, numTypes=0;
   tType    *tmpType, *curType=NULL;
   char     typeName[100];


#ifdef DEVELOP
   debug_out("Parsing local types block\n", "");
#endif

   /**************************************/
   /* Main loop to parse the TYPE block */
   /**************************************/
   while(1)
   {  /*********************/
      /* Get variable name */
      /*********************/
      while((status=LexRule(parse))==FAIL_ENGINE);
      if(status==END_OF_RULE)
      {  AddErrMsg(FATAL_ENGINE|PARSE_LOCTYPE|EARLY_TERM, NULL, 0, parse);
         return(FATAL_ENGINE);
      }
      else if(parse->tokenType == T_VARIABLE)
      {  /******************************************/
         /* Could be new type definition, or could */
         /* be a variable set.                     */
         /******************************************/
         strcpy(typeName, parse->token);

         while((status=LexRule(parse))==FAIL_ENGINE);
         if(status==END_OF_RULE)
         {  AddErrMsg(FATAL_ENGINE|PARSE_LOCTYPE|EARLY_TERM, NULL, 0, parse);
            return(FATAL_ENGINE);
         }
         else if(!strcmp(parse->token, ":="))
         {  /*******************************************************/
            /* Check if the next token is "RECORD" or "ARRAY".  If */
            /* so, then intent was to use '=' instead of ':='.     */
            /* Otherwise, assume out of type block and into rule   */
            /*******************************************************/
            while((status=LexRule(parse))==FAIL_ENGINE);
            if(status==END_OF_RULE)
            {  AddErrMsg(FATAL_ENGINE|PARSE_LOCTYPE|EARLY_TERM, NULL, 0, parse);
               return(FATAL_ENGINE);
            }
            else if(!strcmp(parse->token, "RECORD") || !strcmp(parse->token, "ARRAY"))
            {  if(AddErrMsg(FAIL_ENGINE|PARSE_LOCTYPE|TYPE_DEF_USE_EQUAL,
                                         parse->token, 0, parse)==FATAL_ENGINE)
                  return(FATAL_ENGINE);
               PutBackToken(parse);
            }
            else
            {  PutBackToken(parse);
               break;
            }
         }
         else if(strcmp(parse->token, "="))
         {  if(AddErrMsg(FAIL_ENGINE|PARSE_LOCTYPE|MISSING_SET, 
                                      parse->token, 0, parse)==FATAL_ENGINE)
               return(FATAL_ENGINE);

            if(parse->tokenType == T_VARIABLE || parse->tokenType == T_KEYWORD) 
               PutBackToken(parse);
         }

         /*****************************************/
         /* Make sure type is not already defined */
         /*****************************************/
         tmpType = parse->localTypeList;
         while(tmpType)
         {  if(!NoCaseStrcmp(typeName, tmpType->typeName))
            {  if(AddErrMsg(FAIL_ENGINE|PARSE_LOCTYPE|TYPE_EXISTS_LOCTYPE, 
                                     typeName, 0, parse)==FATAL_ENGINE)
                  return(FATAL_ENGINE);
            }
   
            tmpType = tmpType->next;
         }

         /*******************************************/
         /* Generate warning if type has same name  */
         /* as a global.  It is OK for this happen  */
         /* because of the different scopes, but it */
         /* may be an unwanted error.               */
         /*******************************************/
         tmpType = parse->globalTypeList;
         while(tmpType)
         {  if(!NoCaseStrcmp(typeName, tmpType->typeName))
            {  if(AddErrMsg(WARN_ENGINE|PARSE_LOCTYPE|TYPE_EXISTS_GLOBTYPE, 
                                        typeName, 0, parse)==FATAL_ENGINE)
                  return(FATAL_ENGINE);
            }
   
            tmpType = tmpType->next;
         }

         /******************************************/
         /* Create new tType element for this type */
         /******************************************/
         if((curType = (tType*)MyMalloc(sizeof(tType)))==NULL)
            throw(MALLOC_EXCEPTION);
         memset((char*)curType, 0, sizeof(tType));
         AddTypeToList(&parse->localTypeList, curType);

         /* Set the name */
         if((curType->typeName = (char*)MyMalloc(strlen(typeName)+1))==NULL)
            throw(MALLOC_EXCEPTION);
         strcpy(curType->typeName, typeName);
         
         /****************************************/
         /* Add the variable to the allData list */
         /****************************************/
         AddTypeToAllData(parse->allData, curType);
         numTypes++;
      }
      else if(parse->tokenType == T_KEYWORD)
      {  if(!strcmp(parse->token, "IF") || !strcmp(parse->token, "BEGIN") ||
            !strcmp(parse->token, "TERMINATE") || !strcmp(parse->token, "RETURN") ||
            !strcmp(parse->token, "VAR"))
         {  PutBackToken(parse);
            break;
         }
         else
         {  AddErrMsg(FATAL_ENGINE|PARSE_LOCTYPE|BAD_VARDEF_TOK, parse->token, 0, parse);
            return(FATAL_ENGINE);
         }
      }
      else if(parse->tokenType == T_FUNCTION ||
                parse->tokenType == T_RULE ||
                parse->tokenType == T_RECFLD)
      {  PutBackToken(parse);
         break;
      }
      else 
      {  AddErrMsg(FATAL_ENGINE|PARSE_LOCTYPE|BAD_VARDEF_TOK, parse->token, 0, parse);
         return(FATAL_ENGINE);
      }

      /***********************/
      /* Get type definition */
      /***********************/
      while((status=LexRule(parse))==FAIL_ENGINE);
      if(status==END_OF_RULE)
      {  AddErrMsg(FATAL_ENGINE|PARSE_LOCTYPE|EARLY_TERM, NULL, 0, parse);
         return(FATAL_ENGINE);
      }
      else if(!strcmp(parse->token, "RECORD"))
      {  curType->type = RECORD_TYPE;
         if(ParseRecordDef(parse, curType)==FATAL_ENGINE)
            return(FATAL_ENGINE);
      }
      else if(!strcmp(parse->token, "ARRAY"))
      {  curType->type = ARRAY_TYPE;
         if(ParseArrayDef(parse, curType)==FATAL_ENGINE)
            return(FATAL_ENGINE);
      }
      else 
      {  AddErrMsg(FATAL_ENGINE|PARSE_LOCTYPE|BAD_TYPE, parse->token, 0, parse);
         return(FATAL_ENGINE);
      }


      /*******************/
      /* Get ';', if any */
      /*******************/
      while((status=LexRule(parse))==FAIL_ENGINE);
      if(status==END_OF_RULE)
      {  AddErrMsg(FATAL_ENGINE|PARSE_LOCTYPE|EARLY_TERM, NULL, 0, parse);
         return(FATAL_ENGINE);
      }
      else if(strcmp(parse->token, ";"))
         PutBackToken(parse);
   }
   
   /***********************************************/
   /* Make sure at least one variable was defined */
   /***********************************************/
   if(numTypes==0)
   {  if(AddErrMsg(FAIL_ENGINE|PARSE_LOCTYPE|EMPTY_BLOCK, 
                            parse->token, 0, parse)==FATAL_ENGINE)
         return(FATAL_ENGINE);
   }
   
   return(OK);
}


/*--------------------------------------------------------------------------*/
/* ParseLocalConstants()                                                    */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long ParseLocalConstants(tParse *parse)
{
   long     status, numConst=0, found=0;
   tEngVar  *tmpVar, *curVar=NULL;
   char     errToken[100], varName[100];


#ifdef DEVELOP
   debug_out("Parsing local constants block\n", "");
#endif

   /**************************************/
   /* Main loop to parse the CONST block */
   /**************************************/
   while(1)
   {  /*********************/
      /* Get variable name */
      /*********************/
      while((status=LexRule(parse))==FAIL_ENGINE);
      if(status==END_OF_RULE)
      {  AddErrMsg(FATAL_ENGINE|PARSE_LOCCONST|EARLY_TERM, NULL, 0, parse);
         return(FATAL_ENGINE);
      }
      else if(parse->tokenType == T_VARIABLE)
      {  /***********************************************/
         /* Could be new constant definition, or could  */
         /* be a variable set.  Constant defintions use */
         /* '=' instead of ':='                         */
         /***********************************************/
         strcpy(varName, parse->token);

         while((status=LexRule(parse))==FAIL_ENGINE);
         if(status==END_OF_RULE)
         {  AddErrMsg(FATAL_ENGINE|PARSE_LOCCONST|EARLY_TERM, NULL, 0, parse);
            return(FATAL_ENGINE);
         }
         else if(!strcmp(parse->token, ":="))
         {  /*******************************************************/
            /* Check if varName exists.  If so, then this is a set */ 
            /* statement for the rule.  If not, then assume that   */
            /* this is a new constant definition, and the intent   */
            /* was to use a '=' instead of ':='                    */
            /*******************************************************/
            for(tmpVar = parse->paramVarList; tmpVar; tmpVar = tmpVar->next)
            {  if(!NoCaseStrcmp(varName, tmpVar->varName))
               {  found = 1;
                  break;
               }
            }
            for(tmpVar = parse->localVarList; tmpVar && !found; tmpVar = tmpVar->next)
            {  if(!NoCaseStrcmp(varName, tmpVar->varName))
               {  found = 1;
                  break;
               }
            }
            for(tmpVar = parse->globalVarList; tmpVar && !found; tmpVar = tmpVar->next)
            {  if(!NoCaseStrcmp(varName, tmpVar->varName))
               {  found = 1;
                  break;
               }
            }

            if(found)
            {  PutBackToken(parse);
               break;
            }
            else
            {  if(AddErrMsg(FAIL_ENGINE|PARSE_LOCCONST|CONST_DEF_USE_EQUAL,
                                         parse->token, 0, parse)==FATAL_ENGINE)
                  return(FATAL_ENGINE);
            }
         }
         else if(strcmp(parse->token, "="))
         {  if(AddErrMsg(FAIL_ENGINE|PARSE_LOCCONST|MISSING_SET, 
                                      parse->token, 0, parse)==FATAL_ENGINE)
               return(FATAL_ENGINE);

            if(parse->tokenType == T_VARIABLE || parse->tokenType == T_KEYWORD) 
               PutBackToken(parse);
         }

         /**********************************************/
         /* Make sure constant is not a parameter name */
         /**********************************************/
         tmpVar = parse->paramVarList;
         while(tmpVar)
         {  if(!NoCaseStrcmp(varName, tmpVar->varName))
            {  if(AddErrMsg(FAIL_ENGINE|PARSE_LOCCONST|CONST_EXISTS_PARAM, 
                                     varName, 0, parse)==FATAL_ENGINE)
                  return(FATAL_ENGINE);
            }
   
            tmpVar = tmpVar->next;
         }

         /*******************************************************/
         /* Make sure constant is not a already defined locally */
         /*******************************************************/
         tmpVar = parse->localVarList;
         while(tmpVar)
         {  if(!NoCaseStrcmp(varName, tmpVar->varName))
            {  if(tmpVar->flags&VAR_CONSTANT)
               {  if(AddErrMsg(FAIL_ENGINE|PARSE_LOCCONST|CONST_EXISTS_LOCCONST, 
                                        varName, 0, parse)==FATAL_ENGINE)
                     return(FATAL_ENGINE);
               }
               else
               {  if(AddErrMsg(FAIL_ENGINE|PARSE_LOCCONST|CONST_EXISTS_LOCVAR, 
                                        varName, 0, parse)==FATAL_ENGINE)
                     return(FATAL_ENGINE);
               }
            }
   
            tmpVar = tmpVar->next;
         }

         /**********************************************/
         /* Generate warning if constant has same name */
         /* as a global.  It is OK for this happen     */
         /* because of the different scopes, but it    */
         /* may be an unwanted error.                  */
         /**********************************************/
         tmpVar = parse->globalVarList;
         while(tmpVar)
         {  if(!NoCaseStrcmp(varName, tmpVar->varName))
            {  if(tmpVar->flags&VAR_CONSTANT)
               {  if(AddErrMsg(WARN_ENGINE|PARSE_LOCCONST|CONST_EXISTS_GLOBCONST, 
                                           varName, 0, parse)==FATAL_ENGINE)
                     return(FATAL_ENGINE);
               }
               else
               {  if(AddErrMsg(WARN_ENGINE|PARSE_LOCCONST|CONST_EXISTS_GLOBVAR, 
                                           varName, 0, parse)==FATAL_ENGINE)
                     return(FATAL_ENGINE);
               }
            }
   
            tmpVar = tmpVar->next;
         }

         /***********************************************/
         /* Create new var element for this variable */
         /***********************************************/
         if((curVar = (tEngVar*)MyMalloc(sizeof(tEngVar)))==NULL)
            throw(MALLOC_EXCEPTION);
         memset((char*)curVar, 0, sizeof(tEngVar));
         curVar->flags |= VAR_CONSTANT;
         AddVarToList(&parse->localVarList, curVar);

         /* Set the name */
         if((curVar->varName = (char*)MyMalloc(strlen(varName)+1))==NULL)
            throw(MALLOC_EXCEPTION);
         strcpy(curVar->varName, varName);
         
         /****************************************/
         /* Add the variable to the allData list */
         /****************************************/
         AddVarToAllData(parse->allData, curVar);
         numConst++;
      }
      else if(parse->tokenType == T_KEYWORD)
      {  if(!strcmp(parse->token, "IF") || !strcmp(parse->token, "BEGIN") ||
            !strcmp(parse->token, "TERMINATE") || !strcmp(parse->token, "RETURN") ||
            !strcmp(parse->token, "VAR") || !strcmp(parse->token, "TYPE"))
         {  PutBackToken(parse);
            break;
         }
         else
         {  AddErrMsg(FATAL_ENGINE|PARSE_LOCCONST|BAD_VARDEF_TOK, parse->token, 0, parse);
            return(FATAL_ENGINE);
         }
      }
      else if(parse->tokenType == T_FUNCTION ||
                parse->tokenType == T_RULE ||
                parse->tokenType == T_RECFLD)
      {  PutBackToken(parse);
         break;
      }
      else 
      {  AddErrMsg(FATAL_ENGINE|PARSE_LOCCONST|BAD_VARDEF_TOK, parse->token, 0, parse);
         return(FATAL_ENGINE);
      }

      /**********************/
      /* Get constant value */
      /**********************/
      while((status=LexRule(parse))==FAIL_ENGINE);
      if(status==END_OF_RULE)
      {  AddErrMsg(FATAL_ENGINE|PARSE_LOCCONST|EARLY_TERM, NULL, 0, parse);
         return(FATAL_ENGINE);
      }
      else if(parse->tokenType == T_INTEGER_CONSTANT || 
              parse->tokenType == T_FLOAT_CONSTANT)
      {  curVar->data = (void*)MyMalloc(sizeof(tEngNum));
         NUM_SET((*((tEngNum*)curVar->data)), parse->data.num) 
         curVar->dataSize = sizeof(tEngNum);
         curVar->type = NUMERIC_TYPE;
      }
      else if(parse->tokenType == T_STRING_CONSTANT)
      {  if((curVar->data = (void*)MyMalloc(strlen(parse->token)+1))==NULL)
            throw(MALLOC_EXCEPTION);
         strcpy((char*)curVar->data, parse->token);
         curVar->dataSize = strlen(parse->token)+1;
         curVar->type = HNC_STRING;
      }
      else if(parse->tokenType == T_CHAR_CONSTANT)
      {  if((curVar->data = (void*)MyMalloc(strlen(parse->token)+1))==NULL)
            throw(MALLOC_EXCEPTION);
         strcpy((char*)curVar->data, parse->token);
         curVar->dataSize = strlen(parse->token)+1;
         curVar->type = HNC_CHAR;
      }
      else
      {  sprintf(errToken, "%s$%s", curVar->varName, parse->token);
         curVar->type = NUMERIC_TYPE;
         if(AddErrMsg(FAIL_ENGINE|PARSE_LOCCONST|BAD_CONST_VALUE, 
                                           errToken, 0, parse)==FATAL_ENGINE)
            return(FATAL_ENGINE);
      }

      /***********/
      /* Get ';' */
      /***********/
      while((status=LexRule(parse))==FAIL_ENGINE);
      if(status==END_OF_RULE)
      {  AddErrMsg(FATAL_ENGINE|PARSE_LOCCONST|EARLY_TERM, NULL, 0, parse);
         return(FATAL_ENGINE);
      }
      else if(strcmp(parse->token, ";"))
      {  if(AddErrMsg(FAIL_ENGINE|PARSE_LOCCONST|MISSING_SEMI_COLON, 
                                curVar->varName, 0, parse)==FATAL_ENGINE)
            return(FATAL_ENGINE);
         ReadTillExprEnd(parse, "; TYPE VAR IF BEGIN TERMINATE RETURN");
      }
   }
   
   /***********************************************/
   /* Make sure at least one variable was defined */
   /***********************************************/
   if(numConst==0)
   {  if(AddErrMsg(FAIL_ENGINE|PARSE_LOCCONST|EMPTY_BLOCK, 
                            parse->token, 0, parse)==FATAL_ENGINE)
         return(FATAL_ENGINE);
   }
   
   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CreateArrayData()                                                        */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long CreateArrayData(tEngArray *array, tArrayData **pArrayData, char *data)
{
   tArrayData *arrayData;
   tRecData   *subRec;
   long       i;
   char       *ptr;

   /* Allocate a tArrayData structure */
   if((arrayData = (tArrayData*)MyMalloc(sizeof(tArrayData)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)arrayData, 0, sizeof(tArrayData));

   /* Allocate the element data pointers */
   if((arrayData->data = (char**)MyMalloc(array->numElems*sizeof(char*)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)arrayData->data, 0, sizeof(array->numElems*sizeof(char*)));

   /***********************************/
   /* Need to set up element pointers */
   /***********************************/
   ptr = data;
   for(i=0; i<array->numElems; i++)
   {  if(array->type == RECORD_TYPE) 
      {  CreateRecordData(&array->typeFmt->record, &subRec, ptr, 1); 
         arrayData->data[i] = (char*)subRec;
      }
      else
         arrayData->data[i] = ptr;

      ptr += array->elemSize; 
   }

   /* Set the array structure */   
   arrayData->array = array;
   arrayData->endIndex = array->endIndex;
   *pArrayData = arrayData;

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CreateRecordData()                                                       */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long CreateRecordData(tEngRecord *record, tRecData **pRecData, char *data, short inArray)
{
   tArrayData *subArray;
   tRecData   *recData, *subRec;
   tRecField  *fld;
   long       fldFixed, numFlds=0, f;
   char       *ptr;

   /* Allocate a record data structure */
   if((recData = (tRecData*)MyMalloc(sizeof(tRecData)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)recData, 0, sizeof(tRecData));
   *pRecData = recData;
   recData->record = record;

   /* If a data area is passed in, set it in the structure */
   /* Otherwise, assume it will be set at run-time         */
   if((recData->data = (char**)MyMalloc(sizeof(char*)))==NULL) 
      throw(MALLOC_EXCEPTION);
   if(data) *recData->data = data;
   else *recData->data = NULL;

   /* Get the number of fields and allocate the tFldData array */
   for(numFlds=0, fld=record->fieldList; fld; numFlds++, fld=fld->next); 
   if((recData->flds = (tFldData*)MyCalloc(sizeof(tFldData), numFlds))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)recData->flds, 0, sizeof(tFldData)*numFlds);

   /* Go through the fields and set the data for each field */
   for(f=0, fld=record->fieldList; fld; f++, fld=fld->next)
   {  recData->flds[f].fld = fld;
      fldFixed=1;
      if(fld->nonFixed)
      {  if(fld->nonFixed->skip)
            continue;  

         if(fld->nonFixed->fldID || fld->nonFixed->lengthInd)
            fldFixed=0;
      }

      if(fld->type == ARRAY_TYPE)
      {  if(data && fldFixed)
         {  ptr = *recData->data + fld->offset;
            CreateArrayData(&fld->typeFmt->array, &subArray, ptr);
            recData->flds[f].data = (char*)subArray;
            recData->flds[f].origData = 1;
         }
         else if(!inArray)
         {  /* Allocate an array data structure, but don't fill the elements */
            if((subArray = (tArrayData*)MyMalloc(sizeof(tArrayData)))==NULL)
               throw(MALLOC_EXCEPTION);
            memset((char*)subArray, 0, sizeof(tArrayData));
            subArray->array = &fld->typeFmt->array;
            if(fldFixed) 
            {  subArray->endIndex = subArray->array->endIndex;
               if((subArray->data = 
                     (char**)MyTmpMalloc(subArray->array->numElems*sizeof(char*)))==NULL)
                  throw(MALLOC_EXCEPTION);
               memset((char*)subArray->data, 0, 
                                   sizeof(subArray->array->numElems*sizeof(char*)));
            }

            recData->flds[f].data = (char*)subArray;
            recData->flds[f].origData = 2;
         }
         else
            recData->flds[f].data = NULL;
      }
      else if(fld->type == RECORD_TYPE)
      {  if(data && fldFixed)
         {  ptr = *recData->data + fld->offset;
            CreateRecordData(&fld->typeFmt->record, &subRec, ptr, inArray);
            recData->flds[f].origData = 1;
         }
         else
         {  CreateRecordData(&fld->typeFmt->record, &subRec, NULL, inArray);
            recData->flds[f].origData = 1;
         }

         recData->flds[f].data = (char*)subRec;
      }
      else  
      {  tFlatData *flatData;

         /* Allocate a flat data structure */
         if((flatData=(tFlatData*)MyMalloc(sizeof(tFlatData)))==NULL)
            throw(MALLOC_EXCEPTION);
         memset((char*)flatData, 0, sizeof(tFlatData));
         recData->flds[f].data = (char*)flatData;
         recData->flds[f].origData = 1;

         /* Set the data, if possible */
         if(data && fldFixed)
         {  flatData->data = (*recData->data+fld->offset);
            flatData->length = fld->length;
         }
      }
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* ParseLocalVariables()                                                    */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long ParseLocalVariables(tParse *parse)
{
   long     strLen=255, status, numVars=0, i, j, type=NUMERIC_TYPE;
   tEngVar  *tmpVar=NULL, *curVar=NULL, *strtVar=NULL;
   tType    *varType=NULL;
   char     errToken[100], varName[MAX_VARS][100];


#ifdef DEVELOP
   debug_out("Parsing local variable block\n", "");
#endif

   /************************************/
   /* Main loop to parse the VAR block */
   /************************************/
   while(1)
   {  /*********************/
      /* Get variable name */
      /*********************/
      while((status=LexRule(parse))==FAIL_ENGINE);
      if(status==END_OF_RULE)
      {  AddErrMsg(FATAL_ENGINE|PARSE_LOCVARS|EARLY_TERM, NULL, 0, parse);
         return(FATAL_ENGINE);
      }
      else if(parse->tokenType == T_VARIABLE)
      {  /**********************************************/
         /* Check if next statement is a variable set, */
         /* in which case, the var block is over, or a */
         /* ':' character.                             */
         /**********************************************/
         i = 0;
         strcpy(varName[i], parse->token);

         while((status=LexRule(parse))==FAIL_ENGINE);
         if(status==END_OF_RULE)
         {  AddErrMsg(FATAL_ENGINE|PARSE_LOCVARS|EARLY_TERM, NULL, 0, parse);
            return(FATAL_ENGINE);
         }
         else if(!strcmp(parse->token, ":="))
         {  PutBackToken(parse);
            break;
         }
         else if(!strcmp(parse->token, ","))
         {  /* Get a list of variables of the same type */
            while(i < MAX_VARS)
            {  /* Get next variable name */
               while((status=LexRule(parse))==FAIL_ENGINE);
               if(status==END_OF_RULE)
               {  AddErrMsg(FATAL_ENGINE|PARSE_LOCVARS|EARLY_TERM, NULL, 0, parse);
                  return(FATAL_ENGINE);
               }
               else if(parse->tokenType == T_VARIABLE)
               {  i++;
                  strcpy(varName[i], parse->token);
               }
               else 
               {  if(AddErrMsg(FAIL_ENGINE|PARSE_LOCVARS|SYNTX_ERR, 
                                            parse->token, 0, parse)==FATAL_ENGINE)
                     return(FATAL_ENGINE);
               }

               /* Get the ',' or the ':' */
               while((status=LexRule(parse))==FAIL_ENGINE);
               if(status==END_OF_RULE)
               {  AddErrMsg(FATAL_ENGINE|PARSE_LOCVARS|EARLY_TERM, NULL, 0, parse);
                  return(FATAL_ENGINE);
               }
               else if(!strcmp(parse->token, ":"))
                  break;
               else if(!strcmp(parse->token, ","))
                  continue;
               else
               {  if(AddErrMsg(FAIL_ENGINE|PARSE_LOCVARS|MISSING_COLON, 
                                            varName[0], 0, parse)==FATAL_ENGINE)
                     return(FATAL_ENGINE);

                  if(parse->tokenType == T_VARIABLE || parse->tokenType == T_KEYWORD) 
                     PutBackToken(parse);
               }
            }
         }
         else if(strcmp(parse->token, ":"))
         {  if(AddErrMsg(FAIL_ENGINE|PARSE_LOCVARS|MISSING_COLON, 
                                      varName[0], 0, parse)==FATAL_ENGINE)
               return(FATAL_ENGINE);

            if(parse->tokenType == T_VARIABLE || parse->tokenType == T_KEYWORD) 
               PutBackToken(parse);
         }

         /********************************************/
         /* Loop through all variables for this type */
         /* to check and allocate space.             */
         /********************************************/
         for(j=0; j<=i; j++)
         {  /*****************************************/
            /* Make sure variable is not a parameter */
            /*****************************************/
            tmpVar = parse->paramVarList;
            while(tmpVar)
            {  if(!NoCaseStrcmp(varName[j], tmpVar->varName))
               {  if(AddErrMsg(FAIL_ENGINE|PARSE_LOCVARS|VAR_EXISTS_PARAM, 
                                        varName[j], 0, parse)==FATAL_ENGINE)
                     return(FATAL_ENGINE);
               }
      
               tmpVar = tmpVar->next;
            }

            /*******************************************************/
            /* Make sure variable is not a already defined locally */
            /*******************************************************/
            tmpVar = parse->localVarList;
            while(tmpVar)
            {  if(!NoCaseStrcmp(varName[j], tmpVar->varName))
               {  if(tmpVar->flags&VAR_CONSTANT)
                  {  if(AddErrMsg(FAIL_ENGINE|PARSE_LOCVARS|VAR_EXISTS_LOCCONST, 
                                           varName[j], 0, parse)==FATAL_ENGINE)
                        return(FATAL_ENGINE);
                  }
                  else
                  {  if(AddErrMsg(FAIL_ENGINE|PARSE_LOCVARS|VAR_EXISTS_LOCVAR, 
                                           varName[j], 0, parse)==FATAL_ENGINE)
                     return(FATAL_ENGINE);
                  }
               }
   
               tmpVar = tmpVar->next;
            }

            /**********************************************/
            /* Generate warning if variable has same name */
            /* as a global variable.  It is OK for this   */
            /* happen because of the different scopes,    */
            /* but it may be an unwanted error.           */
            /**********************************************/
            tmpVar = parse->globalVarList;
            while(tmpVar)
            {  if(!NoCaseStrcmp(varName[j], tmpVar->varName))
               {  if(tmpVar->flags&VAR_CONSTANT)
                  {  if(AddErrMsg(WARN_ENGINE|PARSE_LOCVARS|VAR_EXISTS_GLOBCONST, 
                                              varName[j], 0, parse)==FATAL_ENGINE)
                        return(FATAL_ENGINE);
                  }
                  else
                  {  if(AddErrMsg(WARN_ENGINE|PARSE_LOCVARS|VAR_EXISTS_GLOBVAR, 
                                              varName[j], 0, parse)==FATAL_ENGINE)
                        return(FATAL_ENGINE);
                  }
               }
   
               tmpVar = tmpVar->next;
            }
   
            /********************************************/
            /* Create new var element for this variable */
            /********************************************/
            if((curVar = (tEngVar*)MyMalloc(sizeof(tEngVar)))==NULL)
            throw(MALLOC_EXCEPTION);
            memset((char*)curVar, 0, sizeof(tEngVar));
            AddVarToList(&parse->localVarList, curVar);
   
            if((curVar->varName = (char*)MyMalloc(strlen(varName[j])+1))==NULL)
               throw(MALLOC_EXCEPTION);
            strcpy(curVar->varName, varName[j]);

            /****************************************/
            /* Add the variable to the allData list */
            /****************************************/
            AddVarToAllData(parse->allData, curVar);
            numVars++;

            if(j==0) strtVar = curVar;
         }
      }
      else if(parse->tokenType == T_KEYWORD)
      {  if(!strcmp(parse->token, "IF") || !strcmp(parse->token, "BEGIN") ||
            !strcmp(parse->token, "TERMINATE") || !strcmp(parse->token, "RETURN"))
         {  PutBackToken(parse);
            break;
         }
         else
         {  AddErrMsg(FATAL_ENGINE|PARSE_LOCVARS|BAD_VARDEF_TOK, parse->token, 0, parse);
            return(FATAL_ENGINE);
         }
      }
      else if(parse->tokenType == T_FUNCTION ||
                parse->tokenType == T_RULE ||
                parse->tokenType == T_RECFLD)
      {  PutBackToken(parse);
         break;
      }
      else 
      {  AddErrMsg(FATAL_ENGINE|PARSE_LOCVARS|BAD_VARDEF_TOK, parse->token, 0, parse);
         return(FATAL_ENGINE);
      }

      /*********************/
      /* Get variable type */
      /*********************/
      while((status=LexRule(parse))==FAIL_ENGINE);
      if(status==END_OF_RULE)
      {  AddErrMsg(FATAL_ENGINE|PARSE_LOCVARS|EARLY_TERM, NULL, 0, parse);
         return(FATAL_ENGINE);
      }
      else if(!strcmp(parse->token, "NUMERIC"))
         type = NUMERIC_TYPE;
      else if(!strcmp(parse->token, "STRING"))
      {  type = HNC_STRING;
         
         /* Look for string length */
         while((status=LexRule(parse))==FAIL_ENGINE);
         if(status==END_OF_RULE)
         {  AddErrMsg(FATAL_ENGINE|PARSE_LOCVARS|EARLY_TERM, NULL, 0, parse);
            return(FATAL_ENGINE);
         }
         else if(!strcmp(parse->token, "[") || !strcmp(parse->token, "{"))
         {  /* Get length */ 
            parse->inExpr = 1;
            while((status=LexRule(parse))==FAIL_ENGINE);
            parse->inExpr = 0;
            if(status==END_OF_RULE)
            {  AddErrMsg(FATAL_ENGINE|PARSE_LOCVARS|EARLY_TERM, NULL, 0, parse);
               return(FATAL_ENGINE);
            }
            else if(parse->tokenType == T_INTEGER_CONSTANT)
               strLen = NUM_GETLNG(parse->data.num);
            else
            {  if(AddErrMsg(FAIL_ENGINE|PARSE_LOCVARS|BAD_STRLEN, 
                                             parse->token, 0, parse)==FATAL_ENGINE)
                     return(FATAL_ENGINE);
            }
  
            /* Get Ending bracket */ 
            while((status=LexRule(parse))==FAIL_ENGINE);
            if(status==END_OF_RULE)
            {  AddErrMsg(FATAL_ENGINE|PARSE_LOCVARS|EARLY_TERM, NULL, 0, parse);
               return(FATAL_ENGINE);
            }
            else if(strcmp(parse->token, "]") && strcmp(parse->token, "}"))
            {  AddErrMsg(FATAL_ENGINE|PARSE_LOCVARS|STRLEN_NO_END, parse->token, 0, parse);
               return(FATAL_ENGINE);
            }
         }
         else
         {  PutBackToken(parse);
            strLen = 255;
         }
      }
      else if((varType=FindType(parse->token, parse)))
      {  if(varType->type == RECORD_TYPE) 
            type = RECORD_TYPE;
         else if(varType->type == ARRAY_TYPE)
            type = ARRAY_TYPE;
         else
         {  sprintf(errToken, "%s$%s", varName[0], parse->token);
            if(AddErrMsg(FAIL_ENGINE|PARSE_LOCVARS|BAD_VAR_TYPE, 
                                           errToken, 0, parse)==FATAL_ENGINE)
               return(FATAL_ENGINE);
         }
      }
      else
      {  sprintf(errToken, "%s$%s", varName[0], parse->token);
         AddErrMsg(FATAL_ENGINE|PARSE_LOCVARS|BAD_VAR_TYPE, errToken, 0, parse);
         return(FATAL_ENGINE);
      }

      /************************************************************/
      /* Loop through list of "like" variables, setting the types */
      /************************************************************/
      for(curVar = strtVar; curVar; curVar=curVar->next)
      {  if(type==NUMERIC_TYPE)
         {  if((curVar->data = (void*)MyMalloc(sizeof(tEngNum)))==NULL)
               throw(MALLOC_EXCEPTION);
            memset((char*)curVar->data, 0, sizeof(tEngNum));
            curVar->dataSize = sizeof(tEngNum);
            curVar->type = NUMERIC_TYPE;
         }
         else if(type==HNC_STRING)
         {  if((curVar->data = (void*)MyMalloc(strLen))==NULL)
               throw(MALLOC_EXCEPTION);
            memset((char*)curVar->data, 0, strLen);
            curVar->dataSize = strLen;
            if(strLen==1) curVar->type = HNC_CHAR;
            else curVar->type = HNC_STRING;
         }
         else 
         {  if((status=MakeTypeInstance(curVar, varType)))
            {  sprintf(errToken, "%s$%s", varName[0], parse->token);
               AddErrMsg(FATAL_ENGINE|PARSE_LOCVARS|BAD_VAR_TYPE, errToken, 0, parse);
               return(FATAL_ENGINE);
            }
         }
      }


      /***********/
      /* Get ';' */
      /***********/
      while((status=LexRule(parse))==FAIL_ENGINE);
      if(status==END_OF_RULE)
      {  AddErrMsg(FATAL_ENGINE|PARSE_LOCVARS|EARLY_TERM, NULL, 0, parse);
         return(FATAL_ENGINE);
      }
      else if(strcmp(parse->token, ";"))
      {  if(AddErrMsg(FAIL_ENGINE|PARSE_LOCVARS|MISSING_SEMI_COLON, 
                                           varName[0], 0, parse)==FATAL_ENGINE)
            return(FATAL_ENGINE);
      }
   }
   
   /***********************************************/
   /* Make sure at least one variable was defined */
   /***********************************************/
   if(numVars==0)
   {  if(AddErrMsg(FAIL_ENGINE|PARSE_LOCVARS|EMPTY_BLOCK, 
                            parse->token, 0, parse)==FATAL_ENGINE)
         return(FATAL_ENGINE);
   }
   
   return(OK);
}


/*--------------------------------------------------------------------------*/
/* ParseRule()                                                              */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long ParseRule(tParse *parse, tStatement **statementList)
{
   long      status;
   int       lastLine;

#ifdef DEVELOP
   debug_out("\nParsing rule: %s\n", parse->curRule->ruleName);
#endif

   /*********************************************/
   /* Check for local constant definition block */
   /*********************************************/
   while((status=LexRule(parse))==FAIL_ENGINE);
   if(status==END_OF_RULE)
   {  AddErrMsg(FATAL_ENGINE|PARSE_RULE|EARLY_TERM, NULL, 0, parse);
      return(FATAL_ENGINE);
   }
   else if(!strcmp(parse->token, "CONST"))
   {  if((status=ParseLocalConstants(parse))==FATAL_ENGINE)
         return(FATAL_ENGINE);
   }
   else
      PutBackToken(parse);
      
   /*****************************************/
   /* Check for local type definition block */
   /*****************************************/
   while((status=LexRule(parse))==FAIL_ENGINE);
   if(status==END_OF_RULE)
   {  AddErrMsg(FATAL_ENGINE|PARSE_RULE|EARLY_TERM, NULL, 0, parse);
      return(FATAL_ENGINE);
   }
   else if(!strcmp(parse->token, "TYPE"))
   {  if((status=ParseLocalTypes(parse))==FATAL_ENGINE)
         return(FATAL_ENGINE);
   }
   else
      PutBackToken(parse);
      
   /*********************************************/
   /* Check for local variable definition block */
   /*********************************************/
   while((status=LexRule(parse))==FAIL_ENGINE);
   if(status==END_OF_RULE)
   {  AddErrMsg(FATAL_ENGINE|PARSE_RULE|EARLY_TERM, NULL, 0, parse);
      return(FATAL_ENGINE);
   }
   else if(!strcmp(parse->token, "VAR"))
   {  if((status=ParseLocalVariables(parse))==FATAL_ENGINE)
         return(FATAL_ENGINE);
   }
   else
      PutBackToken(parse);

   /****************************************************/
   /* If there are any errors in the definition blocks */
   /* then go no further.                              */
   /****************************************************/
   status = GetHighestParseSeverity(parse->curRule);
   if(status != OK && notWarning(status))
      return(FATAL_ENGINE);

   /****************************************************/
   /* Parse main statement.  Main statement could be a */
   /* single statement, or a statement list with a     */
   /* 'BEGIN' and 'END'                                */
   /****************************************************/
   if(ParseStatementList(parse, FROM_OTHER, statementList)==FATAL_ENGINE)
      return(FATAL_ENGINE);

   /**********************************************/
   /* Check for RETURM if rule has a return type */
   /**********************************************/
   if(parse->curRule->returnType != NOTYPE)
   {  if(!parse->mainReturn)
      {  if(parse->numReturns == 0)
         {   if(AddErrMsg(FAIL_ENGINE|PARSE_RULE|NO_RETURN, 
                                          NULL, 0, parse)==FATAL_ENGINE)
              return(FATAL_ENGINE);
         }
         else if(parse->numReturns != parse->numSubs)
         {   if(AddErrMsg(FAIL_ENGINE|PARSE_RULE|NO_RETURN, 
                                          NULL, 0, parse)==FATAL_ENGINE)
              return(FATAL_ENGINE);
         }
      }
   }

   lastLine = parse->lineNumber;

   /**********************************************************************/
   /* Check to see if any code exists after return from statement block  */
   /**********************************************************************/
   /* Get terminating semi-colon, if any */
   while((status=LexRule(parse))==FAIL_ENGINE);
   if(status==END_OF_RULE)
      return(OK);
   else if(strcmp(parse->token, ";"))
   {   parse->lineNumber = lastLine;
       if(AddErrMsg(WARN_ENGINE|PARSE_RULE|IGNORE_AFTER_LINE, 
                                          NULL, 0, parse)==FATAL_ENGINE)
          return(FATAL_ENGINE);
   }
   else
   {  /* Check for any other code */
      while((status=LexRule(parse))==FAIL_ENGINE);
      if(status==END_OF_RULE)
         return(OK);
      else
      {   parse->lineNumber = lastLine;
          if(AddErrMsg(WARN_ENGINE|PARSE_RULE|IGNORE_AFTER_LINE, 
                                             NULL, 0, parse)==FATAL_ENGINE)
             return(FATAL_ENGINE);
      }
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* ParseRuleList()                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long ParseRuleList
(tParse *parse, tRuleData *ruleList, int ruleType)
{
   long           retStatus=OK, status;
   tParseResults  *parseResults, *newResults, *tmpPR;   
   tRuleArg       *ruleArg;
   tEngVar        *tmpVar=NULL;

   /***********************************************/
   /* Loop through all the rules in the rule list */
   /***********************************************/
   parse->curRule = ruleList;
   while(parse->curRule)
   {  /*************************************************************/
      /* Reset the rule specific parameters in the parse structure */
      /*************************************************************/
      parse->lineNumber=1;
      parse->index=0;
      parse->ruleText = parse->curRule->ruleText;
      parse->curErr = NULL;
      parse->numBegins=0;
      parse->localTypeList=NULL;
      parse->localVarList=NULL;
      parse->paramVarList=NULL;
      parse->curRule->errList = NULL;
      parse->numSubs = 0;
      parse->numReturns = 0;
      parse->mainReturn = 0;
      parse->inExpr = 0;
      parse->traceOn = 0;

      /*********************************************/
      /* Set up a list for the parameter variables */
      /*********************************************/
      ruleArg = parse->curRule->argList;
      while(ruleArg)
      {  if(parse->paramVarList==NULL)
         {  if((tmpVar = (tEngVar*)MyMalloc(sizeof(tEngVar)))==NULL) 
               throw(MALLOC_EXCEPTION);
            parse->paramVarList = tmpVar;
         }
         else
         {  if((tmpVar->next = (tEngVar*)MyMalloc(sizeof(tEngVar)))==NULL) 
               throw(MALLOC_EXCEPTION);
            tmpVar = tmpVar->next;
         }
         memset((char*)tmpVar, 0, sizeof(tEngVar));
   
         tmpVar->varName = ruleArg->argName;
         tmpVar->flags |= VAR_PARAMETER;
         if(ruleArg->reference) tmpVar->flags |= VAR_REFERENCE;
         tmpVar->type = ruleArg->type;

         /* Allocate space for data if it is not a reference pointer */
         if(!(tmpVar->flags&VAR_REFERENCE))
         {  tmpVar->data = (void*)MyMalloc(sizeof(tEngNum));
            memset((char*)tmpVar->data, 0, sizeof(tEngNum));
            tmpVar->dataSize = sizeof(tEngNum);
         }

         /****************************************/
         /* Add the variable to the allData list */
         /****************************************/
         AddVarToAllData(parse->allData, tmpVar);
   
         ruleArg = ruleArg->next;
      }

      /****************************************/
      /* Allocate space for the parse results */
      /****************************************/
      parseResults = parse->parseResults;
      if((newResults = (tParseResults*)MyTmpMalloc(sizeof(tParseResults)))==NULL)
         throw(MALLOC_EXCEPTION);

      if(parseResults==NULL)
      {  parseResults = newResults;
         parse->parseResults = parseResults;
      }
      else
      {  tmpPR = parseResults;
         while(tmpPR->next) tmpPR = tmpPR->next;
         tmpPR->next = newResults;
      }
      memset((char*)newResults, 0, sizeof(tParseResults));
      newResults->engRB = parse->curRB;
 
      newResults->ruleData = parse->curRule;
      newResults->paramVarList = parse->paramVarList;
      newResults->ruleType = ruleType;

      /**************************************************/
      /* Call ParseRule to parse the rule at this index */
      /**************************************************/
      ParseRule(parse, &newResults->ruleStatementList);
 
      /*********************************************/
      /* Get the highest level error to this point */
      /*********************************************/
      status = GetHighestParseSeverity(parse->curRule);
   	if(retStatus==OK) retStatus = status;
   	else if(status!=OK && status < retStatus) retStatus=status;
 
      parse->curRule = parse->curRule->next;
   }

   return(retStatus);
}


/*--------------------------------------------------------------------------*/
/* CheckRuleUnique()                                                        */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long CheckRuleUnique(tRuleBaseData *ruleBaseData, tParse *parse)
{
   tRuleData *curRule, *checkRule;

   checkRule = ruleBaseData->ruleList;
   while(checkRule)
   {  curRule = ruleBaseData->ruleList;
      while(curRule)
      {  if(!NoCaseStrcmp(curRule->ruleName, checkRule->ruleName) && 
             curRule != checkRule)
         {  parse->curRule = checkRule;
            return(FALSE);
         }

         curRule = curRule->next;
      }

      checkRule = checkRule->next;
   }

   return(TRUE);
}


/*--------------------------------------------------------------------------*/
/* CreateTemplateRecord()                                                   */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long 
CreateTemplateRecord(tParse *parse, tDataTemplate *tmplt, tType **pType, short *fixed)
{
   tDataTemplate *tmp;
   tType         *type=NULL, *prevType=NULL, *subRec=NULL, *subArray=NULL;
   tDataField    *tmpltFld;
   tRecField     *recFld=NULL;
   tEngRecord    *record;
   char          errMsg[256];
   char          typeName[100];
   long          status, numFlds=0;

   /*****************************************************/
   /* Look through the type list to find the end of the */
   /* list, or to find if the type is already defined   */
   /*****************************************************/
   sprintf(typeName, "%s_TYPE", tmplt->templateName);  
   if(parse->globalTypeList)
   {  prevType = parse->globalTypeList;
      for(type=parse->globalTypeList; type; type=type->next)
      {  if(type->type == RECORD_TYPE) 
         {  if(!NoCaseStrcmp(typeName, type->fmt.record.recName))
            {  type->flags |= TYPE_SUBTMPLT;
               *pType = type;
               return(OK);
            }
         }

         prevType = type;
      }
   }

   
   /**********************************/
   /* Allocated a new type structure */
   /**********************************/
   if(prevType)
   {  if((prevType->next = (tType*)MyMalloc(sizeof(tType)))==NULL)
         throw(MALLOC_EXCEPTION);
      type = prevType->next;
   }
   else
   {  if((type = (tType*)MyMalloc(sizeof(tType)))==NULL)
         throw(MALLOC_EXCEPTION);
      parse->globalTypeList = type;
   }
   memset((char*)type, 0, sizeof(tType));
   *pType = type;
   type->flags |= TYPE_GLOBINT;
   type->flags |= TYPE_TEMPLATE;
  

   /******************************************************/
   /* Set the record name, which is <template_name>_TYPE */
   /******************************************************/
   if((type->typeName = (char*)MyMalloc(strlen(typeName)+1))==NULL)
      throw(MALLOC_EXCEPTION);
   strcpy(type->typeName, typeName);
   type->fmt.record.recName = type->typeName;
      

   /****************************/
   /* Set the record structure */
   /****************************/
   recFld=NULL;
   type->type = RECORD_TYPE;
   type->fmt.record.dataSize = tmplt->dataSize;
   record = &type->fmt.record;
   for(tmpltFld = tmplt->fieldList; tmpltFld; tmpltFld = tmpltFld->next, numFlds++)
   { 
      /* Allocate a new record field */
      if(recFld)
      {  if((recFld->next = (tRecField*)MyMalloc(sizeof(tRecField)))==NULL)
            throw(MALLOC_EXCEPTION);
         recFld = recFld->next;
      }
      else
      {  if((recFld = (tRecField*)MyMalloc(sizeof(tRecField)))==NULL)
            throw(MALLOC_EXCEPTION);
         type->fmt.record.fieldList = recFld;
      }
      memset((char*)recFld, 0, sizeof(tRecField));
      
      /* Set the other data for the field */
      recFld->fieldName = tmpltFld->fieldName;
      recFld->type = tmpltFld->type;
      recFld->offset = tmpltFld->offset;
      recFld->length = tmpltFld->length;
      recFld->nonFixed = &tmpltFld->nonFixed;

      if(recFld->nonFixed->fldID) 
         recFld->nonFixed->IDLen = (char)strlen(recFld->nonFixed->fldID);

      /* Create record type for any sub records */
      subRec=NULL; subArray=NULL;
      if(tmpltFld->subTmpltName && tmpltFld->type==RECORD_TYPE)
      {  for(tmp=parse->templateList; tmp; tmp=tmp->next)
         {  if(!NoCaseStrcmp(tmp->templateName, tmpltFld->subTmpltName))
               break;
         }

         /* Error out if sub template is not defined */
         if(!tmp)
         {  sprintf(errMsg, "%s$%s$%s",  
                    tmpltFld->fieldName, tmplt->templateName, tmpltFld->subTmpltName);
            AddErrMsg(FATAL_ENGINE|CREATE_TMPVAR|UNDEF_SUB_TMPLT, errMsg, 0, parse);
            return(FATAL_ENGINE);
         }

         /* Call CreateTemplateRecord for the sub-template */ 
         if((status=CreateTemplateRecord(parse, tmp, &subRec, fixed)))
            return(status);
      }
      else if(tmpltFld->subTmpltName)
      {  sprintf(errMsg, "%s$%s$%s",  
                 tmpltFld->fieldName, tmplt->templateName, tmpltFld->subTmpltName);
         AddErrMsg(FATAL_ENGINE|CREATE_TMPVAR|SUBTMPLT_NO_REC, errMsg, 0, parse);
         return(FATAL_ENGINE);
      }
      else if(tmpltFld->type == RECORD_TYPE)
      {  sprintf(errMsg, "%s$%s",  
                 tmpltFld->fieldName, tmplt->templateName);
         AddErrMsg(FATAL_ENGINE|CREATE_TMPVAR|REC_NO_SUBTMPLT, errMsg, 0, parse);
         return(FATAL_ENGINE);
      }


      /****************************************************************/
      /* Check for a nonFixed record to set up an array, if necessary */
      /****************************************************************/
      if(recFld->nonFixed->lengthInd || recFld->nonFixed->fldID)
         *fixed = 0;

      /* Allocate and set up array, if necessary */
      if(recFld->nonFixed->maxElements > 1)
      {  /* Go to the end of the global type list and allocate a new type */
         for(type=parse->globalTypeList; type->next; type=type->next);
         if((type->next = (tType*)MyMalloc(sizeof(tType)))==NULL)
            throw(MALLOC_EXCEPTION);
         type = type->next;
         memset((char*)type, 0, sizeof(tType));
         type->flags |= TYPE_GLOBINT;

         /* Set up type as an array */
         type->type = ARRAY_TYPE;
         type->fmt.array.startIndex = 1; 
         type->fmt.array.endIndex = recFld->nonFixed->maxElements;
         type->fmt.array.numElems = recFld->nonFixed->maxElements;

         /* Set the name as recordName.fieldName */
         sprintf(typeName, "%s.%s", tmplt->templateName, recFld->fieldName);
         if((type->typeName = (char*)MyMalloc(strlen(typeName)+1))==NULL)
            throw(MALLOC_EXCEPTION);
         strcpy(type->typeName, typeName);
         type->fmt.array.name = type->typeName;

         if(recFld->type==RECORD_TYPE)    
         {  type->fmt.array.typeFmt = &subRec->fmt;
            type->fmt.array.type = RECORD_TYPE;
            type->fmt.array.elemSize = subRec->fmt.record.dataSize; 	 
         }
         else 
         {  type->fmt.array.type = tmpltFld->type;
            type->fmt.array.elemSize = tmpltFld->length;
         }

         type->fmt.array.dataSize = 
                type->fmt.array.numElems * type->fmt.array.elemSize;
         subArray = type;
      }

      /* Set the sub-type of the field, if any */
      if(subArray)
      {  recFld->type = ARRAY_TYPE;
         recFld->typeFmt = &subArray->fmt;
      }
      else if(subRec)
      {  recFld->type = RECORD_TYPE;
         recFld->typeFmt = &subRec->fmt;
      }
   }

   /* Set the number of fields in the record */
   record->numFlds = numFlds;

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* CreateVariablesFromTemplates()                                           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static long CreateVariablesFromTemplates(tParse *parse)
{
   tDataTemplate *tmplt;
   tType         *type=NULL;
   tEngVar       *var=NULL, *startVar=NULL;
   tRecInstance  *tmpltRec=NULL;
   char          typeName[100];
   short         exists=FALSE, fixed=1, mainRB=0;
   long          status;

 
   /**************************************************/
   /* Set flag if have first of a list of rule bases */
   /**************************************************/
   mainRB = (parse->curRB == parse->RBList);
   
   /****************************************************************************/
   /* Loop through the templates, creating a record type and variable for each */
   /* Assuming that each field in the template is of a built-in type.  There   */
   /* is currently no support for templates within templates and arrays within */
   /* templates, i.e. variable record lengths.                                 */
   /****************************************************************************/
   for(tmplt=parse->templateList; tmplt; tmplt=tmplt->next)
   {  exists = FALSE;
      sprintf(typeName, "%s_TYPE", tmplt->templateName);
    
      /*********************************************************/
      /* Go to the end of the var list, making sure a variable */
      /* with the same name doesn't already exist              */
      /*********************************************************/
      if(parse->globalVarList)
      {  for(var=parse->globalVarList; var->next; var=var->next)
         {  if(!NoCaseStrcmp(var->varName, tmplt->templateName))
            {   /* If exists but is same definition, then is OK */  
                if(var->type == RECORD_TYPE)  
                {  if(!strcmp(((tRecInstance*)var->data)->data->record->recName, typeName))
                   {  exists = TRUE;
                      break;
                   }
                }

                AddErrMsg(FATAL_ENGINE|CREATE_TMPVAR|VAR_EXISTS_TMPLT, 
                                tmplt->templateName, 0, parse);
                return(FATAL_ENGINE);
            }
         }
      }

      /********************************************************************/
      /* If variable has already been created for this template, continue */
      /********************************************************************/
      if(exists) continue;

      /********************************************************/
      /* Create a type for the template and all sub-templates */
      /********************************************************/
      if((status=CreateTemplateRecord(parse, tmplt, &type, &fixed)))
         return(status);
      
      /*******************************************************************/
      /* Allocated a new variable structure, but only for templates that */
      /* are not sub-templates.                                          */
      /*******************************************************************/
      if(!(type->flags&TYPE_SUBTMPLT))
      {  if(var)
         {  if((var->next = (tEngVar*)MyMalloc(sizeof(tEngVar)))==NULL)
               throw(MALLOC_EXCEPTION);
            var = var->next;
         }
         else
         {  if((var = (tEngVar*)MyMalloc(sizeof(tEngVar)))==NULL)
               throw(MALLOC_EXCEPTION);
            parse->globalVarList = var;
         }
         memset((char*)var, 0, sizeof(tEngVar));
         var->flags |= VAR_GLOBINT;
         var->flags |= VAR_TEMPLATE;
         var->dataSize = tmplt->dataSize;
         if(!startVar) startVar = var;
  
         /*****************************************************/
         /* Set the variable name, which is the template name */
         /*****************************************************/
         if((var->varName = (char*)MyMalloc(strlen(tmplt->templateName)+1))==NULL)
            throw(MALLOC_EXCEPTION);
         strcpy(var->varName, tmplt->templateName);
      
         /*************************************************/
         /* Allocate a record instance to hold the record */
         /* structure and template data.                  */
         /*************************************************/
         if(mainRB)
         {  if(parse->tmpltRecList)
            {  if((tmpltRec->next = (tRecInstance*)MyMalloc(sizeof(tRecInstance)))==NULL)
                  throw(MALLOC_EXCEPTION);
               tmpltRec = tmpltRec->next;
            }
            else
            {  if((tmpltRec = (tRecInstance*)MyMalloc(sizeof(tRecInstance)))==NULL)
                  throw(MALLOC_EXCEPTION);
               parse->tmpltRecList = tmpltRec;
            }
            memset((char*)tmpltRec, 0, sizeof(tRecInstance));
            if(!fixed) tmpltRec->flags |= NON_FIXED;
         }

         /*************************************************/
         /* Set the record data in the variable data area */
         /*************************************************/
         var->type = RECORD_TYPE;

         /*************************************************/
         /* Flag to indicate that data is not to be freed */
         /* because in belongs to the template            */
         /*************************************************/
         var->flags |= VAR_NOFREE;
      
         /********************************************************************/
         /* Create the record instance data, unless it has already been done */
         /********************************************************************/
         if(mainRB && tmplt->record)
         {  *tmpltRec = *tmplt->record;  
            tmpltRec->next = NULL;
            var->data = (void*)tmpltRec;
            var->flags |= VAR_EXTDATA;
  
            continue; 
         }
         else if(!mainRB && tmplt->record)
         {  var->data = (void*)tmplt->record;
            var->flags |= VAR_EXTDATA;
  
            continue; 
         }
         else if(!mainRB && !tmplt->record)
         {  tDataTemplate *mainTmplt;

            /* If the template exists in the main rule base, get its record data */
            /* Otherwise, add a structure to the main tmpltRecList               */
            mainTmplt=parse->RBList->RBData->templateList;
            for(; mainTmplt; mainTmplt=mainTmplt->next)
            {  if(!NoCaseStrcmp(mainTmplt->templateName, tmplt->templateName))
                  break;
            }

            if(mainTmplt)
            {  /* Set the variable data with the template record instance */
               var->data = (void*)mainTmplt->record;
               var->flags |= VAR_EXTDATA;
               continue; 
            }
            else
            {  /* Go to end of list */
               tmpltRec=parse->tmpltRecList;
               for(; tmpltRec->next; tmpltRec=tmpltRec->next);

               /* Allocate a new tmpltRec */
               if(tmpltRec)
               {  if((tmpltRec->next = (tRecInstance*)MyMalloc(sizeof(tRecInstance)))==NULL)
                     throw(MALLOC_EXCEPTION);
                     tmpltRec = tmpltRec->next;
               }
               else
               {  if((tmpltRec = (tRecInstance*)MyMalloc(sizeof(tRecInstance)))==NULL)
                     throw(MALLOC_EXCEPTION);
                  parse->tmpltRecList = tmpltRec;
               }
               memset((char*)tmpltRec, 0, sizeof(tRecInstance));
               if(!fixed) tmpltRec->flags |= NON_FIXED;
            }
         } 

         /* Allocate template record instance data */
         if((status=CreateRecordData(&type->fmt.record, &tmpltRec->data, tmplt->data, 0)))
            return(status); 
         tmplt->record = tmpltRec;
         var->data = (void*)tmpltRec;
      }
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* ParseOneRuleBase()                                                       */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long ParseOneRuleBase(tParse *parse, tEngRBData *engRBData)
{
   long           retStatus=OK, status;

   /**************************************************************/
   /* First, make sure each rule name in the rule base is unique */
   /**************************************************************/
   parse->curErr = NULL;
   if(!CheckRuleUnique(engRBData->RBData, parse))
   {  AddErrMsg(FATAL_ENGINE|PARSE_RULE_BASE|DUP_RULE, 
                     parse->curRule->ruleName, 0, parse);
      return(FATAL_ENGINE);
   }

   /*********************************************************/
   /* Set up the rule base wide data in the parse structure */
   /*********************************************************/
   parse->templateList = engRBData->RBData->templateList;
   parse->funcList = engRBData->RBData->funcList;
   parse->ruleList = engRBData->RBData->ruleList;
   parse->globalVarList = engRBData->RBData->globalVarList;
   parse->globalTypeList = engRBData->RBData->globalTypeList;
   parse->whileLimit = engRBData->RBData->whileLoopLimit;
   parse->curRB = engRBData;
 
   /***************************************************************/
   /* Convert all templates into globally accessible record types */
   /* and variables.                                              */
   /***************************************************************/
   parse->curRule = engRBData->RBData->ruleList;
   status = CreateVariablesFromTemplates(parse);
   if(status == FATAL_ENGINE)
      return(FATAL_ENGINE);

   /****************************************************/
   /* First parse the rule base support rules, if any. */
   /* The init rules allow definition of globals, but  */
   /* no other rules allow it.                         */
   /****************************************************/
   /* Parse init rules */
   parse->allowGlobalDef = TRUE;
   status = ParseRuleList(parse, engRBData->RBData->supportRules.sysInitRuleList, 
                                                                    SYS_INIT_RULE);
   if(retStatus==OK) retStatus = status;
      else if(status!=OK && status < retStatus) retStatus=status;
   parse->allowGlobalDef = FALSE;

   status = ParseRuleList(parse, engRBData->RBData->supportRules.iterInitRuleList, 
                                                                    ITER_INIT_RULE);
   if(retStatus==OK) retStatus = status;
   else if(status!=OK && status < retStatus) retStatus=status;

   /* Parse termination rules */
   status = ParseRuleList(parse, engRBData->RBData->supportRules.sysTermRuleList, 
                                                                    SYS_TERM_RULE);
   if(retStatus==OK) retStatus = status;
   else if(status!=OK && status < retStatus) retStatus=status;

   status = ParseRuleList(parse, engRBData->RBData->supportRules.iterTermRuleList, 
                                                                    ITER_TERM_RULE);
   if(retStatus==OK) retStatus = status;
   else if(status!=OK && status < retStatus) retStatus=status;

   /* Parse the main rules in the rule base */
   status = ParseRuleList(parse, engRBData->RBData->ruleList, EXEC_RULE);
   if(retStatus==OK) retStatus = status;
   else if(status!=OK && status < retStatus) retStatus=status;

   /*************************************************************/
   /* Set the ruleBaseData global var list and global type list */
   /* with any newly added vars or types                        */
   /*************************************************************/
   if(engRBData->RBData->globalVarList==NULL) 
      engRBData->RBData->globalVarList = parse->globalVarList;

   if(engRBData->RBData->globalTypeList==NULL) 
      engRBData->RBData->globalTypeList = parse->globalTypeList;

   if(retStatus && notWarning(retStatus)) return(FATAL_ENGINE);
   else return(retStatus);
}


/*--------------------------------------------------------------------------*/
/* ParseRuleBase()                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long ParseRuleBase(tRuleBaseData *ruleBaseData, void **pParseResults)
{ 
   long          retStatus, status;
   tParse        p, *parse=&p;
   tRuleBaseData *RBData;
   tEngRBData    *engRBData=NULL, *delRBData;

   memset((char*)parse, 0, sizeof(tParse));
   
   /* Allocate the allData structure */
   if((parse->allData = (tDataPtr*)MyMalloc(sizeof(tDataPtr)))==NULL)
      throw(MALLOC_EXCEPTION);
   memset((char*)parse->allData, 0, sizeof(tDataPtr));

   /**************************/
   /* Create the RBData list */
   /**************************/
   for(RBData=ruleBaseData; RBData; RBData=RBData->next)
   {  if(engRBData)
      {  if((engRBData->next = (tEngRBData*)MyMalloc(sizeof(tEngRBData)))==NULL)
            throw(MALLOC_EXCEPTION);
         engRBData = engRBData->next;
      }
      else
      {  if((engRBData = (tEngRBData*)MyMalloc(sizeof(tEngRBData)))==NULL)
            throw(MALLOC_EXCEPTION);
         parse->RBList = engRBData;
      }
      memset((char*)engRBData, 0, sizeof(tEngRBData));
   
      engRBData->RBData = RBData;
   }
   
   /*************************************************************/
   /* Parse the first rule base in the list.  Others are parsed */
   /* later if they are called from the first.                  */
   /*************************************************************/
   status = ParseOneRuleBase(parse, parse->RBList);
   parse->RBList->RBData->status.errCode = status;

   /*****************************************************/
   /* Loop through rule bases, parse the ones that need */
   /* to be parsed, and free all the others.            */
   /*****************************************************/
   retStatus = status;
   engRBData=parse->RBList->next;
   while(engRBData)
   {  if(engRBData->parseIt)
      {  status = ParseOneRuleBase(parse, engRBData);
         engRBData->RBData->status.errCode = status;

         if(retStatus==OK) retStatus = status;
         else if(status!=OK && status < retStatus) 
            retStatus = status;

         engRBData = engRBData->next;
      }
      else
      {  delRBData = engRBData;
         engRBData = engRBData->next;
         MyFree((void*)delRBData);
      }
   }

   /************************************************/
   /* Set the allData in the parse results, if any */
   /************************************************/
   if(parse->parseResults) 
   {  parse->parseResults->allData = parse->allData; 
      parse->parseResults->tmpltRecList = parse->tmpltRecList;
   }

   if(pParseResults) 
      *pParseResults = parse->parseResults;
   else
      FreeParseResults(parse->parseResults, TRUE);

   return(retStatus);
}


/*--------------------------------------------------------------------------*/
/* ParseVarCalcRule()                                                       */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long ParseVarCalcRule(tParse *parse, tEngVar *var)
{
   tParse     p, *tmpParse=&p;
   tRuleData  rd, *tmpRule=&rd;
   long       status, errCt=0;
   char       errStr[4096], *pErrStr;
   tRuleErr   *ruleErr;


   /***********************************************/
   /* Make sure there is a rule for this variable */
   /***********************************************/
   if(var->calcRule==NULL)
      return(OK);

   /*************************************************************/
   /* If there is a data pointer, parsing has already been done */
   /*************************************************************/
   if(var->calcRule->data)
      return(OK);

   /**********************************************************/
   /* Copy most of the data from the current parse structure */
   /**********************************************************/
   memset((char*)tmpParse, 0, sizeof(tParse));

   tmpParse->templateList = parse->templateList;
   tmpParse->funcList = parse->funcList;
   tmpParse->ruleList = parse->ruleList;
   tmpParse->globalVarList = parse->globalVarList;
   tmpParse->globalTypeList = parse->globalTypeList;
   tmpParse->whileLimit = parse->whileLimit;
   tmpParse->traceOn = parse->traceOn;
   tmpParse->allData = parse->allData;
   tmpParse->ruleText = var->calcRule->code;
   tmpParse->lineNumber = 1;

   /******************************/
   /* Create a rule out the code */
   /******************************/
   tmpParse->curRule = tmpRule;
   memset((char*)tmpRule, 0, sizeof(tRuleData));
   tmpRule->ruleName = var->varName;
   tmpRule->ruleText = var->calcRule->code;
   
   if(IsNumeric(var->type))
      tmpRule->returnType = NUMERIC_TYPE;
   else
      tmpRule->returnType = var->type;

   /********************************************/
   /* Parse the code for this calculation rule */
   /********************************************/
   ParseRule(tmpParse, (tStatement**)&var->calcRule->data);

   /******************************************************/
   /* Put the error string, if any into the parse->token */
   /******************************************************/
   if(tmpRule->errList)
   {  ruleErr = tmpRule->errList;
      pErrStr = parse->token;
      errCt = 0;
      sprintf(pErrStr, "in parsing the rule for variable '%s'\n", var->varName);
      pErrStr += strlen(pErrStr);

      while(ruleErr)
      {  MakeRuleErrorString(errStr, tmpRule->ruleName,
                                    ruleErr, FALSE, EXEC_RULE);
         sprintf(pErrStr, "\t\t%s\n", errStr);
         pErrStr += strlen(pErrStr);
         ruleErr = ruleErr->next;
      }

      status = GetHighestParseSeverity(tmpRule);
      if(status == WARN_ENGINE)
         return(WARN_CALC_RULE);
      else if(status == FAIL_ENGINE)
         return(FAIL_CALC_RULE);
      else if(status == FATAL_ENGINE)
         return(FATAL_CALC_RULE);
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* ParseVarRule()                                                           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long ParseVarRule(tEngVar *var, void *handle)
{
   tParse        p, *tmpParse=&p;
   long          status;
   tCalcTable    *calcTab = (tCalcTable*)handle;
   tRuleBaseData *RBData = calcTab->ruleBaseData;

   /**********************************************************/
   /* Copy most of the data from the current parse structure */
   /**********************************************************/
   memset((char*)tmpParse, 0, sizeof(tParse));

   tmpParse->templateList = RBData->templateList;
   tmpParse->funcList = RBData->funcList;
   tmpParse->ruleList = RBData->ruleList;
   tmpParse->globalVarList = RBData->globalVarList;
   tmpParse->globalTypeList = RBData->globalTypeList;
   tmpParse->whileLimit = RBData->whileLoopLimit;
   tmpParse->allData = calcTab->allData;
   tmpParse->lineNumber = 1;
   tmpParse->traceOn = FALSE;

   /********************************************/
   /* Parse the code for this calculation rule */
   /********************************************/
   if((status = ParseVarCalcRule(tmpParse, var))!=OK)
   {  if(!RBData->status.errStr)
         RBData->status.errStr = (char*)MyMalloc(strlen(tmpParse->token)+1);
      strcpy(RBData->status.errStr, tmpParse->token);
      RBData->status.errCode = status; 
      throw(ENGINE_EXCEPTION);
   }

   /***************************************/
   /* Create a calc node for the variable */
   /***************************************/
   if((status=CreateVarCalc(NULL, var, calcTab)))
      return(status);

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* GetRecordsInRule()                                                       */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long GetRecordsInRule(char *varNames, char *ruleText)
{
   tParse     p, *parse=&p;
   tRuleData  rd, *tmpRule=&rd;
   long       i, first=1, status;
   char       *ptr;

   memset((char*)parse, 0, sizeof(tParse));

   parse->ruleText = ruleText;
   parse->curRule = tmpRule;

   ptr = varNames;
   varNames[0] = '\0';
   while((status=LexRule(parse))!=END_OF_RULE)
   {  if(status!=OK) continue;
      else if(parse->tokenType == T_RECFLD)
      {  i=0;

         if(first)
         {  sprintf(ptr, "%s", parse->token);
            first=0;
         }
         else
            sprintf(ptr, ",%s", parse->token);

         ptr += strlen(ptr);
      }
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* GetGlobalVarsInRule()                                                    */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long GetGlobalVarsInRule(char *varNames, char *ruleText)
{
   tParse     p, *parse=&p;
   tRuleData  rd, *tmpRule=&rd;
   long       i, first=1, status;
   char       recName[100], *ptr;

   memset((char*)parse, 0, sizeof(tParse));

   parse->ruleText = ruleText;
   parse->curRule = tmpRule;

   ptr = varNames;
   varNames[0] = '\0';
   while((status=LexRule(parse))!=END_OF_RULE)
   {  if(status!=OK) continue;
      else if(parse->tokenType == T_VARIABLE)
      {  if(first)
         {  sprintf(ptr, "%s", parse->token); 
            first=0;
         }
         else
            sprintf(ptr, ",%s", parse->token); 

         ptr += strlen(ptr);
      }
      else if(parse->tokenType == T_RECFLD)
      {  i=0;
         while(parse->token[i]!='.' && parse->token[i]!='\0')
         {  recName[i] = parse->token[i]; 
            i++;
         }
         recName[i] = '\0';

         if(first)
         {  sprintf(ptr, "%s", recName);
            first=0;
         }
         else
            sprintf(ptr, ",%s", recName);

         ptr += strlen(ptr);
      }
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* GetSetVarsInRule()                                                       */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long GetSetVarsInRule(char *varNames, char *ruleText)
{
   tParse     p, *parse=&p;
   tRuleData  rd, *tmpRule=&rd;
   long       inFunc=0, i, first=1, status;
   char       varName[100], *ptr;

   memset((char*)parse, 0, sizeof(tParse));

   parse->ruleText = ruleText;
   parse->curRule = tmpRule;

   ptr = varNames;
   varNames[0] = '\0';
   while((status=LexRule(parse))!=END_OF_RULE)
   {  if(status!=OK) continue;
      else if(!strcmp(parse->token, ":="))
      {  if(first)
         {  sprintf(ptr, "%s", varName);
            first=0;
         }
         else
            sprintf(ptr, ",%s", varName);

         ptr += strlen(ptr);
      }
      else if(parse->tokenType == T_FUNCTION)
         inFunc++;
      else if(inFunc && !strcmp(parse->token, ")"))
         inFunc--;
      else if(parse->tokenType == T_VARIABLE)
      {  strcpy(varName, parse->token); 

         if(inFunc)
         {  if(first)
            {  sprintf(ptr, "%s", varName);
               first=0;
            }
            else
               sprintf(ptr, ",%s", varName);
   
            ptr += strlen(ptr);
         }
      }
      else if(parse->tokenType == T_RECFLD)
      {  i=0;
         while(parse->token[i]!='.' && parse->token[i]!='\0')
         {  varName[i] = parse->token[i]; 
            i++;
         }
         varName[i] = '\0';

         if(inFunc)
         {  if(first)
            {  sprintf(ptr, "%s", varName);
               first=0;
            }
            else
               sprintf(ptr, ",%s", varName);
   
            ptr += strlen(ptr);
         }
      }
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* GetDependentVarsInRule()                                                 */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long GetDependentVarsInRule(char *mainName, char *varNames, char *ruleText)
{
   tParse     p, *parse=&p;
   tRuleData  rd, *tmpRule=&rd;
   long       inSet=0, inFunc=0, i, j, first=1, status;
   char       funcVars[4096], varName[100], *ptr, *fptr;

   memset((char*)parse, 0, sizeof(tParse));

   parse->ruleText = ruleText;
   parse->curRule = tmpRule;

   ptr = varNames;
   fptr = funcVars;
   varNames[0] = '\0';
   while((status=LexRule(parse))!=END_OF_RULE)
   {  if(status!=OK) continue;
      else if(!strcmp(parse->token, ":="))
      {  if(!strcmp(mainName, varName))
            inSet = 1;
      }
      else if(!strcmp(parse->token, ";") && inSet)
         inSet = 0;
      else if(parse->tokenType == T_FUNCTION)
         inFunc++;
      else if(inFunc && !strcmp(parse->token, ")"))
      {  inFunc--;

         /* If mainName is in the funcVars list, add them all */
         if(inFunc==0)
         {  i=0;
            while(1)
            {  j=0;
               while(funcVars[i]!=',' && funcVars[i]!='\0')
               {  varName[j] = funcVars[i];
                  i++; j++;
               }
               varName[j] ='\0';
             
               if(!strcmp(varName, mainName))
               {  if(first)
                  {  sprintf(ptr, "%s", funcVars);
                     first=0;
                  }
                  else
                     sprintf(ptr, ",%s", funcVars);
                  ptr += strlen(funcVars);
               }

               if(funcVars[i] == '\0') break;
               else i++;
            } 

            funcVars[0] = '\0';
            fptr = funcVars;
         }
      }
      else if(parse->tokenType == T_VARIABLE)
      {  strcpy(varName, parse->token);

         if(inSet)
         {  if(first)
            {  sprintf(ptr, "%s", varName);
               first=0;
            }
            else
               sprintf(ptr, ",%s", varName);
            ptr += strlen(ptr);
         }
      }
      else if(parse->tokenType == T_RECFLD)
      {  i=0;
         while(parse->token[i]!='.' && parse->token[i]!='\0')
         {  varName[i] = parse->token[i]; 
            i++;
         }
         varName[i] = '\0';

         if(inSet)
         {  if(first)
            {  sprintf(ptr, "%s", varName);
               first=0;
            }
            else
               sprintf(ptr, ",%s", varName);
            ptr += strlen(ptr);
         }
      }
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* IsFuncCallInRule()                                                       */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long IsFuncCallInRule(char *ruleText)
{
   tParse     p, *parse=&p;
   tRuleData  rd, *tmpRule=&rd;
   long       status;

   memset((char*)parse, 0, sizeof(tParse));

   parse->ruleText = ruleText;
   parse->curRule = tmpRule;

   while((status=LexRule(parse))!=END_OF_RULE)
   {  if(status!=OK) continue;
      else if(parse->tokenType == T_FUNCTION)
         return(TRUE);
   }

   return(FALSE);
}


/*--------------------------------------------------------------------------*/
/* GetFieldData()                                                           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
char *GetFieldData
(char *templateName, char *fieldName, tDataTemplate *templateList)
{
   tDataTemplate *tmplt;
   tDataField    *fld;

   for(tmplt=templateList; tmplt; tmplt=tmplt->next)
   {  if(!strcmp(templateName, tmplt->templateName))
      {  for(fld=tmplt->fieldList; fld; fld=fld->next)
         {  if(!strcmp(fieldName, fld->fieldName))
               return(tmplt->data + fld->offset);
         }
      }
   }

   return(NULL);
}


/*--------------------------------------------------------------------------*/
/* MakeTypeInstance()                                                       */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long MakeTypeInstance(tEngVar *var, tType *type)
{
   char *data;

   if(type->type==RECORD_TYPE)
   {  tRecInstance *recInstance;

      var->type = RECORD_TYPE;

      /* Create the tRecInstance structure */
      if((recInstance=(tRecInstance*)MyMalloc(sizeof(tRecInstance)))==NULL)
         throw(MALLOC_EXCEPTION); 
      memset((char*)recInstance, 0, sizeof(tRecInstance));

      /* Create a record data structure that holds data pointers for */
      /* all fields, including sub-records and arrays.               */
      if((data=(char*)MyMalloc(type->fmt.record.dataSize))==NULL) 
         throw(MALLOC_EXCEPTION);
      CreateRecordData(&type->fmt.record, &recInstance->data, data, 0);
      var->data = (void*)recInstance;
      var->dataSize = recInstance->data->record->dataSize;
   }
   else if(type->type==ARRAY_TYPE)
   {  tArrayData *aData;

      var->type = ARRAY_TYPE;

      /* Create an array data structue that holds data pointers for */
      /* all the elements in the array, including any records and   */
      /* sub-arrays that may exist.                                 */
      if((data=(char*)MyCalloc(type->fmt.array.elemSize, 
                               type->fmt.array.numElems))==NULL) 
         throw(MALLOC_EXCEPTION);
      CreateArrayData(&type->fmt.array, &aData, data);
      var->data = (void*)aData;
      var->dataSize = aData->array->elemSize * aData->array->numElems;
   }
  
   return(OK);
}

