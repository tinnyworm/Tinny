/*
** $Id: rulpars.c,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: rulpars.c,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.5  1997/02/05  04:16:30  can
 * Changed malloc calls to MyTmpMalloc.
 *
 * Revision 2.4  1997/01/25  07:16:41  can
 * Fixed bug when parsing quoted strings.
 *
 * Revision 2.3  1997/01/17  18:56:09  can
 * Changed hncRulePase to hncRuleParse (duh!)
 *
 * Revision 2.2  1997/01/17  17:22:36  can
 * Updates for latest release
 *
**
*/

/* System includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* HNC Rule Engine includes */
#include "mymall.h"
#include "typechk.h"

/* HNC common includes */
#include "rulpars.h"
#include "except.h"
#include "config.h"
#include "hncrtyp.h"
#include "dyntabl.h"
#include "encrypt.h"

char* GetRcrd(char *buf, int size, FILE *fp, int decrypt)
{
   if(!fgets(buf, size, fp))
      return(NULL);

   if(decrypt)
   {  size = strlen(buf);
      fUuDecode(buf, &size);
     
      if(buf[0]=='\0') strcpy(buf, "\n");
      else buf[size] = '\0';
   }

   return(buf);
}


int GetWord(char *buf, int *index, char *token, int *tokenType)
{
   int     i = *index, j=0;
   int     haveString = FALSE;
   int     haveDigit = FALSE;
   int     havePunct = FALSE;
   int     haveE = FALSE, eIndex=0;
   int     possibleFloat = FALSE, possibleHex = FALSE;
   int     foundDelimiter = FALSE;
   int     inQuote = FALSE;
   char    floatNum[256];


   floatNum[0] = '\0';

   /* skip leading space or new line */
   while(isspace(buf[i]) || buf[i]=='\n') i++;

   if(buf[i] == '"') 
   {  inQuote=TRUE; 
      haveString=TRUE; 
      i++; j=0;
      while(buf[i] != '"')
      {  token[j] = buf[i]; 
         j++; i++;
      }
      token[j] = '\0';

      *tokenType = WORD_STRING;
      *index = i+1;
      return(0);
   }

   while(foundDelimiter == FALSE)
   {
      /* alpha & '_' is considered string */
      if(isalpha(buf[i]) || buf[i]=='_')
      {
         if((buf[i] == 'e' || buf[i] == 'E') && 
                !haveE && !haveString && (possibleFloat || haveDigit))
         {  
            if(buf[i+1] == '-' || buf[i+1] == '+' || isdigit(buf[i+1]))
            {  haveE = 1;
               haveDigit = FALSE; 
               haveString = FALSE; 
               possibleFloat = TRUE; 
               eIndex = i;
            }
            else haveString = TRUE;
         }
         else 
            haveString = TRUE;

         token[j] = buf[i];
         i++; j++;
      }
      else if(isdigit(buf[i]) || ((buf[i]=='-' || buf[i]=='+') && j==0) || 
              ((buf[i]=='+' || buf[i]=='-') && (haveE && i==eIndex+1)))
      {
         if((buf[i] == '+' || buf[i] == '-') && !isdigit(buf[i+1]))
         {  havePunct = TRUE;
            foundDelimiter = TRUE;
         }
         else if(j==0 && buf[i] == '0' && 
           (buf[i+1] == 'x' || buf[i+1] == 'X'))
         {  token[j] = buf[i];
            token[j+1] = 'x';
            i+=2; j+=2;
            haveDigit = TRUE;
            possibleHex = TRUE;
         } 
         else
         {  token[j] = buf[i];
            i++; j++;
            haveDigit = TRUE;
         }
      }
      else if(ispunct(buf[i]))
      {
         /* if "." found & token is all digits it may be a float or     */
         /* delimiter; save the current token and set posssible float   */
         /* and proceed reading until next delimiter; Then if the the   */
         /* next token is also all digits then it is a float;           */
         /* also reset j and save offset i since it will be needed if   */
         /* input is not a float                                        */
         if(buf[i] == '.' && haveDigit && !haveString && !possibleFloat)
         {
            if(j == 0)
               foundDelimiter = TRUE;
            else
            {
               possibleFloat = TRUE;
               haveDigit = FALSE;
               haveString = FALSE;
               token[j] = buf[i];
               i++; j++;
            }
         }
         else if(inQuote && buf[i]!='"')
         {
            token[j] = buf[i];
            i++; j++;
         }
         else 
         {
            if(inQuote) i++;
            foundDelimiter = TRUE;
            havePunct = TRUE;
         }
      }

      /* anything else is a delimiter */
      else if(inQuote)
      {
         token[j] = buf[i];
         i++; j++;
      }
      else 
         foundDelimiter = TRUE;
   }

   /* if first char is a punctuation  return punct char */
   if(possibleFloat && haveDigit && !haveString && !possibleHex)
   {
      token[j] = '\0';
      *tokenType = WORD_FLOAT;
   }
   else if(possibleHex && haveDigit && token[j-1] != 'x')
   {
      *tokenType = WORD_HEX;
      token[j] = '\0';
   }
   else if(possibleHex && haveDigit)
   {
      *tokenType = WORD_STRING;
      token[j] = '\0';
   }
   else if(haveString)
   {
      *tokenType = WORD_STRING;
      token[j] = '\0';
   }
   else if(haveDigit)
   {
      *tokenType = WORD_DIGIT;
      token[j] = '\0';
   }
   /* first char is punctuation */
   else if(havePunct && j==0)
   {
      token[j++] = buf[i++];
      token[j] = '\0';
      *tokenType = WORD_DELIM;
   }
   else
   {
      token[j] = '\0';
      *tokenType = WORD_END;
   }
   *index = i;

   return(0);
}


char CheckPermission(char* permission)
{
   if(permission[0] == 'R')
      return(READ_ONLY);
   else if(permission[0] == 'W')
      return(READ_WRITE);

   return('\0');
}


int CheckFldType(char* type, void **data, int *dataSize)
{
   int      returnType, tDataSize;
   void     *tData; 

   /* type should be two char, unless is string with size, or 101 */
   if(!strcmp(type, "101"))
      return(RECORD_TYPE);
   else if(strlen(type) > 2 && type[0] != '3')
      return(-1);

   returnType = -1;
   tDataSize = 0;

   switch(type[0])
   {
      case '1':
         switch(type[1])
         {
            case '0':
               tDataSize = 4;
               returnType = HNC_UINT32;
               break;
            case '1':
               tDataSize = sizeof(float);
               returnType = HNC_FLOAT;
               break;
            case '2':
               tDataSize = sizeof(double);
               returnType = HNC_DOUBLE;
               break;
            case '3':
               tDataSize = 1;
               returnType = HNC_CHAR;
               break;
            case '\0':
               tDataSize = 255;
               returnType = HNC_INTSTR;
               break;
            default:
               returnType = -1;
               tDataSize = 0;
               break;
         }
         break;
      case '2':
         tDataSize = 255;
         returnType = HNC_FLOATSTR;
         break;
      case '3':
         if(type[1] == '(')
            tDataSize = atoi(type+2);
         else
            tDataSize = 255;
         returnType = HNC_STRING;
         break;
      case '4':
         tDataSize = 8;
         returnType = HNC_DATESTR;
         break;
      case '5':
         tDataSize = 8;
         returnType = HNC_TIMESTR;
         break;
      case '6':
         tDataSize = 16;
         returnType = HNC_DTSTR;
         break;
      case '7':
         tDataSize = 2;
         returnType = HNC_INT16;
         break;
      case '8':
         tDataSize = 2;
         returnType = HNC_UINT16;
         break;
      case '9':
         tDataSize = 4;
         returnType = HNC_INT32;
         break;
      default:
         returnType = -1;
         tDataSize = 0;
         break;
   }

   /* Allow the user to pass in the datasize, mostly for strings */
   if(dataSize)
   {  if(*dataSize != 0)
         tDataSize = *dataSize;
   }

   if(data != NULL && tDataSize > 0)
   {
      if((tData = (void*)MyMalloc(tDataSize)) == NULL)
      {  fprintf(stderr, "Out of memory for data, %d", tDataSize);
         throw(MALLOC_EXCEPTION);
      }
      memset((char*)tData, 0, tDataSize);
      *data = tData;

   }
   if(dataSize != NULL)
      *dataSize = tDataSize;
   return(returnType);
}


