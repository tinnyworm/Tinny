#ifndef HNCCONFIG_DEF
#define HNCCONFIG_DEF

/*
** $Id: config.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: config.h,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 1.1  1996/04/24  18:57:29  can
 * Initial revision
 *
 * Revision 1.6  1996/01/02  22:36:02  can
 * Added function called GetNextItemInConfig to loop through like parameters
 * in a config file.
 *
 * Revision 1.5  1995/11/27  23:43:02  gak
 * Jim added a new config error message -- PARAMETERMISMATCH
 *
 * Revision 1.4  1995/08/09  20:55:44  wle
 * Moved CheckLogin routine here.
 *
 * Revision 1.3  1995/07/05  15:16:59  wle
 * Added new configuration error types.
 *
 * Revision 1.2  1995/06/19  23:29:22  wle
 * Whole bunch of changes
 *
 * Revision 1.1  1995/06/15  22:34:07  wle
 * Initial revision
 *
 * Revision 1.1  1995/06/14  21:34:31  wle
 * Initial revision
 *
*/

/* System include files */
#include <stdio.h>

/* HNC include files */
#include "hnctype.h"

#define MAX_LINE_LENGTH 256
#define MAX_PARAM_LENGTH MAX_LINE_LENGTH


/* Structures used by configuration utility */

typedef struct sConfigExcept
{
	char *Message;
	int  error;
} tConfigExcept;

typedef struct sConfigHandle
{
	char aLastParm[MAX_PARAM_LENGTH];
	long SeekTo;
} tConfigHandle;
		 
/* The Following are configuration errors */
#define cnfgPARAMETERMISSING 1
#define cnfgMISSINGSYSTEMCONFIG 2
#define cnfgMISSINGPROCESSCONFIG 3
#define cnfgILLEGALPARAMETERVALUE 4
#define cnfgILLEGALPARAMETERLENGTH 5
#define cnfgTOOMANYVALUES 7
#define cnfgDBPARAMETERMISSING 8
#define cnfgDBILLEGALPARAMETERVALUE 9
#define cnfgPARAMETERMISMATCH 10
		  
void CreateConfigException(int error,char *ConfigFile,char *parameter);
void SetConfigErrorMessage(char *Message);
char *GetConfigException(void);
BOOL GetParameter(char *conf_file_name,char *param_name, char *value,
																		  INT32 length);
BOOL GetCatParameter(char *ConfigFile,char *Part1,char *Value,int length,char *Part2);

BOOL GetParameterWithHandle(char *conf_file_name,char *param_name, char *value,
	INT32 length,tConfigHandle *handle);

	/* Allocates a handle for iterative calls to GetParameterWithHandle */
	/* Do not forget to free when done with handle by call to free */
tConfigHandle *AllocConfigHandle(void);

	/* Checks that login name matches LOGINID parameter */
char *CheckLogin(char *ConfigFile);
BOOL GetNextConfigItem(char* configFile, char *item, char *value, int length);

#endif
