/*
** $Id: engerr.c,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: engerr.c,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.4  1997/01/16  22:44:06  can
 * Lots of changes for 2.1 release
 *
 * Revision 2.3  1996/07/30  19:02:20  can
 * Added FreeRBDataErrors to free the tRuleErr structures
 *
 * Revision 2.2  1996/06/06  00:59:59  can
 * Removed newlines from some error strings
 *
 * Revision 2.1  1996/04/22  17:55:23  can
 * Several fixes and changes for 2.1
 *
 * Revision 1.13  1995/09/01  01:05:48  wle
 * Put in a counter so that no more than MAX_ERR_CT rules are output
 *
 * Revision 1.12  1995/08/25  01:25:48  can
 * Fixed compile warning messages
 *
 * Revision 1.11  1995/08/23  17:23:53  can
 * Fixed bug and made all lines less than 100 bytes
 *
 * Revision 1.10  1995/08/11  18:26:47  can
 * Modified the errStr allocation scheme.  Also, made MakeRuleErrorString
 * take a string buffer to be filled, instead of having it return the char*
 *
 * Revision 1.9  1995/08/07  23:41:13  can
 * Added message for no return from a rule
 *
 * Revision 1.8  1995/08/02  00:06:32  can
 * Modified call to MakeParseErrorString to put rule type in.
 * Also, changed GetEngineExceptionType to return the HNC
 * severity found in hncErrors.h
 *
 * Revision 1.6  1995/07/28  00:12:42  can
 * Changed error routines to take ruleBaseData structures.
 *
 * Revision 1.5  1995/07/21  21:46:14  can
 * Added check for severity
 *
 * Revision 1.3  1995/07/21  15:35:44  can
 * Added new error messages and error strings
 *
 * Revision 1.2  1995/07/16  18:23:31  can
 * Added error messages for the check to ensure that an expression does
 * something (must set a value, call a function, or call a rule)
 *
 * Revision 1.1  1995/07/09  23:07:20  can
 * Initial revision
 *
 *
*/

/* System include files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* HNC Rules Engine include files */
#include "mymall.h"
#include "engerr.h"
#include "typechk.h"


/*--------------------------------------------------------------------------*/
/* MakeEngineError()                                                        */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long  MakeEngineError(long status, tCalcTable *calcTab)
{
   tREStatus *errStat = &calcTab->ruleBaseData->status; 
   char      str[150];

   errStat->errCode = status;

   /************************************/
   /* Print string for engine severity */
   /************************************/
   if(isFatal(status))
   {  sprintf(str, "\nRules Engine fatally failed on rule base '%s'.  ", 
                                      calcTab->ruleBaseData->ruleBaseName);
      strcat(errStat->errStr, str); 
   }
   else if(notWarning(status))
   {  sprintf(str, "\nRules Engine failed on rule base '%s'.  ", 
                                      calcTab->ruleBaseData->ruleBaseName);
      strcat(errStat->errStr, str); 
   }
   else
   {  sprintf(str, "\nRules Engine warning on rule base '%s'.  ", 
                                      calcTab->ruleBaseData->ruleBaseName);
      strcat(errStat->errStr, str); 
   }

   /********************************/
   /* Print low-level error string */
   /********************************/
   switch(status&0x000000FF)
   {  case BAD_STATEMENT_TYPE:
         strcat(errStat->errStr, "Not a valid statement type\n");
         break;
      case UNKNOWN_OPERATOR:
         strcat(errStat->errStr, "Unknown operator\n");
         break;
      case NO_CALC_TABLE:
         strcat(errStat->errStr, "No calc table exists\n");
         break;
      case FUNCTION_FAILED:
         strcat(errStat->errStr, "Linked function failed\n");
         break;
      case BAD_ATOM_TYPE:
         strcat(errStat->errStr, "Bad data type in atom\n");
         break;
      case UNKNOWN_BLTIN:
         strcat(errStat->errStr, "Use of an undefined built-in function\n");
         break;
      case MEM_POOL_EXCEEDED:
         strcat(errStat->errStr, "Run-time memory pooled exceeded\n");
         break;
      case NO_EVAL_RULES:
         strcat(errStat->errStr, "No evaluatable rules are in the rule base\n");
         break;
      case RULE_PARAM_MISMTCH:
         strcat(errStat->errStr, "Given parameters does not match rule parameter list\n");
         break;
   }
   
   return(status);
}