int GetToken(char *buf, int *index, char *token)
{
   int i = *index, j=0;

   while(isspace(buf[i]) || buf[i]=='\n') i++;

   if(isalpha(buf[i]) || buf[i]=='_')
   {  while(isalnum(buf[i]) || buf[i]=='_')
      {  token[j] = toupper(buf[i]);
         i++; j++;
      }
      token[j] = '\0';
   }
   else
   {  token[0] = buf[i];
      token[1] = '\0';
      i++;
   }

   *index = i;

   return(0);
}


long ReadTemplateMaps(tTmpltMap **tmpltMap, FILE *fp)
{
   char       buf[1024], token[256], errMsg[256];
   long       oldSpot;
   int        i, tokType;
   short      optionCt=0;
   tTmpltMap  *map=NULL;
   tTmpltNode *node=NULL;

   /* Make sure the file pointer is OK */
   if(fp==NULL)
   {  SetConfigErrorMessage("Template map cannot be read, bad file pointer");
      throw(CONFIGURATION_EXCEPTION);
   }
 
   /* Loop through the file, reading MAP statements */
   while(1)
   {  i=0;

      /* Check for a MAP */
      oldSpot = ftell(fp);
      if(!GetRcrd(buf, 1024, fp, 0))
      {  fseek(fp, oldSpot, 0);
         break;
      }
 
      if(memcmp(buf, "INPUT MAP=", 10) && 
         memcmp(buf, "INPUT MAP =", 11))
      {  fseek(fp, oldSpot, 0);
         break;
      }

      /* Allocate a map structure */
      if(map)
      {  if((map->next = (tTmpltMap*)MyMalloc(sizeof(tTmpltMap)))==NULL)
            throw(MALLOC_EXCEPTION);
         map = map->next;
      }
      else
      {  if((map = (tTmpltMap*)MyMalloc(sizeof(tTmpltMap)))==NULL)
            throw(MALLOC_EXCEPTION);
         *tmpltMap = map;
      }
      memset((char*)map, 0, sizeof(tTmpltMap));
          
      /* Skip the "INPUT MAP=" and parse the statement */
      i += 11;
      while(1)
      {  /* Get the node, or node list */ 
         GetWord(buf, &i, token, &tokType);

         if(!strcmp(token, "("))
         {  optionCt=0;
            while(1)
            {  /* Get node name */
               GetWord(buf, &i, token, &tokType);
               if(tokType!=WORD_STRING)
               {  sprintf(errMsg, "Bad template name '%s' in MAP", token);
                  SetConfigErrorMessage(errMsg);
                  throw(CONFIGURATION_EXCEPTION);
               }
               else
               {  if(map->nodeList)
                  {  if((node->next = (tTmpltNode*)MyMalloc(sizeof(tTmpltNode)))==NULL)
                        throw(MALLOC_EXCEPTION);
                     node = node->next;
                  }
                  else
                  {  if((node = (tTmpltNode*)MyMalloc(sizeof(tTmpltNode)))==NULL)
                        throw(MALLOC_EXCEPTION);
                     map->nodeList = node;
                  }
                  memset((char*)node, 0, sizeof(tTmpltNode));
                  node->optionCt = ++optionCt;
                  strcpy(node->name, token);
               }

               /* Get the operation (only | allowed), or the ending ) */
               GetWord(buf, &i, token, &tokType);
               if(!strcmp(token, ")")) 
               {  if(map->nodeList == NULL)
                  {  sprintf(errMsg, "Bad MAP syntax ()");
                     SetConfigErrorMessage(errMsg);
                     throw(CONFIGURATION_EXCEPTION);
                  }
                  break;
               }
               else if(strcmp(token, "|"))
               {  sprintf(errMsg, "Bad MAP format, missing '|' for '%s'", buf);
                  SetConfigErrorMessage(errMsg);
                  throw(CONFIGURATION_EXCEPTION);
               }
            }
         }
         else if(tokType!=WORD_STRING)
         {  sprintf(errMsg, "Bad template name '%s' in MAP", token);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }
         else
         {  if(map->nodeList)
            {  if((node->next = (tTmpltNode*)MyMalloc(sizeof(tTmpltNode)))==NULL)
                  throw(MALLOC_EXCEPTION);
               node = node->next;
            }
            else
            {  if((node = (tTmpltNode*)MyMalloc(sizeof(tTmpltNode)))==NULL)
                  throw(MALLOC_EXCEPTION);
               map->nodeList = node;
            }
            memset((char*)node, 0, sizeof(tTmpltNode));
            strcpy(node->name, token);
         }

         /* Get the operation, (only '+' allowd) */
         GetWord(buf, &i, token, &tokType);
         if(token[0] == '\0')
            break;
         else if(strcmp(token, "+"))
         {  sprintf(errMsg, "Bad MAP format, missing '+' after '%s' for '%s'", 
                                                             node->name, buf);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }
      }
   }

   return(0);
}


