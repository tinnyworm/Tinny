#ifndef TOKSTRC_H
#define TOKSTRC_H

/*
** $Id: toktype.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: toktype.h,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.1  1997/01/16  22:44:06  can
 * Lots of changes for 2.1 release
 *
 * Revision 2.0  1996/03/18  18:45:44  gak
 * GUTS 2.0.1 alpha release
 *
 * Revision 1.4  1995/08/25  01:25:48  can
 * Fixed compile warning messages
 *
 * Revision 1.3  1995/07/26  20:53:26  can
 * Modified allowable variable types to NUMERIC and STRING.
 *
 * Revision 1.2  1995/07/21  16:20:39  can
 * Added tokens for variable block definitions
 *
 * Revision 1.1  1995/07/03  21:18:26  wle
 * Initial revision
 *
*/

typedef enum sTokenType {
   T_RECFLD=0,
   T_VARIABLE,
   T_RULE,
   T_FUNCTION,
   T_KEYWORD,
   T_PUNCTUATION,
   T_ARITH_OPERATOR,
   T_BOOL_OPERATOR,
   T_BOOL_JOIN,
   T_BOOL_NEGATE,
   T_STRING_CONSTANT,
   T_INTEGER_CONSTANT,
   T_FLOAT_CONSTANT,
   T_CHAR_CONSTANT,
   T_ARRAYVAR,
   T_RBNAME
} tTokenType;

#endif