/*--------------------------------------------------------------------------*/
/* MakeRuleErrorString()                                                    */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long MakeRuleErrorString
(char* buf, char* ruleName, tRuleErr *error, int giveLine, int ruleType)
{
   char  *ptr;
   long  errCode;
   char  str1[100], str2[100], str3[100];
   int   i, j, first;

   if(!error->errCode)    
      return(OK);

   errCode = error->errCode;
   ptr = buf;

   if((errCode&0x0000FF00) != PARSE_RULE_BASE)
   {  switch(ruleType)
      {  case SYS_INIT_RULE:
            strcpy(ptr, "System Initialize ");       
            ptr += strlen(ptr);
            break;
         case SYS_TERM_RULE:
            strcpy(ptr, "System Terminate ");       
            ptr += strlen(ptr);
            break;
         case ITER_INIT_RULE:
            strcpy(ptr, "Iteration Initialize ");       
            ptr += strlen(ptr);
            break;
         case ITER_TERM_RULE:
            strcpy(ptr, "Iteration Terminate ");       
            ptr += strlen(ptr);
            break;
      } 

      if(giveLine) sprintf(ptr, "Rule '%s', line %d: ", ruleName, error->lineNum); 
      else sprintf(ptr, "Rule '%s': ", ruleName); 

      ptr += strlen(ptr);
   }

   switch(errCode&0x000000FF)
   {  case UNKNWN_CHAR:
         sprintf(ptr, "Unknown character '%s'", error->errToken);
         break;
      case UNMTCH_DQUOTE:
         sprintf(ptr, "Unmatched \"");
         break;
      case UNMTCH_SQUOTE:
         sprintf(ptr, "Unmatched '");
         break;
      case UNMTCH_RPAREN:
         sprintf(ptr, "Unmatched ')'");
         break;
      case UNMTCH_LPAREN:
         sprintf(ptr, "Unmatched '('");
         break;
      case UNMTCH_RBRACK:
         sprintf(ptr, "Unmatched right array bracket");
         break;
      case UNMTCH_LBRACK:
         sprintf(ptr, "Unmatched left array bracket");
         break;
      case TOO_MANY_DOTS:
         sprintf(ptr, "Use of '%s' illegal", error->errToken);
         break;
      case UNEXPCT_KEYWD:
         /* Get the bad token */
         i=0;
         while(error->errToken[i] != '$') {str1[i] = error->errToken[i]; i++;} 
         str1[i] = '\0';
        
         sprintf(ptr, "Unexpected '%s'.  ", str1);
         strcat(ptr, "Are you missing a ");

         /* Get all possible terminators */
         first=1;
         while(1)
         {  if(first) first=0;
            else strcat(ptr, " or a ");

            j=0; i++;
            while(error->errToken[i] != ' ' && error->errToken[i]!='\0') 
            { str1[j] = error->errToken[i]; j++; i++; } 
            str1[j] = '\0';

            sprintf(str2, "'%s'", str1);
            strcat(ptr, str2);

            if(error->errToken[i] == '\0') break;
         }
         strcat(ptr, "?");
        
         break;
      case OPERATR_EXPCT:
         /* Get the bad token */
         i=0;
         while(error->errToken[i] != '$') {str1[i] = error->errToken[i]; i++;} 
         str1[i] = '\0';
        
         sprintf(ptr, "Operator expected before '%s'.  ", str1);
         strcat(ptr, "Are you missing a ");

         /* Get all possible terminators */
         first=1;
         while(1)
         {  if(first) first=0;
            else strcat(ptr, " or a ");

            j=0; i++;
            while(error->errToken[i] != ' ' && error->errToken[i]!='\0') 
            { str1[j] = error->errToken[i]; j++; i++; } 
            str1[j] = '\0';

            sprintf(str2, "'%s'", str1);
            strcat(ptr, str2);

            if(error->errToken[i] == '\0') break;
         }
         strcat(ptr, "?");
        
         break;
      case EARLY_TERM:
         sprintf(ptr, "Rule terminated improperly");
         break;
      case OPERAND_EXPCT:
         sprintf(ptr, "Operand expected before '%s'", error->errToken);
         break;
      case BAD_STATMNT:
         sprintf(ptr, "Bad statement starting with '%s'", error->errToken);
         break;
      case NOT_STRING:
         sprintf(ptr, "Warning, using the NOT operation on "
                      "a string yields a numeric value of 0");
         break;
      case EMPTY_STATMNT:
         if(isWarning(errCode))
            sprintf(ptr, "Warning, empty statement");
         else
            sprintf(ptr, "Empty statement");
         break;
      case UNMTCH_BEGIN:
         sprintf(ptr, "'BEGIN' does not have matching 'END'");
         break;
      case UNMTCH_END:
         sprintf(ptr, "'END' does not have matching 'BEGIN'");
         break;
      case SYNTX_ERR:
         sprintf(ptr, "Syntax error at '%s'", error->errToken);
         break;
      case IF_NO_THEN:
         sprintf(ptr, "'IF' statement does not have a 'THEN'");
         break;
      case IGNORE_AFTER_LINE:
         sprintf(buf, "Rule '%s': Warning, code after line %d is ignored", 
                                                              ruleName,
                                                              error->lineNum);
         break;
      case IGNORE_AFTER_TOK:
         sprintf(buf, "Rule '%s': Warning, code after '%s' is ignored", 
                                                  ruleName,
                                                  error->errToken);
         break;
      case RULE_NOT_FOUND:
         sprintf(ptr, "Rule '%s' is not defined", error->errToken);
         break;
      case FUNC_NOT_FOUND:
         sprintf(ptr, "Function '%s' is not defined", error->errToken);
         break;
      case BAD_STRING_OP:
         sprintf(ptr, "'%s' operation is not valid on string types", error->errToken);
         break;
      case CANT_NEG_STR:
         sprintf(ptr, "Can't negate a string type");
         break;
      case OUT_OF_MEMORY:
         sprintf(ptr, "Out of memory");
         break;
      case BAD_NUM_PARMS:
         if((errCode&0x0000FF00)==PARSE_RULE_CALL)
            sprintf(ptr, "Incorrect number of parameters passed " 
                         "to rule '%s'.  %d are required", 
                                                 error->errToken, error->errData);
         else
            sprintf(ptr, "Incorrect number of parameters passed "
                         "to function '%s'.  %d are required",
                                                 error->errToken, error->errData);
         break;
      case PARAM_NOT_REF:
         if((errCode&0x0000FF00)==PARSE_RULE_CALL)
            sprintf(ptr, "Parameter #%d to rule '%s' must be "
                         "a data field or a variable",
                                          error->errData, error->errToken);
         else
            sprintf(ptr, "Parameter #%d to function '%s' must be "
                         "a data field or a variable",
                                          error->errData, error->errToken);
         break;
      case ILLEGAL_IN:
         sprintf(ptr, "'IN' operation does not have a '(' to start an argument list");
         break;
      case ILLEGAL_LVALUE:
         sprintf(ptr, "Left side of ':=' must be a data field or a variable");
         break;
      case SET_PARAMETER:
         sprintf(ptr, "Parameter '%s' is not a variable parameter.  "
                      "It may not be modified", error->errToken);
         break;
      case SET_READ_ONLY_TMPLT:
         sprintf(ptr, "Template '%s' is read-only.  "
                      "Fields may not be modified", error->errToken);
         break;
      case SET_READ_ONLY_FLD:
         /*********************/
         /* Get template name */
         /*********************/
         i=0;
         while(error->errToken[i] != '$') 
         {  str1[i] = error->errToken[i]; i++; } 
         str1[i] = '\0'; i++;
          
         /******************/
         /* Get field name */
         /******************/
         j=0;
         while(error->errToken[i] != '\0') 
         {  str2[j] = error->errToken[i]; j++; i++;}
         str2[j] = '\0';

         sprintf(ptr, "Field '%s' in template '%s' is read-only.  It cannot be modified",
                                                                            str2, str1);
         break;
      case EQUAL_NOT_SET:
         sprintf(ptr, "Illegal statement.  Did you intend to use "
                      "a ':=' instead of a '='?");
         break;
      case EXPR_NO_FUNCTION:
         sprintf(ptr, "Illegal statement.  Can only set a value, "
                      "call a function, or execute a rule");
         break;
      case REF_READ_ONLY_TMPLT:
         /*************************/
         /* Get func or rule name */
         /*************************/
         i=0;
         while(error->errToken[i] != '$') {str1[i] = error->errToken[i]; i++;} 
         str1[i] = '\0'; i++;
          
         /*********************/
         /* Get template name */
         /*********************/
         j=0;
         while(error->errToken[i] != '\0') 
         {  str2[j] = error->errToken[i]; j++; i++; }
         str2[j] = '\0'; i++;
          
         if((errCode&0x0000FF00)==PARSE_RULE_CALL)
            sprintf(ptr, "Parameter #%d of rule '%s' requires a modifiable "
                         "data field or variable, but "
                         "template '%s' is read-only", 
                                      error->errData, str1, str2);
         else
            sprintf(ptr, "Parameter #%d of function '%s' requires a "
                         "modifiable data field or variable, but "
                         "template '%s' is read-only", error->errData, str1, str2);
         break;
      case REF_READ_ONLY_FLD:
         /*************************/
         /* Get func or rule name */
         /*************************/
         i=0;
         while(error->errToken[i] != '$') 
         {  str1[i] = error->errToken[i]; i++;} 
         str1[i] = '\0'; i++;
          
         /*********************/
         /* Get template name */
         /*********************/
         j=0;
         while(error->errToken[i] != '$') 
         {  str2[j] = error->errToken[i]; j++; i++;}
         str2[j] = '\0'; i++;
          
         /******************/
         /* Get field name */
         /******************/
         j=0;
         while(error->errToken[i] != '\0') 
         {  str3[j] = error->errToken[i]; j++; i++;}
         str3[j] = '\0';

         if((errCode&0x0000FF00)==PARSE_RULE_CALL)
            sprintf(ptr, "Parameter #%d of rule '%s' requires a "
                         "modifiable data field or variable, but "
                         "field '%s' in template '%s' is read-only", 
                                          error->errData, str1, str3, str2);
         else
            sprintf(ptr, "Parameter #%d of function '%s' requires a "
                         "modifiable data field or variable, but "
                         "field '%s' in template '%s' is read-only", 
                                          error->errData, str1, str3, str2);
         break;
      case TYPE_MISMATCH_DT:
         if((errCode&0x0000FF00)==PARSE_RULE_CALL)
            sprintf(ptr, "Parameter #%d to rule '%s' expects a "
                         "date string, but is passed a time format string",
                                                 error->errData, error->errToken);
         else if((errCode&0x0000FF00)==PARSE_FUNC)
            sprintf(ptr, "Parameter #%d to function '%s' expects "
                         "a date string, but is passed "
                         "a time format string", error->errData, error->errToken);
         else if((errCode&0x0000FF00)==PARSE_IN)
            sprintf(ptr, "Element #%d in the 'IN' operation expects a date string, "
                         "but is passed a time format string", error->errData);
         else if((errCode&0x0000FF00)==PARSE_RETURN)
            sprintf(ptr, "Return type is a date string, but the return "
                         "expression is a time format string");
         else if((errCode&0x0000FF00)==PARSE_EXPR)
            sprintf(ptr, "Left operand of '%s' is a date string, but the right "
                         "operand is a time format string", error->errToken);
         break;
      case TYPE_MISMATCH_DN:
         if((errCode&0x0000FF00)==PARSE_RULE_CALL)
            sprintf(ptr, "Parameter #%d to rule '%s' expects a date string, but "
                         "is passed a numeric type",
                                                      error->errData, error->errToken);
         else if((errCode&0x0000FF00)==PARSE_FUNC)
            sprintf(ptr, "Parameter #%d to function '%s' expects a date string, but is "
                         "passed a numeric type",
                                                      error->errData, error->errToken);
         else if((errCode&0x0000FF00)==PARSE_IN)
            sprintf(ptr, "Element #%d in the 'IN' operation expects a date string, but "
                         "is passed a numeric type",
                                                                        error->errData);
         else if((errCode&0x0000FF00)==PARSE_RETURN)
            sprintf(ptr, "Return type is a date string, but the return expression "
                         "is a numeric type");
         else if((errCode&0x0000FF00)==PARSE_EXPR)
            sprintf(ptr, "Left operand of '%s' is a date string, but the right "
                         "operand is a numeric type", error->errToken);
         break;
      case TYPE_MISMATCH_DS:
         if((errCode&0x0000FF00)==PARSE_RULE_CALL)
            sprintf(ptr, "Parameter #%d to rule '%s' expects a date string, "
                         "but is passed a non-date format string",
                                                      error->errData, error->errToken);
         else if((errCode&0x0000FF00)==PARSE_FUNC)
            sprintf(ptr, "Parameter #%d to function '%s' expects a date string, "
                         "but is passed a non-date format string", 
                                         error->errData, error->errToken);
         else if((errCode&0x0000FF00)==PARSE_IN)
            sprintf(ptr, "Element #%d in the 'IN' operation expects a date string, "
                         "but is passed a non-date format string", error->errData);
         else if((errCode&0x0000FF00)==PARSE_RETURN)
            sprintf(ptr, "Return type is a date string, but the return expression is "
                         "not a date format string");
         else if((errCode&0x0000FF00)==PARSE_EXPR)
            sprintf(ptr, "Left operand of '%s' is a date string, but the right "
                         "operand is not a formatted date string", error->errToken);
         break;
      case TYPE_MISMATCH_TD:
         if((errCode&0x0000FF00)==PARSE_RULE_CALL)
            sprintf(ptr, "Parameter #%d to rule '%s' expects a time string, "
                         "but is passed a date format string",
                                                  error->errData, error->errToken);
         else if((errCode&0x0000FF00)==PARSE_FUNC)
            sprintf(ptr, "Parameter #%d to function '%s' expects a time string, "
                         "but is passed a date format string", 
                                                  error->errData, error->errToken);
         else if((errCode&0x0000FF00)==PARSE_IN)
            sprintf(ptr, "Element #%d in the 'IN' operation expects a time string, "
                         "but is passed a date format string", error->errData);
         else if((errCode&0x0000FF00)==PARSE_RETURN)
            sprintf(ptr, "Return type is a time string, but the return expression "
                         "is a date format string");
         else if((errCode&0x0000FF00)==PARSE_EXPR)
            sprintf(ptr, "Left operand of '%s' is a time string, but the right "
                         "operand is a date format string", error->errToken);
         break;
      case TYPE_MISMATCH_TN:
         if((errCode&0x0000FF00)==PARSE_RULE_CALL)
            sprintf(ptr, "Parameter #%d to rule '%s' expects a time string, "
                         "but is passed a numeric type",
                                                      error->errData, error->errToken);
         else if((errCode&0x0000FF00)==PARSE_FUNC)
            sprintf(ptr, "Parameter #%d to function '%s' expects a time string, "
                         "but is passed a numeric type",
                                                      error->errData, error->errToken);
         else if((errCode&0x0000FF00)==PARSE_IN)
            sprintf(ptr, "Element #%d in the 'IN' operation expects a time "
                         "string, but is passed a numeric type",
                                                                        error->errData);
         else if((errCode&0x0000FF00)==PARSE_RETURN)
            sprintf(ptr, "Return type is a time string, "
                         "but the return expression is a numeric type");
         else if((errCode&0x0000FF00)==PARSE_EXPR)
            sprintf(ptr, "Left operand of '%s' is a time string, but the right "
                         "operand is a numeric type", error->errToken);
         break;
      case TYPE_MISMATCH_TS:
         if((errCode&0x0000FF00)==PARSE_RULE_CALL)
            sprintf(ptr, "Parameter #%d to rule '%s' expects a "
                         "time string, but is passed a non-time format string",
                                                      error->errData, error->errToken);
         else if((errCode&0x0000FF00)==PARSE_FUNC)
            sprintf(ptr, "Parameter #%d to function '%s' expects a time string, "
                         "but is passed a non-time format string", 
                                                      error->errData, error->errToken);
         else if((errCode&0x0000FF00)==PARSE_IN)
            sprintf(ptr, "Element #%d in the 'IN' operation expects a time string, "
                         "but is passed a non-time format string", error->errData);
         else if((errCode&0x0000FF00)==PARSE_RETURN)
            sprintf(ptr, "Return type is a time string, but the return expression is "
                         "not a time format string");
         else if((errCode&0x0000FF00)==PARSE_EXPR)
            sprintf(ptr, "Left operand of '%s' is a time string, but the right "
                         "operand is not a time format string", error->errToken);
         break;
      case TYPE_MISMATCH_ND:
         if((errCode&0x0000FF00)==PARSE_RULE_CALL)
            sprintf(ptr, "Parameter #%d to rule '%s' expects a numeric value, "
                         "but is passed a date format string",
                                                      error->errData, error->errToken);
         else if((errCode&0x0000FF00)==PARSE_FUNC)
            sprintf(ptr, "Parameter #%d to function '%s' expects a "
                         "numeric value, but is passed "
                         "a date format string", error->errData, error->errToken);
         else if((errCode&0x0000FF00)==PARSE_IN)
            sprintf(ptr, "Element #%d in the 'IN' operation expects a "
                         "numeric value, but is passed "
                         "a date format string", error->errData);
         else if((errCode&0x0000FF00)==PARSE_RETURN)
            sprintf(ptr, "Return type is a numeric type, but the return expression "
                         "is a date format string");
         else if((errCode&0x0000FF00)==PARSE_EXPR)
            sprintf(ptr, "Left operand of '%s' is a numeric value, and the right "
                         "operand is a date format string", error->errToken);
         break;
      case TYPE_MISMATCH_NT:
         if((errCode&0x0000FF00)==PARSE_RULE_CALL)
            sprintf(ptr, "Parameter #%d to rule '%s' expects a numeric "
                         "value, but is passed a time format string",
                                                      error->errData, error->errToken);
         else if((errCode&0x0000FF00)==PARSE_FUNC)
            sprintf(ptr, "Parameter #%d to function '%s' expects a "
                         "numeric value, but is passed "
                         "a time format string", error->errData, error->errToken);
         else if((errCode&0x0000FF00)==PARSE_IN)
            sprintf(ptr, "Element #%d in the 'IN' operation expects a "
                         "numeric value, but is passed "
                         "a time format string", error->errData);
         else if((errCode&0x0000FF00)==PARSE_RETURN)
            sprintf(ptr, "Return type is a numeric type, "
                         "but the return expression is a time format string");
         else if((errCode&0x0000FF00)==PARSE_EXPR)
            sprintf(ptr, "Left operand of '%s' is a numeric value, and the right "
                         "operand is a time format string", error->errToken);
         break;
      case TYPE_MISMATCH_NS:
         if((errCode&0x0000FF00)==PARSE_RULE_CALL)
            sprintf(ptr, "Parameter #%d to rule '%s' expects a numeric value, "
                         "but is passed a string",
                                                      error->errData, error->errToken);
         else if((errCode&0x0000FF00)==PARSE_FUNC)
            sprintf(ptr, "Parameter #%d to function '%s' expects a numeric value, "
                         "but is passed a string", error->errData, error->errToken);
         else if((errCode&0x0000FF00)==PARSE_IN)
            sprintf(ptr, "Element #%d in the 'IN' operation expects "
                         "a numeric value, but is passed "
                         "a string", error->errData);
         else if((errCode&0x0000FF00)==PARSE_RETURN)
            sprintf(ptr, "Return type is a numeric type, "
                         "but the return expression is a string");
         else if((errCode&0x0000FF00)==PARSE_EXPR)
            sprintf(ptr, "Left operand of '%s' is a numeric value, and the right "
                         "operand is a string", error->errToken);
         break;
      case TYPE_MISMATCH_SN:
         if((errCode&0x0000FF00)==PARSE_RULE_CALL)
            sprintf(ptr, "Parameter #%d to rule '%s' expects a string, "
                         "but is passed a numeric type",
                                                      error->errData, error->errToken);
         else if((errCode&0x0000FF00)==PARSE_FUNC)
            sprintf(ptr, "Parameter #%d to function '%s' expects a string, "
                         "but is passed a numeric type",
                                                      error->errData, error->errToken);
         else if((errCode&0x0000FF00)==PARSE_IN)
            sprintf(ptr, "Element #%d in the 'IN' operation expects a string, "
                         "but is passed a numeric type",
                                                                        error->errData);
         else if((errCode&0x0000FF00)==PARSE_RETURN)
            sprintf(ptr, "Return type is a string, but the "
                         "return expression is a numeric type");
         else if((errCode&0x0000FF00)==PARSE_EXPR)
            sprintf(ptr, "Left operand of '%s' is a string, and the right "
                         "operand is a numeric type", error->errToken);
         break;
      case TYPE_MISMATCH_DNONE:
         if((errCode&0x0000FF00)==PARSE_RULE_CALL)
            sprintf(ptr, "Parameter #%d to rule '%s' expects a date string, "
                         "but is passed no value",
                                                  error->errData, error->errToken);
         else if((errCode&0x0000FF00)==PARSE_FUNC)
            sprintf(ptr, "Parameter #%d to function '%s' expects a "
                         "date string, but is passed "
                         "no value", error->errData, error->errToken);
         else if((errCode&0x0000FF00)==PARSE_IN)
            sprintf(ptr, "Element #%d in the 'IN' operation expects a date string, "
                         "but is passed no value", error->errData);
         else if((errCode&0x0000FF00)==PARSE_RETURN)
            sprintf(ptr, "Return type is a date string, but the "
                         "return expression has no value");
         else if((errCode&0x0000FF00)==PARSE_EXPR)
            sprintf(ptr, "Left operand of '%s' is a date string, but the right "
                         "operand has no value", error->errToken);
         break;
      case TYPE_MISMATCH_TNONE:
         if((errCode&0x0000FF00)==PARSE_RULE_CALL)
            sprintf(ptr, "Parameter #%d to rule '%s' expects a "
                         "time string, but is passed no value",
                                              error->errData, error->errToken);
         else if((errCode&0x0000FF00)==PARSE_FUNC)
            sprintf(ptr, "Parameter #%d to function '%s' expects a time string, "
                         "but is passed no value", error->errData, error->errToken);
         else if((errCode&0x0000FF00)==PARSE_IN)
            sprintf(ptr, "Element #%d in the 'IN' operation expects "
                         "a time string, but is passed "
                         "no value", error->errData);
         else if((errCode&0x0000FF00)==PARSE_RETURN)
            sprintf(ptr, "Return type is a time string, "
                         "but the return expression has no value");
         else if((errCode&0x0000FF00)==PARSE_EXPR)
            sprintf(ptr, "Left operand of '%s' is a time string, but the right "
                         "operand has no value", error->errToken);
         break;
      case TYPE_MISMATCH_NNONE:
         if((errCode&0x0000FF00)==PARSE_RULE_CALL)
            sprintf(ptr, "Parameter #%d to rule '%s' expects a numeric "
                         "value, but is passed no value",
                                                 error->errData, error->errToken);
         else if((errCode&0x0000FF00)==PARSE_FUNC)
            sprintf(ptr, "Parameter #%d to function '%s' expects a "
                         "numeric value, but is passed no value", 
                                                 error->errData, error->errToken);
         else if((errCode&0x0000FF00)==PARSE_IN)
            sprintf(ptr, "Element #%d in the 'IN' operation expects "
                         "a numeric value, but is passed no value", error->errData);
         else if((errCode&0x0000FF00)==PARSE_RETURN)
            sprintf(ptr, "Return type is a numeric type, "
                         "but the return expression has no value");
         else if((errCode&0x0000FF00)==PARSE_EXPR)
            sprintf(ptr, "Left operand of '%s' is a numeric value, but the right "
                         "operand has no value", error->errToken);
         break;
      case TYPE_MISMATCH_SNONE:
         if((errCode&0x0000FF00)==PARSE_RULE_CALL)
            sprintf(ptr, "Parameter #%d to rule '%s' expects a string, "
                         "but is passed no value",
                                                 error->errData, error->errToken);
         else if((errCode&0x0000FF00)==PARSE_FUNC)
            sprintf(ptr, "Parameter #%d to function '%s' expects a string, but is passed "
                         "no value", error->errData, error->errToken);
         else if((errCode&0x0000FF00)==PARSE_IN)
            sprintf(ptr, "Element #%d in the 'IN' operation expects a string, but is passed "
                         "no value", error->errData);
         else if((errCode&0x0000FF00)==PARSE_RETURN)
            sprintf(ptr, "Return type is a string, but the return expression has no value");
         else if((errCode&0x0000FF00)==PARSE_EXPR)
            sprintf(ptr, "Left operand of '%s' is a string, but the right "
                         "operand has no value", error->errToken);
         break;
      case TYPE_MISMATCH_NONE:
         sprintf(ptr, "Not expecting a value");
         break;
      case BAD_VARDEF_TOK:
         sprintf(ptr, "Bad token in variable definition block, '%s'", error->errToken);
         break;
      case MISSING_COLON:
         if((errCode&0x0000FF00)==PARSE_RECORD)
            sprintf(ptr, "Missing ':' after field '%s'", error->errToken);
         else if((errCode&0x0000FF00)==PARSE_LOCVARS)
            sprintf(ptr, "Missing ':' after variable '%s'", error->errToken);
         break;
      case MISSING_SEMI_COLON:
         if((errCode&0x0000FF00)==PARSE_RECORD)
            sprintf(ptr, "Missing ';' after definition of field '%s'", error->errToken);
         else if((errCode&0x0000FF00)==PARSE_LOCVARS)
            sprintf(ptr, "Missing ';' after definition of variable '%s'", error->errToken);
         else if((errCode&0x0000FF00)==PARSE_LOCCONST)
            sprintf(ptr, "Missing ';' after definition of constant '%s'", error->errToken);
         break;
      case BAD_VAR_TYPE:
         /*********************/
         /* Get variable name */
         /*********************/
         i=0;
         while(error->errToken[i] != '$') 
         {  str1[i] = error->errToken[i]; i++;} 
         str1[i] = '\0'; i++;
          
         /*****************/
         /* Get type used */
         /*****************/
         j=0;
         while(error->errToken[i] != '\0') 
         {  str2[j] = error->errToken[i]; i++; j++;}
         str2[j] = '\0';

         sprintf(ptr, "Variable '%s' is not given a known type, '%s'", str1, str2);
         break;
      case DUP_RULE:
         sprintf(ptr, "Rule '%s' is defined multiple times", error->errToken);
         break;
      case NO_RETURN:
         sprintf(buf, "Rule '%s': This rule requires a return "
                         "expression and there is none", ruleName);
         break;
      case EMPTY_BLOCK:
         if((errCode&0x0000FF00)==PARSE_STATMNT_LST)
            sprintf(ptr, "There is a BEGIN and END without any statements in between");
         else if((errCode&0x0000FF00)==PARSE_LOCVARS)
            sprintf(ptr, "The VAR block contains no variable definitions");
         else if((errCode&0x0000FF00)==PARSE_LOCCONST)
            sprintf(ptr, "The CONST block contains no constant definitions");
         else if((errCode&0x0000FF00)==PARSE_LOCTYPE)
            sprintf(ptr, "The TYPE block contains no type definitions");
         break;
      case FLD_EXISTS:
         /*******************/
         /* Get record name */
         /*******************/
         i=0;
         while(error->errToken[i] != '$') 
         {  str1[i] = error->errToken[i]; i++; }
         str1[i] = '\0'; i++;
          
         /***********************/
         /* Get field name used */
         /***********************/
         j=0;
         while(error->errToken[i] != '\0') 
         {  str2[j] = error->errToken[i]; i++; j++; }
         str2[j] = '\0';

         sprintf(ptr, "Field '%s' is already defined in record '%s'", str2, str1);
         break;
      case BAD_REC_TERM:
         sprintf(ptr, "Illegal '%s'.  Record definition must terminate with 'END'", 
                                                                    error->errToken);
         break;
      case BAD_FLD_TYPE:
         /*******************/
         /* Get record name */
         /*******************/
         i=0;
         while(error->errToken[i] != '$') 
         {  str1[i] = error->errToken[i]; i++; }
         str1[i] = '\0'; i++;

         /******************/
         /* Get field name */
         /******************/
         j=0;
         while(error->errToken[i] != '$') 
         {  str2[j] = error->errToken[i]; i++; j++; }
         str2[j] = '\0'; i++;
          
         /*****************/
         /* Get type used */
         /*****************/
         j=0;
         while(error->errToken[i] != '\0') 
         {  str3[j] = error->errToken[i]; i++; j++; }
         str3[j] = '\0';

         sprintf(ptr, "Field '%s' in record '%s' is not given a known type, '%s'", 
                                                                 str2, str1, str3);
         break;
      case TYPE_DEF_USE_EQUAL:
         sprintf(ptr, "Type definition must use '=' instead of ':='");
         break;
      case MISSING_SET:
         if((errCode&0x0000FF00)==PARSE_LOCCONST)
            sprintf(ptr, "Missing '=' to set constant value");
         else if((errCode&0x0000FF00)==PARSE_LOCTYPE)
            sprintf(ptr, "Missing '=' to set type definition");
         break;
      case TYPE_EXISTS_LOCTYPE:
         sprintf(ptr, "Type '%s' is already defined locally", error->errToken);
         break;
      case TYPE_EXISTS_GLOBTYPE:
         sprintf(ptr, "Warning, type '%s' is also declared as a global type", 
                                                               error->errToken);
         break;
      case CONST_DEF_USE_EQUAL:
         sprintf(ptr, "A constant definition must use '=' instead of ':='");
         break;
      case CONST_EXISTS_PARAM:
         sprintf(ptr, "The constant '%s' has the same name as a "
                      "parameter to the rule", error->errToken);
         break;
      case CONST_EXISTS_LOCCONST:
         sprintf(ptr, "The constant '%s' is already defined locally", 
                                                        error->errToken);
         break;
      case CONST_EXISTS_LOCVAR:
         sprintf(ptr, "The constant '%s' has the same name as a local variable",
                                                         error->errToken);
         break;
      case CONST_EXISTS_GLOBCONST:
         sprintf(ptr, "Warning, the constant '%s' is also declared as "
                      "a global constant", error->errToken);
         break;
      case CONST_EXISTS_GLOBVAR:
         sprintf(ptr, "Warning, the constant '%s' has the same name as "
                      "a global variable", error->errToken);
         break;
      case BAD_CONST_VALUE:
         sprintf(ptr, "Constant '%s' illegal.  Must be a numeric "
                      "value or a string", error->errToken);
         break;
      case VAR_EXISTS_LOCCONST:
         sprintf(ptr, "The variable '%s' has the same name as "
                      "a local constant", error->errToken);
         break;
      case VAR_EXISTS_LOCVAR:
         sprintf(ptr, "The variable '%s' is already defined", error->errToken);
         break;
      case VAR_EXISTS_GLOBCONST:
         sprintf(ptr, "Warning, variable '%s' has the same name as "
                      "a global constant", error->errToken);
         break;
      case VAR_EXISTS_GLOBVAR:
         sprintf(ptr, "Warning, variable '%s', is defined as "
                      "a global variable", error->errToken);
         break;
         break;
      case VAR_EXISTS_PARAM:
         sprintf(ptr, "Variable '%s' has the same name as a "
                      "parameter to the rule", error->errToken);
         break;
      case BAD_STRLEN:
         sprintf(ptr, "Error at '%s'.  String length must be "
                      "an integer constant", error->errToken);
         break;
      case STRLEN_NO_END:
         sprintf(ptr, "Error at '%s'.  Missing ']' of string length", 
                                                        error->errToken);
         break;
      case VAR_NOT_ARRAY:
         sprintf(ptr, "Variable '%s' is not an array!", error->errToken);
         break;
      case VAR_NOT_RECARRAY:
         sprintf(ptr, "Array variable '%s' is not a record", error->errToken);
         break;
      case VAR_NOT_FOUND:
         sprintf(ptr, "Variable '%s' is not defined", error->errToken);
         break;
      case FLD_NOT_FOUND:
         /*******************/
         /* Get record name */
         /*******************/
         i=0;
         while(error->errToken[i] != '$') 
         {  str1[i] = error->errToken[i]; i++;}
         str1[i] = '\0'; i++;

         /******************/
         /* Get field name */
         /******************/
         j=0;
         while(error->errToken[i] != '\0') 
         {  str2[j] = error->errToken[i]; i++; j++;}
         str2[j] = '\0'; 
          
         sprintf(ptr, "Field '%s' is not defined for record '%s'", str2, str1);
         break;
      case FLD_NOT_RECORD:
         /*******************/
         /* Get record name */
         /*******************/
         i=0;
         while(error->errToken[i] != '$') 
         {  str1[i] = error->errToken[i]; i++;}
         str1[i] = '\0'; i++;

         /******************/
         /* Get field name */
         /******************/
         j=0;
         while(error->errToken[i] != '\0') 
         {  str2[j] = error->errToken[i]; j++; i++;} 
         str2[j] = '\0';
          
         sprintf(ptr, "Field '%s' in record '%s' "
                      "is not a record.  Illegal '.'", str2, str1);
         break;
      case FLD_NOT_RECARRAY:
         /*******************/
         /* Get record name */
         /*******************/
         i=0;
         while(error->errToken[i] != '.') 
         {  str1[i] = error->errToken[i]; i++;}
         str1[i] = '\0'; i++;

         /******************/
         /* Get field name */
         /******************/
         j=0;
         while(error->errToken[i] != '\0') 
         {  str2[j] = error->errToken[i]; j++; i++;} 
         str2[j] = '\0';
          
         sprintf(ptr, "Field '%s' in record '%s' "
                      "is an array, but not a record.  Illegal '.'", str2, str1);
         break;
      case PARAM_NOT_ARRAY:
         /**********************/
         /* Get parameter name */
         /**********************/
         i=0;
         while(error->errToken[i] != '.') 
         {  str1[i] = error->errToken[i]; i++;} 
         str1[i] = '\0';
       
         sprintf(ptr, "Parameter '%s' is not a array!", str1);
         break;
      case PARAM_NOT_RECORD:
         /**********************/
         /* Get parameter name */
         /**********************/
         i=0;
         while(error->errToken[i] != '.') 
         {  str1[i] = error->errToken[i]; i++;} 
         str1[i] = '\0';
       
         sprintf(ptr, "Parameter '%s' is not a record.  Illegal '.'", str1);
         break;
      case PARAM_NOT_RECARRAY:
         /**********************/
         /* Get parameter name */
         /**********************/
         i=0;
         while(error->errToken[i] != '.') 
         {  str1[i] = error->errToken[i]; i++;} 
         str1[i] = '\0';
       
         sprintf(ptr, "The array parameter '%s' is not a record", str1);
         break;
      case VAR_NOT_RECORD:
         /*********************/
         /* Get variable name */
         /*********************/
         i=0;
         while(error->errToken[i] != '.')
         {  str1[i] = error->errToken[i]; i++;} 
         str1[i] = '\0';
       
         sprintf(ptr, "Variable '%s' is not a record.  Illegal '.'", str1);
         break;
      case RECORD_NOT_DEFINED:
         /*********************/
         /* Get variable name */
         /*********************/
         i=0;
         while(error->errToken[i] != '.')
         {  str1[i] = error->errToken[i]; i++;} 
         str1[i] = '\0';
       
         sprintf(ptr, "Record variable '%s' is not defined", str1);
         break;
      case SET_RECMISMATCH:
         if(error->errData==0)
         {  /*********************/
            /* Get variable name */
            /*********************/
            i=0;
            while(error->errToken[i] != '$') 
            {  str1[i] = error->errToken[i]; i++;}
            str1[i] = '\0'; i++;
   
            /************************/
            /* Get left record type */
            /************************/
            j=0;
            while(error->errToken[i] != '$') 
            {  str2[j] = error->errToken[i]; j++; i++;} 
            str2[j] = '\0'; i++;
             
            /*************************/
            /* Get right record type */
            /*************************/
            j=0;
            while(error->errToken[i] != '\0') 
            {  str3[j] = error->errToken[i]; j++; i++;} 
            str3[j] = '\0';

            sprintf(ptr, "Illegal set statement.  Record variable '%s' is "
                         "of type '%s' which is incompatible with record type '%s'", 
                         str1, str2, str3);
         }
         else
         {  /*********************/
            /* Get variable name */
            /*********************/
            i=0;
            while(error->errToken[i] != '$') 
            {  str1[i] = error->errToken[i]; i++;}
            str1[i] = '\0'; i++;
   
            /************************/
            /* Get left record type */
            /************************/
            j=0;
            while(error->errToken[i] != '\0') 
            {  str2[j] = error->errToken[i]; j++; i++;} 
            str2[j] = '\0';
             
            if(IsNumeric(error->errData))
               sprintf(ptr, "Illegal set statement.  Record variable '%s' "
                            "is of type '%s' which is incompatible with a "
                            "numeric expression", str1, str2);
            else
               sprintf(ptr, "Illegal set statement.  Record variable '%s' "
                            "is of type '%s' which is incompatible with a "
                            "string expression", str1, str2);
         }
         
         break;
      case SET_ARRMISMATCH:
         if(error->errData==0)
         {  /*********************/
            /* Get variable name */
            /*********************/
            i=0;
            while(error->errToken[i] != '$') 
            {  str1[i] = error->errToken[i]; i++;}
            str1[i] = '\0'; i++;
   
            /***********************/
            /* Get left array type */
            /***********************/
            j=0;
            while(error->errToken[i] != '$') 
            {  str2[j] = error->errToken[i]; j++; i++;} 
            str2[j] = '\0'; i++;
             
            /************************/
            /* Get right array type */
            /************************/
            j=0;
            while(error->errToken[i] != '\0') 
            {  str3[j] = error->errToken[i]; j++; i++;} 
            str3[j] = '\0';

            sprintf(ptr, "Illegal set statement.  Array variable '%s' is "
                         "of type '%s' which is incompatible with array type '%s'", 
                         str1, str2, str3);
         }
         else
         {  /*********************/
            /* Get variable name */
            /*********************/
            i=0;
            while(error->errToken[i] != '$') 
            {  str1[i] = error->errToken[i]; i++;}
            str1[i] = '\0'; i++;
   
            /***********************/
            /* Get left array type */
            /***********************/
            j=0;
            while(error->errToken[i] != '\0') 
            {  str2[j] = error->errToken[i]; j++; i++;} 
            str2[j] = '\0';
             
            if(IsNumeric(error->errData))
               sprintf(ptr, "Illegal set statement.  Array variable '%s' "
                            "is of type '%s' which is incompatible with a "
                            "numeric expression", str1, str2);
            else
               sprintf(ptr, "Illegal set statement.  Array variable '%s' "
                            "is of type '%s' which is incompatible with a "
                            "string expression", str1, str2);
         }
         
         break;
      case BAD_RECORD_OP:
         sprintf(ptr, "Bad operation of record variable '%s'.  "
                      "Records may only be set", error->errToken);
         break;
      case BAD_TYPE:
         sprintf(ptr, "Type '%s' is not defined.  Only allow 'RECORD' "
                      "and 'ARRAY' types", error->errToken);
         break;
      case CASE_NO_OF:
         sprintf(ptr, "Missing 'OF' in 'CASE' statement'");
         break;
      case CASE_NOT_NUMERIC:
         sprintf(ptr, "Element '%s' is case statement is "
                       "not an integer ", error->errToken);
         break;
      case CASE_NOT_STRING:
         sprintf(ptr, "Element '%s' is case statement is "
                      "not a character", error->errToken);
         break;
      case BAD_CASE_TOK:
         sprintf(ptr, "Illegal case element '%s'", error->errToken);
         break;
      case CASE_NO_END:
         sprintf(ptr, "Missing 'END' in 'CASE'statement");
         break;
      case FOR_USE_EQL:
         sprintf(ptr, "Must use ':=' instead of '=' in 'FOR' "
                      "initialization expression");
         break;
      case FOR_BAD_INIT:
         sprintf(ptr, "Bad initialization expression for 'FOR' loop");
         break;
      case FOR_NOT_NUMERIC:
         sprintf(ptr, "'FOR' loop must use numeric loop control expression");
         break;
      case MISSING_TO:
         sprintf(ptr, "Missing 'TO' or 'DOWNTO' in 'FOR' loop");
         break;
      case FOR_NO_DO:
         sprintf(ptr, "Missing 'DO' in 'FOR' loop");
         break;
      case WHILE_NO_DO:
         sprintf(ptr, "Missing 'DO' in 'WHILE' loop");
         break;
      case BAD_CASE_TYPE:
         sprintf(ptr, "Illegal expression type in case statement.  "
                      "Must be an ordinal type (integer or character)");
         break;
      case CASE_VAL_DUP:
         sprintf(ptr, "Case element '%s' is a duplicate of an "
                      "existing element", error->errToken);
         break;
      case TYPE_EXISTS_TMPLT:
         sprintf(ptr, "A type cannot be created for template '%s' "
                      "because '%s_TYPE' already exists", 
                                     error->errToken, error->errToken);
         break;
      case VAR_EXISTS_TMPLT:
         sprintf(ptr, "A variable cannot be created for template '%s' because '%s' "
                      "is already defined as a global variable", 
                       error->errToken, error->errToken);
         break;
      case CANT_MOD_LOOPVAR:
         sprintf(ptr, "Loop control variable '%s' may not be set", error->errToken);
         break;
      case LOOP_NOTVAR:
         sprintf(ptr, "'FOR' loop must use a local numeric variable as a controller");
         break;
      case TERM_NOT_NUMERIC:
         sprintf(ptr, "'TERMINATE' expression must be numeric");
         break;
      case FOR_NO_RUN: 
         sprintf(ptr, "'FOR' loop will never execute");
         break;
      case FOR_ZERO_STEP: 
         sprintf(ptr, "'STEP' value for 'FOR' loop must be non-zero");
         break;
      case EMPTY_CASE: 
         sprintf(ptr, "'CASE' statement has no elements");
         break;
      case BAD_WHILE_LIMIT: 
         sprintf(ptr, "'LIMIT' must be followed by an constant numeric value, > 0");
         break;
      case END_IN_LIST: 
         sprintf(ptr, "Can't have and 'END' in a list of case values");
         break;
      case EXEC_NOT_NAME: 
         sprintf(ptr, "'EXEC' must be followed by the name of "
                      "the rule base to be executed");
         break;
      case EXEC_NOT_FOUND: 
         sprintf(ptr, "Rule base '%s' is not defined for 'EXEC'", error->errToken);
         break;
      case FAIL_CALC_RULE: 
      case FATAL_CALC_RULE: 
         sprintf(ptr, "Error %s", error->errToken);
         break;
      case WARN_CALC_RULE: 
         sprintf(ptr, "Warning %s", error->errToken);
         break;
      case BAD_INDEX_TYPE:
         sprintf(ptr, "Index expression to array '%s' must be an integral type", 
                                                                 error->errToken);
         break;
      case INDEX_OB_LO:
         /*******************/
         /* Get start index */
         /*******************/
         i=0;
         while(error->errToken[i] != '$') 
         {  str1[i] = error->errToken[i]; i++; }
         str1[i] = '\0'; i++;
  
         /******************/
         /* Get array name */
         /******************/
         j=0;
         while(error->errToken[i] != '\0') 
         {  str2[j] = error->errToken[i]; j++; i++;} 
         str2[j] = '\0';
             
         sprintf(ptr, "Index '%d' is lower than starting index '%s' "
                       "for array '%s'", error->errData, str1, str2);
         break;
      case INDEX_OB_HI:
         /*****************/
         /* Get end index */
         /*****************/
         i=0;
         while(error->errToken[i] != '$') 
         {  str1[i] = error->errToken[i]; i++;}
         str1[i] = '\0'; i++;
  
         /******************/
         /* Get array name */
         /******************/
         j=0;
         while(error->errToken[i] != '\0') 
         {  str2[j] = error->errToken[i]; j++; i++;} 
         str2[j] = '\0';
             
         sprintf(ptr, "Index '%d' is higher than ending index "
                      "'%s' for array '%s'", error->errData, str1, str2);
         break;
      case MISSING_LBRACKET:
         sprintf(ptr, "Missing left array bracket before '%s'", error->errToken);
         break;
      case MISSING_RBRACKET:
         sprintf(ptr, "Missing right array bracket before '%s'", error->errToken);
         break;
      case BAD_START_INDEX:
         /*************/
         /* Get token */
         /*************/
         i=0;
         while(error->errToken[i] != '$') 
         {  str1[i] = error->errToken[i]; i++;}
         str1[i] = '\0'; i++;
  
         /******************/
         /* Get array name */
         /******************/
         j=0;
         while(error->errToken[i] != '\0') 
         {  str2[j] = error->errToken[i]; j++; i++;} 
         str2[j] = '\0';
             
         sprintf(ptr, "Bad starting index at '%s' for array '%s'", str1, str2);
         break;
      case MISSING_DOTDOT:
         sprintf(ptr, "Missing '..' for array '%s'", error->errToken);
         break;
      case BAD_END_INDEX:
         /*************/
         /* Get token */
         /*************/
         i=0;
         while(error->errToken[i] != '$') 
         {  str1[i] = error->errToken[i]; i++;}
         str1[i] = '\0'; i++;
  
         /******************/
         /* Get array name */
         /******************/
         j=0;
         while(error->errToken[i] != '\0') 
         {  str2[j] = error->errToken[i]; j++; i++;} 
         str2[j] = '\0';
             
         sprintf(ptr, "Bad ending index at '%s' for array '%s'", str1, str2);
         break;
      case BAD_RANGE:
         /*******************/
         /* Get start index */
         /*******************/
         i=0;
         while(error->errToken[i] != '$') 
         {  str1[i] = error->errToken[i]; i++;}
         str1[i] = '\0'; i++;
  
         /*****************/
         /* Get end index */
         /*****************/
         j=0;
         while(error->errToken[i] != '$') 
         {  str2[j] = error->errToken[i]; j++; i++;} 
         str2[j] = '\0'; i++;
             
         /******************/
         /* Get array name */
         /******************/
         j=0;
         while(error->errToken[i] != '\0') 
         {  str3[j] = error->errToken[i]; j++; i++;} 
         str3[j] = '\0';
             
         sprintf(ptr, "Bad range for array '%s'.  Starting index '%s' must be "
                      "less than or equal to ending index '%s'", str3, str1, str2);
         break;
      case MISSING_OF:
         /*************/
         /* Get token */
         /*************/
         i=0;
         while(error->errToken[i] != '$') 
         {  str1[i] = error->errToken[i]; i++;}
         str1[i] = '\0'; i++;
  
         /******************/
         /* Get array name */
         /******************/
         j=0;
         while(error->errToken[i] != '\0') 
         {  str2[j] = error->errToken[i]; j++; i++;} 
         str2[j] = '\0';
             
         sprintf(ptr, "Missing 'OF' before '%s' for array '%s'", str1, str2);
         break;
      case BAD_ARRAY_TYPE:
         /*****************/
         /* Get type name */
         /*****************/
         i=0;
         while(error->errToken[i] != '$') 
         {  str1[i] = error->errToken[i]; i++;}
         str1[i] = '\0'; i++;
  
         /******************/
         /* Get array name */
         /******************/
         j=0;
         while(error->errToken[i] != '\0') 
         {  str2[j] = error->errToken[i]; j++; i++;} 
         str2[j] = '\0';
             
         sprintf(ptr, "Type '%s' is not defined for use with array '%s'", str1, str2);
         break;
      case ARRAY_OF_ARRAY:
         /*****************/
         /* Get type name */
         /*****************/
         i=0;
         while(error->errToken[i] != '$') 
         {  str1[i] = error->errToken[i]; i++;}
         str1[i] = '\0'; i++;
  
         /******************/
         /* Get array name */
         /******************/
         j=0;
         while(error->errToken[i] != '\0') 
         {  str2[j] = error->errToken[i]; j++; i++;} 
         str2[j] = '\0';
             
         sprintf(ptr, "Array '%s' cannot be of type '%s' because it is also an array", 
                                                                           str2, str1);
         break;
      case UNDEF_SUB_TMPLT:
         /******************/
         /* Get field name */
         /******************/
         i=0;
         while(error->errToken[i] != '$') 
         {  str1[i] = error->errToken[i]; i++;}
         str1[i] = '\0'; i++;
  
         /*********************/
         /* Get template name */
         /*********************/
         j=0;
         while(error->errToken[i] != '$') 
         {  str2[j] = error->errToken[i]; j++; i++;} 
         str2[j] = '\0'; i++;
             
         /*************************/
         /* Get sub-template name */
         /*************************/
         j=0;
         while(error->errToken[i] != '\0') 
         {  str3[j] = error->errToken[i]; j++; i++;} 
         str3[j] = '\0';
             
         sprintf(ptr, "Field '%s' in template '%s' is declared as a "
                      "record of type '%s', which is not defined", 
                      str1, str2, str3);
         break;
      case SUBTMPLT_NO_REC:
         /******************/
         /* Get field name */
         /******************/
         i=0;
         while(error->errToken[i] != '$') 
         {  str1[i] = error->errToken[i]; i++;}
         str1[i] = '\0'; i++;
  
         /*********************/
         /* Get template name */
         /*********************/
         j=0;
         while(error->errToken[i] != '$') 
         {  str2[j] = error->errToken[i]; j++; i++;} 
         str2[j] = '\0'; i++;
             
         /*************************/
         /* Get sub-template name */
         /*************************/
         j=0;
         while(error->errToken[i] != '\0') 
         {  str3[j] = error->errToken[i]; j++; i++;} 
         str3[j] = '\0';
             
         sprintf(ptr, "Field '%s' in template '%s' is not declared as a "
                      "record type, but it is has a sub-template name of '%s'",
                      str1, str2, str3);
         break;
      case REC_NO_SUBTMPLT:
         /******************/
         /* Get field name */
         /******************/
         i=0;
         while(error->errToken[i] != '$') 
         {  str1[i] = error->errToken[i]; i++;}
         str1[i] = '\0'; i++;
  
         /*********************/
         /* Get template name */
         /*********************/
         j=0;
         while(error->errToken[i] != '\0') 
         {  str2[j] = error->errToken[i]; j++; i++;} 
         str2[j] = '\0';
             
         sprintf(ptr, "Field '%s' in template '%s' is declared as a "
                      "record type, but no sub-template name is given",
                      str1, str2);
         break;
      case ALWAYS_ONE:
         sprintf(ptr, "Warning: This %s operation will always result in a value of 1", 
                                                                      error->errToken);
         break;
      case NO_LPAREN:
         sprintf(ptr, "The %s operation requires parentheses around its argument.  "
                      "Missing left parenthesis", error->errToken);
         break;
      case NO_RPAREN:
         sprintf(ptr, "The %s operation requires parentheses around its argument.  "
                      "Missing right parenthesis", error->errToken);
         break;
      case RESOLVE_NOCOMMA:
         sprintf(ptr, "RESOLVE operation requires two arguments, separated by a comma");
         break;
      case RESOLVE_NONUMLEN:
         sprintf(ptr, "The second argument to the RESOLVE operation must be a numeric type");
         break;
      case RESOLVE_BADTYPE:
         sprintf(ptr, "RESOLVE operation only works on template records");
         break;
    }

    return(OK);
}