long CombineTemplates(tTmpltMap *tmpltMap, tDataTemplate *tmpltList)
{
   tDataTemplate *tmplt;
   tTmpltMap     *map;
   tTmpltNode    *node;
   long          offset, maxSize, totSize, nonFixed=0;
   char          *data, errMsg[256];
   short         lastOption=0;

   /**********************************/
   /* Loop through all template maps */
   /**********************************/
   for(map=tmpltMap; map; map=map->next)
   {   
      /* For each node, find the template and delete that template's data area */
      for(node=map->nodeList; node; node=node->next)
      {  for(tmplt=tmpltList; tmplt; tmplt=tmplt->next)
         {  if(!strcmp(tmplt->templateName, node->name))
            {  node->tmplt = tmplt;
               break;
            }
         }

         if(!tmplt)
         {  sprintf(errMsg, "Template '%s' in MAP not found", node->name);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }
         else
         {  MyFree((void*)tmplt->data);
            tmplt->data = NULL;
         }
      }
  
      /* For each map, find the total size, if any.  If any template is not fixed */
      /* then the map is considered non fixed and must be resolved at run time    */
      maxSize=0; totSize=0; nonFixed=0;
      for(node=map->nodeList; node; node=node->next)
      {  
         /* If tmplt has a dataSize, it is fixed.  Add it to the total size */
         if(node->tmplt->dataSize)
         {  /* If the node is an option, compare the size to the max */
            /* size of the current group of OR'd templates           */
            if(node->optionCt && (node->optionCt > lastOption))
            {  if(node->tmplt->dataSize > maxSize)
                  maxSize = node->tmplt->dataSize;
               lastOption = node->optionCt;
            }
            else if(node->optionCt)
            {  /* Must have a maxSize from a previous option block */
               totSize += maxSize;
               lastOption = node->optionCt;
               maxSize = node->tmplt->dataSize;
            }
            else
            {  /* If had a maxSize from previous OR'd group, add it to the total */
               /* and reset the maxSize                                          */
               if(maxSize) 
               {  totSize += maxSize;
                  maxSize = 0;
                  lastOption = 0;
               }

               totSize += node->tmplt->dataSize;
            }
         }
         /* Template is not fixed */
         else
         {  nonFixed = 1;
            break;
         }
      }

      /* Go on to next template map if this one is non fixed */
      if(nonFixed) continue;
     
      /* Add maxSize if last was group of OR'd templates */
      if(maxSize) totSize += maxSize;

      /* Allocate the data area for the whole map */
      if((data = (char*)MyMalloc(totSize))==NULL)
         throw(MALLOC_EXCEPTION);

      /* Set the data pointers for each of the templates in the map */
      offset=0; maxSize=0; lastOption=0;
      for(node=map->nodeList; node; node=node->next)
      {  if(node->optionCt && (node->optionCt > lastOption))
         {  node->tmplt->data = data+offset;
            if(node->tmplt->dataSize > maxSize) 
               maxSize = node->tmplt->dataSize;
            lastOption = node->optionCt;
         }
         else if(node->optionCt)
         {  /* Must have a maxSize from a previous option block */
            offset += maxSize;
            lastOption = node->optionCt;
            maxSize = node->tmplt->dataSize;
            node->tmplt->data = data+offset;
         }
         else 
         {  if(maxSize)
            {  offset += maxSize;
               maxSize = 0;
               lastOption = 0;
            }

            node->tmplt->data = data+offset;
            offset += node->tmplt->dataSize;
         }
      }

      /* Set the dataSize of the lead template to totSize */
      map->nodeList->tmplt->dataSize = totSize;
   }
   
   return(0); 
}


long ParseRuleName(char *buf, int *index, tRuleData *ruleData, char *from)
{
   char     token[50], ruleName[50], errMsg[256];
   int      lasti, i, haveVar=0;
   tRuleArg *ruleArg=NULL;

   i=1;
   while(isalnum(buf[i]) || buf[i]=='_')
   {  ruleName[i-1] = buf[i]; 
      i++;
   }
   ruleName[i-1] = '\0';
   ruleData->ruleName = (char*)MyMalloc(strlen(ruleName)+1);
   strcpy(ruleData->ruleName, ruleName);

   /* Lose any spaces */
   while(isspace(buf[i]))
   {  if(buf[i] == '\n')
      {  *index = i;
         return(0);
      }
      i++;
   }

   if(buf[i]=='(')
   {  i++;

      while(1)
      {  if(!haveVar)
         {  /* Parse out the parameter list */
            if(ruleArg==NULL)
            {  ruleArg = (tRuleArg*)MyMalloc(sizeof(tRuleArg));
               ruleData->argList = ruleArg; 
            }
            else
            {  ruleArg->next = (tRuleArg*)MyMalloc(sizeof(tRuleArg));
               ruleArg = ruleArg->next;
            }
            memset((char*)ruleArg, 0, sizeof(tRuleArg));
            ruleData->numArgs++;
         }
          
         GetToken(buf, &i, token);

         if(!strcmp(token, "VAR")) 
         {  if(haveVar) 
            {  if(from) sprintf(errMsg, "ParseRuleName from '%s', error in rule "
                                        "parameter list.  Too many VARS", from);
               else sprintf(errMsg, "ParseRuleName  error in rule parameter list. " 
                                    "Too many VARS");
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }

            haveVar = 1;
            ruleArg->reference = 1;
            continue;
         }
         else
         {  /* Get variable name */ 
            ruleArg->argName = (char*)MyMalloc(strlen(token)+1);
            strcpy(ruleArg->argName, token);

            /* Get ":" */ 
            GetToken(buf, &i, token);
            if(strcmp(token, ":"))
            {  if(from) sprintf(errMsg, "ParseRuleName from '%s', error in rule "
                                        "parameter list.  Missing ':'", from);
               else sprintf(errMsg, "ParseRuleName, error in rule "
                                    "parameter list.  Missing ':'");
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }

            /* Get type */ 
            GetToken(buf, &i, token);
            if(!strcmp(token, "NUMERIC"))
               ruleArg->type = NUMERIC_TYPE;
            else if(!strcmp(token, "STRING"))
               ruleArg->type = HNC_STRING;
            else 
            {  if(from) sprintf(errMsg, "ParseRuleName from '%s', error in rule "
                                        "parameter list.  Bad type", from);
               else sprintf(errMsg, "ParseRuleName, error in rule "
                                    "parameter list.  Bad type");
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }

            haveVar = 0;
         }

         /* Check for ending parens, or comma */
         GetToken(buf, &i, token);
         if(!strcmp(token, ")")) 
         {  /* Look for return type */
            lasti = i;
            GetToken(buf, &i, token);
            if(!strcmp(token, ":"))
            {  /* Get return type */
               GetToken(buf, &i, token);
               if(!strcmp(token, "NUMERIC"))
                  ruleData->returnType = HNC_FLOAT;
               else if(!strcmp(token, "STRING"))
                  ruleData->returnType = HNC_STRING;
               else 
               {  if(from) sprintf(errMsg, "ParseRuleName from '%s', error in rule "
                                           "parameter list.  Bad return type", from);
                  else sprintf(errMsg, "ParseRuleName, error in rule "
                                       "parameter list.  Bad return type");
                  SetConfigErrorMessage(errMsg);
                  throw(CONFIGURATION_EXCEPTION);
               }
            }
            else i = lasti;
            
            break;
         }
         else if(strcmp(token, ","))
         {  if(from) sprintf(errMsg, "ParseRuleName from '%s', error in rule "
                                     "parameter list at '%s'", from, token);
            else sprintf(errMsg, "ParseRuleName, error in rule "
                                 "parameter list at '%s'", token);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }
      }      
   }

   /* Just for just a return type */
   else if(buf[i] == ':')
   {  i++;
 
      /* Get type */
      GetToken(buf, &i, token);
      if(!strcmp(token, "NUMERIC"))
         ruleData->returnType = NUMERIC_TYPE;
      else if(!strcmp(token, "STRING"))
         ruleData->returnType = HNC_STRING;
      else
      {  if(from) sprintf(errMsg, "ParseRuleName from '%s'.  "
                                  "Bad return type '%s'", from, token);
         else sprintf(errMsg, "ParseRuleName.  Bad return type '%s'", token);
         SetConfigErrorMessage(errMsg);
         throw(CONFIGURATION_EXCEPTION);
      }
   }
 
   *index = i;
 
   return(0);
}


