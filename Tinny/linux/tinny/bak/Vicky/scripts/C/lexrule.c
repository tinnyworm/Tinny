/*
** $Id: lexrule.c,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: lexrule.c,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.2  1997/01/16  22:44:06  can
 * Lots of changes for 2.1 release
 *
** Revision 2.1  1996/04/22 17:55:45  can
** Several fixes and changes for 2.1
**
 * Revision 1.8  1995/09/25  17:58:24  can
 * Fix for bug #34.  Added check for spaces between function name and first '('
 *
 * Revision 1.7  1995/09/01  17:02:47  wle
 * Added '_' as possible firse character for identifiers
 *
 * Revision 1.6  1995/08/25  01:25:48  can
 * Fixed compile warning messages
 *
 * Revision 1.5  1995/08/07  16:47:33  can
 * Took out '!'.
 *
 * Revision 1.4  1995/08/04  21:21:01  can
 * Added {* *} as comments.
 *
 * Revision 1.3  1995/07/21  15:35:44  can
 * Added support for variable name lexing.
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
#include <ctype.h>

/* HNC Rules Engine include files */
#include "lexrule.h"
#include "engerr.h"
#include "typechk.h"

#ifdef DEVELOP 
#include "rbdebug.h"
#endif


static char* aKeyWords[] = {
"VAR",
"RESOLVE",
"ERRSTR",
"ERRNO",
"EXISTS",
"SIZEOF",
"MACRO"
"GLOBAL",
"CONST",
"TYPE",
"NUMERIC",
"STRING",
"ARRAY",
"OF",
"RECORD",
"DO",
"FOR",
"TO",
"DOWNTO",
"STEP",
"WHILE",
"LIMIT",
"IF",
"THEN",
"ELSE",
"CASE",
"OF",
"DEFAULT",
"AND",
"OR",
"NOT",
"IN",
"BEGIN",
"END",
"TERMINATE",
"RETURN",
"TRACEON",
"TRACEOFF",
"EXEC"
};