/*--------------------------------------------------------------------------*/
/* AddErrMsg()                                                              */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long AddErrMsg(long errCode, char* token, int errData, tParse *parse)
{
   tRuleErr *newErr;
   
   if((newErr = (tRuleErr*)MyMalloc(sizeof(tRuleErr)))==NULL)
      return(FATAL_ENGINE);
   memset((char*)newErr, 0, sizeof(tRuleErr));

   if(parse->curErr == NULL)
      parse->curRule->errList = parse->curErr = newErr;
   else
   {  parse->curErr->next = newErr;
      parse->curErr = parse->curErr->next; 
   }

   newErr->lineNum = parse->lineNumber;
   newErr->errData = errData;
   newErr->errCode = errCode;
   if(token)
   {  if((newErr->errToken = (char*)MyMalloc(strlen(token)+1))==NULL)
         return(FATAL_ENGINE);
      strcpy(newErr->errToken, token);
   }

   newErr->next = NULL;

   return(errCode&0xFF000000);
}


/*--------------------------------------------------------------------------*/
/* GetHighestParseSeverity()                                                */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long GetHighestParseSeverity(tRuleData *ruleData)
{
   tRuleErr *tmpErr;
   long     retValue=OK;

   tmpErr = ruleData->errList;
   while(tmpErr)
   {  if(isFatal(tmpErr->errCode))
         return(FATAL_ENGINE);
      else if(notWarning(tmpErr->errCode))
         retValue = FAIL_ENGINE;
      else if(isWarning(tmpErr->errCode) && retValue==OK)
         retValue = WARN_ENGINE;

      tmpErr = tmpErr->next;
   }

   return(retValue);
}