long GetValue(char *data, char *type, char *value)
{
   int  i, j;

   /* Skip to end of type string */
   data += strlen(type);

   /* Skip any spaces leading up to '=' */
   i=0;
   while(isspace(data[i]) && data[i] != '\0') i++;
   if(data[i] == '\0')
      return(-1);

   /* Get '=' sign */
   if(data[i] == '=') i++;
   else 
      return(-2);

   /* Skip remaining spaces until hit value */
   while(isspace(data[i]) && data[i] != '\0') i++;
   if(data[i] == '\0')
      return(-3);
   
   /* Copy data into value until a space, newline, or null is hit */
   j=0;
   while(!isspace(data[i]) && data[i] != '\0' && data[i] != '\n')
   {  value[j] = data[i];
      i++; j++;
   }
   value[j] = '\0';

   return(0);
}


#define T_NAME          0x0001 
#define T_PERM          0x0002 
#define T_TYPE          0x0004 
#define T_OFFSET        0x0008 
#define T_LENGTH        0x0010 
#define T_FLD_ID        0x0020 
#define T_LENGTH_IND    0x0040 
#define T_MAX_ELEMENTS  0x0080 
#define T_SUB_TEMPLATE  0x0100 
#define T_REQUIRED      0x0200 
#define T_SKIP          0x0400 

