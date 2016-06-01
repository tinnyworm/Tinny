/*
** $Id: except.c,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: except.c,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 1.2  1995/06/29  19:41:52  wle
 * Added catchall and rethrow routines.
 *
 * Revision 1.1  1995/06/15  22:34:07  wle
 * Initial revision
 *
 * Revision 1.7  1995/01/27  19:35:51  wle
 * Added ifdef debugs for debug statements.
 *
 * Revision 1.6  1995/01/25  02:49:52  wle
 * Removed debug.
 *
 * Revision 1.5  1994/09/08  23:10:00  gak
 * Changes from Germany: Added process name to exception underflow error
 * message.
 *
 * Revision 1.4  1994/08/16  16:26:28  wle
 * Added code changes to improve error message.
 *
 * Revision 1.3  1994/08/09  16:40:52  wle
 * Removed rcsid
 *
 * Revision 1.2  1994/08/01  22:18:10  mpb
 * *** empty log message ***
 *
 * Revision 1.1  1994/07/29  22:37:25  wle
 * Initial revision
 *
*/
/*#define DEBUG */
/*=====================================================================*/
/*                      except.c                                       */
/*                                                                     *
** Description: This module contains a series of routines which mimic  
** as much as possible the C++ syntax for exception handling. To this 
** end four macros have been defined to perform the exception handling
** These are:
** 		a. try  - Has the same meaning as C++ "try" statement. 
**           This statment should be placed before a block of code which
**           may generate exceptions that the programmer wants to handle
**           immediately. Functions may have more than one "try" block of
**           code. "try" blocks may also be nested. Note that each "block"
**           must have a corresponding "endcatches" statement.
**              b. catch(EXCEPTION) - Has the same meaning as the C++ "catch"
**           statement. "catch" statements are optional, but if included 
**           must immediately follow the the block of code to which the try
**           statement applies.
**		c. endcatches - This statement has no counterpart in C++. 
**           However it must be included immediately after the try block of
**           of code if, or if there are catch statements, immediately after 
**           the last catch statement, following the try block. Failure to 
**           place an endcatches statement where required will cause undefined
**           program results.
**              d. throw(EXCEPTION) - Use this macro to throw an exception.
**           An exception may be thrown at any point in the code. Exceptions
** 	     will automatically go back up the call stack to find a catch 
**           statement to handle the exception. If there is no "catch" for 
**           the exception, then the program will terminate abnormally.
**
**  Examples:
**           try
**           { some_code;
**           } 
**           catch(EXCEPTION1) somecode;
**           catch(EXCEPTION2) somcecode;
**           endcatches;
**
**==========================================================================*/ 
/* System include files */
#include <stdio.h>
#include <stdlib.h>

/* HNC include files */
#include "except.h"

UINT32    _giExcData = 0;
tExcData *_gaExcData[_MAXTRIES+1];
INT32    _Exception = 0;
INT32		_SavedException = 0;

/*---------------------------------------------------------------
* _try
*
* Allocates a buffer to store the long jump address
* Returns the result from the long jump
* Initializes some variables for this try/catch set
*
* Returns: pointer to jmp_buf
*---------------------------------------------------------------*/
tExcData *TRY(void)
{
   if (_giExcData > _MAXTRIES)
      throw(MAXTRY);
   _giExcData++;
   _gaExcData[_giExcData] = (tExcData *)malloc(sizeof(tExcData));
   return(_gaExcData[_giExcData]);
}
/*--------------------------------------------------------------
* _tryfree()
*
* Deallocates memory assigned to manage try block
*
*--------------------------------------------------------------*/
void _tryfree(void)
{
#ifdef DEBUGosc
	extern char gProcessName[];
#endif
    if(_giExcData)
    {
       free((char *)_gaExcData[_giExcData]);
       _giExcData--;
    }
#ifdef DEBUG0sc
    else
      (void)printf("%s Exception Underflow\n",gProcessName);
#endif
}
/*---------------------------------------------------------------
* _catch
*
* Checks if Except is the thrown Exception. If it is, or if 
* Rethrow is requested returns 1. Else returns 0.
*
* Return value: 0 => do not execute catch block
*               1 => execute catch block 
*---------------------------------------------------------------*/
UINT32 _catch(INT32 Except)
{
   UINT32 caught;	
   if (Except == _Exception)
   {
     caught = 1;
	  _SavedException = _Exception;
     _Exception = _CAUGHT;      /* flag exception caught to prevent 2nd free */
     _tryfree();		/* free up try block memory in case another */
                                /* exception occurs in catch block */
   }
   else 
     caught = 0;
   return(caught);
}
/***********************************************************************/
/* _catchall()
**		Catches all remaining exceptions
************************************************************************/
UINT32 _catchall(void)
{
   UINT32 caught;	
	  
   caught = 1;
	_SavedException = _Exception;
	_Exception = _CAUGHT;      /* flag exception caught to prevent 2nd free */
   _tryfree();		/* free up try block memory in case another */
                                /* exception occurs in catch block */
   return(caught);
}
/***********************************************************************/
/* _rethrow
************************************************************************/
void _rethrow(void)
{
	_throw(_SavedException);
}
/*---------------------------------------------------------------
* _endcatches()
*
* Follows the catch blocks after a try block. Serves to clean up
* the try data, and rethrows the exception if it has not been
* caught.
*
*---------------------------------------------------------------*/
void _endcatches(void)
{
    if (_Exception >= 0)         /* if try block data not freed */
    {
       _tryfree();		 /* free it */
       if (_Exception > 0)       /* rethrow exception if not yet caught */
          throw(_Exception);
    }
    _Exception = 0;		/* flag _Exception handled */
	 _SavedException = 0;
}

/*--------------------------------------------------------------------
*
* _throw()
*
* Copies exception information for possible rethrowing   
* Frees up the long jump buffer for this throw
* Throws control back to previous try block
*
*---------------------------------------------------------------------*/
void _throw(INT32 Except)
{
   if (Except)
   {
       _Exception = Except;
       if (_giExcData)        /* if a valid try block throw exception */
          longjmp(_gaExcData[_giExcData]->jumpbuf,(int)Except);
       else                  /* no try block */
       {                     /* exit with error */
          fprintf(stderr,"Uncaught exception, %d\n", Except);
       }
    }
}
