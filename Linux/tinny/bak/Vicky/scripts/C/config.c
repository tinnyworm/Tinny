/*
** $Id: config.c,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: config.c,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 1.1  1996/04/24  18:57:29  can
 * Initial revision
 *
 * Revision 1.13  1996/01/02  22:36:02  can
 * Added function called GetNextItemInConfig to loop through like parameters
 * in a config file.
 *
 * Revision 1.12  1995/11/27  23:43:02  gak
 * Jim added a new config error message -- PARAMETERMISMATCH
 *
 * Revision 1.11  1995/09/27  16:23:45  gak
 * Changes Walter made at Nabanco
 *
 * Revision 1.10  1995/08/09  20:55:44  wle
 * Moved CheckLogin routine here.
 *
 * Revision 1.9  1995/07/23  22:17:51  wle
 * Modified configuration routine to take quotes around parameters to preserve whitespace.
 *
 * Revision 1.8  1995/07/12  23:30:01  wle
 * Throw exception if config file unavailable.
 *
 * Revision 1.7  1995/07/05  15:16:39  wle
 * Added new configuration error types.
 *
 * Revision 1.6  1995/06/29  18:37:27  wle
 * *** empty log message ***
 *
 * Revision 1.5  1995/06/29  18:35:50  wle
 * Added some asserts to catch bugs earlier
 *
 * Revision 1.4  1995/06/22  21:37:46  wle
 * Corrected code to permit repetitive parameters at end of file.
 *
 * Revision 1.3  1995/06/20  01:04:52  wle
 * Added quotes around error message fields.
 *
 * Revision 1.2  1995/06/19  23:28:28  wle
 * Whole bunch of changes.
 *
 * Revision 1.1  1995/06/15  22:34:07  wle
 * Initial revision
 *
 * Revision 1.1  1995/06/14  21:34:31  wle
 * Initial revision
 *
 * Revision 1.3  1994/08/11  16:07:27  mjk
 * Made GetParameter return FALSE if a parameter is assigned an empty value.
 *
 * Revision 1.2  1994/08/09  16:45:32  wle
 * Removed rcsid.
 *
 * Revision 1.1  1994/07/29  22:37:25  wle
 * Initial revision
 *
*/
/*************************************************************************/
/* uconfig.c
**  Contains the function GetParameter, which grabs a parameter from the
** config file and returns its value in the pointer passed as a parameter.
**
** void * GetParameter(char *param_name, void *pdata)
**
**************************************************************************/

/* System include files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <limits.h>
#include <errno.h>

/* HNC include files */
#include "mymall.h"
#include "hnctype.h"
#include "config.h"
#include "except.h"
#include "safopen.h"
#include "maxmisc.h"

#define WHITE_SPACE " \t\r\n"

char *rstrtok(char *init_str, char *separators, char **handle);

/* Globals */
static tConfigExcept gConfigExcept; 

/* Locally used and defined functions */
BOOL GetParameterWithHandle(char *conf_file_name,char *param_name, char *value,
							INT32 length,tConfigHandle *handle);