#define NO_NAME_START   1
#define NO_NAME_END     2
long ReadNewTemplateFormat
(tDataField *newField, FILE *fp, int decrypt, long *nonFixed, char *from, tDataTemplate *tmplt)
{
   char          *ptr, line[1024], buf[100], errMsg[256];
   long          found=0, k, spot, nameEnd=1;

   /* Read the first non-blank, non-comment line, which should be a NAME */
   while(1)
   {  spot = ftell(fp);
      if((GetRcrd(line, 1024, fp, decrypt))!=NULL)
      {  /* Eat up spaces and look for a blank line */ 
         k=0;
         while(isspace(line[k]) && line[k]!='\n') k++;
         if(line[k] == '\n') continue;
         ptr = line + k;

         if(!memcmp(ptr, "NAME", 4) && strchr(ptr, '='))
         {  found |= T_NAME; 
    
            if(GetValue(ptr, "NAME", buf))
            {  if(from) sprintf(errMsg, "GetTemplateFields from '%s', error reading NAME "
                                        "at '%s' for template '%s'", 
                                        from, ptr, tmplt->templateName);
               else sprintf(errMsg, "GetTemplateFields, error reading NAME "
                                    "at '%s' for template '%s'", 
                                    ptr, tmplt->templateName);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }

            if((newField->fieldName = (char*)MyMalloc(strlen(buf)+1))==NULL)
               throw(MALLOC_EXCEPTION);
            strcpy(newField->fieldName, buf);
            break;
         }
         else
         {  fseek(fp, spot, 0);
            return(NO_NAME_START);
         }
      }
      else
         return(NO_NAME_START);
   }

   /******************************************************************/
   /* Loop through the field attributes until the next NAME is found */
   /* or something other than a valid attribute is found             */
   /******************************************************************/
   while(1)
   {  /* Read the line */
      spot = ftell(fp);
      if((GetRcrd(line, 1024, fp, decrypt))!=NULL)
      {  /* Eat up spaces and look for a blank line */ 
         k=0;
         while(isspace(line[k]) && line[k]!='\n') k++;
         if(line[k] == '\n') continue;
         ptr = line + k;

         if(!memcmp(ptr, "NAME", 4) && strchr(ptr, '='))
         {  /* Have next field name */
            fseek(fp, spot, 0);
            break;
         }
         else  if(!memcmp(ptr, "TYPE", 4) && strchr(ptr, '='))
         {  found |= T_TYPE;
           
            if(GetValue(ptr, "TYPE", buf))
            {  if(from) sprintf(errMsg, "GetTemplateFields from '%s', error reading TYPE "
                                        "at '%s' for field '%s' in template '%s'", 
                                        from, ptr, newField->fieldName, tmplt->templateName);
               else sprintf(errMsg, "GetTemplateFields, error reading TYPE "
                                    "at '%s' for field '%s' in template '%s'", 
                                    ptr, newField->fieldName, tmplt->templateName);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }

            if((newField->type = CheckFldType(buf, NULL, NULL))==-1)
            {  if(from) sprintf(errMsg, "GetTemplateFields from '%s', error bad field "
                                        "type for '%s' in template '%s'", 
                                        from, newField->fieldName, tmplt->templateName);
               else sprintf(errMsg, "GetTemplateFields, error bad field "
                                    "type for '%s' in template '%s'", 
                                    newField->fieldName, tmplt->templateName);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }
         }
         else  if(!memcmp(ptr, "PERM", 4) && strchr(ptr, '='))
         {  found |= T_PERM;

            if(GetValue(ptr, "PERM", buf))
            {  if(from) sprintf(errMsg, "GetTemplateFields from '%s', error reading PERM "
                                        "at '%s' for field '%s' in template '%s'", 
                                        from, ptr, newField->fieldName, tmplt->templateName);
               else sprintf(errMsg, "GetTemplateFields, error reading PERM "
                                    "at '%s' for field '%s' in template '%s'", 
                                    ptr, newField->fieldName, tmplt->templateName);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }

            if((newField->permission = CheckPermission(buf))=='\0')
            {  if(from) sprintf(errMsg, "GetTemplateFields from '%s', error bad field "
                                        "permission for '%s' in template '%s'", 
                                        from, newField->fieldName, tmplt->templateName);
               else sprintf(errMsg, "GetTemplateFields, error bad field "
                                    "permission for '%s' in template '%s'", 
                                    newField->fieldName, tmplt->templateName);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }
         }
         else  if(!memcmp(ptr, "OFFSET", 6) && strchr(ptr, '='))
         {  found |= T_OFFSET;

            if(GetValue(ptr, "OFFSET", buf))
            {  if(from) sprintf(errMsg, "GetTemplateFields from '%s', error reading OFFSET "
                                        "at '%s' for field '%s' in template '%s'", 
                                        from, ptr, newField->fieldName, tmplt->templateName);
               else sprintf(errMsg, "GetTemplateFields, error reading OFFSET "
                                    "at '%s' for field '%s' in template '%s'", 
                                    ptr, newField->fieldName, tmplt->templateName);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }

            if((newField->offset = atol(buf)) < 0)
            {  if(from) sprintf(errMsg, "GetTemplateFields from '%s', error bad field "
                                        "offset for '%s' in template '%s'", 
                                        from, newField->fieldName, tmplt->templateName);
               else sprintf(errMsg, "GetTemplateFields, error bad field "
                                    "offset for '%s' in template '%s'", 
                                    newField->fieldName, tmplt->templateName);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }

            if(*nonFixed == 1)
            {
#ifdef DEVELOP
               fprintf(stderr, "Template '%s' is not fixed.  Offset of '%ld' "
                               "for field '%s' is ignored\n", 
                               tmplt->templateName, newField->offset, newField->fieldName); 
#endif
            }
         }
         else  if(!memcmp(ptr, "LENGTH_IND", 10) && strchr(ptr, '='))
         {  found |= T_LENGTH_IND;

            if(GetValue(ptr, "LENGTH_IND", buf))
            {  if(from) sprintf(errMsg, "GetTemplateFields from '%s', error reading "
                                        "LENGTH_IND at '%s' for field '%s' in template '%s'", 
                                        from, ptr, newField->fieldName, tmplt->templateName);
               else sprintf(errMsg, "GetTemplateFields, error reading LENGTH_IND "
                                    "at '%s' for field '%s' in template '%s'", 
                                    ptr, newField->fieldName, tmplt->templateName);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }

            if((newField->nonFixed.lengthInd = atol(buf)) < 0)
            {  if(from) sprintf(errMsg, "GetTemplateFields from '%s', error bad length "
                                        "indicator value for '%s' in template '%s'", 
                                        from, newField->fieldName, tmplt->templateName);
               else sprintf(errMsg, "GetTemplateFields, error bad length indicator "
                                       "value for '%s' in template '%s'", 
                                       newField->fieldName, tmplt->templateName);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }

            if(found&T_OFFSET && *nonFixed)
            {  
#ifdef DEVELOP
               fprintf(stderr, "Template '%s' is not fixed.  Offset of '%ld' "
                               "for field '%s' is ignored.\n", 
                               tmplt->templateName, newField->offset, newField->fieldName); 
#endif
            }

            if(!(*nonFixed)) *nonFixed = 2;
            if(found&T_LENGTH)
            {
#ifdef DEVELOP
               fprintf(stderr, "Field '%s' in template '%s' has a length indicator. "
                               "Given LENGTH of '%ld' is being ignored\n",
                               newField->fieldName, tmplt->templateName, newField->length); 
#endif
            }
         }
         else  if(!memcmp(ptr, "LENGTH", 6) && strchr(ptr, '='))
         {  found |= T_LENGTH;

            if(GetValue(ptr, "LENGTH", buf))
            {  if(from) sprintf(errMsg, "GetTemplateFields from '%s', error reading LENGTH "
                                        "at '%s' for field '%s' in template '%s'", 
                                        from, ptr, newField->fieldName, tmplt->templateName);
               else sprintf(errMsg, "GetTemplateFields, error reading LENGTH "
                                    "at '%s' for field '%s' in template '%s'", 
                                    ptr, newField->fieldName, tmplt->templateName);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }

            if((newField->length = atol(buf)) < 0)
            {  if(from) sprintf(errMsg, "GetTemplateFields from '%s', error bad field "
                                        "length for '%s' in template '%s'", 
                                        from, newField->fieldName, tmplt->templateName);
               else sprintf(errMsg, "GetTemplateFields, error bad field "
                                       "length for '%s' in template '%s'", 
                                       newField->fieldName, tmplt->templateName);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }

            if(found&T_LENGTH_IND)
            {
#ifdef DEVELOP
               fprintf(stderr, "Field '%s' in template '%s' has a length indicator. "
                               "Given LENGTH of '%ld' is being ignored\n",
                               newField->fieldName, tmplt->templateName, newField->length); 
#endif
            }
            else
               tmplt->dataSize += newField->length;
         }
         else  if(!memcmp(ptr, "FLD_ID", 6) && strchr(ptr, '='))
         {  found |= T_FLD_ID;

            if(GetValue(ptr, "FLD_ID", buf))
            {  if(from) sprintf(errMsg, "GetTemplateFields from '%s', error reading FLD_ID "
                                        "at '%s' for field '%s' in template '%s'", 
                                        from, ptr, newField->fieldName, tmplt->templateName);
               else sprintf(errMsg, "GetTemplateFields, error reading FLD_ID "
                                    "at '%s' for field '%s' in template '%s'", 
                                    ptr, newField->fieldName, tmplt->templateName);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }

            if((newField->nonFixed.fldID = (char*)MyMalloc(strlen(buf)+1))==NULL)
               throw(MALLOC_EXCEPTION);
            strcpy(newField->nonFixed.fldID, buf);
            newField->nonFixed.IDLen = strlen(buf);

            *nonFixed = 1;
            if(found&T_OFFSET)
            {  
#ifdef DEVELOP
               fprintf(stderr, "Template '%s' is not fixed.  Offset of '%ld' "
                               "for field '%s' is ignored.\n", 
                               tmplt->templateName, newField->offset, newField->fieldName); 
#endif
            }
         }
         else  if(!memcmp(ptr, "MAX_ELEMENTS", 12) && strchr(ptr, '='))
         {  found |= T_MAX_ELEMENTS;

            if(GetValue(ptr, "MAX_ELEMENTS", buf))
            {  if(from) sprintf(errMsg, "GetTemplateFields from '%s', error reading "
                                        "MAX_ELEMENTS at '%s' for field '%s' "
                                        "in template '%s'", 
                                        from, ptr, newField->fieldName, tmplt->templateName);
               else sprintf(errMsg, "GetTemplateFields, error reading MAX_ELEMENTS "
                                    "at '%s' for field '%s' in template '%s'", 
                                    ptr, newField->fieldName, tmplt->templateName);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }

            if((newField->nonFixed.maxElements = atol(buf)) < 0)
            {  if(from) sprintf(errMsg, "GetTemplateFields from '%s', error bad max "
                                        "elements value for '%s' in template '%s'", 
                                        from, newField->fieldName, tmplt->templateName);
               else sprintf(errMsg, "GetTemplateFields, error bad max elements "
                                       "value for '%s' in template '%s'", 
                                       newField->fieldName, tmplt->templateName);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }
         }
         else  if(!memcmp(ptr, "SUB_TEMPLATE", 12) && strchr(ptr, '='))
         {  found |= T_SUB_TEMPLATE;

            if(GetValue(ptr, "SUB_TEMPLATE", buf))
            {  if(from) sprintf(errMsg, "GetTemplateFields from '%s', error "
                                        "reading SUB_TEMPLATE at '%s' for field "
                                        "'%s' in template '%s'", 
                                        from, ptr, newField->fieldName, tmplt->templateName);
               else sprintf(errMsg, "GetTemplateFields, error reading SUB_TEMPLATE "
                                    "at '%s' for field '%s' in template '%s'", 
                                    ptr, newField->fieldName, tmplt->templateName);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }
          
            if(!strcmp(buf, tmplt->templateName))
            {  if(from) sprintf(errMsg, "GetTemplateFields from '%s', SUB_TEMPLATE "
                                        "for field '%s' in template '%s' cannot be '%s'",
                                        from, newField->fieldName, tmplt->templateName, buf);
               else sprintf(errMsg, "GetTemplateFields, SUB_TEMPLATE "
                                    "for field '%s' in template '%s' cannot be '%s'",
                                    newField->fieldName, tmplt->templateName, buf);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }

            if((newField->subTmpltName = (char*)MyMalloc(strlen(buf)+1))==NULL)
               throw(MALLOC_EXCEPTION);
            strcpy(newField->subTmpltName, buf);
         }
         else  if(!memcmp(ptr, "REQUIRED", 8) && strchr(ptr, '='))
         {  found |= T_REQUIRED;

            if(GetValue(ptr, "REQUIRED", buf))
            {  if(from) sprintf(errMsg, "GetTemplateFields from '%s', error reading REQUIRED "
                                        "at '%s' for field '%s' in template '%s'", 
                                        from, ptr, newField->fieldName, tmplt->templateName);
               else sprintf(errMsg, "GetTemplateFields, error reading REQUIRED "
                                    "at '%s' for field '%s' in template '%s'", 
                                    ptr, newField->fieldName, tmplt->templateName);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }
       
            if(found&T_FLD_ID)
            {  if(buf[0] == 'Y' || buf[0] == 'y')
                  newField->nonFixed.required = 1;
            }
            else
            {
#ifdef DEVELOP
               fprintf(stderr, "Field '%s' in template '%s' does not have a FLD_ID, "
                               "so it is always required.  REQUIRED value is ignored\n",
                               newField->fieldName, tmplt->templateName);
#endif
            }
         }
         else  if(!memcmp(ptr, "SKIP", 4) && strchr(ptr, '='))
         {  found |= T_SKIP;

            if(GetValue(ptr, "SKIP", buf))
            {  if(from) sprintf(errMsg, "GetTemplateFields from '%s', error reading SKIP "
                                        "at '%s' for field '%s' in template '%s'", 
                                        from, ptr, newField->fieldName, tmplt->templateName);
               else sprintf(errMsg, "GetTemplateFields, error reading SKIP "
                                    "at '%s' for field '%s' in template '%s'", 
                                    ptr, newField->fieldName, tmplt->templateName);
               SetConfigErrorMessage(errMsg);
               throw(CONFIGURATION_EXCEPTION);
            }
       
            if(found&T_FLD_ID)
            {  if(buf[0] == 'Y' || buf[0] == 'y')
                  newField->nonFixed.skip = 1;
            }
            else
            {
#ifdef DEVELOP
               fprintf(stderr, "Field '%s' in template '%s' does not have a FLD_ID, "
                               "so it is always used.  SKIP value is ignored\n",
                               newField->fieldName, tmplt->templateName);
#endif
            }
         }
         else
         {  fseek(fp, spot, 0);
            nameEnd=0;
            break;
         }
      }
      else
      {  nameEnd=0;
         break;
      }
   }

   /************************************************************************/
   /* Fields must have type and permission, and offset and length if fixed */
   /* Also, if it is non-fixed, all fields must have field IDs             */
   /************************************************************************/
   if(!(found&T_TYPE))
   {  if(from) sprintf(errMsg, "GetTemplateFields from '%s', field '%s' in template "
                               "'%s' was not given a TYPE value. ",
                               from, newField->fieldName, tmplt->templateName);
      else sprintf(errMsg, "GetTemplateFields, field '%s' in template "
                           "'%s' was not given a TYPE value. ",
                           newField->fieldName, tmplt->templateName);
      SetConfigErrorMessage(errMsg);
      throw(CONFIGURATION_EXCEPTION);
   }
   else if(!(found&T_PERM))
   {  if(from) sprintf(errMsg, "GetTemplateFields from '%s', field '%s' in template "
                               "'%s' was not given a PERM value. ",
                               from, newField->fieldName, tmplt->templateName);
      else sprintf(errMsg, "GetTemplateFields, field '%s' in template "
                           "'%s' was not given a PERM value. ",
                           newField->fieldName, tmplt->templateName);
      SetConfigErrorMessage(errMsg);
      throw(CONFIGURATION_EXCEPTION);
   }
   else if(!(found&T_OFFSET) && !nonFixed)
   {  if(from) sprintf(errMsg, "GetTemplateFields from '%s', field '%s' in template "
                               "'%s' was not given a OFFSET value. ",
                               from, newField->fieldName, tmplt->templateName);
      else sprintf(errMsg, "GetTemplateFields, field '%s' in template "
                           "'%s' was not given a OFFSET value. ",
                           newField->fieldName, tmplt->templateName);
      SetConfigErrorMessage(errMsg);
      throw(CONFIGURATION_EXCEPTION);
   }
   else if(!(found&T_LENGTH) && !nonFixed)
   {  if(from) sprintf(errMsg, "GetTemplateFields from '%s', field '%s' in template "
                               "'%s' was not given a LENGTH value.",
                               from, newField->fieldName, tmplt->templateName);
      else sprintf(errMsg, "GetTemplateFields, field '%s' in template "
                           "'%s' was not given a LENGTH value.",
                           newField->fieldName, tmplt->templateName);
      SetConfigErrorMessage(errMsg);
      throw(CONFIGURATION_EXCEPTION);
   }
   else if(!(found&T_FLD_ID) && *nonFixed==1)
   {  if(from) sprintf(errMsg, "GetTemplateFields from '%s', field '%s' in template "
                               "'%s' was not given a FLD_ID value, but it is in the "
                               "non-fixed portion of the record.",
                               from, newField->fieldName, tmplt->templateName);
      else sprintf(errMsg, "GetTemplateFields, field '%s' in template "
                           "'%s' was not given a FLD_ID value, but it is in the "
                           "non-fixed portion of the record.",
                           newField->fieldName, tmplt->templateName);
      SetConfigErrorMessage(errMsg);
      throw(CONFIGURATION_EXCEPTION);
   }
 
   if(!nameEnd) return(NO_NAME_END);
   else return(0);
}