/*--------------------------------------------------------------------------*/
/* MakeRBParseError()                                                       */
/*                                                                          */
/*--------------------------------------------------------------------------*/
#define MAX_PARSE_ERR_STR   40960
#define MAX_ERR_CT   20
long MakeRBParseError
(long status, tRuleBaseData *RBData, int giveLines, char  **ppErrStr, int *errCt)
{
   tRuleErr      *ruleErr;
   tRuleData     *tmpRule;
   char          *pErrStr, nErrStr[10240];

   pErrStr = *ppErrStr;

   /***************************/
   /* Output severity message */
   /***************************/
   if(notWarning(status))
   {  sprintf(pErrStr, 
          "Failed to parse all rules successfully in rule base '%s'\n", 
                                               RBData->ruleBaseName); 
   }
   else
   {  sprintf(pErrStr, 
          "Warning in rule base '%s'\n", RBData->ruleBaseName); 
   }

   pErrStr += strlen(pErrStr);

   /**************************************************/
   /* Loop through rules and create the error string */ 
   /**************************************************/
   tmpRule = RBData->supportRules.sysInitRuleList;
   while(tmpRule)
   {  ruleErr = tmpRule->errList;
      while(ruleErr)
      {  MakeRuleErrorString(nErrStr, tmpRule->ruleName, 
                                    ruleErr, giveLines, SYS_INIT_RULE);
         sprintf(pErrStr, "\t%s\n", nErrStr);
         pErrStr += strlen(pErrStr);
         ruleErr = ruleErr->next;
          
         (*errCt)++;
         if(*errCt > MAX_ERR_CT)
         {  sprintf(pErrStr, "\tToo Many Errors to Continue\n");
            RBData->status.errCode = status;
            *ppErrStr = pErrStr;
            return(status);
         }
      }

      tmpRule = tmpRule->next;
   }

   tmpRule = RBData->supportRules.sysTermRuleList;
   while(tmpRule)
   {  ruleErr = tmpRule->errList;
      while(ruleErr)
      {  MakeRuleErrorString(nErrStr, tmpRule->ruleName, 
                                    ruleErr, giveLines, SYS_TERM_RULE);
         sprintf(pErrStr, "\t%s\n", nErrStr);
         pErrStr += strlen(pErrStr);
         ruleErr = ruleErr->next;
          
         (*errCt)++;
         if(*errCt > MAX_ERR_CT)
         {  sprintf(pErrStr, "\tToo Many Errors to Continue\n");
            RBData->status.errCode = status;
            *ppErrStr = pErrStr;
            return(status);
         }
      }

      tmpRule = tmpRule->next;
   }

   tmpRule = RBData->supportRules.iterInitRuleList;
   while(tmpRule)
   {  ruleErr = tmpRule->errList;
      while(ruleErr)
      {  MakeRuleErrorString(nErrStr, tmpRule->ruleName, 
                                    ruleErr, giveLines, ITER_INIT_RULE);
         sprintf(pErrStr, "\t%s\n", nErrStr);
         pErrStr += strlen(pErrStr);
         ruleErr = ruleErr->next;
          
         (*errCt)++;
         if(*errCt > MAX_ERR_CT)
         {  sprintf(pErrStr, "\tToo Many Errors to Continue\n");
            RBData->status.errCode = status;
            *ppErrStr = pErrStr;
            return(status);
         }
      }

      tmpRule = tmpRule->next;
   }

   tmpRule = RBData->supportRules.iterTermRuleList;
   while(tmpRule)
   {  ruleErr = tmpRule->errList;
      while(ruleErr)
      {  MakeRuleErrorString(nErrStr, tmpRule->ruleName, 
                                    ruleErr, giveLines, ITER_TERM_RULE);
         sprintf(pErrStr, "\t%s\n", nErrStr);
         pErrStr += strlen(pErrStr);
         ruleErr = ruleErr->next;
          
         (*errCt)++;
         if(*errCt > MAX_ERR_CT)
         {  sprintf(pErrStr, "\tToo Many Errors to Continue\n");
            RBData->status.errCode = status;
            *ppErrStr = pErrStr;
            return(status);
         }
      }

      tmpRule = tmpRule->next;
   }

   tmpRule = RBData->ruleList;
   while(tmpRule)
   {  ruleErr = tmpRule->errList;
      while(ruleErr)
      {  MakeRuleErrorString(nErrStr, tmpRule->ruleName, 
                                    ruleErr, giveLines, EXEC_RULE);
         sprintf(pErrStr, "\t%s\n", nErrStr);
         pErrStr += strlen(pErrStr);
         ruleErr = ruleErr->next;
          
         (*errCt)++;
         if(*errCt > MAX_ERR_CT)
         {  sprintf(pErrStr, "\tToo Many Errors to Continue\n");
            RBData->status.errCode = status;
            *ppErrStr = pErrStr;
            return(status);
         }
      }

      tmpRule = tmpRule->next;
   }

   RBData->status.errCode = status;

   *ppErrStr = pErrStr;
   return(status);
}