/***********************************************************************/
/* GetParameter
************************************************************************/
BOOL GetParameter(char *conf_file_name,char *param_name, char *value, INT32 length)
{
	return(GetParameterWithHandle(conf_file_name,param_name,value,length,NULL));
}
/*************************************************************************/
/* void * GetParameter
**     Obtains the value of a parameter from a parameter definition statement
** in the configuration file. A parameter definition statement is of the form
**     Parameter_Name   =   Parameter_value
** The number of space characters sounding the '=' is not important.
**
** Input: param_name is the name of the parameter
** Return value: a pointer to the value string
**
** Consecutive calls to this function for the same parameter allow the 
** retrieval of multiple definitions for that parameter.
*************************************************************************/
BOOL GetParameterWithHandle(char *conf_file_name,char *param_name, char *value, 
							INT32 length,tConfigHandle *handle)
{
   FILE *ConfFile;
   char ConfLine[MAX_LINE_LENGTH], ParmLine[MAX_LINE_LENGTH +2];
   int  Index;
   BOOL return_value = FALSE;
	char *pTemp;
	int 	eop;

   if ((ConfFile = SafeFopen(conf_file_name)) == NULL)
   {
		char ErrMsg[1025];
		sprintf(ErrMsg,"Unable to open configuration file '%s': %s.",conf_file_name,strerror(errno));
		SetConfigErrorMessage(ErrMsg);
		throw(CONFIGURATION_EXCEPTION);
   }

   /* if calling for the same parameter, start the search from the last */
   /* file access position */
   if (handle && !strcmp(handle->aLastParm, param_name)) 
	{
       if (fseek(ConfFile, handle->SeekTo, 0)) 
		 {
   	   *value = '\0';
	   	return FALSE;
       } 
   }
	else if (handle)
	{
		handle->SeekTo = 0;
		strcpy(handle->aLastParm,param_name);
	}

		/* Search file for parameter */
   while (!feof(ConfFile))
   {
		ConfLine[0] = 0;
      (void) fgets(ConfLine, MAX_LINE_LENGTH, ConfFile);

			/* Remove comment from end of line */
		if (pTemp = strchr(ConfLine,'#'))
			*pTemp = 0;

			/* remove leading and trailing spaces */
		rltrim(ConfLine);


		if (pTemp = strchr(ConfLine,'='))
		{
			*pTemp = 0;	
			rtrim(ConfLine);
			pTemp++;
		}
		else
			pTemp = (char *)NULL;

			/* Does parameter match */
		if (!strcmp(ConfLine,param_name))
		{
				/* if parameter value provided */
			if (pTemp)
			{
					/* remove leading and trailing whitespace */
				rltrim(pTemp);
				eop = strlen(pTemp)-1;
					/* if enclosed in quotes, ignore opening and closing quote */
				if ((*pTemp == '\'' && *(pTemp+eop) == '\'')
				||  (*pTemp == '\"' && *(pTemp+eop) == '\"'))
				{
					*(pTemp+eop) = (char )0;
					pTemp++;
				}
					/* copy parameter to output value */
				strncpy(value,pTemp,length);
				if (strlen(pTemp) > (length-1))
				{
					char ErrMsg[1024];
					sprintf(ErrMsg,"Parameter value '%s' for parameter '%s' in file '%s'	too long. "
						"Expected maximum of %d characters",pTemp,param_name,conf_file_name,length-1);
					SetConfigErrorMessage(ErrMsg);
					throw(CONFIGURATION_EXCEPTION);
				}
			}
			else
				*value = (char )0;

      		/* remember the parameter name and file access position */
			if (handle)
			{
		     	strcpy(handle->aLastParm, param_name);
     			handle->SeekTo = ftell(ConfFile);
			}

			return_value = TRUE;
				/* exit loop */
			break;
		}
	
   }  

   (void) fclose(ConfFile);
   return return_value;
}

