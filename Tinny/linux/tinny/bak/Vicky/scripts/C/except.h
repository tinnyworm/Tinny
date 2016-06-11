#ifndef EXCEPT_DEF
#define EXCEPT_DEF
/*
** $Id: except.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: except.h,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 1.4  1995/06/29  19:41:52  wle
 * Added catchall and rethrow routines.
 *
 * Revision 1.2  1995/06/21  22:42:36  wle
 * Added ISAM_EXCEPTION for CTREE layer exceptions.
 *
 * Revision 1.1  1995/06/15  22:34:07  wle
 * Initial revision
 *
 * Revision 1.2  1995/01/27  03:12:26  wle
 * Added DEVICE_REOPEN exception.
 *
 * Revision 1.1  1994/07/29  22:55:50  wle
 * Initial revision
 *
*/

/*=====================================================================*/
/*                      except.h                                       */
/*                                                                     *
** Description: This module contains a series of macros which mimic
** as much as possible the C++ syntax for exception handling. To this
** end four macros have been defined to perform the exception handling
** These are:
**              a. try  - Has the same meaning as C++ "try" statement.
**           This statment should be placed before a block of code which
**           may generate exceptions that the programmer wants to handle
**           immediately. Functions may have more than one "try" block of
**           code. "try" blocks may also be nested. Note that each "block"
**           must have a corresponding "endcatches" statement.
**              b. catch(EXCEPTION) - Has the same meaning as the C++ "catch"
**           statement. "catch" statements are optional, but if included
**           must immediately follow the the block of code to which the try
**           statement applies. If a "catch" does not follow another "catch"
**           "try" then the compiler may complain with the message
**           'syntax error at or near word "else"'
**              c. endcatches - This statement has no counterpart in C++.
**           However it must be included immediately after the try block of
**           of code, or if there are catch statements, immediately after
**           the last catch statement, following the try block. Failure to
**           place an endcatches statement where required will cause undefined
**           program results. 
**           
**           ***** THERE MUST BE ONE "endcatches" FOR EACH "try" ********
**
**              d. throw(EXCEPTION) - Use this macro to throw an exception.
**           An exception may be thrown at any point in the code. Exceptions
**           will automatically go back up the call stack to find a catch
**           statement to handle the exception. If there is no "catch" for
**           the exception, then the program will terminate abnormally.
**
**  Examples:
**           try
**           { some_code;
**           }
**           catch(EXCEPTION1) somecode;
**           catch(EXCEPTION2) somecode;
**           endcatches;
**
**  Notes:
**	1. If the programmers has more than _MAXTRIES try blocks active
**   then the programm will throw a MAXTRY exception. 
**      2. Exceptions which are thrown, but which are not caught will cause
**   the program to exit abnormally.
**      3.If a "catch" macro is placed somewhere other than immediately 
**   following a "try" or another "catch" block, the compiler may complain
**   as follows.
**          syntax error at or near word "else"
**==========================================================================*/
/* system include files */
#include <setjmp.h> 

/* local include files */
#include "hnctype.h"

/***********************************************************/
/* Exception definitions follow                            */
/***********************************************************/

/* Exceptions for socket operations */
#define SOCKET_EXCEPTION 1
#define DEVICE_EXCEPTION 2
#define HASH_EXCEPTION 3
#define DEVICE_REOPEN 4
#define CONNECTION_EXCEPTION 5
#define ISAM_EXCEPTION 6
#define PRODUCT_EXCEPTION 7
#define CUSTOMER_EXCEPTION 8

#define SCORER_NOT_AVAIL_EXCEPTION 10

#define MALLOC_EXCEPTION 998
#define FATAL_EXCEPTION 999

#define CONFIGURATION_EXCEPTION 400
#define BATCH_EXCEPTION 401

#define DB_EXCEPTION 500
#define DB_DUPLICATE 501

#define ENGINE_EXCEPTION 600

#define _MAXTRIES 100    /* Maximum number of nested try loops before MAXTRY */
                         /* exception */
#define _CAUGHT -1
#define MAXTRY 50


typedef struct sExcData
{
   jmp_buf		jumpbuf;
   int			retries;
} tExcData;


/* Prototypes for local functions */
tExcData *TRY(void);
void _tryfree(void);
UINT32 _catch(INT32);
UINT32 _catchall(void);
void _endcatches(void);
void _rethrow(void);
void _throw(INT32);

extern UINT32 _giExcData;

extern INT32 _Exception;
extern jmp_buf _gJumpBuf;

extern tExcData *_gaExcData[_MAXTRIES+1];

#define throw(Except) \
   _throw(Except)

#define rethrow() \
	_rethrow()

#define try \
   if (!(_Exception = setjmp(TRY()->jumpbuf)))

#define catch(Except) \
   else if (_catch(Except))

#define catchall() \
	else if (_catchall())
#define endcatches \
   _endcatches()

/*
#define throw(value)
#define try if (1)
#define endcatches
#define catch(value) else if (0) 
*/

#endif