/*--------------------------------------------------------------------------*/
/* FreeRBDataErrors()                                                       */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long FreeRBDataErrors(tRuleBaseData *RBData)
{
   tRuleErr      *ruleErr, *delErr;
   tRuleData     *tmpRule;

   /***************************************************/
   /* Loop through rules free and tRuleErr structures */
   /***************************************************/
   tmpRule = RBData->supportRules.sysInitRuleList;
   while(tmpRule)
   {  ruleErr = tmpRule->errList;
      while(ruleErr)
      {  delErr = ruleErr;
         if(delErr->errToken) 
            MyFree((void*)delErr->errToken);
 
         ruleErr = ruleErr->next;
         MyFree((void*)delErr);
      }

      tmpRule->errList = NULL;
      tmpRule = tmpRule->next;
   }

   tmpRule = RBData->supportRules.sysTermRuleList;
   while(tmpRule)
   {  ruleErr = tmpRule->errList;
      while(ruleErr)
      {  delErr = ruleErr;
         if(delErr->errToken) 
            MyFree((void*)delErr->errToken);
 
         ruleErr = ruleErr->next;
         MyFree((void*)delErr);
      }

      tmpRule->errList = NULL;
      tmpRule = tmpRule->next;
   }

   tmpRule = RBData->supportRules.iterInitRuleList;
   while(tmpRule)
   {  ruleErr = tmpRule->errList;
      while(ruleErr)
      {  delErr = ruleErr;
         if(delErr->errToken) 
            MyFree((void*)delErr->errToken);
 
         ruleErr = ruleErr->next;
         MyFree((void*)delErr);
      }

      tmpRule->errList = NULL;
      tmpRule = tmpRule->next;
   }

   tmpRule = RBData->supportRules.iterTermRuleList;
   while(tmpRule)
   {  ruleErr = tmpRule->errList;
      while(ruleErr)
      {  delErr = ruleErr;
         if(delErr->errToken) 
            MyFree((void*)delErr->errToken);
 
         ruleErr = ruleErr->next;
         MyFree((void*)delErr);
      }

      tmpRule->errList = NULL;
      tmpRule = tmpRule->next;
   }

   tmpRule = RBData->ruleList;
   while(tmpRule)
   {  ruleErr = tmpRule->errList;
      while(ruleErr)
      {  delErr = ruleErr;
         if(delErr->errToken) 
            MyFree((void*)delErr->errToken);
 
         ruleErr = ruleErr->next;
         MyFree((void*)delErr);
      }

      tmpRule->errList = NULL;
      tmpRule = tmpRule->next;
   }

   return(OK);
}