/***********************************************************************/
/* char *rstrtok(char *init_str, char *separators, char **handle)
**
** This is a re-entrant version of the string library function strtok().
**  It behaves exactly the same as strtok() except that it requires the
**  third 'handle' parameter in order to hold its place.  This parameter
**  should not be touched by the calling process, but it should be
**  allocated to hold (at least) one char *.
************************************************************************/
char *rstrtok(char *init_str, char *separators, char **handle)
{
   register INT32 iInit = 0, iSep;
   register char *start;
   BOOL found_sep = TRUE;

   if (init_str != NULL)
      start = init_str;
   else if (*handle == NULL)
      return NULL;
   else
      start = *handle;
   /* Search for first character not in separators */
   while (found_sep && *start != '\0')
   {
      found_sep = FALSE;
      for (iSep = 0; !found_sep && separators[iSep] != '\0'; iSep++)
         if (*start == separators[iSep])
            found_sep = TRUE;
      if (found_sep)
         start++;
   }
   if (*start == '\0')
   {
      *handle = NULL;
      return NULL;
   }

   /* Now start points to the first character not in separators. */
   while (!found_sep && start[iInit] != '\0')
   {
      for (iSep = 0; !found_sep && separators[iSep] != '\0'; iSep++)
         if (start[iInit] == separators[iSep])
            found_sep = TRUE;
      iInit++;
   }
   if (found_sep)
   {
      /* iInit now points to the character after the first separator. */
      start[iInit - 1] = '\0';
      *handle = start + iInit;
   }
   else
      *handle = NULL;

   return start;
}
/***********************************************************************/
/* GetConfigException
**   Returns configuration exception message
************************************************************************/
char *GetConfigException()
{
	return(gConfigExcept.Message);
}
/***********************************************************************/
/* CreateConfigException
**		Stores configuration exception information for later retrieval by
** an application
************************************************************************/
void CreateConfigException(int error,char *ConfigFile,char *parameter)
{
	gConfigExcept.error = error;

	switch(error)
	{
			/* the following are for configuration file errors */
		case cnfgPARAMETERMISSING:	
			sprintf(gConfigExcept.Message,
                                        "Required parameter '%s' "
                                        "missing from Configuration File '%s'",
					parameter,ConfigFile);
		break;
		case cnfgMISSINGSYSTEMCONFIG:
			sprintf(gConfigExcept.Message,"Missing system configuration file");
		break;
		case cnfgMISSINGPROCESSCONFIG:
			sprintf(gConfigExcept.Message,"Missing process specific configuration file");
		break;
		case cnfgILLEGALPARAMETERVALUE:
			sprintf(gConfigExcept.Message,"Parameter '%s' in '%s' has illegal value",
				parameter,ConfigFile);
		break;
		case cnfgILLEGALPARAMETERLENGTH:
			sprintf(gConfigExcept.Message,"Parameter '%s' in '%s' has too many characters",
				parameter,ConfigFile);	
		break;
		case cnfgTOOMANYVALUES:
			sprintf(gConfigExcept.Message,"Parameter '%s' in '%s' repeated too many times",
				parameter,ConfigFile);
		break;
			/* The following are for database configuration table errors */
		case cnfgDBPARAMETERMISSING:
			sprintf(gConfigExcept.Message,
                                               "Required parameter '%s' "
                                               "missing from Database configuration table", 
                                               parameter);
		break;
		case cnfgDBILLEGALPARAMETERVALUE:
			sprintf(gConfigExcept.Message, "Parameter '%s' in Database "
                                                       "Configuration table has illegal value",
                                                       ConfigFile);
		break;
		case cnfgPARAMETERMISMATCH:
			sprintf(gConfigExcept.Message, "Parameters '%s' in '%s' are "
                                                       "not compatible",
                                                       parameter,ConfigFile);
		break;
		default:
			sprintf(gConfigExcept.Message,"Error %d detected in '%s'",error,ConfigFile);
		break;
	}
}
/***********************************************************************/
/* SetConfigErrorMessage
**		Creates a configuration error message 
************************************************************************/
void SetConfigErrorMessage(char *Message)
{
   gConfigExcept.Message = (char*)MyMalloc(strlen(Message)+1);
	strcpy(gConfigExcept.Message,Message);
}
/***********************************************************************/
/* GetCatParameter(..)
**  Returns a parameter value consisting of for a parameter which is 
** concatenated from input values
************************************************************************/
BOOL GetCatParameter(char *ConfigFile,char *Part1,char *Value,int length,char *Part2)
{
	char Parameter[MAX_PARAM_LENGTH+1];

	sprintf(Parameter,"%s%s",Part1,Part2);
	return(GetParameter(ConfigFile,Parameter,Value,length));
}
/***********************************************************************/
/* AllocConfigHandle
**   Allocates a configuration handle for getting parameters iteratively
************************************************************************/
tConfigHandle *AllocConfigHandle(void)
{
	return((tConfigHandle *)MyCalloc(1,sizeof(tConfigHandle)));
}

/************************************************************************
 *   GetNextConfigItem
 *      Get the next item with the given type from the config file
 ************************************************************************/
BOOL GetNextConfigItem(char* configFile, char *item, char *value, int length)
{
   static tConfigHandle *ConfigHandle;
   static BOOL Initialized=FALSE;

   /* if not initialized, call alloc to initialize the handle */
   if (!Initialized)
   {
      Initialized = TRUE;
      ConfigHandle = AllocConfigHandle();
   }

   /* if reached the end of service list, free handle */
   if (!GetParameterWithHandle(configFile, item, value, length, ConfigHandle))
   {  MyFree(ConfigHandle);
      Initialized = FALSE;
      return FALSE;
   }
   else
      return TRUE;
}