/*--------------------------------------------------------------------------*/
/* LexRule()                                                                */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long LexRule(tParse *parse)
{
   int  i=0, j, haveDecimal=0, haveRecField=0, inArray=0;
   int  numKeywords=sizeof(aKeyWords)/sizeof(char*);
   long status;
   char temp[MAX_TOKEN_LENGTH];

   parse->data.dataFld = NULL;
   parse->data.var = NULL;

   while(parse->ruleText[parse->index]=='\n' ||
         isspace(parse->ruleText[parse->index]))
   {  if(parse->ruleText[parse->index]=='\n')
         parse->lineNumber++;
      parse->index++;
   }

   parse->lastIndex = parse->index;

   if(parse->ruleText[parse->index] == '\0')
      return(END_OF_RULE);

   while(isspace(parse->ruleText[parse->index])) parse->index++;

   if(parse->ruleText[parse->index]=='@')
   {  /* Have a rule name */
      parse->tokenType = T_RULE;
 
      /* Skip the '@' sign */
      parse->index++;
      while(isspace(parse->ruleText[parse->index])) parse->index++;
 
      while(isalnum(parse->ruleText[parse->index]) ||
            parse->ruleText[parse->index]=='_')
         parse->token[i++] = parse->ruleText[parse->index++];
   }
   else if(isalpha(parse->ruleText[parse->index]) ||
                  (parse->ruleText[parse->index] == '_'))
   {  while(1)
      {  if(parse->inExpr)
         {  if(parse->ruleText[parse->index]=='{' || 
               parse->ruleText[parse->index]=='[')
               inArray++;
            else if(inArray && (parse->ruleText[parse->index]=='}' || 
                     parse->ruleText[parse->index]==']'))
               inArray--;
            else if(!inArray && !(isalnum(parse->ruleText[parse->index]) || 
                     parse->ruleText[parse->index]=='.' ||
                     parse->ruleText[parse->index]=='_'))
               break;
            else if(parse->ruleText[parse->index] == '\0')
               break;
         }
         else 
         {  if(!inArray && !(isalnum(parse->ruleText[parse->index]) || 
                     parse->ruleText[parse->index]=='.' ||
                     parse->ruleText[parse->index]=='_'))
               break;
            else if(parse->ruleText[parse->index] == '\0')
               break;
         }
         
         if(parse->ruleText[parse->index]=='.')
         {  if(parse->ruleText[parse->index+1]=='.')
            {  /*****************************************************/
               /* Error can't have back to back dot's in a variable */
               /*****************************************************/
               /* Read till end of token */
               while(!isspace(parse->ruleText[parse->index]) &&  
                       parse->ruleText[parse->index] != '\n' &&  
                       parse->ruleText[parse->index] != '\0') 
                  parse->token[i++] = parse->ruleText[parse->index++];
          
               parse->token[i] = '\0';
               return(AddErrMsg(FAIL_ENGINE|LEX_RULE|TOO_MANY_DOTS, 
                                                  parse->token, 0, parse));
            }
            
            if(!inArray) haveRecField = 1;
         }

         parse->token[i++] = parse->ruleText[parse->index++];
      }

      /**********************************************************/
      /* See if there was a '(' in the token.  If so, then have */
      /* a function.  Otherwise, it is a variable or data field */
      /**********************************************************/
      if(parse->ruleText[parse->index] == '(')
      {  parse->index++;  /* Skip start parens */
         parse->tokenType = T_FUNCTION;
      }
      else if (!haveRecField)
         parse->tokenType = T_VARIABLE;
      else 
         parse->tokenType = T_RECFLD;
   }
   else if(isdigit(parse->ruleText[parse->index]) || 
           (parse->ruleText[parse->index]=='.' && 
            isdigit(parse->ruleText[parse->index+1])))
   {  /* Have a numeric constant */
      while(isdigit(parse->ruleText[parse->index]) || 
             (parse->ruleText[parse->index]=='.' && !haveDecimal))
      {  if(parse->ruleText[parse->index] == '.' && 
            parse->ruleText[parse->index+1]=='.') 
         {  parse->index--;
            break;
         }
         else if(parse->ruleText[parse->index] == '.') haveDecimal = 1;
         parse->token[i++] = parse->ruleText[parse->index++];
      }

      if(haveDecimal) 
      {  parse->tokenType = T_FLOAT_CONSTANT;
         NUM_SETSTR(&parse->data.num, parse->token, i);
      }
      else
      {  parse->tokenType = T_INTEGER_CONSTANT;
         NUM_SETSTR(&parse->data.num, parse->token, i);
      }
   }
   else
   {  parse->tokenType = T_PUNCTUATION;
      switch(parse->ruleText[parse->index])
      {  case ';':
            parse->token[i++] = parse->ruleText[parse->index++];
            break;
         case '.':
            if(parse->ruleText[parse->index+1] == '.')
            {  parse->token[i++] = '.';
               parse->token[i++] = '.';
               parse->index+=2;
            }
            else
            {  sprintf(parse->token, "%c", parse->ruleText[parse->index]);
               parse->index++;
               if(AddErrMsg(FAIL_ENGINE|LEX_RULE|UNKNWN_CHAR, 
                                       parse->token, 0,parse)==FATAL_ENGINE)
                  return(FATAL_ENGINE);
               else 
                  return(FAIL_ENGINE);
            }
            break;
         case '[':
            parse->token[i++] = parse->ruleText[parse->index++];
            break;
         case ']':
            parse->token[i++] = parse->ruleText[parse->index++];
            break;
         case '(':
            parse->token[i++] = parse->ruleText[parse->index++];
            break;
         case ')':
            parse->token[i++] = parse->ruleText[parse->index++];
            break;
         case ',':
            parse->token[i++] = parse->ruleText[parse->index++];
            break;
         case '}':
            parse->token[i++] = parse->ruleText[parse->index++];
            break;
         case '{':
            if(parse->ruleText[parse->index+1] == '*')
            {  while(!(parse->ruleText[parse->index]=='*' && 
                       parse->ruleText[parse->index+1]=='}')) 
               {  if(parse->ruleText[parse->index++] == '\n')
                     parse->lineNumber++; 
               }

               parse->index += 2;
               return(LexRule(parse));
            }
            else 
               parse->token[i++] = parse->ruleText[parse->index++];
            break;
         case '/':
            if(parse->ruleText[parse->index+1] == '*')
            {  while(!(parse->ruleText[parse->index]=='*' && 
                       parse->ruleText[parse->index+1]=='/')) 
               {  if(parse->ruleText[parse->index++] == '\n')
                     parse->lineNumber++; 
               }

               parse->index += 2;
               return(LexRule(parse));
            }
            else 
            {  parse->tokenType = T_ARITH_OPERATOR;
               parse->token[i++] = parse->ruleText[parse->index++];
            }
            break;
         case '*':
            parse->token[i++] = parse->ruleText[parse->index++];
            if(parse->ruleText[parse->index] == '*')
               parse->token[i++] = parse->ruleText[parse->index++];

            parse->tokenType = T_ARITH_OPERATOR;
            break;
         case '+':
            parse->token[i++] = parse->ruleText[parse->index++];
            parse->tokenType = T_ARITH_OPERATOR;
            break;
         case '-':
            parse->token[i++] = parse->ruleText[parse->index++];
            parse->tokenType = T_ARITH_OPERATOR;
            break;
            break;
         case '<':
            parse->tokenType = T_BOOL_OPERATOR;
            parse->token[i++] = parse->ruleText[parse->index++];

            if(parse->ruleText[parse->index] == '=') 
               parse->token[i++] = parse->ruleText[parse->index++];
            else if(parse->ruleText[parse->index] == '>') 
               parse->token[i++] = parse->ruleText[parse->index++];

            break;
         case '>':
            parse->tokenType = T_BOOL_OPERATOR;
            parse->token[i++] = parse->ruleText[parse->index++];
            if(parse->ruleText[parse->index] == '=') 
               parse->token[i++] = parse->ruleText[parse->index++];
            break;
         case '=':
            parse->tokenType = T_BOOL_OPERATOR;
            parse->token[i++] = parse->ruleText[parse->index++];
            break;
         case ':':
            if(parse->ruleText[parse->index+1] != '=')
            {  parse->token[i++] = parse->ruleText[parse->index];
               parse->index++;
            }
            else
            {  parse->token[i++] = parse->ruleText[parse->index++];
               parse->token[i++] = parse->ruleText[parse->index++];
               parse->tokenType = T_ARITH_OPERATOR;
            }
            break;
         case '"':
            /* Skip start quotes */
            parse->index++;

            while(parse->ruleText[parse->index] != '"') 
            {  if(parse->ruleText[parse->index]== '\n') 
                  parse->lineNumber++; 

               if(parse->ruleText[parse->index]== '\0') 
               {  if(AddErrMsg(FAIL_ENGINE|LEX_RULE|UNMTCH_DQUOTE, 
                                           NULL, 0,parse)==FATAL_ENGINE)
                     return(FATAL_ENGINE);
                  else 
                     return(FAIL_ENGINE);
               }

               parse->token[i++] = parse->ruleText[parse->index++];
            }
 
            /* Skip end quotes */
            parse->index++;
            parse->tokenType = T_STRING_CONSTANT;
            break;
         case '\'':
            if(parse->ruleText[parse->index+2] != '\'')
            {  parse->index++;
               if(AddErrMsg(FAIL_ENGINE|LEX_RULE|UNMTCH_SQUOTE, NULL, 0,parse)==FATAL_ENGINE)
                  return(FATAL_ENGINE);
               else 
                  return(FAIL_ENGINE);
            }

            /* Skip start and end quotes */
            parse->token[i++] = parse->ruleText[parse->index+1];
            parse->index += 3;
            parse->tokenType = T_CHAR_CONSTANT;
            break;
         default:
            sprintf(parse->token, "%c", parse->ruleText[parse->index]);
            parse->index++;
            if(AddErrMsg(FAIL_ENGINE|LEX_RULE|UNKNWN_CHAR, 
                                    parse->token, 0,parse)==FATAL_ENGINE)
               return(FATAL_ENGINE);
            else 
               return(FAIL_ENGINE);
      }
   }

   parse->token[i] = '\0';

   /* Check for keywords */
   if((parse->tokenType == T_VARIABLE) ||
      parse->tokenType == T_FUNCTION)
   {  for(i=0; parse->token[i]!='\0'; i++) 
         temp[i] = toupper(parse->token[i]);
      temp[i] = '\0';

      for(j=0; j<numKeywords; j++)
      {  if(!strcmp(temp, aKeyWords[j]))
         {  if(parse->tokenType == T_FUNCTION)
                  parse->index--; /* Bring back the '(' */

            if(!strcmp(temp, "AND") || 
               !strcmp(temp, "OR"))
               parse->tokenType = T_BOOL_JOIN;
            else if(!strcmp(temp, "NOT"))
               parse->tokenType = T_BOOL_NEGATE;
            else 
               parse->tokenType = T_KEYWORD;

            strcpy(parse->token, temp);
            break;
         }
      }
   }

   /* Check for spaces between a function and the first '(' */
   if(parse->tokenType == T_VARIABLE)
   {  i = parse->index;
      while(isspace(parse->ruleText[i])) i++;

      if(parse->ruleText[i] == '(')
      {  parse->tokenType = T_FUNCTION; 
         parse->index = i+1;
      }
   }
      
   /************************************************************/
   /* This function will determine if this is a valid variable */
   /* or constant, or a new identifier, if one is allowed.     */
   /************************************************************/
   if(parse->inExpr==1 || parse->inExpr==2)
   {  if(parse->tokenType == T_RECFLD)
      {  strcpy(temp, parse->token);
         if((status=CheckRecordField(parse->token, parse)))
         {  AddErrMsg(FAIL_ENGINE|PARSE_EXPR|status, parse->token, 0, parse);
            parse->data.dataFld = NULL;
         }

         strcpy(parse->token, temp);
      }
      else if(parse->tokenType == T_VARIABLE)
      {  strcpy(temp, parse->token);
         if((status=CheckVariable(parse->token, parse)))
         {  if(status==FATAL_CALC_RULE)
            {  parse->data.var = NULL;
               AddErrMsg(FATAL_ENGINE|PARSE_EXPR|status, parse->token, 0, parse);
               return(FATAL_ENGINE);
            }
            else if(status==WARN_CALC_RULE)
               AddErrMsg(WARN_ENGINE|PARSE_EXPR|status, parse->token, 0, parse);
            else
               AddErrMsg(FAIL_ENGINE|PARSE_EXPR|status, parse->token, 0, parse);
            
            strcpy(parse->token, temp);
         }
      }
   }
   
   return(OK);
}


/*--------------------------------------------------------------------------*/
/* PutBackToken()                                                           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long PutBackToken(tParse *parse)
{
   parse->index = parse->lastIndex;
   return(OK);
}