/*--------------------------------------------------------------------------*/
/* MakeParseError()                                                         */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long MakeParseError(long status, tRuleBaseData *RBList, int giveLines)
{
   tRuleBaseData *RBData;
   char          *pErrStr;
   int           errCt=0;
   long          retStatus;

   /*******************************************/
   /* Allocate a large block for parse errors */
   /*******************************************/
   RBList->status.errStr = 
        (char*)MyRealloc(RBList->status.errStr, MAX_PARSE_ERR_STR);
   pErrStr = RBList->status.errStr;

   /*************************************************************/
   /* Loop through all the rule bases, appending error messages */
   /*************************************************************/
   retStatus = status;
   for(RBData=RBList; RBData; RBData=RBData->next)
   {  if(RBData->status.errCode!=OK)
      {  retStatus = MakeRBParseError(status, RBData, giveLines, &pErrStr, &errCt);
         sprintf(pErrStr, "\n");
         pErrStr += strlen(pErrStr);
      }

      if(errCt > MAX_ERR_CT) 
      {  RBList->status.errCode = retStatus;
         return(retStatus);
      }
   }

   /**********************************/
   /* Free all rule error structures */
   /**********************************/
   for(RBData=RBList; RBData; RBData=RBData->next)
   {  if(RBData->status.errCode!=OK)
         FreeRBDataErrors(RBData);
   }

   RBList->status.errCode = retStatus;
   return(retStatus);
}


/*--------------------------------------------------------------------------*/
/* GetEngineExceptionString()                                               */
/*                                                                          */
/*--------------------------------------------------------------------------*/
char* GetEngineExceptionString(tRuleBaseData *RBData)
{
   return(RBData->status.errStr);
}


/*--------------------------------------------------------------------------*/
/* GetEngineExceptionType()                                                 */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long GetEngineExceptionType(tRuleBaseData *RBData)
{
   if(isFatal(RBData->status.errCode)) return(1);
   else if(isFailure(RBData->status.errCode)) return(2);
   else if(isWarning(RBData->status.errCode)) return(3);
   else if(isInform(RBData->status.errCode)) return(4);
   else return(0);
}


/*--------------------------------------------------------------------------*/
/* GetEngineExceptionNumber()                                               */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long GetEngineExceptionNumber(tRuleBaseData *RBData)
{
   return(RBData->status.errCode);
}