long ReadOldTemplateFormat
(tDataField *newField, FILE *fp, int decrypt, 
   long *dataLength, char *from, tDataTemplate *tmplt)
{
   char          line[1024], buf[100], errMsg[256];
   long          value;

   /* Read field name */
   GetRcrd(line, 1024, fp, decrypt);
   sscanf(line, "%s\n", buf);

   newField->fieldName = (char*)MyMalloc(strlen(buf)+1);
   strcpy(newField->fieldName, buf);

   /* Read field type */
   GetRcrd(line, 1024, fp, decrypt);
   sscanf(line, "%s\n", buf);

   if((newField->type = CheckFldType(buf, NULL, NULL))==-1)
   {  if(from) sprintf(errMsg, "GetTemplateFields from '%s', error bad field "
                               "type for '%s' in template '%s'", 
                               from, newField->fieldName, tmplt->templateName);
      else sprintf(errMsg, "GetTemplateFields, error bad field "
                           "type for '%s' in template '%s'", 
                           newField->fieldName, tmplt->templateName);
      SetConfigErrorMessage(errMsg);
      throw(CONFIGURATION_EXCEPTION);
   }

   /* Read field permission */
   GetRcrd(line, 1024, fp, decrypt);
   sscanf(line, "%s\n", buf);
   if((newField->permission = CheckPermission(buf))=='\0')
   {  if(from) sprintf(errMsg, "GetTemplateFields from '%s', error bad field "
                               "permission for '%s' in template '%s'", 
                               from, newField->fieldName, tmplt->templateName);
      else sprintf(errMsg, "GetTemplateFields, error bad field "
                           "permission for '%s' in template '%s'", 
                           newField->fieldName, tmplt->templateName);
      SetConfigErrorMessage(errMsg);
      throw(CONFIGURATION_EXCEPTION);
   }

   /* Read field offset */
   GetRcrd(line, 1024, fp, decrypt);
   if(sscanf(line, "%ld\n", &value)!=1)
   {  if(from) sprintf(errMsg, "GetTemplateFields from '%s', error bad field "
                               "offset for '%s' in template '%s'", 
                               from, newField->fieldName, tmplt->templateName);
      else sprintf(errMsg, "GetTemplateFields, error bad field "
                           "offset for '%s' in template '%s'", 
                           newField->fieldName, tmplt->templateName);
      SetConfigErrorMessage(errMsg);
      throw(CONFIGURATION_EXCEPTION);
   }
   newField->offset = value;

   /****************************************/
   /* Check if the data defined for the    */ 
   /* template is contiguous               */
   /****************************************/
   if(*dataLength != newField->offset)
   {  if(from) sprintf(errMsg, "GetTemplateFields from '%s', error field "
                               "offset for '%s' in template '%s' doesn't match length", 
                               from, newField->fieldName, tmplt->templateName);
      else sprintf(errMsg, "GetTemplateFields, error bad field "
                           "offset for '%s' in template '%s' doesn't match length", 
                           newField->fieldName, tmplt->templateName);
      SetConfigErrorMessage(errMsg);
      throw(CONFIGURATION_EXCEPTION);
   }

   /* Read field length */
   GetRcrd(line, 1024, fp, decrypt);
   if(sscanf(line, "%ld\n", &value)!=1)
   {  if(from) sprintf(errMsg, "GetTemplateFields from '%s', error bad field "
                               "length for '%s' in template '%s'", 
                               from, newField->fieldName, tmplt->templateName);
      else sprintf(errMsg, "GetTemplateFields, error bad field "
                              "length for '%s' in template '%s'", 
                              newField->fieldName, tmplt->templateName);
      SetConfigErrorMessage(errMsg);
      throw(CONFIGURATION_EXCEPTION);
   }

   newField->length = value;
   *dataLength += newField->length;

   return(0);
}


long GetTemplateFields
(FILE *fp, tDataTemplate *tmplt, int nbrFields, char *from, int decrypt)
{
   char          line[1024], errMsg[256];
   long          nonFixed=0, spot, loopCount;
   long          dataLength=0, oldFormat=1, i, status;
   tDataField    *newField=NULL, *fld;

   /**********************************************************/
   /* Read the very first field name.  If it is a NAME= <X>  */
   /* then parse the templates in the new format, else parse */
   /* according to the old format                            */
   /**********************************************************/
   while(1)
   {  spot = ftell(fp);
      
      GetRcrd(line, 1024, fp, decrypt);
    
      /* Skip line if it has a # as the first character */
      if(line[0] == '#') continue;
 
      /* Skip line if it is blank */
      i=0;
      while(isspace(line[i]) && line[i] != '\n') i++;
      if(line[i] == '\n') continue;

      /* Check line for NAME field */
      if(strlen(line) > 6)
      {  if(!memcmp(line+i, "NAME", 4))
         {  if(strchr(line+i+4, '='))
               oldFormat = 0;
         }
      }

      fseek(fp, spot, 0);
      break;
   }

   /*************************************************************/
   /* Loop through the file reading the fields for the template */
   /*************************************************************/
   loopCount=0;
   while(loopCount < nbrFields)
   {  
      loopCount++;
      if(newField)
      {  if((newField->next = (tDataField*)MyMalloc(sizeof(tDataField)))==NULL)
            throw(MALLOC_EXCEPTION);
         newField = newField->next;
      }
      else
      {  if((newField = (tDataField*)MyMalloc(sizeof(tDataField)))==NULL)
            throw(MALLOC_EXCEPTION);
         tmplt->fieldList = newField;
      }
      memset((char*)newField, 0, sizeof(tDataField));
      
      if(oldFormat)
         ReadOldTemplateFormat(newField, fp, decrypt, &dataLength, from, tmplt);
      else
      {  status = ReadNewTemplateFormat(newField, fp, decrypt, &nonFixed, from, tmplt);
         if(status==NO_NAME_END && loopCount!=nbrFields)
         {  if(from) sprintf(errMsg, "GetTemplateFields from '%s', either field "
                                     "'%s' is the last field in template '%s' "
                                     "(which means only %ld of %d fields have "
                                     "been defined), or the field following '%s' "
                                     "doesn't have the correct format", 
                                     from, newField->fieldName, tmplt->templateName, 
                                     loopCount, nbrFields, newField->fieldName);
            else sprintf(errMsg, "GetTemplateFields, either field '%s' is the "
                                 "last field in template '%s' (which means only "
                                 "%ld of %d fields have been defined), or the field "
                                 "following '%s' doesn't have the correct format", 
                                 newField->fieldName, tmplt->templateName, loopCount, 
                                 nbrFields, newField->fieldName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }
         else if (status==0 && loopCount==nbrFields)
         {  if(from) sprintf(errMsg, "GetTemplateFields from '%s', there appear "
                                     "to be more fields defined for template '%s' "
                                     "than %d.  Field '%s' is number %ld",
                                     from, tmplt->templateName, nbrFields, 
                                     newField->fieldName, loopCount);
            else sprintf(errMsg, "GetTemplateFields, there appear to be "
                                 "more fields defined for template '%s' "
                                 "than %d.  Field '%s' is number %ld",
                                 tmplt->templateName, nbrFields, 
                                 newField->fieldName, loopCount);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }
      }
   }

   /************************************************************************/
   /* Sanity check if the fields if they are in the new format.  Also, get */
   /* the dataLength, if it is a fixed template.                           */
   /************************************************************************/
   if(!oldFormat)
   {  for(fld = tmplt->fieldList; fld; fld=fld->next)  
      {  if(fld->length == 0 && fld->nonFixed.lengthInd == 0 && 
            fld->next!=NULL && !IsString(fld->type))
         {  if(from) sprintf(errMsg, "GetTemplateFields from '%s', Error, field '%s' "
                                     "in template '%s' has a length of 0 and a "
                                     "length indicator of 0.", 
                                     from, fld->fieldName, tmplt->templateName);
            else sprintf(errMsg, "GetTemplateFields, Error, field '%s' "
                                 "in template '%s' has a length of 0 and a "
                                 "length indicator of 0.", 
                                 fld->fieldName, tmplt->templateName);
            SetConfigErrorMessage(errMsg);
            throw(CONFIGURATION_EXCEPTION);
         }
      }

      if(!nonFixed)
         dataLength = tmplt->dataSize;
      else
      {  tmplt->dataSize = 0;
         tmplt->data = NULL;
      }
   }
   

   /*********************************************/
   /* Allocate the buffer for the template data */
   /*********************************************/
   if(dataLength > 0)
   {  if((tmplt->data = (char *)MyMalloc(dataLength)) == NULL)
         throw(MALLOC_EXCEPTION);
      memset((char*)tmplt->data, 0, dataLength);
      tmplt->dataSize = dataLength;
   }

   return(0);
}


long ReadRuleBase
(FILE *fp, tRuleBaseData *ruleBaseData, int nbrRule, char *from, int decrypt)
{
   char      buf[MAX_RULE_LEN], rule[MAX_RULE_LEN];
   char      *pRule=NULL;
   int       i,loopCount,getID=0;
   tRuleData *newRule=NULL, *tmpRule;
   char      errMsg[256], ch;

   loopCount = 0;
   while(loopCount < nbrRule )
   {  
      if(!GetRcrd(buf, 4000, fp, decrypt))
         break;    

      if(buf[0]=='&') 
      {  if(newRule!=NULL) 
         {  newRule->ruleText = (char*)MyTmpMalloc(strlen(rule)+1);
            strcpy(newRule->ruleText, rule);

#ifdef DEBUG
fprintf(stderr,"rule = %s\n",rule);
fprintf(stderr,"len  = %d\n",((int)pRule - (int)&rule));
ch = getchar();
#endif
            if(strlen(newRule->ruleName) < 8)
            {  if(ruleBaseData->ruleList==NULL) 
                  ruleBaseData->ruleList = newRule;
               else
               {  tmpRule = ruleBaseData->ruleList;
                  while(tmpRule->next) tmpRule = tmpRule->next;
                  tmpRule->next = newRule;
               }
            }
            else if(!memcmp(newRule->ruleName, "ITER_INIT", 9))
            {  if(ruleBaseData->supportRules.iterInitRuleList==NULL) 
                  ruleBaseData->supportRules.iterInitRuleList = newRule;
               else
               {  tmpRule = ruleBaseData->supportRules.iterInitRuleList;
                  while(tmpRule->next) tmpRule = tmpRule->next;
                  tmpRule->next = newRule;
               }
            }
            else if(!memcmp(newRule->ruleName, "ITER_TERM", 9))
            {  if(ruleBaseData->supportRules.iterTermRuleList==NULL) 
                  ruleBaseData->supportRules.iterTermRuleList = newRule;
               else
               {  tmpRule = ruleBaseData->supportRules.iterTermRuleList;
                  while(tmpRule->next) tmpRule = tmpRule->next;
                  tmpRule->next = newRule;
               }
            }
            else if(!memcmp(newRule->ruleName, "SYS_INIT", 8))
            {  if(ruleBaseData->supportRules.sysInitRuleList==NULL) 
                  ruleBaseData->supportRules.sysInitRuleList = newRule;
               else
               {  tmpRule = ruleBaseData->supportRules.sysInitRuleList;
                  while(tmpRule->next) tmpRule = tmpRule->next;
                  tmpRule->next = newRule;
               }
            }
            else if(!memcmp(newRule->ruleName, "SYS_TERM", 8))
            {  if(ruleBaseData->supportRules.sysTermRuleList==NULL) 
                  ruleBaseData->supportRules.sysTermRuleList = newRule;
               else
               {  tmpRule = ruleBaseData->supportRules.sysTermRuleList;
                  while(tmpRule->next) tmpRule = tmpRule->next;
                  tmpRule->next = newRule;
               }
            }
            else
            {  if(ruleBaseData->ruleList==NULL) 
                  ruleBaseData->ruleList = newRule;
               else
               {  tmpRule = ruleBaseData->ruleList;
                  while(tmpRule->next) tmpRule = tmpRule->next;
                  tmpRule->next = newRule;
               }
            }
         }

         newRule = (tRuleData*)MyMalloc(sizeof(tRuleData));
         newRule->next = NULL;
         memset(newRule, 0, sizeof(tRuleData));

         ParseRuleName(buf, &i, newRule, from);
  
         pRule = rule;
         strcpy(pRule, buf+i);
         pRule += strlen(buf+i);

         getID = 1;
      }
      else if(newRule==NULL) continue;

      /* After rule name, looking for rule ID parameter */
      else if(getID)
      {  if(memcmp(buf, "ID:", 3))
         {  strcpy(pRule, buf);
            pRule += strlen(buf);
         }
         else
            sscanf(buf, "ID: %d", &newRule->ruleID);

         getID = 0;
      }

      /* designated end of a rule */
      else if(!memcmp(buf,"ENDRULE",7))
      {  buf[3] = 0; /* Get rid of "RULE" */
         loopCount++;
         strcpy(pRule, buf);
         pRule += strlen(buf);
         getID = 0;
      }
      else
      {  i=0; while(isspace(buf[i])) i++;
         if(!memcmp(buf+i,"ENDRULE",7))
         {   buf[i+3] = '\0'; /* Get rid of "RULE" */
             loopCount++;
         }

         strcpy(pRule, buf);
         pRule += strlen(buf);
         getID = 0;
      }

      if(((int)pRule - (int)&rule) + 1>  MAX_RULE_LEN ) 
      {  if(from) sprintf(errMsg, "ReadRuleBase from '%s'.  Error, rule '%s' too "
                                   "long or missing 'ENDRULE'", from, newRule->ruleName);
         else sprintf(errMsg, "ReadRuleBase, error rule '%s' too "
                              "long or missing 'ENDRULE'", newRule->ruleName);
         SetConfigErrorMessage(errMsg);
         throw(CONFIGURATION_EXCEPTION);
      }
   }

   if(newRule) 
   {  newRule->ruleText = (char*)MyTmpMalloc(strlen(rule)+1);
      strcpy(newRule->ruleText, rule);

#ifdef DEBUG
fprintf(stderr,"rule = %s\n",rule);
fprintf(stderr,"len  = %d\n",((int)pRule - (int)&rule));
#endif
      if(strlen(newRule->ruleName) < 8)
      {  if(ruleBaseData->ruleList==NULL) 
            ruleBaseData->ruleList = newRule;
         else
         {  tmpRule = ruleBaseData->ruleList;
            while(tmpRule->next) tmpRule = tmpRule->next;
            tmpRule->next = newRule;
         }
      }
      else if(!memcmp(newRule->ruleName, "ITER_INI", 8))
      {  if(ruleBaseData->supportRules.iterInitRuleList==NULL) 
            ruleBaseData->supportRules.iterInitRuleList = newRule;
         else
         {  tmpRule = ruleBaseData->supportRules.iterInitRuleList;
            while(tmpRule->next) tmpRule = tmpRule->next;
            tmpRule->next = newRule;
         }
      }
      else if(!memcmp(newRule->ruleName, "ITER_TER", 8))
      {  if(ruleBaseData->supportRules.iterTermRuleList==NULL) 
            ruleBaseData->supportRules.iterTermRuleList = newRule;
         else
         {  tmpRule = ruleBaseData->supportRules.iterTermRuleList;
            while(tmpRule->next) tmpRule = tmpRule->next;
            tmpRule->next = newRule;
         }
      }
      else if(!memcmp(newRule->ruleName, "SYS_INIT", 8))
      {  if(ruleBaseData->supportRules.sysInitRuleList==NULL) 
            ruleBaseData->supportRules.sysInitRuleList = newRule;
         else
         {  tmpRule = ruleBaseData->supportRules.sysInitRuleList;
            while(tmpRule->next) tmpRule = tmpRule->next;
            tmpRule->next = newRule;
         }
      }
      else if(!memcmp(newRule->ruleName, "SYS_TERM", 8))
      {  if(ruleBaseData->supportRules.sysTermRuleList==NULL) 
            ruleBaseData->supportRules.sysTermRuleList = newRule;
         else
         {  tmpRule = ruleBaseData->supportRules.sysTermRuleList;
            while(tmpRule->next) tmpRule = tmpRule->next;
            tmpRule->next = newRule;
         }
      }
      else
      {  if(ruleBaseData->ruleList==NULL) 
            ruleBaseData->ruleList = newRule;
         else
         {  tmpRule = ruleBaseData->ruleList;
            while(tmpRule->next) tmpRule = tmpRule->next;
            tmpRule->next = newRule;
         }
      }
   }

   return(0);
}
